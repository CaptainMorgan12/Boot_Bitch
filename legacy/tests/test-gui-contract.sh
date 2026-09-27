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
#     `Legacy feature` and `Legacy config` lines for gating/reasons, provide
#     the per-diagnostic runs and the target-only Edit Target File control
#     with the Etch-era configuration key list, and keep --help/--print-config
#     usable before QApplication is created
#
# The full Qt3 build runs natively on the Etch guest via scripts/package-legacy.sh;
# this test never builds a package, installs anything or touches a disk.
set -euo pipefail
# Every assertion is an explicit ||/&& check; pipefail adds no coverage but
# turns `producer | grep -q` into a load-dependent false failure whenever
# grep -q exits early and the producer takes SIGPIPE (141). Disable it.
set +o pipefail

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
grep -q 'Legacy config ' "$PARSER" \
    || fail "parser lost the 'Legacy config' prefix"
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
for key in fstab inittab menu-lst crypttab modules interfaces sources-list apt-conf; do
    grep -q "\"$key\"" "$PARSER" || fail "Etch configuration key missing from the parser: $key"
done
grep -q 'configFileAvailable' "$PARSER" || fail "parser lost the fail-closed configuration probe gate"
grep -q 'configFileState' "$PARSER" || fail "parser lost the configuration state helper"
pass "13-key evidence contract + 6 legacy feature probes + 8 Etch config keys + 16 diagnostic keys"

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
OVERLAY="$ROOT_DIR/legacy/overlay.sh"
[[ -f "$OVERLAY" ]] || fail "legacy overlay not found"
ACTION_BLOCK="$(sed -n '/^const ToolSpec toolSpecs\[\] = {/,/^};/p' "$WINDOW")"
[[ -n "$ACTION_BLOCK" ]] || fail "toolSpecs block not found"
for stage in validate fs-inspect fix-broken dpkg-configure apt-update apt-upgrade initramfs grub display-manager; do
    grep -q "\"$stage\"" <<<"$ACTION_BLOCK" || fail "legacy stage missing from the GUI: $stage"
done
# `unlock` is a dedicated control (passphrase on stdin), never a repair stage;
# the other modern-only stages stay forbidden in the tool table. The
# display-only tools carry an empty stage and can never run.
for forbidden in snapshots host-default host-shell fs-repair copy unlock; do
    grep -q "\"$forbidden\"" <<<"$ACTION_BLOCK" \
        && fail "unsupported stage exposed by the GUI tool table: $forbidden"
done
for key in dkms efi extlinux; do
    grep -qF "{ \"$key\", \"\"," <<<"$ACTION_BLOCK" \
        || fail "display-only tool is not stage-less: $key"
done
grep -qF '{ "bootstack", "boot-stack",' <<<"$ACTION_BLOCK" \
    || fail "bootstack tool is not wired to the legacy boot-stack stage"
for marker in 'Full Repair plan: Manual recovery tool' 'Reconcile Boot Stack' \
    'm_hostDefaultButton' 'Make Default' 'host-default' \
    'm_fileCopyDirectionCombo' 'Add Files...' 'Add Folder...' \
    'Copy and Verify' 'Preview Changes' 'fileCopyRun' 'updateFileCopyTab' \
    'startFileCopyCommand' 'deviceTreeItems' 'setRootIsDecorated(true)' \
    'setTreeStepSize' 'appendRepairSummaryBlock' 'repairStageTitles'; do
    grep -q "$marker" "$WINDOW" || fail "B7 marker missing: $marker"
done
# Cycle 8 markers: Full Repair regeneration, apt intent translation and the
# modern file-copy page structure.
grep -q 'maybeAutoRefreshDiagnostics(QString::fromLatin1("Full Repair"))' "$WINDOW" \
    || fail "Full Repair end-of-plan diagnostics regeneration missing"
grep -q 'Automatic read-only diagnostics regeneration scheduled after %1.' "$WINDOW" \
    || fail "auto-refresh scheduled log line missing"
grep -q 'm_pendingBrowse' "$WINDOW" || fail "browse-target pending flag missing"
grep -q 'handleFileCopyBrowseResult' "$WINDOW" || fail "browse-result handler missing"
for marker in 'm_fileCopyScopeLabel' 'm_fileCopyBrowseButton' \
    'Browse Target Folders...' 'm_fileCopyOwnershipCombo' \
    'Smart destination ownership (recommended)' \
    'Preserve source numeric UID/GID' \
    '1. Select source files or folders from this host' \
    '1. Select source files or folders from the repaired system' \
    '2. Choose destination in repaired system' \
    '3. Ownership and copy policy' 'fileCopyBrowse()' \
    'setTextFormat(Qt::PlainText)'; do
    grep -q "$marker" "$WINDOW" || fail "cycle-8 file-copy marker missing: $marker"
done
# Layout-ownership regression guard: a sub-layout constructed WITH a parent
# layout is added automatically at construction; an extra addLayout() parents
# it twice ("QLayout::addChildLayout: layout already has a parent") and
# double-frees it at shutdown. The direction row must be constructed
# parentless and owned by addLayout(), like the other tabs.
grep -q 'QHBoxLayout \*directionRow = new QHBoxLayout();' "$WINDOW" \
    || fail "file-copy direction row must be constructed parentless (addLayout owns it)"
grep -q 'layout->addLayout(directionRow);' "$WINDOW" \
    || fail "file-copy direction row must be owned by addLayout()"
grep -q 'QVBoxLayout \*hostText = new QVBoxLayout();' "$WINDOW" \
    || fail "host-card text column must be constructed parentless (addLayout owns it)"
grep -q 'hostCardLayout->addLayout(hostText, 1);' "$WINDOW" \
    || fail "host-card text column must be owned by addLayout()"
# Cycle 9 markers: protected-host card, Details dialog, collapsed tree,
# right-edge badge, Run/Re-run text, two-line scope label and the per-user
# settings note.
for marker in 'm_hostCard' 'm_hostSystemLabel' 'm_hostStorageLabel' \
    'm_hostProtectedBadge' 'm_hostDetailsButton' 'showHostDetails()' \
    'updateHostCard()' 'driveDetailsRows' 'Critical mounts:' \
    '"PROTECTED"' '"Details"' 'Re-run Diagnostic' 'Host maintenance:\\n' \
    'headerLayout->addSpacing(8)' 'm_rawView->setMinimumWidth(200)' \
    '~/.qt/' 'setResizeMode(checks, QSplitter::KeepSize)'; do
    grep -q "$marker" "$WINDOW" || fail "cycle-9 marker missing: $marker"
done
# The unlock passphrase dialog must reuse the width-constrained wrapped
# prompt (the same dialog the administrator modal uses).
grep -q 'promptHiddenPassword' "$WINDOW" || fail "hidden-input prompt missing"
grep -q 'kHiddenInputMaximumWidth' "$WINDOW" || fail "hidden-input width constraint missing"
grep -q 'Unlock LUKS repair target' "$WINDOW" || fail "unlock prompt title missing"
# Cycle 9 loop 2: the unlock passphrase travels through a mode-600 keyfile
# argument (Qt 3.3.7 QProcess cannot deliver stdin), and --smoke-test runs
# with settings isolation.
for marker in 'writeUnlockKeyfile' 'discardUnlockKeyfile' \
    '"--key-file"' 'm_unlockKeyfilePath' 'O_CREAT | O_EXCL' \
    'legacySmokeSettingsIsolation' 'applyLegacySettingsDefaults'; do
    grep -q "$marker" "$WINDOW" || fail "cycle-9 loop-2 marker missing: $marker"
