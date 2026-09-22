#!/usr/bin/env bash
# Contract test for the release AppImage.
#
# The release AppImage carries three things that are easy to lose in a refactor
# and expensive to notice after publication:
#   1. the embedded gh-releases-zsync update-information string that Gear
#      Lever/AppImageUpdate read from the .upd_info section;
#   2. the matching .zsync update metadata (Filename/URL/SHA-1) next to it;
#   3. the sha256-verified private helper staging that lets pkexec run the
#      bundled helper from an owner-only AppImage FUSE mount or a noexec
#      filesystem;
#   4. the AppImage-mode file dialog policy: the image does not carry the
#      KIO/GTK worker and portal services the platform themes' native file
#      dialogs need, so AppImage runs force Qt's own dialog while installed
#      builds keep the desktop's native dialogs.
# The static checks always run; the built artifact is inspected when
# build-release/ holds the current version's AppImage (readelf, sha1sum and
# unsquashfs are optional tooling).
set -Eeuo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_APPIMAGE="$ROOT_DIR/scripts/build-appimage.sh"
BUILD_SH="$ROOT_DIR/scripts/build.sh"
MAINWINDOW="$ROOT_DIR/src/MainWindow.cpp"
HELPER="$ROOT_DIR/scripts/boot-repair-helper.sh"
UPDATE_INFORMATION='gh-releases-zsync|CaptainMorgan12|Boot_Bitch|latest|boot-repair_*_x86_64.AppImage.zsync'

fail()
{
    echo "FAIL: $*" >&2
    exit 1
}

note()
{
    printf '  %s\n' "$*"
}

for file in "$BUILD_APPIMAGE" "$BUILD_SH" "$MAINWINDOW" "$HELPER"; do
    [[ -f "$file" ]] || fail "missing file: $file"
done
[[ -x "$HELPER" ]] || fail "the privileged helper is not executable: $HELPER"

bash -n "$BUILD_APPIMAGE" || fail "scripts/build-appimage.sh has a syntax error"
bash -n "$BUILD_SH" || fail "scripts/build.sh has a syntax error"

require_fragment()
{
    local file="$1" fragment="$2" description="$3"
    grep -qF -- "$fragment" "$file" \
        || fail "$description (missing fragment in ${file#"$ROOT_DIR"/}: $fragment)"
}

# ---------------------------------------------------------------------------
# Static contract: build-appimage.sh keeps the update metadata, the zsync
# pairing, the Qt plugin deployment and the helper executable guarantees.
# ---------------------------------------------------------------------------
require_fragment "$BUILD_APPIMAGE" "$UPDATE_INFORMATION" \
    "build-appimage.sh no longer embeds the gh-releases-zsync update information"
require_fragment "$BUILD_APPIMAGE" '--updateinformation' \
    "build-appimage.sh no longer passes the update information to appimagetool"
require_fragment "$BUILD_APPIMAGE" 'readelf -p .upd_info' \
    "build-appimage.sh no longer verifies the embedded .upd_info section"
require_fragment "$BUILD_APPIMAGE" 'resolve_zsyncmake' \
    "build-appimage.sh no longer resolves zsyncmake"
require_fragment "$BUILD_APPIMAGE" 'ZSYNC_OUTPUT="$OUTPUT.zsync"' \
    "build-appimage.sh no longer writes the .zsync next to the AppImage"
require_fragment "$BUILD_APPIMAGE" 'Filename: $(basename -- "$OUTPUT")' \
    "build-appimage.sh no longer verifies the zsync Filename header"
require_fragment "$BUILD_APPIMAGE" 'EXTRA_QT_MODULES="${EXTRA_QT_MODULES:-svg}"' \
    "build-appimage.sh no longer requests the Qt SVG icon engine module"
require_fragment "$BUILD_APPIMAGE" 'iconengines/libqsvgicon.so' \
    "build-appimage.sh no longer asserts the bundled Qt SVG icon engine"
require_fragment "$BUILD_APPIMAGE" 'DEPLOY_PLATFORM_THEMES="${DEPLOY_PLATFORM_THEMES:-1}"' \
    "build-appimage.sh no longer bundles the build host's Qt platform themes"
require_fragment "$BUILD_APPIMAGE" 'usr/libexec/boot-repair/boot-repair-helper' \
    "build-appimage.sh no longer asserts the bundled privileged helper"
require_fragment "$BUILD_APPIMAGE" 'not a Debian-family host' \
    "build-appimage.sh no longer warns on a non-Debian AppImage build host"

require_fragment "$BUILD_SH" 'scripts/build-appimage.sh' \
    "build.sh no longer builds the AppImage"
require_fragment "$BUILD_SH" 'APPIMAGE_OUTPUT.zsync' \
    "build.sh no longer requires the AppImage zsync update metadata"
require_fragment "$BUILD_SH" 'Filename: $(basename -- "$APPIMAGE_OUTPUT")' \
    "build.sh no longer verifies the zsync Filename header"

