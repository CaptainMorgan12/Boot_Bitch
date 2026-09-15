#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
SOURCE_MODE=0
DRY_RUN=0
for argument in "$@"; do
    case "$argument" in
        --source) SOURCE_MODE=1 ;;
        --dry-run|--simulate) DRY_RUN=1 ;;
        *)
            echo "Usage: $0 [--dry-run|--simulate] [--source]" >&2
            exit 2
            ;;
    esac
done

host_family=unknown
if [[ -r /etc/os-release ]]; then
    . /etc/os-release
    case "${ID:-}" in
        debian|ubuntu|tuxedo|linuxmint|pop|elementary|zorin) host_family=debian ;;
        arch|manjaro|endeavouros|garuda|artix) host_family=arch ;;
        fedora|rhel|rocky|almalinux) host_family=rpm ;;
        opensuse*|suse|sles) host_family=suse ;;
        *)
            [[ " ${ID_LIKE:-} " == *" debian "* || " ${ID_LIKE:-} " == *" ubuntu "* ]] && host_family=debian
            [[ " ${ID_LIKE:-} " == *" arch "* ]] && host_family=arch
            [[ " ${ID_LIKE:-} " == *" fedora "* || " ${ID_LIKE:-} " == *" rhel "* ]] && host_family=rpm
            [[ " ${ID_LIKE:-} " == *" suse "* ]] && host_family=suse
            ;;
    esac
fi

# A maintainer can override the selection while testing a package artifact
# from another family; normal users should leave this unset.
if [[ -n "${BOOT_REPAIR_PACKAGE_FAMILY:-}" ]]; then
    host_family="$BOOT_REPAIR_PACKAGE_FAMILY"
fi

find_one()
{
    local pattern="$1"
    find "$ROOT_DIR/build-release" "$ROOT_DIR/build-deb" "$ROOT_DIR/build-rpm" \
        "$ROOT_DIR/Development" -maxdepth 2 -type f -name "$pattern" -print 2>/dev/null \
        | sort -V | tail -1
}

DEB="$(find_one 'boot-repair_*.deb' || true)"
RPM="$(find_one 'boot-bitch-*.rpm' || true)"
[[ -n "$RPM" ]] || RPM="$(find_one 'boot-repair-*.rpm' || true)"
ARCH_PACKAGE="$(find_one 'boot-bitch-*.pkg.tar.*' || true)"

PACKAGE_KIND=""
PACKAGE_PATH=""
case "$host_family" in
    debian)
        [[ -n "$DEB" ]] && PACKAGE_KIND=deb && PACKAGE_PATH="$DEB"
        ;;
    arch)
        [[ -n "$ARCH_PACKAGE" ]] && PACKAGE_KIND=arch && PACKAGE_PATH="$ARCH_PACKAGE"
        ;;
    rpm|suse)
        [[ -n "$RPM" ]] && PACKAGE_KIND=rpm && PACKAGE_PATH="$RPM"
        ;;
    *)
        if [[ -n "$DEB" ]]; then PACKAGE_KIND=deb; PACKAGE_PATH="$DEB"
        elif [[ -n "$RPM" ]]; then PACKAGE_KIND=rpm; PACKAGE_PATH="$RPM"
        elif [[ -n "$ARCH_PACKAGE" ]]; then PACKAGE_KIND=arch; PACKAGE_PATH="$ARCH_PACKAGE"
        fi
        ;;
esac

run_privileged()
{
    if [[ ${EUID:-$(id -u)} -eq 0 ]]; then
        "$@"
    elif command -v sudo >/dev/null 2>&1; then
        sudo "$@"
    elif command -v pkexec >/dev/null 2>&1; then
        pkexec "$@"
    else
        echo "Neither sudo nor pkexec is available for authorization." >&2
        return 1
    fi
}

