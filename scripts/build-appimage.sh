#!/usr/bin/env bash
set -euo pipefail

# Build the portable AppImage with linuxdeploy/linuxdeploy-plugin-qt, falling
# back to appimagetool-only packaging when those tools are not available.
#
# The AppImage embeds a gh-releases-zsync update-information string and a
# matching .zsync file is written next to it, so Gear Lever/AppImageUpdate can
# detect the GitHub release and fetch binary deltas. zsyncmake is required:
# when it cannot be found the build fails with instructions instead of
# silently producing an AppImage without update metadata.
#
# Overrides: BUILD_DIR, BUILD_TYPE, JOBS, ARCH, OUTPUT, APPIMAGETOOL,
#            LINUXDEPLOY, LINUXDEPLOY_PLUGIN_QT, APPIMAGE_RUNTIME_FILE, QMAKE,
#            ZSYNCMAKE, UPDATE_INFORMATION.

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build-appimage}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 2)}"
APPIMAGETOOL="${APPIMAGETOOL:-}"
LINUXDEPLOY="${LINUXDEPLOY:-linuxdeploy}"
LINUXDEPLOY_PLUGIN_QT="${LINUXDEPLOY_PLUGIN_QT:-linuxdeploy-plugin-qt}"
APPIMAGE_RUNTIME_FILE="${APPIMAGE_RUNTIME_FILE:-}"
QMAKE="${QMAKE:-$(command -v qmake6 2>/dev/null || command -v qmake 2>/dev/null || true)}"
ZSYNCMAKE="${ZSYNCMAKE:-}"
# The published release assets keep this version-agnostic name pattern; the
# embedded string is what Gear Lever/AppImageUpdate resolve against GitHub.
UPDATE_INFORMATION="${UPDATE_INFORMATION:-gh-releases-zsync|CaptainMorgan12|Boot_Bitch|latest|boot-repair_*_x86_64.AppImage.zsync}"

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

