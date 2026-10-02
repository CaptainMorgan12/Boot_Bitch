#!/usr/bin/env python3
"""Machine-translation fill step for Boot Bitch i18n (0.2.27 dev cycle, all local).

Fills the unfinished <translation> entries of a Qt Linguist .ts file for a single
target language, honouring a curated overrides table and verifying placeholder
integrity before anything is committed to disk.

What it does
------------
1. Parses the .ts (xml.etree) and selects every <message> whose <translation> is
   empty/unfinished.
2. Skips messages whose <source> is a parse token / format-only string (no
   letters, e.g. a lone "%1", "✓", "•", "--").
3. Produces a translation for each remaining source using the selected backend:
       --backend none    overrides only (no machine translation)
       --backend shell   translate-shell ``trans -b :<target>`` (must be on PATH)
       --backend argos   offline Argos Translate (requires ``argostranslate``
                         and the ``translate-en_<target>`` package installed)
       --backend libre   the LibreTranslate HTTP endpoint only (--endpoint)
       --backend auto    Argos if available, else translate-shell, else the
                         LibreTranslate public HTTP endpoint (--endpoint)
   Overrides are applied LAST and always win over machine translation.
4. Protects machine-relevant fragments before MT and restores them afterwards so
   Argos' tokenizer cannot corrupt them:
       - placeholders ``%1``/``%2``/``%n`` -> bare opaque tokens (e.g. "PH1X";
         the digit is kept mid-token so Argos cannot strip a trailing digit),
         eliminating the ``%1 volume`` -> ``% volume`` class of bug;
       - ``\\n`` newlines are never sent to MT: each source is split on newlines
         and rejoined exactly, so leading/trailing and ``\\n\\n`` newlines survive;
       - glyphs ``… • → ✓ ✗ ▪ &`` -> tokens, restored deterministically;
       - technical terms (``initramfs``, ``mkinitcpio``, ``grub-install``, ``GRUB``,
         ``LUKS``, ``ESP``, ``UEFI``, ``EFI``, ``UKI``, ``Btrfs``, ``Boot Bitch``,
         ``pacman``/``apk``/``dnf``/``apt``/``dpkg``/``dkms``, ...) -> tokens,
         so Argos cannot mangle them (``initramfs`` -> ``initramf``).
5. Verifies placeholder integrity for every candidate (override or MT): the set
   of ``%N`` / ``%n`` placeholders (Qt translators legitimately reorder them, so
   this is a multiset comparison) and the number of ``\\n`` characters must match
   the source exactly.  Any mismatch leaves the message ``type="unfinished"``
   (English fallback) rather than shipping a corrupted format string.
6. Writes the .ts back deterministically (stable indentation, attribute order and
   escaping), so a diff of two runs is minimal and reviewable.

Overrides format
----------------
tools/translate_overrides.tsv is a tab-separated file with the columns::

    source<TAB>de<TAB>fr<TAB>es

Blank lines and lines starting with ``#`` are ignored.  A cell may be empty to
mean "no confident override for this language" (the string then falls through to
machine translation, or stays English if no backend is available).  The source
column is compared against the *unescaped* <source> text (i.e. ``&File``, not
``&amp;File``).

Exit status
-----------
0 on success (even if the backend was unavailable and only overrides applied),
2 on a usage/parse error, 3 if the input .ts cannot be parsed.
"""

from __future__ import annotations

import argparse
import csv
import json
import os
import re
import shutil
import subprocess
import sys
import urllib.error
import urllib.parse
import urllib.request
import xml.etree.ElementTree as ET
from collections import Counter

# --------------------------------------------------------------------------- #
# Constants
# --------------------------------------------------------------------------- #

# The Qt locale codes the application ships .ts/.qm catalogs for. The Argos
# backend maps a small number of these to a different package code below.
SUPPORTED_TARGETS = (
    "de", "fr", "es", "it", "pt", "nl", "pl", "ru", "zh", "ja", "ko",
    "sv", "da", "fi", "no", "cs", "hu", "ro", "tr", "el", "ar", "uk",
)

