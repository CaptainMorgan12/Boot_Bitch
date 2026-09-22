#!/usr/bin/env bash
# Contract test for the legacy Debian Etch packaging path (L2):
#   - scripts/package-legacy.sh syntax and bash 3.1 cleanliness
#   - --dry-run staging layout, control fields and exact dependency list,
#     including the Qt3 GUI binary/desktop/icon layout
#   - the shell-only boot-repair-legacy TUI launcher (and its desktop entry
#     and man page) is deliberately NOT shipped: the GUI is the entry point
#   - the off-Etch refusal and the missing-helper failure stay clear and safe
#
# Uses a fixture helper, so it runs on any host without the ported helper and
# never builds or installs a package. When qmake-qt3 is unavailable the dry run
# stages a placeholder GUI binary (documented); a real Etch build always
# installs the compiled binary. Fast tier.
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
PKG="$ROOT_DIR/scripts/package-legacy.sh"
CONTROL_IN="$ROOT_DIR/legacy/packaging/control.in"
GUI_PRO="$ROOT_DIR/legacy/gui/boot-bitch-legacy.pro"
GUI_DESKTOP="$ROOT_DIR/legacy/gui/data/boot-repair-legacy-gui.desktop"
GUI_ICON="$ROOT_DIR/legacy/gui/data/boot-repair-legacy-48x48.png"

fail()
{
    echo "FAIL: $*" >&2
    exit 1
}

[[ -x "$PKG" ]] || fail "scripts/package-legacy.sh is missing or not executable"
[[ -f "$CONTROL_IN" ]] || fail "legacy/packaging/control.in is missing"
[[ -f "$GUI_PRO" ]] || fail "legacy/gui/boot-bitch-legacy.pro is missing"
[[ -f "$GUI_DESKTOP" ]] || fail "legacy/gui/data/boot-repair-legacy-gui.desktop is missing"
[[ -f "$GUI_ICON" ]] || fail "legacy/gui/data/boot-repair-legacy-48x48.png is missing"

# Qt3-only: no kdelibs, no Qt4/5/6 modules.
grep -q '^CONFIG  += qt' "$GUI_PRO" || fail "GUI project does not enable Qt"
grep -qE '^[[:space:]]*(CONFIG|QT|LIBS)[^#]*kdelibs' "$GUI_PRO" \
    && fail "GUI project must not require kdelibs"
grep -qE '^[[:space:]]*QT[[:space:]]*\+=' "$GUI_PRO" \
    && fail "GUI project must stay Qt3 core-widgets only"

# --- Syntax and bash 3.1 floor ---------------------------------------------
bash -n "$PKG" || fail "scripts/package-legacy.sh failed bash -n"

if grep -nE 'mapfile|readarray|declare[[:space:]]+-A|local[[:space:]]+-A|\$\{[A-Za-z_][A-Za-z0-9_]*(\^\^|\^|,,)\}' "$PKG"; then
    fail "bash-4-only construct in ${PKG#"$ROOT_DIR"/}"
fi

VERSION="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\{1,\}\([0-9][0-9.]*\).*/\1/p' \
    "$ROOT_DIR/CMakeLists.txt" | head -1)"
[[ -n "$VERSION" ]] || fail "cannot parse the project version from CMakeLists.txt"

# --- Dry-run staging --------------------------------------------------------
TMP="$(mktemp -d "${TMPDIR:-/tmp}/boot-repair-legacy-contract.XXXXXX")"
trap 'rm -rf "$TMP"' EXIT

FIXTURE_HELPER="$TMP/fixture-helper.sh"
cat > "$FIXTURE_HELPER" <<'EOF'
#!/bin/bash
# Minimal stand-in for the ported legacy helper (contract fixture only).
set -eu
main() { :; }
main "$@"
EOF
chmod 0755 "$FIXTURE_HELPER"

STAGE="$TMP/stage"
OUT="$TMP/out"
if ! LEGACY_HELPER_SRC="$FIXTURE_HELPER" "$PKG" --dry-run --stage-dir "$STAGE" --output "$OUT" \
        > "$TMP/dry-run.log" 2>&1; then
    cat "$TMP/dry-run.log" >&2
    fail "scripts/package-legacy.sh --dry-run failed"
fi

