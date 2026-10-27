#!/usr/bin/env bash
# legacy/tests/test-ui-parity-qt3.sh — the "UI/UX port contract" for the Etch toolchain.
#
# Static Qt3-API compatibility check for the legacy GUI: scans
# legacy/gui/src/*.cpp + legacy/gui/src/*.h for Qt4+/Qt5+/Qt6-only API usage
# and fails on ANY hit.  The last Etch deploy hit four Qt3-incompatibility
# regressions the host string-analysis cannot catch:
#
#   QProcess::setEnvironment            (Qt 4.3+; Qt3 QProcess has no setEnvironment)
#   QByteArray::constData()             (Qt 4;   Qt3 QByteArray has data() only)
#   QByteArray::operator+=              (Qt 4;   Qt3 QByteArray has no operator+=)
#   QByteArray(const char*)             (Qt 4;   Qt3 needs (const char*, uint))
#
# Comment text is stripped BEFORE matching, so the many "Qt3 has no X" notes in
# the sources do not self-flag.  Every pattern below is a known Qt3-vs-Qt4+
# gap; extend the two tables (PATTERNS / QBA check) when a new one is found.
#
# FORBIDDEN (any hit fails):
#   setEnvironment                       QProcess::setEnvironment (Qt 4.3; Qt3 has none)
#   .constData()                         QByteArray/QString::constData (Qt 4)
#   QTableWidget                         Qt 4 widget (Qt3: QTable)
#   QComboBox::currentIndexChanged via   Qt 5; Qt3's QComboBox signal is a single
#     qOverload / QOverload                string-free slot (no int overload)
#   QByteArray::operator+=               Qt 4; caught by the QBA += / .append()
#                                          pass below (same-line QByteArray)
#   QByteArray(const char                Qt 4 single-arg ctor (Qt3: (const char*,uint))
#   QByteArray name("literal")           Qt 4 single-arg declaration (Qt3: two args)
#   QString::toUtf8().constData          Qt 4; Qt3 toUtf8() returns const char*
#   QHeaderView                          Qt 4 (Qt3 QTable owns its headers)
#   QStyleHints                          Qt 5
#   qRegisterMetaType                    Qt 4.2
#   QLatin1String / QStringLiteral       Qt 4 / Qt 5 (Qt3 has neither)
#   QList/QVector/QHash/QSet/QStack/     Qt 4 containers (Qt3: QValueList/
#     QQueue/QLinkedList/QMultiHash/       QValueVector/QPtrList/QMap)
#     QMultiMap
#   QString::fromStdString / toStdString Qt 4
#   QStringList::removeAll / removeOne      Qt 4 (Qt3 QValueList has remove(),
#                                             which drops all occurrences but
#                                             returns void; probe with contains())
#   QProcess Qt4 methods:                Qt 4 (Qt3: tryTerminate(), no
#     setProcessChannelMode/               waitFor*/setWorkingDirectory/...)
#     setProcessEnvironment/
#     waitForStarted/waitForFinished/
#     setWorkingDirectory/startDetached/
#     setStandard{Output,Error,Input}File
#   .terminate()                         QProcess/QThread::terminate (Qt 4)
#   &Sender::member PMF connect          Qt 5 new-style connect (Qt3: SIGNAL/SLOT)
#
# KNOWN-OK (do NOT flag):
#   .contains(QRegExp)                   QRegExp is Qt3-native (QStringList::contains(QRegExp))
#   QString += / QString::operator+=     Qt3 QByteArray lacks += but QString has it
#   QMap                                 QMap exists in Qt3
#   QValueList / QValueVector / QPtrList Qt3 containers
#   .data()                              QByteArray::data() / QString::data() are Qt3
#   SIGNAL() / SLOT()                    Qt3/4 string-based connections
#   QSettings / QSocketNotifier / ...    Qt3 classes (do not add them)
#
# Host-side; runs on the dev/CI box, not on Etch (needs only bash + awk/grep).
# Round 30 addition: the MAP-PARITY block at the end locks the shared
# repair->diagnostic invalidation map (data/repair-diagnostic-map.tsv) so the
# modern and the legacy GUI can never diverge by a one-sided edit:
#   - the canonical file exists and its sha256 matches the golden checksum
#     recorded here (a map change must touch this contract);
#   - the exact data-row content matches the golden row list (readable
#     failure, the row set itself is locked);
#   - BOTH GUIs reference the file (the modern src/MainWindow.cpp +
#     resources/boot-repair.qrc, the legacy GUI sources +
#     scripts/package-legacy.sh), and the removed hardcoded tables cannot
#     come back (the modern repairDiagnosticSectionSpecs removal lock).
# Round 32 addition: the TOOLTIP-PARITY block locks the legacy per-repair
# button tooltips against the modern updateRepairToolDetails buttonTooltip
# chain (verbatim literals, two documented Etch deviations: the static DKMS
# label and the GRUB2-vs-GRUB backend branch), plus the updateToolDetails
# wiring lock so the table field is actually consumed.
# Round 57 addition: the FILE-COPY-PARITY block locks the legacy File Copy
# surface against the modern two-level destination workflow (the staged-source
# Clear button, the destination-entry dialog controls and the folder-picker
# controls — verbatim labels plus wiring locks), and the REPAIR-TOOL-ORDER
# block locks the individual repair-tool list to the modern functional
# grouping (repairToolKeyOrder; the Full Repair plan order is not locked —
# it follows the helper's repair order).
# Round 68 (I1) re-pin: the destination-entry dialog's explicit Clear button
# was REMOVED from both GUIs (the in-field atlas clear action + backspace
# clear the entry; the button was redundant) — the block now locks the
# ABSENCE of the entry-dialog Clear wiring on both sides while the tab-level
# staged-source Clear buttons stay locked as before.
# Round 58 addition: the CLEAR-REGISTER-PARITY block locks the legacy Clear
# Register confirmation against the modern clearActionRegister() dialog (the
# warning wording, the Clear/Cancel controls and the warm-cache gate).
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
GUI_DIR="$ROOT_DIR/legacy/gui/src"

