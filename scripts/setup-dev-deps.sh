#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"

if [[ -r /etc/os-release ]]; then
    . /etc/os-release
fi

if [[ ${EUID:-$(id -u)} -eq 0 ]]; then
    SUDO=()
else
    command -v sudo >/dev/null 2>&1 || {
        echo "sudo is required when this script is not run as root." >&2
        exit 1
    }
    SUDO=(sudo)
fi

family=unknown
case "${ID:-}" in
    debian|ubuntu|tuxedo|linuxmint|pop|elementary|zorin) family=debian ;;
    arch|manjaro|endeavouros|garuda|artix) family=arch ;;
    fedora|rhel|rocky|almalinux) family=rpm ;;
    opensuse*|suse|sles) family=suse ;;
    *)
        if [[ " ${ID_LIKE:-} " == *" debian "* || " ${ID_LIKE:-} " == *" ubuntu "* ]]; then
            family=debian
        elif [[ " ${ID_LIKE:-} " == *" arch "* ]]; then
            family=arch
        elif [[ " ${ID_LIKE:-} " == *" fedora "* || " ${ID_LIKE:-} " == *" rhel "* ]]; then
            family=rpm
        elif [[ " ${ID_LIKE:-} " == *" suse "* ]]; then
            family=suse
        fi
        ;;
esac

if [[ "$family" == debian && -z "$(command -v apt-get || true)" ]]; then
    family=unknown
elif [[ "$family" == arch && -z "$(command -v pacman || true)" ]]; then
    family=unknown
elif [[ "$family" == rpm && -z "$(command -v dnf || true)" ]]; then
    family=unknown
elif [[ "$family" == suse && -z "$(command -v zypper || true)" ]]; then
    family=unknown
fi

if [[ "$family" == unknown ]]; then
    echo "Unable to select a supported package-manager profile for this build host." >&2
    echo "Supported setup profiles: APT/dpkg, pacman, DNF/RPM, and zypper/RPM." >&2
    echo "Install the packages listed in README.md, then run: $ROOT_DIR/scripts/check-dev-env.sh" >&2
    exit 2
fi

case "$family" in
debian)
    base_packages=(
        build-essential cmake ninja-build pkg-config qt6-base-dev
        qt6-base-dev-tools extra-cmake-modules dpkg-dev desktop-file-utils
        lintian pkexec util-linux mount rsync cryptsetup btrfs-progs systemd
        efibootmgr binutils lvm2 mdadm
    )

    "${SUDO[@]}" apt-get update

    optional_packages=()
    if apt-cache show libkf6auth-dev >/dev/null 2>&1; then
        optional_packages+=(libkf6auth-dev)
    fi
    for package in qt6-gtk-platformtheme qt6-xdgdesktopportal-platformtheme qgnomeplatform-qt6; do
        if apt-cache show "$package" >/dev/null 2>&1; then
            optional_packages+=("$package")
        fi
    done

    "${SUDO[@]}" apt-get install --no-install-recommends -y "${base_packages[@]}" "${optional_packages[@]}"
    ;;
arch)
    # Never run -Sy alone. A full upgrade keeps the package database and
    # installed libraries in sync on Arch-family systems.
    base_packages=(
        base-devel cmake ninja pkgconf qt6-base qt6-tools extra-cmake-modules
        desktop-file-utils appstream polkit util-linux rsync cryptsetup
        btrfs-progs efibootmgr binutils python hicolor-icon-theme
        lvm2 mdadm
    )
    if [[ "${ID:-}" != artix && " ${ID_LIKE:-} " != *" artix "* ]]; then
        base_packages+=(systemd)
    fi
    "${SUDO[@]}" pacman -Syu --needed "${base_packages[@]}"
    ;;
rpm)
    base_packages=(
        gcc-c++ cmake ninja-build pkgconf-pkg-config qt6-qtbase-devel
        extra-cmake-modules desktop-file-utils appstream polkit util-linux
        rsync cryptsetup btrfs-progs systemd efibootmgr binutils python3
        hicolor-icon-theme lvm2 mdadm rpm-build
    )
    "${SUDO[@]}" dnf install -y "${base_packages[@]}"
    ;;
suse)
    base_packages=(
        gcc-c++ cmake ninja pkg-config libqt6-qtbase-devel
        extra-cmake-modules desktop-file-utils appstream polkit util-linux
        rsync cryptsetup btrfsprogs systemd efibootmgr binutils python3
        hicolor-icon-theme lvm2 mdadm rpm-build
    )
    "${SUDO[@]}" zypper --non-interactive install --no-recommends "${base_packages[@]}"
    ;;
esac

printf '\nDependencies installed. Verifying environment...\n\n'
"$ROOT_DIR/scripts/check-dev-env.sh"