for path in \
    "$STAGE/DEBIAN/control" \
    "$STAGE/usr/sbin/boot-repair-legacy-helper" \
    "$STAGE/usr/bin/boot-repair-legacy-gui" \
    "$STAGE/usr/share/applications/boot-repair-legacy-gui.desktop" \
    "$STAGE/usr/share/icons/hicolor/16x16/apps/boot-repair-legacy.png" \
    "$STAGE/usr/share/icons/hicolor/22x22/apps/boot-repair-legacy.png" \
    "$STAGE/usr/share/icons/hicolor/32x32/apps/boot-repair-legacy.png" \
    "$STAGE/usr/share/icons/hicolor/48x48/apps/boot-repair-legacy.png" \
    "$STAGE/usr/share/doc/boot-repair-legacy/copyright" \
    "$STAGE/usr/share/doc/boot-repair-legacy/changelog.Debian.gz"
do
    [[ -f "$path" ]] || fail "staged layout missing: ${path#"$TMP"/}"
done
[[ -x "$STAGE/usr/sbin/boot-repair-legacy-helper" ]] \
    || fail "staged helper is not executable"
[[ -x "$STAGE/usr/bin/boot-repair-legacy-gui" ]] \
    || fail "staged GUI binary is not executable"
[[ -s "$STAGE/usr/bin/boot-repair-legacy-gui" ]] \
    || fail "staged GUI binary is empty"
cmp -s "$FIXTURE_HELPER" "$STAGE/usr/sbin/boot-repair-legacy-helper" \
    || fail "staged helper does not match its source"

# The shell-only TUI launcher, its desktop entry and its man page are gone.
for path in \
    "$STAGE/usr/bin/boot-repair-legacy" \
    "$STAGE/usr/share/applications/boot-repair-legacy.desktop" \
    "$STAGE/usr/share/man/man1/boot-repair-legacy.1.gz"
do
    [[ ! -e "$path" ]] || fail "staged tree still ships the removed launcher path: ${path#"$TMP"/}"
done

# --- Control metadata -------------------------------------------------------
CONTROL="$STAGE/DEBIAN/control"
grep -qx 'Package: boot-repair-legacy' "$CONTROL" || fail "control Package field is wrong"
grep -qx "Version: $VERSION-etch1" "$CONTROL" || fail "control Version field is wrong"
grep -qx 'Architecture: amd64' "$CONTROL" || fail "control Architecture field is wrong"
grep -qx 'Section: admin' "$CONTROL" || fail "control Section field is wrong"
grep -qE '^Installed-Size: [0-9]+$' "$CONTROL" || fail "control Installed-Size is not numeric"
grep -q '^Maintainer: .\+ <.\+@.\+>$' "$CONTROL" || fail "control Maintainer field is malformed"
grep -q '^Description: .\+' "$CONTROL" || fail "control Description is missing"
grep -q '^Recommends: .*dosfstools' "$CONTROL" || fail "control Recommends is missing dosfstools"
grep -q '^Suggests: .*kdelibs4c2a' "$CONTROL" || fail "control Suggests is missing kdelibs4c2a"

DEPENDS='Depends: libqt3-mt (>= 3:3.3.7), bash (>= 3.1), util-linux, mount, e2fsprogs, grub, gksu | sudo, cryptsetup'
grep -qxF "$DEPENDS" "$CONTROL" || fail "staged control dependency list differs from the contract"
grep -qxF "$DEPENDS" "$CONTROL_IN" || fail "control.in dependency list differs from the contract"
grep -qxF 'Recommends: dosfstools, lvm2, mdadm, initramfs-tools, rsync' "$CONTROL" \
    || fail "control Recommends still carries launcher-only entries"
grep -qxF 'Suggests: kdelibs4c2a' "$CONTROL" \
    || fail "control Suggests still carries launcher-only entries"
grep -q 'launcher' "$CONTROL" && fail "control Description still mentions the removed launcher"

