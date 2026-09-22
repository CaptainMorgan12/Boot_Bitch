#!/usr/bin/env bash
# Fast host-side contract for the legacy Qt3 GUI (L1).
#
#   - compiles the Qt-free EvidenceParser/DeviceInventory modules with plain
#     g++ -std=c++98 (no Qt3 needed on the host) and runs the fixture tests:
#     capability parsing, fail-closed gating, change-status invalidation and
#     the read-only device-inventory parsers
#   - asserts the GUI sources use Qt3 widgets/QProcess and the stable evidence
#     prefixes, expose only the legacy-supported command set (including the
#     guarded GRUB-legacy stage), consume the helper's probe-based
#     `Legacy feature` lines for gating/greyed reasons, provide the
#     per-diagnostic runs and the read-only configuration viewer, and keep
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
grep -q 'Legacy feature ' "$PARSER" \
    || fail "parser lost the 'Legacy feature' prefix"
for key in validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub extlinux bootstack; do
    grep -q "\"$key\"" "$PARSER" || fail "capability key missing from the parser: $key"
done
for feature in file-copy shell host-shell host-maintenance snapshots host-default; do
    grep -q "\"$feature\"" "$PARSER" || fail "legacy feature key missing from the parser: $feature"
done
for key in environment backend boot boot-evidence kernel grub uki display errors usage filesystem fstab btrfs mapper luks report; do
    grep -q "\"$key\"" "$PARSER" || fail "diagnostic key missing from the parser: $key"
done
grep -q 'legacyFeatureAvailable' "$PARSER" || fail "parser lost the fail-closed legacy feature gate"
grep -q 'legacyFeatureIsAvailable' "$PARSER" || fail "parser lost the legacy feature state helper"
pass "13-key evidence contract + 6 legacy feature probes + 16 diagnostic keys"

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
for stage in validate fs-inspect fix-broken dpkg-configure apt-update apt-upgrade initramfs grub; do
    grep -q "\"$stage\"" <<<"$ACTION_BLOCK" || fail "legacy stage missing from the GUI: $stage"
done
# `unlock` is a dedicated control (passphrase on stdin), never a repair stage;
# the other modern-only stages stay forbidden in the action table.
for forbidden in snapshots host-default host-shell fs-repair copy unlock; do
    grep -q "\"$forbidden\"" <<<"$ACTION_BLOCK" \
        && fail "unsupported stage exposed by the GUI: $forbidden"
done
grep -q 'host-validate' "$WINDOW" || fail "GUI lost the host-validate command form"
grep -q 'host-diagnose' "$WINDOW" || fail "GUI lost the host-diagnose command form"
grep -q 'host-repair' "$WINDOW" || fail "GUI lost the host-repair command form"
grep -q 'hostMaintenance' "$WINDOW" || fail "GUI lost the host-maintenance feature gate"
pass "legacy command set only (validate/diagnose/fs-inspect/repairs/grub)"

# --- modern-GUI parity controls ---------------------------------------------
grep -q '"unlock"' "$WINDOW" || fail "GUI lost the dedicated unlock command"
grep -q 'm_unlockButton' "$WINDOW" || fail "GUI lost the Unlock button"
grep -q 'QLineEdit::Password' "$WINDOW" || fail "unlock passphrase entry is not hidden"
grep -q 'unlockAuthFailed' "$WINDOW" || fail "GUI lost the unlock retry path"
grep -q 'UNLOCK_AUTH_FAILED' "$PARSER" || fail "parser lost the unlock auth-failure marker"
grep -q 'protected running host cannot be unlocked' "$WINDOW" \
    || fail "unlock is not fail-closed for the protected running host"
grep -q 'writeToStdin' "$GUI_DIR/src/HelperRunner.cpp" \
    || fail "HelperRunner cannot send the passphrase over stdin"
