#!/usr/bin/env bash
# Reason-key registry completeness contract (Phase 1 of language-independent
# output).  Asserts that every stable `reason:<key>` token the modern helper
# and its legacy port can emit has a translatable mapping in the GUI's
# MainWindow::localizedReason() table, and that unknown keys fail closed in the
# GUI (passed through verbatim, never treated as "available").
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="$ROOT_DIR/scripts/boot-repair-helper.sh"
LEGACY_HELPER="$ROOT_DIR/legacy/boot-repair-helper.sh"
GUI_SRC="$ROOT_DIR/src/MainWindow.cpp"

[[ -f "$HELPER" ]] || { echo 'FAIL: modern helper is missing' >&2; exit 1; }
[[ -f "$LEGACY_HELPER" ]] || { echo 'FAIL: legacy helper is missing (run legacy/port.sh)' >&2; exit 1; }
[[ -f "$GUI_SRC" ]] || { echo 'FAIL: MainWindow.cpp is missing' >&2; exit 1; }

# Extract the reason keys a helper can emit.  The helper calls the `reason`
# function as `reason <key> [param ...]` with a literal lowercase kebab-case
# key; full-line comments are excluded and a short curated list of variable
# names that sit next to the word `reason` (`local reason <var>` declarations,
# `$reason <var>` expansions) is dropped.
extract_helper_keys()
{
    local file="$1"
    grep -vE '^[[:space:]]*#' "$file" \
        | grep -oE '\breason [a-z0-9-]+' \
        | sed 's/^reason //' \
        | sort -u \
        | grep -vE '^(key|param|hint|i|kver|layout|reasons|target|that)$'
}

# Extract the keys from the GUI's authoritative ReasonSpec table.
extract_gui_keys()
{
    sed -n '/const ReasonSpec kReasonSpecs\[\] = {/,/^};/p' "$GUI_SRC" \
        | grep -oE '^[[:space:]]*\{ "[a-z0-9-]+"' \
        | grep -oE '"[a-z0-9-]+"' \
        | tr -d '"' \
        | sort -u
}

modern_keys="$(extract_helper_keys "$HELPER")"
legacy_keys="$(extract_helper_keys "$LEGACY_HELPER")"
gui_keys="$(extract_gui_keys)"

[[ -n "$gui_keys" ]] || { echo 'FAIL: GUI reason-key registry is empty' >&2; exit 1; }

modern_missing="$(comm -23 <(printf '%s\n' "$modern_keys") <(printf '%s\n' "$gui_keys") || true)"
if [[ -n "$modern_missing" ]]; then
    echo 'FAIL: modern helper emits reason keys with no GUI mapping:' >&2
    printf '  %s\n' $modern_missing >&2
    exit 1
fi

legacy_missing="$(comm -23 <(printf '%s\n' "$legacy_keys") <(printf '%s\n' "$gui_keys") || true)"
if [[ -n "$legacy_missing" ]]; then
    echo 'FAIL: legacy helper emits reason keys with no GUI mapping:' >&2
    printf '  %s\n' $legacy_missing >&2
    exit 1
fi

# Unknown keys fail closed: localizedReason() must pass an unrecognized token
# through verbatim (never a fabricated string) and the capability gate must
# treat only the literal `available` state as available.
grep -q 'return raw; // unknown key -> raw fallback' "$GUI_SRC" \
    || { echo 'FAIL: localizedReason() lost the unknown-key raw fallback' >&2; exit 1; }
grep -q 'state == QStringLiteral("available")' "$GUI_SRC" \
    || { echo 'FAIL: the capability gate no longer gates on the literal available state' >&2; exit 1; }

printf 'reason-key registry: helper=%s legacy=%s gui=%s (complete)\n' \
    "$(printf '%s\n' "$modern_keys" | wc -l | tr -d '[:space:]')" \
    "$(printf '%s\n' "$legacy_keys" | wc -l | tr -d '[:space:]')" \
    "$(printf '%s\n' "$gui_keys" | wc -l | tr -d '[:space:]')"
