#!/usr/bin/env bash
set -euo pipefail

# Contract test: packaging scripts, dependency lists, desktop/AppStream metadata
# and install paths stay consistent across the supported profiles.

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"

for script in setup-dev-deps.sh check-dev-env.sh build.sh package-deb.sh package-rpm.sh test-rpm.sh package-arch.sh package-tarball.sh install.sh; do
    [[ -x "$ROOT_DIR/scripts/$script" ]] || {
        echo "FAIL: packaging script is not executable: $script" >&2
        exit 1
    }
    bash -n "$ROOT_DIR/scripts/$script"
done

grep -q "pacman -Syu --needed" "$ROOT_DIR/scripts/setup-dev-deps.sh"
grep -q "apt-get install --no-install-recommends" "$ROOT_DIR/scripts/setup-dev-deps.sh"
grep -q 'zypper --non-interactive install' "$ROOT_DIR/scripts/setup-dev-deps.sh"
grep -q 'dnf install -y' "$ROOT_DIR/scripts/setup-dev-deps.sh"

# Every setup profile installs the hard filesystem check tools and collects the
# optional ones that the distribution actually ships.
grep -q 'e2fsprogs dosfstools' "$ROOT_DIR/scripts/setup-dev-deps.sh"
grep -q 'btrfs-progs xfsprogs' "$ROOT_DIR/scripts/setup-dev-deps.sh"
grep -q 'btrfsprogs xfsprogs' "$ROOT_DIR/scripts/setup-dev-deps.sh"
grep -q 'exfatprogs ntfs-3g ntfsprogs f2fs-tools jfsutils reiserfsprogs zfsutils-linux' \
    "$ROOT_DIR/scripts/setup-dev-deps.sh"

# Debian control metadata: hard Depends and optional Recommends.
grep -q 'e2fsprogs, dosfstools, btrfs-progs, xfsprogs' "$ROOT_DIR/CMakeLists.txt"
grep -q 'CPACK_DEBIAN_PACKAGE_RECOMMENDS "exfatprogs, ntfs-3g, f2fs-tools, jfsutils, reiserfsprogs, zfsutils-linux"' \
    "$ROOT_DIR/CMakeLists.txt"
grep -q 'CPACK_RPM_PACKAGE_REQUIRES' "$ROOT_DIR/CMakeLists.txt"
grep -q 'e2fsprogs, dosfstools, .*xfsprogs' "$ROOT_DIR/scripts/package-rpm.sh"

# RPM metadata: URL/summary/description, non-relocatable layout, a generated
# changelog, the post-install cache-refresh scriptlet and the /usr/libexec
# ownership exception used by RPM distributions.
grep -q 'CPACK_PACKAGE_HOMEPAGE_URL' "$ROOT_DIR/CMakeLists.txt"
grep -q 'github.com/CaptainMorgan12/Boot_Bitch' "$ROOT_DIR/CMakeLists.txt"
grep -q 'CPACK_RPM_PACKAGE_SUMMARY.*PROJECT_DESCRIPTION' "$ROOT_DIR/CMakeLists.txt"
grep -q 'CPACK_RPM_PACKAGE_DESCRIPTION' "$ROOT_DIR/CMakeLists.txt"
grep -q 'CPACK_PACKAGE_RELOCATABLE FALSE' "$ROOT_DIR/CMakeLists.txt"
grep -q 'CPACK_RPM_PACKAGE_RELOCATABLE FALSE' "$ROOT_DIR/CMakeLists.txt"
grep -q 'CPACK_RPM_CHANGELOG_FILE' "$ROOT_DIR/CMakeLists.txt"
grep -q 'rpm-changelog' "$ROOT_DIR/CMakeLists.txt"
grep -q 'CPACK_RPM_POST_INSTALL_SCRIPT_FILE.*scripts/rpm-postinst.sh' "$ROOT_DIR/CMakeLists.txt"
grep -q 'CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/libexec"' "$ROOT_DIR/CMakeLists.txt"
grep -q 'RPM-DEFAULT' "$ROOT_DIR/CMakeLists.txt"

# The RPM wrapper keeps the family-specific dependency names (btrfs-progs vs
# btrfsprogs, Qt SVG runtime) and always passes the complete requirement list
# to CPack.
grep -q 'btrfsprogs' "$ROOT_DIR/scripts/package-rpm.sh"
grep -q 'qt6-qtsvg' "$ROOT_DIR/scripts/package-rpm.sh"
grep -q 'libQt6Svg6' "$ROOT_DIR/scripts/package-rpm.sh"
grep -q 'CPACK_RPM_PACKAGE_REQUIRES=' "$ROOT_DIR/scripts/package-rpm.sh"

# AppStream metadata and the desktop entry must stay in sync with the packaged
# application: software centers need remote screenshots, the current release
# version and the package association to list the installed .deb.
project_version="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\{1,\}\([0-9][0-9.]*\)[[:space:]]*$/\1/p' \
    "$ROOT_DIR/CMakeLists.txt" | head -n1)"
