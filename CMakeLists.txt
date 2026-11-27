cmake_minimum_required(VERSION 3.20)

project(BootBitch
    VERSION 0.2.32
    DESCRIPTION "Guarded Linux diagnostics, host maintenance and repair-system recovery utility"
    LANGUAGES C CXX
)

include(GNUInstallDirs)

# Keep install/package directory modes deterministic even when the builder uses
# a collaborative umask such as 0002. Debian package directories should not
# become group-writable merely because the source build directory is. The
# packaging wrappers (scripts/build.sh and scripts/package-*.sh) set umask 022
# for the same reason, so a restrictive builder umask cannot leak 0700/0600
# modes into a package.
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

# Minimum supported Qt is 6.4: CI pins the Ubuntu 24.04 image (Qt 6.4) as the
# compatibility gate, and the source guards Qt 6.5-only APIs behind
# QT_VERSION_CHECK so newer Qt releases keep building with the extra platform
# features.
find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Widgets)
include(CTest)
find_package(Qt6 6.4 REQUIRED COMPONENTS Test)

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
    set(BOOT_BITCH_QT_DBUS_LIBRARY Qt6::DBus)
    set(BOOT_BITCH_QT_DBUS_DEFINITIONS BOOT_BITCH_HAVE_DBUS=1)
else()
    set(BOOT_BITCH_QT_DBUS_LIBRARY)
    set(BOOT_BITCH_QT_DBUS_DEFINITIONS)
endif()

# -----------------------------------------------------------------------------
# Application target
# -----------------------------------------------------------------------------
add_executable(boot-bitch
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/SystemScanner.cpp
    src/SystemScanner.h
    src/CapabilityChecker.cpp
    src/CapabilityChecker.h
    resources/boot-bitch.qrc
)

target_link_libraries(boot-bitch
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        ${BOOT_BITCH_QT_DBUS_LIBRARY}
)

if(BOOT_BITCH_QT_DBUS_DEFINITIONS)
    target_compile_definitions(boot-bitch PRIVATE ${BOOT_BITCH_QT_DBUS_DEFINITIONS})
endif()

if(KF6Auth_FOUND)
    target_link_libraries(boot-bitch PRIVATE KF6::AuthCore)
    target_compile_definitions(boot-bitch PRIVATE BOOT_BITCH_HAVE_KAUTH=1)
else()
    message(STATUS "KF6Auth not found: building without KDE privilege integration")
endif()

# Round 17 (0.2.32 — the in-app PolkitQt1 authentication agent, OPTIONAL):
# present on Debian-family hosts with libpolkit-qt6-1-dev (the Sparky rig;
# the Debian 0.200.0 package exports the CMake package "PolkitQt6-1" with
# the targets PolkitQt6-1::Core/Agent, while upstream 0.201.0 exports
# "PolkitQt1" with PolkitQt1::Core/Agent — both carry the PolkitQt1 C++
# namespace and the same polkitqt1-* headers); the build degrades to the
# external-agent conversation when absent (e.g. Alpine, or a host without
# the dev package — the feature is runtime-gated by
# BOOT_BITCH_HAVE_POLKIT_AGENT).
find_package(PolkitQt1 QUIET)
if(PolkitQt1_FOUND)
    set(BOOT_BITCH_POLKIT_CORE_TARGET PolkitQt1::Core)
    set(BOOT_BITCH_POLKIT_AGENT_TARGET PolkitQt1::Agent)
else()
    find_package(PolkitQt6-1 QUIET)
    if(PolkitQt6-1_FOUND)
        set(BOOT_BITCH_POLKIT_CORE_TARGET PolkitQt6-1::Core)
        set(BOOT_BITCH_POLKIT_AGENT_TARGET PolkitQt6-1::Agent)
    endif()