grep -q 'Re-check elevation' "$WINDOW" || fail "GUI lost the elevation re-check control"
grep -q 'Elevation:' "$WINDOW" || fail "GUI lost the elevation state label"
for filter in 'All tools' 'Available only' 'Unavailable only' 'All entries' 'Errors and warnings'; do
    grep -q "$filter" "$WINDOW" || fail "GUI lost a filter option: $filter"
done
grep -q 'Modern features not available' "$WINDOW" \
    || fail "GUI lost the greyed modern-features list"
# Per-diagnostic runs + read-only config viewer (parity G1/G2).
for marker in 'm_diagnosticList' 'm_runDiagnosticButton' 'Run selected diagnostic' \
    'runSelectedDiagnostic' 'diagnosticKeys' 'Copy results' 'm_copyResultsButton'; do
    grep -q "$marker" "$WINDOW" || fail "per-diagnostic control missing: $marker"
done
for marker in 'm_configCombo' 'm_configButton' 'View target file (read-only)' \
    'viewConfigFile' 'config-read' 'm_configView'; do
    grep -q "$marker" "$WINDOW" || fail "read-only config viewer marker missing: $marker"
done
grep -q 'grub' <<<"$ACTION_BLOCK" || fail "guarded GRUB action missing"
# Probe-based legacy feature gating (fail closed) replaces hardcoded reasons.
for marker in 'legacyFeatureAvailable' 'legacyFeatureState' 'legacyFeatureDisplay' \
    'updateLegacyFeatureView' 'updateFeatureTab' 'helper probe:'; do
    grep -q "$marker" "$WINDOW" || fail "legacy feature gating marker missing: $marker"
done
grep -q "the legacy helper exposes no chroot shell command" "$WINDOW" \
    && fail "hardcoded chroot-shell reason still present"
grep -q "not supported by the legacy helper on Etch" "$WINDOW" \
    && fail "hardcoded host-default reason still present"
pass "parity controls (unlock, elevation, filters, per-diagnostic runs, config viewer, probe gating)"

# --- tab parity, Systems panel, unlock status and Logs session/search --------
for tab in 'Systems' 'Diagnostics' 'Repair' 'Chroot Shell' 'File Copy' 'Logs' 'Settings' 'About'; do
    grep -q "\"$tab\"" "$WINDOW" || fail "GUI lost a modern-parity tab: $tab"
done
for marker in 'Selected drive details' 'Connection:' 'UUID:' 'Protection:' 'updateDriveDetails'; do
    grep -q "$marker" "$WINDOW" || fail "Systems details panel marker missing: $marker"
done
for marker in 'Unlock status' 'm_unlockStatusCache' 'State: unlocked' 'State: locked' 'visibleMapperForDisk'; do
    grep -q "$marker" "$WINDOW" || fail "unlock status marker missing: $marker"
done
for marker in 'Search log:' 'm_logSearchEdit' 'Session logs' 'm_sessionLogList' 'refreshSessionLogList' 'sessionLogSelectionChanged'; do
    grep -q "$marker" "$WINDOW" || fail "Logs session/search marker missing: $marker"
done
for marker in 'Application configuration' 'm_logWrapCheck' 'Mandatory safety controls'; do
    grep -q "$marker" "$WINDOW" || fail "Settings tab marker missing: $marker"
done
for marker in 'm_targetCommitted' 'm_hostMaintenance' 'diagnosticsScopeReady' 'scopeReadyReason' 'Set as repair target' 'Host Maintenance'; do
    grep -q "$marker" "$WINDOW" || fail "modern diagnostics gating marker missing: $marker"
done
pass "tab parity (details/unlock/session/search/settings/gating)"

# --- modal elevation prompt, no inline sudo password echo -------------------
grep -q 'elevationNeedsPassword' "$WINDOW" || fail "GUI does not pre-check an interactive elevation"
grep -q 'authenticateElevation' "$WINDOW" || fail "GUI does not authenticate interactive elevation"
grep -q 'Administrator authorization' "$WINDOW" || fail "GUI lost the modal authorization dialog"
grep -q 'elevationNeedsPassword' "$GUI_DIR/src/HelperRunner.cpp" \
    || fail "HelperRunner lost the interactive-elevation check"
