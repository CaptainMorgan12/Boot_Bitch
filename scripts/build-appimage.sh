#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build-appimage}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 2)}"
APPIMAGETOOL="${APPIMAGETOOL:-appimagetool}"
LINUXDEPLOY="${LINUXDEPLOY:-linuxdeploy}"

need()
{
    command -v "$1" >/dev/null 2>&1 || {
        echo "Missing required build command: $1" >&2
        exit 1
    }
}

for cmd in cmake ninja c++; do
    need "$cmd"
done

# linuxdeploy bundles the Qt and system libraries needed by the executable.
# Keep appimagetool-only mode as a useful fallback for local smoke testing, but
# call it out because that mode packages the AppDir without copying libraries.
USE_LINUXDEPLOY=0
if command -v "$LINUXDEPLOY" >/dev/null 2>&1; then
    need "$APPIMAGETOOL"
    USE_LINUXDEPLOY=1
else
    need "$APPIMAGETOOL"
    echo "WARN: linuxdeploy was not found; creating an AppImage from the AppDir without bundling shared libraries." >&2
    echo "      Install linuxdeploy and its Qt plugin for a portable artifact." >&2
fi

bash -n "$ROOT_DIR/scripts/boot-repair-helper.sh"

BUILD_DIR_REAL="$(realpath -m -- "$BUILD_DIR")"
ROOT_DIR_REAL="$(realpath -m -- "$ROOT_DIR")"
if [[ -z "$BUILD_DIR_REAL" || "$BUILD_DIR_REAL" == / || "$BUILD_DIR_REAL" == "$ROOT_DIR_REAL" ]]; then
    echo "Refusing to remove unsafe build directory: $BUILD_DIR" >&2
    exit 1
fi
rm -rf -- "$BUILD_DIR"

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_INSTALL_PREFIX=/usr
cmake --build "$BUILD_DIR" -j"$JOBS"

APPDIR="$BUILD_DIR/Boot-Bitch.AppDir"
rm -rf -- "$APPDIR"
DESTDIR="$APPDIR" cmake --install "$BUILD_DIR" --strip

[[ -x "$APPDIR/usr/bin/boot-repair" ]] || {
    echo "AppDir executable missing: $APPDIR/usr/bin/boot-repair" >&2
    exit 1
}
[[ -x "$APPDIR/usr/libexec/boot-repair/boot-repair-helper" ]] || {
    echo "AppDir privileged helper missing: $APPDIR/usr/libexec/boot-repair/boot-repair-helper" >&2
    exit 1
}

# AppImage launches through this small wrapper so the image can be mounted at
# any path. The GUI itself remains unprivileged; privileged operations still
# use the host's pkexec/Polkit and system tools.
cat > "$APPDIR/AppRun" <<'APP_RUN'
#!/bin/sh
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
exec "$HERE/usr/bin/boot-repair" "$@"
APP_RUN
chmod 755 "$APPDIR/AppRun"

cp "$APPDIR/usr/share/applications/org.bootrepair.BootRepair.desktop" "$APPDIR/"
cp "$APPDIR/usr/share/icons/hicolor/256x256/apps/org.bootrepair.BootRepair.png" \
   "$APPDIR/org.bootrepair.BootRepair.png"

ARCH="${ARCH:-x86_64}"
OUTPUT="${OUTPUT:-$ROOT_DIR/build-release/boot-repair_${BUILD_VERSION:-$(sed -n 's/.*VERSION \([0-9][0-9.]*\).*/\1/p' "$ROOT_DIR/CMakeLists.txt" | head -1)}_${ARCH}.AppImage}"
mkdir -p -- "$(dirname -- "$OUTPUT")"

if (( USE_LINUXDEPLOY )); then
    # linuxdeploy writes its output in the current directory. Set the output
    # directory to the requested artifact's directory and normalize the name
    # afterwards so releases use the same versioned filename in every mode.
    output_dir="$(dirname -- "$OUTPUT")"
    (cd "$output_dir" && ARCH="$ARCH" "$LINUXDEPLOY" \
        --appdir "$APPDIR" \
        --desktop-file "$APPDIR/org.bootrepair.BootRepair.desktop" \
        --icon-file "$APPDIR/org.bootrepair.BootRepair.png" \
        --output appimage)
    mapfile -t generated < <(find "$output_dir" -maxdepth 1 -type f -name '*.AppImage' -print)
    [[ ${#generated[@]} -gt 0 ]] || {
        echo "linuxdeploy completed but produced no AppImage in $output_dir" >&2
        exit 1
    }
    if [[ "${generated[0]}" != "$OUTPUT" ]]; then
        mv -f -- "${generated[0]}" "$OUTPUT"
    fi
else
    ARCH="$ARCH" "$APPIMAGETOOL" "$APPDIR" "$OUTPUT"
fi
[[ -s "$OUTPUT" ]] || {
    echo "appimagetool completed but produced no AppImage: $OUTPUT" >&2
    exit 1
}

echo
echo "AppImage created:"
echo "  $OUTPUT"
echo
echo "The AppImage still uses host pkexec/Polkit and repair utilities (mount, cryptsetup, btrfs, efibootmgr, and so on)."
if (( ! USE_LINUXDEPLOY )); then
    echo "WARN: this appimagetool-only artifact expects Qt libraries from the host; use linuxdeploy for a portable release artifact." >&2
fi
