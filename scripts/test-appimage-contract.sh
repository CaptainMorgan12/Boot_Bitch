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
#      builds keep the desktop's native dialogs;
#   5. the linuxdeploy portability preflight: RELR-aware patchelf selection,
#      the NO_STRIP=1 strip fallback and APPIMAGE_EXTRACT_AND_RUN for
#      musl/no-FUSE hosts, failing closed when no safe patchelf exists.
# The static checks always run; the built artifact is inspected when
# build-release/ holds the current version's AppImage (readelf, sha1sum and
# unsquashfs are optional tooling). The portability decisions are also
# exercised functionally against synthetic AppDirs through the script's
# BUILD_APPIMAGE_PROBE hook.
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
# Static contract: both build scripts whitelist the build directory before
# rm -rf (only $ROOT_DIR/build-* / $ROOT_DIR/Development/*build* may be
# removed) with the BUILD_DIR_ALLOW_PREFIX escape hatch, and the AppImage
# script documents that the zsync update channel is not content-signed and
# registers the extracted zsyncmake dir with the EXIT cleanup trap.
# ---------------------------------------------------------------------------
require_fragment "$BUILD_APPIMAGE" 'build_dir_allowed' \
    "build-appimage.sh no longer whitelists the build directory before removal"
require_fragment "$BUILD_APPIMAGE" 'Development/"*build*' \
    "build-appimage.sh no longer whitelists the Development build areas"
require_fragment "$BUILD_APPIMAGE" 'BUILD_DIR_ALLOW_PREFIX' \
    "build-appimage.sh no longer documents the BUILD_DIR_ALLOW_PREFIX escape hatch"
require_fragment "$BUILD_APPIMAGE" 'not content-signed' \
    "build-appimage.sh no longer documents that the zsync channel is not content-signed"
require_fragment "$BUILD_APPIMAGE" 'Keep the extracted zsyncmake alive until the script exits' \
    "build-appimage.sh no longer registers the extracted zsyncmake dir with the cleanup trap"
require_fragment "$BUILD_SH" 'build_dir_allowed' \
    "build.sh no longer whitelists the build directory before removal"
require_fragment "$BUILD_SH" 'Development/"*build*' \
    "build.sh no longer whitelists the Development build areas"
require_fragment "$BUILD_SH" 'BUILD_DIR_ALLOW_PREFIX' \
    "build.sh no longer documents the BUILD_DIR_ALLOW_PREFIX escape hatch"

# ---------------------------------------------------------------------------
# Functional contract: dry-run the build-directory whitelist extracted from
# each build script (no build directory is touched). The guard must accept
# only <repo>/build-* / <repo>/Development/*build* paths (and the explicit
# BUILD_DIR_ALLOW_PREFIX prefix) and refuse everything else, so a mistyped
# BUILD_DIR can never turn the clean-build rm -rf into a data-loss accident.
# ---------------------------------------------------------------------------
guard_allowed()
{
    local source="$1" root="$2" dir="$3" allow_prefix="$4"
    ROOT_DIR_REAL="$root" BUILD_DIR_ALLOW_PREFIX="$allow_prefix" \
        bash -c "$source
build_dir_allowed \"\$1\"" guard-sh "$dir"
}

if realpath -m / >/dev/null 2>&1; then
    for build_script in "$BUILD_APPIMAGE" "$BUILD_SH"; do
        script_name="$(basename -- "$build_script")"
        guard_source="$(sed -n '/^build_dir_allowed()/,/^}/p' "$build_script")"
        [[ -n "$guard_source" ]] || fail "$script_name has no extractable build_dir_allowed guard"
        guard_allowed "$guard_source" "$ROOT_DIR" "$ROOT_DIR/build-release" "" \
            || fail "$script_name refused the default build-release directory"
        guard_allowed "$guard_source" "$ROOT_DIR" "$ROOT_DIR/build-release/appimage" "" \
            || fail "$script_name refused the build-release/appimage sub-build directory"
        guard_allowed "$guard_source" "$ROOT_DIR" "$ROOT_DIR/build-appimage" "" \
            || fail "$script_name refused the default build-appimage directory"
        guard_allowed "$guard_source" "$ROOT_DIR" "$ROOT_DIR/Development/build-foo" "" \
            || fail "$script_name refused a Development/build-* directory (the Development/*build* glob is pinned by a static fragment above)"
        guard_allowed "$guard_source" "$ROOT_DIR" "$ROOT_DIR" "" \
            && fail "$script_name accepted the repository root as a build directory"
        guard_allowed "$guard_source" "$ROOT_DIR" / "" \
            && fail "$script_name accepted / as a build directory"
        guard_allowed "$guard_source" "$ROOT_DIR" "$ROOT_DIR/src" "" \
            && fail "$script_name accepted src/ as a build directory"
        guard_allowed "$guard_source" "$ROOT_DIR" "$ROOT_DIR/legacy" "" \
            && fail "$script_name accepted the legacy tree as a build directory"
        guard_allowed "$guard_source" "$ROOT_DIR" "$HOME" "" \
            && fail "$script_name accepted HOME as a build directory"
        guard_allowed "$guard_source" "$ROOT_DIR" /tmp/boot-bitch-guard-check "" \
            && fail "$script_name accepted /tmp as a build directory"
        guard_allowed "$guard_source" "$ROOT_DIR" /tmp/boot-bitch-guard-check /tmp \
            || fail "$script_name refused BUILD_DIR_ALLOW_PREFIX=/tmp for /tmp/boot-bitch-guard-check"
        guard_allowed "$guard_source" "$ROOT_DIR" /etc/boot-bitch-guard-check /tmp \
            && fail "$script_name accepted a path outside BUILD_DIR_ALLOW_PREFIX"
        note "build-dir guard dry-run: $script_name whitelist behaves as specified"
    done
