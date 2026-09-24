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
grep -q 'legacy_apt_intent_translate' "$OVERLAY" \
    || fail "apt intent translation missing from the overlay"
grep -q 'apt intent translated:' "$OVERLAY" \
    || fail "apt intent translation log line missing"
grep -q 'browse_target_directory' "$OVERLAY" \
    || fail "legacy browse-target override missing from the overlay"
grep -q 'BROWSE_ENTRY' "$OVERLAY" \
    || fail "legacy browse record format missing from the overlay"
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
grep -q 'Re-check elevation' "$WINDOW" || fail "GUI lost the elevation re-check control"
grep -q 'Elevation:' "$WINDOW" || fail "GUI lost the elevation state label"
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
grep -q 'm_repairAuthorizeButton' "$WINDOW" || fail "GUI lost the Repair-tab Authorize control"
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
# Cycle 6: one canonical QSettings file (~/.qt/boot-bitchrc), every toggle
# handler persists immediately, and the window close flushes again.
grep -q 'setPath(QString::fromLatin1("boot-bitch"),' "$WINDOW" \
    || fail "settings do not use the canonical boot-bitch path"
grep -q '"boot-repair"), QSettings::User' "$WINDOW" \
    || fail "settings do not use the boot-repair application"
grep -q '~/.qt/boot-bitchrc' "$WINDOW" \
    || fail "settings do not document the canonical file location"
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
bash -n "$TESTS_DIR/test-auth-pipe.sh" || fail "test-auth-pipe.sh failed bash -n"
"$TESTS_DIR/test-auth-pipe.sh"
pass "Qt3 auth-pipe harness present (Etch-runnable, host-skipped)"

echo "legacy GUI contract: PASS"
