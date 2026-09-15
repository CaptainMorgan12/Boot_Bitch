#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/Development/build-tarball}"

command -v cpack >/dev/null 2>&1 || {
    echo "Missing required packaging command: cpack (provided by CMake)." >&2
    exit 1
}

BUILD_DIR_REAL="$(realpath -m -- "$BUILD_DIR")"
ROOT_DIR_REAL="$(realpath -m -- "$ROOT_DIR")"
if [[ -z "$BUILD_DIR_REAL" || "$BUILD_DIR_REAL" == / || "$BUILD_DIR_REAL" == "$ROOT_DIR_REAL" ]]; then
    echo "Refusing to remove unsafe build directory: $BUILD_DIR" >&2
    exit 1
fi

BUILD_TYPE=Release BUILD_DIR="$BUILD_DIR" "$ROOT_DIR/scripts/build.sh"

(
    cd "$BUILD_DIR"
    cpack -G TGZ --config CPackConfig.cmake
)

mapfile -t packages < <(
    find "$BUILD_DIR" -maxdepth 1 -type f \( -name '*.tar.gz' -o -name '*.tgz' \) -print | sort
)
[[ ${#packages[@]} -gt 0 ]] || {
    echo "CPack ran but no TGZ package was generated." >&2
    exit 1
}

echo
echo "Binary install-tree archive created (built for this distribution):"
printf '  %s\n' "${packages[@]}"
echo
echo "Nothing was installed on the host. Extract it for inspection, or install the source tree with cmake --install."