fail()
{
    printf 'FAIL: %s\n' "$*" >&2
    exit 1
}

command -v awk >/dev/null 2>&1 || fail "awk is required"
command -v grep >/dev/null 2>&1 || fail "grep is required"

# Collect the sources deterministically (sorted); every *.cpp and *.h.
SOURCES="$(printf '%s\n' "$GUI_DIR"/*.cpp "$GUI_DIR"/*.h | LC_ALL=C sort)"
[[ -n "$SOURCES" ]] || fail "no legacy GUI sources under $GUI_DIR"

TMP="$(mktemp -d "${TMPDIR:-/tmp}/boot-repair-qt3-contract.XXXXXX")"
trap 'rm -rf "$TMP"' EXIT

# --- strip comments, keep "file:line:" prefixes for reporting -----------------
# One pass per file; // and /* */ (multi-line) are removed so the many "Qt3 has
# no X" documentation notes cannot self-flag.  String literals are NOT parsed,
# so a literal containing // or /* could hide a trailing pattern on that line —
# an accepted, documented limitation (URLs in strings do not carry Qt4 API).
cat > "$TMP/strip.awk" <<'AWK'
BEGIN { inblock = 0 }
{
    line = $0
    out = ""
    i = 1
    n = length(line)
    while (i <= n) {
        if (inblock) {
            end = index(substr(line, i), "*/")
            if (end == 0) { i = n + 1; break }
            i = i + end + 1
            inblock = 0
            continue
        }
        sl = index(substr(line, i), "//")
        st = index(substr(line, i), "/*")
        if (sl == 0 && st == 0) { out = out substr(line, i); i = n + 1; break }
        if (sl > 0 && (st == 0 || sl < st)) {
            out = out substr(line, i, sl - 1)
            break
        }
        out = out substr(line, i, st - 1)
        i = i + st + 1
        inblock = 1
    }
    print file ":" NR ":" out
}
AWK

: > "$TMP/stripped"
while IFS= read -r f; do
    [[ -f "$f" ]] || fail "missing legacy GUI source: ${f#"$ROOT_DIR"/}"
    awk -v file="${f#"$ROOT_DIR"/}" -f "$TMP/strip.awk" "$f" >> "$TMP/stripped"
done <<< "$SOURCES"

# --- table 1: fixed forbidden patterns ----------------------------------------
# Each entry: <grep -E pattern> :: <description>.  Grep against the stripped
# "file:line:text" stream; any match is a Qt4+-only API hit and fails.
HITS=0
check_pattern()
{
    local pat="$1" desc="$2"
    local matches
    matches="$(grep -E "$pat" "$TMP/stripped" || true)"
    if [[ -n "$matches" ]]; then
        printf '  Qt4+ API %s:\n' "$desc"
        printf '%s\n' "$matches" | sed 's/^/    /'
        HITS=$((HITS + 1))
    fi
}

check_pattern 'setEnvironment' 'QProcess::setEnvironment (Qt 4.3; Qt3 has none)'
check_pattern '\.constData\(\)' 'QByteArray/QString::constData() (Qt 4)'
check_pattern 'QTableWidget' 'QTableWidget (Qt 4; Qt3 uses QTable)'
check_pattern 'qOverload|QOverload' 'qOverload/QOverload (Qt 5.7; Qt3 QComboBox has no int currentIndexChanged)'
check_pattern 'QHeaderView' 'QHeaderView (Qt 4)'
check_pattern 'QStyleHints' 'QStyleHints (Qt 5)'
check_pattern 'qRegisterMetaType' 'qRegisterMetaType (Qt 4.2)'
check_pattern 'QByteArray[[:space:]]*\([[:space:]]*const[[:space:]]+char' 'QByteArray(const char*) single-arg ctor (Qt 4)'
check_pattern 'QByteArray[[:space:]]+[A-Za-z_][A-Za-z0-9_]*[[:space:]]*\([[:space:]]*"' 'QByteArray name("literal") single-arg declaration (Qt 4)'
check_pattern '\.toUtf8\(\)[[:space:]]*\.' 'QString::toUtf8() followed by a method call (Qt 4; Qt3 returns const char*)'
check_pattern 'QLatin1String' 'QLatin1String (Qt 4)'
check_pattern 'QStringLiteral' 'QStringLiteral (Qt 5)'
check_pattern '\bQList\b|\bQVector\b|\bQHash\b|\bQSet\b|\bQStack\b|\bQQueue\b|\bQLinkedList\b|\bQMultiHash\b|\bQMultiMap\b' 'Qt 4 container (Qt3: QValueList/QValueVector/QPtrList/QMap)'
check_pattern 'QString::fromStdString|\.toStdString\(' 'QString fromStdString/toStdString (Qt 4)'
check_pattern '\.removeAll\(|\.removeOne\(' 'QStringList::removeAll/removeOne (Qt 4; Qt3 QValueList::remove() drops all but returns void)'
check_pattern 'setProcessChannelMode|setProcessEnvironment|waitForStarted|waitForFinished|setWorkingDirectory|startDetached|setStandardOutputFile|setStandardErrorFile|setStandardInputFile' 'QProcess Qt4 method (Qt3: tryTerminate/kill)'
check_pattern '\.terminate\(\)' 'QProcess/QThread::terminate (Qt 4; Qt3: tryTerminate)'
check_pattern '&[A-Za-z_][A-Za-z0-9_:]*::[A-Za-z_][A-Za-z0-9_]*[[:space:]]*,' 'new-style member-function-pointer connect (Qt 5)'

