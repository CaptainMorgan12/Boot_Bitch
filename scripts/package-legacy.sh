#!/usr/bin/env bash
# Build the legacy Debian Etch package (boot-repair-legacy) with dpkg-deb.
#
# The legacy artifact must be built natively inside the Debian 4.0 "etch"
# guest (dpkg-deb 1.13.26 + fakeroot 1.5.10); this script refuses a real build
# anywhere else so a modern host package can never be mistaken for it.
# --dry-run only validates and stages the package tree and works on any host.
#
# This script itself runs on Etch: it is bash 3.1 clean (no bash-4-only
# builtins, no case-conversion expansions, no associative arrays).
#
# Usage:
#   scripts/package-legacy.sh [--output DIR] [--stage-dir DIR] [--dry-run]
#
# The Qt3 GUI (legacy/gui/) is built natively with qmake-qt3 + make when
# qmake-qt3 is available; --dry-run on a host without Qt3 stages a placeholder
# GUI binary so the layout/control contract stays testable. The GUI is the
# package's only entry point: the shell-only `boot-repair-legacy` TUI launcher
# (legacy/launcher/) is development-only and is deliberately NOT staged.
#
# Helper source coordination (the port agent owns the generated helper):
#   LEGACY_HELPER_SRC      source path, absolute or relative to the repo root.
#                          Default discovery order (legacy/boot-repair-helper.sh
#                          is what legacy/compat.sh and legacy/port.sh
#                          document; the rest are plan-era fallbacks):
#                            legacy/boot-repair-helper.sh
#                            legacy/helper/boot-repair-helper-legacy.sh
#                            legacy/boot-repair-helper-legacy.sh
#                            scripts/legacy/boot-repair-helper-legacy.sh
#                            scripts/boot-repair-legacy-helper.sh
#   LEGACY_ARCH            package architecture, default amd64.
#   LEGACY_OUTPUT_DIR      output directory, default Development/build-legacy-package.
#   LEGACY_STAGE_DIR       staging root, default <output>/stage.
#   LEGACY_GUI_BUILD_DIR   Qt3 shadow build directory, default
#                          <output>/gui-build.
set -eu
# Package modes must not depend on the builder's umask.
umask 022

PROGRAM_NAME="${0##*/}"
ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"

PKG_NAME='boot-repair-legacy'
PKG_RELEASE='etch1'
ARCH="${LEGACY_ARCH:-amd64}"

OUTPUT_DIR="${LEGACY_OUTPUT_DIR:-$ROOT_DIR/Development/build-legacy-package}"
STAGE_DIR_OVERRIDE="${LEGACY_STAGE_DIR:-}"
DRY_RUN=0

HELPER_SRC_ENV="${LEGACY_HELPER_SRC:-}"
GUI_BUILD_DIR_OVERRIDE="${LEGACY_GUI_BUILD_DIR:-}"

CONTROL_TEMPLATE="$ROOT_DIR/legacy/packaging/control.in"
COPYRIGHT_SRC="$ROOT_DIR/legacy/packaging/copyright"
CHANGELOG_SRC="$ROOT_DIR/legacy/packaging/changelog.Debian"
PORT_TOOL="$ROOT_DIR/legacy/port.sh"
PORT_TOOL_FALLBACK="$ROOT_DIR/legacy/tools/port-helper.sh"

GUI_NAME='boot-repair-legacy-gui'
GUI_PRO="$ROOT_DIR/legacy/gui/boot-bitch-legacy.pro"
GUI_DESKTOP_SRC="$ROOT_DIR/legacy/gui/data/boot-repair-legacy-gui.desktop"
GUI_ICON_DIR="$ROOT_DIR/legacy/gui/data"
GUI_ICON_SIZES='16 22 32 48'

usage()
{
    cat <<EOF
Usage: $PROGRAM_NAME [options]

Builds ${PKG_NAME}_${PROJECT_VERSION:-<version>}-${PKG_RELEASE}_${ARCH}.deb from
the staged legacy tree. A real build only runs inside the Debian 4.0 "etch"
guest; --dry-run works anywhere.

Options:
  --output DIR     write the .deb (and the default stage tree) under DIR
  --stage-dir DIR  stage the package tree at DIR instead of <output>/stage
  --dry-run        validate and stage only; never invoke dpkg-deb/fakeroot
  -h, --help       show this help

Environment:
  LEGACY_HELPER_SRC      ported helper source (see the script header)
  LEGACY_ARCH            package architecture (default amd64)
  LEGACY_OUTPUT_DIR      output directory
  LEGACY_STAGE_DIR       staging root
  LEGACY_GUI_BUILD_DIR   Qt3 shadow build directory (default <output>/gui-build)
EOF
}

