#!/usr/bin/env bash
set -euo pipefail

# Configure, build and stage-validate Boot Bitch without installing it;
# create a .deb too when running on a Debian-family host with the packaging
# tools, and the versioned AppImage (+ .zsync) when --with-appimage (or
# --release) is passed and the AppImage tools are available.
#
# Defaults are incremental: without flags an existing $BUILD_DIR is reused
# (no rm -rf) and ninja rebuilds only what changed; the staged-install
# validation always runs, and the .deb/AppImage phases are checksum-gated
# through $BUILD_DIR/.build-manifest (an artifact is rebuilt only when its
# inputs changed). --clean restores the historical full-rebuild behavior
# minus the AppImage, and --release = --clean --with-appimage --fresh.
#
# Flags: --clean, --with-appimage, --release, --fresh, --force-deb,
#        --force-appimage, --skip-deb, --help.
#
# Environment: BUILD_DIR (default build-release), BUILD_TYPE (Release), JOBS,
#              ARCH (default x86_64), APPIMAGETOOL/LINUXDEPLOY/
#              LINUXDEPLOY_PLUGIN_QT/APPIMAGE_RUNTIME_FILE, and the AppImage
#              bundling knobs EXTRA_QT_MODULES (default svg) and
#              DEPLOY_PLATFORM_THEMES (default on) are passed through to
#              build-appimage.sh. Its portability preflight honors PATCHELF
#              (RELR-capable patchelf), NO_STRIP and APPIMAGE_EXTRACT_AND_RUN
#              and otherwise picks the safe fallbacks automatically. Build the
#              release AppImage on a Debian-family desktop (reference host:
#              TUXEDO OS) so the Qt platform themes and SVG icon engine can be
#              bundled; other hosts warn. NO_CCACHE=1 disables the automatic
#              ccache compiler launcher.
#              When --clean is in effect, BUILD_DIR is removed for a clean
#              build, so it must resolve (realpath) under $ROOT_DIR/build-* or
#              $ROOT_DIR/Development/*build*; anything else is refused before
#              any removal unless the one-off BUILD_DIR_ALLOW_PREFIX prefix
#              authorizes it (see the guard below).

# Package/staged-install modes must not depend on the builder's umask: a
# restrictive agent umask (for example 077) would otherwise package 0700
# directories and 0600 files. CMake also normalizes install directory modes.
umask 022

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build-release}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 2)}"

opt_clean=0
opt_with_appimage=0
opt_fresh=0
opt_force_deb=0
opt_force_appimage=0
opt_skip_deb=0
release_mode=0

usage()
{
    cat <<EOF
Usage: $0 [flags]

Build Boot Bitch into $BUILD_DIR and stage-validate the install layout
without installing anything. Without flags the run is incremental: an
existing $BUILD_DIR is reused (no rm -rf) and the Debian package and
AppImage phases are checksum-gated (an artifact is rebuilt only when its
inputs changed).

  --clean           remove $BUILD_DIR first for a full rebuild (no AppImage)
  --with-appimage   build the versioned AppImage (+ .zsync) when the
                    AppImage tooling is available (default: disabled)
  --release         same as --clean --with-appimage --fresh
  --fresh           ignore the cached artifact checksums: always rebuild
                    the Debian package and the AppImage
  --force-deb       rebuild the Debian package even when its inputs match
  --force-appimage  rebuild the AppImage even when its inputs match
  --skip-deb        never build the Debian package
  -h, --help        show this help and exit

Environment: BUILD_DIR (default build-release), BUILD_TYPE (Release), JOBS,
             ARCH (default x86_64), NO_CCACHE=1 to disable the ccache
             compiler launcher, and the AppImage overrides documented in
             scripts/build-appimage.sh.
EOF
}

while (( $# > 0 )); do
    case "$1" in
        --clean) opt_clean=1 ;;
        --with-appimage) opt_with_appimage=1 ;;
        --release) opt_clean=1; opt_with_appimage=1; opt_fresh=1; release_mode=1 ;;
        --fresh) opt_fresh=1 ;;
        --force-deb) opt_force_deb=1 ;;
        --force-appimage) opt_force_appimage=1 ;;
        --skip-deb) opt_skip_deb=1 ;;
        -h|--help) usage; exit 0 ;;
        *) usage >&2; echo "Unknown flag: $1" >&2; exit 2 ;;
    esac
    shift
