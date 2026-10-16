#!/usr/bin/env bash
# legacy/tests/test-ui-parity.sh — the "UI port --check" analog of legacy/port.sh --check.
#
# Extracts the user-visible string surface from BOTH GUIs deterministically and
# compares the two SETS, mirroring port.sh --check's drift-fail behaviour for
# the Etch (Qt3) GUI vs the modern (Qt6) GUI:
#
#   modern = every tr("...") literal in src/MainWindow.cpp
#   legacy = every tr("...") and QT_TRANSLATE_NOOP("LegacyMainWindow","...")
#            literal in legacy/gui/src/LegacyMainWindow.cpp
#
# The comparison keys on the decoded literal text.  String escapes (\" and \\)
# are decoded; \n/\t/\r are re-encoded as the canonical two-character escapes
# so every literal stays a single, sortable line.  A pure %N-index renumber is
# treated as identical (both sides are normalised to %N), while wording
# differences and a differing placeholder COUNT are still reported.
#
# legacy/tests/ui-parity-allowlist.txt (one literal per line, '#' comments)
# holds the modern strings that are KNOWN not-applicable to Etch.  Buckets:
# [feature] strings for features/backends Etch lacks, [helper] reason/msg-key
# localisations the legacy GUI shows verbatim from the helper, [status] repair
# change-status result phrases + GUI status/busy/log lines (the legacy GUI
# renders its own ASCII summary/wording), and [glyph] already-ported labels
# that differ only by the ellipsis glyph.  A modern-only string present in the
# allowlist is not a failure.
#
# Exit 0 only when every non-allowlisted modern-only string is absent (full
# parity modulo the allowlist); otherwise it prints the missing literals and
# exits 1, exactly like `legacy/port.sh --check` fails on drift.
#
# This script runs on the HOST (dev/CI), not on Etch, so it may use comm/join
# and is not held to the bash-3.1 cleanliness that port.sh requires.
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
MODERN="$ROOT_DIR/src/MainWindow.cpp"
LEGACY="$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp"
ALLOWLIST="$ROOT_DIR/legacy/tests/ui-parity-allowlist.txt"

for file in "$MODERN" "$LEGACY" "$ALLOWLIST"; do
    [[ -f "$file" ]] || { printf 'FAIL: missing input file: %s\n' "${file#"$ROOT_DIR"/}" >&2; exit 1; }
done

TMP="$(mktemp -d "${TMPDIR:-/tmp}/boot-repair-ui-parity.XXXXXX")"
trap 'rm -rf "$TMP"' EXIT

# --- the extractor -----------------------------------------------------------
# Reads a source file into memory and scans it char-by-char for tr(...) and
# QT_TRANSLATE_NOOP(...) calls, skipping // and /* */ comments, and emits one
# canonical literal per line.  Canonical form is single-line: \n, \t, \r are
# written as backslash escapes and a literal backslash is written \\, so the
# output is deterministic, sortable and allowlist-friendly.
cat > "$TMP/extract.awk" <<'AWK'
BEGIN { buf = "" }
{ buf = buf $0 "\n" }
END { scan(buf) }

function ident(c) {
    return (c == "_" || (c >= "A" && c <= "Z") || (c >= "a" && c <= "z") || (c >= "0" && c <= "9"))
}

# Parse one C++ string literal starting at buf[IP] == '"'.  Advances IP past
# the closing quote.  Returns the canonical (escaped) literal text.
function parse_string(   out, c) {
    IP++                                # skip opening quote
    out = ""
    while (IP <= N) {
        c = substr(buf, IP, 1)
        if (c == "\\") {
            IP++
            if (IP > N) return out
            c = substr(buf, IP, 1)
            if (c == "n") out = out "\\n"
            else if (c == "t") out = out "\\t"
            else if (c == "r") out = out "\\r"
            else if (c == "\\") out = out "\\\\"
            else if (c == "\"") out = out "\""
            else out = out "\\" c
            IP++
        } else if (c == "\"") {
            IP++                        # closing quote
            return out
        } else {
            out = out c
            IP++
        }
    }
    return out
}

# Skip whitespace; return 1 if more input remains.
function skip_ws(   c) {
    while (IP <= N) {
        c = substr(buf, IP, 1)
        if (c == " " || c == "\t" || c == "\n" || c == "\r") { IP++; continue }
        return 1
    }
    return 0
}

# Collect one-or-more adjacent string literals (C++ concatenation) starting at
# the current IP.  Stops at the first non-whitespace, non-quote char.
function collect_literals(   out) {
    out = ""
    while (skip_ws()) {
        if (substr(buf, IP, 1) == "\"") { out = out parse_string() } else { break }
    }
    return out
}

function scan(   i, c) {
    N = length(buf)
    IP = 1
    while (IP <= N) {
        c = substr(buf, IP, 1)
        # // line comment
        if (c == "/" && substr(buf, IP+1, 1) == "/") {
            IP += 2
            while (IP <= N && substr(buf, IP, 1) != "\n") IP++
            continue
        }
        # /* */ block comment
        if (c == "/" && substr(buf, IP+1, 1) == "*") {
            IP += 2
            while (IP <= N && !(substr(buf, IP, 1) == "*" && substr(buf, IP+1, 1) == "/")) IP++
            IP += 2
            continue
        }
        # tr(  (preceding char must not be an identifier char, to skip substr(/c_str()
        if (c == "t" && substr(buf, IP+1, 1) == "r" && substr(buf, IP+2, 1) == "(" && !ident(substr(buf, IP-1, 1))) {
            IP += 3
            src = collect_literals()
            if (src != "") print src
            continue
        }
        # QT_TRANSLATE_NOOP(
        if (c == "Q" && substr(buf, IP, 17) == "QT_TRANSLATE_NOOP" && substr(buf, IP+17, 1) == "(") {
            IP += 18
            collect_literals()            # context literal (ignored)
            skip_ws()
            if (substr(buf, IP, 1) == ",") IP++
            src = collect_literals()
            if (src != "") print src
            continue
        }
        IP++
    }
}
AWK