# Qt locale code -> Argos Translate package code. Qt calls Norwegian "no";
# Argos ships the Bokmål package as "nb". Everything else is identical.
ARGOS_LANGUAGE_CODES = {
    "no": "nb",
}

# The spec'd public endpoint.  NOTE: as of 2025 libretranslate.com requires a
# portal API key, so --backend auto will normally fall through to "no MT"
# unless ``trans`` is installed or --endpoint points at a working mirror.
DEFAULT_LIBRETRANSLATE_URL = "https://libretranslate.com/translate"

# A placeholder is "%1".."%99" or the Qt plural marker "%n".
PLACEHOLDER_RE = re.compile(r"%(?:\d+|n)")

# --------------------------------------------------------------------------- #
# Sentinel protection
# --------------------------------------------------------------------------- #
# Machine-relevant fragments (placeholders, newlines, glyphs and technical
# terms) are replaced by opaque sentinel tokens before the text reaches the MT
# backend and restored afterwards.  Argos' SentencePiece tokenizer is the reason:
# it strips a trailing digit from an unknown token (so "%1 volume" becomes
# "% volume"), drops leading/trailing newlines, and normalises or drops the
# glyphs.  The tokens are *bare* alphanumeric strings (letters/digits, no
# punctuation delimiters): parenthesised/bracketed tokens are rewritten by Argos
# in CJK scripts ("(PH1)" -> "( PH1)") and Arabic ("(PH1)" -> "(" + Arabic
# transliteration), dropped outright by Finnish, or transliterated by the
# Cyrillic models, while a bare token like "PH1X" is copied verbatim everywhere.
# The digit is kept mid-token (never trailing) so no model strips it.

# Machine-relevant glyphs: token -> glyph. "&" is handled separately below
# because it is a mnemonic accelerator and the following word must still be
# translated.
GLYPH_TOKENS = (
    ("GL1X", "\u2026"),  # …
    ("GL2X", "\u2022"),  # •
    ("GL3X", "\u2192"),  # →
    ("GL4X", "\u2713"),  # ✓
    ("GL5X", "\u2717"),  # ✗
    ("GL6X", "\u25aa"),  # ▪
)

# Technical terms that must survive MT verbatim (e.g. Argos turns "initramfs"
# into "initramf").  They are matched case-sensitively and word-bounded so a
# short command name cannot match inside an ordinary word ("apt" must never
# match "captured"/"adaptive", and "EFI" must not match inside "UEFI").  The
# list is sorted longest-first below so a compound term (update-initramfs,
# systemd-sysv-install) is protected before its constituent part.
PROTECTED_TERMS = tuple(sorted((
    "systemd-sysv-install", "update-initramfs", "update-grub", "grub-mkconfig",
    "grub-install", "grub-probe", "systemd-boot", "Boot Bitch",
    "mkinitcpio", "mkinitfs", "initramfs", "extlinux", "syslinux",
    "pacman", "TUXEDO", "dracut", "Btrfs", "UEFI", "NVRAM",
    "GRUB", "LUKS", "ESP", "EFI", "UKI", "ZFS", "Shell",
    "apt", "apk", "dnf", "dpkg", "dkms",
), key=len, reverse=True))

# Term token: "TR<n>X" where <n> is the 1-based index in the ordered list above.
TERM_TOKENS = {
    term: f"TR{index + 1}X" for index, term in enumerate(PROTECTED_TERMS)
}

# Placeholder token: "%N" -> "PH<N>X", "%n" -> "PHNX".
PH_DIGIT_SENTINEL_RE = re.compile(r"PH(\d+)X")
PH_N_SENTINEL = "PHNX"

ANSI_RE = re.compile(r"\x1b\[[0-9;]*m")

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DEFAULT_OVERRIDES = os.path.join(SCRIPT_DIR, "translate_overrides.tsv")


# --------------------------------------------------------------------------- #
# XML (TS) helpers
# --------------------------------------------------------------------------- #