# --- table 2: QByteArray operator+= / append() --------------------------------
# Qt3 QByteArray has neither operator+= nor append(); accumulation must go
# through resize()+memcpy.  grep cannot type-check a bare `ba += x`, so the
# guard flags every line that mentions QByteArray AND a `+=` or `.append(` —
# the two Qt4-only forms.  A `+=` on a QByteArray local declared on an earlier
# line (no "QByteArray" text on the use line) is a known, documented gap:
# keep QByteArray accumulation adjacent to its declaration or extend this.
QBA_HITS="$(grep -E 'QByteArray[^\n]*\+?=' "$TMP/stripped" | grep -E '\+=' || true)"
QBA_APPEND="$(grep -E 'QByteArray[^\n]*\.append[[:space:]]*\(' "$TMP/stripped" || true)"
if [[ -n "$QBA_HITS" || -n "$QBA_APPEND" ]]; then
    printf '  Qt4+ API QByteArray operator+= / append():\n'
    printf '%s\n' "$QBA_HITS" "$QBA_APPEND" | grep -v '^$' | sed 's/^/    /'
    HITS=$((HITS + 1))
fi

if [[ "$HITS" -gt 0 ]]; then
    printf '\nLEGACY QT3 API CHECK: %d Qt4+-only API site(s) found\n' "$HITS" >&2
    exit 1
fi

# ===== map-parity block (round 30): the shared repair->diagnostic map ========
# The single data file both GUIs consume (the modern GUI embeds it through
# resources/boot-repair.qrc; the legacy GUI loads the staged package copy).
# A one-sided edit anywhere fails here.
MAP_FILE="$ROOT_DIR/data/repair-diagnostic-map.tsv"
[[ -f "$MAP_FILE" ]] || fail "canonical map missing: data/repair-diagnostic-map.tsv"

# (1) Golden full-file checksum: a map edit must deliberately re-baseline this
# contract (the "checksum/content assertion" lock).
MAP_SHA="$(sha256sum "$MAP_FILE" | awk '{print $1}')"
[[ "$MAP_SHA" == '1fc9ae6e3828dadabdd8de5406087c27682ee033016999d0f56de58c8b26a035' ]] \
    || fail "repair-diagnostic-map.tsv sha256 $MAP_SHA != the golden checksum (a map change must update the parity contract)"

# (2) Golden data-row content: every row key in the locked order (the row
# content is asserted row-by-row below so a drift names the exact row).
MAP_ROWS="$(grep -vE '^#|^[[:space:]]*$' "$MAP_FILE")"
[[ -n "$MAP_ROWS" ]] || fail "repair-diagnostic-map.tsv carries no data rows"
GOLDEN_ROW_COUNT=44
[[ "$(printf '%s\n' "$MAP_ROWS" | wc -l)" -eq "$GOLDEN_ROW_COUNT" ]] \
    || fail "repair-diagnostic-map.tsv row count changed (expected $GOLDEN_ROW_COUNT)"

cat > "$TMP/map-golden.tsv" <<'MAPEOF'
validate	environment	-
filesystem	filesystem	environment usage btrfs filesystem
fs-inspect	-	-
dpkg	package	package backend usage capabilities
dpkg-configure	package	package backend usage capabilities
fixbroken	package	package backend usage capabilities
fix-broken	package	package backend usage capabilities
aptupdate	package	package backend usage capabilities
apt-update	package	package backend usage capabilities
upgrade	package	package backend usage capabilities
apt-upgrade	package	package backend usage capabilities
install-target-package	package	package backend usage capabilities display
install-host-package	package	package backend usage capabilities display
dkms	kernel	kernel
display	display	display
display-manager	display	display
gpu	gpu kernel	gpu display kernel boot boot-evidence grub uki package usage capabilities
gpu:*	gpu kernel	all
nomodeset	gpu kernel	all
initramfs	kernel	kernel
efi	uki	uki grub boot-evidence
grub	grub	grub
extlinux	boot-evidence	boot boot-evidence
bootstack	boot-evidence kernel uki grub	boot boot-evidence kernel grub uki
boot-stack	boot-evidence kernel uki grub	boot boot-evidence kernel grub uki
kernels	kernel	kernel usage boot boot-evidence grub uki
kernels:*	kernel	kernel usage boot boot-evidence grub uki
host-default	report	all
boot-entry-save	-	uki grub boot-evidence
shell:package-manager	-	package backend usage capabilities
shell:boot-config	-	boot grub uki kernel
shell:general	-	all
shell:no-op	-	-
snapshot-load	-	-
snapshot-inspect	-	-
snapshot-rollback	-	all
file-copy	-	all
plan	union	union
cancelled-repair-with-change	-	union
cancelled-repair-no-change	-	-
cancelled-plan-with-change	-	union
cancelled-shell-with-change	-	shell-class
cancelled-shell-no-change	-	-
cancelled-unclassified	-	all
MAPEOF
if ! diff -u "$TMP/map-golden.tsv" <(printf '%s\n' "$MAP_ROWS") > "$TMP/map-diff.txt"; then
    printf '  repair-diagnostic-map.tsv drifted from the golden row set:\n'
    sed 's/^/    /' "$TMP/map-diff.txt" || true
    fail 'repair-diagnostic-map.tsv rows differ from the parity-contract golden set'
