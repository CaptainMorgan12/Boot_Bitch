cmake_minimum_required(VERSION 3.20)

project(BootRepair
    VERSION 0.2.25
    DESCRIPTION "Guarded Linux diagnostics, host maintenance and repair-system recovery utility"
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

# KDE KAuth remains optional. When present it is linked so the same source tree
# validates the future privilege-integration dependency without making KDE
# mandatory on non-Plasma desktops.
find_package(KF6Auth QUIET NO_MODULE)

# QtDBus remains optional. When present it enables the XDG desktop portal
# fallback that resolves the desktop color scheme on GNOME sessions whose Qt
# platform theme never exposes it (for example a Fedora/GNOME dark desktop at
# startup). Without QtDBus the deferred platform re-check still runs.
find_package(Qt6 QUIET COMPONENTS DBus)
if(TARGET Qt6::DBus)
    set(BOOT_REPAIR_QT_DBUS_LIBRARY Qt6::DBus)
    set(BOOT_REPAIR_QT_DBUS_DEFINITIONS BOOT_REPAIR_HAVE_DBUS=1)
else()
    set(BOOT_REPAIR_QT_DBUS_LIBRARY)
    set(BOOT_REPAIR_QT_DBUS_DEFINITIONS)
endif()

# -----------------------------------------------------------------------------
# Application target
# -----------------------------------------------------------------------------
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
        ${BOOT_REPAIR_QT_DBUS_LIBRARY}
)

if(BOOT_REPAIR_QT_DBUS_DEFINITIONS)
    target_compile_definitions(boot-repair PRIVATE ${BOOT_REPAIR_QT_DBUS_DEFINITIONS})
endif()

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

# -----------------------------------------------------------------------------
# Install rules
# -----------------------------------------------------------------------------
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

# -----------------------------------------------------------------------------
# Compressed documentation
# -----------------------------------------------------------------------------
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
set(CPACK_DEBIAN_PACKAGE_DEPENDS "pkexec, mount, cryptsetup, rsync, e2fsprogs, dosfstools, btrfs-progs, xfsprogs, efibootmgr, binutils, python3, libqt6svg6, hicolor-icon-theme")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "exfatprogs, ntfs-3g, f2fs-tools, jfsutils, reiserfsprogs, zfsutils-linux")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Boot Bitch is a graphical Linux recovery and boot-repair utility.\n It provides protected discovery and read-only diagnostics for the running\n host or a selected repair system. The privileged backend supports guarded\n Debian/Ubuntu-family maintenance and repair-system recovery, plus guarded\n Arch package, initramfs, GRUB and conventional EFI stages after their\n transaction-specific preflights. It also provides LUKS unlock and verified\n bidirectional file recovery/copy.")
# Software centers import the desktop entry into their AppStream view. The
# packaged entry references the installed icon by absolute path so AppStream
# records a local icon: Discover 6.7.x on KIconThemes 6.28 cannot resolve stock
# theme icons (AppStreamUtils::kIconLoaderHasIcon compares icon names against
# KIconLoader::queryIcons(), which returns file paths). Source installs and the
# AppImage keep the themed icon name and are not rewritten.
set(CPACK_PRE_BUILD_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/cpack-rewrite-desktop-icon.cmake")
# Refresh the GTK icon cache right after installation so GTK-based software
# centers and file managers see the packaged hicolor icons immediately.
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/scripts/postinst")

# CPack can also produce an RPM for Fedora, openSUSE and other RPM-based
# systems.  Keep these requirements separate from the Debian dependency names;
# rpmbuild resolves the shared Qt libraries from the generated ELF metadata.
# The per-family Qt SVG package name (qt6-qtsvg on Fedora/RHEL, libQt6Svg6 on
# openSUSE) is added by scripts/package-rpm.sh because it differs per target.
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/CaptainMorgan12/Boot_Bitch")
set(CPACK_RPM_PACKAGE_NAME "boot-bitch")
set(CPACK_RPM_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Applications/System")
set(CPACK_RPM_PACKAGE_SUMMARY "${PROJECT_DESCRIPTION}")
# Same paragraph as the Debian description, without Debian's leading-space
# continuation markers (RPM keeps the newlines as written).
set(CPACK_RPM_PACKAGE_DESCRIPTION "Boot Bitch is a graphical Linux recovery and boot-repair utility.\nIt provides protected discovery and read-only diagnostics for the running\nhost or a selected repair system. The privileged backend supports guarded\nDebian/Ubuntu-family maintenance and repair-system recovery, plus guarded\nArch package, initramfs, GRUB and conventional EFI stages after their\ntransaction-specific preflights. It also provides LUKS unlock and verified\nbidirectional file recovery/copy.")
set(CPACK_RPM_PACKAGE_REQUIRES "polkit, util-linux, cryptsetup, rsync, e2fsprogs, dosfstools, btrfs-progs, xfsprogs, efibootmgr, binutils, python3, hicolor-icon-theme")
# Installed files (privileged helper, desktop entry, AppStream metadata) embed
# absolute /usr paths, so both package formats stay non-relocatable.
set(CPACK_PACKAGE_RELOCATABLE FALSE)
set(CPACK_RPM_PACKAGE_RELOCATABLE FALSE)
# rpmbuild requires a %changelog section. Generate it at configure time from
# the project version so a version bump cannot leave a stale entry behind.
string(TIMESTAMP BOOT_REPAIR_RPM_CHANGELOG_DATE "%a %b %d %Y" UTC)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/rpm-changelog"
    "* ${BOOT_REPAIR_RPM_CHANGELOG_DATE} ${CPACK_PACKAGE_CONTACT} - ${PROJECT_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}\n"
    "- Boot Bitch ${PROJECT_VERSION} release\n"
)
set(CPACK_RPM_CHANGELOG_FILE "${CMAKE_CURRENT_BINARY_DIR}/rpm-changelog")
# Refresh the GTK icon cache and desktop database after install/upgrade; RPM
# runs the scriptlet with /bin/sh.
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/scripts/rpm-postinst.sh")
# /usr/libexec is owned by the base filesystem package on RPM distributions;
# this package must not claim it.
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/libexec")
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)