done
# Cycle 10 markers: --key-owner unlock channel, ASCII repair summary, the
# wrapped confirmation helper and the /host save-log default.
for marker in '"--key-owner"' 'getuid()' 'confirmWrapped' \
    '==== REPAIR ====' '\[OK\] %1 - changed' '\[FAIL\] %1 -' '\[-\] %1 - %2' \
    'writable system share mount exists'; do
    grep -q "$marker" "$WINDOW" || fail "cycle-10 marker missing: $marker"
done
grep -q 'QMessageBox::question' "$WINDOW" \
    && fail "an unwrapped QMessageBox::question confirmation remains"
# Cycle 11 markers: Already Unlocked, the UNLOCKED_ROOT fallback and the
# deferred-only Authorize affordance.
for marker in '"Already Unlocked"' 'unlockRoot' 'unlockRootFstype' \
    'm_unlockedDisk' 'm_unlockedRoot' 'setHidden(!deferred)'; do
    grep -q "$marker" "$WINDOW" || fail "cycle-11 marker missing: $marker"
done
# Cycle 11 fix: the summary builder maps plan-stage keys to the transcript's
# tool keys before matching the change-status lines.
grep -q 'repairToolKeyForStage(toStd(stage))' "$WINDOW" \
    || fail "repair summary does not map plan-stage keys to tool keys"
# Cycle 13 markers: sortable capability table, per-drive Select Target, the
# Details-pane behavior, the modern chroot notices, the help buttons and the
# Application log heading.
for marker in 'setSorting(true)' 'alreadyCommitted' 'm_inspectingHostDetails' \
    'm_detailsPaneTitle' 'makeHelpButton' \
    'Run a command on the running host as root (sudo is not needed)' \
    'Run a command inside the selected repair system as root' \
    'Committed repair target. Repair, Diagnostics and File Copy' \
    'Host Maintenance is the current scope, but the selected' \
    'isLinuxFileSystemName(own.fstype)' \
    'Select an offline repair target to unlock.' \
    'Committed repair target. Repair, Diagnostics and File Copy' \
    'showHelpPopup' 'QToolButton' 'QSignalMapper' 'setFrameShape(QFrame::NoFrame)' \
    'unlockRootUuid(' 'm_unlockedRootUuid' 'm_unlockedMapper' \
    'setHScrollBarMode' 'applicationLog->setMinimumWidth(480)' \
    'sessions->setMinimumWidth(240)' 'not mounted (offline target)' \
    'Repair drive selected: %1' \
    'toolListHeaderClicked' 'header(), SIGNAL(clicked(int)' \
    'm_toolList->clear()' 'm_toolSortColumn' 'toolIndexForTitle' \
    'QFontMetrics(m_headerBadge->font()).width'; do
    grep -q "$marker" "$WINDOW" || fail "cycle-13 marker missing: $marker"
done
grep -q 'QString::fromLatin1("Application log"), page)' "$WINDOW" \
    || fail "Logs page heading is not Application log"
grep -q 'setMinimumHeight(80)' "$WINDOW" \
    || fail "vertical-shrink floors missing"
grep -q 'UNLOCKED_ROOT' "$OVERLAY" \
    || fail "legacy unlock does not emit the UNLOCKED_ROOT probe line"
grep -q 'legacy_blkid_value_path' "$OVERLAY" \
    || fail "legacy unlock fstype probe must use the direct mapper path"
grep -q 'injectUnlockedMapperRows' "$WINDOW" \
    || fail "GUI does not inject the unlocked mapper rows"
grep -q 'setCommunication(QProcess::Stdout | QProcess::DupStderr)' "$GUI_DIR/src/HelperRunner.cpp" \
    || fail "helper runner stdin channel was not reverted (stdin unused by design)"
grep -q 'legacy::legacySetSmokeSettingsIsolation(true)' "$GUI_DIR/src/main.cpp" \
    || fail "main() does not enable the smoke settings isolation"
awk '/legacySetSmokeSettingsIsolation\(true\)/{iso=NR} /LegacyMainWindow window;/{win=NR} END{exit !(iso && iso < win)}' "$GUI_DIR/src/main.cpp" \
    || fail "smoke settings isolation must be set before the window is constructed"
grep -q 'legacy_apt_intent_translate' "$OVERLAY" \
    || fail "apt intent translation missing from the overlay"
grep -q 'apt intent translated:' "$OVERLAY" \
    || fail "apt intent translation log line missing"
grep -q 'browse_target_directory' "$OVERLAY" \
    || fail "legacy browse-target override missing from the overlay"
grep -q 'BROWSE_ENTRY' "$OVERLAY" \
    || fail "legacy browse record format missing from the overlay"
# A9-13: TAB is percent-encoded by the helper and decoded by the picker.
grep -q 's/\\t/%09/g' "$OVERLAY" \
    || fail "legacy browse encoder does not percent-encode TAB"
grep -q 'legacyPercentDecode' "$WINDOW" \
    || fail "browse-result decoder missing"
grep -q '"%09"' "$WINDOW" \
    || fail "browse-result decoder lost the %09 TAB decode"
# Cycle 4b: the display tool is the legacy host-scope SysV stage (runnable
# with the helper's display capability line; offline stays disabled).
grep -qF '{ "display", "display-manager",' <<<"$ACTION_BLOCK" \
    || fail "display tool is not wired to the display-manager stage"
for marker in 'Restore Graphical Login' 'hostOnly' \
    'Host-scope stage - not part of the Full Repair plan' \
    'enter Host Maintenance to run it'; do
    grep -q "$marker" "$WINDOW" || fail "display stage marker missing: $marker"
done
grep -q 'host-validate' "$WINDOW" || fail "GUI lost the host-validate command form"
grep -q 'host-diagnose' "$WINDOW" || fail "GUI lost the host-diagnose command form"
grep -q 'host-repair' "$WINDOW" || fail "GUI lost the host-repair command form"
grep -q 'hostMaintenance' "$WINDOW" || fail "GUI lost the host-maintenance feature gate"
# The Full Repair plan runs exactly the legacy helper stages in rank order.
PLAN_BLOCK="$(sed -n '/^const PlanSpec planSpecs\[\] = {/,/^};/p' "$WINDOW")"
[[ -n "$PLAN_BLOCK" ]] || fail "planSpecs block not found"
for stage in dpkg-configure fix-broken apt-update apt-upgrade initramfs grub; do
    grep -q "\"$stage\"" <<<"$PLAN_BLOCK" || fail "plan stage missing from the GUI: $stage"