else
    note "realpath -m is unavailable; skipping the build-dir guard dry-run"
fi

# ---------------------------------------------------------------------------
# Static contract: the linuxdeploy portability preflight keeps the automatic
# fallbacks modern distro toolchains need (RELR-aware patchelf, NO_STRIP on a
# strip that cannot parse RELR, self-extracting AppImages without FUSE) and
# fails closed when no safe patchelf exists.
# ---------------------------------------------------------------------------
require_fragment "$BUILD_APPIMAGE" 'SHT_RELR' \
    "build-appimage.sh no longer mentions the SHT_RELR section"
require_fragment "$BUILD_APPIMAGE" 'relr_in_file' \
    "build-appimage.sh no longer probes ELF files for .relr.dyn"
require_fragment "$BUILD_APPIMAGE" 'linuxdeploy_excluded_library' \
    "build-appimage.sh no longer excludes the C library/loader from the RELR probe"
require_fragment "$BUILD_APPIMAGE" 'patchelf_handles_relr' \
    "build-appimage.sh no longer checks patchelf for RELR support"
require_fragment "$BUILD_APPIMAGE" 'export PATCHELF="$system_patchelf"' \
    "build-appimage.sh no longer prefers a RELR-capable system patchelf"
require_fragment "$BUILD_APPIMAGE" 'export NO_STRIP=1' \
    "build-appimage.sh no longer falls back to NO_STRIP=1"
require_fragment "$BUILD_APPIMAGE" 'host_is_musl' \
    "build-appimage.sh no longer detects musl hosts"
require_fragment "$BUILD_APPIMAGE" 'fuse_available' \
    "build-appimage.sh no longer detects FUSE availability"
require_fragment "$BUILD_APPIMAGE" 'export APPIMAGE_EXTRACT_AND_RUN=1' \
    "build-appimage.sh no longer enables self-extracting AppImage mode"
require_fragment "$BUILD_APPIMAGE" 'patchelf 0.18 or newer' \
    "build-appimage.sh no longer fails with actionable patchelf instructions"
require_fragment "$BUILD_APPIMAGE" 'BUILD_APPIMAGE_PROBE' \
    "build-appimage.sh no longer exposes the portability probe hook"
require_fragment "$BUILD_SH" 'PATCHELF' \
    "build.sh no longer documents the PATCHELF pass-through"
require_fragment "$BUILD_SH" 'APPIMAGE_EXTRACT_AND_RUN' \
    "build.sh no longer documents the APPIMAGE_EXTRACT_AND_RUN pass-through"

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
# Functional contract: drive the portability preflight through the
# BUILD_APPIMAGE_PROBE hook against synthetic AppDirs, so the fallback
# decisions are exercised without a full linuxdeploy build. The RELR-specific
# cases need a linker that understands -z pack-relative-relocs and are skipped
# when the local toolchain cannot emit a .relr.dyn section.
# ---------------------------------------------------------------------------
probe_dir="$(mktemp -d "${TMPDIR:-/tmp}/boot-bitch-appimage-probe.XXXXXX")"
cleanup_probe_dir()
{
    [[ -n "${probe_dir:-}" ]] && rm -rf -- "$probe_dir"
}
trap cleanup_probe_dir EXIT

probe_bin="$probe_dir/bin"
mkdir -p "$probe_bin"

has_relr_section()
{
    if command -v readelf >/dev/null 2>&1; then
        readelf -S -- "$1" 2>/dev/null | grep -q '[.]relr[.]dyn'
    else
        grep -qF '.relr.dyn' -- "$1" 2>/dev/null
    fi
}