def escape_text(value: str) -> str:
    """Escape a string for use as XML element text (matches lupdate's output)."""
    return (
        value.replace("&", "&amp;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
        .replace("'", "&apos;")
        .replace('"', "&quot;")
    )


def escape_attr(value: str) -> str:
    """Escape a string for use as an XML attribute value."""
    return escape_text(value)


def parse_ts(path: str) -> dict:
    """Parse a Qt .ts file into a plain structure (entities already decoded)."""
    try:
        tree = ET.parse(path)
    except (ET.ParseError, OSError) as exc:
        print(f"translate_ts: cannot parse {path}: {exc}", file=sys.stderr)
        sys.exit(3)

    root = tree.getroot()
    attrs = dict(root.attrib)
    contexts = []
    for ctx in root.findall("context"):
        name_el = ctx.find("name")
        name = (name_el.text or "") if name_el is not None else ""
        messages = []
        for msg in ctx.findall("message"):
            locations = [
                (loc.get("filename", ""), loc.get("line", ""))
                for loc in msg.findall("location")
            ]
            src_el = msg.find("source")
            source = (src_el.text or "") if src_el is not None else ""
            comment_el = msg.find("comment")
            comment = (comment_el.text or "") if comment_el is not None else ""
            tr_el = msg.find("translation")
            trans = (tr_el.text or "") if tr_el is not None else ""
            ttype = tr_el.get("type") if tr_el is not None else None
            messages.append(
                {
                    "locations": locations,
                    "source": source,
                    "comment": comment,
                    "translation": trans,
                    "type": ttype,
                }
            )
        contexts.append({"name": name, "messages": messages})
    return {"attrs": attrs, "contexts": contexts}


def write_ts(parsed: dict, path: str) -> None:
    """Write the structure back to path in a deterministic lupdate-style format."""
    lines = []
    lines.append('<?xml version="1.0" encoding="utf-8"?>')
    lines.append("<!DOCTYPE TS>")

    attrs = parsed["attrs"]
    ordered = []
    for key in ("version", "language"):
        if key in attrs:
            ordered.append((key, attrs[key]))
    for key, value in attrs.items():
        if key not in ("version", "language"):
            ordered.append((key, value))
    attr_str = " ".join(f'{k}="{escape_attr(v)}"' for k, v in ordered)
    lines.append(f"<TS {attr_str}>".rstrip() if attr_str else "<TS>")

    for ctx in parsed["contexts"]:
        lines.append("<context>")
        lines.append(f"    <name>{escape_text(ctx['name'])}</name>")
        for msg in ctx["messages"]:
            lines.append("    <message>")
            for filename, line in msg["locations"]:
                lines.append(
                    f'        <location filename="{escape_attr(filename)}" '
                    f'line="{escape_attr(line)}"/>'
                )
            lines.append(f"        <source>{escape_text(msg['source'])}</source>")
            # Preserve lupdate's disambiguation comment (the second argument to
            # tr()/QT_TRANSLATE_NOOP). Dropping it would let a later lupdate run
            # merge a disambiguated entry back into its plain homonym.
            if msg.get("comment"):
                lines.append(f"        <comment>{escape_text(msg['comment'])}</comment>")
            translation = msg["translation"]
            if translation.strip():
                lines.append(
                    f"        <translation>{escape_text(translation)}</translation>"
                )
            else:
                lines.append('        <translation type="unfinished"></translation>')
            lines.append("    </message>")
        lines.append("</context>")
    lines.append("</TS>")

    output = "\n".join(lines) + "\n"
    tmp_path = path + ".tmp"
    with open(tmp_path, "w", encoding="utf-8", newline="") as handle:
        handle.write(output)
    os.replace(tmp_path, path)


# --------------------------------------------------------------------------- #
# Overrides
# --------------------------------------------------------------------------- #

def load_overrides(path: str) -> dict:
    """Load the overrides TSV into {source: {target: translation}}.

    Empty cells are omitted so a language without a confident override falls
    through to the machine-translation backend.
    """
    overrides = {}
    if not os.path.isfile(path):
        print(f"translate_ts: warning: overrides file not found: {path}",
              file=sys.stderr)
        return overrides

    with open(path, "r", encoding="utf-8", newline="") as handle:
        reader = csv.reader(handle, delimiter="\t")
        for row in reader:
            if not row:
                continue
            if row[0].lstrip().startswith("#"):
                continue
            cells = [cell.strip() for cell in row]
            while len(cells) < 4:
                cells.append("")
            source, de, fr, es = cells[:4]
            if not source:
                continue
            entry = {}
            if de:
                entry["de"] = de
            if fr:
                entry["fr"] = fr
            if es:
                entry["es"] = es
            if entry:
                overrides[source] = entry
    return overrides


# --------------------------------------------------------------------------- #
# Source classification and verification
# --------------------------------------------------------------------------- #

def is_token_only(source: str) -> bool:
    """True when a source is a format/parse token rather than human text.

    Anything without a single ASCII letter is treated as programmatic (a lone
    "%1", a glyph like "✓", a separator like "---"), and left untranslated.
    """
    stripped = source.strip()
    if not stripped:
        return True
    return not re.search(r"[A-Za-z]", stripped)


def placeholders(text: str):
    """List of %N / %n placeholders in ``text`` (order preserved)."""
    return PLACEHOLDER_RE.findall(text)


def verify(source: str, translation: str) -> bool:
    """Return True when ``translation`` preserves the source's placeholder set
    (Qt translators legitimately reorder %1/%2 for their word order, so this is
    a multiset comparison) and newline count, and is non-empty."""
    if not translation or not translation.strip():
        return False
    if Counter(placeholders(translation)) != Counter(placeholders(source)):
        return False
    if translation.count("\n") != source.count("\n"):
        return False
    return True


def _placeholder_token(placeholder: str) -> str:
    """Sentinel for a matched placeholder ("%1" -> "PH1X", "%n" -> "PHNX")."""
    body = placeholder[1:]
    if body == "n":
        return PH_N_SENTINEL
    return "PH" + body + "X"


def protect_segment(segment: str) -> str:
    """Replace placeholders, glyphs, ``&`` and technical terms with sentinels.

    Operates on a *single line*: the caller splits a multi-line source on newlines
    so newlines never reach the MT backend (Argos trims leading/trailing newlines
    and collapses a consecutive ``\\n\\n`` into one)."""
    text = segment
    text = PLACEHOLDER_RE.sub(lambda match: _placeholder_token(match.group(0)), text)
    # Drop a possessive clitic directly after a placeholder token: Argos drops
    # the token otherwise ("the %1's root" -> "the PH1X's root" -> token lost),
    # and the target grammar expresses the genitive itself, so only the "'s" is
    # dropped from the MT input.
    text = re.sub(r"(PH\d+X|PHNX)['\u2019]s\b", r"\1", text)
    for token, glyph in GLYPH_TOKENS:
        text = text.replace(glyph, token)
    # "&" is a mnemonic accelerator: keep a space after the sentinel so the
    # following word stays a separate token and still gets translated.
    text = text.replace("&", "GLAMX ")
    for term, token in TERM_TOKENS.items():
        text = re.sub(r"(?<![\w-])" + re.escape(term) + r"(?![\w-])", token, text)
    return text


def restore_segment(segment: str) -> str:
    """Deterministic inverse of :func:`protect_segment`."""
    text = segment
    text = PH_DIGIT_SENTINEL_RE.sub(r"%\1", text)
    text = text.replace(PH_N_SENTINEL, "%n")
    for token, glyph in GLYPH_TOKENS:
        text = text.replace(token, glyph)
    text = text.replace("GLAMX", "&")
    text = re.sub(r"& +", "&", text)  # mnemonic "&" stays attached to its letter
    for term, token in TERM_TOKENS.items():
        text = text.replace(token, term)
    return text


def restore_mnemonic(source: str, translation: str) -> str:
    """Re-attach a leading ``&`` accelerator mnemonic.

    The mnemonic sentinel normally survives, but Argos sometimes drops it or
    moves it to the end of the word (Italian ``&File`` -> ``File &``).  For a
    source that carries a mnemonic (``&File``), guarantee the translation has
    ``&`` immediately before its first Latin letter (falling back to the start
    of the word for scripts without Latin letters, e.g. CJK).  The overrides pin
    exact accelerator letters for de/fr/es; the hard checks remain placeholders
    and newlines.
    """
    if not re.search(r"&[A-Za-z]", source):
        return translation
    if re.search(r"&[A-Za-z]", translation):
        return translation  # already immediately before an accelerator letter
    translation = translation.replace("&", "").strip()
    match = re.search(r"[A-Za-z]", translation)
    if match:
        pos = match.start()
        return translation[:pos] + "&" + translation[pos:]
    return "&" + translation


# --------------------------------------------------------------------------- #
# Translation backends
# --------------------------------------------------------------------------- #

def _strip_ansi(text: str) -> str:
    return ANSI_RE.sub("", text)


class ShellTranslator:
    """translate-shell ``trans`` backend (batched over stdin)."""

    label = "shell"

    def __init__(self, timeout: float):
        self.timeout = timeout

    def translate_lines(self, sources, target: str):
        """Translate a list of *single-line* sources, returning {source: text}."""
        if not sources:
            return {}
        try:
            proc = subprocess.run(
                ["trans", "-b", f":{target}", "-no-ansi"],
                input="\n".join(sources) + "\n",
                capture_output=True,
                text=True,
                timeout=self.timeout,
            )
        except (OSError, subprocess.SubprocessError):
            return {}
        if proc.returncode != 0:
            return {}
        out_lines = [_strip_ansi(line).strip() for line in proc.stdout.splitlines()]
        result = {}
        for src, translated in zip(sources, out_lines):
            if translated:
                result[src] = translated
        return result


class LibreTranslateTranslator:
    """LibreTranslate public HTTP backend (urllib, no third-party deps)."""

    label = "http"

    def __init__(self, base_url: str, timeout: float):
        self.base_url = base_url
        self.timeout = timeout
        self.available = True
        self.reason = None

    def _post(self, q: str, target: str):
        data = urllib.parse.urlencode(
            {"q": q, "source": "en", "target": target, "format": "text"}
        ).encode("utf-8")
        req = urllib.request.Request(
            self.base_url,
            data=data,
            headers={
                "Content-Type": "application/x-www-form-urlencoded",
                "User-Agent": "boot-repair/translate_ts (stdlib urllib)",
            },
        )
        with urllib.request.urlopen(req, timeout=self.timeout) as resp:
            payload = json.loads(resp.read().decode("utf-8"))
        return payload.get("translatedText", "")

    def translate_lines(self, sources, target: str):
        if not sources:
            return {}
        if not self.available:
            return {}
        result = {}
        try:
            joined = "\n".join(sources)
            translated = self._post(joined, target)
            out_lines = translated.split("\n")
            if len(out_lines) == len(sources):
                for src, line in zip(sources, out_lines):
                    if line.strip():
                        result[src] = line.strip()
                return result
            # Line count mismatch: fall back to one request per source.
            for src in sources:
                try:
                    translated = self._post(src, target).strip()
                    if translated:
                        result[src] = translated
                except (urllib.error.URLError, urllib.error.HTTPError,
                        OSError, ValueError, KeyError):
                    pass
        except urllib.error.HTTPError as exc:
            self.available = False
            detail = ""
            try:
                detail = exc.read().decode("utf-8", "replace").strip()
            except OSError:
                pass
            self.reason = f"HTTP {exc.code}"
            if detail:
                self.reason += f": {detail}"
        except (urllib.error.URLError, OSError, ValueError, KeyError) as exc:
            self.available = False
            self.reason = str(exc)
        return result


def backend_available_trans(prog="trans") -> bool:
    return shutil.which(prog) is not None


def backend_available_argos() -> bool:
    """True when the ``argostranslate`` package is importable (offline engine)."""
    try:
        import importlib.util
        return importlib.util.find_spec("argostranslate") is not None
    except (ImportError, ValueError):
        return False


class ArgosTranslator:
    """Argos Translate offline backend (the engine behind LibreTranslate).

    Lazily imports the optional ``argostranslate`` package and builds one
    en->target translation object per target language on first use.  A target
    without an installed ``translate-en_<code>`` package yields no translations
    (so ``auto`` can fall through to the next backend), never an error.
    """

    label = "argos"

    def __init__(self, timeout: float):
        self.timeout = timeout
        self.available = True
        self.reason = None
        self._from_lang = None
        self._installed = None
        self._translations = {}

    @staticmethod
    def _argos_code(qt_code: str) -> str:
        return ARGOS_LANGUAGE_CODES.get(qt_code, qt_code)

    def _ensure(self):
        """Import argostranslate and resolve the installed English language."""
        if self._from_lang is not None:
            return True
        if not backend_available_argos():
            self.available = False
            self.reason = "argostranslate is not installed"
            return False
        try:
            import logging
            logging.getLogger("argostranslate").setLevel(logging.ERROR)
            import argostranslate.translate as atr
            installed = atr.get_installed_languages()
        except Exception as exc:  # noqa: BLE001 - optional dev dependency
            self.available = False
            self.reason = f"argostranslate failed to load: {exc}"
            return False
        self._installed = installed
        self._from_lang = next((lang for lang in installed if lang.code == "en"),
                               None)
        if self._from_lang is None:
            self.available = False
            self.reason = "no English language package installed in Argos"
            return False
        return True

    def translate_lines(self, sources, target: str):
        """Translate a list of sources, returning {source: text}."""
        if not sources:
            return {}
        if not self.available:
            return {}
        if not self._ensure():
            return {}

        to_code = self._argos_code(target)
        translation = self._translations.get(to_code)
        if translation is None:
            to_lang = next((lang for lang in self._installed
                            if lang.code == to_code), None)
            if to_lang is None:
                self.reason = f"no translate-en_{to_code} package installed"
                return {}
            translation = self._from_lang.get_translation(to_lang)
            self._translations[to_code] = translation

        result = {}
        for src in sources:
            try:
                out = translation.translate(src)
            except Exception:  # noqa: BLE001 - a bad sentence must not abort
                continue
            if out and out.strip():
                result[src] = out.strip()
        return result


# --------------------------------------------------------------------------- #
# Machine-translation driver
# --------------------------------------------------------------------------- #

def run_machine_translation(translators, sources, target: str):
    """Translate ``sources`` (unique, human-readable) through ``translators``.

    ``translators`` is tried in order (a fallback chain): the first backend that
    produces a translation for a segment wins, so a preferred offline backend can
    be fronted by an HTTP one.  Each source is split on newlines and every
    translatable line is protected (placeholders/glyphs/terms -> sentinels),
    translated as a single line (batched), then restored; newlines are
    reconstructed from the source exactly, so Argos can never trim or collapse
    them.  A source is returned only when every one of its translatable lines
    was translated.  Returns ``({source: text}, {source: backend_label})`` —
    format verification happens in the caller.
    """
    result = {}
    producer = {}

    # source -> list of its newline-delimited lines (newlines are not sent to MT).
    lines_by_source = {src: src.split("\n") for src in sources}

    # protected-line -> list of (source, line_index).  Deduplicated so an
    # identical protected line shared by several sources is translated once.
    jobs = {}
    for src, lines in lines_by_source.items():
        for index, line in enumerate(lines):
            if re.search(r"[A-Za-z]", line):
                protected = protect_segment(line)
                jobs.setdefault(protected, []).append((src, index))

    # Translate the unique protected lines through the fallback chain.
    translated = {}     # protected line -> translated text
    line_producer = {}  # protected line -> backend label
    remaining = list(jobs)
    batch_size = 25
    for translator in translators:
        if not remaining:
            break
        got = {}
        for start in range(0, len(remaining), batch_size):
            batch = remaining[start:start + batch_size]
            got.update(translator.translate_lines(batch, target))
        for protected, text in got.items():
            translated[protected] = text
            line_producer[protected] = translator.label
        remaining = [p for p in remaining if p not in translated]

    # Reassemble each source, restoring sentinels.  All-or-nothing: if any
    # translatable line failed, the whole source is left for the caller to mark
    # unfinished rather than shipping a half-English format string.
    for src, lines in lines_by_source.items():
        restored = []
        labels = []
        complete = True
        for index, line in enumerate(lines):
            if re.search(r"[A-Za-z]", line):
                protected = protect_segment(line)
                if protected in translated:
                    restored.append(restore_segment(translated[protected]))
                    labels.append(line_producer.get(protected, "machine"))
                else:
                    complete = False
                    break
            else:
                restored.append(restore_segment(line))
        if complete:
            text = restore_mnemonic(src, "\n".join(restored))
            result[src] = text
            producer[src] = labels[0] if labels else "machine"
    return result, producer


def resolve_translators(args):
    """Return the ordered translator chain for the requested backend."""
    backend = args.backend
    if backend == "none":
        return []
    if backend == "shell":
        if backend_available_trans():
            return [ShellTranslator(args.timeout)]
        print("translate_ts: 'trans' (translate-shell) not found on PATH",
              file=sys.stderr)
        return []
    if backend == "argos":
        return [ArgosTranslator(args.timeout)]
    if backend == "libre":
        return [LibreTranslateTranslator(args.endpoint, args.timeout)]
    # auto: prefer the offline Argos engine, then translate-shell, then the
    # LibreTranslate HTTP endpoint as a last resort.
    chain = []
    if backend_available_argos():
        chain.append(ArgosTranslator(args.timeout))
    if backend_available_trans():
        chain.append(ShellTranslator(args.timeout))
    chain.append(LibreTranslateTranslator(args.endpoint, args.timeout))
    return chain


# --------------------------------------------------------------------------- #
# Main
# --------------------------------------------------------------------------- #

def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Machine-translate unfinished .ts messages for one language.",
    )
    parser.add_argument("ts", help="Path to the .ts file to fill.")
    parser.add_argument(
        "--target",
        required=True,
        choices=SUPPORTED_TARGETS,
        help="Target language code (see SUPPORTED_TARGETS).",
    )
    parser.add_argument(
        "--backend",
        choices=("auto", "argos", "shell", "libre", "none"),
        default="auto",
        help="Translation backend: auto (Argos, else translate-shell, else "
        "LibreTranslate HTTP), argos (offline Argos Translate only), shell "
        "(trans only), libre (LibreTranslate HTTP endpoint only), none "
        "(overrides only).",
    )
    parser.add_argument(
        "--endpoint",
        default=DEFAULT_LIBRETRANSLATE_URL,
        help="LibreTranslate-compatible HTTP endpoint for --backend auto "
        "(default: %(default)s).",
    )
    parser.add_argument(
        "--overrides",
        default=DEFAULT_OVERRIDES,
        help="Path to the overrides TSV (source<TAB>de<TAB>fr<TAB>es).",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=10.0,
        help="Per-request timeout in seconds (default: %(default)s).",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Compute and report coverage without writing the .ts.",
    )
    return parser


