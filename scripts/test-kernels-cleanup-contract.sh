#!/usr/bin/env bash
# Old-kernel cleanup (`kernels` stage / `kernel` diagnostic) helper contract.
#
# Static wiring plus a behavioural harness that sources the helper without main
# and replaces only the read-only target probes, the package-manager transcript
# plumbing and the initramfs/GRUB reconcilers.  No real block device, package
# manager or bootloader is ever touched.  Kernel identity is verified from the
# authoritative module-tree directories mapped to packages through dpkg -S
# ownership (.list files), so the TUXEDO linux-headers-<ver>-common-<arch> /
# -tuxedo-amd64 naming, the default -amd64 naming and a mixed set all resolve
# identically.
# shellcheck disable=SC1090,SC2034,SC2317
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="${HELPER:-$ROOT_DIR/scripts/boot-repair-helper.sh}"

[[ -x "$HELPER" ]] || { echo 'FAIL: helper is not executable' >&2; exit 1; }
bash -n "$HELPER"

fail() { printf 'FAIL: %s\n' "$*" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Static wiring contracts
# ---------------------------------------------------------------------------
for fn in apt_installed_kernel_packages apt_installed_kernel_support_packages dpkg_path_owner apt_package_depends \
    apt_package_owned_dirs apt_managed_kernel_metapackage apt_managed_kernel_versions \
    apt_kernel_version_from_package apt_kernel_support_version_from_package \
    kernel_support_version_is_orphaned kernel_version_total_size kernel_inventory_collect \
    parse_cached_kernel_inventory repair_kernel_prepare_inventory \
    kernel_is_removable kernel_package_is_removable kernel_is_removal_candidate \
    kernel_package_is_removal_candidate apt_kernel_repo_latest diagnostic_kernel \
    diagnostic_kernel_inventory \
    kernels_unavailable_reason apt_kernel_removal_is_safe apt_kernel_protected_set_present \
    apt_kernel_cleanup_stage apt_kernel_zombie_cleanup apt_install_missing_latest_headers \
    apt_cleanup_orphaned_headers apt_orphaned_headers_removal_is_safe \
    apt_cleanup_orphaned_build_support \
    repair_kernel_cleanup repair_kernel_remove_oldest repair_kernel_remove_newest \
    repair_kernel_remove_orphaned_headers repair_kernel_remove_orphaned_kernels \
    repair_kernel_remove_orphaned_support \
    repair_kernel_install_headers repair_kernel_reinstall repair_kernel_install_repo_latest \
    repair_kernel_verify; do
    grep -q "^${fn}()" "$HELPER" || fail "helper is missing ${fn}()"
done

# The 15 capability keys are registered; kernels gates through the reasons array.
grep -q 'local -a keys=(validate filesystem dpkg fixbroken aptupdate upgrade dkms display gpu initramfs efi grub extlinux bootstack kernels)' "$HELPER" \
    || fail 'the keys array does not carry the kernels capability key'
grep -q 'reasons\[kernels\]="available"' "$HELPER" \
    || fail 'diagnostic_repair_capabilities does not gate kernels through the reasons array'
# The stage is wired into validate/stage_rank/backend-validation/dispatch.
grep -q 'dpkg-configure|fix-broken|apt-update|apt-upgrade|dkms|display-manager|initramfs|efi|boot-stack|grub|extlinux|kernels|gpu|' "$HELPER" \
    || fail 'validate_stage does not accept the kernels stage'
grep -q 'kernels) printf' "$HELPER" || fail 'stage_rank has no kernels rank'
grep -q 'kernels) kernel_interactive_menu' "$HELPER" || fail 'repair dispatch is missing kernels'
grep -q '^kernel_interactive_menu()' "$HELPER" || fail 'helper is missing kernel_interactive_menu()'
grep -q '^helper_menu_prompt()' "$HELPER" || fail 'helper is missing helper_menu_prompt()'
grep -q 'kernel-cleanup:apt/dpkg) apt_kernel_cleanup_stage' "$HELPER" \
    || fail 'run_package_stage has no kernel-cleanup apt/dpkg case'
# The dedicated validator must NOT reuse apt_simulation_is_safe's blanket
# linux-* refusal: it permits only the exact removable package set.
grep -q 'apt_kernel_removal_is_safe' "$HELPER" || fail 'kernel validator missing'
grep -q 'kernel_package_is_removable' "$HELPER" || fail 'exact-membership removable check missing'
# Stable evidence lines are emitted verbatim (never translated).
grep -q '^boot_space_evidence_line()' "$HELPER" || fail '/boot/ESP free-space evidence helper missing'
grep -q 'boot_space_evidence_line "\$(target_path /boot)" "/boot"' "$HELPER" || fail '/boot free-space evidence line missing'
grep -q 'Kernel inventory: total=' "$HELPER" || fail 'kernel inventory evidence line missing'
grep -q 'Kernel removable: ' "$HELPER" || fail 'kernel removable evidence line missing'
grep -q 'Kernel protected: ' "$HELPER" || fail 'kernel protected evidence line missing'
grep -q 'Kernel missing headers: ' "$HELPER" || fail 'kernel missing-headers evidence line missing'
grep -q 'Kernel orphaned headers: ' "$HELPER" || fail 'kernel orphaned-headers evidence line missing'
grep -q 'Kernel repository latest: ' "$HELPER" || fail 'kernel repo-latest evidence line missing'
grep -q 'Kernel state: running=' "$HELPER" || fail 'kernel state relationship line missing'
grep -q 'Kernel installed: ' "$HELPER" || fail 'kernel per-kernel installed evidence line missing'
# The missing-headers install mode hint is accepted by the repair arg parser.
grep -q -- '--install-latest-headers) REPAIR_INSTALL_LATEST_HEADERS=true' "$HELPER" \
    || fail 'the --install-latest-headers mode hint is not parsed'
grep -q -- '--remove-newest) REPAIR_REMOVE_NEWEST=true' "$HELPER" \
    || fail 'the --remove-newest mode hint is not parsed'
grep -q -- '--clean-orphaned-headers) REPAIR_CLEAN_ORPHANED_HEADERS=true' "$HELPER" \
    || fail 'the --clean-orphaned-headers mode hint is not parsed'
# The cached kernel-section baseline is accepted by the repair arg parser.
grep -q -- '--baseline)' "$HELPER" \
    || fail 'the --baseline kernel baseline mode hint is not parsed'
grep -q 'KERNEL_BASELINE_B64=' "$HELPER" \
    || fail 'the kernel baseline state variable is missing'
# The boot-space diagnostic is consolidated into `kernel`: no dispatch case and
# no section-list entry may remain.
! grep -q 'boot-space)  diagnostic_boot_space' "$HELPER" || fail 'boot-space diagnostic dispatch still present'
! grep -q '^diagnostic_boot_space()' "$HELPER" || fail 'diagnostic_boot_space function still present'
grep -q 'errors usage fstab btrfs mapper luks filesystem' "$HELPER" || fail 'section list must place usage directly before fstab'
# The interactive kernel repair evidence lines are emitted verbatim.
grep -q 'Kernel orphaned kernel: ' "$HELPER" || fail 'kernel orphaned-kernel evidence line missing'
grep -q 'Kernel incomplete: ' "$HELPER" || fail 'kernel incomplete evidence line missing'
grep -q 'Kernel orphaned build support: ' "$HELPER" || fail 'kernel orphaned build-support evidence line missing'
# Each interactive sub-action is wired into validate/stage_rank/backend-validation/dispatch.
for stage in kernel-remove-oldest kernel-remove-newest kernel-remove-orphaned-headers \
    kernel-remove-orphaned-kernels kernel-remove-orphaned-support kernel-install-headers kernel-reinstall kernel-install-repo-latest; do
    grep -q "${stage}) repair_kernel_" "$HELPER" \
        || fail "repair dispatch is missing ${stage}"
done
grep -q 'kernel-remove-oldest|kernel-remove-newest' "$HELPER" || fail 'validate_stage does not accept the kernel repair sub-actions'
grep -q 'KERNEL_STAGE_STATUS_KEY="kernels:remove-oldest"' "$HELPER" || fail 'remove-oldest does not emit its sub-action status key'
grep -q 'KERNEL_STAGE_STATUS_KEY="kernels:remove-newest"' "$HELPER" || fail 'remove-newest does not emit its sub-action status key'
grep -q 'KERNEL_STAGE_STATUS_KEY="kernels:remove-orphaned-support"' "$HELPER" || fail 'remove-orphaned-support does not emit its sub-action status key'
grep -q 'remove-orphaned-support) repair_kernel_remove_orphaned_support' "$HELPER" || fail 'interactive menu is missing remove-orphaned-support'

# ---------------------------------------------------------------------------
# Behavioural harness
# ---------------------------------------------------------------------------
sandbox="$(mktemp -d)"
cleanup_sandbox() { rm -rf -- "$sandbox"; }
trap cleanup_sandbox EXIT

fake_root="$sandbox/root"
mkdir -p "$fake_root/var/lib/dpkg/info" "$fake_root/boot" "$fake_root/usr/bin" \
    "$fake_root/etc/apt"
touch "$fake_root/usr/bin/dpkg" "$fake_root/usr/bin/apt-get" "$fake_root/usr/bin/update-initramfs" \
    "$fake_root/usr/bin/grub-mkconfig"
chmod +x "$fake_root/usr/bin"/*

# Write one package file list (the dpkg -S ownership database).
write_list() {
    local pkg="$1"; shift
    : > "$fake_root/var/lib/dpkg/info/$pkg.list"
    for path in "$@"; do
        printf '%s\n' "$path" >> "$fake_root/var/lib/dpkg/info/$pkg.list"
    done
}

# Reset every fixture-owned path so one fixture never pollutes the next.
reset_fixture() {
    rm -rf "$fake_root/var/lib/dpkg/info" "$fake_root/usr/lib/modules" \
        "$fake_root/lib/modules" "$fake_root/boot" "$fake_root/usr/src" \
        "$fake_root/var/lib/apt/lists"
    mkdir -p "$fake_root/var/lib/dpkg/info" "$fake_root/boot" "$fake_root/usr/src"
    : > "$fake_root/var/lib/dpkg/status"
}

# TUXEDO fixture: old 7.1.8.2 + running/latest 7.1.9.1, each with
# -common-* arch-independent headers and both linux-image-tuxedo /
# linux-headers-tuxedo-amd64 metapackages.
write_tuxedo_fixture() {
    reset_fixture
    cat > "$fake_root/var/lib/dpkg/status" <<'STATUS'
Package: linux-image-7.1.8.2-tuxedo-amd64
Version: 7.1.8.2
Status: install ok installed

Package: linux-modules-7.1.8.2-tuxedo-amd64
Version: 7.1.8.2
Status: install ok installed

Package: linux-headers-7.1.8.2-tuxedo-amd64
Version: 7.1.8.2
Depends: linux-headers-7.1.8.2-common-tuxedo (= 7.1.8.2-1)
Status: install ok installed

Package: linux-headers-7.1.8.2-common-tuxedo
Version: 7.1.8.2
Status: install ok installed

Package: linux-image-7.1.9.1-tuxedo-amd64
Version: 7.1.9.1
Status: install ok installed

Package: linux-modules-7.1.9.1-tuxedo-amd64
Version: 7.1.9.1
Status: install ok installed

Package: linux-headers-7.1.9.1-tuxedo-amd64
Version: 7.1.9.1
Depends: linux-headers-7.1.9.1-common-tuxedo (= 7.1.9.1-1)
Status: install ok installed

Package: linux-headers-7.1.9.1-common-tuxedo
Version: 7.1.9.1
Status: install ok installed

Package: linux-image-tuxedo
Version: 7.1.9.1.2
Depends: linux-image-7.1.9.1-tuxedo-amd64 (= 7.1.9.1-1)
Status: install ok installed

Package: linux-headers-tuxedo-amd64
Version: 7.1.9.1.2
Depends: linux-headers-7.1.9.1-tuxedo-amd64 (= 7.1.9.1-1)
Status: install ok installed

STATUS
    local old=7.1.8.2-tuxedo-amd64 new=7.1.9.1-tuxedo-amd64
    mkdir -p "$fake_root/usr/lib/modules/$old" "$fake_root/usr/lib/modules/$new" \
        "$fake_root/usr/src/linux-headers-$old" "$fake_root/usr/src/linux-headers-$new" \
        "$fake_root/usr/src/linux-headers-7.1.8.2-common" "$fake_root/usr/src/linux-headers-7.1.9.1-common"
    touch "$fake_root/boot/vmlinuz-$old" "$fake_root/boot/vmlinuz-$new" \
        "$fake_root/boot/initrd.img-$old" "$fake_root/boot/initrd.img-$new" \
        "$fake_root/boot/System.map-$old" "$fake_root/boot/config-$old"
    write_list "linux-image-$old" "/boot/vmlinuz-$old"
    write_list "linux-modules-$old" "/usr/lib/modules/$old" "/usr/lib/modules/$old/kernel/drv.ko"
    write_list "linux-headers-$old" "/usr/src/linux-headers-$old" "/usr/src/linux-headers-$old/include/x.h"
    write_list "linux-headers-7.1.8.2-common-tuxedo" "/usr/src/linux-headers-7.1.8.2-common" "/usr/src/linux-headers-7.1.8.2-common/include/y.h"
    write_list "linux-image-$new" "/boot/vmlinuz-$new"
    write_list "linux-modules-$new" "/usr/lib/modules/$new" "/usr/lib/modules/$new/kernel/drv.ko"
    write_list "linux-headers-$new" "/usr/src/linux-headers-$new" "/usr/src/linux-headers-$new/include/x.h"
    write_list "linux-headers-7.1.9.1-common-tuxedo" "/usr/src/linux-headers-7.1.9.1-common" "/usr/src/linux-headers-7.1.9.1-common/include/y.h"
}

# Default -amd64/generic naming: old 6.2.0-1000-generic + running/latest
# 6.5.0-10038-generic, with bare linux-headers-<series> common headers.
write_generic_fixture() {
    reset_fixture
    cat > "$fake_root/var/lib/dpkg/status" <<'STATUS'
Package: linux-image-6.2.0-1000-generic
Version: 6.2.0-1000
Status: install ok installed

Package: linux-modules-6.2.0-1000-generic
Version: 6.2.0-1000
Status: install ok installed

Package: linux-headers-6.2.0-1000-generic
Version: 6.2.0-1000
Depends: linux-headers-6.2.0-1000 (= 6.2.0-1000.1)
Status: install ok installed

Package: linux-headers-6.2.0-1000
Version: 6.2.0-1000
Status: install ok installed

Package: linux-image-6.5.0-10038-generic
Version: 6.5.0-10038
Status: install ok installed

Package: linux-modules-6.5.0-10038-generic
Version: 6.5.0-10038
Status: install ok installed

Package: linux-headers-6.5.0-10038-generic
Version: 6.5.0-10038
Depends: linux-headers-6.5.0-10038 (= 6.5.0-10038.1)
Status: install ok installed

Package: linux-headers-6.5.0-10038
Version: 6.5.0-10038
Status: install ok installed

Package: linux-image-generic
Version: 6.5.0.10038.1
Depends: linux-image-6.5.0-10038-generic (= 6.5.0-10038.1)
Status: install ok installed

Package: linux-headers-generic
Version: 6.5.0.10038.1
Depends: linux-headers-6.5.0-10038-generic (= 6.5.0-10038.1)
Status: install ok installed

STATUS
    local old=6.2.0-1000-generic new=6.5.0-10038-generic
    mkdir -p "$fake_root/usr/lib/modules/$old" "$fake_root/usr/lib/modules/$new" \
        "$fake_root/usr/src/linux-headers-$old" "$fake_root/usr/src/linux-headers-$new" \
        "$fake_root/usr/src/linux-headers-6.2.0-1000" "$fake_root/usr/src/linux-headers-6.5.0-10038"
    touch "$fake_root/boot/vmlinuz-$old" "$fake_root/boot/vmlinuz-$new"
    write_list "linux-image-$old" "/boot/vmlinuz-$old"
    write_list "linux-modules-$old" "/usr/lib/modules/$old" "/usr/lib/modules/$old/kernel/drv.ko"
    write_list "linux-headers-$old" "/usr/src/linux-headers-$old" "/usr/src/linux-headers-$old/include/x.h"
    write_list "linux-headers-6.2.0-1000" "/usr/src/linux-headers-6.2.0-1000" "/usr/src/linux-headers-6.2.0-1000/include/y.h"
    write_list "linux-image-$new" "/boot/vmlinuz-$new"
    write_list "linux-modules-$new" "/usr/lib/modules/$new" "/usr/lib/modules/$new/kernel/drv.ko"
    write_list "linux-headers-$new" "/usr/src/linux-headers-$new" "/usr/src/linux-headers-$new/include/x.h"
    write_list "linux-headers-6.5.0-10038" "/usr/src/linux-headers-6.5.0-10038" "/usr/src/linux-headers-6.5.0-10038/include/y.h"
}

# Legacy 2.6.x/3.x naming: three kernels whose module-tree enumeration must be
# ordered by `sort -V` (2.6.26 < 2.6.32 < 3.5.0), never by lexical file-name
# flavour parsing.  The running kernel is the middle 2.6.32-5-amd64, so the
# oldest 2.6.26-2-amd64 is removable and the newest 3.5.0-27-generic is latest.
write_legacy_fixture() {
    reset_fixture
    cat > "$fake_root/var/lib/dpkg/status" <<'STATUS'
Package: linux-image-2.6.26-2-amd64
Version: 2.6.26-2
Status: install ok installed

Package: linux-modules-2.6.26-2-amd64
Version: 2.6.26-2
Status: install ok installed

Package: linux-headers-2.6.26-2-amd64
Version: 2.6.26-2
Status: install ok installed

Package: linux-image-2.6.32-5-amd64
Version: 2.6.32-5
Status: install ok installed

Package: linux-modules-2.6.32-5-amd64
Version: 2.6.32-5
Status: install ok installed

Package: linux-headers-2.6.32-5-amd64
Version: 2.6.32-5
Status: install ok installed

Package: linux-image-3.5.0-27-generic
Version: 3.5.0-27
Status: install ok installed

Package: linux-modules-3.5.0-27-generic
Version: 3.5.0-27
Status: install ok installed

Package: linux-headers-3.5.0-27-generic
Version: 3.5.0-27
Status: install ok installed

Package: linux-image-amd64
Version: 3.5.0.27.30
Depends: linux-image-3.5.0-27-generic (= 3.5.0-27.30)
Status: install ok installed

STATUS
    local oldest=2.6.26-2-amd64 mid=2.6.32-5-amd64 newest=3.5.0-27-generic
    mkdir -p "$fake_root/usr/lib/modules/$oldest" "$fake_root/usr/lib/modules/$mid" \
        "$fake_root/usr/lib/modules/$newest" \
        "$fake_root/usr/src/linux-headers-$oldest" "$fake_root/usr/src/linux-headers-$mid" \
        "$fake_root/usr/src/linux-headers-$newest"
    touch "$fake_root/boot/vmlinuz-$oldest" "$fake_root/boot/vmlinuz-$mid" "$fake_root/boot/vmlinuz-$newest"
    write_list "linux-image-$oldest" "/boot/vmlinuz-$oldest"
    write_list "linux-modules-$oldest" "/usr/lib/modules/$oldest" "/usr/lib/modules/$oldest/kernel/a.ko"
    write_list "linux-headers-$oldest" "/usr/src/linux-headers-$oldest" "/usr/src/linux-headers-$oldest/include/a.h"
    write_list "linux-image-$mid" "/boot/vmlinuz-$mid"
    write_list "linux-modules-$mid" "/usr/lib/modules/$mid" "/usr/lib/modules/$mid/kernel/b.ko"
    write_list "linux-headers-$mid" "/usr/src/linux-headers-$mid" "/usr/src/linux-headers-$mid/include/b.h"
    write_list "linux-image-$newest" "/boot/vmlinuz-$newest"
    write_list "linux-modules-$newest" "/usr/lib/modules/$newest" "/usr/lib/modules/$newest/kernel/c.ko"
    write_list "linux-headers-$newest" "/usr/src/linux-headers-$newest" "/usr/src/linux-headers-$newest/include/c.h"
}

# Orphaned build/support fixture: the 7.1.10.2 kernel was removed (no module
# tree and no image/modules package) while its linux-kbuild-7.1.10.2 support
# package survived; the still-installed 7.1.13.1 kernel keeps linux-kbuild-7.1.13.1.
# The kbuild packages own their own files, never the module tree, so they only
# surface through the support-package enumerator.
write_kbuild_fixture() {
    reset_fixture
    cat > "$fake_root/var/lib/dpkg/status" <<'STATUS'
Package: linux-image-7.1.13.1-tuxedo-amd64
Version: 7.1.13.1
Status: install ok installed

Package: linux-modules-7.1.13.1-tuxedo-amd64
Version: 7.1.13.1
Status: install ok installed

Package: linux-kbuild-7.1.10.2
Version: 7.1.10.2
Status: install ok installed

Package: linux-kbuild-7.1.13.1
Version: 7.1.13.1
Status: install ok installed

STATUS
    local new=7.1.13.1-tuxedo-amd64
    mkdir -p "$fake_root/usr/lib/modules/$new" \
        "$fake_root/usr/lib/linux-kbuild-7.1.10.2" \
        "$fake_root/usr/lib/linux-kbuild-7.1.13.1"
    touch "$fake_root/boot/vmlinuz-$new"
    write_list "linux-image-$new" "/boot/vmlinuz-$new"
    write_list "linux-modules-$new" "/usr/lib/modules/$new" "/usr/lib/modules/$new/kernel/drv.ko"
    write_list "linux-kbuild-7.1.10.2" "/usr/lib/linux-kbuild-7.1.10.2/tools/Makefile"
    write_list "linux-kbuild-7.1.13.1" "/usr/lib/linux-kbuild-7.1.13.1/tools/Makefile"
}

source <(sed '/^main "$@"/d' "$HELPER")
trap - EXIT INT TERM HUP

profile_target_backends() {
    TARGET_DISTRO_FAMILY=debian
    TARGET_PACKAGE_MANAGERS=(apt/dpkg)
    TARGET_PACKAGE_MANAGER=apt/dpkg
    TARGET_SERVICE_MANAGERS=(systemd)
    TARGET_SERVICE_MANAGER=systemd
    TARGET_DISPLAY_BACKEND=systemd
    TARGET_INITRAMFS_BACKENDS=(initramfs-tools)
    TARGET_INITRAMFS_BACKEND=initramfs-tools
    TARGET_LOGGING_BACKEND=journald
    TARGET_BOOTLOADER_BACKEND=grub
    TARGET_ESP_MOUNT=unresolved
    TARGET_KERNEL_LAYOUT='versioned vmlinuz-* kernels'
    TARGET_REPAIR_BACKEND='Debian/APT'
}
running_kernel_version() { printf '%s\n' "${FAKE_RUNNING-7.1.9.1-tuxedo-amd64}"; }

TARGET_ROOT="$fake_root"
RUNNING_HOST_MODE=0
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"

# ---- TUXEDO inventory: grouping, protection set, removable plan -------------
write_tuxedo_fixture
meta="$(apt_managed_kernel_metapackage)"
grep -qx 'linux-headers-tuxedo-amd64' <<<"$meta" \
    || fail "managed metapackage must include linux-headers-tuxedo-amd64: $(echo "$meta")"
grep -qx 'linux-image-tuxedo' <<<"$meta" \
    || fail "managed metapackage must include linux-image-tuxedo: $(echo "$meta")"

kernel_inventory_collect
[[ "$KERNEL_RUNNING" == 7.1.9.1-tuxedo-amd64 ]] || fail "running kernel: $KERNEL_RUNNING"
[[ "$KERNEL_LATEST" == 7.1.9.1-tuxedo-amd64 ]] || fail "latest kernel: $KERNEL_LATEST"
[[ "${#KERNEL_INVENTORY_VERSIONS[@]}" -eq 2 ]] || fail "version count: ${#KERNEL_INVENTORY_VERSIONS[@]}"
[[ "${#KERNEL_REMOVABLE[@]}" -eq 1 ]] || fail "removable count: ${#KERNEL_REMOVABLE[@]}"
[[ "${KERNEL_REMOVABLE[0]}" == 7.1.8.2-tuxedo-amd64 ]] || fail "removable version: ${KERNEL_REMOVABLE[0]}"
[[ "${#KERNEL_METAPACKAGES[@]}" -eq 2 ]] || fail "metapackage count: ${#KERNEL_METAPACKAGES[@]}"

# The old kernel's plan is exactly image+modules+arch-headers+common-headers.
rem_pkgs="${KERNEL_REMOVABLE_PKGS[*]}"
for p in linux-image-7.1.8.2-tuxedo-amd64 linux-modules-7.1.8.2-tuxedo-amd64 \
         linux-headers-7.1.8.2-tuxedo-amd64 linux-headers-7.1.8.2-common-tuxedo; do
    grep -q "$p" <<<"$rem_pkgs" || fail "removable plan missing $p"
done
[[ "$(wc -w <<<"$rem_pkgs" | tr -d '[:space:]')" -eq 4 ]] \
    || fail "removable plan must hold exactly 4 packages: $rem_pkgs"
# The protected kernel keeps its common headers and both metapackages.
kernel_package_is_removable linux-headers-7.1.9.1-common-tuxedo \
    && fail 'the protected kernel common headers must never be removable'
kernel_package_is_removable linux-headers-tuxedo-amd64 \
    && fail 'the headers metapackage must never be removable'
kernel_package_is_removable linux-image-tuxedo \
    && fail 'the image metapackage must never be removable'

# ---- capability gate -------------------------------------------------------
if reason="$(kernels_unavailable_reason)"; then
    : # available
else
    fail "kernels should be available with a removable kernel; got $reason"
fi

# ---- diagnostic evidence ---------------------------------------------------
output="$(diagnostic_kernel || true)"
grep -q '^Kernel inventory: total=2 protected=1 removable=1 backend=apt/dpkg$' <<<"$output" \
    || fail 'kernel inventory summary wrong'
grep -q '^Kernel removable: 7.1.8.2-tuxedo-amd64 size=.* pkgs=.*linux-headers-7.1.8.2-common-tuxedo.*$' <<<"$output" \
    || fail 'removable line must carry the grouped common headers'
grep -q '^Kernel protected: linux-headers-tuxedo-amd64 role=metapackage$' <<<"$output" \
    || fail 'headers metapackage protected line missing'
grep -q '^Kernel protected: linux-image-tuxedo role=metapackage$' <<<"$output" \
    || fail 'image metapackage protected line missing'

# ---- cached-baseline parser round-trip --------------------------------------
# Parsing the cached `kernel` diagnostic section must reconstruct the same
# inventory globals kernel_inventory_collect sets, without re-collecting.
write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
kernel_inventory_collect
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
# Wipe every inventory global so the parse is provably rebuilding from the
# baseline rather than reusing the collected state.
KERNEL_INVENTORY_VERSIONS=()
KERNEL_INVENTORY_PKGS=()
KERNEL_INVENTORY_SIZES=()
KERNEL_RUNNING=""
KERNEL_LATEST=""
KERNEL_REPO_LATEST=""
KERNEL_METAPACKAGES=()
KERNEL_MANAGED=()
KERNEL_REMOVABLE=()
KERNEL_REMOVABLE_PKGS=()
KERNEL_LATEST_PKGS=()
KERNEL_MISSING_HEADERS=()
KERNEL_ORPHANED_HEADERS=()
KERNEL_ORPHANED_KERNELS=()
KERNEL_ORPHANED_SUPPORT_VERSIONS=()
KERNEL_ORPHANED_SUPPORT_PKGS=()
KERNEL_PARTIAL=()
parse_cached_kernel_inventory
[[ "$KERNEL_RUNNING" == 7.1.9.1-tuxedo-amd64 ]] || fail "parsed running kernel: $KERNEL_RUNNING"
[[ "$KERNEL_LATEST" == 7.1.9.1-tuxedo-amd64 ]] || fail "parsed latest kernel: $KERNEL_LATEST"
[[ "${#KERNEL_INVENTORY_VERSIONS[@]}" -eq 2 ]] || fail "parsed version count: ${#KERNEL_INVENTORY_VERSIONS[@]}"
[[ "${#KERNEL_REMOVABLE[@]}" -eq 1 && "${KERNEL_REMOVABLE[0]}" == 7.1.8.2-tuxedo-amd64 ]] \
    || fail "parsed removable: ${KERNEL_REMOVABLE[*]}"
[[ "${#KERNEL_METAPACKAGES[@]}" -eq 2 ]] || fail "parsed metapackage count: ${#KERNEL_METAPACKAGES[@]}"
rem_pkgs="${KERNEL_REMOVABLE_PKGS[*]}"
for p in linux-image-7.1.8.2-tuxedo-amd64 linux-modules-7.1.8.2-tuxedo-amd64 \
         linux-headers-7.1.8.2-tuxedo-amd64 linux-headers-7.1.8.2-common-tuxedo; do
    grep -q "$p" <<<"$rem_pkgs" || fail "parsed removable plan missing $p"
done
[[ "$(wc -w <<<"$rem_pkgs" | tr -d '[:space:]')" -eq 4 ]] \
    || fail "parsed removable plan must hold exactly 4 packages: $rem_pkgs"
kernel_package_is_removable linux-image-tuxedo \
    && fail 'the parsed image metapackage must never be removable'

# ---- cached-baseline fail-closed --------------------------------------------
# A missing baseline fails closed with "run diagnostics first".
KERNEL_BASELINE_B64=""
set +e
refusal_out="$(parse_cached_kernel_inventory 2>&1)"
refusal_rc=$?
set -e
[[ "$refusal_rc" -ne 0 ]] || fail 'a missing cached baseline must fail closed'
grep -q 'no cached kernel inventory baseline' <<<"$refusal_out" \
    || fail 'the missing-baseline refusal must name the cached baseline'
# A baseline that names no installed kernels fails closed too.
KERNEL_BASELINE_B64="$(printf 'Kernel running: 7.1.9.1-tuxedo-amd64\n' | base64 | tr -d '\n')"
set +e
refusal_out="$(parse_cached_kernel_inventory 2>&1)"
refusal_rc=$?
set -e
[[ "$refusal_rc" -ne 0 ]] || fail 'a baseline without installed kernels must fail closed'
KERNEL_BASELINE_B64=""

# ---- stale-baseline running-kernel re-check fails closed --------------------
# The running kernel changed since the diagnostic, so the baseline's protection
# set is stale: the repair action must fail closed before any mutation.
write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
kernel_inventory_collect
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
# The running kernel is now the OLDER kernel, not the baseline's running kernel.
FAKE_RUNNING=7.1.8.2-tuxedo-amd64
set +e
refusal_out="$(repair_kernel_remove_oldest 2>&1)"
refusal_rc=$?
set -e
[[ "$refusal_rc" -ne 0 ]] || fail 'a changed running kernel must fail closed before removal'
grep -q 'running kernel changed' <<<"$refusal_out" \
    || fail 'the stale-running-kernel refusal must name the changed kernel'
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
KERNEL_BASELINE_B64=""

# ---- validator -------------------------------------------------------------
sim_ok='Reading package lists...
Remv linux-image-7.1.8.2-tuxedo-amd64 [7.1.8.2]
Remv linux-modules-7.1.8.2-tuxedo-amd64 [7.1.8.2]
Remv linux-headers-7.1.8.2-tuxedo-amd64 [7.1.8.2]
Remv linux-headers-7.1.8.2-common-tuxedo [7.1.8.2]
0 upgraded, 0 newly installed, 4 to remove and 0 not upgraded.'
apt_kernel_removal_is_safe "$sim_ok" || fail 'validator must accept the exact removable set'
apt_kernel_removal_is_safe 'Remv linux-image-7.1.9.1-tuxedo-amd64 [7.1.9.1]' \
    && fail 'validator must refuse the latest kernel'
apt_kernel_removal_is_safe 'Remv linux-headers-7.1.9.1-common-tuxedo [7.1.9.1]' \
    && fail 'validator must refuse the protected kernel common headers'
apt_kernel_removal_is_safe 'Remv linux-headers-tuxedo-amd64 [7.1.9.1.2]' \
    && fail 'validator must refuse the headers metapackage'
apt_kernel_removal_is_safe 'Remv linux-image-tuxedo [7.1.9.1.2]' \
    && fail 'validator must refuse the image metapackage'
apt_kernel_removal_is_safe 'Remv systemd [255]' \
    && fail 'validator must refuse protected essentials'
apt_kernel_removal_is_safe 'Remv unrelated-package [1.0]' \
    && fail 'validator must refuse any package outside the removable set'

# ---- removal backend (apply argv correctness) ------------------------------
: > "$sandbox/apply.log"
: > "$sandbox/rollback.log"
run_selected_chroot() {
    printf '%s\n' \
        'Reading package lists...' \
        'Remv linux-image-7.1.8.2-tuxedo-amd64 [7.1.8.2]' \
        'Remv linux-modules-7.1.8.2-tuxedo-amd64 [7.1.8.2]' \
        'Remv linux-headers-7.1.8.2-tuxedo-amd64 [7.1.8.2]' \
        'Remv linux-headers-7.1.8.2-common-tuxedo [7.1.8.2]' \
        '0 upgraded, 0 newly installed, 4 to remove and 0 not upgraded.'
    return 0
}
run_chroot_try_display() {
    local label="$1"; shift
    if [[ "$label" == *reinstall-protected-kernels* ]]; then
        printf 'reinstall\n' >> "$sandbox/rollback.log"
        printf '%s\n' "$@" >> "$sandbox/rollback.log"
        CHROOT_TRY_RC=0
        return 0
    fi
    CHROOT_TRY_RC=0
    CHROOT_TRY_OUTPUT=''
    printf '%s\n' "$@" >> "$sandbox/apply.log"
    return 0
}
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(apt_kernel_cleanup_stage)"
grep -q '^Repair change status kernels: changed$' <<<"$status_line" \
    || fail 'kernel removal must report changed'
if grep -qE '7\.1\.13\.1|linux-headers-tuxedo-amd64|linux-image-tuxedo' "$sandbox/apply.log"; then
    fail "protected package leaked into the removal list: $(tr '\n' ' ' < "$sandbox/apply.log")"
fi
grep -q 'linux-image-7.1.8.2-tuxedo-amd64' "$sandbox/apply.log" \
    || fail 'the superseded kernel image package was not in the removal list'
grep -q 'linux-headers-7.1.8.2-common-tuxedo' "$sandbox/apply.log" \
    || fail 'the grouped common headers were not in the removal list'
[[ ! -s "$sandbox/rollback.log" ]] || fail 'no rollback must run on a clean removal'

# ---- rollback completeness (metapackage + common headers restored) ---------
write_tuxedo_fixture
kernel_inventory_collect
: > "$sandbox/rollback.log"
run_chroot_try_display() {
    local label="$1"; shift
    if [[ "$label" == *reinstall-protected-kernels* ]]; then
        printf 'reinstall\n' >> "$sandbox/rollback.log"
        printf '%s\n' "$@" >> "$sandbox/rollback.log"
        CHROOT_TRY_RC=0
        return 0
    fi
    CHROOT_TRY_RC=0
    CHROOT_TRY_OUTPUT=''
    # Simulate the transaction cascade-removing the running kernel's headers
    # metapackage and its common headers.
    awk 'BEGIN { RS = ""; ORS = "\n\n" }
         $0 !~ /^Package: linux-headers-tuxedo-amd64\n/m \
         && $0 !~ /^Package: linux-headers-7.1.9.1-common-tuxedo\n/m { print }' \
        "$fake_root/var/lib/dpkg/status" > "$fake_root/var/lib/dpkg/status.new"
    mv "$fake_root/var/lib/dpkg/status.new" "$fake_root/var/lib/dpkg/status"
    return 0
}
set +e
rollback_out="$(apt_kernel_cleanup_stage 2>&1)"
rollback_rc=$?
set -e
[[ "$rollback_rc" -ne 0 ]] || fail 'a protected-kernel removal must fail the stage'
grep -q 'protected kernel was removed; restored' <<<"$rollback_out" \
    || fail 'the rollback failure message must name the restored protected kernel'
[[ -s "$sandbox/rollback.log" ]] || fail 'the rollback must reinstall the protected packages'
grep -q 'linux-headers-tuxedo-amd64' "$sandbox/rollback.log" \
    || fail 'the rollback must restore the headers metapackage'
grep -q 'linux-headers-7.1.9.1-common-tuxedo' "$sandbox/rollback.log" \
    || fail 'the rollback must restore the common headers'
grep -q 'linux-image-tuxedo' "$sandbox/rollback.log" \
    || fail 'the rollback must restore the image metapackage'

# ---- zombie cleanup on failed purge ----------------------------------------
write_tuxedo_fixture
# Exercise the alternate module-tree layout and the initrd image too.
mkdir -p "$fake_root/lib/modules/7.1.8.2-tuxedo-amd64"
touch "$fake_root/lib/modules/7.1.8.2-tuxedo-amd64/drv.ko"
apt_kernel_zombie_cleanup 7.1.8.2-tuxedo-amd64 linux-headers-7.1.8.2-common-tuxedo
[[ ! -d "$fake_root/usr/lib/modules/7.1.8.2-tuxedo-amd64" ]] \
    || fail 'zombie cleanup must remove the module tree'
[[ ! -d "$fake_root/lib/modules/7.1.8.2-tuxedo-amd64" ]] \
    || fail 'zombie cleanup must remove the /lib module tree'
[[ ! -d "$fake_root/usr/src/linux-headers-7.1.8.2-tuxedo-amd64" ]] \
    || fail 'zombie cleanup must remove the arch headers tree'
[[ ! -d "$fake_root/usr/src/linux-headers-7.1.8.2-common" ]] \
    || fail 'zombie cleanup must remove the common headers tree'
[[ ! -f "$fake_root/boot/vmlinuz-7.1.8.2-tuxedo-amd64" ]] \
    || fail 'zombie cleanup must remove the vmlinuz image'
[[ ! -f "$fake_root/boot/initrd.img-7.1.8.2-tuxedo-amd64" ]] \
    || fail 'zombie cleanup must remove the initrd image'
[[ ! -f "$fake_root/boot/System.map-7.1.8.2-tuxedo-amd64" ]] \
    || fail 'zombie cleanup must remove System.map'
[[ ! -f "$fake_root/boot/config-7.1.8.2-tuxedo-amd64" ]] \
    || fail 'zombie cleanup must remove the config'
# The protected kernel's files must never be touched.
[[ -d "$fake_root/usr/lib/modules/7.1.9.1-tuxedo-amd64" ]] \
    || fail 'zombie cleanup must not remove a protected kernel'
[[ -f "$fake_root/boot/initrd.img-7.1.9.1-tuxedo-amd64" ]] \
    || fail 'zombie cleanup must not remove a protected kernel initrd'

# ---- missing-headers detection ---------------------------------------------
write_tuxedo_fixture
# Remove the latest kernel's arch headers (dir + package) and its common headers.
rm -rf "$fake_root/usr/src/linux-headers-7.1.9.1-tuxedo-amd64" \
       "$fake_root/usr/src/linux-headers-7.1.9.1-common"
rm -f "$fake_root/var/lib/dpkg/info/linux-headers-7.1.9.1-tuxedo-amd64.list" \
      "$fake_root/var/lib/dpkg/info/linux-headers-7.1.9.1-common-tuxedo.list"
awk 'BEGIN { RS = ""; ORS = "\n\n" }
     $0 !~ /^Package: linux-headers-7.1.9.1-tuxedo-amd64\n/m \
     && $0 !~ /^Package: linux-headers-7.1.9.1-common-tuxedo\n/m { print }' \
    "$fake_root/var/lib/dpkg/status" > "$fake_root/var/lib/dpkg/status.new"
mv "$fake_root/var/lib/dpkg/status.new" "$fake_root/var/lib/dpkg/status"
kernel_inventory_collect
grep -q '7.1.9.1-tuxedo-amd64' <<<"${KERNEL_MISSING_HEADERS[*]}" \
    || fail 'the latest kernel without headers must be surfaced as missing headers'
output="$(diagnostic_kernel || true)"
grep -q '^Kernel missing headers: 7.1.9.1-tuxedo-amd64$' <<<"$output" \
    || fail 'the missing-headers evidence line must name the latest kernel'

# ---- default -amd64/generic naming ----------------------------------------
write_generic_fixture
kernel_inventory_collect
[[ "$KERNEL_LATEST" == 6.5.0-10038-generic ]] || fail "generic latest kernel: $KERNEL_LATEST"
[[ "${#KERNEL_REMOVABLE[@]}" -eq 1 && "${KERNEL_REMOVABLE[0]}" == 6.2.0-1000-generic ]] \
    || fail "generic removable: ${KERNEL_REMOVABLE[*]}"
rem_pkgs="${KERNEL_REMOVABLE_PKGS[*]}"
grep -q 'linux-headers-6.2.0-1000' <<<"$rem_pkgs" \
    || fail 'generic common headers must group with the old kernel'
kernel_package_is_removable linux-headers-6.5.0-10038 \
    && fail 'the protected generic common headers must never be removable'
grep -q 'linux-headers-generic' <<<"$(apt_managed_kernel_metapackage)" \
    || fail 'the generic headers metapackage must be detected'

# ---- single kernel -> still available (menu reports verified, no prompt) ----
write_generic_fixture
# Drop the old kernel entirely (module tree AND headers packages) so only
# running/latest remains with no orphaned headers left behind.
FAKE_RUNNING=6.5.0-10038-generic
rm -rf "$fake_root/usr/lib/modules/6.2.0-1000-generic" \
       "$fake_root/usr/src/linux-headers-6.2.0-1000-generic" \
       "$fake_root/usr/src/linux-headers-6.2.0-1000"
awk 'BEGIN { RS = ""; ORS = "\n\n" }
     $0 !~ /^Package: linux-headers-6.2.0-1000-generic\n/m \
     && $0 !~ /^Package: linux-headers-6.2.0-1000\n/m { print }' \
    "$fake_root/var/lib/dpkg/status" > "$fake_root/var/lib/dpkg/status.new"
mv "$fake_root/var/lib/dpkg/status.new" "$fake_root/var/lib/dpkg/status"
kernel_inventory_collect
# The cached `kernel` diagnostic section is the repair baseline.
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
# The gate is no longer "removable kernels exist": the stage now covers install
# headers/reinstall/repo-latest/orphan cleanup too, so it must stay available
# with a single kernel + a detected package backend.
if ! reason="$(kernels_unavailable_reason)"; then
    fail "kernels must be available with a single kernel + package backend; got $reason"
fi
# The interactive menu reports the read-only verified result without prompting
# when the baseline has nothing actionable (no removable/orphaned/missing/
# partial kernel and no newer repository kernel): the verify fallback action is
# gone, replaced by a consistent-state terminal.  Prompts are recorded to files
# because the menu runs inside a command substitution (a subshell), so a
# counter variable would not survive back here.
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
BOOT_REPAIR_SESSION_PROTOCOL=1
BOOT_REPAIR_SESSION_REQUEST=7
PROMPT_FILE="$sandbox/prompt.txt"
PROMPT_COUNT_FILE="$sandbox/prompt-count.txt"
: > "$PROMPT_FILE"
printf '0' > "$PROMPT_COUNT_FILE"
helper_menu_prompt() {
    local n=0
    n="$(cat "$PROMPT_COUNT_FILE" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$PROMPT_COUNT_FILE"
    printf '%s\n' "$1" >> "$PROMPT_FILE"
    HELPER_MENU_ANSWER=""
    return 1   # a consistent inventory must never prompt; abort the loop if it does
}
status_line="$(kernel_interactive_menu 2>/dev/null || true)"
[[ "$(cat "$PROMPT_COUNT_FILE")" -eq 0 ]] \
    || fail 'a consistent kernel inventory must report verified without prompting'
grep -q '^Repair change status kernels:verify: unchanged|reason:kernel-verified$' <<<"$status_line" \
    || fail 'the consistent inventory must report kernel-verified'
KERNEL_BASELINE_B64=""

# ---- interactive menu: baseline first display, fresh re-collect after action ----
# The first menu display consumes the cached `kernel` diagnostic baseline; after
# the chosen sub-action mutates (remove-oldest collapses the fixture), the next
# iteration re-collects fresh and reports the consistent state (kernel-verified)
# WITHOUT re-offering the already-removed kernel.
write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
kernel_inventory_collect
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
BOOT_REPAIR_SESSION_PROTOCOL=1
BOOT_REPAIR_SESSION_REQUEST=7
LOOP_PROMPT_FILE="$sandbox/loop-prompt.txt"
LOOP_PROMPT_COUNT_FILE="$sandbox/loop-prompt-count.txt"
: > "$LOOP_PROMPT_FILE"
printf '0' > "$LOOP_PROMPT_COUNT_FILE"
MENU_ANSWER="1"
repair_kernel_remove_oldest() {
    # Simulate the removal: collapse the inventory to a single consistent
    # kernel (remove the old kernel's module tree, headers, boot files and dpkg
    # records) so the next fresh collection finds nothing actionable.
    rm -rf "$fake_root/usr/lib/modules/7.1.8.2-tuxedo-amd64" \
           "$fake_root/usr/src/linux-headers-7.1.8.2-tuxedo-amd64" \
           "$fake_root/usr/src/linux-headers-7.1.8.2-common"
    rm -f "$fake_root/boot/vmlinuz-7.1.8.2-tuxedo-amd64" \
          "$fake_root/boot/initrd.img-7.1.8.2-tuxedo-amd64" \
          "$fake_root/boot/System.map-7.1.8.2-tuxedo-amd64" \
          "$fake_root/boot/config-7.1.8.2-tuxedo-amd64"
    rm -f "$fake_root/var/lib/dpkg/info/linux-image-7.1.8.2-tuxedo-amd64.list" \
          "$fake_root/var/lib/dpkg/info/linux-modules-7.1.8.2-tuxedo-amd64.list" \
          "$fake_root/var/lib/dpkg/info/linux-headers-7.1.8.2-tuxedo-amd64.list" \
          "$fake_root/var/lib/dpkg/info/linux-headers-7.1.8.2-common-tuxedo.list"
    awk 'BEGIN { RS = ""; ORS = "\n\n" }
         $0 !~ /^Package: linux-image-7\.1\.8\.2-tuxedo-amd64\n/m \
         && $0 !~ /^Package: linux-modules-7\.1\.8\.2-tuxedo-amd64\n/m \
         && $0 !~ /^Package: linux-headers-7\.1\.8\.2-tuxedo-amd64\n/m \
         && $0 !~ /^Package: linux-headers-7\.1\.8\.2-common-tuxedo\n/m { print }' \
        "$fake_root/var/lib/dpkg/status" > "$fake_root/var/lib/dpkg/status.new"
    mv "$fake_root/var/lib/dpkg/status.new" "$fake_root/var/lib/dpkg/status"
}
helper_menu_prompt() {
    local n=0
    n="$(cat "$LOOP_PROMPT_COUNT_FILE" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$LOOP_PROMPT_COUNT_FILE"
    printf '%s\n' "$1" >> "$LOOP_PROMPT_FILE"
    HELPER_MENU_ANSWER="$MENU_ANSWER"
    return 0
}
status_line="$(kernel_interactive_menu 2>/dev/null || true)"
[[ "$(cat "$LOOP_PROMPT_COUNT_FILE")" -eq 1 ]] \
    || fail 'the removable-kernel menu must prompt once for remove-oldest, then report verified'
grep -q 'Remove oldest/superseded kernel(s)' "$LOOP_PROMPT_FILE" \
    || fail 'the removable-kernel menu must offer remove-oldest'
grep -q '^Found:$' "$LOOP_PROMPT_FILE" \
    || fail 'the kernel menu must begin with a Found: evidence summary'
grep -q 'removable kernel: 7.1.8.2-tuxedo-amd64' "$LOOP_PROMPT_FILE" \
    || fail 'the kernel menu summary must name the removable kernel from the baseline'
grep -q 'linux-image-7.1.8.2-tuxedo-amd64' "$LOOP_PROMPT_FILE" \
    || fail 'the kernel menu summary must name the removable kernel packages'
grep -q '0) Abort' "$LOOP_PROMPT_FILE" \
    || fail 'the kernel menu must use the 0) Abort abort line'
if grep -q 'Choice:' "$LOOP_PROMPT_FILE"; then
    fail 'the kernel menu must not append a Choice: prompt suffix'
fi
if grep -q 'no-removable-kernels' <<<"$status_line"; then
    fail 'the removable-kernel menu must not return early with no-removable-kernels'
fi
# The post-mutation display must NOT re-offer the already-removed kernel and
# must terminate by reporting the verified consistent state.
grep -q '^Repair change status kernels:verify: unchanged|reason:kernel-verified$' <<<"$status_line" \
    || fail 'the kernel menu must report kernel-verified after the removal'
BOOT_REPAIR_SESSION_PROTOCOL=0
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
KERNEL_BASELINE_B64=""

# ---- legacy 2.6.x/3.x version ordering and grouping ------------------------
write_legacy_fixture
FAKE_RUNNING=2.6.32-5-amd64
kernel_inventory_collect
[[ "${KERNEL_INVENTORY_VERSIONS[*]}" == '2.6.26-2-amd64 2.6.32-5-amd64 3.5.0-27-generic' ]] \
    || fail "legacy versions must sort by version, got: ${KERNEL_INVENTORY_VERSIONS[*]}"
[[ "$KERNEL_LATEST" == 3.5.0-27-generic ]] || fail "legacy latest kernel: $KERNEL_LATEST"
[[ "${#KERNEL_REMOVABLE[@]}" -eq 1 && "${KERNEL_REMOVABLE[0]}" == 2.6.26-2-amd64 ]] \
    || fail "legacy removable: ${KERNEL_REMOVABLE[*]}"
FAKE_RUNNING=7.1.9.1-tuxedo-amd64

# ---- repository latest evidence (direct /var/lib/apt/lists read) -----------
write_tuxedo_fixture
mkdir -p "$fake_root/var/lib/apt/lists"
cat > "$fake_root/var/lib/apt/lists/tuxedo_Packages" <<'PACKAGES'
Package: linux-image-tuxedo
Version: 7.1.9.3
Depends: linux-image-7.1.9.3-tuxedo-amd64 (= 7.1.9.3-1)

PACKAGES
kernel_inventory_collect
repo_latest="$(apt_kernel_repo_latest)"
[[ "$repo_latest" == 7.1.9.3-tuxedo-amd64 ]] || fail "repo latest kernel: $repo_latest"
output="$(diagnostic_kernel || true)"
grep -q '^Kernel repository latest: 7.1.9.3-tuxedo-amd64$' <<<"$output" \
    || fail 'repo-latest evidence line missing'
grep -q '^Kernel state: running=7.1.9.1-tuxedo-amd64 installed-latest=7.1.9.1-tuxedo-amd64 repo-latest=7.1.9.3-tuxedo-amd64$' <<<"$output" \
    || fail 'kernel state relationship line wrong'
# Offline/no-candidate falls back to `unavailable` and never gates the stage.
rm -rf "$fake_root/var/lib/apt/lists"
kernel_inventory_collect
[[ -z "$(apt_kernel_repo_latest || true)" ]] || fail 'offline repo probe must be empty'
output="$(diagnostic_kernel || true)"
grep -q '^Kernel repository latest: unavailable$' <<<"$output" \
    || fail 'offline repo-latest must be unavailable'

# ---- orphaned headers detection --------------------------------------------
write_tuxedo_fixture
# Remove the old kernel's module tree but keep its headers -> orphaned.
rm -rf "$fake_root/usr/lib/modules/7.1.8.2-tuxedo-amd64"
kernel_inventory_collect
grep -q '7.1.8.2-tuxedo-amd64' <<<"${KERNEL_ORPHANED_HEADERS[*]}" \
    || fail 'the old kernel headers without a module tree must be orphaned'
grep -q '7.1.9.1-tuxedo-amd64' <<<"${KERNEL_ORPHANED_HEADERS[*]}" \
    && fail 'the latest kernel (with a module tree) must never be orphaned'
run_selected_chroot() { return 1; }
output="$(diagnostic_kernel || true)"
grep -q '^Kernel orphaned headers: 7.1.8.2-tuxedo-amd64$' <<<"$output" \
    || fail 'orphaned-headers evidence line must name the orphaned version'

# ---- remove-newest: broken newest removable, running always protected -------
write_tuxedo_fixture
FAKE_RUNNING=7.1.8.2-tuxedo-amd64   # running is the OLDER kernel
kernel_inventory_collect
REPAIR_REMOVE_NEWEST=false
kernel_is_removal_candidate 7.1.9.1-tuxedo-amd64 \
    && fail 'the newest kernel must not be a candidate without --remove-newest'
kernel_is_removal_candidate 7.1.8.2-tuxedo-amd64 \
    && fail 'the running kernel must never be a removal candidate'
REPAIR_REMOVE_NEWEST=true
kernel_is_removal_candidate 7.1.9.1-tuxedo-amd64 \
    || fail 'the newest kernel must be removable with --remove-newest'
kernel_is_removal_candidate 7.1.8.2-tuxedo-amd64 \
    && fail 'the running kernel must remain protected even with --remove-newest'
# The exact-membership validator mirrors the candidate set.
: > "$sandbox/apply.log"
run_selected_chroot() {
    printf '%s\n' \
        'Reading package lists...' \
        'Remv linux-image-7.1.9.1-tuxedo-amd64 [7.1.9.1]' \
        'Remv linux-modules-7.1.9.1-tuxedo-amd64 [7.1.9.1]' \
        'Remv linux-headers-7.1.9.1-tuxedo-amd64 [7.1.9.1]' \
        'Remv linux-headers-7.1.9.1-common-tuxedo [7.1.9.1]' \
        '0 upgraded, 0 newly installed, 4 to remove and 0 not upgraded.'
    return 0
}
run_chroot_try_display() {
    local label="$1"; shift
    if [[ "$label" == *reinstall-protected-kernels* ]]; then
        CHROOT_TRY_RC=0
        return 0
    fi
    CHROOT_TRY_RC=0
    CHROOT_TRY_OUTPUT=''
    printf '%s\n' "$@" >> "$sandbox/apply.log"
    return 0
}
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(apt_kernel_cleanup_stage)"
grep -q '^Repair change status kernels: changed$' <<<"$status_line" \
    || fail 'remove-newest must report changed'
grep -q 'linux-image-7.1.9.1-tuxedo-amd64' "$sandbox/apply.log" \
    || fail 'remove-newest must remove the newest image package'
grep -q 'linux-image-7.1.8.2-tuxedo-amd64' "$sandbox/apply.log" \
    && fail 'the running kernel package must never be removed'
REPAIR_REMOVE_NEWEST=false
FAKE_RUNNING=7.1.9.1-tuxedo-amd64

# ---- remove-newest refusal when newest == running --------------------------
write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64   # newest IS the running kernel
REPAIR_REMOVE_NEWEST=true
kernel_inventory_collect
run_package_stage() { :; }
adaptive_initramfs_repair() { :; }
adaptive_extlinux_repair() { :; }
adaptive_grub_stage() { :; }
set +e
refusal_out="$(repair_kernel_cleanup 2>&1)"
refusal_rc=$?
set -e
[[ "$refusal_rc" -ne 0 ]] || fail 'removing the running kernel via --remove-newest must be refused'
grep -q 'cannot be removed. Boot another kernel first' <<<"$refusal_out" \
    || fail 'the running-kernel refusal must tell the user to boot another kernel'
REPAIR_REMOVE_NEWEST=false
FAKE_RUNNING=7.1.9.1-tuxedo-amd64

# ---- orchestration reuses initramfs + GRUB after a removal -----------------
# A real removal (KERNEL_CLEANUP_APPLIED=true, set by the cleanup backend after
# it applied a transaction) triggers the initramfs + bootloader regen.
write_tuxedo_fixture
kernel_inventory_collect
INITRAMFS_RAN=0
GRUB_RAN=0
run_package_stage() { KERNEL_CLEANUP_APPLIED=true; }
adaptive_initramfs_repair() { INITRAMFS_RAN=1; }
adaptive_extlinux_repair() { :; }
adaptive_grub_stage() { GRUB_RAN=1; }
repair_kernel_cleanup
[[ "$INITRAMFS_RAN" -eq 1 ]] || fail 'kernel cleanup must rebuild initramfs'
[[ "$GRUB_RAN" -eq 1 ]] || fail 'kernel cleanup must regenerate GRUB'

# ---- no-op removal does NOT trigger initramfs/bootloader regen --------------
# The cleanup backend proves no changes (KERNEL_CLEANUP_APPLIED stays false), so
# repair_kernel_cleanup / repair_kernel_remove_oldest must skip the regen.
write_tuxedo_fixture
kernel_inventory_collect
KERNEL_CLEANUP_APPLIED=false
NOOP_INITRAMFS_RAN=0
NOOP_GRUB_RAN=0
run_package_stage() { KERNEL_CLEANUP_APPLIED=false; }
adaptive_initramfs_repair() { NOOP_INITRAMFS_RAN=1; }
adaptive_extlinux_repair() { :; }
adaptive_grub_stage() { NOOP_GRUB_RAN=1; }
repair_kernel_cleanup
[[ "$NOOP_INITRAMFS_RAN" -eq 0 ]] || fail 'a no-op kernel cleanup must not rebuild initramfs'
[[ "$NOOP_GRUB_RAN" -eq 0 ]] || fail 'a no-op kernel cleanup must not regenerate GRUB'
# Same gate for the interactive remove-oldest sub-action.
write_tuxedo_fixture
kernel_inventory_collect
KERNEL_CLEANUP_APPLIED=false
NOOP_INITRAMFS_RAN=0
NOOP_GRUB_RAN=0
run_package_stage() { KERNEL_CLEANUP_APPLIED=false; }
adaptive_initramfs_repair() { NOOP_INITRAMFS_RAN=1; }
adaptive_extlinux_repair() { :; }
adaptive_grub_stage() { NOOP_GRUB_RAN=1; }
repair_kernel_remove_oldest
[[ "$NOOP_INITRAMFS_RAN" -eq 0 ]] || fail 'a no-op remove-oldest must not rebuild initramfs'
[[ "$NOOP_GRUB_RAN" -eq 0 ]] || fail 'a no-op remove-oldest must not regenerate GRUB'

# ---- orphaned kernel components (zombie) detection and removal --------------
write_tuxedo_fixture
# Leave a module tree (and a stray boot image) no package owns -> zombie.
mkdir -p "$fake_root/usr/lib/modules/7.0.0-0-tuxedo-amd64"
touch "$fake_root/boot/vmlinuz-7.0.0-0-tuxedo-amd64"
kernel_inventory_collect
grep -q '7.0.0-0-tuxedo-amd64' <<<"${KERNEL_ORPHANED_KERNELS[*]}" \
    || fail 'an unowned module tree must be detected as an orphaned kernel'
# The fully-installed kernels are never orphaned.
grep -q '7.1.9.1-tuxedo-amd64' <<<"${KERNEL_ORPHANED_KERNELS[*]}" \
    && fail 'a fully-installed kernel must never be orphaned'
output="$(diagnostic_kernel || true)"
grep -q '^Kernel orphaned kernel: 7.0.0-0-tuxedo-amd64$' <<<"$output" \
    || fail 'the orphaned-kernel evidence line must name the zombie version'
# The gate stays available while a zombie is actionable.
if ! reason="$(kernels_unavailable_reason)"; then
    fail "kernels should be available with an orphaned kernel; got $reason"
fi
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_remove_orphaned_kernels)"
grep -q '^Repair change status kernels:remove-orphaned-kernels: changed$' <<<"$status_line" \
    || fail 'orphaned-kernel removal must report changed'
[[ ! -d "$fake_root/usr/lib/modules/7.0.0-0-tuxedo-amd64" ]] \
    || fail 'orphaned-kernel removal must remove the zombie module tree'
[[ ! -f "$fake_root/boot/vmlinuz-7.0.0-0-tuxedo-amd64" ]] \
    || fail 'orphaned-kernel removal must remove the zombie boot image'
[[ -d "$fake_root/usr/lib/modules/7.1.9.1-tuxedo-amd64" ]] \
    || fail 'orphaned-kernel removal must not touch a protected kernel'

# ---- incomplete kernel detection (reinstall) --------------------------------
write_tuxedo_fixture
# The image package stays installed but its vmlinuz is missing -> incomplete.
rm -f "$fake_root/boot/vmlinuz-7.1.8.2-tuxedo-amd64"
kernel_inventory_collect
grep -q '7.1.8.2-tuxedo-amd64' <<<"${KERNEL_PARTIAL[*]}" \
    || fail 'an installed image without its vmlinuz must be flagged incomplete'
grep -q '7.1.9.1-tuxedo-amd64' <<<"${KERNEL_PARTIAL[*]}" \
    && fail 'a complete kernel must never be flagged incomplete'
output="$(diagnostic_kernel || true)"
grep -q '^Kernel incomplete: 7.1.8.2-tuxedo-amd64$' <<<"$output" \
    || fail 'the incomplete evidence line must name the partial kernel'
if ! reason="$(kernels_unavailable_reason)"; then
    fail "kernels should be available with an incomplete kernel; got $reason"
fi
# The reinstall stage is a no-op (unchanged) when nothing is incomplete.
write_tuxedo_fixture
kernel_inventory_collect
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_reinstall)"
grep -q '^Repair change status kernels:reinstall: unchanged|reason:no-partial-kernels$' <<<"$status_line" \
    || fail 'the reinstall stage must report unchanged when nothing is incomplete'

# ---- repository-latest install (unchanged when no newer kernel) -------------
write_tuxedo_fixture
kernel_inventory_collect
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_install_repo_latest)"
grep -q '^Repair change status kernels:install-repo-latest: unchanged|reason:no-repo-latest-kernel$' <<<"$status_line" \
    || fail 'repo-latest install must report unchanged offline/without a newer kernel'

# ---- reinstall follow-on: initramfs + bootloader config --------------------
# repair_kernel_reinstall reinstalls the incomplete kernel's packages and must
# then rebuild the initramfs and the bootloader config so the repaired kernel
# boots (mirror repair_kernel_remove_newest's tail).  Calls are recorded to
# files because the sub-actions run inside command substitutions (a subshell).
write_tuxedo_fixture
rm -f "$fake_root/boot/vmlinuz-7.1.8.2-tuxedo-amd64"
kernel_inventory_collect
grep -q '7.1.8.2-tuxedo-amd64' <<<"${KERNEL_PARTIAL[*]}" \
    || fail 'fixture must make the old kernel incomplete for the reinstall follow-on test'
FOLLOWON_LOG="$sandbox/followon.log"
: > "$FOLLOWON_LOG"
safe_apt_install_packages() { printf 'reinstall\n' >> "$FOLLOWON_LOG"; }
adaptive_initramfs_repair() { printf 'initramfs\n' >> "$FOLLOWON_LOG"; }
adaptive_grub_stage() { printf 'grub\n' >> "$FOLLOWON_LOG"; }
adaptive_extlinux_repair() { printf 'extlinux\n' >> "$FOLLOWON_LOG"; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_reinstall)"
grep -q '^Repair change status kernels:reinstall: changed$' <<<"$status_line" \
    || fail 'reinstall must report changed after reinstalling the partial kernel'
[[ "$(cat "$FOLLOWON_LOG")" == $'reinstall\ninitramfs\ngrub' ]] \
    || fail "reinstall follow-on order wrong: $(tr '\n' ',' < "$FOLLOWON_LOG")"

# ---- orphaned-kernel removal refreshes grub (no initramfs) -----------------
# repair_kernel_remove_orphaned_kernels removes the zombie module tree + boot
# artifacts; the kernel is gone, so no initramfs rebuild is needed, but the
# bootloader config must be regenerated so the menu drops the removed kernel.
write_tuxedo_fixture
mkdir -p "$fake_root/usr/lib/modules/7.0.0-0-tuxedo-amd64"
touch "$fake_root/boot/vmlinuz-7.0.0-0-tuxedo-amd64"
kernel_inventory_collect
grep -q '7.0.0-0-tuxedo-amd64' <<<"${KERNEL_ORPHANED_KERNELS[*]}" \
    || fail 'fixture must surface the zombie kernel for the orphaned follow-on test'
: > "$FOLLOWON_LOG"
adaptive_initramfs_repair() { printf 'initramfs\n' >> "$FOLLOWON_LOG"; }
adaptive_grub_stage() { printf 'grub\n' >> "$FOLLOWON_LOG"; }
adaptive_extlinux_repair() { printf 'extlinux\n' >> "$FOLLOWON_LOG"; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_remove_orphaned_kernels)"
grep -q '^Repair change status kernels:remove-orphaned-kernels: changed$' <<<"$status_line" \
    || fail 'orphaned-kernel removal must report changed'
[[ "$(cat "$FOLLOWON_LOG")" == $'grub' ]] \
    || fail "orphaned-kernel removal must only refresh grub (no initramfs): $(tr '\n' ',' < "$FOLLOWON_LOG")"

# ---- orphaned build/support detection ---------------------------------------
# The 7.1.10.2 kernel is gone (no module tree, no image/modules package) while
# linux-kbuild-7.1.10.2 survived -> orphaned; linux-kbuild-7.1.13.1 belongs to
# the still-installed 7.1.13.1 kernel -> protected.
FAKE_RUNNING=7.1.13.1-tuxedo-amd64
write_kbuild_fixture
kernel_inventory_collect
[[ "$KERNEL_RUNNING" == 7.1.13.1-tuxedo-amd64 ]] || fail "running kernel: $KERNEL_RUNNING"
[[ "$KERNEL_LATEST" == 7.1.13.1-tuxedo-amd64 ]] || fail "latest kernel: $KERNEL_LATEST"
grep -q '7.1.10.2' <<<"${KERNEL_ORPHANED_SUPPORT_VERSIONS[*]}" \
    || fail 'linux-kbuild-7.1.10.2 (kernel gone) must be detected as orphaned build support'
grep -q '7.1.13.1' <<<"${KERNEL_ORPHANED_SUPPORT_VERSIONS[*]}" \
    && fail 'linux-kbuild-7.1.13.1 (kernel present) must never be orphaned'
[[ "${#KERNEL_ORPHANED_SUPPORT_VERSIONS[@]}" -eq 1 ]] \
    || fail "orphaned support count must be exactly 1: ${KERNEL_ORPHANED_SUPPORT_VERSIONS[*]}"
grep -q 'linux-kbuild-7.1.10.2' <<<"${KERNEL_ORPHANED_SUPPORT_PKGS[*]}" \
    || fail 'the orphaned support package list must carry linux-kbuild-7.1.10.2'
# The support package owns its own files, so it is not confused with an
# orphaned kernel component (zombie) nor a partial kernel.
grep -q '7.1.10.2' <<<"${KERNEL_ORPHANED_KERNELS[*]}" \
    && fail 'a kbuild package must never be reported as an orphaned kernel component'
output="$(diagnostic_kernel || true)"
grep -q '^Kernel orphaned build support: 7.1.10.2 pkg=linux-kbuild-7.1.10.2$' <<<"$output" \
    || fail 'the orphaned build-support evidence line must name the version and package'
if ! reason="$(kernels_unavailable_reason)"; then
    fail "kernels should be available with an orphaned build-support package; got $reason"
fi

# ---- orphaned build/support removal (simulation-first, guarded) --------------
write_kbuild_fixture
kernel_inventory_collect
: > "$sandbox/apply.log"
: > "$sandbox/rollback.log"
run_selected_chroot() {
    printf '%s\n' \
        'Reading package lists...' \
        'Remv linux-kbuild-7.1.10.2 [7.1.10.2]' \
        '0 upgraded, 0 newly installed, 1 to remove and 0 not upgraded.'
    return 0
}
run_chroot_try_display() {
    local label="$1"; shift
    if [[ "$label" == *reinstall-protected-kernels* ]]; then
        printf 'reinstall\n' >> "$sandbox/rollback.log"
        printf '%s\n' "$@" >> "$sandbox/rollback.log"
        CHROOT_TRY_RC=0
        return 0
    fi
    CHROOT_TRY_RC=0
    CHROOT_TRY_OUTPUT=''
    printf '%s\n' "$@" >> "$sandbox/apply.log"
    return 0
}
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_remove_orphaned_support)"
grep -q '^Repair change status kernels:remove-orphaned-support: changed$' <<<"$status_line" \
    || fail 'orphaned build-support removal must report changed'
grep -q 'linux-kbuild-7.1.10.2' "$sandbox/apply.log" \
    || fail 'the orphaned kbuild package was not in the removal list'
grep -q 'linux-kbuild-7.1.13.1' "$sandbox/apply.log" \
    && fail 'the protected kbuild package must never be removed'
[[ ! -s "$sandbox/rollback.log" ]] || fail 'no rollback must run on a clean orphaned build-support removal'

# The exact-membership validator refuses the protected kbuild and any unrelated
# package, and accepts only the orphaned package.
apt_orphaned_headers_removal_is_safe 'Remv linux-kbuild-7.1.13.1 [7.1.13.1]' 'linux-kbuild-7.1.10.2' \
    && fail 'the validator must refuse the protected kbuild package'
apt_orphaned_headers_removal_is_safe 'Remv systemd [255]' 'linux-kbuild-7.1.10.2' \
    && fail 'the validator must refuse protected essentials'
apt_orphaned_headers_removal_is_safe 'Remv unrelated-package [1.0]' 'linux-kbuild-7.1.10.2' \
    && fail 'the validator must refuse any package outside the orphaned support set'
apt_orphaned_headers_removal_is_safe 'Remv linux-kbuild-7.1.10.2 [7.1.10.2]' 'linux-kbuild-7.1.10.2' \
    || fail 'the validator must accept the exact orphaned support package'

# ---- orphaned build/support removal failure rolls back / fails closed --------
write_kbuild_fixture
kernel_inventory_collect
run_selected_chroot() { return 0; }
run_chroot_try_display() {
    local label="$1"; shift
    if [[ "$label" == *reinstall-protected-kernels* ]]; then
        CHROOT_TRY_RC=0
        return 0
    fi
    CHROOT_TRY_RC=42
    CHROOT_TRY_OUTPUT=''
    return 0
}
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
set +e
refusal_out="$(repair_kernel_remove_orphaned_support 2>&1)"
refusal_rc=$?
set -e
[[ "$refusal_rc" -ne 0 ]] || fail 'a failed orphaned build-support removal must fail the stage'
grep -q 'Orphaned kernel build-support removal failed with exit code 42' <<<"$refusal_out" \
    || fail 'the orphaned build-support failure must name the exit code'

FAKE_RUNNING=7.1.9.1-tuxedo-amd64

# ---- repair action with a baseline never re-collects ------------------------
# With a cached baseline supplied, a repair sub-action must parse it (not call
# kernel_inventory_collect); the fresh running-kernel re-check runs instead.
write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
kernel_inventory_collect
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
COLLECT_CALLS=0
kernel_inventory_collect() { COLLECT_CALLS=$((COLLECT_CALLS + 1)); }
# The action is a no-op (nothing incomplete), but it must still re-verify the
# running kernel fresh via the baseline without re-collecting.
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
set +e
status_line="$(repair_kernel_reinstall 2>&1)"
set -e
[[ "$COLLECT_CALLS" -eq 0 ]] \
    || fail 'a baseline-driven repair action must not re-run kernel_inventory_collect'
grep -q '^Repair change status kernels:reinstall: unchanged|reason:no-partial-kernels$' <<<"$status_line" \
    || fail 'the baseline-driven reinstall action must report unchanged via the cached baseline'
KERNEL_BASELINE_B64=""

# ---- interactive menu never re-collects before a mutation -------------------
# The first display and any re-display after an invalid answer re-parse the
# cached baseline (no fresh collect); only a dispatched sub-action clears the
# baseline so the FOLLOWING display re-collects fresh.
write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
kernel_inventory_collect
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
COLLECT_CALLS=0
kernel_inventory_collect() { COLLECT_CALLS=$((COLLECT_CALLS + 1)); }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
BOOT_REPAIR_SESSION_PROTOCOL=1
BOOT_REPAIR_SESSION_REQUEST=7
NOCOLLECT_PROMPT_FILE="$sandbox/nocollect-prompt.txt"
NOCOLLECT_PROMPT_COUNT_FILE="$sandbox/nocollect-prompt-count.txt"
: > "$NOCOLLECT_PROMPT_FILE"
printf '0' > "$NOCOLLECT_PROMPT_COUNT_FILE"
NOCOLLECT_ANSWERS=("99" "0")
helper_menu_prompt() {
    local n=0
    n="$(cat "$NOCOLLECT_PROMPT_COUNT_FILE" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$NOCOLLECT_PROMPT_COUNT_FILE"
    printf '%s\n' "$1" >> "$NOCOLLECT_PROMPT_FILE"
    if (( n <= ${#NOCOLLECT_ANSWERS[@]} )); then
        HELPER_MENU_ANSWER="${NOCOLLECT_ANSWERS[$((n - 1))]}"
    else
        HELPER_MENU_ANSWER="0"
    fi
    return 0
}
kernel_interactive_menu >/dev/null 2>&1 || true
[[ "$COLLECT_CALLS" -eq 0 ]] \
    || fail 'the menu must not re-collect before a mutation (invalid answer + abort)'
[[ "$(cat "$NOCOLLECT_PROMPT_COUNT_FILE")" -eq 2 ]] \
    || fail 'the menu must re-prompt once after an invalid answer, then abort'
BOOT_REPAIR_SESSION_PROTOCOL=0
KERNEL_BASELINE_B64=""

echo 'kernels-cleanup-contract: PASS'
