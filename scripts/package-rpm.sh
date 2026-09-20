#!/usr/bin/env bash
set -euo pipefail

# Build the RPM package on an RPM-family host (Fedora/RHEL/openSUSE).
#
# Environment: BUILD_DIR (default Development/build-rpm). The application is
# always built as a Release build.
#
# The generated RPM carries auto ELF Requires/Provides plus the explicit
# package list below. The Qt SVG image format plugin is loaded at runtime, so
# its package is added explicitly and named per RPM family.

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/Development/build-rpm}"

rpm_family=other
if [[ -r /etc/os-release ]]; then
    . /etc/os-release
    case "${ID:-}" in
        fedora|rhel|centos|rocky|almalinux) rpm_family=fedora ;;
        opensuse*|suse|sles) rpm_family=suse ;;
        *)
            case " ${ID_LIKE:-} " in
                *" fedora "*|*" rhel "*) rpm_family=fedora ;;
                *" suse "*|*" opensuse "*) rpm_family=suse ;;
            esac
            ;;
    esac
fi

rpm_btrfs_package=btrfs-progs
rpm_qt_svg_package=""
case "$rpm_family" in
    fedora)
        rpm_qt_svg_package=qt6-qtsvg
        ;;
    suse)
        rpm_btrfs_package=btrfsprogs
        rpm_qt_svg_package=libQt6Svg6
        ;;
esac

rpm_requires="polkit, util-linux, cryptsetup, rsync, e2fsprogs, dosfstools, ${rpm_btrfs_package}, xfsprogs, efibootmgr, binutils, python3, hicolor-icon-theme"
if [[ -n "$rpm_qt_svg_package" ]]; then
    rpm_requires+=", $rpm_qt_svg_package"
fi

for command in cpack rpmbuild; do
    command -v "$command" >/dev/null 2>&1 || {
        echo "Missing required RPM packaging command: $command" >&2
        echo "On Fedora/RHEL:   sudo dnf install cmake rpm-build" >&2
        echo "On openSUSE:      sudo zypper install cmake rpm-build" >&2
        echo "On Debian/Ubuntu: sudo apt install cmake rpm (local RPM builds/validation only)" >&2
        exit 1
    }
done

BUILD_TYPE=Release BUILD_DIR="$BUILD_DIR" "$ROOT_DIR/scripts/build.sh"

(
    cd "$BUILD_DIR"
    cpack -G RPM --config CPackConfig.cmake \
        -D "CPACK_RPM_PACKAGE_REQUIRES=${rpm_requires}"
)

mapfile -t packages < <(
    find "$BUILD_DIR" -maxdepth 1 -type f -name '*.rpm' -print | sort
)
[[ ${#packages[@]} -gt 0 ]] || {
    echo "CPack ran but no .rpm was generated." >&2
    exit 1
}

echo
echo "RPM package created:"
printf '  %s\n' "${packages[@]}"
echo
echo "Validate without installing:"
echo "  $ROOT_DIR/scripts/test-rpm.sh ${packages[0]}"
echo
echo "Nothing was installed on the host. Install with the distribution's RPM frontend, for example:"
echo "  sudo dnf install ${packages[0]}"
echo "  sudo zypper install ${packages[0]}"