# --- Default helper discovery and the port-helper drift gate ---------------
TREE="$TMP/tree"
mkdir -p "$TREE/scripts" "$TREE/legacy" "$TREE/legacy/packaging"
cp "$PKG" "$TREE/scripts/package-legacy.sh"
cp "$ROOT_DIR/legacy/packaging/"* "$TREE/legacy/packaging/"
cp -a "$ROOT_DIR/legacy/gui" "$TREE/legacy/gui"
printf 'project(BootRepair\n    VERSION %s\n)\n' "$VERSION" > "$TREE/CMakeLists.txt"
cp "$FIXTURE_HELPER" "$TREE/legacy/boot-repair-helper.sh"
printf '#!/bin/bash\nexit 0\n' > "$TREE/legacy/port.sh"

"$TREE/scripts/package-legacy.sh" --dry-run --stage-dir "$TMP/tree-stage" --output "$TMP/tree-out" \
    > "$TMP/tree-ok.log" 2>&1 || { cat "$TMP/tree-ok.log" >&2; fail "default helper discovery failed"; }
grep -q 'legacy/port.sh --check' "$TMP/tree-ok.log" || fail "drift gate did not run"
grep -q "$TREE/legacy/boot-repair-helper.sh" "$TMP/tree-ok.log" \
    || fail "default helper discovery did not find legacy/boot-repair-helper.sh"

printf '#!/bin/bash\nexit 1\n' > "$TREE/legacy/port.sh"
if "$TREE/scripts/package-legacy.sh" --dry-run --stage-dir "$TMP/tree-stage2" --output "$TMP/tree-out2" \
        > "$TMP/tree-fail.log" 2>&1; then
    fail "a failing port-helper.sh --check did not abort the dry run"
fi
grep -q 'port.sh --check failed' "$TMP/tree-fail.log" || fail "drift-gate failure message is unclear"

# --- GUI desktop entry ------------------------------------------------------
grep -qx 'Type=Application' "$GUI_DESKTOP" || fail "GUI desktop entry Type is wrong"
grep -qx 'Exec=boot-repair-legacy-gui' "$GUI_DESKTOP" || fail "GUI desktop entry Exec is wrong"
grep -qx 'Icon=boot-repair-legacy' "$GUI_DESKTOP" || fail "GUI desktop entry Icon is wrong"
grep -qx 'Terminal=false' "$GUI_DESKTOP" || fail "GUI desktop entry Terminal is wrong"
grep -qx 'Categories=System;' "$GUI_DESKTOP" || fail "GUI desktop entry Categories is wrong"
cmp -s "$GUI_DESKTOP" "$STAGE/usr/share/applications/boot-repair-legacy-gui.desktop" \
    || fail "staged GUI desktop entry differs from its source"
cmp -s "$GUI_ICON" "$STAGE/usr/share/icons/hicolor/48x48/apps/boot-repair-legacy.png" \
    || fail "staged 48x48 icon differs from its source"

# --- Dry-run never builds, off-Etch build refuses, missing helper fails -----
if find "$OUT" -name '*.deb' -print | grep -q .; then
    fail "--dry-run produced a .deb"
fi
grep -q "boot-repair-legacy_${VERSION}-etch1_amd64.deb" "$TMP/dry-run.log" \
    || fail "dry-run did not report the expected artifact name"

if LEGACY_HELPER_SRC="$FIXTURE_HELPER" "$PKG" --output "$TMP/offetch-out" \
        > "$TMP/offetch.log" 2>&1; then
    fail "package-legacy.sh attempted a real build off-Etch"
fi
grep -qi 'etch' "$TMP/offetch.log" || fail "off-Etch refusal does not name Etch"
if find "$TMP/offetch-out" -name '*.deb' -print 2>/dev/null | grep -q .; then
    fail "off-Etch refusal still produced a .deb"
fi

if LEGACY_HELPER_SRC="$TMP/does-not-exist.sh" "$PKG" --dry-run --stage-dir "$TMP/stage-missing" \
        > "$TMP/missing.log" 2>&1; then
    fail "package-legacy.sh accepted a missing helper source"
fi
grep -q 'LEGACY_HELPER_SRC' "$TMP/missing.log" \
    || fail "missing-helper error does not name the documented LEGACY_HELPER_SRC variable"

"$PKG" --help | grep -q -- '--dry-run' || fail "package-legacy.sh --help is missing --dry-run"
"$PKG" --help | grep -q -- '--output' || fail "package-legacy.sh --help is missing --output"

echo "PASS: legacy package staging, GUI entry point, control metadata and dry-run/off-Etch guards are wired."
