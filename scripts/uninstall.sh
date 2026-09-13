#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"

if command -v dpkg-query >/dev/null 2>&1 \
   && dpkg-query -W -f='${Status}\n' boot-repair 2>/dev/null \
      | grep -q '^install ok installed$'
then
    echo "Boot Bitch is installed as a Debian package."
    echo "Recommended removal command: apt-get remove boot-repair"
    echo
    read -r -p "Type UNINSTALL to remove the package: " confirm

    [[ "$confirm" == "UNINSTALL" ]] || {
        echo "Uninstall cancelled."
        exit 0
    }

    if command -v sudo >/dev/null 2>&1
    then
        sudo apt-get remove boot-repair
    elif command -v pkexec >/dev/null 2>&1
    then
        pkexec apt-get remove boot-repair
    else
        echo "Neither sudo nor pkexec is available for authorization." >&2
        exit 1
    fi

    exit 0
fi

MANIFEST=""
for candidate in \
    "$ROOT_DIR/build/install_manifest.txt" \
    "$ROOT_DIR/build-release/install_manifest.txt" \
    "$ROOT_DIR/build-deb/install_manifest.txt"
 do
    if [[ -f "$candidate" ]]
    then
        MANIFEST="$candidate"
        break
    fi
 done

[[ -n "$MANIFEST" ]] || {
    echo "No installed Debian package or CMake install manifest was found." >&2
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
        /usr/bin/boot-repair|/usr/libexec/boot-repair/boot-repair-helper|\
        /usr/share/applications/org.bootrepair.BootRepair.desktop|\
        /usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml|\
        /usr/share/icons/hicolor/*/apps/org.bootrepair.BootRepair.png|\
        /usr/share/doc/boot-repair/*|/usr/share/man/man1/boot-repair.1.gz|\
        /usr/local/bin/boot-repair|/usr/local/libexec/boot-repair/boot-repair-helper|\
        /usr/local/share/applications/org.bootrepair.BootRepair.desktop|\
        /usr/local/share/metainfo/org.bootrepair.BootRepair.metainfo.xml|\
        /usr/local/share/icons/hicolor/*/apps/org.bootrepair.BootRepair.png|\
        /usr/local/share/doc/boot-repair/*|/usr/local/share/man/man1/boot-repair.1.gz)
            return 0 ;;
        *) return 1 ;;
    esac
}

echo "CMake install manifest: $MANIFEST"
echo
cat "$MANIFEST"
echo
read -r -p "Type UNINSTALL to remove exactly these files: " confirm

[[ "$confirm" == "UNINSTALL" ]] || {
    echo "Uninstall cancelled."
    exit 0
}

REMOVE_SCRIPT="$(mktemp /tmp/boot-repair-uninstall.XXXXXX)"
trap 'rm -f "$REMOVE_SCRIPT"' EXIT

{
    echo '#!/usr/bin/env bash'
    echo 'set -euo pipefail'
    while IFS= read -r file
    do
        [[ -z "$file" ]] && continue
        validate_manifest_entry "$file" || {
            echo "Refusing unsafe or unrecognized manifest entry: $file" >&2
            exit 1
        }
        printf 'rm -f -- %q\n' "$file"
    done < "$MANIFEST"
} > "$REMOVE_SCRIPT"
chmod 700 "$REMOVE_SCRIPT"

if command -v sudo >/dev/null 2>&1
then
    sudo "$REMOVE_SCRIPT"
elif command -v pkexec >/dev/null 2>&1
then
    pkexec "$REMOVE_SCRIPT"
else
    echo "Neither sudo nor pkexec is available for authorization." >&2
    exit 1
fi

echo "Boot Bitch files from the recorded manifest were removed."
