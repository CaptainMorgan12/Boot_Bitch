#!/usr/bin/env bash
set -u

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
failures=0
optional_missing=0

host_family=unknown
host_package_manager=unknown
if [[ -r /etc/os-release ]]; then
    . /etc/os-release
    case "${ID:-}" in
        debian|ubuntu|tuxedo|linuxmint|pop|elementary|zorin) host_family=debian; host_package_manager='APT / dpkg' ;;
        arch|manjaro|endeavouros|garuda|artix) host_family=arch; host_package_manager=pacman ;;
        fedora|rhel|rocky|almalinux) host_family=rpm; host_package_manager='DNF / RPM' ;;
        opensuse*|suse|sles) host_family=suse; host_package_manager='zypper / RPM' ;;
        *)
            if [[ " ${ID_LIKE:-} " == *" debian "* || " ${ID_LIKE:-} " == *" ubuntu "* ]]; then
                host_family=debian; host_package_manager='APT / dpkg'
            elif [[ " ${ID_LIKE:-} " == *" arch "* ]]; then
                host_family=arch; host_package_manager=pacman
            elif [[ " ${ID_LIKE:-} " == *" fedora "* || " ${ID_LIKE:-} " == *" rhel "* ]]; then
                host_family=rpm; host_package_manager='DNF / RPM'
            elif [[ " ${ID_LIKE:-} " == *" suse "* ]]; then
                host_family=suse; host_package_manager='zypper / RPM'
            fi
            ;;
    esac
fi

check_cmd()
{
    local label="$1" cmd="$2" required="${3:-yes}"
    if command -v "$cmd" >/dev/null 2>&1; then
        printf 'PASS  %-26s %s\n' "$label" "$(command -v "$cmd")"
    else
        if [[ "$required" == yes ]]; then
            printf 'FAIL  %-26s missing (%s)\n' "$label" "$cmd"
            failures=$((failures + 1))
        else
            printf 'OPT   %-26s missing (%s)\n' "$label" "$cmd"
            optional_missing=$((optional_missing + 1))
        fi
    fi
}

printf '%s\n' '========================================'
printf '%s\n' ' BOOT BITCH DEVELOPMENT ENVIRONMENT'
printf '%s\n' '========================================'
if [[ -r /etc/os-release ]]; then
    printf 'Host: %s\n\n' "${PRETTY_NAME:-Linux}"
fi
printf 'Backend: %s (%s)\n\n' "$host_family" "$host_package_manager"

check_cmd 'C++ compiler' c++
check_cmd 'CMake' cmake
check_cmd 'Ninja' ninja
check_cmd 'pkg-config' pkg-config
check_cmd 'CPack' cpack
check_cmd 'dpkg-deb' dpkg-deb no
check_cmd 'dpkg-shlibdeps' dpkg-shlibdeps no
check_cmd 'rpmbuild' rpmbuild no
check_cmd 'makepkg (Arch)' makepkg no
check_cmd 'desktop-file-validate' desktop-file-validate no
check_cmd 'lintian' lintian no

printf '\nRuntime/recovery commands:\n'
check_cmd 'pkexec / Polkit' pkexec
check_cmd 'lsblk' lsblk
check_cmd 'findmnt' findmnt
check_cmd 'blkid' blkid
check_cmd 'mount' mount
check_cmd 'umount' umount
check_cmd 'cryptsetup' cryptsetup
check_cmd 'rsync' rsync
check_cmd 'realpath' realpath
check_cmd 'sha256sum' sha256sum
check_cmd 'getent' getent
check_cmd 'chroot' chroot
check_cmd 'base64 session codec' base64
check_cmd 'offline systemd repair' systemctl no
check_cmd 'UEFI boot manager' efibootmgr no
check_cmd 'UKI section inspection' objcopy no

check_any()
{
    local label="$1"
    shift
    local command
    for command in "$@"; do
        if command -v "$command" >/dev/null 2>&1; then
            printf 'PASS  %-26s %s\n' "$label" "$(command -v "$command")"
            return 0
        fi
    done
    printf 'OPT   %-26s missing (%s)\n' "$label" "$*"
    optional_missing=$((optional_missing + 1))
}

check_any 'Package manager' apt-get pacman dnf zypper
check_any 'GRUB generator' update-grub grub-mkconfig
check_any 'Initramfs generator' update-initramfs mkinitcpio dracut
check_any 'Initramfs inspector' lsinitramfs lsinitrd
check_any 'systemd-boot inspector' bootctl

printf '\nQt 6 Widgets: '
if pkg-config --exists Qt6Widgets 2>/dev/null; then
    pkg-config --modversion Qt6Widgets
else
    printf 'MISSING\n'
    failures=$((failures + 1))
fi

printf 'KF6 Auth: '
if pkg-config --exists KF6AuthCore 2>/dev/null; then
    pkg-config --modversion KF6AuthCore
else
    printf 'optional / not detected\n'
fi

printf '\nShell helper syntax: '
if bash -n "$ROOT_DIR/scripts/boot-repair-helper.sh"; then
    printf 'PASS\n'
else
    printf 'FAIL\n'
    failures=$((failures + 1))
fi

printf '\n'
if ((failures == 0)); then
    printf 'Build prerequisites: READY\n'
else
    printf 'Build prerequisites: %d required item(s) missing.\n' "$failures"
    printf 'Run: ./scripts/setup-dev-deps.sh\n'
fi
if ((optional_missing > 0)); then
    printf 'Optional tooling missing: %d item(s).\n' "$optional_missing"
fi

exit "$failures"