done
grep -q 'repair/dpkgConfigure' "$WINDOW" || fail "plan settings key missing: repair/dpkgConfigure"
grep -q 'repair/refreshMetadata' "$WINDOW" || fail "plan settings key missing: repair/refreshMetadata"
grep -q 'repair/upgradePackages' "$WINDOW" || fail "plan settings key missing: repair/upgradePackages"
pass "legacy command set only (validate/diagnose/fs-inspect/repairs/grub) + 6-stage plan"

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
grep -q 'Privilege elevation' "$WINDOW" \
    && fail "the legacy-only Privilege elevation section still exists"
grep -q 'm_settingsElevationLabel' "$WINDOW" \
    || fail "Settings lost the privileged-authorization support label"
# Modern 1:1 Logs filter combo: All entries / Diagnostics / Repairs / the
# three workflows / the 16 diagnostic section titles.
for filter in 'All entries' 'Diagnostics' 'Repairs' 'File system repair' \
    'Package repair' 'File copy'; do
    grep -q "$filter" "$WINDOW" || fail "GUI lost a log filter option: $filter"
done
grep -q 'logFilterSpecs' "$WINDOW" || fail "GUI lost the log filter spec table"
grep -q 'stagesContainPackageStage' "$WINDOW" \
    || fail "GUI lost the package-workflow stage mapping"
grep -q 'LogEntry::Diagnostic' "$WINDOW" \
    || fail "GUI lost the capture-time log entry tagging"
grep -q 'Diagnostic: ' "$WINDOW" \
    || fail "GUI lost the diagnostic section marker handling"
grep -q 'Errors and warnings' "$WINDOW" \
    && fail "removed all/errors filter still present"
# Modern parity: the Diagnostics tab has no capability list and no filter.
for removed in 'm_diagFilterCombo' 'm_capabilityList' 'All tools' \
    'Reason / evidence' 'View target file (read-only)' 'm_configView'; do
    grep -q "$removed" "$WINDOW" && fail "removed Diagnostics element still present: $removed"
done
# Repair parity (cycle 3): the Full Repair plan section replaces the old
# button grid, the unsupported-features list is gone, and the individual tools
# are a list + Selected tool pane with the modern result popup.
for marker in 'm_planParagraph' 'm_planCountLabel' 'm_planReadinessLabel' \
    'm_planStageList' 'm_configurePlanButton' 'm_runFullRepairButton' \
    'Configure Plan...' 'Run Full Repair' 'configurePlan' 'runFullRepair' \
    'updatePlanView' 'planRunReady' 'selectedPlanStages' \
    'm_toolList' 'm_toolTitle' 'm_toolDescription' 'm_toolPlanStatus' \
    'm_toolRunButton' 'runSelectedTool' 'updateToolDetails' 'toolRunReady' \
    'showRepairResultDialog' 'm_resultStatus' 'm_resultView' \
    'm_resultCloseButton' 'm_repairContent' 'QScrollView' \
    'setWordWrap(m_logWrapEnabled' \
    'Choose Full Repair stages in Settings' \
    'Always preflight' 'Manual recovery tool' \
    'Read-only check - not part of the Full Repair plan' \
    'Enabled in Settings' 'Disabled in Settings - enable it to include this stage' \
    'Privileged operation completed successfully'; do
    grep -q "$marker" "$WINDOW" || fail "Repair parity marker missing: $marker"
done
for removed in 'Modern features not available' 'm_unsupportedList' \
    'unsupportedSpecs' 'm_actionButtons' 'actionSpecs'; do
    grep -q "$removed" "$WINDOW" && fail "removed Repair element still present: $removed"
done
# Systems parity: page heading, Available repair targets list, Select Target /
# Unlock / Host Maintenance / Authorize action row and the auto-resolved root.
for marker in 'm_systemsHeading' 'm_targetsHeading' 'Available repair targets' \
    'm_setTargetButton' 'Select Target' 'Refresh Devices' 'm_scanButton' \
    'm_deviceList' 'm_unlockButton' 'm_hostMaintenanceButton' \
    'm_authorizeButton' 'm_authStatusLabel' 'authorizeNow' \
    'updateAuthorizationAffordance' 'autoResolvedRoot' 'autoResolvedLuks' \
    'isLinuxFileSystemName' 'm_targetSummary' \
    'Committed target: none'; do
    grep -q "$marker" "$WINDOW" || fail "Systems parity marker missing: $marker"
done
grep -q 'm_scopeLabel' "$WINDOW" || fail "GUI lost the Diagnostics scope label"
grep -q 'updateScopeLabel' "$WINDOW" || fail "GUI lost the shared scope label updater"
# The removed Systems combos/grid must stay gone. (The "Physical drive:" /
# "Detected component:" strings remain only as helper fact keys parsed from
# diagnostics, never as visible form labels.)
for removed in 'm_diskCombo' 'm_rootCombo' 'm_unlockCombo' \
    'Root component:' 'LUKS component:' 'Selected scope and target'; do
    grep -q "$removed" "$WINDOW" && fail "removed Systems element still present: $removed"
done
# Global header (modern parity): icon, title, subtitle, version badge and the
# reserved busy-indicator slot.
for marker in 'm_headerTitle' 'Boot Bitch' 'Linux recovery and boot-repair utility' \
    'm_headerBadge' 'GUARDED REPAIR' 'LEGACY_VERSION' 'legacyHeaderPixmap' \
    'setPointSizeFloat' 'm_busyLabel' 'Working...' 'updateBusyIndicator'; do
    grep -q "$marker" "$WINDOW" || fail "global header marker missing: $marker"
done
# Diagnostics parity: Selected diagnostic pane, Run All / Run Diagnostic,
# Results + Copy/Save Results, no Cancel button.
for marker in 'm_diagnosticList' 'm_runDiagnosticButton' 'Run Diagnostic' \
    'runSelectedDiagnostic' 'diagnosticKeys' 'Copy Results' 'm_copyResultsButton' \
    'Save Results...' 'm_saveResultsButton' 'saveResults' 'Selected diagnostic' \
    'm_diagTitle' 'm_diagDescription' 'm_diagAvailability' 'updateDiagnosticDetails' \
    'm_diagnosticsButton' 'Run All' 'm_scopeLabel'; do
    grep -q "$marker" "$WINDOW" || fail "per-diagnostic control missing: $marker"
done
for removed in 'Run selected diagnostic' 'Run All diagnostics (read-only)' \
    'Cancel running command' 'm_cancelButton' '"Reads"'; do
    grep -q "$removed" "$WINDOW" && fail "removed Diagnostics element still present: $removed"
done
# The runner keeps its cancel capability for window close, without a dead UI.
grep -q 'closeEvent' "$WINDOW" || fail "GUI lost the window-close helper cancel path"
grep -q 'm_runner->cancel()' "$WINDOW" || fail "GUI lost the internal helper cancel call"
for marker in 'm_configCombo' 'm_configButton' 'Edit Target File...' \
    'editTargetConfigFile' 'openConfigEditor' 'config-read' 'config-write' \
    'Save Target File' 'm_configReasonLabel' 'configSpecs' \
    'configKeyForPath'; do
    grep -q "$marker" "$WINDOW" || fail "Edit Target File marker missing: $marker"
