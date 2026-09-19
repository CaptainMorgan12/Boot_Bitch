#!/usr/bin/env bash
set -euo pipefail

# Build the Debian package on a Debian-family host (requires dpkg-dev).

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"

if [[ -r /etc/os-release ]]; then
    . /etc/os-release
fi
case "${ID:-}" in
    debian|ubuntu|tuxedo|linuxmint|pop|elementary|zorin) ;;
    *)
        if [[ " ${ID_LIKE:-} " != *" debian "* && " ${ID_LIKE:-} " != *" ubuntu "* ]]; then
            echo "The .deb workflow is restricted to Debian-family packaging hosts." >&2
            echo "Use package-arch.sh, package-rpm.sh, or install.sh --source on this host." >&2
            exit 2
        fi
        ;;
esac

for cmd in cpack dpkg-shlibdeps dpkg-deb; do
    command -v "$cmd" >/dev/null 2>&1 || {
        echo "Missing required Debian packaging command: $cmd" >&2
        echo "On Debian-family systems install dpkg-dev/cmake packaging tools first." >&2
        exit 1
    }
done

BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build-deb}" \
BUILD_TYPE=Release \
"$ROOT_DIR/scripts/build.sh"

echo
echo "Debian packaging workflow completed. Nothing was installed on the host."
