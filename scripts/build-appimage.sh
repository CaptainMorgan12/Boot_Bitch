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
# The reference build host is a Debian-family desktop (TUXEDO OS) with the Qt
# platform theme packages installed: linuxdeploy can only bundle the platform
# themes, widget styles and the SVG icon engine that the build host provides,
# so dialogs and file pickers match the desktop only when the release AppImage
# is built there. Other hosts still build, with a non-fatal warning.
#
# Overrides: BUILD_DIR, BUILD_TYPE, JOBS, ARCH, OUTPUT, APPIMAGETOOL,
#            LINUXDEPLOY, LINUXDEPLOY_PLUGIN_QT, APPIMAGE_RUNTIME_FILE, QMAKE,
#            ZSYNCMAKE, UPDATE_INFORMATION, EXTRA_QT_MODULES,
#            DEPLOY_PLATFORM_THEMES, PATCHELF, NO_STRIP,
#            APPIMAGE_EXTRACT_AND_RUN.
#
# Portability: linuxdeploy bundles binutils 2.35 strip and patchelf 0.15,
# which cannot process SHT_RELR (".relr.dyn") sections emitted by modern
# Arch/Fedora/Alpine toolchains. The script probes the payload and applies
# safe fallbacks automatically: a system patchelf >= 0.18 through
# linuxdeploy's $PATCHELF override when available, NO_STRIP=1 when the strip
# linuxdeploy would use cannot parse RELR, and APPIMAGE_EXTRACT_AND_RUN=1 on
# musl or FUSE-less hosts so linuxdeploy and linuxdeploy-plugin-qt run in
# self-extracting mode. It fails with install instructions only when RELR is
# present and no RELR-capable patchelf exists.

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

# The GUI's semantic icon atlas is SVG. Qt loads it through the SVG icon
# engine plugin (iconengines/libqsvgicon.so), which linuxdeploy-plugin-qt only
# deploys when it detects the "svg" module among the AppDir libraries. The GUI
# links no QtSvg library directly, so request the module explicitly; without it
# QIcon silently degrades to generic style icons.
EXTRA_QT_MODULES="${EXTRA_QT_MODULES:-svg}"
# The Qt platform themes and widget styles must come from the build host. Keep
# the deployment on by default so the release AppImage carries the desktop
# integration (KDE/GNOME/GTK widget styling); set DEPLOY_PLATFORM_THEMES=0 to
# opt out. The themes' native file dialogs are deliberately not used inside the
# image: they need KIO/GTK worker and portal services linuxdeploy does not
# bundle, so MainWindow forces Qt's own file dialog whenever it runs from an
# AppImage. linuxdeploy-plugin-qt only checks whether the variable is present,
# so "0" must not be exported to it.
DEPLOY_PLATFORM_THEMES="${DEPLOY_PLATFORM_THEMES:-1}"

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