# --- extract + normalise ------------------------------------------------------
# norm_key replaces every %N placeholder index with %N so a pure renumber of
# the same placeholders compares equal, while wording/count differences still
# differ.  The raw canonical literals (real indices) are kept for reporting.
norm_key() { sed 's/%[0-9][0-9]*/%N/g'; }

LC_ALL=C awk -f "$TMP/extract.awk" "$MODERN" > "$TMP/modern.raw"
LC_ALL=C awk -f "$TMP/extract.awk" "$LEGACY"  > "$TMP/legacy.raw"

MODERN_SITES="$(wc -l < "$TMP/modern.raw" | tr -d '[:space:]')"
LEGACY_SITES="$(wc -l < "$TMP/legacy.raw" | tr -d '[:space:]')"

norm_key < "$TMP/modern.raw" | LC_ALL=C sort -u > "$TMP/modern.keys"
norm_key < "$TMP/legacy.raw"  | LC_ALL=C sort -u > "$TMP/legacy.keys"

MODERN_N="$(wc -l < "$TMP/modern.keys" | tr -d '[:space:]')"
LEGACY_N="$(wc -l < "$TMP/legacy.keys" | tr -d '[:space:]')"

LC_ALL=C comm -23 "$TMP/modern.keys" "$TMP/legacy.keys" > "$TMP/modern-only.keys"
LC_ALL=C comm -13 "$TMP/modern.keys" "$TMP/legacy.keys" > "$TMP/legacy-only.keys"
MODERN_ONLY_N="$(wc -l < "$TMP/modern-only.keys" | tr -d '[:space:]')"
LEGACY_ONLY_N="$(wc -l < "$TMP/legacy-only.keys" | tr -d '[:space:]')"

# key -> first canonical literal (real %N indices), for readable reporting.
TAB="$(printf '\t')"
awk '{ k = $0; gsub(/%[0-9]+/, "%N", k); print k "\t" $0 }' "$TMP/modern.raw" \
    | LC_ALL=C sort -u -t "$TAB" -k1,1 > "$TMP/modern.map"
awk '{ k = $0; gsub(/%[0-9]+/, "%N", k); print k "\t" $0 }' "$TMP/legacy.raw" \
    | LC_ALL=C sort -u -t "$TAB" -k1,1 > "$TMP/legacy.map"

LC_ALL=C join -t "$TAB" -1 1 -2 1 "$TMP/modern-only.keys" "$TMP/modern.map" \
    | cut -f2- | LC_ALL=C sort > "$TMP/modern-only.real"
LC_ALL=C join -t "$TAB" -1 1 -2 1 "$TMP/legacy-only.keys" "$TMP/legacy.map" \
    | cut -f2- | LC_ALL=C sort > "$TMP/legacy-only.real"

# --- allowlist ----------------------------------------------------------------
# One literal per line, exact match (leading/trailing spaces are significant),
# '#' starts a comment, blank lines are skipped.  A trailing CR is stripped.
awk '!/^[[:space:]]*#/ && /[^[:space:]]/ { sub(/\r$/, ""); print }' "$ALLOWLIST" \
    | norm_key | LC_ALL=C sort -u > "$TMP/allow.keys"
ALLOW_N="$(wc -l < "$TMP/allow.keys" | tr -d '[:space:]')"

LC_ALL=C comm -23 "$TMP/modern-only.keys" "$TMP/allow.keys" > "$TMP/port.keys"
PORT_N="$(wc -l < "$TMP/port.keys" | tr -d '[:space:]')"
LC_ALL=C join -t "$TAB" -1 1 -2 1 "$TMP/port.keys" "$TMP/modern.map" \
    | cut -f2- | LC_ALL=C sort > "$TMP/port.real"

# --- report -------------------------------------------------------------------
printf '== legacy UI parity ==\n'
printf 'modern tr() literals (call sites): %s\n' "$MODERN_SITES"
printf 'legacy tr()/NOOP literals (call sites): %s\n' "$LEGACY_SITES"
printf 'modern unique strings: %s\n' "$MODERN_N"
printf 'legacy unique strings: %s\n' "$LEGACY_N"
printf 'modern-only: %s\n' "$MODERN_ONLY_N"
printf 'legacy-only: %s\n' "$LEGACY_ONLY_N"
printf 'allowlisted (N/A feature/helper/status/glyph): %s\n' "$ALLOW_N"
printf 'PORT backlog (modern-only, not allowlisted): %s\n' "$PORT_N"

if [[ "$PORT_N" -gt 0 ]]; then
    printf '\nmodern-only strings not ported (PORT backlog):\n'
    sed 's/^/  /' "$TMP/port.real"
fi

if [[ "$LEGACY_ONLY_N" -gt 0 ]]; then
    printf '\nlegacy-only strings (keep/remove candidates):\n'
    sed 's/^/  /' "$TMP/legacy-only.real"
fi

if [[ "$PORT_N" -eq 0 ]]; then
    printf '\nLEGACY UI PARITY: in sync (modulo %s allowlisted N/A strings)\n' "$ALLOW_N"
    exit 0
fi

printf '\nLEGACY UI PARITY: %s modern-only strings not ported\n' "$PORT_N" >&2
exit 1
