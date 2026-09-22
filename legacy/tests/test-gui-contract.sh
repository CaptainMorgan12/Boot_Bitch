#!/usr/bin/env bash
# Fast host-side contract for the legacy Qt3 GUI (L1).
#
#   - compiles the Qt-free EvidenceParser/DeviceInventory modules with plain
#     g++ -std=c++98 (no Qt3 needed on the host) and runs the fixture tests:
#     capability parsing, fail-closed gating, change-status invalidation and
#     the read-only device-inventory parsers
#   - asserts the GUI sources use Qt3 widgets/QProcess and the stable evidence
#     prefixes, expose only the legacy-supported command set and keep
#     --help/--print-config usable before QApplication is created
#
# The full Qt3 build runs natively on the Etch guest via scripts/package-legacy.sh;
# this test never builds a package, installs anything or touches a disk.
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
GUI_DIR="$ROOT_DIR/legacy/gui"
TESTS_DIR="$ROOT_DIR/legacy/tests"
FIXTURE="$TESTS_DIR/fixtures/gui-host-diagnose.txt"

fail()
{
    printf 'FAIL: %s\n' "$*" >&2
    exit 1
}

pass()
{
    printf 'ok - %s\n' "$*"
}

echo "== legacy GUI contract =="

for file in \
    "$GUI_DIR/boot-bitch-legacy.pro" \
    "$GUI_DIR/src/EvidenceParser.cpp" \
    "$GUI_DIR/src/DeviceInventory.cpp" \
    "$GUI_DIR/src/HelperRunner.cpp" \
    "$GUI_DIR/src/LegacyMainWindow.cpp" \
    "$GUI_DIR/src/main.cpp" \
    "$TESTS_DIR/gui-parser-test.cpp" \
    "$FIXTURE"
do
    [[ -f "$file" ]] || fail "missing GUI source: ${file#"$ROOT_DIR"/}"
done

# --- Qt-free parser/device tests (host-compilable) --------------------------
command -v g++ >/dev/null 2>&1 \
    || fail "g++ is required for the host-side GUI parser contract"

TMP="$(mktemp -d "${TMPDIR:-/tmp}/boot-repair-legacy-gui-contract.XXXXXX")"
trap 'rm -rf "$TMP"' EXIT

g++ -std=c++98 -Wall -Wextra -Werror -I"$GUI_DIR/src" \
    -o "$TMP/gui-parser-test" \
    "$TESTS_DIR/gui-parser-test.cpp" \
    "$GUI_DIR/src/EvidenceParser.cpp" \
    "$GUI_DIR/src/DeviceInventory.cpp" \
    || fail "GUI parser modules failed to compile with g++ -std=c++98"
pass "Qt-free parser modules compile (g++ -std=c++98 -Werror)"

"$TMP/gui-parser-test" "$FIXTURE" || fail "GUI parser fixture test failed"
pass "capability parsing, fail-closed gating and device parsers"

# --- evidence contract prefixes ---------------------------------------------
PARSER="$GUI_DIR/src/EvidenceParser.cpp"
grep -q 'Repair tool ' "$PARSER" || fail "parser lost the 'Repair tool' prefix"
grep -q 'Repair capability evidence ' "$PARSER" \
    || fail "parser lost the 'Repair capability evidence' prefix"
grep -q 'Repair change status ' "$PARSER" \
    || fail "parser lost the 'Repair change status' prefix"
for key in validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub extlinux bootstack; do
    grep -q "\"$key\"" "$PARSER" || fail "capability key missing from the parser: $key"
done
pass "13-key evidence contract present"

# --- Qt3 widget/toolkit usage ------------------------------------------------
for symbol in QMainWindow QListView QTextEdit QTabWidget QProcess; do
    grep -rq "$symbol" "$GUI_DIR/src" || fail "GUI does not use $symbol"
done
grep -q 'qprocess.h' "$GUI_DIR/src/HelperRunner.cpp" || fail "HelperRunner does not use QProcess"
grep -q '^CONFIG  += qt' "$GUI_DIR/boot-bitch-legacy.pro" || fail "qmake project lost CONFIG += qt"
grep -qE '^[[:space:]]*(CONFIG|QT|LIBS)[^#]*kdelibs' "$GUI_DIR/boot-bitch-legacy.pro" \
    && fail "qmake project must not require kdelibs"
pass "Qt3-only widget/toolkit usage (no kdelibs)"

# --- only the legacy-supported command set ----------------------------------
WINDOW="$GUI_DIR/src/LegacyMainWindow.cpp"
ACTION_BLOCK="$(sed -n '/^const ActionSpec actionSpecs\[\] = {/,/^};/p' "$WINDOW")"
[[ -n "$ACTION_BLOCK" ]] || fail "actionSpecs block not found"
for stage in validate fs-inspect fix-broken dpkg-configure apt-update apt-upgrade initramfs; do
    grep -q "\"$stage\"" <<<"$ACTION_BLOCK" || fail "legacy stage missing from the GUI: $stage"
done
for forbidden in snapshots host-default host-shell fs-repair copy unlock; do
    grep -q "\"$forbidden\"" <<<"$ACTION_BLOCK" \
        && fail "unsupported stage exposed by the GUI: $forbidden"
done
grep -q 'host-validate' "$WINDOW" || fail "GUI lost the host-validate command form"
grep -q 'host-diagnose' "$WINDOW" || fail "GUI lost the host-diagnose command form"
grep -q 'host-repair' "$WINDOW" || fail "GUI lost the host-repair command form"
pass "legacy command set only (validate/diagnose/fs-inspect/repairs)"

# --- headless CLI before QApplication ---------------------------------------
MAIN="$GUI_DIR/src/main.cpp"
help_line="$(grep -n 'printUsage(stdout, argv\[0\])' "$MAIN" | head -1 | cut -d: -f1)"
app_line="$(grep -n 'QApplication application(argc, argv)' "$MAIN" | head -1 | cut -d: -f1)"
[[ -n "$help_line" && -n "$app_line" ]] || fail "cannot locate --help/QApplication in main.cpp"
[[ "$help_line" -lt "$app_line" ]] || fail "--help must be handled before QApplication (headless)"
grep -q -- '--print-config' "$MAIN" || fail "main.cpp lost --print-config"
grep -q -- '--smoke-test' "$MAIN" || fail "main.cpp lost --smoke-test"
pass "headless --help/--version/--print-config before QApplication"

# --- launcher documents the same command set --------------------------------
LAUNCHER="$ROOT_DIR/legacy/launcher/boot-repair-legacy"
grep -q 'apt-upgrade' "$LAUNCHER" || fail "launcher command list is missing apt-upgrade"
pass "launcher documents the GUI command set"

echo "legacy GUI contract: PASS"
