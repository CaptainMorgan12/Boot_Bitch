# Boot Bitch legacy Qt3 GUI (Debian Etch / KDE 3.5 era).
#
# Qt3-only: core widgets + libc/libstdc++, no kdelibs. Build natively with
# qmake-qt3 + g++ 4.1 on the Etch guest (scripts/package-legacy.sh). A shadow
# build works by running qmake from an empty build directory:
#     cd Development/build-legacy-package/gui && qmake-qt3 /path/to/boot-bitch-legacy.pro
#
# The Q3* widget class names belong to Qt4's Qt3-support module; Qt3 itself
# provides QMainWindow/QListView/QTextEdit/QTabWidget, which is what this
# frontend uses.

TEMPLATE = app
TARGET   = boot-repair-legacy-gui
CONFIG  += qt warn_on release
CONFIG  -= kde

# Overridden by scripts/package-legacy.sh with the project version parsed from
# CMakeLists.txt (export BOOT_REPAIR_LEGACY_VERSION=<version> before qmake).
VERSION = $$(BOOT_REPAIR_LEGACY_VERSION)
isEmpty(VERSION) {
    VERSION = 0.0.0
}
DEFINES += LEGACY_VERSION=\"$$VERSION\"

# Keep all build output in the shadow build directory (the Makefile's
# directory); the source tree stays clean.
MOC_DIR     = .
OBJECTS_DIR = .
DESTDIR     = .

INCLUDEPATH += src
HEADERS = \
    src/EvidenceParser.h \
    src/DeviceInventory.h \
    src/HelperRunner.h \
    src/LegacyMainWindow.h
SOURCES = \
    src/EvidenceParser.cpp \
    src/DeviceInventory.cpp \
    src/HelperRunner.cpp \
    src/LegacyMainWindow.cpp \
    src/main.cpp

# C++98 baseline (g++ 4.1 on Etch).
QMAKE_CXXFLAGS += -std=c++98

# Qt3 i18n: the shipped translation catalogs (sources). The .qm binaries are
# compiled in-guest by Qt3 lrelease during packaging (scripts/package-legacy.sh),
# because Qt3 and Qt6 .qm formats are incompatible. Deferred: zh ja ko ar (CJK
# fonts / bidi not font-safe on Etch).
TRANSLATIONS = \
    translations/boot-repair-legacy_de.ts \
    translations/boot-repair-legacy_fr.ts \
    translations/boot-repair-legacy_es.ts \
    translations/boot-repair-legacy_it.ts \
    translations/boot-repair-legacy_pt.ts \
    translations/boot-repair-legacy_nl.ts \
    translations/boot-repair-legacy_sv.ts \
    translations/boot-repair-legacy_da.ts \
    translations/boot-repair-legacy_fi.ts \
    translations/boot-repair-legacy_no.ts \
    translations/boot-repair-legacy_pl.ts \
    translations/boot-repair-legacy_cs.ts \
    translations/boot-repair-legacy_hu.ts \
    translations/boot-repair-legacy_ro.ts \
    translations/boot-repair-legacy_tr.ts \
    translations/boot-repair-legacy_el.ts \
    translations/boot-repair-legacy_ru.ts \
    translations/boot-repair-legacy_uk.ts