# Print a usable zsyncmake path or return 1. Priority: explicit ZSYNCMAKE,
# a binary in Development/tools/, PATH, then the copy bundled inside the
# appimagetool AppImage (extracted into a temporary directory).
resolve_zsyncmake()
{
    if [[ -n "$ZSYNCMAKE" ]]; then
        [[ -x "$ZSYNCMAKE" ]] || {
            echo "ZSYNCMAKE is not executable: $ZSYNCMAKE" >&2
            exit 1
        }
        printf '%s\n' "$ZSYNCMAKE"
        return 0
    fi
    if [[ -x "$ROOT_DIR/Development/tools/zsyncmake" ]]; then
        printf '%s\n' "$ROOT_DIR/Development/tools/zsyncmake"
        return 0
    fi
    if command -v zsyncmake >/dev/null 2>&1; then
        command -v zsyncmake
        return 0
    fi
    if [[ "$APPIMAGETOOL" == *.AppImage ]]; then
        local extract_dir
        extract_dir="$(mktemp -d "${TMPDIR:-/tmp}/boot-bitch-zsyncmake.XXXXXX")"
        if (cd "$extract_dir" && "$APPIMAGETOOL" --appimage-extract 'usr/bin/zsyncmake' >/dev/null 2>&1) \
                && [[ -x "$extract_dir/squashfs-root/usr/bin/zsyncmake" ]]; then
            printf '%s\n' "$extract_dir/squashfs-root/usr/bin/zsyncmake"
            return 0
        fi
        rm -rf -- "$extract_dir"
    fi
    return 1
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

ZSYNCMAKE="$(resolve_zsyncmake)" || {
    cat >&2 <<'EOF'
ERROR: zsyncmake was not found; the release AppImage needs it to generate the
       .zsync update metadata that Gear Lever/AppImageUpdate read.
       Install the zsync package (it provides zsyncmake), place a zsyncmake
       binary at Development/tools/zsyncmake, set ZSYNCMAKE=/path/to/zsyncmake,
       or use the appimagetool AppImage from Development/tools/ (it bundles one).
EOF
    exit 1
}

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

# Every executable must keep mode 0755 in the AppDir so the squashfs carries
# the executable bit. The privileged helper is the critical one: pkexec runs
# it as root, and the AppImage's FUSE mount is read-only, so a build that
# dropped the bit could not be repaired at runtime. linuxdeploy runs after the
# install, so the permissions are re-asserted before packaging as well.
assert_appdir_executables()
{
    local executable
    for executable in \
        "usr/bin/boot-repair" \
        "usr/libexec/boot-repair/boot-repair-helper" \
        "usr/libexec/boot-repair/boot-repair-efi-label.py"; do
        [[ -f "$APPDIR/$executable" ]] || {
            echo "AppDir executable missing: $APPDIR/$executable" >&2
            exit 1
        }
        chmod 0755 "$APPDIR/$executable"
        [[ -x "$APPDIR/$executable" ]] || {
            echo "AppDir executable is not executable: $APPDIR/$executable" >&2
            exit 1
        }
    done
}

assert_appdir_executables

# AppImage launches through this small wrapper so the image can be mounted at
# any path. The GUI itself remains unprivileged; privileged operations still
# use the host's pkexec/Polkit and system tools. The type2 runtime mounts the
# squashfs read-only without noexec, but a FUSE mount is only accessible to the
# user who mounted it, so the GUI stages a sha256-verified private copy of the
# helper before pkexec runs it as root (see MainWindow::privilegedSessionHelper).
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

update_args=(--updateinformation "$UPDATE_INFORMATION")

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
    # linuxdeploy may rewrite AppDir entries; never package a helper without
    # the executable bit.
    assert_appdir_executables
    ARCH="$ARCH" run_appimagetool "$APPIMAGETOOL" "${runtime_args[@]}" "${update_args[@]}" "$APPDIR" "$OUTPUT"
else
    ARCH="$ARCH" run_appimagetool "$APPIMAGETOOL" "${runtime_args[@]}" "${update_args[@]}" "$APPDIR" "$OUTPUT"
fi
[[ -s "$OUTPUT" ]] || {
    echo "appimagetool completed but produced no AppImage: $OUTPUT" >&2
    exit 1
}

# The embedded .upd_info section is what Gear Lever/AppImageUpdate read from
# the file itself; readelf is optional tooling, so the check is skipped when
# it is unavailable (verify-release.sh re-checks the captured artifact).
if command -v readelf >/dev/null 2>&1; then
    embedded_update_info="$(readelf -p .upd_info "$OUTPUT" 2>/dev/null \
        | sed -n 's/.*\]  //p' | head -n1)"
    if [[ "$embedded_update_info" != "$UPDATE_INFORMATION" ]]; then
        echo "ERROR: the AppImage .upd_info section does not carry the update information" >&2
        echo "       expected: $UPDATE_INFORMATION" >&2
        echo "       found:    ${embedded_update_info:-<empty>}" >&2
        exit 1
    fi
fi

# Write the .zsync next to the AppImage ourselves: appimagetool also invokes
# zsyncmake, but under --appimage-extract-and-run it runs from the discarded
# extraction directory. The relative URL matches what zsync clients resolve
# against the published .zsync asset URL.
ZSYNC_OUTPUT="$OUTPUT.zsync"
"$ZSYNCMAKE" -u "$(basename -- "$OUTPUT")" -o "$ZSYNC_OUTPUT" "$OUTPUT"
[[ -s "$ZSYNC_OUTPUT" ]] || {
    echo "zsyncmake completed but produced no metadata: $ZSYNC_OUTPUT" >&2
    exit 1
}
grep -qF "Filename: $(basename -- "$OUTPUT")" "$ZSYNC_OUTPUT" || {
    echo "ERROR: $ZSYNC_OUTPUT does not reference $(basename -- "$OUTPUT")" >&2
    exit 1
}

echo
echo "AppImage created:"
echo "  $OUTPUT"
echo "  $ZSYNC_OUTPUT"
echo
echo "The AppImage still uses host pkexec/Polkit and repair utilities (mount, cryptsetup, btrfs, efibootmgr, and so on)."
if (( ! USE_LINUXDEPLOY )); then
    echo "WARN: this appimagetool-only artifact expects Qt libraries from the host; use linuxdeploy-plugin-qt for a portable release artifact." >&2
fi