done

if (( opt_skip_deb && opt_force_deb )); then
    echo "--skip-deb and --force-deb are mutually exclusive." >&2
    exit 2
fi

if (( opt_fresh )); then
    # --fresh ignores the artifact checksum caches: both packaging phases
    # rebuild regardless of the manifest.
    opt_force_deb=1
    opt_force_appimage=1
fi

if (( release_mode )); then
    echo "RELEASE MODE: all artifacts rebuilt fresh"
fi

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

need()
{
    command -v "$1" >/dev/null 2>&1 || {
        echo "Missing required build command: $1" >&2
        exit 1
    }
}

# The AppImage is part of the release build (--release/--with-appimage).
# build-appimage.sh owns the tool discovery (including Development/tools) and
# the linuxdeploy/plugin fallback behavior; this check only decides whether
# the step can run at all. An explicit APPIMAGETOOL override is always passed
# through so a bad path is reported by the dedicated script instead of being
# silently skipped.
appimagetool_available()
{
    if [[ -n "${APPIMAGETOOL:-}" ]]; then
        return 0
    fi
    if [[ -x "$ROOT_DIR/Development/tools/appimagetool-x86_64.AppImage" ]]; then
        return 0
    fi
    command -v appimagetool >/dev/null 2>&1
}

for cmd in cmake ninja c++
do
    need "$cmd"
done

# Fail early if the shipped privileged helper is malformed.
bash -n "$ROOT_DIR/scripts/boot-repair-helper.sh"

PROJECT_VERSION="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\+\([0-9][0-9.]*\).*/\1/p' \
    "$ROOT_DIR/CMakeLists.txt" | head -1)"
[[ -n "$PROJECT_VERSION" ]] || {
    echo "Unable to determine the project version from CMakeLists.txt." >&2
    exit 1
}

