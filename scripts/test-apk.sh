#!/usr/bin/env bash
set -euo pipefail

# Inspect, extract and validate a built .apk without installing it.
#
# Usage: test-apk.sh [path/to/package.apk]

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/Development/build-alpine-package}"
APK="${1:-}"

if ! command -v tar >/dev/null 2>&1; then
    echo "Missing required test command: tar" >&2
    echo "An .apk is a concatenated gzip tar archive; install GNU tar (Alpine: apk add tar)." >&2
    exit 1
fi
for cmd in find mktemp grep sed python3; do
    command -v "$cmd" >/dev/null 2>&1 || {
        echo "Missing required test command: $cmd" >&2
        exit 1
    }
done

if [[ -z "$APK" ]]
then
    if [[ -d "$BUILD_DIR" ]]
    then
        APK="$(find "$BUILD_DIR" -type f -name '*.apk' -print | sort | tail -1)"
    fi
fi

[[ -n "$APK" && -f "$APK" ]] || {
    echo "No Alpine package found. Run ./scripts/package-alpine.sh first." >&2
    exit 1
}

APK="$(realpath "$APK")"
TMP="$(mktemp -d "${TMPDIR:-/tmp}/boot-repair-apk-test.XXXXXX")"
trap 'rm -rf "$TMP"' EXIT

# GNU tar emits APK-TOOLS extended-header warnings for some .apk archives
# (older apk-tools adds them to the control stream). They are informational
# only, so keep them out of the output while leaving real errors visible.
tar_filter_stderr()
{
    grep -v -e 'APK-TOOLS' -e 'unknown extended header' || true
}

VERSION="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\+\([0-9][0-9.]*\).*/\1/p' "$ROOT_DIR/CMakeLists.txt" | head -1)"
[[ -n "$VERSION" ]] || {
    echo "Unable to determine project version from CMakeLists.txt." >&2
    exit 1
}

echo "========================================"
echo " BOOT BITCH APK VALIDATION"
echo "========================================"
echo "Package: $APK"
echo

echo "=== PACKAGE CONTENTS ==="
tar -tzf "$APK" 2> >(tar_filter_stderr >&2) | tee "$TMP/listing"
echo

echo "=== PACKAGE METADATA ==="
tar -xOf "$APK" .PKGINFO 2> >(tar_filter_stderr >&2) | tee "$TMP/.PKGINFO"
echo

grep -q '^pkgname = boot-bitch$' "$TMP/.PKGINFO" || {
    echo "FAIL: .PKGINFO pkgname is not boot-bitch." >&2
    exit 1
}

grep -q "^pkgver = ${VERSION}-r0$" "$TMP/.PKGINFO" || {
    echo "FAIL: .PKGINFO pkgver is not ${VERSION}-r0." >&2
    exit 1
}

grep -q '^arch = x86_64$' "$TMP/.PKGINFO" || {
    echo "FAIL: .PKGINFO arch is not x86_64." >&2
    exit 1
}

grep -q '^license = MIT$' "$TMP/.PKGINFO" || {
    echo "FAIL: .PKGINFO license is not MIT." >&2
    exit 1
}

echo "=== PACKAGE DEPENDENCIES ==="
expected_depends=(
    qt6-qtbase
    qt6-qtsvg
    polkit-elogind
    coreutils
    findutils
    util-linux
    cryptsetup
    rsync
    e2fsprogs
    dosfstools
    btrfs-progs
    xfsprogs
    efibootmgr
    binutils
    python3
    hicolor-icon-theme
)
expected_sorted="$(printf '%s\n' "${expected_depends[@]}" | sort -u)"
# abuild adds automatic so:/cmd: dependencies and /bin/sh for install hooks;
# only the explicit dependency list is a contract.
actual_sorted="$(
    sed -n 's/^depend = //p' "$TMP/.PKGINFO" \
        | grep -v -e '^so:' -e '^cmd:' -e '^/bin/sh$' \
        | sort -u || true
)"
if [[ "$actual_sorted" != "$expected_sorted" ]]; then
    echo "FAIL: explicit .PKGINFO depend set does not match the expected runtime dependencies." >&2
    diff <(printf '%s\n' "$expected_sorted") <(printf '%s\n' "$actual_sorted") >&2 || true
    exit 1
