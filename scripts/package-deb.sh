#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"

for cmd in cpack dpkg-shlibdeps dpkg-deb
 do
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
