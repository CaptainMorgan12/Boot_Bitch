#!/usr/bin/env bash
# Kernel-classification parity contract: the modern helper and the generated
# legacy port must classify the same kernel inventory identically (same
# KERNEL_INVENTORY_VERSIONS / MISSING_HEADERS / ORPHANED_HEADERS /
# ORPHANED_KERNELS / PARTIAL / BROKEN / STALE / REMOVABLE / LATEST / RUNNING
# and the same per-version package grouping).  Kernel-classification fixes in
# the modern helper therefore reach the legacy port — and a divergence fails
# this contract even when each helper's own tests pass.
#
# Fixtures cover the trap classes found on the rigs:
#   F1 Etch bare-common naming: linux-headers-<series> (no "-common" suffix)
#      is the arch headers package's dependency and must NEVER be reported as
#      an orphaned header version (mawk/POSIX-class and depends-parse traps).
#   F2 TUXEDO -common naming: linux-headers-<v>-common-<arch> likewise never
#      orphaned while its arch headers are installed.
#   F3 mixed set: one healthy latest, one partial, one stale — the
#      per-version roles must match exactly.
# shellcheck disable=SC1090,SC2034
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
MODERN="$ROOT_DIR/scripts/boot-repair-helper.sh"
LEGACY="$ROOT_DIR/legacy/boot-repair-helper.sh"

[[ -f "$MODERN" && -f "$LEGACY" ]] || { echo 'FAIL: helper(s) missing' >&2; exit 1; }
bash -n "$MODERN" && bash -n "$LEGACY"

fail() { printf 'FAIL: %s\n' "$*" >&2; exit 1; }

# Snapshot the classification globals after a fresh collection.  The helper's
# top-level init RESETS the env-derived globals on source, so TARGET_ROOT and
# friends are set AFTER sourcing (documented harness rule, see the 0.2.28
# kernel-menu investigation).
collect_classification()
{
    local helper="$1" root="$2" pm="${3:-apt/dpkg}" snapshot="$4"
    HELPER="$helper" SAVED_ROOT="$root" SAVED_PM="$pm" SNAPSHOT="$snapshot" \
        bash --noprofile --norc -c '
        set -u
        source <(head -n -1 "$HELPER") >/dev/null 2>&1
        TARGET_ROOT="$SAVED_ROOT"
        TARGET_DEVICE=/dev/hda
        RUNNING_HOST_MODE=1
        TARGET_PACKAGE_MANAGER="$SAVED_PM"
        kernel_inventory_collect
        {
            printf "VERSIONS: %s\n" "${KERNEL_INVENTORY_VERSIONS[*]}"
            printf "RUNNING: %s\n" "${KERNEL_RUNNING:-}"
            printf "LATEST: %s\n" "${KERNEL_LATEST:-}"
            printf "MISSING_HEADERS: %s\n" "${KERNEL_MISSING_HEADERS[*]}"
            printf "ORPHANED_HEADERS: %s\n" "${KERNEL_ORPHANED_HEADERS[*]}"
            printf "ORPHANED_KERNELS: %s\n" "${KERNEL_ORPHANED_KERNELS[*]}"
            printf "PARTIAL: %s\n" "${KERNEL_PARTIAL[*]}"
            printf "BROKEN: %s\n" "${KERNEL_BROKEN[*]}"
            printf "STALE: %s\n" "${KERNEL_STALE[*]}"
            printf "REMOVABLE: %s\n" "${KERNEL_REMOVABLE[*]}"
            printf "PKGS: %s\n" "${KERNEL_INVENTORY_PKGS[*]}"
        } > "$SNAPSHOT"
    '
}

# Fixture 1: Etch bare-common naming (the 2026-10-16 rig finding).
fixture_etch()
{
    local root="$1"
    mkdir -p "$root/var/lib/dpkg/info" "$root/lib/modules/2.6.18-6-amd64" "$root/boot"
    cat > "$root/var/lib/dpkg/status" <<'EOF'
Package: linux-image-2.6.18-6-amd64
Status: install ok installed
Version: 2.6.18.dfsg.1-24

Package: linux-image-2.6-amd64
Status: install ok installed
Version: 2.6.18.dfsg.1-24

Package: linux-headers-2.6.18-6
Status: install ok installed
Version: 2.6.18.dfsg.1-26etch2

Package: linux-headers-2.6.18-6-amd64
Status: install ok installed
Version: 2.6.18.dfsg.1-26etch2
Depends: linux-headers-2.6.18-6 (= 2.6.18.dfsg.1-26etch2), linux-kbuild-2.6.18

Package: linux-kbuild-2.6.18
Status: install ok installed
Version: 2.6.18.dfsg.1-26etch2
EOF
}

