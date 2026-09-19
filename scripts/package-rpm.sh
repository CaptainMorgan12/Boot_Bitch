#!/usr/bin/env bash
set -euo pipefail

# Build the RPM package on an RPM-family host (Fedora/RHEL/openSUSE).

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/Development/build-rpm}"

rpm_btrfs_package=btrfs-progs
if [[ -r /etc/os-release ]]; then
    . /etc/os-release
    case "${ID:-}" in
        opensuse*|suse|sles) rpm_btrfs_package=btrfsprogs ;;
    esac
fi

for command in cpack rpmbuild; do
    command -v "$command" >/dev/null 2>&1 || {
        echo "Missing required RPM packaging command: $command" >&2
        echo "Install rpm-build and CMake's CPack module with setup-dev-deps.sh." >&2
        exit 1
    }
done

BUILD_TYPE=Release BUILD_DIR="$BUILD_DIR" "$ROOT_DIR/scripts/build.sh"

(
    cd "$BUILD_DIR"
    cpack -G RPM --config CPackConfig.cmake \
        -D "CPACK_RPM_PACKAGE_REQUIRES=polkit, util-linux, cryptsetup, rsync, e2fsprogs, dosfstools, ${rpm_btrfs_package}, xfsprogs, efibootmgr, binutils, python3, hicolor-icon-theme"
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
echo "Nothing was installed on the host. Install with the distribution's RPM frontend, for example:"
echo "  sudo dnf install ${packages[0]}"
echo "  sudo zypper install ${packages[0]}"