fi

# (3) BOTH GUIs consume the file (the reference greps): the modern GUI embeds
# it through the qrc and loads it in MainWindow.cpp; the legacy GUI loads it
# in the Qt3 sources and the package stages a copy.
grep -q 'repair-diagnostic-map.tsv' "$ROOT_DIR/src/MainWindow.cpp" \
    || fail "src/MainWindow.cpp no longer references repair-diagnostic-map.tsv"
grep -q 'repair-diagnostic-map.tsv' "$ROOT_DIR/resources/boot-repair.qrc" \
    || fail "resources/boot-repair.qrc no longer embeds repair-diagnostic-map.tsv"
grep -q 'repair-diagnostic-map.tsv' "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "the legacy GUI no longer references repair-diagnostic-map.tsv"
grep -q 'repair-diagnostic-map.tsv' "$ROOT_DIR/legacy/gui/src/EvidenceParser.cpp" \
    || fail "the legacy map loader no longer references repair-diagnostic-map.tsv"
grep -q 'repair-diagnostic-map.tsv' "$ROOT_DIR/scripts/package-legacy.sh" \
    || fail "scripts/package-legacy.sh no longer stages repair-diagnostic-map.tsv"

# (4) The hardcoded tables are gone (removal lock): a reintroduced hardcoded
# spec table is exactly the one-sided divergence this contract forbids.
if grep -n 'repairDiagnosticSectionSpecs' "$ROOT_DIR/src/MainWindow.cpp" >/dev/null; then
    fail "src/MainWindow.cpp reintroduced the hardcoded repairDiagnosticSectionSpecs table"
fi

# ===== tooltip-parity block (round 32): per-repair button tooltips ===========
# The legacy individual repair tools must carry the SAME concise per-function
# button tooltip as the modern updateRepairToolDetails buttonTooltip chain.
# The modern source is the reference; every modern per-tool literal must exist
# verbatim in the legacy source. Two documented Etch deviations:
#   - dkms: the modern "Rebuild %1 kernel modules." fills the DETECTED
#     module-build label (akmods/akms/dkms); on Etch the label is statically
#     DKMS (the round-31 A5 audit), so the legacy carries "Rebuild DKMS
#     kernel modules.".
#   - grub: the modern GRUB2 variant ("Regenerate the GRUB2 configuration.")
#     is the Fedora-only backend branch; the legacy carries the plain
#     "Regenerate the GRUB configuration.".
printf '  per-repair tooltip parity:\n'
TIPTOOL_HITS=0
check_tooltip_pair()
{
    local modern="$1" legacy="$2" note="$3"
    grep -qF "\"$modern\"" "$ROOT_DIR/src/MainWindow.cpp" \
        || fail "modern per-repair tooltip missing from src/MainWindow.cpp: $modern"
    if ! grep -qF "\"$legacy\"" "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp"; then
        printf '    legacy tooltip missing: %s\n' "$legacy"
        TIPTOOL_HITS=$((TIPTOOL_HITS + 1))
    fi
    printf '    %s\n' "$note"
}
check_tooltip_pair 'Run the read-only safety preflight.' \
    'Run the read-only safety preflight.' 'validate: identical'
check_tooltip_pair 'Check and repair the detected file systems.' \
    'Check and repair the detected file systems.' 'filesystem: identical'
check_tooltip_pair 'Complete interrupted package configuration.' \
    'Complete interrupted package configuration.' 'dpkg: identical'
check_tooltip_pair 'Repair broken package dependencies.' \
    'Repair broken package dependencies.' 'fixbroken: identical'
check_tooltip_pair 'Refresh package metadata.' \
    'Refresh package metadata.' 'aptupdate: identical'
check_tooltip_pair 'Simulate and apply a safe package upgrade.' \
    'Simulate and apply a safe package upgrade.' 'upgrade: identical'
check_tooltip_pair 'Rebuild %1 kernel modules.' \
    'Rebuild DKMS kernel modules.' 'dkms: Etch deviation (static DKMS label)'
check_tooltip_pair 'Restore the graphical login manager.' \
    'Restore the graphical login manager.' 'display: identical'
check_tooltip_pair 'Choose a GPU driver repair action from the evidence.' \
    'Choose a GPU driver repair action from the evidence.' 'gpu: identical (round-32 modern chain completion)'
check_tooltip_pair 'Rebuild initramfs images.' \
    'Rebuild initramfs images.' 'initramfs: identical'