# ---------------------------------------------------------------------------
# Static contract: the GUI stages a verified private helper copy for pkexec
# whenever the bundled helper lives on an AppImage/FUSE mount or noexec
# filesystem (the AppImage runtime path).
# ---------------------------------------------------------------------------
require_fragment "$MAINWINDOW" \
    'the AppImage FUSE mount at %1 is only accessible to the user who mounted it and pkexec runs the helper as root' \
    "MainWindow no longer detects the owner-only AppImage FUSE mount"
require_fragment "$MAINWINDOW" 'the helper filesystem at %1 is mounted noexec' \
    "MainWindow no longer detects a noexec helper filesystem"
require_fragment "$MAINWINDOW" '%1; staged a private copy at %2 because %3 (sha256 %4 verified)' \
    "MainWindow no longer logs the verified private helper copy"
require_fragment "$MAINWINDOW" 'QStandardPaths::RuntimeLocation' \
    "MainWindow no longer stages the helper copy under the user runtime directory"
require_fragment "$MAINWINDOW" 'privilegedSessionHelper' \
    "MainWindow no longer exposes the privileged-session helper staging"

# ---------------------------------------------------------------------------
# Static contract: the GUI forces Qt's own file dialogs in AppImage mode and
# keys the policy off the runtime mount, not just the inherited APPIMAGE
# variable (an AppImage terminal exports APPIMAGE to installed children).
# ---------------------------------------------------------------------------
require_fragment "$MAINWINDOW" 'Qt::AA_DontUseNativeDialogs' \
    "MainWindow no longer forces Qt's own file dialogs in AppImage mode"
require_fragment "$MAINWINDOW" 'QFileDialog::DontUseNativeDialog' \
    "MainWindow no longer applies the non-native file dialog option"
require_fragment "$MAINWINDOW" 'qEnvironmentVariable("APPDIR")' \
    "MainWindow no longer ties the AppImage dialog policy to the runtime mount"

# ---------------------------------------------------------------------------
# Artifact contract: inspect the built AppImage when it is present.
# ---------------------------------------------------------------------------
VERSION="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\+\([0-9][0-9.]*\).*/\1/p' \
    "$ROOT_DIR/CMakeLists.txt" | head -1)"
[[ -n "$VERSION" ]] || fail "cannot read the project version from CMakeLists.txt"

IMAGE="$ROOT_DIR/build-release/boot-repair_${VERSION}_x86_64.AppImage"
if [[ ! -f "$IMAGE" ]]; then
    echo "PASS: AppImage contract is intact (no build-release/boot-repair_${VERSION}_x86_64.AppImage to inspect)."
    exit 0
fi
ZSYNC="$IMAGE.zsync"

# Embedded update information.
if command -v readelf >/dev/null 2>&1; then
    info="$(readelf -p .upd_info "$IMAGE" 2>/dev/null | sed -n 's/.*\]  //p' | head -n1)"
    [[ "$info" == "$UPDATE_INFORMATION" ]] \
        || fail "$(basename -- "$IMAGE") does not embed the gh-releases-zsync update information (found: ${info:-<empty>})"
    note ".upd_info: $info"
else
    note "readelf not available; skipping the .upd_info check"
fi

# zsync pairing: Filename/URL must name this AppImage and SHA-1 must match.
[[ -f "$ZSYNC" ]] \
    || fail "missing zsync update metadata next to the AppImage: $(basename -- "$ZSYNC")"
filename="$(awk -F': ' '/^Filename: / { print $2; exit }' "$ZSYNC")"
[[ "$filename" == "$(basename -- "$IMAGE")" ]] \
    || fail "$(basename -- "$ZSYNC") targets '$filename', expected $(basename -- "$IMAGE")"
url="$(awk -F': ' '/^URL: / { print $2; exit }' "$ZSYNC")"
[[ -n "$url" ]] || fail "$(basename -- "$ZSYNC") has no URL header"
case "$url" in
    "$filename"|*"/$filename") ;;
    *) fail "$(basename -- "$ZSYNC") URL '$url' does not resolve to $filename" ;;
esac
if command -v sha1sum >/dev/null 2>&1; then
    sha1="$(awk -F': ' '/^SHA-1: / { print $2; exit }' "$ZSYNC")"
    actual="$(sha1sum -- "$IMAGE")"
    actual="${actual%% *}"
    [[ "$sha1" == "$actual" ]] \
        || fail "$(basename -- "$ZSYNC") is stale for the AppImage (SHA-1 $sha1 != $actual)"
    note "zsync pairing: $filename (SHA-1 $sha1)"
else
    note "sha1sum not available; skipping the zsync SHA-1 check"
fi

