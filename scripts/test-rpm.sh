#!/usr/bin/env bash
set -euo pipefail

# Inspect, extract and validate a built .rpm without installing it.
#
# Usage: test-rpm.sh [path/to/package.rpm]
#
# RPM inspection tools are not part of a default Debian/Ubuntu install:
#   sudo apt install rpm rpmlint

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/Development/build-rpm}"
RPM="${1:-}"

need()
{
    command -v "$1" >/dev/null 2>&1 || {
        echo "Missing required test command: $1" >&2
        echo "Install the RPM inspection tools first:" >&2
        echo "  Debian/Ubuntu: sudo apt install rpm rpmlint" >&2
        echo "  Fedora/RHEL:   sudo dnf install rpm rpmlint" >&2
        echo "  openSUSE:      sudo zypper install rpm rpmlint" >&2
        exit 1
    }
}

for cmd in rpm rpm2cpio cpio find mktemp realpath python3; do
    need "$cmd"
done

if [[ -z "$RPM" ]]
then
    if [[ -d "$BUILD_DIR" ]]
    then
        RPM="$(find "$BUILD_DIR" -maxdepth 1 -type f -name '*.rpm' -print | sort | tail -1)"
    fi
fi

[[ -n "$RPM" && -f "$RPM" ]] || {
    echo "No RPM package found. Run ./scripts/package-rpm.sh first." >&2
    exit 1
}

RPM="$(realpath "$RPM")"
TMP="$(mktemp -d "${TMPDIR:-/tmp}/boot-repair-rpm-test.XXXXXX")"
trap 'rm -rf "$TMP"' EXIT

fail()
{
    echo "FAIL: $*" >&2
    exit 1
}

echo "========================================"
echo " BOOT BITCH RPM VALIDATION"
echo "========================================"
echo "Package: $RPM"
echo

echo "=== PACKAGE METADATA ==="
rpm -qpi "$RPM"
echo

echo "=== PACKAGE CONTENTS ==="
rpm -qpl "$RPM"
echo

echo "=== PACKAGE REQUIREMENTS ==="
rpm -qpR "$RPM"
echo

echo "=== PACKAGE SCRIPTS ==="
rpm -qp --scripts "$RPM" || true
echo

echo "=== PACKAGE CHANGELOG ==="
rpm -qp --changelog "$RPM" || true
echo

echo "=== EXTRACT WITHOUT INSTALLING ==="
mkdir -p "$TMP/root"
rpm2cpio "$RPM" | (cd "$TMP/root" && cpio -idm --quiet)
echo "Extracted into the temporary directory."
echo

echo "=== CONTENT CONTRACT ==="

file_list="$(rpm -qpl "$RPM")"

assert_listed()
{
    local path="$1"
    local description="$2"
    grep -Fqx "$path" <<<"$file_list" || fail "$description is missing from the package file list: $path"
}

assert_listed /usr/bin/boot-repair "application binary"
assert_listed /usr/share/applications/org.bootrepair.BootRepair.desktop "desktop entry"
assert_listed /usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml "AppStream metadata"
assert_listed /usr/libexec/boot-repair/boot-repair-helper "privileged repair helper"
assert_listed /usr/libexec/boot-repair/boot-repair-efi-label.py "EFI label updater"

[[ -x "$TMP/root/usr/bin/boot-repair" ]] || \
    fail "application binary is not executable in the extracted package."
[[ -f "$TMP/root/usr/share/applications/org.bootrepair.BootRepair.desktop" ]] || \
    fail "desktop entry is missing from the extracted package."
[[ -f "$TMP/root/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml" ]] || \
    fail "AppStream metadata is missing from the extracted package."
[[ -x "$TMP/root/usr/libexec/boot-repair/boot-repair-helper" ]] || \
    fail "privileged repair helper is missing from the extracted package."
[[ -x "$TMP/root/usr/libexec/boot-repair/boot-repair-efi-label.py" ]] || \
    fail "EFI label updater is missing from the extracted package."