done
for key in fstab inittab menu-lst crypttab modules interfaces sources-list apt-conf; do
    grep -q "\"$key\"" "$WINDOW" || fail "Etch configuration key missing from the GUI: $key"
done
# The target configuration row follows the modern visibility rule exactly:
# committed repair target only, never Host Maintenance.
grep -q 'targetCommitted() && !hostMaintenanceActive()' "$WINDOW" \
    || fail "target configuration row lost the committed-target-only gate"
grep -q 'grub' <<<"$ACTION_BLOCK" || fail "guarded GRUB action missing"
# Probe-based legacy feature gating (fail closed) replaces hardcoded reasons.
for marker in 'legacyFeatureAvailable' 'legacyFeatureState' 'legacyFeatureDisplay' \
    'updateLegacyFeatureView' 'updateFeatureTab'; do
    grep -q "$marker" "$WINDOW" || fail "legacy feature gating marker missing: $marker"
done
grep -q "the legacy helper exposes no chroot shell command" "$WINDOW" \
    && fail "hardcoded chroot-shell reason still present"
grep -q "not supported by the legacy helper on Etch" "$WINDOW" \
    && fail "hardcoded host-default reason still present"
pass "parity controls (Systems header/actions, Diagnostics pane, unlock, elevation, per-diagnostic runs, config viewer, probe gating)"

# --- no independent scope selector; target eligibility; Authorize affordance -
grep -q 'm_scopeCombo' "$WINDOW" && fail "Systems still carries the removed scope dropdown"
grep -q '"Scope:"' "$WINDOW" && fail "Systems still carries a Scope label"
grep -q 'scopeChanged' "$WINDOW" && fail "GUI still wires the removed scope selector"
grep -q 'Authenticate' "$WINDOW" && fail "GUI still shows a standalone Authenticate control"
# Modern parity: the deferred-authorization affordance IS an Authorize control.
grep -q 'm_authorizeButton' "$WINDOW" || fail "GUI lost the deferred Authorize control"
grep -q 'm_repairAuthorizeButton' "$WINDOW" \
    && fail "the Repair-tab Authorize control still exists (Systems only now)"
grep -q 'scopeFeatureKey' "$WINDOW" || fail "GUI lost the scope feature mapping (shell vs host-shell)"
# The protected running-host disk stays excluded from the commit eligibility.
grep -q 'selectedDisk() == runningHostDisk()' "$WINDOW" \
    || fail "GUI lost the protected running-host commit exclusion"
pass "no scope selector / standalone auth control; deferred Authorize affordance; target eligibility is selection-based"

# --- authorization is requested on commit/maintenance, never on Run All -----
grep -q 'ensureAdministratorSession' "$WINDOW" || fail "GUI lost the session authorization entry point"
grep -q 'administratorSessionActive' "$WINDOW" || fail "GUI lost the cached session check"
grep -q 'sessionStillCurrent' "$WINDOW" || fail "GUI lost the pre-command session expiry check"
START_BLOCK="$(sed -n '/^bool LegacyMainWindow::startCommand/,/^}/p' "$WINDOW")"
[[ -n "$START_BLOCK" ]] || fail "startCommand block not found"
grep -q 'QInputDialog' <<<"$START_BLOCK" \
    && fail "Run All still opens the password prompt (must fail closed)"
grep -q 'administratorSessionActive' <<<"$START_BLOCK" \
    || fail "startCommand does not fail closed without an authorized session"
grep -q 'sessionStillCurrent' <<<"$START_BLOCK" \
    || fail "startCommand does not re-check the cached session before running"
grep -q 'transcriptSuggestsAuthFailure' "$WINDOW" \
    || fail "GUI lost the narrowed auth-failure detection"
COMMIT_BLOCK="$(sed -n '/^void LegacyMainWindow::setRepairTarget/,/^}/p' "$WINDOW")"
[[ -n "$COMMIT_BLOCK" ]] || fail "setRepairTarget block not found"
grep -q 'ensureAdministratorSession' <<<"$COMMIT_BLOCK" \
    || fail "committing a repair target does not request authorization"
MAINT_BLOCK="$(sed -n '/^void LegacyMainWindow::toggleHostMaintenance/,/^}/p' "$WINDOW")"
[[ -n "$MAINT_BLOCK" ]] || fail "toggleHostMaintenance block not found"
grep -q 'ensureAdministratorSession' <<<"$MAINT_BLOCK" \
    || fail "entering Host Maintenance does not request authorization"
grep -q 'SIGPIPE' "$GUI_DIR/src/HelperRunner.cpp" \
    || fail "HelperRunner does not guard the password pipe write against SIGPIPE"
grep -q 'ScopedSigPipeIgnore' "$GUI_DIR/src/HelperRunner.cpp" \
    || fail "HelperRunner lost the scoped SIGPIPE guard"
grep -q 'sessionIsCurrent' "$GUI_DIR/src/HelperRunner.cpp" \
    || fail "HelperRunner lost the non-blocking session-validity probe"
grep -q 'probeCommandWithStdinNull' "$GUI_DIR/src/HelperRunner.cpp" \
    || fail "HelperRunner lost the stdin-closed sudo timestamp probe"
pass "authorization on commit/Host Maintenance, cached for the session, Authorize re-auth, SIGPIPE-safe"

# --- Chroot Shell follows the scope probe + authorization (greyed otherwise) -
for marker in 'm_shellCommandEdit' 'm_shellOutput' 'm_shellRunButton' \
    'm_shellClearButton' 'runChrootShell' 'clearChrootOutput' \
    'updateChrootShellState' 'updateChrootShellMode' 'host-shell' \
    'Run Command' 'Run on Host' 'Host Shell' 'setTabLabel'; do
    grep -q "$marker" "$WINDOW" || fail "chroot shell wiring marker missing: $marker"
done
grep -q '"shell"' "$WINDOW" || fail "chroot shell lost the target shell verb"
grep -q 'unavailable: see the helper.s Legacy feature shell: probe reason above' "$WINDOW" \
    && fail "chroot shell still hardcodes the greyed placeholder"
pass "chroot shell wired to shell/host-shell with probe + session gating and host-mode labels"

# --- tab parity, Systems panel, unlock status and Logs session/search --------
# Modern parity: seven tabs (the About tab is gone; Help -> About Boot Bitch
# opens the dialog).
for tab in 'Systems' 'Diagnostics' 'Repair' 'Chroot Shell' 'File Copy' 'Logs' 'Settings'; do
    grep -q "\"$tab\"" "$WINDOW" || fail "GUI lost a modern-parity tab: $tab"
done
grep -q 'buildAboutTab' "$WINDOW" && fail "About tab still present"
grep -q 'expected 7 tabs' "$WINDOW" || fail "smoke tab-count assertion still expects 8 tabs"
for marker in 'Developer:' 'CaptainMorgan12' 'Qt 3.3.x frontend' \
    'Lock Administrator Session'; do
    grep -q "$marker" "$WINDOW" || fail "About dialog marker missing: $marker"
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
for marker in 'm_targetCommitted' 'm_hostMaintenance' 'diagnosticsScopeReady' 'scopeReadyReason' 'Select Target' 'Host Maintenance'; do
    grep -q "$marker" "$WINDOW" || fail "modern diagnostics gating marker missing: $marker"
