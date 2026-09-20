#!/usr/bin/env bash
set -euo pipefail

# Build the Arch package (makepkg) from a versioned copy of the working tree.
# Must run as an unprivileged user on an Arch-family host.

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/Development/build-arch-package}"

for command in makepkg cmake ninja c++ tar gzip bsdtar zstd; do
    command -v "$command" >/dev/null 2>&1 || {
        echo "Missing required Arch packaging command: $command" >&2
        echo "Run ./scripts/setup-dev-deps.sh on an Arch-family build host." >&2
        exit 1
    }
done

if [[ ${EUID:-$(id -u)} -eq 0 ]]; then
    echo "makepkg must run as an unprivileged build user; use pacman -U only for installation." >&2
    exit 1
fi

if [[ ! -r /etc/os-release ]]; then
    echo "Cannot verify an Arch-family build host: /etc/os-release is unavailable." >&2
    exit 1
fi
. /etc/os-release
if [[ "${ID:-}" != arch && " ${ID_LIKE:-} " != *" arch "* ]]; then
    echo "This recipe is for Arch-family package hosts; detected ${PRETTY_NAME:-unknown}." >&2
    exit 1
fi

VERSION="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\+\([0-9][0-9.]*\).*/\1/p' "$ROOT_DIR/CMakeLists.txt" | head -1)"
[[ -n "$VERSION" ]] || {
    echo "Unable to determine project version from CMakeLists.txt." >&2
    exit 1
}

BUILD_DIR_REAL="$(realpath -m -- "$BUILD_DIR")"
ROOT_DIR_REAL="$(realpath -m -- "$ROOT_DIR")"
if [[ -z "$BUILD_DIR_REAL" || "$BUILD_DIR_REAL" == / || "$BUILD_DIR_REAL" == "$ROOT_DIR_REAL" ]]; then
    echo "Refusing to remove unsafe package directory: $BUILD_DIR" >&2
    exit 1
fi
rm -rf -- "$BUILD_DIR"
mkdir -p -- "$BUILD_DIR"

# Package the working tree rather than HEAD so local fixes are included before
# they are committed. Build output, VCS state and the local Development area
# are intentionally excluded from the source archive. List source directories
# explicitly so a file such as scripts/build.sh is never mistaken for a
# build-output path.
SOURCE_ARCHIVE="$BUILD_DIR/boot-bitch-$VERSION.tar.gz"
# AGENTS.md is part of the archive so the source tree keeps the workflow
# contract available to any packaged test run.
tar -C "$ROOT_DIR" \
    --transform="s,^,boot-bitch-$VERSION/," \
    -czf "$SOURCE_ARCHIVE" \
    CMakeLists.txt LICENSE README.md CHANGELOG.md AGENTS.md .gitignore \
    src scripts tests data resources docs .github

cat > "$BUILD_DIR/PKGBUILD" <<'PKGBUILD'
pkgname=boot-bitch
pkgver=__VERSION__
pkgrel=1
pkgdesc='Guarded Linux diagnostics, host maintenance and repair-system recovery utility'
arch=('x86_64')
url='https://github.com/CaptainMorgan12/Boot_Bitch'
license=('MIT')
# makepkg has no !buildinfo option (pacman 7.1.0 rejects it as an unknown
# option), so sanitize_arch_package strips the .BUILDINFO builddir/startdir
# fields after the build instead.
options=(!debug)
depends=('qt6-base' 'qt6-svg' 'polkit' 'util-linux' 'cryptsetup' 'rsync' 'e2fsprogs' 'dosfstools' 'btrfs-progs' 'xfsprogs' 'efibootmgr' 'binutils' 'python' 'hicolor-icon-theme')
optdepends=('exfatprogs: exFAT file system check and repair (fsck.exfat)'
            'ntfs-3g: NTFS mount and read-write support'
            'ntfsprogs: NTFS file system check and dirty-state repair (ntfsfix)'
            'f2fs-tools: F2FS file system repair (fsck.f2fs)'
            'jfsutils: JFS file system check and repair (jfs_fsck)'
            'reiserfsprogs: ReiserFS file system check and repair (reiserfsck)'
            'zfsutils-linux: ZFS pool status and scrub (zpool)')
makedepends=('cmake' 'ninja' 'gcc' 'pkgconf')
install=boot-bitch.install
source=("boot-bitch-${pkgver}.tar.gz")
sha256sums=('SKIP')

build() {
    cd "$srcdir/boot-bitch-$pkgver"
    cmake -S . -B build -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr
    cmake --build build
}

package() {
    cd "$srcdir/boot-bitch-$pkgver"
    DESTDIR="$pkgdir" cmake --install build --strip
}
PKGBUILD
sed -i "s/__VERSION__/$VERSION/" "$BUILD_DIR/PKGBUILD"

# Refresh the hicolor icon cache and the desktop-entry database after the
# package transaction so launchers resolve org.bootrepair.BootRepair without
# waiting for an unrelated cache refresh. Both tools are optional at runtime
# (hicolor-icon-theme ships the directory, desktop-file-utils the database
# tool), so failures are ignored and the hooks stay idempotent. makepkg embeds
# this as .INSTALL only when the PKGBUILD references it through install=.
cat > "$BUILD_DIR/boot-bitch.install" <<'INSTALL'
post_install() {
    gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true
    update-desktop-database -q /usr/share/applications || true
}