# Bundled content: the privileged helper, the Qt SVG plugins, the desktop
# metadata and the helper-staging messages. unsquashfs cannot read an AppImage
# directly, so locate and validate the squashfs superblock without executing
# the artifact.
if command -v unsquashfs >/dev/null 2>&1; then
    offset=""
    while IFS=: read -r candidate _; do
        [[ "$candidate" =~ ^[0-9]+$ ]] || continue
        if unsquashfs -o "$candidate" -s "$IMAGE" >/dev/null 2>&1; then
            offset="$candidate"
            break
        fi
    done < <(grep -abo 'hsqs' "$IMAGE" 2>/dev/null || true)
    [[ -n "$offset" ]] || fail "cannot locate the squashfs superblock in $(basename -- "$IMAGE")"

    listing="$(unsquashfs -o "$offset" -ll "$IMAGE" 2>/dev/null)" \
        || fail "cannot list $(basename -- "$IMAGE")"
    grep -qF 'usr/libexec/boot-repair/boot-repair-helper' <<<"$listing" \
        || fail "$(basename -- "$IMAGE") does not bundle usr/libexec/boot-repair/boot-repair-helper"
    grep -qE '^-rwx.*usr/libexec/boot-repair/boot-repair-helper$' <<<"$listing" \
        || fail "$(basename -- "$IMAGE") bundles a non-executable privileged helper"
    grep -qF 'usr/plugins/iconengines/libqsvgicon.so' <<<"$listing" \
        || fail "$(basename -- "$IMAGE") does not bundle the Qt SVG icon engine plugin"
    grep -qF 'usr/plugins/imageformats/libqsvg.so' <<<"$listing" \
        || fail "$(basename -- "$IMAGE") does not bundle the Qt SVG image-format plugin"
    grep -qF 'usr/share/applications/org.bootrepair.BootRepair.desktop' <<<"$listing" \
        || fail "$(basename -- "$IMAGE") does not bundle the desktop entry"
    grep -qF 'usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml' <<<"$listing" \
        || fail "$(basename -- "$IMAGE") does not bundle the AppStream metadata"
    grep -qF 'usr/share/icons/hicolor/256x256/apps/org.bootrepair.BootRepair.png' <<<"$listing" \
        || fail "$(basename -- "$IMAGE") does not bundle the application icon"
    grep -qF 'org.bootrepair.BootRepair.desktop' <<<"$listing" \
        || fail "$(basename -- "$IMAGE") does not bundle the top-level desktop entry"
    note "squashfs: helper, Qt SVG plugins and desktop metadata present"

    if command -v sha256sum >/dev/null 2>&1; then
        bundled_helper="$(unsquashfs -o "$offset" -cat "$IMAGE" \
            usr/libexec/boot-repair/boot-repair-helper 2>/dev/null | sha256sum)"
        bundled_helper="${bundled_helper%% *}"
        source_helper="$(sha256sum -- "$HELPER")"
        source_helper="${source_helper%% *}"
        [[ "$bundled_helper" == "$source_helper" ]] \
            || fail "$(basename -- "$IMAGE") bundles a stale privileged helper (sha256 $bundled_helper != $source_helper)"
        note "bundled helper matches scripts/boot-repair-helper.sh"
    fi

    if command -v strings >/dev/null 2>&1; then
        extracted="$(mktemp)"
        extracted_strings="$extracted.strings"
        trap 'rm -f -- "$extracted" "$extracted_strings"' EXIT
        unsquashfs -o "$offset" -cat "$IMAGE" usr/bin/boot-repair > "$extracted" 2>/dev/null \
            || fail "cannot extract usr/bin/boot-repair from $(basename -- "$IMAGE")"
        # QStringLiteral stores UTF-16, so scan the binary as little-endian
        # wide strings once; a pipeline into grep -q would SIGPIPE strings
        # under pipefail even on a match.
        strings -a -e l "$extracted" > "$extracted_strings"
        for fragment in \
            'the AppImage FUSE mount at %1 is only accessible to the user who mounted it and pkexec runs the helper as root' \
            'the helper filesystem at %1 is mounted noexec' \
            '%1; staged a private copy at %2 because %3 (sha256 %4 verified)'; do
            grep -qF -- "$fragment" "$extracted_strings" \
                || fail "$(basename -- "$IMAGE") does not carry the helper-staging message: $fragment"
        done
        note "bundled GUI carries the helper-staging messages"
    fi

    if grep -qE 'usr/plugins/platformthemes/.+\.so' <<<"$listing"; then
        note "bundled Qt platform themes: $(grep -oE 'usr/plugins/platformthemes/[^ ]+\.so' <<<"$listing" | xargs -r -n1 basename | sort -u | tr '\n' ' ')"
    else
        echo "WARN: $(basename -- "$IMAGE") bundles no Qt platform theme plugins; dialogs and file pickers use Qt's fallback theme (build the release AppImage on a Debian-family desktop)." >&2
    fi
else
    note "unsquashfs not available; skipping the bundled-content checks"
fi

echo "PASS: AppImage update metadata, zsync pairing and helper staging contract is intact."