done
pass "tab parity (details/unlock/session/search/settings/gating)"

# --- menus, Logs management, Settings filters/capabilities (cycle 2) --------
for marker in 'm_fileMenu' 'm_viewMenu' 'm_helpMenu' 'm_wrapLogsMenuId' \
    'm_wrapLogsMenuValid' 'Refresh Devices' 'Lock Administrator Session' \
    'Auto-size Device Columns' 'Wrap Log Lines' 'Using Boot Bitch' \
    'About Boot Bitch' 'lockAdministratorSession' 'autoSizeDeviceColumns' \
    'toggleLogWrapFromMenu' 'showUsageHelp' 'clearSudoTimestamp' 'menuItemId' \
    'label.left(tab)'; do
    grep -q "$marker" "$WINDOW" || fail "menu parity marker missing: $marker"
done
# Qt3 QPopupMenu auto-ids are negative and QListView sorts by the first column
# by default; both must be handled explicitly (id sign is not validity, and
# insertion-order lists disable the default sort).
grep -q 'm_wrapLogsMenuId >= 0\|m_wrapLogsMenuId < 0' "$WINDOW" \
    && fail "menu validity still keys on the Qt3 negative auto-id sign"
for list in m_diagnosticList m_detailList m_planStageList m_toolList; do
    grep -q "$list->setSorting(-1)" "$WINDOW" \
        || fail "$list does not disable Qt3's default first-column sorting"
done
# With sorting disabled Qt3 prepends plain insertions; the insertion-order
# lists must chain their items with the after-form constructor.
for marker in 'm_diagnosticList, lastDiagnostic' \
    'm_detailList, lastDetail' \
    'm_toolList, lastTool'; do
    grep -qF "$marker" "$WINDOW" \
        || fail "list does not keep insertion order with the after-form constructor: $marker"
done
for marker in 'Save As...' 'Clear Register' 'New Session Log' 'Add Note' \
    'm_newSessionLogButton' 'm_addNoteButton' 'm_deleteSessionLogButton' \
    'startNewSessionLog' 'addSessionNote' 'deleteSelectedSessionLog' \
    'm_priorLogBanner' 'Viewing a prior session log'; do
    grep -q "$marker" "$WINDOW" || fail "Logs parity marker missing: $marker"
done
grep -q 'Save log\.\.\.' "$WINDOW" && fail "Logs still shows the removed Save log... button"
grep -q '"Clear log"' "$WINDOW" && fail "Logs still shows the removed Clear log button"
for marker in 'm_showNonLinuxCheck' 'm_showRemovableCheck' 'm_showEncryptedCheck' \
    'm_autoRefreshCheck' 'deviceFilterChanged' 'autoRefreshToggled' \
    'Host capabilities and dependencies' 'm_capDistributionLabel' \
    'm_refreshCapabilitiesButton' 'Refresh Capabilities' 'refreshCapabilities' \
    'm_capabilityTable' 'm_installSupportButton' 'Install Missing Support...' \
    'Privileged authorization support' 'no KAuth' 'capabilitySpecs' \
    'Process namespace isolation' '"unshare"' \
    'Automatic installation will require explicit package mapping' \
    'QSettings' 'devices/showNonLinux' 'devices/showRemovable' \
    'devices/showEncrypted' 'logs/wrapLines' 'diagnostics/autoRefreshStale' \
    'maybeAutoRefreshDiagnostics' 'runScheduledAutoRefresh' \
    'entering Host Maintenance changed the diagnostics scope' \
    'committing the repair target changed the diagnostics scope' \
    'maybeAutoRefreshDiagnostics(finishedLabel)' \
    'm_settingsContent' 'QScrollView' \
    'm_repairVerticalSplitter' 'Qt::Vertical' 'kSectionTitleLeftTolerance'; do
    grep -q "$marker" "$WINDOW" || fail "Settings parity marker missing: $marker"
done
grep -q 'updateCapabilityView' "$WINDOW" \
    && fail "compact capability label grid still present"
# Cycle 6/9: every toggle handler persists immediately and the window close
# flushes again. Qt 3.3.7 stores each settings group in its own per-user file
# under ~/.qt/ named after the group (devicesrc, logsrc, diagnosticsrc,
# repairrc); the GUI must document those actual file names.
grep -q 'setPath(QString::fromLatin1("boot-bitch"),' "$WINDOW" \
    || fail "settings do not use the canonical boot-bitch path"
grep -q '"boot-repair"), QSettings::User' "$WINDOW" \
    || fail "settings do not use the boot-repair application"
for file in devicesrc logsrc diagnosticsrc repairrc; do
    grep -q "$file" "$WINDOW" \
        || fail "settings do not document the per-group $file location"
done
grep -q '~/.qt/boot-bitchrc' "$WINDOW" \
    && fail "settings still claim a single boot-bitchrc file"
grep -q 'boot-bitch.local"),' "$WINDOW" \
    && fail "scattered per-subkey settings path still present"
for handler in deviceFilterChanged autoRefreshToggled toggleLogWrap planCheckboxChanged; do
    sed -n "/^void LegacyMainWindow::$handler/,/^}/p" "$WINDOW" \
        | grep -q 'saveLegacySettings' \
        || fail "$handler does not persist its toggle"
done
sed -n '/^void LegacyMainWindow::closeEvent/,/^}/p' "$WINDOW" \
    | grep -q 'saveLegacySettings' \
    || fail "closeEvent does not flush the settings"
grep -q 'QTable::SingleRow' "$WINDOW" \
    || fail "capability table lost the full-row selection mode"
grep -q 'selectRow(0)' "$WINDOW" \
    || fail "capability table does not select its first row after refresh"
grep -q 'diskFilesystemSummary' "$WINDOW" \
    || fail "GUI lost the disk filesystem aggregation"
grep -q 'return QString::fromLatin1("unknown");' "$WINDOW" \
    || fail "Filesystem column can still render empty"
for marker in 'Model / label:' 'Mounts:' 'UUID:'; do
    grep -q "$marker" "$WINDOW" || fail "details panel marker missing: $marker"
done
pass "canonical settings path, per-toggle persistence, row selection, ten details rows, non-empty Filesystem labels"
grep -q 'm_diagKeyByTitle' "$WINDOW" || fail "diagnostic list lost the title->key mapping"
grep -q 'diagnosticTitle' "$WINDOW" || fail "diagnostic list lost the friendly titles"
pass "menus, Logs session management, Settings filters/capabilities and diagnostic titles"

# --- modal elevation prompt, no inline sudo password echo -------------------
grep -q 'elevationNeedsPassword' "$WINDOW" || fail "GUI does not pre-check an interactive elevation"
grep -q 'authenticateElevation' "$WINDOW" || fail "GUI does not authenticate interactive elevation"
grep -q 'Administrator authorization' "$WINDOW" || fail "GUI lost the modal authorization dialog"
# The hidden-input modal must be width-constrained and word-wrapped (the
# reported defect was the unwrapped long text sizing the dialog too wide).
# Qt3 has no QLabel::setWordWrap, so the wrapping idiom is `Qt::WordBreak`
# alignment plus an explicit maximum width.
grep -q 'promptHiddenPassword' "$WINDOW" || fail "GUI lost the width-constrained hidden-input prompt"
grep -q 'enableLabelWordWrap' "$WINDOW" || fail "hidden-input modal text is not word-wrapped"
grep -q 'Qt::WordBreak' "$WINDOW" || fail "hidden-input modal lost the Qt3 wrap alignment"
grep -q 'setMaximumWidth(kHiddenInputMaximumWidth)' "$WINDOW" \
    || fail "hidden-input modal is not width-constrained"