post_upgrade() {
    gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true
    update-desktop-database -q /usr/share/applications || true
}
INSTALL

# makepkg always records the absolute build directory in .BUILDINFO
# (builddir/startdir) and pacman 7.1.0 rejects a !buildinfo option, so the
# fields are stripped after the build. .MTREE records .BUILDINFO's size and
# sha256, so it is regenerated with the same bsdtar command makepkg uses; the
# package is then recompressed with root ownership so installed files stay
# owned by root. The function is a no-op when .BUILDINFO is already clean.
list_package_files()
{
    (
        export LC_COLLATE=C
        shopt -s dotglob globstar
        printf '%s\0' **/*
    )
}

sanitize_arch_package()
{
    local pkg="$1"
    local work extract builddate needs_repack=0 listing_before listing_after

    work="$(mktemp -d "${TMPDIR:-/tmp}/boot-bitch-arch-sanitize.XXXXXX")"
    extract="$work/pkg"
    mkdir -p -- "$extract"

    if ! bsdtar --zstd -xf "$pkg" -C "$extract"; then
        rm -rf -- "$work"
        return 1
    fi

    if grep -qE '^(builddir|startdir) = ' "$extract/.BUILDINFO"; then
        grep -vE '^(builddir|startdir) = ' "$extract/.BUILDINFO" > "$extract/.BUILDINFO.new"
        mv -- "$extract/.BUILDINFO.new" "$extract/.BUILDINFO"
        needs_repack=1
    fi
    if grep -q '/home/' "$extract/.BUILDINFO"; then
        sed -i 's#/home/[A-Za-z0-9._-]\+#~#g' "$extract/.BUILDINFO"
        needs_repack=1
    fi

    if (( ! needs_repack )); then
        rm -rf -- "$work"
        return 0
    fi

    builddate="$(sed -n 's/^builddate = \([0-9][0-9]*\)$/\1/p' "$extract/.BUILDINFO" | head -n1)"
    [[ -n "$builddate" ]] || builddate="$(stat -c %Y -- "$extract/.BUILDINFO")"

    listing_before="$(bsdtar --zstd -tf "$pkg" | LC_ALL=C sort)"

    (
        cd -- "$extract" || exit 1
        # Match makepkg's mtime normalization and .MTREE generation.
        find . -exec touch -h -d "@$builddate" {} +
        list_package_files | LANG=C bsdtar -cnf - --format=mtree \
            --options='!all,use-set,type,uid,gid,mode,time,size,sha256,link' \
            --null --files-from - --exclude .MTREE | gzip -c -f -n > .MTREE
        touch -d "@$builddate" .MTREE
        list_package_files | LANG=C bsdtar --no-fflags --no-read-sparse \
            --uid 0 --gid 0 -cnf - --null --files-from - |
            zstd -c -z -q - > "$work/package.pkg.tar.zst"
    ) || {
        rm -rf -- "$work"
        return 1
    }

    listing_after="$(bsdtar --zstd -tf "$work/package.pkg.tar.zst" | LC_ALL=C sort)"
    if [[ "$listing_before" != "$listing_after" ]]; then
        echo "Sanitized Arch package file list differs from the makepkg output." >&2
        rm -rf -- "$work"
        return 1
    fi
    if bsdtar --zstd -xOf "$work/package.pkg.tar.zst" .BUILDINFO |
            grep -qE '^(builddir|startdir) = |/home/'; then
        echo "Sanitized Arch package still records a build path in .BUILDINFO." >&2
        rm -rf -- "$work"
        return 1
    fi

    mv -f -- "$work/package.pkg.tar.zst" "$pkg"
    rm -rf -- "$work"
}

# makepkg itself does not modify the VM's package set unless --syncdeps is
# explicitly requested. Keep package creation on the existing environment by
# default; use MAKEPKG_ARGS='--force --syncdeps' when desired.
read -r -a makepkg_args <<< "${MAKEPKG_ARGS:---force}"
(
    cd "$BUILD_DIR"
    makepkg "${makepkg_args[@]}"
)

mapfile -t packages < <(
    find "$BUILD_DIR" -maxdepth 1 -type f -name '*.pkg.tar.*' -print | sort
)
[[ ${#packages[@]} -gt 0 ]] || {
    echo "makepkg completed but no Arch package was generated." >&2
    exit 1
}

for package in "${packages[@]}"; do
    sanitize_arch_package "$package" || {
        echo "Failed to strip the .BUILDINFO build paths from $package." >&2
        exit 1
    }
    pacman -Qp "$package" >/dev/null || {
        echo "pacman cannot read the sanitized Arch package: $package" >&2
        exit 1
    }
done

echo
echo "Arch package created:"
printf '  %s\n' "${packages[@]}"
echo
echo "Nothing was installed on the host. Install with:"
echo "  sudo pacman -U ${packages[0]}"
