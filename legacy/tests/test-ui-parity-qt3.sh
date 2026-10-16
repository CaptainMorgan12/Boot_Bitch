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

printf 'legacy Qt3 API check: PASS (no Qt4+/Qt5+/Qt6-only API usage)\n'
exit 0