def main(argv=None) -> int:
    args = build_arg_parser().parse_args(argv)

    if not os.path.isfile(args.ts):
        print(f"translate_ts: no such file: {args.ts}", file=sys.stderr)
        return 2

    parsed = parse_ts(args.ts)
    overrides = load_overrides(args.overrides)

    # Normalise the .ts language attribute to the two-letter Qt code. lupdate
    # writes region-qualified codes (de_DE, it_IT, nb_NO); the application
    # convention is the bare code, and it also keeps the committed catalogs
    # deterministic between runs.
    parsed["attrs"]["language"] = args.target

    # Collect the unique sources that still need a translation.
    needed = []
    for ctx in parsed["contexts"]:
        for msg in ctx["messages"]:
            if not msg["translation"].strip():
                needed.append(msg["source"])
    unique_sources = list(dict.fromkeys(needed))

    # Classify: token-only (skip) vs human text.
    human_sources = [s for s in unique_sources if not is_token_only(s)]
    token_sources = [s for s in unique_sources if is_token_only(s)]

    # Overrides win; a standalone protected term stays verbatim; whatever is
    # left goes to the MT backend.
    override_result = {}
    term_only_result = {}
    mt_sources = []
    for src in human_sources:
        override = overrides.get(src, {}).get(args.target)
        if override is not None:
            override_result[src] = override
        elif src in TERM_TOKENS:
            # A source that is exactly a protected technical term (e.g. the
            # standalone command-shell label "Shell") must stay identical in
            # every language. Argos' Cyrillic/CJK models transliterate a lone
            # sentinel token ("TR19X" -> "Т19Х"), so these never reach MT.
            term_only_result[src] = src
        else:
            mt_sources.append(src)

    translators = resolve_translators(args)
    mt_result = {}
    producer = {}
    if translators and mt_sources:
        mt_result, producer = run_machine_translation(
            translators, mt_sources, args.target)

    # Finalise each source, verifying placeholder/newline integrity.  Sentinel
    # protection and restoration already happened inside the MT driver, so the
    # candidate here is the restored translation; verify is the hard invariant
    # gate that leaves a message unfinished if Argos dropped a sentinel.
    final = {}
    mismatch_warnings = []
    for src in human_sources:
        candidate = override_result.get(src)
        origin = "override"
        if candidate is None:
            candidate = term_only_result.get(src)
            origin = "term"
        if candidate is None:
            candidate = mt_result.get(src)
            origin = "machine"
        if candidate is not None and verify(src, candidate):
            final[src] = candidate
        else:
            final[src] = None
            if candidate is not None:
                mismatch_warnings.append((src, candidate, origin))

    # Apply results to the parsed structure.
    translated_count = 0
    left_count = 0
    override_count = 0
    term_count = 0
    mt_count = 0
    backend_counts = {}
    for ctx in parsed["contexts"]:
        for msg in ctx["messages"]:
            src = msg["source"]
            if not msg["translation"].strip():
                if src in final and final[src] is not None:
                    msg["translation"] = final[src]
                    msg["type"] = None
                    translated_count += 1
                    if src in override_result:
                        override_count += 1
                    elif src in term_only_result:
                        term_count += 1
                    elif src in mt_result:
                        mt_count += 1
                        label = producer.get(src, "machine")
                        backend_counts[label] = backend_counts.get(label, 0) + 1
                else:
                    msg["translation"] = ""
                    msg["type"] = "unfinished"
                    left_count += 1
            else:
                translated_count += 1  # already translated in a previous run

    total = translated_count + left_count

    if not args.dry_run:
        write_ts(parsed, args.ts)

    # ------------------------------------------------------------------ #
    # Report
    # ------------------------------------------------------------------ #
    backend_label = args.backend
    if not translators:
        if backend_label == "auto":
            backend_label = "auto (Argos/trans absent)"
        elif backend_label == "shell":
            backend_label = "shell (trans absent)"
        elif backend_label == "argos":
            backend_label = "argos (not importable)"
        else:
            backend_label = "none"

    print(f"translate_ts: target={args.target} backend={backend_label}")
    print(f"translate_ts: {args.ts}")
    print(f"translate_ts: total messages      {total}")
    print(f"translate_ts: translated          {translated_count}")
    print(f"translate_ts:   overrides          {override_count}")
    print(f"translate_ts:   protected terms     {term_count}")
    print(f"translate_ts:   machine-translated {mt_count}")
    for label in sorted(backend_counts):
        print(f"translate_ts:     {label:<8}        {backend_counts[label]}")
    print(f"translate_ts: left English        {left_count}")
    if token_sources:
        print(f"translate_ts: skipped token-only  {len(token_sources)}")
    if mismatch_warnings:
        print(f"translate_ts: placeholder/newline mismatch -> left unfinished: "
              f"{len(mismatch_warnings)}", file=sys.stderr)
        for src, cand, origin in mismatch_warnings[:20]:
            print(f"  [{origin}] {src!r} -> {cand!r}", file=sys.stderr)

    for translator in translators:
        if not translator.available:
            print(
                f"translate_ts: {translator.label} backend unavailable: "
                f"{translator.reason}",
                file=sys.stderr,
            )
    if args.backend == "auto" and not backend_counts:
        print(
            "translate_ts: no machine translations produced; install "
            "argostranslate, translate-shell (trans) or set --endpoint to a "
            "working LibreTranslate-compatible mirror.",
            file=sys.stderr,
        )

    return 0


if __name__ == "__main__":
    sys.exit(main())