endif()
if(DEFINED BOOT_BITCH_POLKIT_AGENT_TARGET)
    target_link_libraries(boot-bitch PRIVATE
        ${BOOT_BITCH_POLKIT_CORE_TARGET}
        ${BOOT_BITCH_POLKIT_AGENT_TARGET})
    target_compile_definitions(boot-bitch PRIVATE BOOT_BITCH_HAVE_POLKIT_AGENT=1)
    target_sources(boot-bitch PRIVATE
        src/BootBitchPolkitAgent.cpp
        src/BootBitchPolkitAgent.h
    )
else()
    message(STATUS "PolkitQt1 not found: building with the external polkit-agent conversation")
endif()

target_compile_definitions(boot-bitch
    PRIVATE
        BOOT_BITCH_VERSION="${PROJECT_VERSION}"
        BOOT_BITCH_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(boot-bitch PRIVATE -Wall -Wextra -Wpedantic)
endif()

# -----------------------------------------------------------------------------
# Install rules
# -----------------------------------------------------------------------------
install(TARGETS boot-bitch
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES data/org.bootbitch.BootBitch.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

# AppStream metadata lets KDE Discover, GNOME Software and other software
# centers associate the desktop launcher with the installed Debian package.
# Those centers can then expose their normal uninstall workflow from the
# application entry/context menu.
install(FILES data/org.bootbitch.BootBitch.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo
)

set(BOOT_BITCH_ICON_NAME "org.bootbitch.BootBitch")
set(BOOT_BITCH_ICON_SIZES 16 22 24 32 48 64 128 256 512 1024)
foreach(icon_size IN LISTS BOOT_BITCH_ICON_SIZES)
    install(FILES
        data/icons/hicolor/${icon_size}x${icon_size}/apps/${BOOT_BITCH_ICON_NAME}.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${icon_size}x${icon_size}/apps
    )
endforeach()

install(PROGRAMS scripts/boot-bitch-helper.sh
    DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/boot-bitch
    RENAME boot-bitch-helper
)

install(PROGRAMS scripts/boot-bitch-efi-label.py
    DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/boot-bitch
    RENAME boot-bitch-efi-label.py
)

# Round 24 (0.2.32, the Num Lock early-boot repair — phase 3): the build-own
# micro-binary. Built NATIVELY per family from the same source (glibc on the
# .deb/.rpm/.pkg.tar.zst builds, musl on the Alpine .apk build — the per-family
# build makes the musl/glibc loader question impossible; no helper-embedded
# payload). It is the phase-4 initramfs mechanism's payload; the diagnostic
# section (phase 2) and the packaging guards reference the install path.
add_executable(boot-bitch-numlock-on src/numlock-on.c)
target_compile_options(boot-bitch-numlock-on PRIVATE -Os)
set_target_properties(boot-bitch-numlock-on PROPERTIES
    AUTOMOC OFF
    AUTOUIC OFF
    OUTPUT_NAME numlock-on
)
install(TARGETS boot-bitch-numlock-on
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/boot-bitch
)

# Install Debian-compatible package copyright metadata. The full upstream MIT
# license remains in LICENSE at the source-tree root.
install(FILES data/copyright
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/boot-bitch
)

# Committed Qt UI translations. The .qm catalogs are generated from the
# committed .ts sources with lrelease and installed so the application can load
# them at runtime from ${CMAKE_INSTALL_DATADIR}/boot-bitch/translations.  Every
# .qm under translations/ is installed; the .ts sources are not.  The shipped
# set is exactly the 23 catalogs — the complete set, nothing outside it
# exists (the 2026-11-14 purge ruling); the rpm/apk guards assert the exact
# set.
install(DIRECTORY translations/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/boot-bitch/translations
    FILES_MATCHING PATTERN "*.qm"
)

# Polkit authorization action for the privileged helper. The
# org.freedesktop.policykit.exec.path annotation inside the file maps the
# installed helper path to a dedicated action whose <message> is shown in the
# pkexec/Polkit prompt instead of the raw command line.
install(FILES data/org.bootbitch.BootBitch.policy
    DESTINATION ${CMAKE_INSTALL_DATADIR}/polkit-1/actions
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
        COMMAND ${GZIP_EXECUTABLE} -9 -n -c ${CMAKE_CURRENT_SOURCE_DIR}/data/boot-bitch.1
        OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/boot-bitch.1.gz
        COMMAND_ERROR_IS_FATAL ANY
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/changelog.gz
        DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/boot-bitch
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/boot-bitch.1.gz
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
set(CPACK_PACKAGE_NAME "boot-bitch")
set(CPACK_PACKAGE_VENDOR "CaptainMorgan12")
set(CPACK_PACKAGE_CONTACT "CaptainMorgan12 <captainmorgan12@users.noreply.github.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Linux recovery and boot recovery utility")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Boot Bitch")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
# The package/identity rename (round A, 0.2.31): the new package identity is
# fully independent — NO Conflicts/Replaces/Obsoletes relationship to the
# OLD boot-repair name is declared in EITHER direction. The rename alone is
# the collision fix (a differently-named package is never touched by an apt
# upgrade), the two packages share no file paths so they can coexist, and
# installing the upstream boot-repair tool never removes this package (nor
# does installing this package remove the old one — the transition is a
# manual uninstall of the pre-rename package).
# Optional platform-theme integrations make Qt controls and dialogs blend
# into GNOME without overriding KDE's native Qt theme.
set(CPACK_DEBIAN_PACKAGE_SUGGESTS "qt6-gtk-platformtheme, qt6-xdgdesktopportal-platformtheme, qgnomeplatform-qt6")
# Qt loads the bundled SVG icon atlas through the SVG image-format plugin, not
# through a linked library, so the plugin package is the runtime dependency.
# Debian moved the plugin out of libqt6svg6 into qt6-svg-plugins in qt6-svg
# 6.7.2-5 (Debian trixie and newer, TUXEDO OS); bookworm and Ubuntu 24.04
# still ship it inside libqt6svg6. The versioned alternative keeps both
# installable and still forces the plugin on trixie upgrades where the old
# libqt6svg6 is already present.
set(CPACK_DEBIAN_PACKAGE_DEPENDS "pkexec, mount, cryptsetup, rsync, e2fsprogs, dosfstools, btrfs-progs, xfsprogs, efibootmgr, binutils, python3, qt6-svg-plugins | libqt6svg6 (<< 6.7.2-5~), hicolor-icon-theme")
# Round 19 (0.2.32 — the user's packaging question): the in-app PolkitQt1
# authentication agent is an OPTIONAL feature — the round-17 find_package
# block above defines BOOT_BITCH_POLKIT_AGENT_TARGET only when the dev
# package was found at configure time.  The runtime dependency on
# libpolkit-qt6-1-1 must therefore be declared only in that case: a degraded
# build's binary does not link the library, and declaring it unconditionally
# made a degraded .deb demand a package the target's repositories may not
# carry (e.g. Debian bookworm) — the install then fails with unmet
# dependencies.  The versioned constraint matches the Debian 0.200.0 API the
# round-17 find block links against (0.200.0 is the first Debian release to
# ship the PolkitQt6-1 CMake package the build consumes).
if(DEFINED BOOT_BITCH_POLKIT_AGENT_TARGET)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libpolkit-qt6-1-1 (>= 0.200.0)")
endif()
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "exfatprogs, ntfs-3g, f2fs-tools, jfsutils, reiserfsprogs, zfsutils-linux")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Boot Bitch is a graphical Linux recovery and boot recovery utility.\n It provides protected discovery and read-only diagnostics for the running\n host or a selected repair system. The privileged backend supports guarded\n Debian/Ubuntu-family maintenance and repair-system recovery, plus guarded\n Arch package, initramfs, GRUB and conventional EFI stages after their\n transaction-specific preflights. It also provides LUKS unlock and verified\n bidirectional file recovery/copy.")
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
# The auto ELF Requires pin the build host's Qt symbol version (for example
# libQt6Core.so.6(Qt_6.x)), so release RPMs are built on the oldest supported
# target distribution; scripts/package-rpm.sh documents that build-host rule.
# The per-family Qt SVG package name (qt6-qtsvg on Fedora/RHEL, libQt6Svg6 on
# openSUSE) is added by scripts/package-rpm.sh because it differs per target;
# those packages ship the SVG image-format plugin, as do Arch's qt6-svg and
# Alpine's qt6-qtsvg, so only the Debian split needs the alternative above.
# Optional file system tools stay out of weak RPM dependencies because their
# package names differ per RPM family; the Debian Recommends and the Arch/
# Alpine optdepends carry the optional contract instead.
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/CaptainMorgan12/Boot_Bitch")
set(CPACK_RPM_PACKAGE_NAME "boot-bitch")
set(CPACK_RPM_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_RPM_PACKAGE_RELEASE "1")
# The package/identity rename (round A, 0.2.31): fully independent identity —
# no Conflicts/Obsoletes against the old boot-repair name (see the deb-side
# comment: the rename alone is the collision fix, the transition is manual).
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Applications/System")
set(CPACK_RPM_PACKAGE_SUMMARY "${PROJECT_DESCRIPTION}")
# Same paragraph as the Debian description, without Debian's leading-space
# continuation markers (RPM keeps the newlines as written).
set(CPACK_RPM_PACKAGE_DESCRIPTION "Boot Bitch is a graphical Linux recovery and boot recovery utility.\nIt provides protected discovery and read-only diagnostics for the running\nhost or a selected repair system. The privileged backend supports guarded\nDebian/Ubuntu-family maintenance and repair-system recovery, plus guarded\nArch package, initramfs, GRUB and conventional EFI stages after their\ntransaction-specific preflights. It also provides LUKS unlock and verified\nbidirectional file recovery/copy.")
# Round 19 (0.2.32): the in-app PolkitQt1 agent's client library
# (polkit-qt6-1) is declared ONLY when the optional agent feature built —
# the same condition as the Debian Depends above; a degraded RPM build
# links no polkit-qt6 libraries, so the auto ELF Requires stay silent and
# an explicit unconditional entry would demand a package the target may
# not carry (and the Fedora name does not exist on openSUSE).
set(CPACK_RPM_PACKAGE_REQUIRES "polkit, util-linux, cryptsetup, rsync, e2fsprogs, dosfstools, btrfs-progs, xfsprogs, efibootmgr, binutils, python3, hicolor-icon-theme")
if(DEFINED BOOT_BITCH_POLKIT_AGENT_TARGET)
    set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES}, polkit-qt6-1")
endif()
# Installed files (privileged helper, desktop entry, AppStream metadata) embed
# absolute /usr paths, so both package formats stay non-relocatable.
set(CPACK_PACKAGE_RELOCATABLE FALSE)
set(CPACK_RPM_PACKAGE_RELOCATABLE FALSE)
# rpmbuild requires a %changelog section. Generate it at configure time from
# the project version so a version bump cannot leave a stale entry behind.
string(TIMESTAMP BOOT_BITCH_RPM_CHANGELOG_DATE "%a %b %d %Y" UTC)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/rpm-changelog"
    "* ${BOOT_BITCH_RPM_CHANGELOG_DATE} ${CPACK_PACKAGE_CONTACT} - ${PROJECT_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}\n"
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
# MainWindowUiTest.cpp was 895 KB in a single translation unit, so the suite
# is split into several translation units (MainWindowUiTest.cpp plus the
# MainWindowUiTest_<feature>.cpp files) that compile in parallel.  They share
# one QTest class (tests/MainWindowUiTest.h) and one fixture/helper header
# (tests/UiTestHelpers.h); the single registered test boot-bitch-ui-read-only
# is unchanged.
enable_testing()

add_executable(boot-bitch-ui-tests
    tests/MainWindowUiTest.cpp
    tests/MainWindowUiTest.h
    tests/MainWindowUiTest_Logs.cpp
    tests/MainWindowUiTest_Scope.cpp
    tests/MainWindowUiTest_Plan.cpp
    tests/MainWindowUiTest_Shell.cpp
    tests/MainWindowUiTest_BootEntries.cpp
    tests/MainWindowUiTest_Capabilities.cpp
    tests/MainWindowUiTest_Tabs.cpp
    tests/UiTestHelpers.h
    src/MainWindow.cpp
    src/MainWindow.h
    src/SystemScanner.cpp
    src/SystemScanner.h
    src/CapabilityChecker.cpp
    src/CapabilityChecker.h
    resources/boot-bitch.qrc
)

target_link_libraries(boot-bitch-ui-tests
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Test
        ${BOOT_BITCH_QT_DBUS_LIBRARY}
)

if(BOOT_BITCH_QT_DBUS_DEFINITIONS)
    target_compile_definitions(boot-bitch-ui-tests PRIVATE ${BOOT_BITCH_QT_DBUS_DEFINITIONS})
endif()

target_include_directories(boot-bitch-ui-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_definitions(boot-bitch-ui-tests
    PRIVATE
        BOOT_BITCH_VERSION="${PROJECT_VERSION}"
        BOOT_BITCH_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        # Host diagnostics normally use the interactive Polkit-backed helper.
        # The UI regression test must stay deterministic in headless CI, so
        # MainWindow uses its read-only in-process diagnostic implementation
        # for this test target only.
        BOOT_BITCH_UI_TEST=1
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(boot-bitch-ui-tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME boot-bitch-ui-read-only COMMAND boot-bitch-ui-tests)
# Fast-tier unit tests for the three boot-menu protocol parsers (efi-list,
# grub-list and grub-legacy-list): run the parser slots of the shared UI-test
# binary in one ctest case (no window, no privileged helper).
add_test(NAME boot-bitch-boot-menu-parsers
    COMMAND boot-bitch-ui-tests bootEntriesParserParsesProtocol grubMenuParserParsesProtocol grubLegacyParserParsesProtocol)
add_test(NAME boot-bitch-chroot-shell-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-chroot-shell-contract.sh)
add_test(NAME boot-bitch-display-manager-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-display-manager-contract.sh)
add_test(NAME boot-bitch-backend-profile-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-backend-profile-contract.sh)
add_test(NAME boot-bitch-filesystem-repair-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-filesystem-repair-contract.sh)
add_test(NAME boot-bitch-host-snapshot-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-host-snapshot-contract.sh)
add_test(NAME boot-bitch-host-default-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-host-default-contract.sh)
add_test(NAME boot-bitch-reason-key-registry-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-reason-key-registry-contract.sh)
add_test(NAME boot-bitch-msg-key-registry-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-msg-key-registry-contract.sh)
add_test(NAME boot-bitch-kernels-cleanup-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-kernels-cleanup-contract.sh)
add_test(NAME boot-bitch-gpu-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-gpu-contract.sh)
add_test(NAME boot-bitch-baseline-contract
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-baseline-contract.sh)
add_test(NAME boot-bitch-boot-errors-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-boot-errors-contract.sh)
add_test(NAME boot-bitch-efi-entry-policy-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-efi-entry-policy-contract.sh)
add_test(NAME boot-bitch-repair-log-filter-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-repair-log-filter-contract.sh)
add_test(NAME boot-bitch-network-conflict-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-network-conflict-contract.sh)
add_test(NAME boot-bitch-packaging-profile-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-packaging-profile-contract.sh)
add_test(NAME boot-bitch-icon-atlas-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check-icon-atlas.sh)
add_test(NAME boot-bitch-appimage-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-appimage-contract.sh)
# Legacy (Debian Etch / bash 3.1) helper port: test-registration-only edit.
# The legacy tree is generated from the modern helper by legacy/port.sh; these
# tests keep the port deterministic and the modern helper untouched.
add_test(NAME boot-bitch-legacy-port-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/legacy/tests/test-port-contract.sh)
add_test(NAME boot-bitch-legacy-smoke
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/legacy/tests/test-smoke.sh)
add_test(NAME boot-bitch-legacy-package-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-legacy-package-contract.sh)
add_test(NAME boot-bitch-legacy-gui-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/legacy/tests/test-gui-contract.sh)
add_test(NAME boot-bitch-legacy-kernel-parity-contract
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/legacy/tests/test-kernel-parity-contract.sh)

add_test(NAME boot-bitch-efi-label-update
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
set_tests_properties(boot-bitch-chroot-shell-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-display-manager-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-filesystem-repair-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-host-snapshot-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-host-default-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-reason-key-registry-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-msg-key-registry-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-kernels-cleanup-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-gpu-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-baseline-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-boot-errors-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-efi-entry-policy-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-repair-log-filter-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-network-conflict-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-icon-atlas-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-appimage-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-legacy-package-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-legacy-gui-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-efi-label-update
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-legacy-port-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-legacy-smoke
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-legacy-kernel-parity-contract
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-backend-profile-contract
    PROPERTIES LABELS "slow")
set_tests_properties(boot-bitch-ui-read-only
    PROPERTIES LABELS "ui")
set_tests_properties(boot-bitch-boot-menu-parsers
    PROPERTIES LABELS "fast")
set_tests_properties(boot-bitch-packaging-profile-contract
    PROPERTIES LABELS "packaging")

# -----------------------------------------------------------------------------
# Translation tooling (dev-only, explicitly invoked)
# -----------------------------------------------------------------------------
# qt_add_translations is deliberately not used: it needs qttools6-dev, which is
# absent from the CI image. lupdate/lrelease are located at configure time and
# invoked directly, and the generated .qm catalogs are committed so a default
# source build or CI run never depends on the translation tools. The target is
# not part of ALL, so it runs only when explicitly requested. The Qt6 tool
# locations are searched first because distro /usr/bin wrappers (qtchooser) can
# resolve to an absent Qt5 tool on a Qt6-only host.
find_program(BOOT_BITCH_LUPDATE_EXECUTABLE lupdate
    HINTS /usr/lib/qt6/bin /usr/lib64/qt6/bin)
find_program(BOOT_BITCH_LRELEASE_EXECUTABLE lrelease
    HINTS /usr/lib/qt6/bin /usr/lib64/qt6/bin)

if(BOOT_BITCH_LUPDATE_EXECUTABLE AND BOOT_BITCH_LRELEASE_EXECUTABLE)
    # The COMPLETE language set — the shipped 23 (the 2026-11-14 purge
    # ruling: nothing outside this set exists; bare Qt locale codes; "no" is
    # Norwegian Bokmål, which Argos packages as "nb"). Keep this in sync
    # with the packaging guards (scripts/test-rpm.sh / scripts/test-apk.sh),
    # SUPPORTED_TARGETS in tools/translate_ts.py and the catalog-inventory
    # table in Development/docs/translation.md.
    set(BOOT_BITCH_TRANSLATION_TS)
    foreach(lang IN ITEMS de fr es it pt nl pl ru zh ja ko sv da fi no nb
                          cs hu ro tr el ar uk)
        list(APPEND BOOT_BITCH_TRANSLATION_TS
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/boot-bitch_${lang}.ts)
    endforeach()

    add_custom_target(update_translations
        COMMAND ${BOOT_BITCH_LUPDATE_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.h
            ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
            -ts
            ${BOOT_BITCH_TRANSLATION_TS}
        COMMAND ${BOOT_BITCH_LRELEASE_EXECUTABLE}
            ${BOOT_BITCH_TRANSLATION_TS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/translations
        COMMENT "Updating Boot Bitch UI translation sources (.ts) and compiling catalogs (.qm)"
        VERBATIM
    )
else()
    message(STATUS "lupdate/lrelease not found: update_translations target unavailable")
endif()