# Whitelist the build directory before rm -rf: only a path that resolves
# under $ROOT_DIR/build-* or $ROOT_DIR/Development/*build* (which covers
# Development/build-*) may be removed. Everything else — $HOME, /tmp,
# $ROOT_DIR/legacy or any other repo subdirectory — is refused with a clear
# message before any removal. BUILD_DIR_ALLOW_PREFIX is the documented escape
# hatch for one-off out-of-tree builds: when set, the build directory may
# instead live under that prefix (its own realpath).
BUILD_DIR_REAL="$(realpath -m -- "$BUILD_DIR")"
ROOT_DIR_REAL="$(realpath -m -- "$ROOT_DIR")"
build_dir_allowed()
{
    local dir="$1"
    [[ -n "$dir" ]] || return 1
    [[ "$dir" == "$ROOT_DIR_REAL/build-"* ]] && return 0
    [[ "$dir" == "$ROOT_DIR_REAL/Development/"*build* ]] && return 0
    if [[ -n "${BUILD_DIR_ALLOW_PREFIX:-}" ]]; then
        local prefix
        prefix="$(realpath -m -- "$BUILD_DIR_ALLOW_PREFIX")"
        if [[ "$prefix" == / ]]; then
            [[ "$dir" == /* ]] && return 0
        elif [[ -n "$prefix" && "$dir" == "$prefix"/* ]]; then
            return 0
        fi
    fi
    return 1
}
if ! build_dir_allowed "$BUILD_DIR_REAL"; then
    cat >&2 <<EOF
Refusing to remove unsafe build directory: $BUILD_DIR (resolves to $BUILD_DIR_REAL).
The build directory must live under $ROOT_DIR/build-* or
$ROOT_DIR/Development/*build*; set BUILD_DIR_ALLOW_PREFIX=<parent> to authorize
a one-off out-of-tree location under that prefix. Nothing was removed.
EOF
    exit 1
fi
if (( opt_clean )); then
    rm -rf -- "$BUILD_DIR"
fi

# ---------------------------------------------------------------------------
# Checksum-gated caching. $BUILD_DIR/.build-manifest records one "key sha256"
# line per gated phase; a phase is skipped when its artifact already exists
# and the current input hash matches the recorded one (unless forced). The
# file is rewritten atomically (temp + mv) on every update.
# ---------------------------------------------------------------------------
MANIFEST_FILE="$BUILD_DIR/.build-manifest"
declare -A BUILD_MANIFEST=()

manifest_load()
{
    BUILD_MANIFEST=()
    local key value
    if [[ -f "$MANIFEST_FILE" ]]; then
        while read -r key value; do
            [[ -n "$key" && -n "$value" ]] || continue
            BUILD_MANIFEST["$key"]="$value"
        done < "$MANIFEST_FILE"
    fi
}

manifest_get()
{
    local key="$1"
    printf '%s' "${BUILD_MANIFEST[$key]:-}"
}

manifest_write()
{
    local tmp
    tmp="$(mktemp "$BUILD_DIR/.build-manifest.XXXXXX")"
    local key
    for key in "${!BUILD_MANIFEST[@]}"; do
        printf '%s %s\n' "$key" "${BUILD_MANIFEST[$key]}" >> "$tmp"
    done
    sort -o "$tmp" "$tmp"
    mv -- "$tmp" "$MANIFEST_FILE"
}

manifest_set()
{
    BUILD_MANIFEST["$1"]="$2"
    manifest_write
}

# One digest over the presence and content of the given files: every file
# contributes either "<sha256>  <path>" or a MISSING marker (a missing tool
# hashes differently from an installed one), and the whole stream is hashed
# once. Sorting makes the result independent of the argument order.
sha256_inputs()
{
    local tmp
    tmp="$(mktemp "${TMPDIR:-/tmp}/boot-bitch-build-inputs.XXXXXX")"
    : > "$tmp"
    local file
    for file in "$@"; do
        if [[ -f "$file" ]]; then
            sha256sum -- "$file" >> "$tmp"
        else
            printf 'MISSING  %s\n' "$file" >> "$tmp"
        fi
    done
    sort "$tmp" | sha256sum | cut -d' ' -f1
    rm -f -- "$tmp"
}

# The staged-install digest: one sha256 over the sorted content hashes of
# every file under the stage directory, so any binary/helper/asset byte
# change invalidates the deb and appimage keys that build on it.
stage_manifest_hash()
{
    find "$STAGE_DIR" -type f -exec sha256sum -- {} + | cut -d' ' -f1 | sort | sha256sum | cut -d' ' -f1
}

# The Qt libraries and plugins linuxdeploy bundles come from the build host
# and are not part of the stage digest. Hash the libraries in the built
# binary's loader closure plus every plugin under QT_INSTALL_PLUGINS: a
# conservative superset of what linuxdeploy/linuxdeploy-plugin-qt deploy, so
# a host Qt upgrade invalidates the cached AppImage.
qt_files_hash()
{
    local qmake_cmd="${QMAKE:-}"
    [[ -n "$qmake_cmd" ]] || qmake_cmd="$(command -v qmake6 2>/dev/null || command -v qmake 2>/dev/null || true)"
    local tmp
    tmp="$(mktemp "${TMPDIR:-/tmp}/boot-bitch-qt-hash.XXXXXX")"
    : > "$tmp"
    if command -v ldd >/dev/null 2>&1 && [[ -f "$BUILD_DIR/boot-repair" ]]; then
        local lib
        while IFS= read -r lib; do
            [[ -n "$lib" && -f "$lib" ]] || continue
            sha256sum -- "$lib" >> "$tmp"
        done < <(ldd "$BUILD_DIR/boot-repair" 2>/dev/null \
            | awk '/=> \// { print $3 } /^\// { print $1 }' | sort -u)
    else
        printf 'MISSING  loader-closure\n' >> "$tmp"
    fi
    if [[ -n "$qmake_cmd" ]] && command -v "$qmake_cmd" >/dev/null 2>&1; then
        local plugins
        plugins="$("$qmake_cmd" -query QT_INSTALL_PLUGINS 2>/dev/null || true)"
        if [[ -n "$plugins" && -d "$plugins" ]]; then
            find "$plugins" -type f -exec sha256sum -- {} + >> "$tmp"
        else
            printf 'MISSING  %s\n' "${plugins:-qt-install-plugins}" >> "$tmp"
        fi
    else
        printf 'MISSING  qmake\n' >> "$tmp"
    fi
    cut -d' ' -f1 < "$tmp" | sort | sha256sum | cut -d' ' -f1
    rm -f -- "$tmp"
}

# The AppImage toolchain files, mirroring build-appimage.sh's discovery
# (explicit override, Development/tools/, then PATH). Unresolvable entries
# hash as MISSING so installing or removing a tool changes the key.
appimage_tool_inputs_hash()
{
    local tmp
    tmp="$(mktemp "${TMPDIR:-/tmp}/boot-bitch-tools-hash.XXXXXX")"
    : > "$tmp"
    local spec file
    for spec in \
        "${APPIMAGETOOL:-appimagetool}" \
        "$ROOT_DIR/Development/tools/appimagetool-x86_64.AppImage" \
        "${LINUXDEPLOY:-linuxdeploy}" \
        "$ROOT_DIR/Development/tools/linuxdeploy-x86_64.AppImage" \
        "${LINUXDEPLOY_PLUGIN_QT:-linuxdeploy-plugin-qt}" \
        "$ROOT_DIR/Development/tools/linuxdeploy-plugin-qt-x86_64.AppImage" \
        "${APPIMAGE_RUNTIME_FILE:-}" \
        "$ROOT_DIR/Development/tools/runtime-x86_64" \
        "${ZSYNCMAKE:-}" \
        "$ROOT_DIR/Development/tools/zsyncmake"; do
        [[ -n "$spec" ]] || continue
        if [[ "$spec" == */* ]]; then
            file="$spec"
        else
            file="$(command -v "$spec" 2>/dev/null || true)"
        fi
        if [[ -n "$file" && -f "$file" ]]; then
            sha256sum -- "$file" >> "$tmp"
        else
            printf 'MISSING  %s\n' "$spec" >> "$tmp"
        fi
    done
    cut -d' ' -f1 < "$tmp" | sort | sha256sum | cut -d' ' -f1
    rm -f -- "$tmp"
}

