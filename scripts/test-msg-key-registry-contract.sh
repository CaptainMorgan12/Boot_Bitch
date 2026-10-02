#!/usr/bin/env bash
# Message-key registry completeness contract (Phase 2 of language-independent
# output).  Asserts that every stable `msg:<key>` token the modern helper and
# its legacy port can emit has a translatable mapping in the GUI's
# MainWindow::localizedMessage() table (kMsgSpecs), that unknown `msg:<key>`
# tokens pass through raw in the GUI (never a fabricated string), and that the
# machine-parsed contract lines stay un-keyed.
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="$ROOT_DIR/scripts/boot-repair-helper.sh"
LEGACY_HELPER="$ROOT_DIR/legacy/boot-repair-helper.sh"
GUI_SRC="$ROOT_DIR/src/MainWindow.cpp"

[[ -f "$HELPER" ]] || { echo 'FAIL: modern helper is missing' >&2; exit 1; }
[[ -f "$LEGACY_HELPER" ]] || { echo 'FAIL: legacy helper is missing (run legacy/port.sh)' >&2; exit 1; }
[[ -f "$GUI_SRC" ]] || { echo 'FAIL: MainWindow.cpp is missing' >&2; exit 1; }

# Extract the message keys a helper can emit.  The helper calls `msg_log` (and
# `msg`) with a literal lowercase kebab-case key; full-line comments are
# excluded.
extract_helper_keys()
{
    local file="$1"
    grep -vE '^[[:space:]]*#' "$file" \
        | grep -oE '\bmsg(_log)? [a-z0-9-]+' \
        | sed -E 's/^msg(_log)? //' \
        | sort -u
}

# Extract the keys from the GUI's authoritative MsgSpec table.
extract_gui_keys()
{
    sed -n '/const MsgSpec kMsgSpecs\[\] = {/,/^};/p' "$GUI_SRC" \
        | grep -oE '^[[:space:]]*\{ "[a-z0-9-]+"' \
        | grep -oE '"[a-z0-9-]+"' \
        | tr -d '"' \
        | sort -u
}

modern_keys="$(extract_helper_keys "$HELPER")"
legacy_keys="$(extract_helper_keys "$LEGACY_HELPER")"
gui_keys="$(extract_gui_keys)"

[[ -n "$gui_keys" ]] || { echo 'FAIL: GUI message-key registry is empty' >&2; exit 1; }

modern_missing="$(comm -23 <(printf '%s\n' "$modern_keys") <(printf '%s\n' "$gui_keys") || true)"
if [[ -n "$modern_missing" ]]; then
    echo 'FAIL: modern helper emits msg keys with no GUI mapping:' >&2
    printf '  %s\n' $modern_missing >&2
    exit 1
fi

legacy_missing="$(comm -23 <(printf '%s\n' "$legacy_keys") <(printf '%s\n' "$gui_keys") || true)"
if [[ -n "$legacy_missing" ]]; then
    echo 'FAIL: legacy helper emits msg keys with no GUI mapping:' >&2
    printf '  %s\n' $legacy_missing >&2
    exit 1
fi

# Unknown keys fail closed: localizedMessage() must pass an unrecognized token
# through verbatim (never a fabricated string).
grep -q 'return raw; // unknown key -> raw fallback' "$GUI_SRC" \
    || { echo 'FAIL: localizedMessage() lost the unknown-key raw fallback' >&2; exit 1; }

# The machine-parsed contract lines must never be keyed: the helper's own
# `Repair tool <key>:` / `Repair change status <key>:` / `ERROR:` / `Host
# default:` evidence and the backend-profile labels are emitted through
# printf/echo/log (never msg_log).  Guard the canonical emit sites directly.
for f in "$HELPER" "$LEGACY_HELPER"; do
    grep -q 'log "ERROR:' "$f" \
        || { echo "FAIL: $f lost its parsed ERROR: emit sites" >&2; exit 1; }
    grep -q 'log "Host default:' "$f" \
        || { echo "FAIL: $f lost its parsed Host default: emit sites" >&2; exit 1; }
    grep -q "printf 'Repair tool %s: available" "$f" \
        || { echo "FAIL: $f lost its parsed Repair tool emit site" >&2; exit 1; }
    grep -q "printf 'Repair change status %s: %s" "$f" \
        || { echo "FAIL: $f lost its parsed Repair change status emit site" >&2; exit 1; }
    grep -q 'echo "Package manager backends:' "$f" \
        || { echo "FAIL: $f lost its parsed backend-profile label" >&2; exit 1; }
done

printf 'msg-key registry: helper=%s legacy=%s gui=%s (complete)\n' \
    "$(printf '%s\n' "$modern_keys" | wc -l | tr -d '[:space:]')" \
    "$(printf '%s\n' "$legacy_keys" | wc -l | tr -d '[:space:]')" \
    "$(printf '%s\n' "$gui_keys" | wc -l | tr -d '[:space:]')"
