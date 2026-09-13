#!/usr/bin/env bash
set -euo pipefail

if ! command -v apt-get >/dev/null 2>&1; then
    echo "This one-command installer currently supports Debian/Ubuntu/TUXEDO-family build hosts." >&2
    echo "See README.md for the dependency list on other distributions." >&2
    exit 2
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

base_packages=(
    build-essential
    cmake
    ninja-build
    pkg-config
    qt6-base-dev
    qt6-base-dev-tools
    extra-cmake-modules
    dpkg-dev
    desktop-file-utils
    lintian
    pkexec
    util-linux
    mount
    rsync
    cryptsetup
    btrfs-progs
    systemd
    efibootmgr
    binutils
    lvm2
    mdadm
)

"${SUDO[@]}" apt-get update

optional_packages=()
if apt-cache show libkf6auth-dev >/dev/null 2>&1; then
    optional_packages+=(libkf6auth-dev)
fi

"${SUDO[@]}" apt-get install --no-install-recommends -y "${base_packages[@]}" "${optional_packages[@]}"

printf '\nDependencies installed. Verifying environment...\n\n'
"$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)/check-dev-env.sh"