check_tooltip_pair 'Repair the EFI / UKI boot path.' \
    'Repair the EFI / UKI boot path.' 'efi: identical'
check_tooltip_pair 'Regenerate the GRUB configuration.' \
    'Regenerate the GRUB configuration.' 'grub: identical (the GRUB2 variant is the Fedora-only branch)'
check_tooltip_pair 'Regenerate the extlinux configuration.' \
    'Regenerate the extlinux configuration.' 'extlinux: identical'
check_tooltip_pair 'Reconcile the boot stack.' \
    'Reconcile the boot stack.' 'bootstack: identical'
check_tooltip_pair 'Choose a kernel repair action from the evidence.' \
    'Choose a kernel repair action from the evidence.' 'kernels: identical'
if [[ "$TIPTOOL_HITS" -gt 0 ]]; then
    fail "$TIPTOOL_HITS legacy per-repair tooltip(s) drifted from the modern reference"
fi
# The legacy Run button must actually consume the spec tooltip (the wiring
# lock): a table field that updateToolDetails never reads is invisible.
grep -q 'tr(spec.tooltip)' "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "updateToolDetails no longer reads the per-tool spec.tooltip"
printf '  per-repair tooltip parity: PASS (%d modern per-tool tooltips locked)\n' 15

# ===== file-copy-surface parity block (round 57): the legacy File Copy tab
# and its destination browse must carry the MODERN two-level workflow
# surface — the staged-source Clear button on the tab, the level-1
# destination-entry dialog (folder entry field + Browse repair folders... +
# OK/Cancel under "Select repair-system destination") and the level-2
# folder picker (Up / Open Selected / Refresh / Choose This Folder / Cancel).
# Round 68 (I1): the level-1 dialog's explicit Clear button was removed from
# BOTH GUIs (the in-field clear action + keyboard backspace clear the entry);
# the absence is locked below so a re-introduced Clear cannot drift one-sided.
# Control labels are locked verbatim on both sides (one documented glyph
# deviation: modern "Browse repair folders…" U+2026 vs the legacy ASCII
# "Browse repair folders...", which Etch's fonts render correctly); wiring
# locks assert the buttons are actually connected (clearSourceList /
# fileCopyClearStaging, the entry Browse -> fileCopyBrowseEntryBrowse, the
# picker navigation slots).
printf '  file-copy surface parity:\n'
FCOPY_HITS=0
check_fcopy_pair()
{
    local modern="$1" legacy="$2" note="$3"
    grep -qF "\"$modern\"" "$ROOT_DIR/src/MainWindow.cpp" \
        || fail "modern file-copy string missing from src/MainWindow.cpp: $modern"
    if ! grep -qF "\"$legacy\"" "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp"; then
        printf '    legacy file-copy string missing: %s\n' "$legacy"
        FCOPY_HITS=$((FCOPY_HITS + 1))
    fi
    printf '    %s\n' "$note"
}
check_fcopy_pair 'Select repair-system destination' \
    'Select repair-system destination' 'entry dialog title: identical'
check_fcopy_pair 'Choose a destination folder inside the repaired system' \
    'Choose a destination folder inside the repaired system' 'entry heading: identical'
check_fcopy_pair 'Browse repair folders…' \
    'Browse repair folders...' 'browse action: Qt3 ASCII-ellipsis deviation'
check_fcopy_pair 'Up' 'Up' 'picker Up: identical'
check_fcopy_pair 'Open Selected' 'Open Selected' 'picker Open Selected: identical'
check_fcopy_pair 'Refresh' 'Refresh' 'picker Refresh: identical'
check_fcopy_pair 'Choose This Folder' 'Choose This Folder' 'picker accept: identical'
check_fcopy_pair 'Read-only target view; no files are modified.' \
    'Read-only target view; no files are modified.' 'picker status: identical'
check_fcopy_pair 'Current repaired-system folder: %1' \
    'Current repaired-system folder: %1' 'picker path label: identical'
if [[ "$FCOPY_HITS" -gt 0 ]]; then
    fail "$FCOPY_HITS legacy file-copy string(s) drifted from the modern reference"
fi
# Round 68 (I1) absence locks: the level-1 entry dialog must NOT carry an
# explicit Clear button on either side (the in-field clear action + backspace
# are the clearing surface; the tab-level staged-source Clear stays).
grep -qF 'connect(clearPath, &QPushButton::clicked' \
    "$ROOT_DIR/src/MainWindow.cpp" \
    && fail "modern entry dialog Clear button re-introduced (clearPath wiring must stay removed)"
grep -qF 'connect(clear, SIGNAL(clicked()), m_fileCopyBrowseEntryEdit' \
    "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    && fail "legacy entry dialog Clear button re-introduced (entry-edit clear wiring must stay removed)"
grep -qF 'connect(clear, &QPushButton::clicked, this, &MainWindow::clearSourceList)' \
    "$ROOT_DIR/src/MainWindow.cpp" \
    || fail "modern file-copy Clear button no longer wired to clearSourceList"
grep -qF 'connect(m_fileCopyClearButton, SIGNAL(clicked()), this, SLOT(fileCopyClearStaging()));' \
    "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy file-copy Clear button no longer wired to fileCopyClearStaging"
grep -q 'runRepairFolderBrowser' "$ROOT_DIR/src/MainWindow.cpp" \
    || fail "modern destination browse no longer opens the repair folder browser"