# Fixture 2: TUXEDO -common naming plus a stale removable kernel.
fixture_tuxedo()
{
    local root="$1"
    mkdir -p "$root/var/lib/dpkg/info" \
        "$root/lib/modules/6.11.0-108009-tuxedo" "$root/lib/modules/6.11.0-107009-tuxedo" \
        "$root/boot"
    cat > "$root/var/lib/dpkg/status" <<'EOF'
Package: linux-image-6.11.0-108009-tuxedo
Status: install ok installed
Version: 6.11.0-108009.1

Package: linux-headers-6.11.0-108009-tuxedo-amd64
Status: install ok installed
Version: 6.11.0-108009.1
Depends: linux-headers-6.11.0-108009-common-amd64, linux-kbuild-6.11.0

Package: linux-headers-6.11.0-108009-common-amd64
Status: install ok installed
Version: 6.11.0-108009.1

Package: linux-kbuild-6.11.0
Status: install ok installed
Version: 6.11.0-108009.1

Package: linux-image-6.11.0-107009-tuxedo
Status: install ok installed
Version: 6.11.0-107009.1

Package: linux-headers-6.11.0-107009-tuxedo-amd64
Status: install ok installed
Version: 6.11.0-107009.1
Depends: linux-headers-6.11.0-107009-common-amd64, linux-kbuild-6.11.0

Package: linux-headers-6.11.0-107009-common-amd64
Status: install ok installed
Version: 6.11.0-107009.1
EOF
}

# Fixture 3: partial + stale mix (Debian -amd64 naming).
fixture_mixed()
{
    local root="$1"
    mkdir -p "$root/var/lib/dpkg/info" \
        "$root/lib/modules/6.1.0-25-amd64" "$root/lib/modules/6.1.0-23-amd64" \
        "$root/boot"
    cat > "$root/var/lib/dpkg/status" <<'EOF'
Package: linux-image-6.1.0-25-amd64
Status: install ok installed
Version: 6.1.115-1

Package: linux-headers-6.1.0-25-amd64
Status: install ok installed
Version: 6.1.115-1
Depends: linux-headers-6.1.0-25-common, linux-kbuild-6.1

Package: linux-headers-6.1.0-25-common
Status: install ok installed
Version: 6.1.115-1

Package: linux-kbuild-6.1
Status: install ok installed
Version: 6.1.115-1

Package: linux-image-6.1.0-23-amd64
Status: deinstall ok config-files
Version: 6.1.90-1
EOF
}

sandbox=$(mktemp -d "${TMPDIR:-/tmp}/kpar.XXXXXX")
trap 'rm -rf "$sandbox"' EXIT

i=0
for fixture in fixture_etch fixture_tuxedo fixture_mixed; do
    i=$((i + 1))
    root="$sandbox/f$i"
    "$fixture" "$root"
    collect_classification "$MODERN" "$root" apt/dpkg "$sandbox/f${i}.modern"
    collect_classification "$LEGACY" "$root" apt/dpkg "$sandbox/f${i}.legacy"
    if ! diff -u "$sandbox/f${i}.modern" "$sandbox/f${i}.legacy" > "$sandbox/f${i}.diff"; then
        echo "FAIL: modern vs legacy kernel classification diverged on $fixture" >&2
        cat "$sandbox/f${i}.diff" >&2
        exit 1
    fi
    # Grounded trap assertion (the 2026-10-16 Etch rig finding): the
    # bare-common headers must never surface as orphaned in either helper.
    case "$fixture" in
        fixture_etch)
            grep -q 'ORPHANED_HEADERS: $' "$sandbox/f${i}.modern" \
                || fail "etch fixture: the bare-common headers are flagged as orphaned"
            ;;
    esac
    echo "ok - $fixture classification parity (modern == legacy)"
done

echo "kernel classification parity contract: PASS"