[[ -n "$project_version" ]] || {
    echo 'FAIL: could not read the project version from CMakeLists.txt' >&2
    exit 1
}
grep -q "<release version=\"$project_version\" date=\"" \
    "$ROOT_DIR/data/org.bootrepair.BootRepair.metainfo.xml"
grep -q '<screenshots>' "$ROOT_DIR/data/org.bootrepair.BootRepair.metainfo.xml"
grep -q '<image type="source">https://raw.githubusercontent.com/CaptainMorgan12/Boot_Bitch/main/docs/screenshots/' \
    "$ROOT_DIR/data/org.bootrepair.BootRepair.metainfo.xml"
grep -q '<pkgname>boot-repair</pkgname>' "$ROOT_DIR/data/org.bootrepair.BootRepair.metainfo.xml"
grep -q '<launchable type="desktop-id">org.bootrepair.BootRepair.desktop</launchable>' \
    "$ROOT_DIR/data/org.bootrepair.BootRepair.metainfo.xml"
grep -q '^Icon=org.bootrepair.BootRepair$' "$ROOT_DIR/data/org.bootrepair.BootRepair.desktop"
grep -q '^StartupWMClass=BootRepair$' "$ROOT_DIR/data/org.bootrepair.BootRepair.desktop"

# Packaged desktop entries must hand software centers an installed local icon.
# Discover 6.7.x on KIconThemes 6.28 cannot resolve stock theme icons, so the
# package rewrites the staged desktop entry and refreshes the GTK icon cache.
grep -q 'CPACK_PRE_BUILD_SCRIPTS.*cpack-rewrite-desktop-icon.cmake' "$ROOT_DIR/CMakeLists.txt"
grep -q 'CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA.*scripts/postinst' "$ROOT_DIR/CMakeLists.txt"
grep -q 'gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor' "$ROOT_DIR/scripts/postinst"

# The RPM post-install scriptlet is executed by /bin/sh, so it must stay
# POSIX and refresh both the icon cache and the desktop database.
[[ -x "$ROOT_DIR/scripts/rpm-postinst.sh" ]] || {
    echo 'FAIL: RPM post-install scriptlet is not executable.' >&2
    exit 1
}
sh -n "$ROOT_DIR/scripts/rpm-postinst.sh"
grep -q '^#!/bin/sh$' "$ROOT_DIR/scripts/rpm-postinst.sh"
grep -q 'gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor' "$ROOT_DIR/scripts/rpm-postinst.sh"
grep -q 'update-desktop-database -q /usr/share/applications' "$ROOT_DIR/scripts/rpm-postinst.sh"
if grep -q '\[\[' "$ROOT_DIR/scripts/rpm-postinst.sh"; then
    echo 'FAIL: RPM post-install scriptlet contains a bash-only [[ test.' >&2
    exit 1
fi

rewrite_dir="$(mktemp -d)"
trap 'rm -rf "$rewrite_dir"' EXIT
mkdir -p "$rewrite_dir/usr/share/applications"
cp "$ROOT_DIR/data/org.bootrepair.BootRepair.desktop" \
    "$rewrite_dir/usr/share/applications/org.bootrepair.BootRepair.desktop"
cmake -DCPACK_TEMPORARY_DIRECTORY="$rewrite_dir" -DCPACK_PACKAGING_INSTALL_PREFIX=/usr \
    -P "$ROOT_DIR/scripts/cpack-rewrite-desktop-icon.cmake" >/dev/null
grep -q '^Icon=/usr/share/icons/hicolor/512x512/apps/org.bootrepair.BootRepair.png$' \
    "$rewrite_dir/usr/share/applications/org.bootrepair.BootRepair.desktop" || {
    echo 'FAIL: CPack desktop icon rewrite did not produce the installed icon path.' >&2
    exit 1
}

# Arch PKGBUILD metadata: hard depends and optdepends naming each filesystem.
grep -q "'e2fsprogs' 'dosfstools' 'btrfs-progs' 'xfsprogs'" "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'optdepends=' "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'exfatprogs: exFAT file system check and repair' "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'ntfs-3g: NTFS mount and read-write support' "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'ntfsprogs: NTFS file system check and dirty-state repair' "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'f2fs-tools: F2FS file system repair' "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'jfsutils: JFS file system check and repair' "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'reiserfsprogs: ReiserFS file system check and repair' "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'zfsutils-linux: ZFS pool status and scrub' "$ROOT_DIR/scripts/package-arch.sh"

# Alpine APKBUILD metadata: the elogind Polkit build is required because the
# plain polkit package pulls the ConsoleKit libraries and cannot register a
# session authentication agent (pkexec then falls back to a textual agent and
# fails without a controlling terminal). The helper also uses GNU coreutils and
# findutils at runtime, and a session Polkit agent is documented as an optional
# dependency because pkexec cannot show an authorization prompt without one.
grep -q "depends='qt6-qtbase qt6-qtsvg polkit-elogind coreutils findutils" \
    "$ROOT_DIR/scripts/package-alpine.sh"