fail()
{
    printf '%s: %s\n' "$PROGRAM_NAME" "$*" >&2
    exit 1
}

is_etch_host()
{
    [ -r /etc/debian_version ] || return 1
    _debian_version="$(sed -n '1p' /etc/debian_version 2>/dev/null || true)"
    case "$_debian_version" in
        4.0*|*[Ee]tch*) return 0 ;;
    esac
    return 1
}

find_helper()
{
    if [ -n "$HELPER_SRC_ENV" ]; then
        case "$HELPER_SRC_ENV" in
            /*) printf '%s\n' "$HELPER_SRC_ENV" ;;
            *) printf '%s\n' "$ROOT_DIR/$HELPER_SRC_ENV" ;;
        esac
        return 0
    fi
    for _candidate in \
        "$ROOT_DIR/legacy/boot-repair-helper.sh" \
        "$ROOT_DIR/legacy/helper/boot-repair-helper-legacy.sh" \
        "$ROOT_DIR/legacy/boot-repair-helper-legacy.sh" \
        "$ROOT_DIR/scripts/legacy/boot-repair-helper-legacy.sh" \
        "$ROOT_DIR/scripts/boot-repair-legacy-helper.sh"
    do
        if [ -f "$_candidate" ]; then
            printf '%s\n' "$_candidate"
            return 0
        fi
    done
    return 1
}

run_drift_check()
{
    if [ -n "$HELPER_SRC_ENV" ]; then
        printf 'NOTE: LEGACY_HELPER_SRC override set; skipping the generated-helper drift gate.\n'
        return 0
    fi
    _tool=''
    for _candidate in "$PORT_TOOL" "$PORT_TOOL_FALLBACK"; do
        if [ -f "$_candidate" ]; then
            _tool="$_candidate"
            break
        fi
    done
    if [ -n "$_tool" ]; then
        printf 'Checking generated legacy helper drift (%s --check)...\n' "${_tool#"$ROOT_DIR"/}"
        ( cd -- "$ROOT_DIR" && bash "$_tool" --check ) \
            || fail "$_tool --check failed; regenerate the legacy helper before packaging."
    else
        printf 'NOTE: %s not present; skipping the generated-helper drift gate.\n' \
            "${PORT_TOOL#"$ROOT_DIR"/}"
    fi
}

# Build the Qt3 GUI with qmake-qt3/make into a shadow build directory. Returns
# nonzero when the toolchain is unavailable during --dry-run (the caller then
# stages a placeholder so the layout contract still validates on any host).
build_gui()
{
    _gui_build="$1"
    if [ -z "$(command -v qmake-qt3 2>/dev/null || true)" ] \
        || [ -z "$(command -v make 2>/dev/null || true)" ]; then
        if [ "$DRY_RUN" -eq 1 ]; then
            printf 'NOTE: qmake-qt3/make not found; staging a placeholder GUI binary (dry run only).\n'
            return 1
        fi
        fail 'qmake-qt3 and make are required to build the legacy GUI (install qt3-dev-tools and g++).'
    fi
    [ -f "$GUI_PRO" ] || fail "missing Qt3 GUI project: $GUI_PRO"
    printf 'Building legacy Qt3 GUI (qmake-qt3 + make)...\n'
    rm -rf -- "$_gui_build"
    mkdir -p -- "$_gui_build"
    (
        cd -- "$_gui_build" || exit 1
        BOOT_REPAIR_LEGACY_VERSION="$PROJECT_VERSION" qmake-qt3 "$GUI_PRO" || exit 1
        make || exit 1
    ) || fail "legacy GUI build failed (see the output above); expected $GUI_NAME."
    [ -x "$_gui_build/$GUI_NAME" ] \
        || fail "legacy GUI build produced no executable: $_gui_build/$GUI_NAME"
    return 0
}

stage_gui()
{
    _stage="$1"
    _gui_build="$GUI_BUILD_DIR"
    if build_gui "$_gui_build"; then
        install -m 0755 "$_gui_build/$GUI_NAME" "$_stage/usr/bin/$GUI_NAME"
    else
        # Dry-run placeholder: proves the staged layout/desktop/icon contract
        # without a Qt3 toolchain. A real build always installs the binary.
        printf '#!/bin/sh\n# dry-run placeholder: build on Etch with qmake-qt3.\necho "%s: dry-run placeholder (no Qt3 toolchain on this host)" >&2\nexit 0\n' \
            "$GUI_NAME" > "$_stage/usr/bin/$GUI_NAME"
        chmod 0755 "$_stage/usr/bin/$GUI_NAME"
    fi
    install -m 0644 "$GUI_DESKTOP_SRC" \
        "$_stage/usr/share/applications/$GUI_NAME.desktop"
    for _size in $GUI_ICON_SIZES; do
        _icon="$GUI_ICON_DIR/boot-repair-legacy-${_size}x${_size}.png"
        [ -f "$_icon" ] || fail "missing legacy icon: $_icon"
        mkdir -p "$_stage/usr/share/icons/hicolor/${_size}x${_size}/apps"
        install -m 0644 "$_icon" \
            "$_stage/usr/share/icons/hicolor/${_size}x${_size}/apps/boot-repair-legacy.png"
    done
}

stage_tree()
{
    _stage="$1"
    case "$_stage" in
        ''|'/'|'.'|'..') fail "refusing to stage into '$_stage'" ;;
    esac
    rm -rf -- "$_stage"
    mkdir -p \
        "$_stage/DEBIAN" \
        "$_stage/usr/sbin" \
        "$_stage/usr/bin" \
        "$_stage/usr/share/applications" \
        "$_stage/usr/share/doc/$PKG_NAME"

    install -m 0755 "$HELPER_SRC" "$_stage/usr/sbin/boot-repair-legacy-helper"
    install -m 0644 "$COPYRIGHT_SRC" "$_stage/usr/share/doc/$PKG_NAME/copyright"
    gzip -9 -n -c "$CHANGELOG_SRC" > "$_stage/usr/share/doc/$PKG_NAME/changelog.Debian.gz"

    stage_gui "$_stage"

    bash -n "$_stage/usr/sbin/boot-repair-legacy-helper" \
        || fail 'staged helper failed bash -n.'

    _installed_size="$(du -sk "$_stage/usr" | awk '{print $1}')"
    sed -e "s|@VERSION@|$PROJECT_VERSION-$PKG_RELEASE|g" \
        -e "s|@ARCH@|$ARCH|g" \
        -e "s|@INSTALLED_SIZE@|$_installed_size|g" \
        "$CONTROL_TEMPLATE" > "$_stage/DEBIAN/control"
    chmod 0644 "$_stage/DEBIAN/control"
}

validate_artifact()
{
    _deb="$1"
    dpkg-deb --info "$_deb"
    dpkg-deb --contents "$_deb"
    [ "$(dpkg-deb --field "$_deb" Package)" = "$PKG_NAME" ] \
        || fail "built package has the wrong Package field"
    [ "$(dpkg-deb --field "$_deb" Version)" = "$PROJECT_VERSION-$PKG_RELEASE" ] \
        || fail "built package has the wrong Version field"
    [ "$(dpkg-deb --field "$_deb" Architecture)" = "$ARCH" ] \
        || fail "built package has the wrong Architecture field"
    dpkg-deb --field "$_deb" Depends | grep -q 'cryptsetup' \
        || fail "built package Depends is missing cryptsetup"
    dpkg-deb --field "$_deb" Depends | grep -q 'libqt3-mt' \
        || fail "built package Depends is missing the Qt3 runtime libqt3-mt"
    for _path in \
        ./usr/sbin/boot-repair-legacy-helper \
        ./usr/bin/$GUI_NAME \
        ./usr/share/applications/$GUI_NAME.desktop \
        ./usr/share/icons/hicolor/48x48/apps/boot-repair-legacy.png \
        ./usr/share/doc/$PKG_NAME/copyright
    do
        if ! dpkg-deb --contents "$_deb" | grep -q -- "$_path\$"; then
            fail "built package is missing $_path"
        fi
    done
    # The shell-only TUI launcher (and its desktop entry/man page) is
    # development-only and must never ship: the GUI is the entry point.
    for _path in \
        ./usr/bin/boot-repair-legacy \
        ./usr/share/applications/$PKG_NAME.desktop \
        ./usr/share/man/man1/$PKG_NAME.1.gz
    do
        if dpkg-deb --contents "$_deb" | grep -q -- "$_path\$"; then
            fail "built package still ships the removed launcher path $_path"
        fi
    done
    sha256sum "$_deb"
}

main()
{
    while [ $# -gt 0 ]; do
        case "$1" in
            --output)
                shift
                [ $# -gt 0 ] || fail '--output requires a directory'
                OUTPUT_DIR="$1"
                ;;
            --output=*) OUTPUT_DIR="${1#--output=}" ;;
            --stage-dir)
                shift
                [ $# -gt 0 ] || fail '--stage-dir requires a directory'
                STAGE_DIR_OVERRIDE="$1"
                ;;
            --stage-dir=*) STAGE_DIR_OVERRIDE="${1#--stage-dir=}" ;;
            -n|--dry-run) DRY_RUN=1 ;;
            -h|--help) usage; exit 0 ;;
            *) usage >&2; fail "unknown option: $1" ;;
        esac
        shift
    done

    [ -f "$ROOT_DIR/CMakeLists.txt" ] || fail "cannot find CMakeLists.txt under $ROOT_DIR"
    PROJECT_VERSION="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\{1,\}\([0-9][0-9.]*\).*/\1/p' \
        "$ROOT_DIR/CMakeLists.txt" | head -1)"
    [ -n "$PROJECT_VERSION" ] || fail 'unable to determine the project version from CMakeLists.txt.'

    for _required in "$CONTROL_TEMPLATE" "$COPYRIGHT_SRC" "$CHANGELOG_SRC" \
        "$GUI_PRO" "$GUI_DESKTOP_SRC"; do
        [ -f "$_required" ] || fail "missing legacy packaging input: $_required"
    done
    for _size in $GUI_ICON_SIZES; do
        _required="$GUI_ICON_DIR/boot-repair-legacy-${_size}x${_size}.png"
        [ -f "$_required" ] || fail "missing legacy icon: $_required"
    done

    HELPER_SRC="$(find_helper)" \
        || fail "legacy helper source not found. Set LEGACY_HELPER_SRC=<path> (documented variable; see the script header)."
    [ -f "$HELPER_SRC" ] || fail "LEGACY_HELPER_SRC does not exist: $HELPER_SRC"

    if [ "$DRY_RUN" -eq 0 ]; then
        if ! is_etch_host; then
            cat >&2 <<'EOF'
This legacy package must be built natively inside the Debian 4.0 "etch" guest
(dpkg-deb 1.13.26 + fakeroot), never on a modern host. Nothing was built.
Use --dry-run here to validate the staging tree and control metadata without
building; run this script without --dry-run in the Etch guest.
EOF
            exit 2
        fi
        for _cmd in dpkg-deb fakeroot gzip sed install du awk sha256sum qmake-qt3 make; do
            command -v "$_cmd" >/dev/null 2>&1 \
                || fail "missing required Etch packaging command: $_cmd"
        done
    fi

    run_drift_check

    if [ -n "$STAGE_DIR_OVERRIDE" ]; then
        STAGE="$STAGE_DIR_OVERRIDE"
    else
        STAGE="$OUTPUT_DIR/stage"
    fi
    if [ -n "$GUI_BUILD_DIR_OVERRIDE" ]; then
        GUI_BUILD_DIR="$GUI_BUILD_DIR_OVERRIDE"
    else
        GUI_BUILD_DIR="$OUTPUT_DIR/gui-build"
    fi
    mkdir -p "$OUTPUT_DIR"
    DEB_PATH="$OUTPUT_DIR/${PKG_NAME}_${PROJECT_VERSION}-${PKG_RELEASE}_${ARCH}.deb"

    printf 'Package:    %s %s-%s (%s)\n' "$PKG_NAME" "$PROJECT_VERSION" "$PKG_RELEASE" "$ARCH"
    printf 'Helper:     %s -> /usr/sbin/boot-repair-legacy-helper\n' "$HELPER_SRC"
    printf 'GUI:        %s (Qt3, qmake-qt3; package entry point)\n' "${GUI_PRO#"$ROOT_DIR"/}"
    printf 'Stage tree: %s\n' "$STAGE"
    printf 'Artifact:   %s\n' "$DEB_PATH"

    stage_tree "$STAGE"

    if [ "$DRY_RUN" -eq 1 ]; then
        printf '\nDRY RUN complete: staged tree validated, no .deb built.\n'
        printf 'Control metadata:\n'
        sed 's/^/  /' "$STAGE/DEBIAN/control"
        exit 0
    fi

    fakeroot dpkg-deb --build "$STAGE" "$DEB_PATH"
    validate_artifact "$DEB_PATH"

    printf '\nBuilt %s\n' "$DEB_PATH"
    printf 'Nothing was installed.\n'
}

main "$@"