grep -qF 'SLOT(fileCopyBrowseEntryBrowse())' \
    "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy entry Browse button no longer wired to fileCopyBrowseEntryBrowse"
grep -qF 'SLOT(fileCopyBrowsePickerUp())' \
    "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy picker Up button no longer wired"
grep -qF 'SLOT(fileCopyBrowsePickerOpen())' \
    "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy picker Open Selected button no longer wired"
grep -qF 'SLOT(fileCopyBrowsePickerRefresh())' \
    "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy picker Refresh button no longer wired"
printf '  file-copy surface parity: PASS (9 modern file-copy strings locked + wiring + entry-Clear absence)\n'

# ===== repair-tool order parity block (round 57): the individual repair-tool
# list must follow the modern FUNCTIONAL grouping (CapabilityChecker's
# repairToolKeyOrder: validate/filesystem, the package chain, the kernel group
# kernels+dkms+initramfs, display/gpu, the boot chain, boot-stack).  UI-side
# only — the legacy Full Repair plan order (planSpecs) and the helper's repair
# stage order are deliberately NOT locked here (they stay the helper order).
MODERN_TOOL_ORDER="$(sed -n '/static const QStringList &repairToolKeyOrder()/,/^}/p' "$ROOT_DIR/src/CapabilityChecker.cpp" \
    | grep -oE 'QStringLiteral\("[a-z-]+"\)' \
    | sed -E 's/QStringLiteral\("([a-z-]+)"\)/\1/' | tr '\n' ' ')"
LEGACY_TOOL_ORDER="$(sed -n '/const ToolSpec toolSpecs\[\] = {/,/^};/p' "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    | grep -oE '^    \{ "[a-z-]+"' \
    | sed -E 's/^    \{ "([a-z-]+)"/\1/' | tr '\n' ' ')"
[[ -n "$MODERN_TOOL_ORDER" ]] \
    || fail "could not extract the modern repair-tool order from CapabilityChecker.cpp"
if [[ "$MODERN_TOOL_ORDER" != "$LEGACY_TOOL_ORDER" ]]; then
    printf '  modern order: %s\n' "$MODERN_TOOL_ORDER"
    printf '  legacy order: %s\n' "$LEGACY_TOOL_ORDER"
    fail "legacy toolSpecs order drifted from the modern functional grouping"
fi
printf '  repair-tool order parity: PASS (modern order: %s)\n' "$MODERN_TOOL_ORDER"

# ===== clear-register parity block (round 58): the legacy Clear Register
# confirmation must mirror the modern clearActionRegister() dialog — the same
# warning wording (locked verbatim on both sides), the same Clear/Cancel
# controls, and the same warm-cache gate (the confirmation appears only when
# cached diagnostic evidence exists; a Cancel aborts the clear).
printf '  clear-register parity:\n'
CREG_HITS=0
check_creg_pair()
{
    local modern="$1" legacy="$2" note="$3"
    grep -qF "\"$modern\"" "$ROOT_DIR/src/MainWindow.cpp" \
        || fail "modern clear-register string missing from src/MainWindow.cpp: $modern"
    if ! grep -qF "\"$legacy\"" "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp"; then
        printf '    legacy clear-register string missing: %s\n' "$legacy"
        CREG_HITS=$((CREG_HITS + 1))
    fi
    printf '    %s\n' "$note"
}
check_creg_pair 'Clear Register' 'Clear Register' 'dialog title: identical'
check_creg_pair 'Clear the action register and cached diagnostics?' \
    'Clear the action register and cached diagnostics?' 'warning text: identical'
# The detail sentence is one tr() split across two adjacent literals on BOTH
# sides (the SAME split points, so a rewording or a one-sided re-split fails);
# lock the two fragments verbatim.
check_creg_pair 'Cached diagnostics will also be cleared, and repair tools will be ' \
    'Cached diagnostics will also be cleared, and repair tools will be ' 'warning detail fragment 1: identical'
check_creg_pair 'disabled until diagnostics are regenerated.' \
    'disabled until diagnostics are regenerated.' 'warning detail fragment 2: identical'
check_creg_pair 'Clear' 'Clear' 'accept button: identical'
check_creg_pair 'Cancel' 'Cancel' 'reject button: identical'
if [[ "$CREG_HITS" -gt 0 ]]; then
    fail "$CREG_HITS legacy clear-register string(s) drifted from the modern reference"
fi
# Wiring locks: the legacy clearLog() must gate the confirmation on the cached
# diagnostics (the modern hasCachedDiagnostics warm check) and show the
# Clear/Cancel confirmWrapped dialog with Cancel as the default; the modern
# clearActionRegister() must keep its runConfirmDialog call with the Clear
# action label.
grep -q 'hasCachedDiagnostics' "$ROOT_DIR/src/MainWindow.cpp" \
    || fail "modern clearActionRegister no longer gates on hasCachedDiagnostics"
grep -qF 'runConfirmDialog(this, tr("Clear Register")' \
    "$ROOT_DIR/src/MainWindow.cpp" \
    || fail "modern clearActionRegister no longer opens the Clear Register confirmation"
grep -q 'hasCachedDiagnostics' "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy clearLog no longer gates on hasCachedDiagnostics"
grep -qF 'confirmWrapped(' "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy clearLog no longer uses the wrapped confirmation dialog"
grep -qF '/*noIsDefault=*/true' "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy Clear Register confirmation lost its Cancel-default parity"
printf '  clear-register parity: PASS (6 strings locked + warm-cache gate + wiring)\n'