# The Add Note dialog is the only QInputDialog use; the administrator
# authorization prompt must stay the width-constrained hidden-input dialog.
grep -q 'QInputDialog::getText' "$WINDOW" || fail "GUI lost the Add Note input dialog"
AUTH_BLOCK="$(sed -n '/^bool LegacyMainWindow::ensureAdministratorSession/,/^}/p' "$WINDOW")"
[[ -n "$AUTH_BLOCK" ]] || fail "ensureAdministratorSession block not found"
grep -q 'QInputDialog' <<<"$AUTH_BLOCK" \
    && fail "administrator authorization still uses the self-sizing QInputDialog"
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
# Read-only udev metadata: the LUKS/filesystem probe behind Select Target and
# Unlock (never a block-device read).
grep -q 'parseUdevDatabase' "$GUI_DIR/src/DeviceInventory.cpp" \
    || fail "DeviceInventory lost the udev metadata parser"
grep -q 'udevFsTypeFor' "$GUI_DIR/src/DeviceInventory.cpp" \
    || fail "DeviceInventory lost the udev filesystem probe"
grep -q 'probedFstype' "$GUI_DIR/src/DeviceInventory.h" \
    || fail "DeviceRow lost the probed filesystem field"
grep -q 'encrypted' "$GUI_DIR/src/DeviceInventory.h" \
    || fail "DeviceRow lost the LUKS detection field"
grep -q 'isLinuxFileSystemName' "$GUI_DIR/src/DeviceInventory.cpp" \
    || fail "DeviceInventory lost the Linux filesystem classifier"
grep -q 'looksLikeLuks' "$GUI_DIR/src/DeviceInventory.cpp" \
    || fail "DeviceInventory lost the LUKS classifier"
pass "read-only UUID/label/transport + udev LUKS/filesystem inventory"

# --- layout guards and the extended smoke test ------------------------------
grep -q 'QFontMetrics' "$WINDOW" || fail "GUI lost the title/button font-metric layout guard"
grep -q 'setMinimumWidth' "$WINDOW" || fail "GUI lost the minimum-width layout guard"
grep -q 'setMinimumHeight' "$WINDOW" || fail "GUI lost the minimum-height layout guard"
grep -q 'kGroupTitlePadding' "$WINDOW" || fail "GUI lost the group-title padding floor"
grep -q 'kListHeaderPadding' "$WINDOW" || fail "GUI lost the list-header font-metric floor"
grep -q 'makeSectionTitle' "$WINDOW" || fail "GUI lost the sectionTitle-style heading helper"
grep -q 'm_sectionTitles' "$WINDOW" || fail "GUI lost the section-title layout assertions"
grep -q 'addListViewColumn' "$WINDOW" || fail "GUI lost the header-safe list column helper"
grep -q 'QListView::LastColumn' "$WINDOW" \
    || fail "list last columns no longer stretch to the right edge"
grep -q 'listLastColumnFills' "$WINDOW" || fail "GUI lost the last-column gutter check"
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

# --- legacy icons generated from the modern master ---------------------------
ICON_SCRIPT="$GUI_DIR/data/make-icons.py"
[[ -f "$ICON_SCRIPT" ]] || fail "missing legacy icon script"
grep -q 'org.bootrepair.BootRepair.png' "$ICON_SCRIPT" \
    || fail "icon script no longer loads the modern master artwork"
grep -q 'LANCZOS' "$ICON_SCRIPT" \
    || fail "icon script no longer resizes with LANCZOS"
grep -q 'sys.exit' "$ICON_SCRIPT" \
    || fail "icon script does not fail when the master is missing"
for size in 16 22 32 48; do
    icon="$GUI_DIR/data/boot-repair-legacy-${size}x${size}.png"
    [[ -f "$icon" ]] || fail "missing generated legacy icon: $icon"
done
MASTER="$ROOT_DIR/data/icons/hicolor/1024x1024/apps/org.bootrepair.BootRepair.png"
[[ -f "$MASTER" ]] || fail "modern master icon missing"
grep -q 'setIcon(headerPixmap)' "$WINDOW" \
    || fail "GUI window does not use the packaged icon artwork"
pass "legacy icons generated from the modern master (LANCZOS, fail-fast script)"

# --- B3 hardening: secrets/logs/bounded waits (A10-01/06/07/08/09) ----------
grep -q '<config-write content redacted>' "$WINDOW" \
    || fail "startCommand does not redact the config-write payload element"
grep -q '<keyfile path redacted>' "$WINDOW" \
    || fail "startCommand does not redact the unlock keyfile path argument"
grep -q 'O_WRONLY | O_CREAT | O_APPEND | O_NOFOLLOW' "$WINDOW" \
    || fail "appendToLogFile does not open append-only with O_NOFOLLOW"
grep -q 'fchmod(fd, 0600)' "$WINDOW" \
    || fail "appendToLogFile does not re-tighten the log file to 0600"
grep -q 'fdopen(fd' "$WINDOW" \
    || fail "appendToLogFile no longer writes through fdopen(FILE*)"
grep -q 'isDefaultLogTree' "$WINDOW" \
    || fail "the private-log-tree check is missing"
grep -q 'ensureLogDirectory' "$WINDOW" \
    || fail "the private log-directory helper is missing"
grep -q 'ensureDirectory(m_logDirectory, 0700)' "$WINDOW" \
    || fail "the default log directory is not created 0700"
grep -q 'chmod(m_logDirectory.local8Bit().data(), 0700)' "$WINDOW" \
    || fail "the default log directory leaf is not re-tightened to 0700"
grep -q 'm_logDirectory + QString::fromLatin1("/.keys")' "$WINDOW" \
    || fail "unlock keyfiles are not moved to a dedicated .keys directory"
grep -q 'mkdir(keysDirectory.local8Bit().data(), 0700)' "$WINDOW" \
    || fail "the .keys directory is not created 0700"
grep -q 'chmod(keysDirectory.local8Bit().data(), 0700)' "$WINDOW" \
    || fail "the .keys directory is not re-tightened to 0700"
grep -q 'legacyKeyfileTerminationHandler' "$WINDOW" \
    || fail "the keyfile termination handlers are missing"
grep -q 'SIGTERM' "$WINDOW" || fail "the SIGTERM keyfile handler is missing"
grep -q 'SIGINT' "$WINDOW" || fail "the SIGINT keyfile handler is missing"
grep -q 'SIGHUP' "$WINDOW" || fail "the SIGHUP keyfile handler is missing"
grep -q '_exit(128 + signalNumber)' "$WINDOW" \
    || fail "the keyfile handler does not exit with 128+signum"