fi
for dependency in "${expected_depends[@]}"; do
    grep -q "^depend = ${dependency}$" "$TMP/.PKGINFO" || {
        echo "FAIL: .PKGINFO is missing the runtime dependency: $dependency" >&2
        exit 1
    }
done
grep -q '^depend = /bin/sh$' "$TMP/.PKGINFO" || {
    echo "FAIL: .PKGINFO is missing the /bin/sh dependency added for the post-install hook." >&2
    exit 1
}
echo "PASS: runtime dependencies and the post-install hook dependency are declared."

echo
echo "=== PACKAGE SIGNATURE ==="
grep -q '^\.SIGN\.RSA\.' "$TMP/listing" || {
    echo "FAIL: package has no .SIGN.RSA.* signature entry; abuild did not sign it." >&2
    exit 1
}
echo "PASS: package carries an abuild RSA signature."

echo
echo "=== EXTRACT WITHOUT INSTALLING ==="
mkdir -p "$TMP/root"
tar -xzf "$APK" -C "$TMP/root" 2> >(tar_filter_stderr >&2)

[[ -x "$TMP/root/usr/bin/boot-repair" ]] || {
    echo "FAIL: /usr/bin/boot-repair missing from extracted package." >&2
    exit 1
}

[[ -f "$TMP/root/usr/share/applications/org.bootrepair.BootRepair.desktop" ]] || {
    echo "FAIL: desktop entry missing from extracted package." >&2
    exit 1
}

[[ -f "$TMP/root/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml" ]] || {
    echo "FAIL: AppStream metadata missing from extracted package." >&2
    exit 1
}

for icon_size in 16 22 24 32 48 64 128 256 512 1024; do
    icon="$TMP/root/usr/share/icons/hicolor/${icon_size}x${icon_size}/apps/org.bootrepair.BootRepair.png"
    [[ -f "$icon" ]] || {
        echo "FAIL: application icon missing from extracted package: $icon" >&2
        exit 1
    }
done

grep -q '<id>org.bootrepair.BootRepair</id>' \
    "$TMP/root/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml" || {
    echo "FAIL: AppStream metadata has the wrong component ID." >&2
    exit 1
}
grep -q '<launchable type="desktop-id">org.bootrepair.BootRepair.desktop</launchable>' \
    "$TMP/root/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml" || {
    echo "FAIL: AppStream metadata does not link the desktop entry." >&2
    exit 1
}
grep -q '<binary>boot-repair</binary>' \
    "$TMP/root/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml" || {
    echo "FAIL: AppStream metadata does not advertise the installed binary." >&2
    exit 1
}

[[ -x "$TMP/root/usr/libexec/boot-repair/boot-repair-helper" ]] || {
    echo "FAIL: privileged repair helper missing from extracted package." >&2
    exit 1
}

[[ -x "$TMP/root/usr/libexec/boot-repair/boot-repair-efi-label.py" ]] || {
    echo "FAIL: EFI label updater missing from extracted package." >&2
    exit 1
}

bash -n "$TMP/root/usr/libexec/boot-repair/boot-repair-helper"
python3 -m py_compile "$TMP/root/usr/libexec/boot-repair/boot-repair-efi-label.py"

echo "PASS: executable, desktop entry, application icons and privileged helper are present."

echo
echo "=== DESKTOP ENTRY ==="
if command -v desktop-file-validate >/dev/null 2>&1
then
    desktop-file-validate "$TMP/root/usr/share/applications/org.bootrepair.BootRepair.desktop"
    echo "PASS: desktop entry validation completed."
else
    echo "SKIP: desktop-file-validate not installed (optional: desktop-file-utils)."
fi

echo
echo "=== APPSTREAM METADATA ==="
if command -v xmllint >/dev/null 2>&1
then
    xmllint --noout \
        "$TMP/root/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml"
    echo "PASS: AppStream metadata is well-formed XML."
else
    echo "SKIP: xmllint not installed (optional XML QA tool)."
fi

echo
echo "=== DYNAMIC LIBRARIES ==="
if command -v ldd >/dev/null 2>&1
then
    ldd "$TMP/root/usr/bin/boot-repair" || true
else
    echo "SKIP: ldd not installed."
fi

echo
echo "========================================"
echo " VALIDATION COMPLETE - NOTHING INSTALLED"
echo "========================================"