compute_deb_key()
{
    {
        printf 'stage %s\n' "${STAGE_KEY:-}"
        printf 'packaging %s\n' "$(sha256_inputs \
            "$BUILD_DIR/CPackConfig.cmake" \
            "$ROOT_DIR/CMakeLists.txt" \
            "$ROOT_DIR/scripts/postinst" \
            "$ROOT_DIR/scripts/cpack-rewrite-desktop-icon.cmake")"
    } | sha256sum | cut -d' ' -f1
}

compute_appimage_key()
{
    {
        printf 'stage %s\n' "${STAGE_KEY:-}"
        printf 'script %s\n' "$(sha256sum -- "$ROOT_DIR/scripts/build-appimage.sh" | cut -d' ' -f1)"
        printf 'version %s\n' "$PROJECT_VERSION"
        printf 'tools %s\n' "$(appimage_tool_inputs_hash)"
        printf 'qt %s\n' "$(qt_files_hash)"
    } | sha256sum | cut -d' ' -f1
}

configure_and_build()
{
    local configure_args=()
    configure_args+=(-S "$ROOT_DIR" -B "$BUILD_DIR" -G Ninja)
    configure_args+=("-DCMAKE_BUILD_TYPE=$BUILD_TYPE")
    configure_args+=(-DCMAKE_INSTALL_PREFIX=/usr)
    if command -v ccache >/dev/null 2>&1 && [[ -z "${NO_CCACHE:-}" ]]; then
        configure_args+=(-DCMAKE_CXX_COMPILER_LAUNCHER=ccache)
        echo "ccache detected: enabling CMAKE_CXX_COMPILER_LAUNCHER=ccache (set NO_CCACHE=1 to disable)."
    fi
    cmake "${configure_args[@]}"
    cmake --build "$BUILD_DIR" -j"$JOBS"

    echo
    echo "Application build complete:"
    echo "  $BUILD_DIR/boot-repair"
}

