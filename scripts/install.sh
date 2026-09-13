#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
MODE="${1:-}"

find_deb()
{
    find "$ROOT_DIR"/build-release "$ROOT_DIR"/build-deb "$ROOT_DIR"/build \
        -maxdepth 1 -type f -name 'boot-repair_*.deb' -print 2>/dev/null \
        | sort -V | tail -1
}

DEB="$(find_deb || true)"

if [[ -z "$DEB" || ! -f "$DEB" ]]
then
    echo "No Boot Bitch .deb package was found." >&2
    echo "Build one first with: ./scripts/build.sh" >&2
    exit 1
fi

DEB="$(realpath "$DEB")"

echo "Boot Bitch package:"
echo "  $DEB"
echo

dpkg-deb --info "$DEB" | sed -n '/^ Package:/p;/^ Version:/p;/^ Architecture:/p;/^ Depends:/p'
echo

if [[ "$MODE" == "--dry-run" || "$MODE" == "--simulate" ]]
then
    if command -v apt-get >/dev/null 2>&1
    then
        echo "Simulating installation only. No files will be installed."
        apt-get -s -o Debug::NoLocking=1 install "$DEB"
        exit $?
    fi

    echo "apt-get is not available; cannot simulate package installation." >&2
    exit 1
fi

cat <<'EOT'
This will install Boot Bitch through the system package manager.
No installation occurs unless you type INSTALL exactly.
EOT

read -r -p "Type INSTALL to continue: " confirm

if [[ "$confirm" != "INSTALL" ]]
then
    echo "Installation cancelled."
    exit 0
fi

if command -v sudo >/dev/null 2>&1
then
    sudo apt-get install "$DEB"
elif command -v pkexec >/dev/null 2>&1
then
    pkexec apt-get install "$DEB"
else
    echo "Neither sudo nor pkexec is available for authorization." >&2
    exit 1
fi
