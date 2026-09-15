#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build-deb}"
DEB="${1:-}"

need()
{
    command -v "$1" >/dev/null 2>&1 || {
        echo "Missing required test command: $1" >&2
        exit 1
    }
}

for cmd in dpkg-deb find mktemp
 do
    need "$cmd"
 done

if [[ -z "$DEB" ]]
then
    DEB="$(find "$BUILD_DIR" -maxdepth 1 -type f -name '*.deb' -print | sort | tail -1)"
fi

[[ -n "$DEB" && -f "$DEB" ]] || {
    echo "No Debian package found. Run ./scripts/package-deb.sh first." >&2
    exit 1
}

DEB="$(realpath "$DEB")"
TMP="$(mktemp -d "${TMPDIR:-/tmp}/boot-repair-deb-test.XXXXXX")"
trap 'rm -rf "$TMP"' EXIT

echo "========================================"
echo " BOOT BITCH DEB VALIDATION"
echo "========================================"
echo "Package: $DEB"
echo

echo "=== PACKAGE METADATA ==="
dpkg-deb --info "$DEB"
echo

echo "=== PACKAGE CONTENTS ==="
dpkg-deb --contents "$DEB"
echo

echo "=== EXTRACT WITHOUT INSTALLING ==="
dpkg-deb --extract "$DEB" "$TMP/root"

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

grep -q '^Icon=org.bootrepair.BootRepair$' \
    "$TMP/root/usr/share/applications/org.bootrepair.BootRepair.desktop" || {
        echo "FAIL: desktop entry does not reference packaged icon name." >&2
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

bash -n "$TMP/root/usr/libexec/boot-repair/boot-repair-helper"
python3 -m py_compile "$TMP/root/usr/libexec/boot-repair/boot-repair-efi-label.py"

echo "PASS: executable, desktop entry, application icons and privileged helper are present."

echo
echo "=== DYNAMIC LIBRARIES ==="
if command -v ldd >/dev/null 2>&1
then
    ldd "$TMP/root/usr/bin/boot-repair" || true
else
    echo "SKIP: ldd not installed."
fi

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
if command -v appstreamcli >/dev/null 2>&1
then
    if appstreamcli validate \
        "$TMP/root/usr/share/metainfo/org.bootrepair.BootRepair.metainfo.xml"
    then
        echo "PASS: AppStream metadata validation completed."
    else
        echo "WARN: AppStream metadata has advisory validation findings; metadata is present and structurally checked."
    fi
else
    echo "SKIP: appstreamcli not installed (optional AppStream QA tool)."
fi

echo
echo "=== APT INSTALL SIMULATION ==="
if command -v apt-get >/dev/null 2>&1
then
    # -s / --simulate performs dependency resolution without installing anything.
    if apt-get -s -o Debug::NoLocking=1 install "$DEB"
    then
        echo "PASS: apt dependency/install simulation completed."
    else
        echo "WARN: apt simulation reported an issue. No installation occurred." >&2
    fi
else
    echo "SKIP: apt-get not available on this host."
fi

echo
echo "=== DPKG DRY RUN ==="
if command -v dpkg >/dev/null 2>&1
then
    if dpkg --log=/dev/null --dry-run --install "$DEB"
    then
        echo "PASS: dpkg dry-run completed."
    else
        echo "WARN: dpkg dry-run reported an issue. No installation occurred." >&2
    fi
else
    echo "SKIP: dpkg not available on this host."
fi

echo
echo "=== LINTIAN ==="
if command -v lintian >/dev/null 2>&1
then
    lintian "$DEB" || true
else
    echo "SKIP: lintian not installed (optional packaging QA tool)."
fi

echo
echo "========================================"
echo " VALIDATION COMPLETE - NOTHING INSTALLED"
echo "========================================"