# ---------------------------------------------------------------------------
# linuxdeploy portability preflight.
#
# linuxdeploy 1-alpha bundles binutils 2.35 strip and patchelf 0.15. Both
# predate SHT_RELR (".relr.dyn") support: the bundled strip rejects such
# binaries ("unknown type [0x13] section `.relr.dyn`"), and the bundled
# patchelf corrupts RELR-carrying binaries. Modern Arch, Fedora and Alpine
# toolchains emit RELR in their system libraries, so before linuxdeploy runs
# the script probes the payload and applies safe fallbacks:
#
#   - prefer a system patchelf >= 0.18 (RELR-aware) through linuxdeploy's
#     $PATCHELF override; fail with install instructions when RELR is present
#     and no RELR-capable patchelf exists;
#   - set NO_STRIP=1 when the strip linuxdeploy would use cannot parse RELR
#     (the payload is already stripped by `cmake --install --strip`);
#   - set APPIMAGE_EXTRACT_AND_RUN=1 on musl or FUSE-less hosts so linuxdeploy
#     and linuxdeploy-plugin-qt run in self-extracting mode.
#
# Callers keep control: an exported PATCHELF, NO_STRIP or
# APPIMAGE_EXTRACT_AND_RUN is honored and only reported.
# ---------------------------------------------------------------------------
PORTABILITY_TEMP_DIRS=()
cleanup_portability_temps()
{
    (( ${#PORTABILITY_TEMP_DIRS[@]} == 0 )) || rm -rf -- "${PORTABILITY_TEMP_DIRS[@]}"
}
trap cleanup_portability_temps EXIT

host_is_musl()
{
    [[ -e /lib/ld-musl-x86_64.so.1 || -e /lib/ld-musl-aarch64.so.1 ]] && return 0
    command -v ldd >/dev/null 2>&1 && ldd --version 2>&1 | grep -qi musl
}

fuse_available()
{
    [[ -c /dev/fuse && -r /dev/fuse && -w /dev/fuse ]] || return 1
    command -v fusermount3 >/dev/null 2>&1 || command -v fusermount >/dev/null 2>&1
}

# True when the ELF file carries a SHT_RELR section. readelf ships with
# binutils; the grep fallback matches the section-name string in the ELF
# header tables when readelf is unavailable.
relr_in_file()
{
    local file="$1"
    [[ -f "$file" ]] || return 1
    if command -v readelf >/dev/null 2>&1; then
        readelf -S -- "$file" 2>/dev/null | grep -q '[.]relr[.]dyn'
    else
        grep -qF '.relr.dyn' -- "$file" 2>/dev/null
    fi
}

# Print the resolved library paths of ldd output for the given files.
ldd_paths()
{
    local file
    for file in "$@"; do
        [[ -f "$file" ]] || continue
        ldd "$file" 2>/dev/null | awk '/=> \// { print $3 } /^\// { print $1 }'
    done
    return 0
}

# True for libraries linuxdeploy never bundles (the C library and dynamic
# loader family). They can carry SHT_RELR on a build host even when every
# bundled library is RELR-free, so they must not drive the fallback decision.
linuxdeploy_excluded_library()
{
    local name
    name="$(basename -- "$1")"
    case "$name" in
        libc.so.*|ld-linux*.so.*|ld-*.so.*|libanl.so.*|libBrokenLocale.so.*| \
        libdl.so.*|libm.so.*|libmvec.so.*|libnsl.so.*|libnss_*.so.*| \
        libpthread.so.*|libresolv.so.*|librt.so.*|libthread_db.so.*| \
        libutil.so.*|libcidn.so.*|libc_malloc_debug.so.*|libmemusage.so*| \
        libpcprofile.so*)
            return 0 ;;
    esac
    return 1
}

# Print the ELF files linuxdeploy may copy, strip or patch: the AppDir
# payload, the shared-library closure of the GUI and the Qt libraries and
# plugins linuxdeploy-plugin-qt deploys from the build host.
collect_linuxdeploy_candidates()
{
    local appdir="$1" qt_libs qt_plugins
    find "$appdir" -type f 2>/dev/null
    if command -v ldd >/dev/null 2>&1; then
        ldd_paths "$appdir/usr/bin/boot-repair" "$appdir/usr/libexec/boot-repair/boot-repair-helper"
        if [[ -n "${QMAKE:-}" ]] && command -v "$QMAKE" >/dev/null 2>&1; then
            qt_libs="$("$QMAKE" -query QT_INSTALL_LIBS 2>/dev/null || true)"
            if [[ -n "$qt_libs" && -d "$qt_libs" ]]; then
                ldd_paths "$qt_libs"/libQt6*.so*
            fi
            qt_plugins="$("$QMAKE" -query QT_INSTALL_PLUGINS 2>/dev/null || true)"
            if [[ -n "$qt_plugins" && -d "$qt_plugins" ]]; then
                ldd_paths "$qt_plugins"/*/*.so
            fi
        fi
    fi
    return 0
}

# Extract the linuxdeploy AppImage payload once (strip and patchelf) and cache
# the directory. --appimage-extract does not need FUSE, so it also works on
# guests where the AppImages cannot be mounted.
linuxdeploy_bundled_tool()
{
    local tool="$1" extract_dir
    [[ "$LINUXDEPLOY" == *.AppImage ]] || return 1
    if [[ -n "${_LINUXDEPLOY_EXTRACT_DIR:-}" && -d "$_LINUXDEPLOY_EXTRACT_DIR" ]]; then
        extract_dir="$_LINUXDEPLOY_EXTRACT_DIR"
    else
        extract_dir="$(mktemp -d "${TMPDIR:-/tmp}/boot-bitch-linuxdeploy.XXXXXX")"
        if ! (cd "$extract_dir" && "$LINUXDEPLOY" --appimage-extract 'usr/bin/*' >/dev/null 2>&1); then
            rm -rf -- "$extract_dir"
            return 1
        fi
        _LINUXDEPLOY_EXTRACT_DIR="$extract_dir"
        PORTABILITY_TEMP_DIRS+=("$extract_dir")
    fi
    [[ -x "$extract_dir/squashfs-root/$tool" ]] || return 1
    printf '%s\n' "$extract_dir/squashfs-root/$tool"
}

resolve_linuxdeploy_strip()
{
    local tool
    if tool="$(linuxdeploy_bundled_tool usr/bin/strip)"; then
        printf '%s\n' "$tool"
        return 0
    fi
    command -v strip 2>/dev/null
}

patchelf_handles_relr()
{
    local version major minor
    version="$("$1" --version 2>/dev/null | grep -oE '[0-9]+\.[0-9]+' | head -n1)"
    [[ -n "$version" ]] || return 1
    IFS=. read -r major minor <<<"$version"
    if (( major > 0 || (major == 0 && minor >= 18) )); then
        return 0
    fi
    return 1
}

apply_linuxdeploy_portability()
{
    local appdir="$1"
    local candidate relr_file="" strip_path="" system_patchelf="" bundled_patchelf="" version

    if [[ -n "${PATCHELF:-}" ]]; then
        echo "linuxdeploy portability: caller provided PATCHELF=$PATCHELF."
    else
        system_patchelf="$(command -v patchelf 2>/dev/null || true)"
        if [[ -n "$system_patchelf" ]] && patchelf_handles_relr "$system_patchelf"; then
            export PATCHELF="$system_patchelf"
            version="$("$PATCHELF" --version 2>/dev/null | head -n1)"
            echo "linuxdeploy portability: preferring RELR-capable system patchelf ($version)."
        fi
    fi
    if [[ -n "${NO_STRIP:-}" ]]; then
        echo "linuxdeploy portability: caller provided NO_STRIP=$NO_STRIP; stripping stays disabled."
    fi

    while IFS= read -r candidate; do
        linuxdeploy_excluded_library "$candidate" && continue
        if relr_in_file "$candidate"; then
            relr_file="$candidate"
            break
        fi
    done < <(collect_linuxdeploy_candidates "$appdir" | sort -u)

    if [[ -z "$relr_file" ]]; then
        echo "linuxdeploy portability: no SHT_RELR binaries detected; linuxdeploy's bundled patchelf is safe."
    else
        echo "linuxdeploy portability: SHT_RELR detected (first: $relr_file)."
        if [[ -z "${PATCHELF:-}" ]]; then
            if bundled_patchelf="$(linuxdeploy_bundled_tool usr/bin/patchelf)" \
                    && patchelf_handles_relr "$bundled_patchelf"; then
                echo "linuxdeploy portability: linuxdeploy's bundled patchelf is RELR-capable."
            else
                if [[ -n "$system_patchelf" ]]; then
                    version="$("$system_patchelf" --version 2>/dev/null | head -n1)"
                else
                    version="not found"
                fi
                cat >&2 <<EOF
ERROR: SHT_RELR (.relr.dyn) relocations were found (for example in
       $relr_file), but no RELR-capable patchelf is available: linuxdeploy's
       bundled patchelf 0.15 corrupts such binaries. System patchelf: $version.
       Install patchelf 0.18 or newer (the 'patchelf' package on Arch, Fedora,
       Alpine and Debian-family systems) or set PATCHELF=/path/to/patchelf,
       then re-run the build.
EOF
                exit 1
            fi
        fi
    fi

    # Probe the strip linuxdeploy would use even without RELR: on musl hosts
    # the bundled glibc strip may not run at all. A failed probe falls back to
    # NO_STRIP=1, which is always safe (the AppDir payload is already stripped
    # by `cmake --install --strip`).
    if [[ -z "${NO_STRIP:-}" ]]; then
        local probe_target="$relr_file" probe_file
        if [[ -z "$probe_target" ]]; then
            probe_target="$appdir/usr/bin/boot-repair"
        fi
        strip_path="$(resolve_linuxdeploy_strip 2>/dev/null || true)"
        if [[ -f "$probe_target" ]]; then
            probe_file="$(mktemp "${TMPDIR:-/tmp}/boot-bitch-strip-probe.XXXXXX")"
            cp -- "$probe_target" "$probe_file"
            if [[ -n "$strip_path" ]] && "$strip_path" "$probe_file" >/dev/null 2>&1; then
                if [[ -n "$relr_file" ]]; then
                    echo "linuxdeploy portability: $strip_path can parse SHT_RELR; stripping stays enabled."
                fi
            else
                export NO_STRIP=1
                if [[ -n "$relr_file" ]]; then
                    echo "linuxdeploy portability: strip (${strip_path:-not found}) cannot parse SHT_RELR; setting NO_STRIP=1 (the payload is already stripped by cmake --install --strip)."
                else
                    echo "linuxdeploy portability: strip (${strip_path:-not found}) cannot process the AppDir payload; setting NO_STRIP=1 (the payload is already stripped by cmake --install --strip)."
                fi
            fi
            rm -f -- "$probe_file"
        fi
    fi

    if [[ "${APPIMAGE_EXTRACT_AND_RUN:-}" == "1" ]]; then
        echo "linuxdeploy portability: caller provided APPIMAGE_EXTRACT_AND_RUN=1; AppImages run self-extracting."
    elif host_is_musl; then
        export APPIMAGE_EXTRACT_AND_RUN=1
        echo "linuxdeploy portability: musl host detected; setting APPIMAGE_EXTRACT_AND_RUN=1 for linuxdeploy and linuxdeploy-plugin-qt."
    elif ! fuse_available; then
        export APPIMAGE_EXTRACT_AND_RUN=1
        echo "linuxdeploy portability: FUSE is unavailable; setting APPIMAGE_EXTRACT_AND_RUN=1 for linuxdeploy and linuxdeploy-plugin-qt."
    fi
}

# Hidden probe used by scripts/test-appimage-contract.sh: run only the
# portability preflight against a caller-provided AppDir and print the
# decisions, without configuring or building anything.
if [[ "${BUILD_APPIMAGE_PROBE:-0}" == "1" ]]; then
    [[ -n "${APPDIR:-}" && -d "${APPDIR:-}" ]] || {
        echo "BUILD_APPIMAGE_PROBE=1 requires APPDIR=<existing AppDir>" >&2
        exit 1
    }
    apply_linuxdeploy_portability "$APPDIR"
    exit 0
fi

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

# The reference AppImage build host is a Debian-family desktop (TUXEDO OS) with
# plasma-integration, qt6-gtk-platformtheme, qgnomeplatform-qt6 and
# qt6-svg-plugins installed: linuxdeploy can only bundle the Qt platform
# themes, widget styles and the SVG icon engine the host provides. Warn, never
# fail, on other hosts so a local smoke-test AppImage can still be produced.
host_is_debian=0
if [[ -r /etc/os-release ]]; then
    . /etc/os-release
    case "${ID:-}" in
        debian|ubuntu|tuxedo|linuxmint|pop|elementary|zorin) host_is_debian=1 ;;
        *)
            if [[ " ${ID_LIKE:-} " == *" debian "* || " ${ID_LIKE:-} " == *" ubuntu "* ]]; then
                host_is_debian=1
            fi
            ;;
    esac
fi
if (( ! host_is_debian )); then
    echo "WARN: this is not a Debian-family host; linuxdeploy can only bundle the Qt platform theme plugins the build host provides." >&2
    echo "      Build the release AppImage on a Debian-family desktop (reference host: TUXEDO OS) with plasma-integration, qt6-gtk-platformtheme, qgnomeplatform-qt6 and qt6-svg-plugins installed," >&2
    echo "      or dialogs and file pickers fall back to Qt's default theme." >&2
fi

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

# The GUI depends on Qt plugins that are loaded at runtime, so linuxdeploy can
# only bundle them when it is told to: the SVG icon engine (the whole icon
# atlas is SVG) and the platform themes/widget styles the build host provides.
# A missing icon engine breaks the icons and fails the build; a missing
# platform theme only means Qt's fallback dialogs, so it stays a warning that
# names the reference build host.
assert_appdir_qt_plugins()
{
    local icon_engine="$APPDIR/usr/plugins/iconengines/libqsvgicon.so"
    [[ -f "$icon_engine" ]] || {
        echo "ERROR: the AppDir is missing the Qt SVG icon engine plugin: $icon_engine" >&2
        echo "       Install the host Qt SVG plugin package (for example qt6-svg-plugins on Debian) so linuxdeploy can bundle it." >&2
        exit 1
    }
    local theme themes=()
    if [[ -d "$APPDIR/usr/plugins/platformthemes" ]]; then
        for theme in "$APPDIR"/usr/plugins/platformthemes/*.so; do
            [[ -f "$theme" ]] && themes+=("$(basename -- "$theme")")
        done
    fi
    if (( ${#themes[@]} == 0 )); then
        echo "WARN: no Qt platform theme plugins were bundled; dialogs and file pickers will use Qt's fallback theme." >&2
        echo "      Build the release AppImage on a Debian-family desktop (reference host: TUXEDO OS) with plasma-integration, qt6-gtk-platformtheme and qgnomeplatform-qt6 installed." >&2
    else
        echo "Bundled Qt platform themes: ${themes[*]}"
    fi
}

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
    trap 'cleanup_portability_temps; rm -rf -- "$deploy_dir" "$plugin_dir"' EXIT
    # Probe the installed payload for SHT_RELR before linuxdeploy touches it
    # and pick the safe strip/patchelf/FUSE fallbacks (see the preflight
    # section above).
    apply_linuxdeploy_portability "$APPDIR"
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
    (
        cd "$deploy_dir"
        # linuxdeploy-plugin-qt reads these from the environment; the export is
        # scoped to this subshell so the rest of the build stays unaffected.
        export EXTRA_QT_MODULES
        if [[ "$DEPLOY_PLATFORM_THEMES" == "0" ]]; then
            # A caller may have exported DEPLOY_PLATFORM_THEMES=0; the plugin
            # only checks presence, so remove it instead of passing "0".
            unset DEPLOY_PLATFORM_THEMES
        else
            export DEPLOY_PLATFORM_THEMES=1
        fi
        PATH="$plugin_dir:$PATH" ARCH="$ARCH" QMAKE="$QMAKE" run_tool "$LINUXDEPLOY" \
            --appdir "$APPDIR" \
            --desktop-file "$APPDIR/org.bootrepair.BootRepair.desktop" \
            --icon-file "$APPDIR/org.bootrepair.BootRepair.png" \
            --plugin qt
    )
    # linuxdeploy may rewrite AppDir entries; never package a helper without
    # the executable bit, and never package without the runtime-loaded Qt
    # plugins the GUI needs.
    assert_appdir_executables
    assert_appdir_qt_plugins
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
