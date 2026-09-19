#!/usr/bin/env bash
set -euo pipefail

# Build the portable AppImage with linuxdeploy/linuxdeploy-plugin-qt, falling
# back to appimagetool-only packaging when those tools are not available.
#
# Overrides: BUILD_DIR, BUILD_TYPE, JOBS, ARCH, OUTPUT, APPIMAGETOOL,
#            LINUXDEPLOY, LINUXDEPLOY_PLUGIN_QT, APPIMAGE_RUNTIME_FILE, QMAKE.

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build-appimage}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 2)}"
APPIMAGETOOL="${APPIMAGETOOL:-}"
LINUXDEPLOY="${LINUXDEPLOY:-linuxdeploy}"
LINUXDEPLOY_PLUGIN_QT="${LINUXDEPLOY_PLUGIN_QT:-linuxdeploy-plugin-qt}"
APPIMAGE_RUNTIME_FILE="${APPIMAGE_RUNTIME_FILE:-}"
QMAKE="${QMAKE:-$(command -v qmake6 2>/dev/null || command -v qmake 2>/dev/null || true)}"

# Prefer the disposable local development install when no system tool was
# selected. This keeps AppImage tooling out of the host package manager while
# making repeated release-development builds one-command operations.
if [[ "$LINUXDEPLOY" == "linuxdeploy" && -x "$ROOT_DIR/Development/tools/linuxdeploy-x86_64.AppImage" ]]; then
    LINUXDEPLOY="$ROOT_DIR/Development/tools/linuxdeploy-x86_64.AppImage"
fi
if [[ "$LINUXDEPLOY_PLUGIN_QT" == "linuxdeploy-plugin-qt" && -x "$ROOT_DIR/Development/tools/linuxdeploy-plugin-qt-x86_64.AppImage" ]]; then
    LINUXDEPLOY_PLUGIN_QT="$ROOT_DIR/Development/tools/linuxdeploy-plugin-qt-x86_64.AppImage"
fi
if [[ -z "$APPIMAGETOOL" && -x "$ROOT_DIR/Development/tools/appimagetool-x86_64.AppImage" ]]; then
    APPIMAGETOOL="$ROOT_DIR/Development/tools/appimagetool-x86_64.AppImage"
fi
if [[ -z "$APPIMAGE_RUNTIME_FILE" && -f "$ROOT_DIR/Development/tools/runtime-x86_64" ]]; then
    APPIMAGE_RUNTIME_FILE="$ROOT_DIR/Development/tools/runtime-x86_64"
fi
APPIMAGETOOL="${APPIMAGETOOL:-appimagetool}"

run_tool()
{
    local tool="$1"
    shift
    if [[ "$tool" == *.AppImage ]]; then
        "$tool" --appimage-extract-and-run "$@"
    else
        "$tool" "$@"
    fi
}

run_appimagetool()
{
    local tool="$1"
    shift
    local tool_dir
    tool_dir="$(cd -- "$(dirname -- "$(command -v "$tool")")" && pwd)"
    PATH="$tool_dir:$PATH" run_tool "$tool" "$@"
}

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
if command -v "$LINUXDEPLOY" >/dev/null 2>&1 \
   && command -v "$LINUXDEPLOY_PLUGIN_QT" >/dev/null 2>&1; then
    need "$APPIMAGETOOL"
    USE_LINUXDEPLOY=1
    # linuxdeploy discovers plugins by executable name on PATH. Preserve that
    # behavior when callers provide an absolute plugin path via the override.
    LINUXDEPLOY_PLUGIN_DIR="$(dirname -- "$(command -v "$LINUXDEPLOY_PLUGIN_QT")")"
    export PATH="$LINUXDEPLOY_PLUGIN_DIR:$PATH"
else
    need "$APPIMAGETOOL"
    echo "WARN: linuxdeploy and linuxdeploy-plugin-qt were not both found; creating an AppImage from the AppDir without bundling shared libraries." >&2
    echo "      Install linuxdeploy, linuxdeploy-plugin-qt, and appimagetool for a portable artifact." >&2
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
PROJECT_VERSION="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\+\([0-9][0-9.]*\).*/\1/p' "$ROOT_DIR/CMakeLists.txt" | head -1)"
[[ -n "$PROJECT_VERSION" ]] || { echo "Unable to determine project version from CMakeLists.txt" >&2; exit 1; }
OUTPUT="${OUTPUT:-$ROOT_DIR/build-release/boot-repair_${BUILD_VERSION:-$PROJECT_VERSION}_${ARCH}.AppImage}"
mkdir -p -- "$(dirname -- "$OUTPUT")"

runtime_args=()
if [[ -n "$APPIMAGE_RUNTIME_FILE" ]]; then
    [[ -f "$APPIMAGE_RUNTIME_FILE" ]] || { echo "AppImage runtime file not found: $APPIMAGE_RUNTIME_FILE" >&2; exit 1; }
    runtime_args+=(--runtime-file "$APPIMAGE_RUNTIME_FILE")
fi

if (( USE_LINUXDEPLOY )); then
    # First let linuxdeploy populate the AppDir and bundle Qt. We invoke
    # appimagetool ourselves so APPIMAGE_RUNTIME_FILE can be supplied on hosts
    # where the tool cannot download its runtime (for example, restricted CI).
    output_dir="$(dirname -- "$OUTPUT")"
    deploy_dir="$(mktemp -d "$output_dir/.appimage-output.XXXXXX")"
    plugin_dir="$(mktemp -d "$output_dir/.appimage-plugin.XXXXXX")"
    trap 'rm -rf -- "$deploy_dir" "$plugin_dir"' EXIT
    # linuxdeploy discovers plugins by the canonical linuxdeploy-plugin-*
    # name. This also supports a downloaded, architecture-suffixed AppImage
    # supplied through LINUXDEPLOY_PLUGIN_QT.
    plugin_path="$(command -v "$LINUXDEPLOY_PLUGIN_QT")"
    if [[ "$plugin_path" == *.AppImage ]]; then
        cat > "$plugin_dir/linuxdeploy-plugin-qt" <<PLUGIN_WRAPPER
#!/bin/sh
exec "$plugin_path" --appimage-extract-and-run "\$@"
PLUGIN_WRAPPER
        chmod 755 "$plugin_dir/linuxdeploy-plugin-qt"
    else
        ln -s "$plugin_path" "$plugin_dir/linuxdeploy-plugin-qt"
    fi
    (cd "$deploy_dir" && PATH="$plugin_dir:$PATH" ARCH="$ARCH" QMAKE="$QMAKE" run_tool "$LINUXDEPLOY" \
        --appdir "$APPDIR" \
        --desktop-file "$APPDIR/org.bootrepair.BootRepair.desktop" \
        --icon-file "$APPDIR/org.bootrepair.BootRepair.png" \
        --plugin qt)
    ARCH="$ARCH" run_appimagetool "$APPIMAGETOOL" "${runtime_args[@]}" "$APPDIR" "$OUTPUT"
else
    ARCH="$ARCH" run_appimagetool "$APPIMAGETOOL" "${runtime_args[@]}" "$APPDIR" "$OUTPUT"
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
    echo "WARN: this appimagetool-only artifact expects Qt libraries from the host; use linuxdeploy-plugin-qt for a portable release artifact." >&2
fi