stage_and_validate()
{
    # Validate the install layout without installing anything on the host.
    STAGE_DIR="$BUILD_DIR/stage"
    rm -rf -- "$STAGE_DIR"

    if [[ "$BUILD_TYPE" == "Release" || "$BUILD_TYPE" == "RelWithDebInfo" || "$BUILD_TYPE" == "MinSizeRel" ]]
    then
        DESTDIR="$STAGE_DIR" cmake --install "$BUILD_DIR" --strip
    else
        DESTDIR="$STAGE_DIR" cmake --install "$BUILD_DIR"
    fi

    require_staged_file()
    {
        local path="$1"
        local description="$2"
        [[ -f "$path" ]] || {
            echo "Staged $description missing: $path" >&2
            exit 1
        }
    }

    [[ -x "$STAGE_DIR/usr/bin/boot-repair" ]] || {
        echo "Staged executable missing: $STAGE_DIR/usr/bin/boot-repair" >&2
        exit 1
    }

    require_staged_file \
        "$STAGE_DIR/usr/share/applications/org.bootrepair.BootRepair.desktop" \
        "desktop entry"

    require_staged_file \
        "$STAGE_DIR/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml" \
        "AppStream metadata"

    [[ -x "$STAGE_DIR/usr/libexec/boot-repair/boot-repair-helper" ]] || {
        echo "Staged privileged helper missing: $STAGE_DIR/usr/libexec/boot-repair/boot-repair-helper" >&2
        exit 1
    }

    require_staged_file \
        "$STAGE_DIR/usr/share/doc/boot-repair/copyright" \
        "MIT copyright/license file"

    # 0.2.11+ packages a full KDE/Freedesktop hicolor icon set. Check several
    # representative sizes here so a source/package omission fails the build.
    for icon_size in 16 48 128 256 512 1024
    do
        require_staged_file \
            "$STAGE_DIR/usr/share/icons/hicolor/${icon_size}x${icon_size}/apps/org.bootrepair.BootRepair.png" \
            "${icon_size}px application icon"
    done

    if command -v desktop-file-validate >/dev/null 2>&1
    then
        desktop-file-validate \
            "$STAGE_DIR/usr/share/applications/org.bootrepair.BootRepair.desktop"
    fi

    if command -v appstreamcli >/dev/null 2>&1
    then
        # appstreamcli treats advisory metadata (for example, a missing project
        # homepage) as a failed validation. Keep packaging usable for this local
        # utility while still surfacing those advisories to the maintainer.
        if ! appstreamcli validate \
            "$STAGE_DIR/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml"
        then
            echo "WARN: AppStream metadata has advisory validation findings; XML is installed as authored."
        fi
    fi

    echo "Staged install validation: PASS"

    STAGE_KEY="$(stage_manifest_hash)"
    manifest_set stage "$STAGE_KEY"
}