grep -q 'authenticateElevation' "$GUI_DIR/src/HelperRunner.cpp" \
    || fail "HelperRunner lost the sudo authentication path"
grep -q '"\-S", "\-p", "", "\-v"' "$GUI_DIR/src/HelperRunner.cpp" \
    || fail "HelperRunner no longer feeds sudo -S -v over the pipe"
grep -q 'command: %1 %2' "$WINDOW" || fail "GUI lost the command transcript line"
grep -q 'never logged or placed' "$WINDOW" || fail "GUI lost the secret-handling wording"
pass "modal hidden-input elevation prompt (sudo -S -v, no inline echo)"

# --- device inventory details (UUID/label/transport) -------------------------
grep -q 'readIdentityLinks' "$GUI_DIR/src/DeviceInventory.cpp" \
    || fail "DeviceInventory lost the /dev/disk/by-* identity links"
grep -q 'resolveLinkTarget' "$GUI_DIR/src/DeviceInventory.cpp" \
    || fail "DeviceInventory lost the symlink resolution"
grep -q 'transportForDisk' "$GUI_DIR/src/DeviceInventory.cpp" \
    || fail "DeviceInventory lost the connection probe"
grep -q 'uuid' "$GUI_DIR/src/DeviceInventory.h" || fail "DeviceRow lost the UUID field"
grep -q 'transport' "$GUI_DIR/src/DeviceInventory.h" || fail "DeviceRow lost the transport field"
pass "read-only UUID/label/transport inventory for the details panel"

# --- layout guards and the extended smoke test ------------------------------
grep -q 'QFontMetrics' "$WINDOW" || fail "GUI lost the title/button font-metric layout guard"
grep -q 'setMinimumWidth' "$WINDOW" || fail "GUI lost the minimum-width layout guard"
grep -q 'setMinimumHeight' "$WINDOW" || fail "GUI lost the minimum-height layout guard"
grep -q 'setChildrenCollapsible(false)' "$WINDOW" \
    || fail "splitter panes may collapse and clip their titles"
grep -q 'setMinimumSize(820, 600)' "$WINDOW" || fail "window minimum size contract changed"
grep -q 'verifyLayout' "$WINDOW" || fail "GUI lost the programmatic layout verification"
grep -q 'verifySmokeControls' "$WINDOW" || fail "GUI lost the smoke control verification"
grep -q 'controls: %3; layout: %4' "$WINDOW" \
    || fail "--smoke-test does not report the controls/layout results"
pass "layout guards and the extended --smoke-test coverage"

# --- headless CLI before QApplication ---------------------------------------
MAIN="$GUI_DIR/src/main.cpp"
help_line="$(grep -n 'printUsage(stdout, argv\[0\])' "$MAIN" | head -1 | cut -d: -f1)"
app_line="$(grep -n 'QApplication application(argc, argv)' "$MAIN" | head -1 | cut -d: -f1)"
[[ -n "$help_line" && -n "$app_line" ]] || fail "cannot locate --help/QApplication in main.cpp"
[[ "$help_line" -lt "$app_line" ]] || fail "--help must be handled before QApplication (headless)"
grep -q -- '--print-config' "$MAIN" || fail "main.cpp lost --print-config"
grep -q -- '--smoke-test' "$MAIN" || fail "main.cpp lost --smoke-test"
pass "headless --help/--version/--print-config before QApplication"

# --- GUI is the packaged entry point (no TUI launcher fallback) --------------
grep -q -- '--tui' "$WINDOW" \
    && fail "GUI still advertises the removed boot-repair-legacy --tui fallback"
grep -q 'TUI fallback' "$WINDOW" && fail "GUI still documents a TUI fallback"
grep -q 'TUI fallback' "$MAIN" && fail "GUI usage still documents a TUI fallback"
grep -q "package's entry point" "$WINDOW" \
    || fail "GUI About does not state the packaged entry point"
pass "GUI is the packaged entry point (no launcher fallback advertised)"

echo "legacy GUI contract: PASS"