# TGZ is a package-manager-neutral staging artifact for Arch, SUSE variants,
# immutable desktops and other systems where a native package recipe is not
# available.  It contains the same /usr install tree as the DEB/RPM formats.
set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)

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
        ${BOOT_REPAIR_QT_DBUS_LIBRARY}
)

if(BOOT_REPAIR_QT_DBUS_DEFINITIONS)
    target_compile_definitions(boot-repair-ui-tests PRIVATE ${BOOT_REPAIR_QT_DBUS_DEFINITIONS})
endif()

target_include_directories(boot-repair-ui-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_definitions(boot-repair-ui-tests
    PRIVATE
        BOOT_REPAIR_VERSION="${PROJECT_VERSION}"
        BOOT_REPAIR_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        # Host diagnostics normally use the interactive Polkit-backed helper.
        # The UI regression test must stay deterministic in headless CI, so
        # MainWindow uses its read-only in-process diagnostic implementation
        # for this test target only.
        BOOT_REPAIR_UI_TEST=1
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(boot-repair-ui-tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME boot-repair-ui-read-only COMMAND boot-repair-ui-tests)
add_test(NAME boot-repair-chroot-shell-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-chroot-shell-contract.sh)
add_test(NAME boot-repair-display-manager-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-display-manager-contract.sh)
add_test(NAME boot-repair-backend-profile-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-backend-profile-contract.sh)
add_test(NAME boot-repair-filesystem-repair-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-filesystem-repair-contract.sh)
add_test(NAME boot-repair-host-snapshot-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-host-snapshot-contract.sh)
add_test(NAME boot-repair-packaging-profile-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-packaging-profile-contract.sh)
add_test(NAME boot-repair-icon-atlas-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check-icon-atlas.sh)

add_test(NAME boot-repair-efi-label-update
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/tests/efi-label-update-test.py)

# -----------------------------------------------------------------------------
# Test tiers (Development/scripts/dev-check.sh drives these)
# -----------------------------------------------------------------------------
# Label map:
#   fast      - quick shell/contract gate, run per change (`ctest -L fast`)
#   ui        - MainWindow regression suite, run only when UI/UX changed
#   slow      - long-running contract test, run at checkpoints (`--full`)
#   packaging - packaging-profile contract; known trap: fails while a built
#               package sits in the local release capture
# A test may carry more than one label; `ctest -L fast` is the default quick
# gate and must stay fast (the backend-profile contract runs in the slow tier).
# The maintainer contracts (release-preparation, repository-hygiene,
# published-immutability and agent-safety) moved with the maintainer tooling
# to Development/scripts/ (never published); Development/scripts/dev-check.sh
# runs them on every tier.
set_tests_properties(boot-repair-chroot-shell-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-repair-display-manager-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-repair-filesystem-repair-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-repair-host-snapshot-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-repair-icon-atlas-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-repair-efi-label-update
    PROPERTIES LABELS "fast")
set_tests_properties(boot-repair-backend-profile-contract
    PROPERTIES LABELS "slow")
set_tests_properties(boot-repair-ui-read-only
    PROPERTIES LABELS "ui")
set_tests_properties(boot-repair-packaging-profile-contract
    PROPERTIES LABELS "packaging")