# Debian packaging is automatic when the normal Debian packaging tools exist.
build_deb()
{
    if (( opt_skip_deb )); then
        echo
        echo "Debian package phase skipped (--skip-deb)."
        return 0
    fi

    if ! (( host_is_debian )) \
       || ! command -v cpack >/dev/null 2>&1 \
       || ! command -v dpkg-shlibdeps >/dev/null 2>&1 \
       || ! command -v dpkg-deb >/dev/null 2>&1
    then
        echo
        if (( host_is_debian )); then
            echo "Debian packaging tools were not found."
            echo "The application build is valid; .deb generation was skipped."
            echo "On Debian-family systems install dpkg-dev to enable .deb packaging."
        else
            echo "Non-Debian build host detected; .deb generation was skipped by policy."
            echo "Use package-arch.sh, package-rpm.sh, package-tarball.sh, or install.sh --source."
        fi
        return 0
    fi

    local deb_key deb_present=0
    deb_key="$(compute_deb_key)"

    # The mirrored legacy user-test package (boot-repair-legacy_*.deb) must not
    # satisfy the cache gate for CPack's own boot-repair_<version>_<arch>.deb.
    if compgen -G "$BUILD_DIR/boot-repair_*.deb" >/dev/null 2>&1; then
        deb_present=1
    fi

    if (( deb_present )) \
            && [[ "$(manifest_get deb)" == "$deb_key" ]] \
            && (( ! opt_force_deb )); then
        echo
        echo "SKIPPED: deb (inputs unchanged, sha256 ${deb_key:0:12})"
    else
        (
            cd "$BUILD_DIR"
            cpack -G DEB --config CPackConfig.cmake
        )
        manifest_set deb "$deb_key"
    fi

    mapfile -t packages < <(
        find "$BUILD_DIR" -maxdepth 1 -type f -name '*.deb' -print | sort
    )

    [[ ${#packages[@]} -gt 0 ]] || {
        echo "CPack ran but no .deb was generated." >&2
        exit 1
    }

    echo
    echo "Debian package created:"
    printf '  %s\n' "${packages[@]}"
    echo
    echo "Validate without installing:"
    echo "  $ROOT_DIR/scripts/test-deb.sh ${packages[0]}"
}

# Build the portable AppImage (with its .zsync update metadata and the bundled
# Qt SVG icon engine/platform themes) when requested. The dedicated script
# gets a private build directory so it cannot disturb this build tree or the
# generated .deb. Missing tooling is a warning; a failing build with tooling
# present stays a hard error.
build_appimage()
{
    if (( ! opt_with_appimage )); then
        echo
        echo "AppImage phase not enabled; pass --with-appimage (or --release) to build the versioned AppImage."
        return 0
    fi

    if ! appimagetool_available; then
        echo
        echo "WARN: AppImage tooling was not found; skipping the AppImage." >&2
        echo "      Install appimagetool or place it in Development/tools/ to build it." >&2
        echo "      The application build and .deb are still valid."
        return 0
    fi

    APPIMAGE_ARCH="${ARCH:-x86_64}"
    APPIMAGE_OUTPUT="$BUILD_DIR/boot-repair_${PROJECT_VERSION}_${APPIMAGE_ARCH}.AppImage"

    local appimage_key
    appimage_key="$(compute_appimage_key)"

    # zsync update metadata is only ever regenerated together with its
    # AppImage, so the skip rule requires both artifacts and one manifest key.
    if [[ -s "$APPIMAGE_OUTPUT" && -s "$APPIMAGE_OUTPUT.zsync" ]] \
            && [[ "$(manifest_get appimage)" == "$appimage_key" ]] \
            && (( ! opt_force_appimage )); then
        echo
        echo "SKIPPED: appimage (inputs unchanged, sha256 ${appimage_key:0:12})"
    else
        echo
        echo "Building the AppImage..."
        BUILD_DIR="$BUILD_DIR/appimage" \
        BUILD_TYPE="$BUILD_TYPE" \
        JOBS="$JOBS" \
        OUTPUT="$APPIMAGE_OUTPUT" \
        "$ROOT_DIR/scripts/build-appimage.sh"
        manifest_set appimage "$appimage_key"
    fi

    [[ -s "$APPIMAGE_OUTPUT" ]] || {
        echo "AppImage build completed but produced no artifact: $APPIMAGE_OUTPUT" >&2
        exit 1
    }
    [[ -s "$APPIMAGE_OUTPUT.zsync" ]] || {
        echo "AppImage build completed but produced no zsync update metadata: $APPIMAGE_OUTPUT.zsync" >&2
        exit 1
    }
    # The .zsync must pair with this exact AppImage: zsync clients resolve the
    # relative URL against the .zsync asset location, so the Filename header
    # stays the release asset basename.
    grep -qF "Filename: $(basename -- "$APPIMAGE_OUTPUT")" "$APPIMAGE_OUTPUT.zsync" || {
        echo "AppImage zsync update metadata does not target $(basename -- "$APPIMAGE_OUTPUT"): $APPIMAGE_OUTPUT.zsync" >&2
        exit 1
    }

    echo
    echo "AppImage created:"
    echo "  $APPIMAGE_OUTPUT"
    echo "  $APPIMAGE_OUTPUT.zsync"
}

manifest_load
configure_and_build
stage_and_validate
build_deb
build_appimage

echo
echo "Release mirror: VM-built packages (Alpine .apk, Arch .pkg.tar.zst, RPM)"
echo "are mirrored into $BUILD_DIR by Development/scripts/sync-release-artifacts.sh;"
echo "the authoritative capture stays under Development/release-<version>/."

echo
echo "Nothing was installed on the host."
