# Qt3 console harness for the HelperRunner sudo authentication pipe.
#
# Built and run by legacy/tests/test-auth-pipe.sh on the Etch guest; the modern
# host has no Qt3 toolchain and skips it.
TEMPLATE = app
TARGET   = legacy-auth-pipe-test
CONFIG  += qt console warn_on release
CONFIG  -= kde

MOC_DIR     = .
OBJECTS_DIR = .
DESTDIR     = .

INCLUDEPATH += ../gui/src
HEADERS = \
    ../gui/src/HelperRunner.h
SOURCES = \
    auth-pipe-test.cpp \
    ../gui/src/HelperRunner.cpp

# C++98 baseline (g++ 4.1 on Etch).
QMAKE_CXXFLAGS += -std=c++98

# ENV-1: this harness is the only target that needs the documented test seams
# (BOOT_REPAIR_LEGACY_FAKE_SUDO / BOOT_REPAIR_LEGACY_HELPER); production GUI
# builds compile them out entirely and root never honours them even here.
DEFINES += BOOT_REPAIR_LEGACY_TEST_SEAMS