grep -q 'xfce-polkit: .*authentication agent.*pkexec' "$ROOT_DIR/scripts/package-alpine.sh"
grep -q 'polkit-gnome' "$ROOT_DIR/scripts/package-alpine.sh"
grep -q 'host_is_debian' "$ROOT_DIR/scripts/build.sh"
grep -q 'Debian-family packaging hosts' "$ROOT_DIR/scripts/package-deb.sh"
grep -q 'cpack -G RPM' "$ROOT_DIR/scripts/package-rpm.sh"
grep -q 'test-rpm.sh' "$ROOT_DIR/scripts/package-rpm.sh"

# The RPM validation script inspects, extracts and lints the artifact without
# installing it.
grep -q 'rpm -qpi' "$ROOT_DIR/scripts/test-rpm.sh"
grep -q 'rpm -qpl' "$ROOT_DIR/scripts/test-rpm.sh"
grep -q 'rpm -qpR' "$ROOT_DIR/scripts/test-rpm.sh"
grep -q 'rpm2cpio' "$ROOT_DIR/scripts/test-rpm.sh"
grep -q 'rpmkeys -K' "$ROOT_DIR/scripts/test-rpm.sh"
grep -q 'rpmlint' "$ROOT_DIR/scripts/test-rpm.sh"
grep -q 'python3 -m py_compile' "$ROOT_DIR/scripts/test-rpm.sh"
grep -q 'sudo apt install rpm rpmlint' "$ROOT_DIR/scripts/test-rpm.sh"
grep -q 'makepkg' "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'options=(!debug)' "$ROOT_DIR/scripts/package-arch.sh"
# The generated PKGBUILD references an install script that refreshes the
# hicolor icon cache and the desktop database on install and upgrade so
# launchers resolve the packaged icon. makepkg only embeds the hooks when the
# PKGBUILD declares install=.
grep -q '^install=boot-bitch.install$' "$ROOT_DIR/scripts/package-arch.sh"
grep -q '^post_install()' "$ROOT_DIR/scripts/package-arch.sh"
grep -q '^post_upgrade()' "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor' "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'update-desktop-database -q /usr/share/applications' "$ROOT_DIR/scripts/package-arch.sh"
grep -q -- '--source' "$ROOT_DIR/scripts/install.sh"
grep -q 'pacman -U' "$ROOT_DIR/scripts/install.sh"
grep -q 'zypper --non-interactive install' "$ROOT_DIR/scripts/install.sh"
grep -q 'boot-repair-source-install-manifest.txt' "$ROOT_DIR/scripts/install.sh"
grep -q 'INSTALL_MANIFEST' "$ROOT_DIR/scripts/uninstall.sh"

# Exercise install.sh's no-package path without building or invoking a package
# manager. This checks that an Arch/SUSE host receives the native-package and
# source-install guidance instead of an unconditional apt-get failure.
output="$(env BOOT_REPAIR_PACKAGE_FAMILY=arch PATH=/usr/bin:/bin "$ROOT_DIR/scripts/install.sh" 2>&1 || true)"
grep -q 'package-arch.sh' <<<"$output"
grep -q -- '--source' <<<"$output"

# install.sh reports missing filesystem check tools (and the filesystems they
# cover) as a non-fatal warning. Stub availability so the outcome does not
# depend on the tools installed on the test host.
grep -q '^report_filesystem_check_tools()' "$ROOT_DIR/scripts/install.sh"
grep -q '^report_filesystem_check_tools$' "$ROOT_DIR/scripts/install.sh"
preflight_source="$(sed -n '/^report_filesystem_check_tools()/,/^}/p' "$ROOT_DIR/scripts/install.sh")"
[[ -n "$preflight_source" ]] || {
    echo 'FAIL: install.sh filesystem check tool preflight is missing' >&2
    exit 1
}
missing_output="$(
    filesystem_check_tool_available() { return 1; }
    eval "$preflight_source"
    report_filesystem_check_tools
)"
grep -q 'WARNING: Missing file system check tools:.*e2fsck' <<<"$missing_output"
grep -q 'WARNING: File systems that cannot be checked without them:.*ext2/ext3/ext4' <<<"$missing_output"
grep -q 'WARNING: Missing file system check tools:.*zpool' <<<"$missing_output"
grep -q 'WARNING: File systems that cannot be checked without them:.*zfs' <<<"$missing_output"
present_output="$(
    filesystem_check_tool_available() { return 0; }
    eval "$preflight_source"
    report_filesystem_check_tools
)"
[[ -z "$present_output" ]] || {
    echo 'FAIL: install.sh warned although every filesystem check tool is available' >&2
    exit 1
}

echo "PASS: package-manager-aware build, native-package and source-install contracts are wired."
