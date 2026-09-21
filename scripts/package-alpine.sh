#!/usr/bin/env bash
set -euo pipefail

# Build the Alpine package (abuild) from a versioned copy of the working tree.
# Must run as an unprivileged user inside an Alpine environment (an Alpine
# host, chroot or container); abuild refuses to run as root.

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/Development/build-alpine-package}"

if [[ ${EUID:-$(id -u)} -eq 0 ]]; then
    echo "abuild must run as an unprivileged build user; use apk add only for installation." >&2
    exit 1
fi

if [[ ! -r /etc/os-release ]]; then
    echo "Cannot verify an Alpine build host: /etc/os-release is unavailable." >&2
    echo "Run this script inside an Alpine environment (host, chroot or container)." >&2
    exit 1
fi
# shellcheck source=/dev/null
. /etc/os-release
if [[ "${ID:-}" != alpine && " ${ID_LIKE:-} " != *" alpine "* ]]; then
    echo "This recipe is for Alpine-family package hosts; detected ${PRETTY_NAME:-unknown}." >&2
    echo "Run this script inside an Alpine chroot or container as an unprivileged user." >&2
    exit 1
fi

for command in abuild abuild-keygen cmake ninja c++ tar gzip sha512sum; do
    command -v "$command" >/dev/null 2>&1 || {
        echo "Missing required Alpine packaging command: $command" >&2
        echo "Run ./scripts/setup-dev-deps.sh on an Alpine build host." >&2
        exit 1
    }
done

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
tar -C "$ROOT_DIR" \
    --transform="s,^,boot-bitch-$VERSION/," \
    -czf "$SOURCE_ARCHIVE" \
    CMakeLists.txt LICENSE README.md CHANGELOG.md .gitignore \
    src scripts tests data resources docs .github
SOURCE_SHA512="$(sha512sum "$SOURCE_ARCHIVE" | awk '{print $1}')"

# Alpine package names for the runtime and optional dependencies. Alpine ships
# no ntfsprogs (ntfs-3g provides ntfsfix), no reiserfsprogs in v3.21 and names
# the ZFS userspace tools simply 'zfs', so those optdepends differ from the
# Arch recipe. The elogind Polkit build is a hard dependency: the plain 'polkit'
# package pulls the ConsoleKit variant (polkit-noelogind-libs), which cannot
# register a session authentication agent, so pkexec falls back to its textual
# agent and fails without a controlling terminal. GNU coreutils and findutils
# are runtime dependencies because the helper uses `find -printf`, `sort -V`,
# `date --iso-8601` and `timeout --foreground`.
cat > "$BUILD_DIR/APKBUILD" <<'APKBUILD'
# Maintainer: CaptainMorgan12 <captainmorgan12@users.noreply.github.com>
pkgname=boot-bitch
pkgver=__VERSION__
pkgrel=0
pkgdesc='Guarded Linux diagnostics, host maintenance and repair-system recovery utility'
url='https://github.com/CaptainMorgan12/Boot_Bitch'
arch='x86_64'
license='MIT'
depends='qt6-qtbase qt6-qtsvg polkit-elogind coreutils findutils util-linux cryptsetup rsync e2fsprogs dosfstools btrfs-progs xfsprogs efibootmgr binutils python3 hicolor-icon-theme'
optdepends='xfce-polkit: session Polkit authentication agent required for pkexec authorization prompts (alternative: polkit-gnome)
exfatprogs: exFAT file system check and repair (fsck.exfat)
ntfs-3g: NTFS mount, file system check and dirty-state repair (ntfsfix)
f2fs-tools: F2FS file system repair (fsck.f2fs)
jfsutils: JFS file system check and repair (jfs_fsck)
zfs: ZFS pool status and scrub (zpool)'
makedepends='cmake samurai gcc pkgconf qt6-qtbase-dev qt6-qtsvg-dev'
# The check phase runs the offscreen UI test and the shell contract tests.
# They need bash (the scripts are not POSIX sh), git (the release-preparation
# contract initializes a throwaway staging clone) and python3 (the EFI label
# test), which are not implied by the build-only makedepends.
checkdepends='bash git python3'
install="$pkgname.post-install"
source="$pkgname-$pkgver.tar.gz"
builddir="$srcdir/$pkgname-$pkgver"
sha512sums='__SHA512__  __SOURCE__'

build() {
    cmake -S "$builddir" -B "$builddir/build" -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr
    cmake --build "$builddir/build"
}

check() {
    # Run the project test suite in the build tree from build() so abuild
    # reports a checked package instead of its "does not run any tests"
    # warning. The packaging-profile contract is excluded because it audits
    # repository-level packaging metadata and install.sh guidance against a
    # canonical checkout (including the Development/ release captures), which
    # the stripped source archive abuild unpacks does not provide; the built
    # package itself is validated afterwards by scripts/test-apk.sh. The
    # remaining tests exercise the built tree and are self-contained.
    QT_QPA_PLATFORM=offscreen ctest --test-dir "$builddir/build" --output-on-failure \
        -E boot-repair-packaging-profile-contract
}