# Run the build script's preflight against an AppDir with the bundled tools
# shadowed by probe scripts on PATH. Extra KEY=VALUE arguments are exported.
# QMAKE points at a path that cannot exist: an empty value falls back to the
# host qmake, and the host Qt libraries can carry SHT_RELR themselves (Alpine
# 3.24 builds Qt with RELR), which would make the synthetic RELR-free fixture
# host-dependent.
probe_run()
{
    local appdir="$1" output="$2"
    shift 2
    env -u PATCHELF -u NO_STRIP -u APPIMAGE_EXTRACT_AND_RUN "$@" \
        PATH="$probe_bin:$PATH" QMAKE=/nonexistent/qmake6 LINUXDEPLOY=/bin/true APPDIR="$appdir" \
        BUILD_APPIMAGE_PROBE=1 "$BUILD_APPIMAGE" >"$output" 2>&1
}

plain_appdir="$probe_dir/plain-appdir"
mkdir -p "$plain_appdir/usr/bin"
plain_ok=0
if command -v c++ >/dev/null 2>&1 \
        && printf 'int main(){return 0;}\n' | c++ -x c++ -o "$plain_appdir/usr/bin/boot-repair" - 2>/dev/null; then
    plain_ok=1
fi
if (( plain_ok )); then
    probe_run "$plain_appdir" "$probe_dir/plain.out" \
        || fail "portability probe failed on a RELR-free AppDir"
    grep -qF 'no SHT_RELR binaries detected' "$probe_dir/plain.out" \
        || fail "portability probe did not report a RELR-free AppDir"
    note "portability probe: RELR-free AppDir keeps the bundled tools"

    probe_run "$plain_appdir" "$probe_dir/plain-extract.out" APPIMAGE_EXTRACT_AND_RUN=1 \
        || fail "portability probe failed with APPIMAGE_EXTRACT_AND_RUN=1"
    grep -qF 'caller provided APPIMAGE_EXTRACT_AND_RUN=1' "$probe_dir/plain-extract.out" \
        || fail "portability probe did not honor APPIMAGE_EXTRACT_AND_RUN"
    note "portability probe: APPIMAGE_EXTRACT_AND_RUN is honored"
else
    note "local c++ cannot compile the probe binary; skipping the portability probe"
fi

relr_appdir="$probe_dir/relr-appdir"
mkdir -p "$relr_appdir/usr/bin"
relr_ok=0
if (( plain_ok )) \
        && c++ -Wl,-z,pack-relative-relocs -x c++ -o "$relr_appdir/usr/bin/boot-repair" - <<<'int main(){return 0;}' 2>/dev/null \
        && has_relr_section "$relr_appdir/usr/bin/boot-repair"; then
    relr_ok=1
fi
if (( relr_ok )); then
    cat > "$probe_bin/strip" <<'EOF'
#!/bin/sh
exit 1
EOF
    cat > "$probe_bin/patchelf" <<'EOF'
#!/bin/sh
echo "patchelf 0.18.0"
EOF
    chmod 755 "$probe_bin/strip" "$probe_bin/patchelf"
    if ! probe_run "$relr_appdir" "$probe_dir/relr-capable.out"; then
        fail "portability probe failed on a RELR AppDir with a capable patchelf"
    fi
    grep -qF 'preferring RELR-capable system patchelf' "$probe_dir/relr-capable.out" \
        || fail "portability probe did not prefer the RELR-capable system patchelf"
    grep -qF 'setting NO_STRIP=1' "$probe_dir/relr-capable.out" \
        || fail "portability probe did not set NO_STRIP=1 when strip cannot parse RELR"
    note "portability probe: RELR AppDir selects system patchelf and NO_STRIP=1"

    cat > "$probe_bin/strip" <<'EOF'
#!/bin/sh
exit 0
EOF
    cat > "$probe_bin/patchelf" <<'EOF'
#!/bin/sh
echo "patchelf 0.15.0"
EOF
    chmod 755 "$probe_bin/strip" "$probe_bin/patchelf"
    if probe_run "$relr_appdir" "$probe_dir/relr-old.out"; then
        fail "portability probe accepted an old patchelf for a RELR AppDir"
    fi
    grep -qF 'patchelf 0.18 or newer' "$probe_dir/relr-old.out" \
        || fail "portability probe did not fail with actionable patchelf instructions"
    note "portability probe: RELR AppDir without a capable patchelf fails closed"
else
    note "local linker cannot emit SHT_RELR; skipping the RELR portability probe"
fi

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
        trap 'cleanup_probe_dir; rm -f -- "$extracted" "$extracted_strings"' EXIT
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
