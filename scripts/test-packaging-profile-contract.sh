#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"

for script in setup-dev-deps.sh check-dev-env.sh build.sh package-deb.sh package-rpm.sh package-arch.sh package-tarball.sh install.sh; do
    [[ -x "$ROOT_DIR/scripts/$script" ]] || {
        echo "FAIL: packaging script is not executable: $script" >&2
        exit 1
    }
    bash -n "$ROOT_DIR/scripts/$script"
done

grep -q "pacman -Syu --needed" "$ROOT_DIR/scripts/setup-dev-deps.sh"
grep -q "apt-get install --no-install-recommends" "$ROOT_DIR/scripts/setup-dev-deps.sh"
grep -q 'zypper --non-interactive install' "$ROOT_DIR/scripts/setup-dev-deps.sh"
grep -q 'dnf install -y' "$ROOT_DIR/scripts/setup-dev-deps.sh"
grep -q 'CPACK_RPM_PACKAGE_REQUIRES' "$ROOT_DIR/CMakeLists.txt"
grep -q 'host_is_debian' "$ROOT_DIR/scripts/build.sh"
grep -q 'Debian-family packaging hosts' "$ROOT_DIR/scripts/package-deb.sh"
grep -q 'cpack -G RPM' "$ROOT_DIR/scripts/package-rpm.sh"
grep -q 'makepkg' "$ROOT_DIR/scripts/package-arch.sh"
grep -q 'options=(!debug)' "$ROOT_DIR/scripts/package-arch.sh"
grep -q -- '--source' "$ROOT_DIR/scripts/install.sh"
grep -q 'pacman -U' "$ROOT_DIR/scripts/install.sh"
grep -q 'zypper --non-interactive install' "$ROOT_DIR/scripts/install.sh"
grep -q 'boot-repair-source-install-manifest.txt' "$ROOT_DIR/scripts/install.sh"
grep -q 'INSTALL_MANIFEST' "$ROOT_DIR/scripts/uninstall.sh"

# Exercise install.sh's no-package path without building or invoking a package
# manager. This checks that an Arch/SUSE host receives the native-package and
# source-install guidance instead of an unconditional apt-get failure.
output="$(env BOOT_REPAIR_PACKAGE_FAMILY=arch PATH=/usr/bin:/bin "$ROOT_DIR/scripts/install.sh" 2>&1 || true)"
grep -q 'package-arch.sh' <<<"$output"
grep -q -- '--source' <<<"$output"

echo "PASS: package-manager-aware build, native-package and source-install contracts are wired."
