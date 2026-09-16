#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"

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

remove_legacy_source_install()
{
    # Older source installs under /usr/local take precedence over packaged
    # /usr/bin/boot-repair and make desktop launches appear stale. Remove only
    # the files owned by this application; leave unrelated directories intact.
    run_privileged rm -f -- \
        /usr/local/bin/boot-repair \
        /usr/local/libexec/boot-repair/boot-repair-helper \
        /usr/local/libexec/boot-repair/boot-repair-efi-label.py \
        /usr/local/share/applications/org.bootrepair.BootRepair.desktop
    run_privileged sh -c 'find /usr/local/share/icons/hicolor -path "*/apps/org.bootrepair.BootRepair.png" -type f -delete 2>/dev/null || true'
}

if command -v dpkg-query >/dev/null 2>&1 \
   && dpkg-query -W -f='${Status}\n' boot-repair 2>/dev/null \
      | grep -q '^install ok installed$'
then
    echo "Boot Bitch is installed as a Debian package."
    echo "Recommended removal command: apt-get remove boot-repair"
    echo
    read -r -p "Type UNINSTALL to remove the package: " confirm
    [[ "$confirm" == UNINSTALL ]] || { echo "Uninstall cancelled."; exit 0; }
    if command -v apt-get >/dev/null 2>&1; then
        run_privileged apt-get remove boot-repair
    else
        run_privileged dpkg --remove boot-repair
    fi
    remove_legacy_source_install
    exit $?
fi

if command -v pacman >/dev/null 2>&1 \
   && pacman -Q boot-bitch >/dev/null 2>&1
then
    echo "Boot Bitch is installed as an Arch package."
    echo "Recommended removal command: pacman -R boot-bitch"
    echo
    read -r -p "Type UNINSTALL to remove the package: " confirm
    [[ "$confirm" == UNINSTALL ]] || { echo "Uninstall cancelled."; exit 0; }
    run_privileged pacman -R boot-bitch
    remove_legacy_source_install
    exit $?
fi

if command -v rpm >/dev/null 2>&1 \
   && rpm -q boot-bitch >/dev/null 2>&1
then
    echo "Boot Bitch is installed as an RPM package."
    echo "Recommended removal command: dnf remove boot-bitch or zypper remove boot-bitch"
    echo
    read -r -p "Type UNINSTALL to remove the package: " confirm
    [[ "$confirm" == UNINSTALL ]] || { echo "Uninstall cancelled."; exit 0; }
    if command -v dnf >/dev/null 2>&1; then
        run_privileged dnf remove boot-bitch
    elif command -v zypper >/dev/null 2>&1; then
        run_privileged zypper --non-interactive remove boot-bitch
    else
        run_privileged rpm -e boot-bitch
    fi
    remove_legacy_source_install
    exit $?
fi

MANIFEST="${INSTALL_MANIFEST:-}"
if [[ -z "$MANIFEST" ]]; then
    for candidate in \
        "$ROOT_DIR/build-release/boot-repair-source-install-manifest.txt" \
        "$ROOT_DIR/build/boot-repair-source-install-manifest.txt" \
        "$ROOT_DIR/Development/build-release/boot-repair-source-install-manifest.txt"
    do
        if [[ -f "$candidate" ]]; then
            MANIFEST="$candidate"
            break
        fi
    done
fi

[[ -n "$MANIFEST" ]] || {
    echo "No installed package or recorded source-install manifest was found." >&2
    echo "Use INSTALL_MANIFEST=/absolute/path for a manually reviewed CMake manifest." >&2
    exit 1
}

validate_manifest_entry()
{
    local file="$1" canonical
    [[ "$file" == /* && "$file" != *$'\n'* && "$file" != *$'\r'* ]] || return 1
    canonical="$(realpath -m -- "$file" 2>/dev/null)" || return 1
    [[ "$canonical" == "$file" ]] || return 1
    if [[ -e "$canonical" || -L "$canonical" ]]; then
        [[ ! -L "$canonical" ]] || return 1
    fi
    case "$canonical" in
        /usr/bin/boot-repair|/usr/libexec/boot-repair/boot-repair-helper|/usr/libexec/boot-repair/boot-repair-efi-label.py|\
        /usr/share/applications/org.bootrepair.BootRepair.desktop|\
        /usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml|\
        /usr/share/icons/hicolor/*/apps/org.bootrepair.BootRepair.png|\
        /usr/share/doc/boot-repair/*|/usr/share/man/man1/boot-repair.1.gz|\
        /usr/local/bin/boot-repair|/usr/local/libexec/boot-repair/boot-repair-helper|/usr/local/libexec/boot-repair/boot-repair-efi-label.py|\
        /usr/local/share/applications/org.bootrepair.BootRepair.desktop|\
        /usr/local/share/metainfo/org.bootrepair.BootRepair.metainfo.xml|\
        /usr/local/share/icons/hicolor/*/apps/org.bootrepair.BootRepair.png|\
        /usr/local/share/doc/boot-repair/*|/usr/local/share/man/man1/boot-repair.1.gz|\
        /opt/boot-repair/bin/boot-repair|/opt/boot-repair/libexec/boot-repair/boot-repair-helper|\
        /opt/boot-repair/libexec/boot-repair/boot-repair-efi-label.py|\
        /opt/boot-repair/share/applications/org.bootrepair.BootRepair.desktop|\
        /opt/boot-repair/share/metainfo/org.bootrepair.BootRepair.metainfo.xml|\
        /opt/boot-repair/share/icons/hicolor/*/apps/org.bootrepair.BootRepair.png|\
        /opt/boot-repair/share/doc/boot-repair/*|/opt/boot-repair/share/man/man1/boot-repair.1.gz)
            return 0 ;;
        *) return 1 ;;
    esac
}

echo "Recorded CMake source-install manifest: $MANIFEST"
echo
cat "$MANIFEST"
echo
read -r -p "Type UNINSTALL to remove exactly these files: " confirm
[[ "$confirm" == UNINSTALL ]] || { echo "Uninstall cancelled."; exit 0; }

mkdir -p -- "$ROOT_DIR/Development"
REMOVE_SCRIPT="$(mktemp "$ROOT_DIR/Development/boot-repair-uninstall.XXXXXX")"
trap 'rm -f "$REMOVE_SCRIPT"' EXIT

{
    echo '#!/usr/bin/env bash'
    echo 'set -euo pipefail'
    while IFS= read -r file; do
        [[ -z "$file" ]] && continue
        validate_manifest_entry "$file" || {
            echo "Refusing unsafe or unrecognized manifest entry: $file" >&2
            exit 1
        }
        printf 'rm -f -- %q\n' "$file"
    done < "$MANIFEST"
} > "$REMOVE_SCRIPT"
chmod 700 "$REMOVE_SCRIPT"
run_privileged "$REMOVE_SCRIPT"

echo "Boot Bitch files from the recorded manifest were removed."