package() {
    DESTDIR="$pkgdir" cmake --install "$builddir/build" --strip
}
APKBUILD
sed -i "s/__VERSION__/$VERSION/; s/__SHA512__/$SOURCE_SHA512/; s/__SOURCE__/boot-bitch-$VERSION.tar.gz/" "$BUILD_DIR/APKBUILD"

# Refresh the hicolor icon cache and the desktop-entry database after the
# package transaction so launchers resolve org.bootrepair.BootRepair without
# waiting for an unrelated cache refresh. Both tools are optional at runtime
# (hicolor-icon-theme ships the icon directories, desktop-file-utils the
# database tool), so failures are ignored and the hook stays idempotent.
# abuild packages this as .post-install only when the APKBUILD references it
# through install=.
cat > "$BUILD_DIR/boot-bitch.post-install" <<'INSTALL'
#!/bin/sh
gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor 2>/dev/null || true
update-desktop-database -q /usr/share/applications 2>/dev/null || true
INSTALL
chmod 755 "$BUILD_DIR/boot-bitch.post-install"

# abuild signs the package with the user's RSA key. Generate one on first use;
# abuild-keygen -i also installs the public key into /etc/apk/keys through
# doas/sudo, which may be unavailable to the build user. Signing only needs
# the private key, so keep going with a warning when only that step failed and
# make sure abuild.conf references the generated key (abuild-keygen -a would
# normally do that, but it stops at the failed -i step first).
mapfile -t abuild_keys < <(compgen -G "$HOME/.abuild/*.rsa" || true)
if ((${#abuild_keys[@]} == 0)); then
    echo "Generating an abuild signing key (abuild-keygen -a -i -n)..."
    if ! abuild-keygen -a -i -n; then
        mapfile -t abuild_keys < <(compgen -G "$HOME/.abuild/*.rsa" || true)
        if ((${#abuild_keys[@]} == 0)); then
            echo "abuild-keygen failed to create a signing key." >&2
            exit 1
        fi
        echo "WARNING: the signing key was generated, but the public key could not be installed into /etc/apk/keys." >&2
        echo "         Install it to trust the package without --allow-untrusted:" >&2
        echo "           sudo cp $HOME/.abuild/*.rsa.pub /etc/apk/keys/" >&2
    fi
    mapfile -t abuild_keys < <(compgen -G "$HOME/.abuild/*.rsa" || true)
fi
if ! grep -q '^PACKAGER_PRIVKEY=' "$HOME/.abuild/abuild.conf" 2>/dev/null; then
    mkdir -p "$HOME/.abuild"
    printf 'PACKAGER_PRIVKEY="%s"\n' "${abuild_keys[0]}" >> "$HOME/.abuild/abuild.conf"
fi

# abuild -r runs its apk wrapper through the setuid abuild-sudo helper, which
# only accepts members of the abuild group. Without membership abuild fails
# with a bare "not a member of group abuild" error, so warn with the fix.
if ! id -nG 2>/dev/null | grep -qw abuild; then
    echo "WARNING: $(id -un) is not a member of the 'abuild' group; 'abuild -r' cannot install missing makedepends." >&2
    echo "         Add the build user to the group and start a new session first:" >&2
    echo "           sudo addgroup $(id -un) abuild" >&2
fi

# abuild itself does not modify the VM's package set unless -r is requested to
# install missing makedepends. Keep package creation on the existing
# environment by default; use ABUILD_ARGS='-r -d' (or similar) when desired.
read -r -a abuild_args <<< "${ABUILD_ARGS:--r}"
(
    cd "$BUILD_DIR"
    abuild "${abuild_args[@]}" -P "$BUILD_DIR/repo"
)

mapfile -t packages < <(
    find "$BUILD_DIR" -type f -name '*.apk' -print | sort
)
[[ ${#packages[@]} -gt 0 ]] || {
    echo "abuild completed but no Alpine package was generated." >&2
    exit 1
}

echo
echo "Alpine package created:"
printf '  %s\n' "${packages[@]}"
echo
echo "Nothing was installed on the host. Install with:"
echo "  sudo apk add --allow-untrusted ${packages[0]}"
echo
echo "Alpine has no repository for this package. The package is signed by the"
echo "local abuild key; to install it without --allow-untrusted, install the"
echo "matching public key first:"
echo "  sudo cp $HOME/.abuild/*.rsa.pub /etc/apk/keys/"

"$ROOT_DIR/scripts/test-apk.sh" "${packages[0]}"
