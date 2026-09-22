#!/usr/bin/env bash
# Contract test for the legacy Debian Etch packaging path (L2):
#   - scripts/package-legacy.sh and legacy/launcher/* syntax (sh + bash)
#   - bash 3.1 cleanliness of the package script and the launcher
#   - --dry-run staging layout, control fields and exact dependency list
#   - staged launcher version/helper substitution and documented command list
#   - the off-Etch refusal and the missing-helper failure stay clear and safe
#
# Uses a fixture helper, so it runs on any host without the ported helper and
# never builds or installs a package. Fast tier.
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
PKG="$ROOT_DIR/scripts/package-legacy.sh"
LAUNCHER="$ROOT_DIR/legacy/launcher/boot-repair-legacy"
DESKTOP="$ROOT_DIR/legacy/launcher/boot-repair-legacy.desktop"
MAN="$ROOT_DIR/legacy/launcher/boot-repair-legacy.1"
CONTROL_IN="$ROOT_DIR/legacy/packaging/control.in"

fail()
{
    echo "FAIL: $*" >&2
    exit 1
}

[[ -x "$PKG" ]] || fail "scripts/package-legacy.sh is missing or not executable"
[[ -x "$LAUNCHER" ]] || fail "legacy/launcher/boot-repair-legacy is missing or not executable"
[[ -f "$DESKTOP" ]] || fail "legacy/launcher/boot-repair-legacy.desktop is missing"
[[ -f "$MAN" ]] || fail "legacy/launcher/boot-repair-legacy.1 is missing"
[[ -f "$CONTROL_IN" ]] || fail "legacy/packaging/control.in is missing"

# --- Syntax and bash 3.1 floor ---------------------------------------------
bash -n "$PKG" || fail "scripts/package-legacy.sh failed bash -n"
bash -n "$LAUNCHER" || fail "legacy/launcher/boot-repair-legacy failed bash -n"
sh -n "$LAUNCHER" || fail "legacy/launcher/boot-repair-legacy failed sh -n (POSIX parse)"

for file in "$PKG" "$LAUNCHER"; do
    if grep -nE 'mapfile|readarray|declare[[:space:]]+-A|local[[:space:]]+-A|\$\{[A-Za-z_][A-Za-z0-9_]*(\^\^|\^|,,)\}' "$file"; then
        fail "bash-4-only construct in ${file#"$ROOT_DIR"/}"
    fi
done

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
    "$STAGE/usr/bin/boot-repair-legacy" \
    "$STAGE/usr/share/applications/boot-repair-legacy.desktop" \
    "$STAGE/usr/share/man/man1/boot-repair-legacy.1.gz" \
    "$STAGE/usr/share/doc/boot-repair-legacy/copyright" \
    "$STAGE/usr/share/doc/boot-repair-legacy/changelog.Debian.gz"
do
    [[ -f "$path" ]] || fail "staged layout missing: ${path#"$TMP"/}"
done
[[ -x "$STAGE/usr/sbin/boot-repair-legacy-helper" ]] \
    || fail "staged helper is not executable"
[[ -x "$STAGE/usr/bin/boot-repair-legacy" ]] \
    || fail "staged launcher is not executable"
cmp -s "$FIXTURE_HELPER" "$STAGE/usr/sbin/boot-repair-legacy-helper" \
    || fail "staged helper does not match its source"

# --- Control metadata -------------------------------------------------------
CONTROL="$STAGE/DEBIAN/control"
grep -qx 'Package: boot-repair-legacy' "$CONTROL" || fail "control Package field is wrong"
grep -qx "Version: $VERSION-etch1" "$CONTROL" || fail "control Version field is wrong"
grep -qx 'Architecture: amd64' "$CONTROL" || fail "control Architecture field is wrong"
grep -qx 'Section: admin' "$CONTROL" || fail "control Section field is wrong"
grep -qE '^Installed-Size: [0-9]+$' "$CONTROL" || fail "control Installed-Size is not numeric"
grep -q '^Maintainer: .\+ <.\+@.\+>$' "$CONTROL" || fail "control Maintainer field is malformed"
grep -q '^Description: .\+' "$CONTROL" || fail "control Description is missing"
grep -q '^Recommends: .*dialog' "$CONTROL" || fail "control Recommends is missing dialog"
grep -q '^Suggests: .*kdelibs4c2a' "$CONTROL" || fail "control Suggests is missing kdelibs4c2a"