grep -q 'strncpy(registeredPath' "$WINDOW" \
    || fail "the created secret file is not registered with the termination handlers"
grep -q 'lstat(canonicalPath.local8Bit().data(), &earlier)' "$WINDOW" \
    || fail "deleteSelectedSessionLog lost the pre-confirmation lstat"
grep -q 'lstat(canonicalPath.local8Bit().data(), &current)' "$WINDOW" \
    || fail "deleteSelectedSessionLog lost the immediate pre-unlink lstat"
grep -q 'current.st_dev != earlier.st_dev' "$WINDOW" \
    || fail "deleteSelectedSessionLog does not compare the device identity"
grep -q 'current.st_ino != earlier.st_ino' "$WINDOW" \
    || fail "deleteSelectedSessionLog does not compare the inode identity"
grep -q '::unlink(canonicalPath.local8Bit().data()) != 0' "$WINDOW" \
    || fail "deleteSelectedSessionLog no longer unlinks after the lstat"
grep -q 'static const char \*const settingsFiles\[\]' "$WINDOW" \
    || fail "the post-save settings permission pass is missing"
for file in devicesrc logsrc diagnosticsrc repairrc; do
    grep -q "\"$file\"" "$WINDOW" \
        || fail "settings chmod pass misses ~/.qt/$file"
done
grep -q 'chmod(settingsPath.local8Bit().data(), 0600)' "$WINDOW" \
    || fail "settings files are not chmod'ed 0600 after saves"
# B5: the config-write content transport.  The edited file travels through
# --content-file/--content-owner (never argv); the mode-600 O_EXCL writer is
# the generalized .keys writer reused by the unlock keyfile path; the content
# file is unlinked on every completion path and by the termination handlers;
# the editor cap is the helper's 1 MiB file bound.
for marker in '"--content-file"' '"--content-owner"' 'writeSecretFile' \
    'writeConfigContentFile' 'm_configContentFilePath' \
    'discardConfigContentFile' 'gConfigContentFilePath' \
    'config-content-' 'kConfigEditMaximumBytes = 1048576' \
    'NUL bytes; the guarded write' 'larger than 1 MiB' \
    '<config-write content path redacted>'; do
    grep -q "$marker" "$WINDOW" || fail "B5 config-write marker missing: $marker"
done
UNLOCK_WRITER_BLOCK="$(sed -n '/^bool LegacyMainWindow::writeUnlockKeyfile/,/^}/p' "$WINDOW")"
[[ -n "$UNLOCK_WRITER_BLOCK" ]] || fail "writeUnlockKeyfile block not found"
grep -q 'writeSecretFile(secret' <<<"$UNLOCK_WRITER_BLOCK" \
    || fail "the unlock keyfile writer does not reuse the shared writeSecretFile writer"
SECRET_BLOCK="$(sed -n '/^bool LegacyMainWindow::writeSecretFile/,/^}/p' "$WINDOW")"
[[ -n "$SECRET_BLOCK" ]] || fail "writeSecretFile block not found"
grep -q 'O_WRONLY | O_CREAT | O_EXCL, 0600' <<<"$SECRET_BLOCK" \
    || fail "writeSecretFile does not O_EXCL-create at mode 0600"
grep -q 'registeredPath\[registeredPathSize - 1\] = .\\0.;' <<<"$SECRET_BLOCK" \
    || fail "writeSecretFile does not NUL-terminate the registered path"
HANDLER_BLOCK="$(sed -n '/^extern "C" void legacyKeyfileTerminationHandler/,/^}/p' "$WINDOW")"
[[ -n "$HANDLER_BLOCK" ]] || fail "termination handler block not found"
grep -q 'gUnlockKeyfilePath' <<<"$HANDLER_BLOCK" \
    || fail "termination handler no longer unlinks the pending unlock keyfile"
grep -q 'gConfigContentFilePath' <<<"$HANDLER_BLOCK" \
    || fail "termination handler does not unlink the pending config content file"
FINISH_BLOCK="$(sed -n '/^void LegacyMainWindow::helperFinished/,/^}/p' "$WINDOW")"
[[ -n "$FINISH_BLOCK" ]] || fail "helperFinished block not found"
grep -q 'discardConfigContentFile()' <<<"$FINISH_BLOCK" \
    || fail "helperFinished does not unlink the config content file on completion"
START_BLOCK="$(sed -n '/^bool LegacyMainWindow::startCommand/,/^}/p' "$WINDOW")"
grep -q 'discardConfigContentFile()' <<<"$START_BLOCK" \
    || fail "startCommand refusal paths do not discard the config content file"
EDITOR_BLOCK="$(sed -n '/^void LegacyMainWindow::openConfigEditor/,/^}/p' "$WINDOW")"
[[ -n "$EDITOR_BLOCK" ]] || fail "openConfigEditor block not found"
grep -q 'QString::fromLatin1("--content-file") << contentFilePath' <<<"$EDITOR_BLOCK" \
    || fail "openConfigEditor does not pass --content-file with the content path"
grep -q '"--content-owner"' <<<"$EDITOR_BLOCK" \
    || fail "openConfigEditor does not pass --content-owner"
grep -q '::getuid()' <<<"$EDITOR_BLOCK" \
    || fail "openConfigEditor does not pass the invoker uid as --content-owner"
pass "B5 config-write transport (--content-file args, .keys writer reuse, unlink on completion + termination, 1 MiB cap)"
RUNNER_CPP="$GUI_DIR/src/HelperRunner.cpp"
grep -q 'boundedWaitPid' "$RUNNER_CPP" \
    || fail "HelperRunner lost the bounded waitpid helper"
grep -q 'waitpid(pid, status, WNOHANG)' "$RUNNER_CPP" \
    || fail "HelperRunner waitpids are not WNOHANG polls"
grep -q 'kill(pid, SIGKILL)' "$RUNNER_CPP" \
    || fail "HelperRunner does not SIGKILL a wedged child"
grep -q 'time(NULL) + 10' "$RUNNER_CPP" \
    || fail "HelperRunner lost the 10-second wait deadline"
grep -q 'poll(&pollFd' "$RUNNER_CPP" \
    || fail "authenticateElevation stderr read is not poll-bounded"
grep -q 'capturedLimit = 32 \* 1024' "$RUNNER_CPP" \
    || fail "authenticateElevation stderr capture is not capped at 32 KiB"
grep -q 'did not respond within the timeout' "$RUNNER_CPP" \
    || fail "authenticateElevation lost the bounded-wait failure wording"
pass "B3 hardening (redacted argv, O_NOFOLLOW/0600 log, 0700 log dir, .keys + signal handlers, lstat-before-unlink, rc 0600, bounded waits)"

# --- B4 elevation trust: fixed paths, lstat checks, no bare execvp ----------
RUNNER_CPP="$GUI_DIR/src/HelperRunner.cpp"
for path in '/usr/bin/sudo' '/usr/local/bin/sudo' '/bin/sudo' \
    '/usr/bin/gksu' '/usr/bin/gksudo'; do
    grep -qF "\"$path\"" "$RUNNER_CPP" \
        || fail "fixed trusted elevation path missing from HelperRunner: $path"