if (( ! SOURCE_MODE )) && [[ -n "$PACKAGE_PATH" && -f "$PACKAGE_PATH" ]]; then
    PACKAGE_PATH="$(realpath "$PACKAGE_PATH")"
    echo "Boot Bitch package for $host_family host:"
    echo "  $PACKAGE_PATH"
    echo

    case "$PACKAGE_KIND" in
        deb)
            command -v dpkg-deb >/dev/null 2>&1 || {
                echo "dpkg-deb is required to inspect this Debian package." >&2
                exit 1
            }
            dpkg-deb --info "$PACKAGE_PATH" | sed -n '/^ Package:/p;/^ Version:/p;/^ Architecture:/p;/^ Depends:/p'
            ;;
        rpm)
            if command -v rpm >/dev/null 2>&1; then
                rpm -qip "$PACKAGE_PATH" | sed -n '/^Name[[:space:]]*:/p;/^Version[[:space:]]*:/p;/^Architecture[[:space:]]*:/p;/^Requires[[:space:]]*:/p'
            else
                echo "RPM metadata tool is unavailable; package filename is $PACKAGE_PATH"
            fi
            ;;
        arch)
            if command -v pacman >/dev/null 2>&1; then
                pacman -Qip "$PACKAGE_PATH" | sed -n '/^Name[[:space:]]*:/p;/^Version[[:space:]]*:/p;/^Architecture[[:space:]]*:/p;/^Depends On[[:space:]]*:/p'
            else
                echo "pacman is unavailable; package filename is $PACKAGE_PATH"
            fi
            ;;
    esac
    echo

    if (( DRY_RUN )); then
        case "$PACKAGE_KIND" in
            deb)
                if command -v apt-get >/dev/null 2>&1; then
                    echo "Simulating installation only. No files will be installed."
                    apt-get -s -o Debug::NoLocking=1 install "$PACKAGE_PATH"
                elif command -v dpkg >/dev/null 2>&1; then
                    echo "Simulating dpkg installation only. No files will be installed."
                    dpkg --dry-run --install "$PACKAGE_PATH"
                else
                    echo "No Debian transaction simulator is available; no files were changed."
                fi
                ;;
            rpm)
                if command -v dnf >/dev/null 2>&1; then
                    echo "Simulating DNF installation only. No files will be installed."
                    dnf install --assumeno "$PACKAGE_PATH"
                elif command -v zypper >/dev/null 2>&1; then
                    echo "Simulating zypper installation only. No files will be installed."
                    zypper --non-interactive --dry-run install "$PACKAGE_PATH"
                else
                    echo "No RPM transaction simulator is available; no files were changed."
                fi
                ;;
            arch)
                echo "pacman local-package dependency simulation is not run by this script."
                echo "No files were changed; installation would run: pacman -U $PACKAGE_PATH"
                ;;
        esac
        exit 0
    fi

    cat <<'EOT'
This will install Boot Bitch through the native package manager.
No installation occurs unless you type INSTALL exactly.
EOT
    read -r -p "Type INSTALL to continue: " confirm
    [[ "$confirm" == INSTALL ]] || {
        echo "Installation cancelled."
        exit 0
    }

    case "$PACKAGE_KIND" in
        deb)
            if command -v apt-get >/dev/null 2>&1; then
                run_privileged apt-get install "$PACKAGE_PATH"
            elif command -v dpkg >/dev/null 2>&1; then
                run_privileged dpkg --install "$PACKAGE_PATH"
            else
                echo "Neither apt-get nor dpkg is available for this Debian package." >&2
                exit 1
            fi
            ;;
        rpm)
            if command -v dnf >/dev/null 2>&1; then
                run_privileged dnf install "$PACKAGE_PATH"
            elif command -v zypper >/dev/null 2>&1; then
                run_privileged zypper --non-interactive install "$PACKAGE_PATH"
            else
                echo "Neither dnf nor zypper is available for this RPM package." >&2
                exit 1
            fi
            ;;
        arch) run_privileged pacman -U "$PACKAGE_PATH" ;;
    esac
    exit $?
fi

if (( ! SOURCE_MODE )); then
    echo "No native package for the detected $host_family host was found." >&2
    echo "Build the matching package with one of these commands:" >&2
    echo "  ./scripts/package-deb.sh   # Debian/Ubuntu/TUXEDO" >&2
    echo "  ./scripts/package-arch.sh  # Arch/Manjaro/EndeavourOS" >&2
    echo "  ./scripts/package-rpm.sh   # Fedora/RHEL/openSUSE" >&2
    echo "  ./scripts/install.sh --source  # any supported Linux desktop" >&2
    exit 1
fi

SOURCE_PREFIX="${PREFIX:-/usr/local}"
[[ "$SOURCE_PREFIX" == /* && "$SOURCE_PREFIX" != / ]] || {
    echo "PREFIX must be an absolute path other than /." >&2
    exit 1
}
SOURCE_BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build-release}"
if [[ ! -x "$SOURCE_BUILD_DIR/boot-repair" ]]; then
    BUILD_DIR="$SOURCE_BUILD_DIR" "$ROOT_DIR/scripts/build.sh"
fi
[[ -x "$SOURCE_BUILD_DIR/boot-repair" ]] || {
    echo "Source build did not produce $SOURCE_BUILD_DIR/boot-repair." >&2
    exit 1
}

if (( DRY_RUN )); then
    echo "Source installation dry run; no files will be installed."
    echo "  cmake --install $SOURCE_BUILD_DIR --prefix $SOURCE_PREFIX"
    exit 0
fi

cat <<EOT
This will install the staged CMake build under $SOURCE_PREFIX.
No installation occurs unless you type INSTALL exactly.
EOT
read -r -p "Type INSTALL to continue: " confirm
[[ "$confirm" == INSTALL ]] || {
    echo "Installation cancelled."
    exit 0
}
run_privileged cmake --install "$SOURCE_BUILD_DIR" --prefix "$SOURCE_PREFIX"
echo "Source installation completed under $SOURCE_PREFIX."

# CMake updates its install manifest to the effective --prefix. Keep a local
# copy so the cross-distro uninstall helper can remove exactly this source
# install.
if [[ -f "$SOURCE_BUILD_DIR/install_manifest.txt" ]]; then
    source_manifest="$SOURCE_BUILD_DIR/boot-repair-source-install-manifest.txt"
    cp -- "$SOURCE_BUILD_DIR/install_manifest.txt" "$source_manifest"
    echo "Source install manifest recorded: $source_manifest"
fi