DEPENDS='Depends: libqt3-mt | libqt3c102-mt, bash (>= 3.1), util-linux, mount, e2fsprogs, grub, gksu | sudo, cryptsetup'
grep -qxF "$DEPENDS" "$CONTROL" || fail "staged control dependency list differs from the contract"
grep -qxF "$DEPENDS" "$CONTROL_IN" || fail "control.in dependency list differs from the contract"

# --- Staged launcher --------------------------------------------------------
grep -qF "VERSION='$VERSION'" "$STAGE/usr/bin/boot-repair-legacy" \
    || fail "staged launcher did not receive the project version"
grep -qF "DEFAULT_HELPER='/usr/sbin/boot-repair-legacy-helper'" "$STAGE/usr/bin/boot-repair-legacy" \
    || fail "staged launcher did not receive the installed helper path"

LIST="$("$STAGE/usr/bin/boot-repair-legacy" --list-commands)"
for token in validate diagnose fs-inspect fix-broken dpkg-configure initramfs apt-update; do
    grep -q -- "$token" <<<"$LIST" || fail "documented command list is missing: $token"
done

"$STAGE/usr/bin/boot-repair-legacy" --version | grep -qxF "boot-repair-legacy $VERSION" \
    || fail "staged launcher --version is wrong"

resolved="$(BOOT_REPAIR_LEGACY_HELPER=/bin/true "$STAGE/usr/bin/boot-repair-legacy" --print-helper)"
[[ "$resolved" == /bin/true ]] || fail "launcher helper discovery override failed: $resolved"

# --dry-run must print the command and must never execute the helper.
cat > "$TMP/exec-helper.sh" <<EOF
#!/bin/sh
touch "$TMP/executed"
EOF
chmod 0755 "$TMP/exec-helper.sh"
"$STAGE/usr/bin/boot-repair-legacy" --helper "$TMP/exec-helper.sh" --dry-run \
    validate /dev/hdb /dev/hdb1 > "$TMP/launcher-dry.log" 2>&1 \
    || { cat "$TMP/launcher-dry.log" >&2; fail "launcher --dry-run failed"; }
[[ ! -e "$TMP/executed" ]] || fail "launcher --dry-run executed the helper"
grep -q 'validate /dev/hdb /dev/hdb1' "$TMP/launcher-dry.log" \
    || fail "launcher --dry-run did not print the helper command"

# --- Default helper discovery and the port-helper drift gate ---------------
TREE="$TMP/tree"
mkdir -p "$TREE/scripts" "$TREE/legacy" "$TREE/legacy/launcher" "$TREE/legacy/packaging"
cp "$PKG" "$TREE/scripts/package-legacy.sh"
cp "$ROOT_DIR/legacy/launcher/"* "$TREE/legacy/launcher/"
cp "$ROOT_DIR/legacy/packaging/"* "$TREE/legacy/packaging/"
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

# --- Desktop entry and man page --------------------------------------------
grep -qx 'Type=Application' "$DESKTOP" || fail "desktop entry Type is wrong"
grep -qx 'Exec=boot-repair-legacy' "$DESKTOP" || fail "desktop entry Exec is wrong"
grep -qx 'Terminal=false' "$DESKTOP" || fail "desktop entry Terminal is wrong"
grep -qx 'Categories=System;' "$DESKTOP" || fail "desktop entry Categories is wrong"

gzip -dc "$STAGE/usr/share/man/man1/boot-repair-legacy.1.gz" > "$TMP/man.roff" \
    || fail "staged man page is not valid gzip"
grep -q '^\.TH BOOT-REPAIR-LEGACY 1 ' "$TMP/man.roff" || fail "staged man page is malformed"

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

echo "PASS: legacy package staging, control metadata, launcher contract and dry-run/off-Etch guards are wired."