done
grep -q 'lstat(' "$RUNNER_CPP" \
    || fail "HelperRunner does not lstat the elevation candidates"
grep -q 'S_ISREG' "$RUNNER_CPP" \
    || fail "HelperRunner does not require a regular elevation tool"
grep -q 'st_uid != 0' "$RUNNER_CPP" \
    || fail "HelperRunner does not require root ownership"
grep -q 'S_IWGRP | S_IWOTH' "$RUNNER_CPP" \
    || fail "HelperRunner does not refuse group/world-writable candidates"
grep -q 'S_ISDIR' "$RUNNER_CPP" \
    || fail "HelperRunner does not verify the candidate directory"
grep -q 'execv(' "$RUNNER_CPP" \
    || fail "HelperRunner does not exec the verified absolute path"
grep -q 'execvp(' "$RUNNER_CPP" \
    && fail "HelperRunner still execvp's with a bare name (never PATH)"
grep -q 'no trusted sudo/gksu found in the fixed system locations' "$RUNNER_CPP" \
    || fail "HelperRunner lost the fail-closed elevation message"
grep -q 'verifyTrustedToolPath' "$RUNNER_CPP" \
    || fail "HelperRunner lost the trusted-tool verification"
# B4 helper verification for elevation + the --print-config surface.
grep -q 'verifyHelperForElevation' "$RUNNER_CPP" \
    || fail "HelperRunner lost the helper elevation verification"
grep -q 'realpath(' "$RUNNER_CPP" \
    || fail "helper verification lost the realpath comparison"
grep -q 'verifyHelperForElevation' "$GUI_DIR/src/main.cpp" \
    || fail "--print-config does not verify the helper"
grep -q 'helper elevation trust' "$GUI_DIR/src/main.cpp" \
    || fail "--print-config lost the helper trust line"
grep -q 'BOOT_REPAIR_LEGACY_ELEVATE' "$GUI_DIR/src/main.cpp" \
    || fail "main() lost the BOOT_REPAIR_LEGACY_ELEVATE escape hatch"
# B4 cancel wiring: the GUI-owned cancel file travels with every command and
# cancel() touches it before the direct kill.
grep -q -- '--cancel-file' "$GUI_DIR/src/LegacyMainWindow.cpp" \
    || fail "the GUI does not pass --cancel-file to the helper"
grep -q 'cancel-request' "$GUI_DIR/src/LegacyMainWindow.cpp" \
    || fail "the GUI lost its private cancel-request path"
grep -q 'setCancelFile' "$GUI_DIR/src/LegacyMainWindow.cpp" \
    || fail "startCommand does not wire the cancel file into the runner"
grep -q 'setCancelFile' "$RUNNER_CPP" \
    || fail "HelperRunner lost setCancelFile"
grep -q 'm_cancelFile' "$RUNNER_CPP" \
    || fail "HelperRunner lost the cancel-file state"
grep -q 'O_WRONLY | O_CREAT | O_NOFOLLOW' "$RUNNER_CPP" \
    || fail "cancel() does not touch the cancel file with O_NOFOLLOW"
grep -q 'm_cancelFilePath' "$GUI_DIR/src/LegacyMainWindow.cpp" \
    || fail "LegacyMainWindow lost the cancel-file path state"
grep -q 'boundedPollSleep();' "$RUNNER_CPP" \
    || fail "cancel() lost the grace sleep before the direct kill"
# B4 helper-side cancel token (overlay source): the predicate, the watcher
# with its parent-death poll and the stage-boundary check.
grep -q 'legacy_cancel_requested' "$OVERLAY" \
    || fail "overlay lost the cancel-token predicate"
grep -q 'legacy_cancel_watcher' "$OVERLAY" \
    || fail "overlay lost the shell-command cancel watcher"
grep -q 'kill -0 "$PPID"' "$OVERLAY" \
    || fail "overlay watcher does not poll kill -0 PPID"
grep -q 'terminate_helper_tree' "$OVERLAY" \
    || fail "overlay watcher lost the bounded TERM->KILL escalation"
grep -q 'CANCEL_TOKEN' "$OVERLAY" \
    || fail "overlay lost the CANCEL_TOKEN surface"
grep -q 'legacy_cancel_stage_check' "$OVERLAY" \
    || fail "overlay lost the stage-boundary cancel check"
pass "B4 elevation trust (fixed paths, lstat/uid0/not-writable, no bare execvp), helper verification, cancel-file wiring, watcher kill -0 PPID"

# --- Qt3 auth-pipe harness (runs on Etch; skips without qmake-qt3) ----------
for file in "$TESTS_DIR/auth-pipe-test.cpp" "$TESTS_DIR/auth-pipe-test.pro" \
    "$TESTS_DIR/test-auth-pipe.sh"; do
    [[ -f "$file" ]] || fail "missing auth-pipe harness file: ${file#"$ROOT_DIR"/}"
done
grep -q 'not-read-by-sudo' "$TESTS_DIR/auth-pipe-test.cpp" \
    || fail "auth-pipe harness lost the fake-sudo stdin check"
grep -q 'SIGPIPE' "$TESTS_DIR/auth-pipe-test.cpp" \
    || fail "auth-pipe harness does not name the SIGPIPE failure"
grep -q 'SESSION-EXPIRY OK' "$TESTS_DIR/auth-pipe-test.cpp" \
    || fail "auth-pipe harness lost the cached-session expiry coverage"
grep -q 'sessionIsCurrent' "$TESTS_DIR/auth-pipe-test.cpp" \
    || fail "auth-pipe harness does not exercise the session-validity probe"
# B4: with fixed-path elevation trust the harness drives the fake sudo through
# the documented test seam, never through PATH.
grep -q 'BOOT_REPAIR_LEGACY_FAKE_SUDO' "$TESTS_DIR/auth-pipe-test.cpp" \
    || fail "auth-pipe harness no longer uses the fixed-path fake-sudo test seam"
grep -q 'BOOT_REPAIR_LEGACY_FAKE_SUDO' "$RUNNER_CPP" \
    || fail "HelperRunner lost the fake-sudo test seam"
# B3 (A10-06): the harness must also cover the slow-fake-sudo bounded-wait
# case (the 10-second WNOHANG deadlines and the 32 KiB stderr cap).
grep -q 'slow' "$TESTS_DIR/test-auth-pipe.sh" \
    || fail "auth-pipe harness lost the slow-fake-sudo bounded-wait case"
grep -q 'BOUNDED-WAIT OK' "$TESTS_DIR/auth-pipe-test.cpp" \
    || fail "auth-pipe harness does not exercise the bounded-wait deadlines"
grep -q '32 \* 1024' "$TESTS_DIR/auth-pipe-test.cpp" \
    || fail "auth-pipe harness does not assert the 32 KiB stderr cap"
bash -n "$TESTS_DIR/test-auth-pipe.sh" || fail "test-auth-pipe.sh failed bash -n"
"$TESTS_DIR/test-auth-pipe.sh"
pass "Qt3 auth-pipe harness present (Etch-runnable, host-skipped)"

echo "legacy GUI contract: PASS"