# ===== dialog button-order/default parity block (round 72): the legacy
# multi-button dialogs must mirror the MODERN dialog families' button order
# and defaults (the audit result).  Modern (the reference, verified in the
# code):
# - input dialogs (addUniformDialogButtons): [accept][Cancel], accept is the
#   DEFAULT (setDefault(true));
# - confirmations (runConfirmDialog): [action][Cancel], the action's
#   autoDefault is OFF and CANCEL is the default + focus — modern's ACTUAL
#   default for EVERY confirmation (incl. the full-repair "Run Repair" and
#   the Clear Register "Clear") is Cancel.
# Legacy: the positive button is ADDED to its row before Cancel on both
# families, every confirmWrapped call site passes noIsDefault=true (Cancel
# default + focus), the input-family dialogs keep the positive default, and
# the Add Note dialog must not regress to the platform-ordered QInputDialog.
printf '  dialog button-order/default parity:\n'
DLOG_HITS=0
check_dialog_lock()
{
    local side="$1" file="$2" pattern="$3" note="$4"
    if ! grep -qF "$pattern" "$file"; then
        printf '    %s dialog order/default drift: %s\n' "$side" "$note"
        DLOG_HITS=$((DLOG_HITS + 1))
    fi
    printf '    %s %s\n' "$side" "$note"
}
MODERN_DLG="$ROOT_DIR/src/MainWindow.cpp"
LEGACY_DLG="$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp"
# Modern reference locks (the ACTUAL order/default the round-72 audit
# verified against; a modern-side drift fails here too).
check_dialog_lock modern "$MODERN_DLG" \
    'buttonRow->addWidget(pair.accept);' \
    'addUniformDialogButtons adds accept before Cancel'
check_dialog_lock modern "$MODERN_DLG" \
    'pair.accept->setDefault(true);' \
    'addUniformDialogButtons accept is the default'
check_dialog_lock modern "$MODERN_DLG" \
    'actionButton->setAutoDefault(false);' \
    'runConfirmDialog disables the action autoDefault'
check_dialog_lock modern "$MODERN_DLG" \
    'cancelButton->setDefault(true);' \
    'runConfirmDialog Cancel is the default'
check_dialog_lock modern "$MODERN_DLG" \
    'cancelButton->setFocus();' \
    'runConfirmDialog Cancel owns the focus'
check_dialog_lock modern "$MODERN_DLG" \
    'buttonRow->addWidget(actionButton);' \
    'runConfirmDialog adds the action before Cancel'
# Legacy locks (every audited dialog aligned to its modern family).
check_dialog_lock legacy "$LEGACY_DLG" \
    'buttons->addWidget(yesButton);' \
    'confirmWrapped adds the positive button before No/Cancel'
check_dialog_lock legacy "$LEGACY_DLG" \
    '/*noIsDefault=*/true' \
    'every confirmWrapped call site passes the Cancel default'
check_dialog_lock legacy "$LEGACY_DLG" \
    'buttons->addWidget(save);' \
    'the config editor adds Save Target File before Cancel'
check_dialog_lock legacy "$LEGACY_DLG" \
    'save->setFocus();' \
    'the config editor Save button owns the focus (Enter saves)'
check_dialog_lock legacy "$LEGACY_DLG" \
    'buttons->addWidget(addNote);' \
    'the Add Note dialog adds Add Note before Cancel'
check_dialog_lock legacy "$LEGACY_DLG" \
    'addNote->setFocus();' \
    'the Add Note dialog focuses Add Note (Enter saves)'
check_dialog_lock legacy "$LEGACY_DLG" \
    'buttons->addWidget(unlockButton);' \
    'the unlock passphrase dialog adds Unlock before Cancel'
check_dialog_lock legacy "$LEGACY_DLG" \
    'buttons->addWidget(ok);' \
    'the level-1 entry dialog adds OK before Cancel'
check_dialog_lock legacy "$LEGACY_DLG" \
    'pickerButtons->addWidget(pickerChoose);' \
    'the level-2 picker adds Choose This Folder before Cancel'
check_dialog_lock legacy "$LEGACY_DLG" \
    'buttons->addWidget(accept);' \
    'promptHiddenPassword adds OK before Cancel'
check_dialog_lock legacy "$LEGACY_DLG" \
    'tr("Try Again"), tr("Cancel"),' \
    'the passphrase retry is the Try Again/Cancel confirmation'
# The round-72 ITEM 1 trap lock: the prompt inputs must consume Return/Enter
# (Qt 3.3.7's QLineEdit leaves the key unaccepted, so the propagated Return
# clicks the focused Cancel and cancels the run right after the answer — the
# round-72 rig evidence).  Modern is safe by construction (Qt6 QLineEdit
# consumes Return); the legacy class is the lock.
check_dialog_lock legacy "$LEGACY_DLG" \
    'class PromptAnswerLineEdit : public QLineEdit' \
    'the prompt input class exists (Return/Enter consumption)'
check_dialog_lock legacy "$LEGACY_DLG" \
    'new PromptAnswerLineEdit(dialog)' \
    'the repair result prompt input consumes Return/Enter'
