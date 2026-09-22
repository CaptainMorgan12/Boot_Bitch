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