for icon_size in 16 22 24 32 48 64 128 256 512 1024; do
    icon_path="/usr/share/icons/hicolor/${icon_size}x${icon_size}/apps/org.bootrepair.BootRepair.png"
    assert_listed "$icon_path" "${icon_size}px application icon"
    [[ -f "$TMP/root$icon_path" ]] || \
        fail "${icon_size}px application icon is missing from the extracted package."
done

grep -q '^Icon=/usr/share/icons/hicolor/512x512/apps/org.bootrepair.BootRepair.png$' \
    "$TMP/root/usr/share/applications/org.bootrepair.BootRepair.desktop" || \
    fail "desktop entry does not reference the installed icon path."

grep -q '<id>org.bootrepair.BootRepair</id>' \
    "$TMP/root/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml" || \
    fail "AppStream metadata has the wrong component ID."
grep -q '<launchable type="desktop-id">org.bootrepair.BootRepair.desktop</launchable>' \
    "$TMP/root/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml" || \
    fail "AppStream metadata does not link the desktop entry."
grep -q '<binary>boot-repair</binary>' \
    "$TMP/root/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml" || \
    fail "AppStream metadata does not advertise the installed binary."

bash -n "$TMP/root/usr/libexec/boot-repair/boot-repair-helper" || \
    fail "privileged repair helper is not valid Bash."
python3 -m py_compile "$TMP/root/usr/libexec/boot-repair/boot-repair-efi-label.py" || \
    fail "EFI label updater is not valid Python."

rpm_name="$(rpm -qp --qf '%{NAME}' "$RPM")"
[[ "$rpm_name" == "boot-bitch" ]] || \
    fail "unexpected RPM package name: $rpm_name"
rpm_license="$(rpm -qp --qf '%{LICENSE}' "$RPM")"
[[ "$rpm_license" == "MIT" ]] || \
    fail "unexpected RPM license: $rpm_license"
rpm_url="$(rpm -qp --qf '%{URL}' "$RPM")"
[[ "$rpm_url" == "https://github.com/CaptainMorgan12/Boot_Bitch" ]] || \
    fail "unexpected RPM URL: $rpm_url"
rpm_description="$(rpm -qp --qf '%{DESCRIPTION}' "$RPM")"
grep -q 'protected discovery and read-only diagnostics' <<<"$rpm_description" || \
    fail "RPM description is missing the packaged long description."
rpm_changelog="$(rpm -qp --changelog "$RPM")"
[[ -n "$rpm_changelog" ]] || \
    fail "RPM changelog is empty."

rpm_scripts="$(rpm -qp --scripts "$RPM")"
grep -q 'gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor' <<<"$rpm_scripts" || \
    fail "RPM post-install scriptlet does not refresh the hicolor icon cache."
grep -q 'update-desktop-database -q /usr/share/applications' <<<"$rpm_scripts" || \
    fail "RPM post-install scriptlet does not refresh the desktop database."

rpm_requires="$(rpm -qpR "$RPM")"
for requirement in polkit util-linux cryptsetup rsync e2fsprogs dosfstools xfsprogs \
                   efibootmgr binutils python3 hicolor-icon-theme
do
    grep -Fqx "$requirement" <<<"$rpm_requires" || \
        fail "RPM is missing the explicit runtime requirement: $requirement"
done

echo "PASS: executable, desktop entry, application icons, privileged helper, EFI updater and metadata are present."
echo

echo "=== PACKAGE DIGEST VERIFICATION ==="
if command -v rpmkeys >/dev/null 2>&1
then
    if rpmkeys -K "$RPM"
    then
        echo "PASS: package digests verified (local unsigned builds report digests only)."
    else
        echo "WARN: rpmkeys -K reported an issue; local builds are unsigned." >&2
    fi
else
    echo "SKIP: rpmkeys not installed (part of the rpm package)."
fi

echo
echo "=== RPMLINT ==="
if command -v rpmlint >/dev/null 2>&1
then
    # Advisory only. On Debian-family hosts some findings describe the local
    # rpm tooling rather than this package, so they never fail validation.
    rpmlint "$RPM" || true
else
    echo "SKIP: rpmlint not installed (optional packaging QA tool: sudo apt install rpmlint)."
fi

echo
echo "========================================"
echo " VALIDATION COMPLETE - NOTHING INSTALLED"
echo "========================================"
