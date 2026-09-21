#!/usr/bin/env bash
set -euo pipefail

# Configure, build and stage-validate Boot Bitch without installing it; create
# a .deb too when running on a Debian-family host with the packaging tools and
# the versioned AppImage when the AppImage tools are available.
#
# Environment: BUILD_DIR (default build-release), BUILD_TYPE (Release), JOBS,
#              ARCH (default x86_64), APPIMAGETOOL/LINUXDEPLOY/
#              LINUXDEPLOY_PLUGIN_QT/APPIMAGE_RUNTIME_FILE (passed through to
#              build-appimage.sh).

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build-release}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 2)}"

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

# The AppImage is part of the standard release build. build-appimage.sh owns
# the tool discovery (including Development/tools) and the linuxdeploy/plugin
# fallback behavior; this check only decides whether the step can run at all.
# An explicit APPIMAGETOOL override is always passed through so a bad path is
# reported by the dedicated script instead of being silently skipped.
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

BUILD_DIR_REAL="$(realpath -m -- "$BUILD_DIR")"
ROOT_DIR_REAL="$(realpath -m -- "$ROOT_DIR")"
if [[ -z "$BUILD_DIR_REAL" || "$BUILD_DIR_REAL" == / || "$BUILD_DIR_REAL" == "$ROOT_DIR_REAL" ]]; then
    echo "Refusing to remove unsafe build directory: $BUILD_DIR" >&2
    exit 1
fi
for protected in "$ROOT_DIR_REAL/src" "$ROOT_DIR_REAL/scripts" "$ROOT_DIR_REAL/data" \
                 "$ROOT_DIR_REAL/resources" "$ROOT_DIR_REAL/tests"; do
    if [[ "$BUILD_DIR_REAL" == "$protected" || "$BUILD_DIR_REAL" == ${protected}/* \
          || "$protected" == "$BUILD_DIR_REAL" || "$protected" == ${BUILD_DIR_REAL}/* ]]; then
        echo "Refusing to remove a source/protected directory as the build directory: $BUILD_DIR" >&2
        exit 1
    fi
done
rm -rf -- "$BUILD_DIR"

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_INSTALL_PREFIX=/usr

cmake --build "$BUILD_DIR" -j"$JOBS"

echo
echo "Application build complete:"
echo "  $BUILD_DIR/boot-repair"

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

# Debian packaging is automatic when the normal Debian packaging tools exist.
if (( host_is_debian )) \
   && command -v cpack >/dev/null 2>&1 \
   && command -v dpkg-shlibdeps >/dev/null 2>&1 \
   && command -v dpkg-deb >/dev/null 2>&1
then
    (
        cd "$BUILD_DIR"
        cpack -G DEB --config CPackConfig.cmake
    )

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
else
    echo
    if (( host_is_debian )); then
        echo "Debian packaging tools were not found."
        echo "The application build is valid; .deb generation was skipped."
        echo "On Debian-family systems install dpkg-dev to enable .deb packaging."
    else
        echo "Non-Debian build host detected; .deb generation was skipped by policy."
        echo "Use package-arch.sh, package-rpm.sh, package-tarball.sh, or install.sh --source."
    fi
fi

# Build the portable AppImage in the same release run. The dedicated script
# gets a private build directory so it cannot disturb this build tree or the
# generated .deb. Missing tooling is a warning; a failing build with tooling
# present stays a hard error.
if appimagetool_available
then
    PROJECT_VERSION="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\+\([0-9][0-9.]*\).*/\1/p' \
        "$ROOT_DIR/CMakeLists.txt" | head -1)"
    [[ -n "$PROJECT_VERSION" ]] || {
        echo "Unable to determine the project version from CMakeLists.txt." >&2
        exit 1
    }
    APPIMAGE_ARCH="${ARCH:-x86_64}"
    APPIMAGE_OUTPUT="$BUILD_DIR/boot-repair_${PROJECT_VERSION}_${APPIMAGE_ARCH}.AppImage"

    echo
    echo "Building the AppImage..."
    BUILD_DIR="$BUILD_DIR/appimage" \
    BUILD_TYPE="$BUILD_TYPE" \
    JOBS="$JOBS" \
    OUTPUT="$APPIMAGE_OUTPUT" \
    "$ROOT_DIR/scripts/build-appimage.sh"

    [[ -s "$APPIMAGE_OUTPUT" ]] || {
        echo "AppImage build completed but produced no artifact: $APPIMAGE_OUTPUT" >&2
        exit 1
    }

    echo
    echo "AppImage created:"
    echo "  $APPIMAGE_OUTPUT"
else
    echo
    echo "WARN: AppImage tooling was not found; skipping the AppImage." >&2
    echo "      Install appimagetool or place it in Development/tools/ to build it." >&2
    echo "      The application build and .deb are still valid."
fi

echo
echo "Release mirror: VM-built packages (Alpine .apk, Arch .pkg.tar.zst, RPM)"
echo "are mirrored into $BUILD_DIR by Development/scripts/sync-release-artifacts.sh;"
echo "the authoritative capture stays under Development/release-<version>/."

echo
echo "Nothing was installed on the host."