check_dialog_lock legacy "$LEGACY_DLG" \
    'new PromptAnswerLineEdit(m_chrootGroup)' \
    'the shell prompt input consumes Return/Enter'
if grep -qF 'QInputDialog::getText' "$LEGACY_DLG"; then
    printf '    legacy Add Note regressed to QInputDialog::getText (platform-ordered OK/Cancel)\n'
    DLOG_HITS=$((DLOG_HITS + 1))
fi
# Round 74 (the modern-parity Add Note memo + pinned width): the modern
# round-66/73-A5 reference (multi-line memo + placeholder + 640 min/max pin)
# and the legacy mirror (the round-31 PlaceholderTextEdit memo, the vertical
# stretch, the 640 min/max pin).  A drift on either side fails here.
check_dialog_lock modern "$MODERN_DLG" \
    'auto *memo = new QPlainTextEdit;' \
    'the modern Add Note memo is the multi-line editor'
check_dialog_lock modern "$MODERN_DLG" \
    'memo->setPlaceholderText(tr("Enter the note text"));' \
    'the modern Add Note memo carries the placeholder'
check_dialog_lock modern "$MODERN_DLG" \
    'standardDialogLayout(&dialog, 640)' \
    'the modern Add Note dialog pins the 640 floor'
check_dialog_lock modern "$MODERN_DLG" \
    'dialog.setMaximumWidth(640);' \
    'the modern Add Note dialog pins the 640 cap'
check_dialog_lock legacy "$LEGACY_DLG" \
    'PlaceholderTextEdit *memo = new PlaceholderTextEdit(&dialog, "addNoteMemo");' \
    'the legacy Add Note input is the multi-line memo'
check_dialog_lock legacy "$LEGACY_DLG" \
    'memo->setPlaceholderText(tr("Enter the note text"));' \
    'the legacy Add Note memo carries the mirrored placeholder'
check_dialog_lock legacy "$LEGACY_DLG" \
    'layout->addWidget(memo, 1);' \
    'the legacy Add Note memo stretches vertically (resizable like modern)'
check_dialog_lock legacy "$LEGACY_DLG" \
    'dialog.setMinimumWidth(640);' \
    'the legacy Add Note dialog pins the 640 floor'
check_dialog_lock legacy "$LEGACY_DLG" \
    'dialog.setMaximumWidth(640);' \
    'the legacy Add Note dialog pins the 640 cap'
check_dialog_lock legacy "$LEGACY_DLG" \
    'layout->setResizeMode(QLayout::FreeResize);' \
    'the legacy Add Note width pin keeps the Qt3 layout-clobber guard (FreeResize)'
if [[ "$DLOG_HITS" -gt 0 ]]; then
    fail "$DLOG_HITS dialog button-order/default drift(s)"
fi
printf '  dialog button-order/default parity: PASS (modern reference + legacy order/default locks)\n'

# ===== host-name resolution parity block (round 70, E3): the running-system
# name on the Systems host card resolves through the SAME mechanism in both
# GUIs — the running host's own /etc/os-release PRETTY_NAME first, then the
# helper's resolved fact line(s), then the shared static fallback.  The
# resolved VALUE is machine-dependent (PRETTY_NAME differs per host and the
# Etch rigs have no os-release at all), so this lock pins the MECHANISM (the
# resolution order and the fallback literal), never the value.  Modern:
# CapabilityChecker::distributionLabel() (PRETTY_NAME from /etc/os-release)
# consumed by MainWindow::updateHostCard with the "Current running Linux
# system" fallback.  Legacy: readOsReleaseValue(PRETTY_NAME) -> the helper's
# "System:" fact (the ported TARGET_PRETTY, os-release-aware with the
# legacy-root evidence fallback on Etch) -> the historical "OS:" /
# "Distribution:" facts -> the same fallback literal.
printf '  host-name resolution parity:\n'
grep -qF 'PRETTY_NAME' "$ROOT_DIR/src/CapabilityChecker.cpp" \
    || fail "modern distributionLabel no longer reads PRETTY_NAME from os-release"
grep -qF 'tr("Current running Linux system")' "$ROOT_DIR/src/MainWindow.cpp" \
    || fail "modern host card lost the Current running Linux system fallback"
grep -qF 'readOsReleaseValue(QString::fromLatin1("PRETTY_NAME"))' \
    "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy host card no longer resolves PRETTY_NAME first"
grep -qF 'mapValue(m_factMap, QString::fromLatin1("System:"))' \
    "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy host card no longer falls back to the helper System: fact"
grep -qF 'mapValue(m_factMap, QString::fromLatin1("OS:"))' \
    "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy host card lost the historical OS: fact fallback"
grep -qF 'mapValue(m_factMap, QString::fromLatin1("Distribution:"))' \
    "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy host card lost the historical Distribution: fact fallback"
grep -qF 'tr("Current running Linux system")' \
    "$ROOT_DIR/legacy/gui/src/LegacyMainWindow.cpp" \
    || fail "legacy host card lost the Current running Linux system fallback"
printf '  host-name resolution parity: PASS (os-release PRETTY_NAME mechanism + shared fallback literal)\n'

printf 'repair-diagnostic map parity: PASS (both GUIs consume the locked shared file)\n'
printf 'legacy Qt3 API check: PASS (no Qt4+/Qt5+/Qt6-only API usage)\n'
exit 0
