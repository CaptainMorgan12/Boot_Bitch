cmake_minimum_required(VERSION 3.20)

project(BootRepair
    VERSION 0.2.23
    DESCRIPTION "Guarded Linux boot repair and recovery utility with diagnostics and privileged target repair backend"
    LANGUAGES CXX
)

include(GNUInstallDirs)

# Keep install/package directory modes deterministic even when the builder uses
# a collaborative umask such as 0002. Debian package directories should not
# become group-writable merely because the source build directory is.
set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
include(CTest)
find_package(Qt6 REQUIRED COMPONENTS Test)

# KDE KAuth remains optional. When present we link it
# so the same source tree validates the future privilege-integration dependency
# without making KDE mandatory on non-Plasma desktops.
find_package(KF6Auth QUIET NO_MODULE)

add_executable(boot-repair
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/SystemScanner.cpp
    src/SystemScanner.h
    src/CapabilityChecker.cpp
    src/CapabilityChecker.h
    resources/boot-repair.qrc
)

target_link_libraries(boot-repair
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

if(KF6Auth_FOUND)
    target_link_libraries(boot-repair PRIVATE KF6::AuthCore)
    target_compile_definitions(boot-repair PRIVATE BOOT_REPAIR_HAVE_KAUTH=1)
else()
    message(STATUS "KF6Auth not found: building without KDE privilege integration")
endif()

target_compile_definitions(boot-repair
    PRIVATE
        BOOT_REPAIR_VERSION="${PROJECT_VERSION}"
        BOOT_REPAIR_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(boot-repair PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS boot-repair
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES data/org.bootrepair.BootRepair.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

# AppStream metadata lets KDE Discover, GNOME Software and other software
# centers associate the desktop launcher with the installed Debian package.
# Those centers can then expose their normal uninstall workflow from the
# application entry/context menu.
install(FILES data/org.bootrepair.BootRepair.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo
)

set(BOOT_REPAIR_ICON_NAME "org.bootrepair.BootRepair")
set(BOOT_REPAIR_ICON_SIZES 16 22 24 32 48 64 128 256 512 1024)
foreach(icon_size IN LISTS BOOT_REPAIR_ICON_SIZES)
    install(FILES
        data/icons/hicolor/${icon_size}x${icon_size}/apps/${BOOT_REPAIR_ICON_NAME}.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${icon_size}x${icon_size}/apps
    )
endforeach()

install(PROGRAMS scripts/boot-repair-helper.sh
    DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/boot-repair
    RENAME boot-repair-helper
)

install(PROGRAMS scripts/boot-repair-efi-label.py
    DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/boot-repair
    RENAME boot-repair-efi-label.py
)

# Install Debian-compatible package copyright metadata. The full upstream MIT
# license remains in LICENSE at the source-tree root.
install(FILES data/copyright
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/boot-repair
)

# Build reproducible compressed documentation when gzip is available. This is
# standard on Debian-family packaging hosts, but remains optional for portable
# source builds on other systems.
find_program(GZIP_EXECUTABLE gzip)
if(GZIP_EXECUTABLE)
    execute_process(
        COMMAND ${GZIP_EXECUTABLE} -9 -n -c ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.md
        OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/changelog.gz
        COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(
        COMMAND ${GZIP_EXECUTABLE} -9 -n -c ${CMAKE_CURRENT_SOURCE_DIR}/data/boot-repair.1
        OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/boot-repair.1.gz
        COMMAND_ERROR_IS_FATAL ANY
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/changelog.gz
        DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/boot-repair
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/boot-repair.1.gz
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
    )
else()
    message(STATUS "gzip not found: compressed changelog/man page will not be installed")
endif()


# -----------------------------------------------------------------------------
# Debian package support (CPack)
# -----------------------------------------------------------------------------
# Normal source installs still honor CMAKE_INSTALL_PREFIX (default /usr/local).
# The generated Debian package uses standard /usr paths regardless of that
# local-install prefix.
set(CPACK_PACKAGE_NAME "boot-repair")
set(CPACK_PACKAGE_VENDOR "CaptainMorgan12")
set(CPACK_PACKAGE_CONTACT "CaptainMorgan12 <captainmorgan12@users.noreply.github.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Linux recovery and boot-repair utility")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Boot Bitch")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
# Optional platform-theme integrations make Qt controls and dialogs blend
# into GNOME without overriding KDE's native Qt theme.
set(CPACK_DEBIAN_PACKAGE_SUGGESTS "qt6-gtk-platformtheme, qt6-xdgdesktopportal-platformtheme, qgnomeplatform-qt6")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "pkexec, mount, cryptsetup, rsync, btrfs-progs, efibootmgr, binutils, python3, hicolor-icon-theme")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Boot Bitch is a graphical Linux recovery and boot-repair utility.\n It provides protected system discovery, diagnostics and guarded target\n repairs. The privileged backend currently supports Debian/Ubuntu-family\n target repairs, LUKS unlock and guarded bidirectional file recovery/copy.")

include(CPack)

# -----------------------------------------------------------------------------
# Read-only GUI regression tests
# -----------------------------------------------------------------------------
enable_testing()

add_executable(boot-repair-ui-tests
    tests/MainWindowUiTest.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/SystemScanner.cpp
    src/SystemScanner.h
    src/CapabilityChecker.cpp
    src/CapabilityChecker.h
    resources/boot-repair.qrc
)

target_link_libraries(boot-repair-ui-tests
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Test
)

target_include_directories(boot-repair-ui-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_definitions(boot-repair-ui-tests
    PRIVATE
        BOOT_REPAIR_VERSION="${PROJECT_VERSION}"
        BOOT_REPAIR_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(boot-repair-ui-tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME boot-repair-ui-read-only COMMAND boot-repair-ui-tests)
add_test(NAME boot-repair-chroot-shell-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-chroot-shell-contract.sh)
add_test(NAME boot-repair-display-manager-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-display-manager-contract.sh)
add_test(NAME boot-repair-icon-atlas-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check-icon-atlas.sh)

add_test(NAME boot-repair-efi-label-update
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/tests/efi-label-update-test.py)
