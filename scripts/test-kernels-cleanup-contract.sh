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
# shellcheck disable=SC2218,SC2115
# SC2218: the harness defines stub functions at runtime before the calls
# (subshell-scoped) — intentional contract style.  SC2115: the fixture-path
# expansions (rm/mount under "$fake_root") always name a set variable.
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="${HELPER:-$ROOT_DIR/scripts/boot-repair-helper.sh}"

[[ -x "$HELPER" ]] || { echo 'FAIL: helper is not executable' >&2; exit 1; }
bash -n "$HELPER"

fail() { printf 'FAIL: %s\n' "$*" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Static wiring contracts
# ---------------------------------------------------------------------------
for fn in apt_installed_kernel_packages apt_installed_kernel_support_packages apt_config_files_kernel_image_packages \
    dpkg_path_owner apt_package_depends \
    apt_package_owned_dirs apt_managed_kernel_metapackage apt_managed_kernel_versions \
    apt_kernel_version_from_package apt_kernel_support_version_from_package \
    kernel_support_version_is_orphaned kernel_version_total_size kernel_inventory_collect \
    parse_cached_kernel_inventory repair_kernel_prepare_inventory \
    kernel_is_removable kernel_package_is_removable kernel_is_removal_candidate \
    kernel_package_is_removal_candidate kernel_newest_is_removable apt_kernel_repo_latest \
    apt_image_installability apt_image_installable diagnostic_kernel \
    diagnostic_kernel_inventory \
    kernels_unavailable_reason apt_kernel_removal_is_safe apt_kernel_protected_set_present \
    apt_kernel_cleanup_stage apt_kernel_zombie_cleanup apt_install_missing_latest_headers \
    apt_cleanup_orphaned_headers apt_orphaned_headers_removal_is_safe \
    apt_cleanup_orphaned_build_support \
    rpm_kernel_removal_is_safe rpm_kernel_protected_set_present rpm_kernel_cleanup_stage \
    rpm_install_missing_kernel_headers target_rpm_nevra_installed kernel_cleanup_stage \
    rpm_installed_kernel_devel_nevras rpm_cleanup_orphaned_kernel_devel \
    alpine_apk_kernel_removal_is_safe apk_kernel_protected_set_present apk_kernel_cleanup_stage \
    alpine_apk_kernel_zombie_cleanup alpine_kernel_flavor alpine_apk_path_owner \
    apk_install_missing_kernel_headers apk_repair_kernel_reinstall \
    pacman_path_owner pacman_kernel_removal_is_safe pacman_kernel_protected_set_present \
    pacman_kernel_cleanup_stage pacman_kernel_repo_latest \
    pacman_package_installed pacman_kernel_packages pacman_package_module_dirs \
    pacman_partial_kernels pacman_kernel_headers_package pacman_package_availability \
    pacman_install_missing_kernel_headers \
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
grep -q 'kernel-cleanup:rpm) rpm_kernel_cleanup_stage' "$HELPER" \
    || fail 'run_package_stage has no kernel-cleanup rpm case'
grep -q 'kernel-cleanup:apk) apk_kernel_cleanup_stage' "$HELPER" \
    || fail 'run_package_stage has no kernel-cleanup apk case'
grep -q 'kernel-cleanup:pacman) pacman_kernel_cleanup_stage' "$HELPER" \
    || fail 'run_package_stage has no kernel-cleanup pacman case'
grep -q 'kernel-cleanup:apt/dpkg|kernel-cleanup:rpm|kernel-cleanup:apk|kernel-cleanup:pacman) printf' "$HELPER" \
    || fail 'package_stage_backends does not expose the apk kernel-cleanup backend'
grep -q '^kernel_cleanup_stage()' "$HELPER" || fail 'helper is missing kernel_cleanup_stage()'
grep -q 'apk) apk_kernel_cleanup_stage' "$HELPER" \
    || fail 'kernel_cleanup_stage has no apk dispatch case'
grep -q 'pacman) pacman_kernel_cleanup_stage' "$HELPER" \
    || fail 'kernel_cleanup_stage has no pacman dispatch case'
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
# The orphaned-headers removal dispatches to the rpm kernel-devel cleanup on rpm.
grep -q 'rpm) rpm_cleanup_orphaned_kernel_devel' "$HELPER" || fail 'remove-orphaned-headers has no rpm dispatch'
grep -q 'apt_installed_kernel_packages; apt_config_files_kernel_image_packages' "$HELPER" || fail 'partial probe does not enumerate removed-but-not-purged image metas'

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

# TUXEDO split-kernel fixture: the kernel is split across a versioned image
# METAPACKAGE (linux-image-<v>, which depends on linux-binary-<v> +
# linux-modules-<v> but owns NO files), linux-binary-<v> (owns
# /boot/vmlinuz-<v>), linux-modules-<v> (owns the module tree), linux-base-<v>,
# linux-headers-<v> and linux-headers-<v>-common-<flavour>.  dpkg -S ownership
# maps only the file-owning packages, so the image metapackage never appears in
# the grouped set unless the inventory adds it explicitly; it is the package apt
# cascade-removes when its binary/modules dependencies are purged.
write_tuxedo_split_fixture() {
    reset_fixture
    cat > "$fake_root/var/lib/dpkg/status" <<'STATUS'
Package: linux-image-7.1.8.2-tuxedo
Version: 7.1.8.2
Depends: linux-binary-7.1.8.2-tuxedo (= 7.1.8.2-1), linux-modules-7.1.8.2-tuxedo (= 7.1.8.2-1)
Status: install ok installed

Package: linux-binary-7.1.8.2-tuxedo
Version: 7.1.8.2
Depends: linux-base-7.1.8.2-tuxedo (= 7.1.8.2-1)
Status: install ok installed

Package: linux-modules-7.1.8.2-tuxedo
Version: 7.1.8.2
Depends: linux-base-7.1.8.2-tuxedo (= 7.1.8.2-1)
Status: install ok installed

Package: linux-base-7.1.8.2-tuxedo
Version: 7.1.8.2
Status: install ok installed

Package: linux-headers-7.1.8.2-tuxedo
Version: 7.1.8.2
Depends: linux-headers-7.1.8.2-common-tuxedo (= 7.1.8.2-1)
Status: install ok installed

Package: linux-headers-7.1.8.2-common-tuxedo
Version: 7.1.8.2
Status: install ok installed

Package: linux-image-7.1.9.1-tuxedo
Version: 7.1.9.1
Depends: linux-binary-7.1.9.1-tuxedo (= 7.1.9.1-1), linux-modules-7.1.9.1-tuxedo (= 7.1.9.1-1)
Status: install ok installed

Package: linux-binary-7.1.9.1-tuxedo
Version: 7.1.9.1
Depends: linux-base-7.1.9.1-tuxedo (= 7.1.9.1-1)
Status: install ok installed

Package: linux-modules-7.1.9.1-tuxedo
Version: 7.1.9.1
Depends: linux-base-7.1.9.1-tuxedo (= 7.1.9.1-1)
Status: install ok installed

Package: linux-base-7.1.9.1-tuxedo
Version: 7.1.9.1
Status: install ok installed

Package: linux-headers-7.1.9.1-tuxedo
Version: 7.1.9.1
Depends: linux-headers-7.1.9.1-common-tuxedo (= 7.1.9.1-1)
Status: install ok installed

Package: linux-headers-7.1.9.1-common-tuxedo
Version: 7.1.9.1
Status: install ok installed

Package: linux-image-tuxedo
Version: 7.1.9.1.2
Depends: linux-image-7.1.9.1-tuxedo (= 7.1.9.1-1)
Status: install ok installed

STATUS
    local old=7.1.8.2-tuxedo new=7.1.9.1-tuxedo
    mkdir -p "$fake_root/usr/lib/modules/$old" "$fake_root/usr/lib/modules/$new" \
        "$fake_root/usr/src/linux-headers-$old" "$fake_root/usr/src/linux-headers-$new" \
        "$fake_root/usr/src/linux-headers-7.1.8.2-common-tuxedo" "$fake_root/usr/src/linux-headers-7.1.9.1-common-tuxedo"
    touch "$fake_root/boot/vmlinuz-$old" "$fake_root/boot/vmlinuz-$new" \
        "$fake_root/boot/initrd.img-$old" "$fake_root/boot/initrd.img-$new"
    # linux-image-<v> owns NO files (it is the versioned metapackage).
    write_list "linux-binary-$old" "/boot/vmlinuz-$old"
    write_list "linux-modules-$old" "/usr/lib/modules/$old" "/usr/lib/modules/$old/kernel/drv.ko"
    write_list "linux-base-$old" "/usr/share/doc/linux-base-$old/copyright"
    write_list "linux-headers-$old" "/usr/src/linux-headers-$old" "/usr/src/linux-headers-$old/include/x.h"
    write_list "linux-headers-7.1.8.2-common-tuxedo" "/usr/src/linux-headers-7.1.8.2-common-tuxedo" "/usr/src/linux-headers-7.1.8.2-common-tuxedo/include/y.h"
    write_list "linux-binary-$new" "/boot/vmlinuz-$new"
    write_list "linux-modules-$new" "/usr/lib/modules/$new" "/usr/lib/modules/$new/kernel/drv.ko"
    write_list "linux-base-$new" "/usr/share/doc/linux-base-$new/copyright"
    write_list "linux-headers-$new" "/usr/src/linux-headers-$new" "/usr/src/linux-headers-$new/include/x.h"
    write_list "linux-headers-7.1.9.1-common-tuxedo" "/usr/src/linux-headers-7.1.9.1-common-tuxedo" "/usr/src/linux-headers-7.1.9.1-common-tuxedo/include/y.h"
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

# Five generic kernels (6.0.0-1 .. 6.4.0-5) for the menu-size guard.  The
# level-1 kernel menu must stay concise even with many kernels: its base64 must
# stay far under the GUI's PROMPT payload bound.
write_five_kernel_fixture() {
    reset_fixture
    local v
    {
        for v in 6.0.0-1 6.1.0-2 6.2.0-3 6.3.0-4 6.4.0-5; do
            cat <<STATUS
Package: linux-image-$v-generic
Version: $v
Status: install ok installed

Package: linux-modules-$v-generic
Version: $v
Status: install ok installed

Package: linux-headers-$v-generic
Version: $v
Status: install ok installed

STATUS
        done
    } > "$fake_root/var/lib/dpkg/status"
    for v in 6.0.0-1 6.1.0-2 6.2.0-3 6.3.0-4 6.4.0-5; do
        mkdir -p "$fake_root/usr/lib/modules/$v-generic" \
            "$fake_root/usr/src/linux-headers-$v-generic"
        touch "$fake_root/boot/vmlinuz-$v-generic"
        write_list "linux-image-$v-generic" "/boot/vmlinuz-$v-generic"
        write_list "linux-modules-$v-generic" "/usr/lib/modules/$v-generic" "/usr/lib/modules/$v-generic/kernel/drv.ko"
        write_list "linux-headers-$v-generic" "/usr/src/linux-headers-$v-generic" "/usr/src/linux-headers-$v-generic/include/x.h"
    done
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

# Leftover-kernel fixture (the REAL TUXEDO host symptom): one intact split kernel
# (7.3.0.1-tuxedo-amd64) plus four strays whose on-disk grouped files are gone --
# three image METAs left "removed but not purged" (Status: deinstall ok
# config-files, the `rc` flag; two TUXEDO linux-image-<v>-tuxedo-amd64 split
# metas whose binary/modules were removed, one stray Debian image
# linux-image-7.1.13+deb14-amd64) and one orphaned arch-independent common header
# (linux-headers-7.1.8.2-common-tuxedo, whose arch header and module tree are
# gone).  The intact kernel must stay protected; the image metas surface as
# incomplete (KERNEL_PARTIAL, removable through the existing partial action) and
# the common header as an orphaned header (KERNEL_ORPHANED_HEADERS).
write_tuxedo_strays_fixture() {
    reset_fixture
    cat > "$fake_root/var/lib/dpkg/status" <<'STATUS'
Package: linux-image-7.3.0.1-tuxedo-amd64
Version: 7.3.0.1
Depends: linux-binary-7.3.0.1-tuxedo-amd64 (= 7.3.0.1-1), linux-modules-7.3.0.1-tuxedo-amd64 (= 7.3.0.1-1)
Status: install ok installed

Package: linux-binary-7.3.0.1-tuxedo-amd64
Version: 7.3.0.1
Depends: linux-base-7.3.0.1-tuxedo-amd64 (= 7.3.0.1-1)
Status: install ok installed

Package: linux-modules-7.3.0.1-tuxedo-amd64
Version: 7.3.0.1
Depends: linux-base-7.3.0.1-tuxedo-amd64 (= 7.3.0.1-1)
Status: install ok installed

Package: linux-base-7.3.0.1-tuxedo-amd64
Version: 7.3.0.1
Status: install ok installed

Package: linux-headers-7.3.0.1-tuxedo-amd64
Version: 7.3.0.1
Depends: linux-headers-7.3.0.1-common-tuxedo (= 7.3.0.1-1)
Status: install ok installed

Package: linux-headers-7.3.0.1-common-tuxedo
Version: 7.3.0.1
Status: install ok installed

Package: linux-image-tuxedo-amd64
Version: 7.3.0.1.2
Depends: linux-image-7.3.0.1-tuxedo-amd64 (= 7.3.0.1-1)
Status: install ok installed

Package: linux-headers-tuxedo-amd64
Version: 7.3.0.1.2
Depends: linux-headers-7.3.0.1-tuxedo-amd64 (= 7.3.0.1-1)
Status: install ok installed

Package: linux-image-7.1.10.2-tuxedo-amd64
Version: 7.1.10.2
Depends: linux-binary-7.1.10.2-tuxedo-amd64 (= 7.1.10.2-1), linux-modules-7.1.10.2-tuxedo-amd64 (= 7.1.10.2-1)
Status: deinstall ok config-files

Package: linux-image-7.2.7.1-tuxedo-amd64
Version: 7.2.7.1
Depends: linux-binary-7.2.7.1-tuxedo-amd64 (= 7.2.7.1-1), linux-modules-7.2.7.1-tuxedo-amd64 (= 7.2.7.1-1)
Status: deinstall ok config-files

Package: linux-image-7.1.13+deb14-amd64
Version: 7.1.13+deb14
Status: deinstall ok config-files

Package: linux-headers-7.1.8.2-common-tuxedo
Version: 7.1.8.2
Status: install ok installed

STATUS
    local intact=7.3.0.1-tuxedo-amd64
    mkdir -p "$fake_root/usr/lib/modules/$intact" \
        "$fake_root/usr/src/linux-headers-$intact" \
        "$fake_root/usr/src/linux-headers-7.3.0.1-common-tuxedo"
    touch "$fake_root/boot/vmlinuz-$intact"
    # Only the intact kernel owns files; every stray owns none.
    write_list "linux-binary-$intact" "/boot/vmlinuz-$intact"
    write_list "linux-modules-$intact" "/usr/lib/modules/$intact" "/usr/lib/modules/$intact/kernel/drv.ko"
    write_list "linux-base-$intact" "/usr/share/doc/linux-base-$intact/copyright"
    write_list "linux-headers-$intact" "/usr/src/linux-headers-$intact" "/usr/src/linux-headers-$intact/include/x.h"
    write_list "linux-headers-7.3.0.1-common-tuxedo" "/usr/src/linux-headers-7.3.0.1-common-tuxedo" "/usr/src/linux-headers-7.3.0.1-common-tuxedo/include/y.h"
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
[[ -z "$KERNEL_RUNNING" ]] || fail "a repair target has no running kernel: $KERNEL_RUNNING"
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
[[ -z "$KERNEL_RUNNING" ]] || fail "a cached target diagnostic must parse to an empty running kernel: $KERNEL_RUNNING"
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

# ---- empty-inventory + repo-latest parses (Arch stale-running-kernel) --------
# The Arch stale-running-kernel state reports total=0 with a repository-latest,
# so an empty inventory must parse when a repo-latest kernel is known (the
# install-repo-latest recovery needs no installed kernels), and must still fail
# closed when no repo-latest is known.
KERNEL_BASELINE_B64="$(printf '%s' 'Kernel running: 7.2.8-arch1-2
Kernel latest: unknown
Kernel repository latest: 7.2.9-arch1-1
' | base64 | tr -d '\n')"
KERNEL_INVENTORY_VERSIONS=()
KERNEL_RUNNING=""
KERNEL_LATEST=""
KERNEL_REPO_LATEST=""
parse_cached_kernel_inventory
[[ "${#KERNEL_INVENTORY_VERSIONS[@]}" -eq 0 ]] \
    || fail "empty inventory must stay empty: ${KERNEL_INVENTORY_VERSIONS[*]}"
[[ "$KERNEL_REPO_LATEST" == 7.2.9-arch1-1 ]] \
    || fail "repo-latest: ${KERNEL_REPO_LATEST:-<empty>}"
[[ "$KERNEL_RUNNING" == 7.2.8-arch1-2 ]] \
    || fail "running kernel: ${KERNEL_RUNNING:-<empty>}"
[[ -z "$KERNEL_LATEST" ]] \
    || fail "Kernel latest: unknown must leave KERNEL_LATEST empty: $KERNEL_LATEST"

# The same empty inventory with no repo-latest still fails closed.
KERNEL_BASELINE_B64="$(printf '%s' 'Kernel running: 7.2.8-arch1-2
Kernel latest: unknown
' | base64 | tr -d '\n')"
KERNEL_REPO_LATEST=""
set +e
refusal_out="$(parse_cached_kernel_inventory 2>&1)"
refusal_rc=$?
set -e
[[ "$refusal_rc" -ne 0 ]] || fail 'an empty inventory with no repo-latest must fail closed'
grep -q 'names no installed kernels' <<<"$refusal_out" \
    || fail 'the empty-inventory refusal must name the installed-kernel baseline'
KERNEL_BASELINE_B64=""

# ---- stale-baseline running-kernel re-check fails closed (HOST only) ---------
# The running kernel changed since the diagnostic, so the baseline's protection
# set is stale: the repair action must fail closed before any mutation.  This
# fresh re-verification is a host-only safety invariant (a repair target has no
# running kernel to re-verify).
RUNNING_HOST_MODE=1
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
RUNNING_HOST_MODE=0
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
# The kernels capability evidence must surface the missing-headers state too
# (display evidence only -- the capability stays available).
cap_detail="$(repair_capability_evidence kernels)"
grep -q 'missing headers: 7.1.9.1-tuxedo-amd64' <<<"$cap_detail" \
    || fail "the kernels capability evidence must surface missing headers: $cap_detail"
grep -q 'removable=1' <<<"$cap_detail" \
    || fail "the kernels capability evidence must still report the removable kernel: $cap_detail"

# ---- TUXEDO split kernel: versioned image metapackage in the removal plan ----
# linux-image-<v> owns no files (it depends on linux-binary-<v> +
# linux-modules-<v>), so dpkg -S ownership never maps it.  The inventory must
# still group it so apt_kernel_cleanup_stage removes it explicitly instead of
# letting apt cascade-remove it as an unrequested package (which the removal
# validator refuses fail-closed with apt-refused-protected).
write_tuxedo_split_fixture
DPKG_PATH_OWNER_INDEX=""
kernel_inventory_collect
[[ "$KERNEL_LATEST" == 7.1.9.1-tuxedo ]] || fail "split latest kernel: $KERNEL_LATEST"
[[ "${#KERNEL_REMOVABLE[@]}" -eq 1 && "${KERNEL_REMOVABLE[0]}" == 7.1.8.2-tuxedo ]] \
    || fail "split removable: ${KERNEL_REMOVABLE[*]}"
rem_pkgs="${KERNEL_REMOVABLE_PKGS[*]}"
grep -q 'linux-image-7.1.8.2-tuxedo' <<<"$rem_pkgs" \
    || fail "split inventory must group the versioned image metapackage: $rem_pkgs"
grep -q 'linux-binary-7.1.8.2-tuxedo' <<<"$rem_pkgs" \
    || fail "split inventory must group linux-binary: $rem_pkgs"
grep -q 'linux-modules-7.1.8.2-tuxedo' <<<"$rem_pkgs" \
    || fail "split inventory must group linux-modules: $rem_pkgs"
grep -q 'linux-headers-7.1.8.2-common-tuxedo' <<<"$rem_pkgs" \
    || fail "split inventory must group the common headers: $rem_pkgs"
[[ "$(wc -w <<<"$rem_pkgs" | tr -d '[:space:]')" -eq 5 ]] \
    || fail "split removable plan must hold exactly 5 packages: $rem_pkgs"
kernel_package_is_removable linux-image-7.1.8.2-tuxedo \
    || fail 'the split image metapackage must be a removal candidate'
kernel_package_is_removable linux-image-7.1.9.1-tuxedo \
    && fail 'the latest split image metapackage must never be removable'
kernel_package_is_removable linux-image-tuxedo \
    && fail 'the unversioned image metapackage must never be removable'

# ---- TUXEDO split kernel removal stage: no protected refusal -----------------
# Simulate apt proposing to remove the old kernel's whole split package set
# (including the versioned image metapackage that apt cascade-removes); the
# removal validator must accept the now-complete plan instead of refusing
# linux-image-<v> as apt-refused-protected.
write_tuxedo_split_fixture
DPKG_PATH_OWNER_INDEX=""
kernel_inventory_collect
: > "$sandbox/apply.log"
: > "$sandbox/rollback.log"
run_selected_chroot() {
    printf '%s\n' \
        'Reading package lists...' \
        'Remv linux-image-7.1.8.2-tuxedo [7.1.8.2]' \
        'Remv linux-binary-7.1.8.2-tuxedo [7.1.8.2]' \
        'Remv linux-modules-7.1.8.2-tuxedo [7.1.8.2]' \
        'Remv linux-headers-7.1.8.2-tuxedo [7.1.8.2]' \
        'Remv linux-headers-7.1.8.2-common-tuxedo [7.1.8.2]' \
        '0 upgraded, 0 newly installed, 5 to remove and 0 not upgraded.'
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
    || fail "split kernel removal must report changed (got: $status_line)"
grep -q 'linux-image-7.1.8.2-tuxedo' "$sandbox/apply.log" \
    || fail 'the split image metapackage was not in the removal list'
if grep -q 'linux-image-7.1.9.1-tuxedo' "$sandbox/apply.log"; then
    fail "protected split image metapackage leaked into the removal list: $(tr '\n' ' ' < "$sandbox/apply.log")"
fi
[[ ! -s "$sandbox/rollback.log" ]] || fail 'no rollback must run on a clean split removal'

# ---- rpm kernel inventory: no dpkg ownership -> no false orphaned kernels ----
# On rpm, ownership is mapped through dnf/rpm (never dpkg -S), so the dpkg-only
# orphaned/partial probes must be skipped: a healthy rpm kernel must never be
# flagged orphaned, and the missing-headers probe must detect the kernel-devel
# build tree (a protected kernel with running == latest is listed at most once).
profile_target_backends() {
    TARGET_DISTRO_FAMILY=fedora
    TARGET_PACKAGE_MANAGERS=(rpm)
    TARGET_PACKAGE_MANAGER=rpm
    TARGET_SERVICE_MANAGERS=(systemd)
    TARGET_SERVICE_MANAGER=systemd
    TARGET_DISPLAY_BACKEND=systemd
    TARGET_INITRAMFS_BACKENDS=(dracut)
    TARGET_INITRAMFS_BACKEND=dracut
    TARGET_LOGGING_BACKEND=journald
    TARGET_BOOTLOADER_BACKEND=grub
    TARGET_ESP_MOUNT=unresolved
    TARGET_KERNEL_LAYOUT='versioned vmlinuz-* kernels'
    TARGET_REPAIR_BACKEND='Fedora/rpm'
}
DPKG_PATH_OWNER_INDEX=""
FAKE_RUNNING=6.8.1-200.fc44.x86_64
reset_fixture
# A real rpm target is detected by the rpm executable (target_rpm_detected),
# which gates the dpkg-only orphan/partial probes off.  The dnf5 stub, sqlite
# rpmdb and one enabled repository make the guarded rpm backend runnable for the
# capability gate below.
touch "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5"
chmod +x "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5"
mkdir -p "$fake_root/var/lib/rpm" "$fake_root/etc/yum.repos.d"
printf 'sqlite' > "$fake_root/var/lib/rpm/rpmdb.sqlite"
printf '[fedora]\nenabled=1\n' > "$fake_root/etc/yum.repos.d/fedora.repo"
rpm_old=6.8.0-100.fc44.x86_64
rpm_new=6.8.1-200.fc44.x86_64
# A real Fedora target has the kernel-devel build tree as the real directory
# /usr/src/kernels/<kver> (with /lib/modules/<kver>/build an absolute symlink
# to it), so the missing-headers probe checks /usr/src/kernels/<kver>.
mkdir -p "$fake_root/usr/lib/modules/$rpm_old" "$fake_root/usr/lib/modules/$rpm_new" \
         "$fake_root/usr/src/kernels/$rpm_old" "$fake_root/usr/src/kernels/$rpm_new"
touch "$fake_root/boot/vmlinuz-$rpm_old" "$fake_root/boot/vmlinuz-$rpm_new"

# Emulate the read-only `rpm -qf` ownership query inside the chroot: each
# authoritative file maps to its owning package NEVRA (name-version-release.arch).
run_selected_chroot() {
    local arg path ver
    for arg in "$@"; do
        [[ "$arg" == /* ]] || continue
        path="$arg"
        case "$path" in
            /boot/vmlinuz-*)
                ver="${path#/boot/vmlinuz-}"
                printf 'kernel-core-%s\n' "$ver"
                ;;
            /usr/lib/modules/*|/lib/modules/*)
                ver="${path#/usr/lib/modules/}"
                ver="${ver#/lib/modules/}"
                printf 'kernel-modules-%s\n' "$ver"
                ;;
            /usr/src/kernels/*)
                ver="${path#/usr/src/kernels/}"
                printf 'kernel-devel-%s\n' "$ver"
                ;;
        esac
    done
    return 0
}

kernel_inventory_collect
[[ "${#KERNEL_ORPHANED_KERNELS[@]}" -eq 0 ]] \
    || fail "rpm must not flag kernels without dpkg ownership as orphaned: ${KERNEL_ORPHANED_KERNELS[*]}"
[[ "${#KERNEL_ORPHANED_HEADERS[@]}" -eq 0 ]] \
    || fail "rpm must not report orphaned headers: ${KERNEL_ORPHANED_HEADERS[*]}"
[[ "${#KERNEL_PARTIAL[@]}" -eq 0 ]] \
    || fail "rpm must not report partial kernels: ${KERNEL_PARTIAL[*]}"
[[ "${#KERNEL_ORPHANED_SUPPORT_VERSIONS[@]}" -eq 0 ]] \
    || fail "rpm must not report orphaned build support: ${KERNEL_ORPHANED_SUPPORT_VERSIONS[*]}"
[[ "${#KERNEL_MISSING_HEADERS[@]}" -eq 0 ]] \
    || fail "rpm with build trees present must report no missing headers: ${KERNEL_MISSING_HEADERS[*]}"

# The akmods capability gate must accept the real kernel-devel directory too:
# the build-tree probe now checks /usr/src/kernels/<kver> (the real directory,
# never the /lib/modules/<kver>/build absolute symlink dereferenced against the
# recovery host's root), so a Fedora target with kernel-devel installed is never
# reported akmods-no-build-tree.
touch "$fake_root/usr/bin/akmods"
chmod +x "$fake_root/usr/bin/akmods"
if reason="$(dkms_unavailable_reason)"; then
    : # available
else
    fail "akmods must be available with /usr/src/kernels/<kver> present; got $reason"
fi
# The display-only capability detail probes the same real kernel-devel
# directory (never the /lib/modules/<kver>/build symlink dereferenced against
# the recovery host's root), so a healthy target reports "build tree present".
detail="$(repair_capability_evidence dkms)"
grep -q "kernel $rpm_old build tree present" <<<"$detail" \
    || fail "akmods capability detail must report the old kernel build tree present: $detail"
grep -q "kernel $rpm_new build tree present" <<<"$detail" \
    || fail "akmods capability detail must report the new kernel build tree present: $detail"
grep -q "build tree missing" <<<"$detail" \
    && fail "akmods capability detail must not report a missing build tree: $detail"
rm -f "$fake_root/usr/bin/akmods"

# Part 1 acceptance: the rpm ownership mapping populates the per-kernel package
# set (the cause of the former empty `pkgs=`), never leaving a kernel orphaned.
[[ "${#KERNEL_INVENTORY_PKGS[@]}" -eq 2 ]] \
    || fail "rpm inventory must map both kernels: ${KERNEL_INVENTORY_PKGS[*]}"
for joined in "${KERNEL_INVENTORY_PKGS[@]}"; do
    [[ -n "$joined" ]] \
        || fail "rpm inventory must not leave a kernel with empty pkgs: ${KERNEL_INVENTORY_PKGS[*]}"
done
grep -q 'kernel-core-6.8.0-100.fc44.x86_64' <<<"${KERNEL_INVENTORY_PKGS[0]}" \
    || fail "rpm ownership must map the old kernel to kernel-core: ${KERNEL_INVENTORY_PKGS[0]}"
grep -q 'kernel-modules-6.8.0-100.fc44.x86_64' <<<"${KERNEL_INVENTORY_PKGS[0]}" \
    || fail "rpm ownership must map the old kernel to kernel-modules: ${KERNEL_INVENTORY_PKGS[0]}"

# Part 3 acceptance: the kernels capability gate now reports available on a
# healthy rpm target (kernel-cleanup:rpm is wired and runnable).
if reason="$(kernels_unavailable_reason)"; then
    : # available
else
    fail "kernels must be available on a healthy rpm target; got $reason"
fi

# Drop the protected (running == latest) kernel's build tree: it is reported
# exactly once as missing headers, and no kernel is falsely orphaned.
rm -rf "$fake_root/usr/src/kernels/$rpm_new"
kernel_inventory_collect
[[ "${KERNEL_MISSING_HEADERS[*]}" == "$rpm_new" ]] \
    || fail "rpm missing headers must name the protected kernel exactly once: ${KERNEL_MISSING_HEADERS[*]}"
[[ "${#KERNEL_ORPHANED_KERNELS[@]}" -eq 0 ]] \
    || fail "rpm must not flag the build-tree-less kernel as orphaned: ${KERNEL_ORPHANED_KERNELS[*]}"

# ---- rpm install-headers: exact NEVRA, guarded rpm backend, deduplicated -----
# With the protected kernel's build tree dropped, KERNEL_MISSING_HEADERS names
# 6.8.1-200.fc44.x86_64, so the install action builds kernel-devel-<kver> and
# hands it to the guarded rpm install backend (never apt-get).  The availability
# pre-flight is stubbed available so the install path is exercised.
: > "$sandbox/rpm-install.log"
rpm_package_available() { return 0; }
guarded_rpm_install_packages() {
    printf '%s\n' "$@" >> "$sandbox/rpm-install.log"
    return 0
}
rpm_install_missing_kernel_headers
grep -qx 'kernel-devel-6.8.1-200.fc44.x86_64' "$sandbox/rpm-install.log" \
    || fail "rpm install-headers must install the kernel-devel NEVRA: $(tr '\n' ' ' < "$sandbox/rpm-install.log")"
[[ "$(wc -l < "$sandbox/rpm-install.log" | tr -d '[:space:]')" -eq 1 ]] \
    || fail "rpm install-headers must deduplicate the NEVRA list: $(tr '\n' ' ' < "$sandbox/rpm-install.log")"
# A repeated missing-header entry installs exactly one NEVRA.
: > "$sandbox/rpm-install.log"
KERNEL_MISSING_HEADERS=("$rpm_new" "$rpm_new")
rpm_install_missing_kernel_headers
[[ "$(wc -l < "$sandbox/rpm-install.log" | tr -d '[:space:]')" -eq 1 ]] \
    || fail "rpm install-headers must deduplicate repeated versions: $(tr '\n' ' ' < "$sandbox/rpm-install.log")"
grep -qx 'kernel-devel-6.8.1-200.fc44.x86_64' "$sandbox/rpm-install.log" \
    || fail "rpm install-headers dedup must keep the kernel-devel NEVRA: $(tr '\n' ' ' < "$sandbox/rpm-install.log")"

# A proven no-op install (kernel-devel already installed, e.g. a stale cached
# baseline) must report unchanged, never changed: guarded_rpm_install_packages
# returns 1 to signal the no-op and repair_kernel_install_headers propagates it.
: > "$sandbox/rpm-install.log"
guarded_rpm_install_packages() {
    printf '%s\n' "$@" >> "$sandbox/rpm-install.log"
    return 1
}
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_install_headers)"
grep -q '^Repair change status kernels:install-headers: unchanged|reason:no-missing-headers$' <<<"$status_line" \
    || fail "rpm install-headers must report unchanged on a proven no-op: $status_line"

# ---- rpm install-headers: pre-flight availability ---------------------------
# A stale running kernel whose kernel-devel was pruned from the enabled
# repositories must be pre-flighted with rpm_package_available (dnf5 repoquery):
# when it is unavailable rpm_install_missing_kernel_headers records
# KERNEL_HEADERS_UNAVAILABLE and returns 2, and repair_kernel_install_headers
# reports headers-package-unavailable — never a raw `dnf5 ... No match`.
: > "$sandbox/rpm-install.log"
RPM_INSTALL_UNAVAILABLE_CALLED=""
rpm_package_available() { return 1; }
guarded_rpm_install_packages() {
    RPM_INSTALL_UNAVAILABLE_CALLED=yes
    printf '%s\n' "$*" >> "$sandbox/rpm-install.log"
    return 0
}
set +e
rpm_install_missing_kernel_headers
rpm_hdr_rc=$?
set -e
[[ "$rpm_hdr_rc" -eq 2 ]] \
    || fail "rpm install-headers must return 2 when kernel-devel is unavailable (got $rpm_hdr_rc)"
[[ "${KERNEL_HEADERS_UNAVAILABLE:-}" == "$rpm_new" ]] \
    || fail "rpm install-headers must record KERNEL_HEADERS_UNAVAILABLE=$rpm_new (got ${KERNEL_HEADERS_UNAVAILABLE:-})"
[[ -z "$RPM_INSTALL_UNAVAILABLE_CALLED" ]] \
    || fail 'rpm install-headers must never install an unavailable kernel-devel'
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_install_headers)"
grep -q '^Repair change status kernels:install-headers: unchanged|reason:headers-package-unavailable' <<<"$status_line" \
    || fail "rpm install-headers must report headers-package-unavailable: $status_line"
grep -q "param:${rpm_new}" <<<"$status_line" \
    || fail "rpm install-headers headers-package-unavailable must name the version: $status_line"

# ---- rpm orphaned kernel-devel: dev installed, module tree gone --------------
# A kernel-devel-<kver> still installed whose kernel module tree is gone (the
# kernel-core/-modules NEVRAs were removed while the per-kernel build tree
# remained) surfaces as an orphaned header (KERNEL_ORPHANED_HEADERS) so the menu
# offers removal; the intact kernel's kernel-devel (module tree present) is never
# flagged, and the single unversioned kernel-headers UAPI package is never
# confused with a per-kernel kernel-devel NEVRA.
reset_fixture
touch "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5"
chmod +x "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5"
mkdir -p "$fake_root/var/lib/rpm" "$fake_root/etc/yum.repos.d"
printf 'sqlite' > "$fake_root/var/lib/rpm/rpmdb.sqlite"
printf '[fedora]\nenabled=1\n' > "$fake_root/etc/yum.repos.d/fedora.repo"
rpm_intact=6.8.1-200.fc44.x86_64
rpm_orphaned_dev=6.8.0-100.fc44.x86_64
# The kernel-devel-matched metapackage is also installed in this rpmdb (it pulls
# the matching per-kernel kernel-devel) but must NEVER surface as an orphaned
# header: it is a meta, never a per-kernel kernel-devel-<NEVRA>.  The exact
# `rpm -q kernel-devel` enumeration returns only the real per-kernel kernel-devel
# NEVRAs, so the orphaned probe never sees the matched-* NEVRA (the old
# `rpm -qa 'kernel-devel-*'` glob also matched the kernel-devel-matched-<NEVRA>
# string and leaked it).
mkdir -p "$fake_root/usr/lib/modules/$rpm_intact" \
         "$fake_root/usr/src/kernels/$rpm_intact" \
         "$fake_root/usr/src/kernels/$rpm_orphaned_dev"
touch "$fake_root/boot/vmlinuz-$rpm_intact"
run_selected_chroot() {
    local arg path ver
    # Emulate the exact `rpm -q kernel-devel` enumeration (the repaired query):
    # it returns ONLY the real per-kernel kernel-devel NEVRAs, never the
    # kernel-devel-matched metapackage (a different package name).
    for arg in "$@"; do
        [[ "$arg" == -q ]] || continue
        printf 'kernel-devel-%s\n' "$rpm_intact"
        printf 'kernel-devel-%s\n' "$rpm_orphaned_dev"
        return 0
    done
    # Emulate `rpm -qf` (ownership of each authoritative path).
    for arg in "$@"; do
        [[ "$arg" == /* ]] || continue
        path="$arg"
        case "$path" in
            /boot/vmlinuz-*)
                printf 'kernel-core-%s\n' "${path#/boot/vmlinuz-}" ;;
            /usr/lib/modules/*|/lib/modules/*)
                ver="${path#/usr/lib/modules/}"
                ver="${ver#/lib/modules/}"
                printf 'kernel-modules-%s\n' "$ver" ;;
            /usr/src/kernels/*)
                printf 'kernel-devel-%s\n' "${path#/usr/src/kernels/}" ;;
        esac
    done
    return 0
}
# Fix 1 direct: rpm_installed_kernel_devel_nevras enumerates ONLY the real
# per-kernel kernel-devel NEVRAs (never kernel-devel-matched).
dev_nevras="$(rpm_installed_kernel_devel_nevras)"
grep -q "kernel-devel-$rpm_intact" <<<"$dev_nevras" \
    || fail "rpm_installed_kernel_devel_nevras must enumerate the real kernel-devel NEVRA: $dev_nevras"
grep -q 'matched' <<<"$dev_nevras" \
    && fail "rpm_installed_kernel_devel_nevras must never enumerate kernel-devel-matched: $dev_nevras"
kernel_inventory_collect
[[ "${KERNEL_INVENTORY_VERSIONS[*]}" == "$rpm_intact" ]] \
    || fail "rpm orphaned-devel fixture must keep only the intact kernel in the inventory: ${KERNEL_INVENTORY_VERSIONS[*]}"
[[ "${KERNEL_ORPHANED_HEADERS[*]}" == "$rpm_orphaned_dev" ]] \
    || fail "rpm must surface the orphaned kernel-devel as an orphaned header: ${KERNEL_ORPHANED_HEADERS[*]}"
[[ "${#KERNEL_PARTIAL[@]}" -eq 0 ]] \
    || fail "rpm must not flag the orphaned kernel-devel as partial: ${KERNEL_PARTIAL[*]}"
[[ "${#KERNEL_ORPHANED_KERNELS[@]}" -eq 0 ]] \
    || fail "rpm must not flag the orphaned kernel-devel as an orphaned kernel: ${KERNEL_ORPHANED_KERNELS[*]}"
grep -q "$rpm_intact" <<<"${KERNEL_ORPHANED_HEADERS[*]}" \
    && fail "the intact kernel's kernel-devel must never be flagged orphaned"
# Fix 1 regression: the kernel-devel-matched metapackage must never leak into
# the orphaned headers (never surfaced as matched-<NEVRA>).
grep -q 'matched-' <<<"${KERNEL_ORPHANED_HEADERS[*]}" \
    && fail "kernel-devel-matched must never surface as an orphaned header: ${KERNEL_ORPHANED_HEADERS[*]}"

# Restore the Debian/APT harness for the remaining fixtures.
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
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
DPKG_PATH_OWNER_INDEX=""
rm -f "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5"
rm -rf "$fake_root/var/lib/rpm" "$fake_root/etc/yum.repos.d"

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
# Drop the old kernel entirely (module tree, boot image AND its image/modules/
# headers packages) so only running/latest remains with no orphaned/partial
# kernel left behind.  The status edit matches each stanza by its package name
# (the second whitespace token of the "Package:" line) rather than a
# GNU-awk-only `\n`/`m` regex, so it behaves identically under GNU and BusyBox
# awk and leaves no half-removed image package for the partial-kernel probe.
FAKE_RUNNING=6.5.0-10038-generic
rm -rf "$fake_root/usr/lib/modules/6.2.0-1000-generic" \
       "$fake_root/usr/src/linux-headers-6.2.0-1000-generic" \
       "$fake_root/usr/src/linux-headers-6.2.0-1000"
rm -f "$fake_root/boot/vmlinuz-6.2.0-1000-generic"
awk 'BEGIN { RS = ""; ORS = "\n\n" }
     $2 != "linux-image-6.2.0-1000-generic" \
     && $2 != "linux-modules-6.2.0-1000-generic" \
     && $2 != "linux-headers-6.2.0-1000-generic" \
     && $2 != "linux-headers-6.2.0-1000" { print }' \
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
# WITHOUT re-offering the already-removed kernel.  remove-oldest is a GLOBAL
# action, so the two-level menu runs it directly from the kernel-selection
# prompt (kernels are numbered 1..2, the global action follows at 3).
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
MENU_ANSWER="3"
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
grep -q 'newest installed kernel: 7.1.9.1-tuxedo-amd64' "$LOOP_PROMPT_FILE" \
    || fail 'the kernel menu must begin with the concise newest-installed summary'
grep -q 'Select a kernel (or a global action):' "$LOOP_PROMPT_FILE" \
    || fail 'the kernel menu must offer the kernel-selection prompt'
grep -q 'Global actions:' "$LOOP_PROMPT_FILE" \
    || fail 'the kernel menu must list the Global actions section'
grep -q '0) Abort' "$LOOP_PROMPT_FILE" \
    || fail 'the kernel menu must use the 0) Abort abort line'
if grep -q '^Found:$' "$LOOP_PROMPT_FILE"; then
    fail 'the kernel menu must not dump the full Found: evidence block'
fi
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
# Host mode: the middle kernel is the running kernel, so it is protected and the
# oldest kernel is removable.  (In target mode there is no running kernel, so
# every non-newest kernel would be removable instead.)
RUNNING_HOST_MODE=1
write_legacy_fixture
FAKE_RUNNING=2.6.32-5-amd64
kernel_inventory_collect
[[ "${KERNEL_INVENTORY_VERSIONS[*]}" == '2.6.26-2-amd64 2.6.32-5-amd64 3.5.0-27-generic' ]] \
    || fail "legacy versions must sort by version, got: ${KERNEL_INVENTORY_VERSIONS[*]}"
[[ "$KERNEL_LATEST" == 3.5.0-27-generic ]] || fail "legacy latest kernel: $KERNEL_LATEST"
[[ "${#KERNEL_REMOVABLE[@]}" -eq 1 && "${KERNEL_REMOVABLE[0]}" == 2.6.26-2-amd64 ]] \
    || fail "legacy removable: ${KERNEL_REMOVABLE[*]}"
RUNNING_HOST_MODE=0
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
grep -q '^Kernel state: running=none installed-latest=7.1.9.1-tuxedo-amd64 repo-latest=7.1.9.3-tuxedo-amd64$' <<<"$output" \
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

# ---- leftover kernels: TUXEDO strays + intact current kernel ----------------
# A half-removed split META (installed, its binary/modules removed -> no vmlinuz
# and no module tree) and a stray non-flavour image surface as incomplete
# (KERNEL_PARTIAL, removable through the existing partial action); an orphaned
# arch-independent common header surfaces as KERNEL_ORPHANED_HEADERS; the intact
# current kernel stays protected and never appears in orphaned/partial/removable.
write_tuxedo_strays_fixture
kernel_inventory_collect
[[ "${KERNEL_INVENTORY_VERSIONS[*]}" == '7.3.0.1-tuxedo-amd64' ]] \
    || fail "strays fixture must leave exactly the intact kernel in the inventory: ${KERNEL_INVENTORY_VERSIONS[*]}"
[[ "$KERNEL_LATEST" == 7.3.0.1-tuxedo-amd64 ]] || fail "strays fixture latest kernel: $KERNEL_LATEST"
# The two half-removed TUXEDO METAs and the stray Debian image are incomplete.
[[ "${KERNEL_PARTIAL[*]}" == '7.1.10.2-tuxedo-amd64 7.1.13+deb14-amd64 7.2.7.1-tuxedo-amd64' ]] \
    || fail "the image strays must surface as incomplete (partial): ${KERNEL_PARTIAL[*]}"
# The orphaned common header (arch header + module tree gone) is an orphaned header.
[[ "${KERNEL_ORPHANED_HEADERS[*]}" == '7.1.8.2-common-tuxedo' ]] \
    || fail "the orphaned common header must surface as an orphaned header: ${KERNEL_ORPHANED_HEADERS[*]}"
# No zombie residue (on-disk files without an owning package) exists in this fixture.
[[ "${#KERNEL_ORPHANED_KERNELS[@]}" -eq 0 ]] \
    || fail "the strays fixture must not flag any orphaned kernel component: ${KERNEL_ORPHANED_KERNELS[*]}"
# The intact kernel is never orphaned / partial / removable.
grep -q '7.3.0.1-tuxedo-amd64' <<<"${KERNEL_ORPHANED_HEADERS[*]}" \
    && fail 'the intact kernel must never be flagged orphaned'
grep -q '7.3.0.1-tuxedo-amd64' <<<"${KERNEL_PARTIAL[*]}" \
    && fail 'the intact kernel must never be flagged incomplete'
grep -q '7.3.0.1-tuxedo-amd64' <<<"${KERNEL_REMOVABLE[*]}" \
    && fail 'the intact kernel must never be flagged removable'
run_selected_chroot() { return 1; }
output="$(diagnostic_kernel || true)"
grep -q '^Kernel inventory: total=1 protected=1 removable=0 backend=apt/dpkg$' <<<"$output" \
    || fail 'strays inventory summary must stay clean except for the incomplete/orphaned leftovers'
grep -q '^Kernel orphaned headers: 7.1.8.2-common-tuxedo$' <<<"$output" \
    || fail 'strays orphaned-headers evidence line must name the common header'
grep -q '^Kernel incomplete: 7.1.10.2-tuxedo-amd64$' <<<"$output" \
    || fail 'strays incomplete evidence line must name the half-removed meta'
grep -q '^Kernel incomplete: 7.2.7.1-tuxedo-amd64$' <<<"$output" \
    || fail 'strays incomplete evidence line must name the second half-removed meta'
grep -q '^Kernel incomplete: 7.1.13+deb14-amd64$' <<<"$output" \
    || fail 'strays incomplete evidence line must name the stray Debian image'

# ---- remove-newest: broken newest removable, running always protected -------
# This is a HOST-mode scenario: the running kernel is the older kernel, so the
# newest (broken) kernel is removable but the running kernel is never.
RUNNING_HOST_MODE=1
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

# ---- remove-newest refusal when newest == running (HOST) -------------------
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
RUNNING_HOST_MODE=0

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

# ---- initramfs rebuild after a removal uses the FRESH inventory ------------
# kernel_cleanup_stage removes the oldest kernel and marks the removal applied;
# repair_kernel_remove_oldest must then DROP the pre-removal cached baseline and
# re-collect the live inventory so adaptive_initramfs_repair rebuilds only the
# REMAINING kernels (never the just-removed one).
# Re-source the helper to restore the real sub-action (the interactive-menu test
# above replaced repair_kernel_remove_oldest with a simulation stub), then
# re-establish the Debian harness environment.
source <(sed '/^main "$@"/d' "$HELPER")
TARGET_ROOT="$fake_root"
RUNNING_HOST_MODE=0
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"
DPKG_PATH_OWNER_INDEX=""
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

write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
kernel_inventory_collect
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"

# Simulate the guarded removal exactly as apt_kernel_cleanup_stage would: drop
# the old kernel's module tree, headers, boot files, dpkg file lists and status.
kernel_cleanup_stage() {
    rm -rf "$fake_root/usr/lib/modules/7.1.8.2-tuxedo-amd64" \
           "$fake_root/usr/src/linux-headers-7.1.8.2-tuxedo-amd64" \
           "$fake_root/usr/src/linux-headers-7.1.8.2-common"
    rm -f "$fake_root/boot/vmlinuz-7.1.8.2-tuxedo-amd64" \
          "$fake_root/boot/initrd.img-7.1.8.2-tuxedo-amd64"
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
    KERNEL_CLEANUP_APPLIED=true
}
dpkg_path_owner_map_clear() { DPKG_PATH_OWNER_INDEX=""; }

INITRAMFS_KERNELS=""
adaptive_initramfs_repair() {
    INITRAMFS_KERNELS="$(repair_kernel_versions installed | paste -sd, -)"
}
adaptive_grub_stage() { :; }
adaptive_extlinux_repair() { :; }

REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
repair_kernel_remove_oldest

# The cached baseline must be dropped and the inventory re-collected fresh.
[[ -z "$KERNEL_BASELINE_B64" ]] \
    || fail 'remove-oldest must drop the cached baseline after the removal'
[[ "${KERNEL_INVENTORY_VERSIONS[*]}" == '7.1.9.1-tuxedo-amd64' ]] \
    || fail "remove-oldest must re-collect only the remaining kernel, got: ${KERNEL_INVENTORY_VERSIONS[*]}"
# The initramfs rebuild must target only the remaining kernel, never the removed one.
[[ "$INITRAMFS_KERNELS" == '7.1.9.1-tuxedo-amd64' ]] \
    || fail "initramfs rebuild must target only the remaining kernel, got: $INITRAMFS_KERNELS"
grep -q '7.1.8.2-tuxedo-amd64' <<<"$INITRAMFS_KERNELS" \
    && fail 'initramfs rebuild must never target the removed kernel'
KERNEL_BASELINE_B64=""

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

# ---- reinstall drops the cached baseline and re-collects before regen -------
# repair_kernel_reinstall mutates the target (reinstall/upgrade), so it must
# drop the pre-install baseline and re-collect before adaptive_initramfs_repair,
# mirroring the removal sub-actions.  With the baseline still set,
# repair_kernel_versions installed would re-parse the stale pre-install
# inventory; the fresh scan (installed_kernel_versions) reflects the real
# on-disk vmlinuz set.
write_tuxedo_fixture
rm -f "$fake_root/boot/vmlinuz-7.1.8.2-tuxedo-amd64"
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
safe_apt_install_packages() { :; }
INITRAMFS_KERNELS=""
adaptive_initramfs_repair() {
    INITRAMFS_KERNELS="$(repair_kernel_versions installed | paste -sd, -)"
}
adaptive_grub_stage() { :; }
adaptive_extlinux_repair() { :; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
repair_kernel_reinstall
[[ -z "$KERNEL_BASELINE_B64" ]] \
    || fail 'reinstall must drop the cached baseline after the mutation'
# The stale baseline still lists 7.1.8.2 (its vmlinuz is gone), but the fresh
# scan must only see kernels with a real /boot/vmlinuz-* image.
[[ "$INITRAMFS_KERNELS" == *7.1.9.1-tuxedo-amd64* ]] \
    || fail "reinstall must re-collect before the initramfs regen, got: $INITRAMFS_KERNELS"
grep -q '7.1.8.2-tuxedo-amd64' <<<"$INITRAMFS_KERNELS" \
    && fail 'reinstall must not re-parse the stale baseline (7.1.8.2 has no vmlinuz)'
KERNEL_BASELINE_B64=""

# ---- install-repo-latest re-collects so the NEW kernel is rebuilt -----------
# repair_kernel_install_repo_latest installs a kernel that was NOT in the
# pre-install baseline; it must drop the baseline and re-collect so the
# initramfs regen includes the newly-installed kernel (otherwise it is left
# without an initramfs and unbootable).
write_tuxedo_fixture
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
apt_kernel_repo_latest() { printf '%s\n' '7.1.10.1-tuxedo-amd64'; }
safe_apt_install_packages() {
    local new=7.1.10.1-tuxedo-amd64
    mkdir -p "$fake_root/usr/lib/modules/$new" \
        "$fake_root/usr/src/linux-headers-$new" "$fake_root/usr/src/linux-headers-7.1.10.1-common"
    touch "$fake_root/boot/vmlinuz-$new" "$fake_root/boot/initrd.img-$new"
    write_list "linux-image-$new" "/boot/vmlinuz-$new"
    write_list "linux-modules-$new" "/usr/lib/modules/$new" "/usr/lib/modules/$new/kernel/drv.ko"
    write_list "linux-headers-$new" "/usr/src/linux-headers-$new" "/usr/src/linux-headers-$new/include/x.h"
    write_list "linux-headers-7.1.10.1-common-tuxedo" "/usr/src/linux-headers-7.1.10.1-common" "/usr/src/linux-headers-7.1.10.1-common/include/y.h"
    cat >> "$fake_root/var/lib/dpkg/status" <<'STATUS'

Package: linux-image-7.1.10.1-tuxedo-amd64
Version: 7.1.10.1
Status: install ok installed

Package: linux-modules-7.1.10.1-tuxedo-amd64
Version: 7.1.10.1
Status: install ok installed

Package: linux-headers-7.1.10.1-tuxedo-amd64
Version: 7.1.10.1
Depends: linux-headers-7.1.10.1-common-tuxedo (= 7.1.10.1-1)
Status: install ok installed

Package: linux-headers-7.1.10.1-common-tuxedo
Version: 7.1.10.1
Status: install ok installed
STATUS
}
INITRAMFS_KERNELS=""
adaptive_initramfs_repair() {
    INITRAMFS_KERNELS="$(repair_kernel_versions installed | paste -sd, -)"
}
adaptive_grub_stage() { :; }
adaptive_extlinux_repair() { :; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
repair_kernel_install_repo_latest
[[ -z "$KERNEL_BASELINE_B64" ]] \
    || fail 'install-repo-latest must drop the cached baseline after the install'
grep -q '7.1.10.1-tuxedo-amd64' <<<"$INITRAMFS_KERNELS" \
    || fail "install-repo-latest must include the NEW kernel in the initramfs regen, got: $INITRAMFS_KERNELS"
KERNEL_BASELINE_B64=""

# ---- orphaned build/support detection ---------------------------------------
# The 7.1.10.2 kernel is gone (no module tree, no image/modules package) while
# linux-kbuild-7.1.10.2 survived -> orphaned; linux-kbuild-7.1.13.1 belongs to
# the still-installed 7.1.13.1 kernel -> protected.
FAKE_RUNNING=7.1.13.1-tuxedo-amd64
write_kbuild_fixture
kernel_inventory_collect
[[ -z "$KERNEL_RUNNING" ]] || fail "a repair target has no running kernel: $KERNEL_RUNNING"
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
kernel_interactive_menu >"$sandbox/nocollect-status.txt" 2>&1 || true
[[ "$COLLECT_CALLS" -eq 0 ]] \
    || fail 'the menu must not re-collect before a mutation (invalid answer + abort)'
[[ "$(cat "$NOCOLLECT_PROMPT_COUNT_FILE")" -eq 2 ]] \
    || fail 'the menu must re-prompt once after an invalid answer, then abort'
grep -q '^Repair change status kernels: unchanged|reason:kernels-aborted$' "$sandbox/nocollect-status.txt" \
    || fail 'an aborted kernel menu must run no action and report kernels-aborted (unchanged)'
BOOT_REPAIR_SESSION_PROTOCOL=0
KERNEL_BASELINE_B64=""

# ---- apk kernel inventory: healthy Alpine -> no false orphans/missing --------
# On apk, ownership is mapped read-only through `apk info --who-owns` (never the
# dpkg path map), so a healthy Alpine kernel (linux-lts owning vmlinuz-lts + its
# module tree, linux-lts-dev installed) must never be flagged orphaned or
# missing headers.
# Re-source the helper to restore the real kernel_inventory_collect/diagnostic_kernel
# definitions (the menu test above replaced collect with a call counter).  The
# re-source resets the helper's own top-level globals, so re-establish the
# harness environment (TARGET_ROOT/session state) before any collect.
source <(sed '/^main "$@"/d' "$HELPER")
TARGET_ROOT="$fake_root"
RUNNING_HOST_MODE=0
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"
running_kernel_version() { printf '%s\n' "${FAKE_RUNNING-6.18.55-0-lts}"; }
profile_target_backends() {
    TARGET_DISTRO_FAMILY=alpine
    TARGET_PACKAGE_MANAGERS=(apk)
    TARGET_PACKAGE_MANAGER=apk
    TARGET_SERVICE_MANAGERS=(openrc)
    TARGET_SERVICE_MANAGER=openrc
    TARGET_DISPLAY_BACKEND=openrc
    TARGET_INITRAMFS_BACKENDS=(mkinitfs)
    TARGET_INITRAMFS_BACKEND=mkinitfs
    TARGET_LOGGING_BACKEND=syslog
    TARGET_BOOTLOADER_BACKEND=syslinux/extlinux
    TARGET_ESP_MOUNT=unresolved
    TARGET_KERNEL_LAYOUT='flavor vmlinuz-lts/virt kernels'
    TARGET_REPAIR_BACKEND='Alpine/apk'
}
FAKE_RUNNING=6.18.55-0-lts
DPKG_PATH_OWNER_INDEX=""
rm -rf "$fake_root/lib/modules" "$fake_root/usr/lib/modules" "$fake_root/boot" "$fake_root/lib/apk" "$fake_root/etc/apk" "$fake_root/sbin"
mkdir -p "$fake_root/lib/modules/6.18.55-0-lts" "$fake_root/boot" "$fake_root/lib/apk/db" "$fake_root/etc/apk" "$fake_root/sbin"
printf '%s\n' '-lts' > "$fake_root/lib/modules/6.18.55-0-lts/kernel-suffix"
touch "$fake_root/boot/vmlinuz-lts" "$fake_root/boot/initramfs-lts"
# apk backend prerequisites (so package_backend_unavailable_reason kernel-cleanup
# apk reports runnable for the dispatch test below).
printf 'https://dl-cdn.alpinelinux.org/alpine/v3.20/main\n' > "$fake_root/etc/apk/repositories"
: > "$fake_root/lib/apk/db/installed"
: > "$fake_root/etc/apk/world"
touch "$fake_root/sbin/apk"
chmod +x "$fake_root/sbin/apk"

# Emulate the read-only `apk info --who-owns` ownership query inside the chroot.
run_selected_chroot() {
    local arg path
    for arg in "$@"; do
        [[ "$arg" == /* ]] || continue
        path="$arg"
        case "$path" in
            /lib/modules/*|/usr/lib/modules/*|/boot/vmlinuz-*)
                printf '%s is owned by linux-lts-6.18.55-r0\n' "$path"
                ;;
        esac
    done
    return 0
}
# Emulate the read-only apk installed-database probe: linux-lts and linux-lts-dev
# are installed, nothing else is.
target_apk_package_installed() {
    case "$1" in
        linux-lts|linux-lts-dev) return 0 ;;
        *) return 1 ;;
    esac
}

kernel_inventory_collect
[[ "${#KERNEL_INVENTORY_VERSIONS[@]}" -eq 1 ]] \
    || fail "apk inventory must map one kernel: ${KERNEL_INVENTORY_VERSIONS[*]}"
[[ "${KERNEL_INVENTORY_PKGS[0]}" == linux-lts ]] \
    || fail "apk ownership must map the kernel to linux-lts: ${KERNEL_INVENTORY_PKGS[0]}"
[[ "${#KERNEL_ORPHANED_KERNELS[@]}" -eq 0 ]] \
    || fail "apk must not flag a healthy kernel as orphaned: ${KERNEL_ORPHANED_KERNELS[*]}"
[[ "${#KERNEL_MISSING_HEADERS[@]}" -eq 0 ]] \
    || fail "apk with linux-lts-dev installed must report no missing headers: ${KERNEL_MISSING_HEADERS[*]}"
[[ "${#KERNEL_PARTIAL[@]}" -eq 0 ]] \
    || fail "apk must not report a healthy kernel as partial: ${KERNEL_PARTIAL[*]}"

# The diagnostic evidence reflects the apk inventory correctly.
output="$(diagnostic_kernel || true)"
grep -q '^Kernel inventory: total=1 protected=1 removable=0 backend=apk$' <<<"$output" \
    || fail 'apk kernel inventory summary wrong'
grep -q '^Kernel installed: 6.18.55-0-lts .* pkgs=linux-lts .*$' <<<"$output" \
    || fail 'apk installed evidence line must carry the linux-lts package'

# ---- apk kernel inventory: linux-lts-dev absent -> missing headers -----------
target_apk_package_installed() {
    case "$1" in
        linux-lts) return 0 ;;
        *) return 1 ;;
    esac
}
kernel_inventory_collect
[[ "${KERNEL_MISSING_HEADERS[*]}" == 6.18.55-0-lts ]] \
    || fail "apk missing headers must name the protected kernel exactly once: ${KERNEL_MISSING_HEADERS[*]}"
[[ "${#KERNEL_ORPHANED_KERNELS[@]}" -eq 0 ]] \
    || fail "apk must not flag the headers-less kernel as orphaned: ${KERNEL_ORPHANED_KERNELS[*]}"

# ---- apk backend wiring: package_stage_backends returns apk ----------------
backends="$(package_stage_backends kernel-cleanup)"
grep -qx 'apk' <<<"$backends" \
    || fail "package_stage_backends kernel-cleanup must return apk: $backends"

# ---- apk kernel-removal validator: superseded accepted, protected/non-kernel refused
# Set up a two-kernel inventory: 6.18.55-0-lts (superseded) + 6.18.56-0-virt
# (running == latest).  The validator must accept a Purging row for the
# superseded flavor package and refuse a protected flavor or any non-kernel
# package or unrequested kernel.
KERNEL_INVENTORY_VERSIONS=(6.18.55-0-lts 6.18.56-0-virt)
KERNEL_INVENTORY_PKGS=(linux-lts linux-virt)
KERNEL_INVENTORY_SIZES=(0 0)
KERNEL_RUNNING=6.18.56-0-virt
KERNEL_LATEST=6.18.56-0-virt
KERNEL_REMOVABLE=(6.18.55-0-lts)
KERNEL_REMOVABLE_PKGS=(linux-lts)
KERNEL_LATEST_PKGS=(linux-virt)
KERNEL_STALE=()
KERNEL_STALE_PKGS=()
KERNEL_REMOVE_MODE=oldest

alpine_apk_kernel_removal_is_safe '(1/1) Purging linux-lts (6.18.55-r0)' 0 linux-lts \
    || fail 'the apk validator must accept a superseded kernel flavor removal'
alpine_apk_kernel_removal_is_safe '(1/1) Purging linux-virt (6.18.56-r0)' 0 linux-lts \
    && fail 'the apk validator must refuse an unrequested kernel flavor'
alpine_apk_kernel_removal_is_safe '(1/1) Purging musl (1.2.5-r0)' 0 linux-lts \
    && fail 'the apk validator must refuse a non-kernel package'
alpine_apk_kernel_removal_is_safe '(1/1) Purging linux-virt (6.18.56-r0)' 0 linux-virt \
    && fail 'the apk validator must refuse a protected kernel flavor'
alpine_apk_kernel_removal_is_safe '(1/2) Purging linux-lts (6.18.55-r0)
(2/2) Purging linux-lts-dev (6.18.55-r0)' 0 linux-lts \
    && fail 'the apk validator must refuse a dependency cascade (linux-lts-dev)'
alpine_apk_kernel_removal_is_safe '(1/1) Purging linux-lts (6.18.55-r0)' 1 linux-lts \
    && fail 'the apk validator must fail closed on a non-zero simulation exit code'
KERNEL_REMOVE_MODE=""

# ---- apk kernel-cleanup stage: simulation-first, guarded apply -----------------
# The stage simulates `apk del` before applying it and asserts the protected set
# survived; the simulated Purging row runs through the real validator.
apk_sim_log="$sandbox/apk-sim.log"
apk_apply_log="$sandbox/apk-apply.log"
: > "$apk_sim_log"
: > "$apk_apply_log"
KERNEL_REMOVE_MODE=oldest
KERNEL_STAGE_STATUS_KEY=kernels
KERNEL_CLEANUP_APPLIED=false
alpine_apk_preflight() { :; }
alpine_apk_fingerprint() { printf 'stub-fingerprint\n'; }
alpine_apk_transaction_try() {
    printf 'simulate:%s\n' "$*" >> "$apk_sim_log"
    APK_SIM_OUTPUT='(1/1) Purging linux-lts (6.18.55-r0)'
    APK_SIM_RC=0
    return 0
}
alpine_apk_transaction_reported_no_changes() { return 1; }
run_chroot_try_display() {
    local label="$1"; shift
    printf 'apply:%s\n' "$*" >> "$apk_apply_log"
    CHROOT_TRY_RC=0
    CHROOT_TRY_OUTPUT=''
    return 0
}
target_apk_package_installed() { return 0; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
# Run in the current shell (not a command substitution) so KERNEL_CLEANUP_APPLIED
# propagates back for the applied assertion below.
apk_kernel_cleanup_stage > "$sandbox/apk-stage-status.txt"
status_line="$(cat "$sandbox/apk-stage-status.txt")"
grep -q '^Repair change status kernels: changed$' <<<"$status_line" \
    || fail "apk kernel-cleanup stage must report changed: $status_line"
grep -q 'simulate:.*del linux-lts .*--simulate' "$apk_sim_log" \
    || fail "apk kernel-cleanup stage must simulate apk del first: $(tr '\n' ',' < "$apk_sim_log")"
grep -q 'apply:apk del linux-lts' "$apk_apply_log" \
    || fail "apk kernel-cleanup stage must apply the simulated apk del: $(tr '\n' ',' < "$apk_apply_log")"
[[ "$KERNEL_CLEANUP_APPLIED" == true ]] \
    || fail 'apk kernel-cleanup stage must mark the removal applied'
KERNEL_REMOVE_MODE=""

# ---- apk kernel_cleanup_stage dispatch ---------------------------------------
# kernel_cleanup_stage dispatches to apk_kernel_cleanup_stage when apk is the
# runnable backend (stubbed marker proves the dispatch case is reached).  Runs
# last because it replaces apk_kernel_cleanup_stage with a marker stub.
APK_STAGE_DISPATCHED=0
apk_kernel_cleanup_stage() { APK_STAGE_DISPATCHED=1; }
KERNEL_REMOVE_MODE=oldest
KERNEL_STAGE_STATUS_KEY=kernels
kernel_cleanup_stage
[[ "$APK_STAGE_DISPATCHED" -eq 1 ]] \
    || fail 'kernel_cleanup_stage did not dispatch to apk_kernel_cleanup_stage'
KERNEL_REMOVE_MODE=""

# ---- apk partial-kernel reinstall completes the missing image package --------
# A partial kernel whose only owning package is the linux-<flavor>-dev headers
# package (version skew: the dev package is newer than the image) must be
# completed by installing the missing IMAGE package linux-<flavor>, never by
# reinstalling the already-installed headers (a no-op that looped the menu).
apk_reinstall_log="$sandbox/apk-reinstall.log"
: > "$apk_reinstall_log"
guarded_apk_install_packages() {
    printf 'install:%s\n' "$*" >> "$apk_reinstall_log"
    return 0
}
guarded_apk_upgrade_packages() {
    printf 'upgrade:%s\n' "$*" >> "$apk_reinstall_log"
    return 0
}
# The missing image package is NOT installed: a plain guarded install completes it.
target_apk_package_installed() { return 1; }
KERNEL_INVENTORY_VERSIONS=(6.18.55-0-lts)
KERNEL_INVENTORY_PKGS=(linux-lts-dev)
KERNEL_PARTIAL=(6.18.55-0-lts)
apk_repair_kernel_reinstall \
    || fail 'apk reinstall of a headers-only partial kernel must install the missing image package'
grep -q '^install:linux-lts$' "$apk_reinstall_log" \
    || fail "apk reinstall must install linux-lts, got: $(tr '\n' ',' < "$apk_reinstall_log")"
grep -q 'linux-lts-dev' "$apk_reinstall_log" \
    && fail 'apk reinstall must never reinstall the linux-lts-dev headers package'

# The image package is already installed (version skew: linux-lts older than the
# -dev headers that own the newer module tree): a plain `apk add` would be a
# no-op, so completing the partial kernel must UPGRADE the image package.
: > "$apk_reinstall_log"
target_apk_package_installed() {
    case "$1" in linux-lts) return 0 ;; *) return 1 ;; esac
}
KERNEL_INVENTORY_PKGS=(linux-lts)
apk_repair_kernel_reinstall \
    || fail 'apk reinstall of an image-owning partial kernel must upgrade the installed image package'
grep -q '^upgrade:linux-lts$' "$apk_reinstall_log" \
    || fail "apk reinstall must UPGRADE the installed linux-lts (apk add -u), got: $(tr '\n' ',' < "$apk_reinstall_log")"
grep -q '^install:linux-lts$' "$apk_reinstall_log" \
    && fail 'apk reinstall must not plain-add an already-installed image package'

# The version-skewed form: the owning package is the -dev headers, and the image
# package it resolves to is already installed at an older version.  It must go
# through the UPGRADE path, never the plain no-op add.
: > "$apk_reinstall_log"
KERNEL_INVENTORY_PKGS=(linux-lts-dev)
apk_repair_kernel_reinstall \
    || fail 'apk reinstall of a version-skewed partial kernel must upgrade the image package'
grep -q '^upgrade:linux-lts$' "$apk_reinstall_log" \
    || fail "version-skewed apk reinstall must upgrade linux-lts, got: $(tr '\n' ',' < "$apk_reinstall_log")"
grep -q 'linux-lts-dev' "$apk_reinstall_log" \
    && fail 'version-skewed apk reinstall must never reinstall the linux-lts-dev headers package'


# ---- pacman kernel inventory: healthy Arch -> correct ownership, no orphans --
# On pacman, ownership is mapped read-only through `pacman -Qo` (never the dpkg
# path map), so a healthy Arch kernel (linux owning vmlinuz-linux + its module
# tree, linux-lts owning the lts flavor) must map its packages and never be
# flagged orphaned or missing headers.
source <(sed '/^main "$@"/d' "$HELPER")
TARGET_ROOT="$fake_root"
RUNNING_HOST_MODE=0
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"
running_kernel_version() { printf '%s\n' "${FAKE_RUNNING-6.1.2-lts-1}"; }
profile_target_backends() {
    TARGET_DISTRO_FAMILY=arch
    TARGET_PACKAGE_MANAGERS=(pacman)
    TARGET_PACKAGE_MANAGER=pacman
    TARGET_SERVICE_MANAGERS=(systemd)
    TARGET_SERVICE_MANAGER=systemd
    TARGET_DISPLAY_BACKEND=systemd
    TARGET_INITRAMFS_BACKENDS=(mkinitcpio)
    TARGET_INITRAMFS_BACKEND=mkinitcpio
    TARGET_LOGGING_BACKEND=journald
    TARGET_BOOTLOADER_BACKEND=grub
    TARGET_ESP_MOUNT=unresolved
    TARGET_KERNEL_LAYOUT='named vmlinuz-linux kernels'
    TARGET_REPAIR_BACKEND='Arch/pacman'
}
FAKE_RUNNING=6.1.2-lts-1
DPKG_PATH_OWNER_INDEX=""
reset_fixture
rm -rf "$fake_root/lib/apk" "$fake_root/etc/apk" "$fake_root/sbin" \
       "$fake_root/var/lib/pacman" "$fake_root/etc/pacman.conf"
rm -f "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5" "$fake_root/usr/bin/pacman"
mkdir -p "$fake_root/usr/lib/modules/6.1.1-arch1-1" "$fake_root/usr/lib/modules/6.1.2-lts-1" \
         "$fake_root/boot" "$fake_root/var/lib/pacman"
touch "$fake_root/boot/vmlinuz-linux" "$fake_root/boot/vmlinuz-lts" \
      "$fake_root/boot/initramfs-linux.img" "$fake_root/boot/initramfs-lts.img"
# pacman backend prerequisites (so package_backend_unavailable_reason
# kernel-cleanup pacman reports runnable for the dispatch test below).
touch "$fake_root/usr/bin/pacman"
chmod +x "$fake_root/usr/bin/pacman"
printf '[options]\n' > "$fake_root/etc/pacman.conf"

# Emulate the read-only `pacman -Qo` ownership query inside the chroot: the
# linux and linux-lts module trees map to their owning packages.
run_selected_chroot() {
    local arg path doing_qo=false
    for arg in "$@"; do
        [[ "$arg" == -Qo ]] && doing_qo=true
    done
    if [[ "$doing_qo" == true ]]; then
        for arg in "$@"; do
            [[ "$arg" == /* ]] || continue
            path="$arg"
            case "$path" in
                */6.1.1-arch1-1*)
                    printf '%s is owned by linux 6.1.1.arch1-1\n' "$path"
                    ;;
                */6.1.2-lts-1*)
                    printf '%s is owned by linux-lts 6.1.2.lts-1\n' "$path"
                    ;;
            esac
        done
        return 0
    fi
    return 0
}

kernel_inventory_collect
[[ "${#KERNEL_INVENTORY_VERSIONS[@]}" -eq 2 ]] \
    || fail "pacman inventory must map two kernels: ${KERNEL_INVENTORY_VERSIONS[*]}"
[[ "${KERNEL_INVENTORY_PKGS[0]}" == linux ]] \
    || fail "pacman ownership must map the older kernel to linux: ${KERNEL_INVENTORY_PKGS[0]}"
[[ "${KERNEL_INVENTORY_PKGS[1]}" == linux-lts ]] \
    || fail "pacman ownership must map the newer kernel to linux-lts: ${KERNEL_INVENTORY_PKGS[1]}"
[[ "${#KERNEL_ORPHANED_KERNELS[@]}" -eq 0 ]] \
    || fail "pacman must not flag a healthy kernel as orphaned: ${KERNEL_ORPHANED_KERNELS[*]}"
[[ "${#KERNEL_MISSING_HEADERS[@]}" -eq 0 ]] \
    || fail "pacman must never flag missing headers: ${KERNEL_MISSING_HEADERS[*]}"
[[ "${#KERNEL_PARTIAL[@]}" -eq 0 ]] \
    || fail "pacman must not report a healthy kernel as partial: ${KERNEL_PARTIAL[*]}"
[[ "${#KERNEL_METAPACKAGES[@]}" -eq 0 && "${#KERNEL_MANAGED[@]}" -eq 0 ]] \
    || fail "pacman must keep the metapackage/managed sets empty: ${KERNEL_METAPACKAGES[*]} ${KERNEL_MANAGED[*]}"

# ---- pacman backend wiring: package_stage_backends returns pacman -----------
backends="$(package_stage_backends kernel-cleanup)"
grep -qx 'pacman' <<<"$backends" \
    || fail "package_stage_backends kernel-cleanup must return pacman: $backends"

# ---- pacman kernel-removal validator: superseded accepted, others refused ---
KERNEL_INVENTORY_VERSIONS=(6.1.1-arch1-1 6.1.2-lts-1)
KERNEL_INVENTORY_PKGS=(linux linux-lts)
KERNEL_INVENTORY_SIZES=(0 0)
KERNEL_RUNNING=6.1.2-lts-1
KERNEL_LATEST=6.1.2-lts-1
KERNEL_REMOVABLE=(6.1.1-arch1-1)
KERNEL_REMOVABLE_PKGS=(linux)
KERNEL_LATEST_PKGS=(linux-lts)
KERNEL_STALE=()
KERNEL_STALE_PKGS=()
KERNEL_REMOVE_MODE=oldest

pacman_kernel_removal_is_safe 'Packages (1) linux-6.1.1.arch1-1' 0 linux \
    || fail 'the pacman validator must accept a superseded kernel removal'
pacman_kernel_removal_is_safe 'Packages (1) linux-lts-6.1.2.lts-1' 0 linux \
    && fail 'the pacman validator must refuse an unrequested kernel package'
pacman_kernel_removal_is_safe 'Packages (1) musl-1.2.5-1' 0 linux \
    && fail 'the pacman validator must refuse a non-kernel package'
pacman_kernel_removal_is_safe 'Packages (1) linux-lts-6.1.2.lts-1' 0 linux-lts \
    && fail 'the pacman validator must refuse a protected kernel package'
pacman_kernel_removal_is_safe 'Packages (1) linux-6.1.1.arch1-1' 1 linux \
    && fail 'the pacman validator must fail closed on a non-zero simulation exit code'

# ---- pacman kernel-cleanup stage: simulation-first, guarded apply ------------
# The stage simulates `pacman -R --print` inside the sandbox before applying the
# real chroot `pacman --noconfirm -R`, then asserts the protected set survived;
# the simulated Packages row runs through the real validator.
pacman_sim_log="$sandbox/pacman-sim.log"
pacman_apply_log="$sandbox/pacman-apply.log"
: > "$pacman_sim_log"
: > "$pacman_apply_log"
KERNEL_REMOVE_MODE=oldest
KERNEL_STAGE_STATUS_KEY=kernels
KERNEL_CLEANUP_APPLIED=false
arch_pacman_prepare_sandbox() { :; }
arch_pacman_transaction_try() {
    local label="$1"; shift
    printf 'simulate:%s\n' "$*" >> "$pacman_sim_log"
    ARCH_PACMAN_TRY_OUTPUT='Packages (1) linux-6.1.1.arch1-1'
    ARCH_PACMAN_TRY_RC=0
    return 0
}
run_chroot_try_display() {
    local label="$1"; shift
    printf 'apply:%s\n' "$*" >> "$pacman_apply_log"
    CHROOT_TRY_RC=0
    CHROOT_TRY_OUTPUT=''
    return 0
}
pacman_kernel_protected_set_present() { return 0; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
# Run in the current shell (not a command substitution) so KERNEL_CLEANUP_APPLIED
# propagates back for the applied assertion below.
pacman_kernel_cleanup_stage > "$sandbox/pacman-stage-status.txt"
status_line="$(cat "$sandbox/pacman-stage-status.txt")"
grep -q '^Repair change status kernels: changed$' <<<"$status_line" \
    || fail "pacman kernel-cleanup stage must report changed: $status_line"
grep -q 'simulate:.*-R --print linux' "$pacman_sim_log" \
    || fail "pacman kernel-cleanup stage must simulate pacman -R --print first: $(tr '\n' ',' < "$pacman_sim_log")"
grep -q 'apply:pacman --noconfirm -R linux' "$pacman_apply_log" \
    || fail "pacman kernel-cleanup stage must apply the simulated pacman -R: $(tr '\n' ',' < "$pacman_apply_log")"
[[ "$KERNEL_CLEANUP_APPLIED" == true ]] \
    || fail 'pacman kernel-cleanup stage must mark the removal applied'
KERNEL_REMOVE_MODE=""

# ---- pacman kernel-cleanup no-op: empty removal set reports no-op -------------
# With only a single (protected) kernel installed there is nothing to remove; the
# stage must report no-removable-kernels, never changed.
KERNEL_INVENTORY_VERSIONS=(6.1.2-lts-1)
KERNEL_INVENTORY_PKGS=(linux-lts)
KERNEL_INVENTORY_SIZES=(0)
KERNEL_RUNNING=6.1.2-lts-1
KERNEL_LATEST=6.1.2-lts-1
KERNEL_REMOVABLE=()
KERNEL_REMOVABLE_PKGS=()
KERNEL_LATEST_PKGS=(linux-lts)
KERNEL_STALE=()
KERNEL_STALE_PKGS=()
KERNEL_REMOVE_MODE=oldest
KERNEL_STAGE_STATUS_KEY=kernels
KERNEL_CLEANUP_APPLIED=false
pacman_kernel_cleanup_stage > "$sandbox/pacman-noop.txt"
status_line="$(cat "$sandbox/pacman-noop.txt")"
grep -q '^Repair change status kernels: unchanged|reason:no-removable-kernels$' <<<"$status_line" \
    || fail "pacman kernel-cleanup stage must no-op on an empty removal set: $status_line"
[[ "$KERNEL_CLEANUP_APPLIED" == false ]] \
    || fail 'pacman kernel-cleanup stage must not mark an empty removal applied'
KERNEL_REMOVE_MODE=""

# ---- pacman kernel_cleanup_stage dispatch -------------------------------------
# kernel_cleanup_stage dispatches to pacman_kernel_cleanup_stage when pacman is
# the runnable backend (stubbed marker proves the dispatch case is reached).
# Runs last because it replaces pacman_kernel_cleanup_stage with a marker stub.
PACMAN_STAGE_DISPATCHED=0
pacman_kernel_cleanup_stage() { PACMAN_STAGE_DISPATCHED=1; }
KERNEL_REMOVE_MODE=oldest
KERNEL_STAGE_STATUS_KEY=kernels
kernel_cleanup_stage
[[ "$PACMAN_STAGE_DISPATCHED" -eq 1 ]] \
    || fail 'kernel_cleanup_stage did not dispatch to pacman_kernel_cleanup_stage'
KERNEL_REMOVE_MODE=""

# ---- pacman kernel zombie: half-removed detection ---------------------------
# pacman_kernel_is_half_removed marks a kernel half-removed only on positive
# evidence: its module tree is already gone (positive evidence a), or one of its
# grouped packages is no longer installed via `pacman -Q` (positive evidence b).
# A fully intact kernel (module tree present + every grouped package installed)
# is NOT half-removed (a complete removal failure), so the destructive sweep is
# skipped.
reset_fixture
mkdir -p "$fake_root/usr/lib/modules/6.1.1-arch1-1" \
         "$fake_root/usr/lib/modules/6.1.2-lts-1"
run_selected_chroot() {
    local -a cmd=("$@")
    if [[ "${cmd[*]}" == *" pacman -Q "* ]]; then
        local pkg="${cmd[${#cmd[@]}-1]}"
        [[ "$pkg" == linux-old ]] && return 1
        return 0
    fi
    return 0
}
pacman_kernel_is_half_removed 6.1.1-arch1-1 linux \
    && fail 'an intact pacman kernel (module tree + installed package) must not be half-removed'
pacman_kernel_is_half_removed 6.1.9-arch1-1 linux \
    || fail 'a pacman kernel whose module tree is gone must be half-removed'
pacman_kernel_is_half_removed 6.1.2-lts-1 linux-old \
    || fail 'a pacman kernel whose grouped package is not installed must be half-removed'

# ---- pacman kernel zombie: cleanup removes module tree + pkgbase boot files --
# pacman_kernel_zombie_cleanup removes the module tree and the pkgbase-named
# vmlinuz/initramfs images, and leaves a protected (different) kernel untouched.
reset_fixture
mkdir -p "$fake_root/usr/lib/modules/6.1.1-arch1-1" \
         "$fake_root/usr/lib/modules/6.1.2-lts-1" "$fake_root/boot"
printf '%s\n' 'linux' > "$fake_root/usr/lib/modules/6.1.1-arch1-1/pkgbase"
printf '%s\n' 'linux-lts' > "$fake_root/usr/lib/modules/6.1.2-lts-1/pkgbase"
touch "$fake_root/usr/lib/modules/6.1.1-arch1-1/kernel.ko" \
      "$fake_root/usr/lib/modules/6.1.2-lts-1/kernel.ko" \
      "$fake_root/boot/vmlinuz-linux" "$fake_root/boot/initramfs-linux.img" \
      "$fake_root/boot/initramfs-linux-fallback.img" \
      "$fake_root/boot/vmlinuz-linux-lts" "$fake_root/boot/initramfs-linux-lts.img" \
      "$fake_root/boot/initramfs-linux-lts-fallback.img"
pacman_kernel_zombie_cleanup 6.1.1-arch1-1
[[ ! -d "$fake_root/usr/lib/modules/6.1.1-arch1-1" ]] \
    || fail 'pacman zombie cleanup must remove the module tree'
[[ ! -f "$fake_root/boot/vmlinuz-linux" ]] \
    || fail 'pacman zombie cleanup must remove the pkgbase-named vmlinuz'
[[ ! -f "$fake_root/boot/initramfs-linux.img" ]] \
    || fail 'pacman zombie cleanup must remove the pkgbase-named initramfs'
[[ ! -f "$fake_root/boot/initramfs-linux-fallback.img" ]] \
    || fail 'pacman zombie cleanup must remove the pkgbase-named fallback initramfs'
# The protected kernel's module tree and pkgbase-named boot files are untouched.
[[ -d "$fake_root/usr/lib/modules/6.1.2-lts-1" ]] \
    || fail 'pacman zombie cleanup must not remove a protected kernel module tree'
[[ -f "$fake_root/boot/vmlinuz-linux-lts" ]] \
    || fail 'pacman zombie cleanup must not remove a protected kernel vmlinuz'
[[ -f "$fake_root/boot/initramfs-linux-lts.img" ]] \
    || fail 'pacman zombie cleanup must not remove a protected kernel initramfs'

# ---- pacman kernel zombie: no pkgbase file removes only the module tree -----
# Without a readable pkgbase file the cleanup must fail closed: it removes only
# the module tree and never guesses a flavour string that could delete a
# different kernel's vmlinuz.
reset_fixture
mkdir -p "$fake_root/usr/lib/modules/6.1.9-arch1-1" "$fake_root/boot"
touch "$fake_root/usr/lib/modules/6.1.9-arch1-1/kernel.ko" \
      "$fake_root/boot/vmlinuz-6.1.9-arch1-1"
pacman_kernel_zombie_cleanup 6.1.9-arch1-1
[[ ! -d "$fake_root/usr/lib/modules/6.1.9-arch1-1" ]] \
    || fail 'pacman zombie cleanup without a pkgbase file must still remove the module tree'
[[ -f "$fake_root/boot/vmlinuz-6.1.9-arch1-1" ]] \
    || fail 'pacman zombie cleanup without a pkgbase file must never remove a guessed boot image'

# ---- pacman partial kernel: package installed, module tree gone ------------
# A pacman kernel PACKAGE (linux) is still installed (`pacman -Q` lists it and
# `pacman -Ql` still owns a /usr/lib/modules/<dir> entry) but the module tree is
# absent on disk (interrupted `pacman -S` / manual delete).  The partial probe
# must flag the version in KERNEL_PARTIAL, record the owning package in
# KERNEL_PARTIAL_PKGS, and repair_kernel_reinstall must reinstall the OWNING
# package through guarded_pacman_install_packages and report changed.
reset_fixture
rm -rf "$fake_root/lib/apk" "$fake_root/etc/apk" "$fake_root/sbin" \
       "$fake_root/var/lib/pacman" "$fake_root/etc/pacman.conf"
rm -f "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5" "$fake_root/usr/bin/pacman"
mkdir -p "$fake_root/var/lib/pacman" "$fake_root/boot"
touch "$fake_root/usr/bin/pacman"
chmod +x "$fake_root/usr/bin/pacman"
printf '[options]\n' > "$fake_root/etc/pacman.conf"
FAKE_RUNNING=""
profile_target_backends() {
    TARGET_DISTRO_FAMILY=arch
    TARGET_PACKAGE_MANAGERS=(pacman)
    TARGET_PACKAGE_MANAGER=pacman
    TARGET_SERVICE_MANAGERS=(systemd)
    TARGET_SERVICE_MANAGER=systemd
    TARGET_DISPLAY_BACKEND=systemd
    TARGET_INITRAMFS_BACKENDS=(mkinitcpio)
    TARGET_INITRAMFS_BACKEND=mkinitcpio
    TARGET_LOGGING_BACKEND=journald
    TARGET_BOOTLOADER_BACKEND=grub
    TARGET_ESP_MOUNT=unresolved
    TARGET_KERNEL_LAYOUT='named vmlinuz-linux kernels'
    TARGET_REPAIR_BACKEND='Arch/pacman'
}
# `pacman -Q` lists the linux package; `pacman -Ql linux` owns a module tree
# whose directory is NOT created on disk (the partial state).
run_selected_chroot() {
    local -a cmd=("$@")
    if [[ "${cmd[*]}" == *" -Ql "* ]]; then
        printf 'linux /usr/lib/modules/6.1.1-arch1-1/\n'
        printf 'linux /usr/lib/modules/6.1.1-arch1-1/kernel/\n'
        return 0
    fi
    if [[ "${cmd[*]}" == *" -Q"* ]]; then
        # `pacman -Q` (list) or `pacman -Q <pkg>` (installed check)
        printf 'linux 6.1.1.arch1-1\n'
        return 0
    fi
    return 0
}
kernel_inventory_collect
grep -q '6.1.1-arch1-1' <<<"${KERNEL_PARTIAL[*]}" \
    || fail "pacman must flag an installed package whose module tree is missing as partial: ${KERNEL_PARTIAL[*]}"
grep -q 'linux' <<<"${KERNEL_PARTIAL_PKGS[*]}" \
    || fail "pacman partial must record the owning package: ${KERNEL_PARTIAL_PKGS[*]}"
PACMAN_REINSTALL_LOG="$sandbox/pacman-reinstall.log"
: > "$PACMAN_REINSTALL_LOG"
guarded_pacman_install_packages() {
    printf '%s\n' "$*" >> "$PACMAN_REINSTALL_LOG"
    return 0
}
adaptive_initramfs_repair() { :; }
adaptive_grub_stage() { :; }
adaptive_extlinux_repair() { :; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_reinstall)"
grep -q '^Repair change status kernels:reinstall: changed$' <<<"$status_line" \
    || fail "pacman reinstall must report changed: $status_line"
grep -qx 'linux' "$PACMAN_REINSTALL_LOG" \
    || fail "pacman reinstall must reinstall the owning package linux, got: $(tr '\n' ',' < "$PACMAN_REINSTALL_LOG")"

# ---- pacman stale probe: unambiguous not-found vs missing sync DB ----------
# A non-running pacman kernel is classified stale ONLY when `pacman -Si <pkg>`
# unambiguously reports the package as not found; a missing/absent sync DB (any
# other error) must never classify stale (fail closed).  The owning package is
# resolved from the module-tree owner, never a version-string guess.
reset_fixture
rm -rf "$fake_root/lib/apk" "$fake_root/etc/apk" "$fake_root/sbin" \
       "$fake_root/var/lib/pacman" "$fake_root/etc/pacman.conf"
rm -f "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5" "$fake_root/usr/bin/pacman"
mkdir -p "$fake_root/usr/lib/modules/6.1.1-arch1-1" "$fake_root/usr/lib/modules/6.1.2-lts-1" \
         "$fake_root/var/lib/pacman" "$fake_root/boot"
touch "$fake_root/usr/bin/pacman"
chmod +x "$fake_root/usr/bin/pacman"
printf '[options]\n' > "$fake_root/etc/pacman.conf"
FAKE_RUNNING=6.1.2-lts-1
profile_target_backends() {
    TARGET_DISTRO_FAMILY=arch
    TARGET_PACKAGE_MANAGERS=(pacman)
    TARGET_PACKAGE_MANAGER=pacman
    TARGET_SERVICE_MANAGERS=(systemd)
    TARGET_SERVICE_MANAGER=systemd
    TARGET_DISPLAY_BACKEND=systemd
    TARGET_INITRAMFS_BACKENDS=(mkinitcpio)
    TARGET_INITRAMFS_BACKEND=mkinitcpio
    TARGET_LOGGING_BACKEND=journald
    TARGET_BOOTLOADER_BACKEND=grub
    TARGET_ESP_MOUNT=unresolved
    TARGET_KERNEL_LAYOUT='named vmlinuz-linux kernels'
    TARGET_REPAIR_BACKEND='Arch/pacman'
}
# Sub-case 1: `pacman -Si linux` unambiguously reports not found -> stale.
run_selected_chroot() {
    local -a cmd=("$@")
    if [[ "${cmd[*]}" == *" -Qo "* ]]; then
        for a in "${cmd[@]}"; do
            [[ "$a" == /* ]] || continue
            case "$a" in
                */6.1.1-arch1-1*) printf '%s is owned by linux 6.1.1.arch1-1\n' "$a" ;;
                */6.1.2-lts-1*)   printf '%s is owned by linux-lts 6.1.2.lts-1\n' "$a" ;;
            esac
        done
        return 0
    fi
    if [[ "${cmd[*]}" == *" -Si "* ]]; then
        printf "error: package 'linux' was not found\n"
        return 1
    fi
    return 0
}
kernel_inventory_collect
grep -q '6.1.1-arch1-1' <<<"${KERNEL_STALE[*]}" \
    || fail "pacman must classify a non-running kernel stale on an unambiguous not found: ${KERNEL_STALE[*]}"
grep -q 'linux' <<<"${KERNEL_STALE_PKGS[*]}" \
    || fail "pacman stale must record the owning package in the flat plan: ${KERNEL_STALE_PKGS[*]}"
# Sub-case 2: `pacman -Si linux` fails with no output (missing sync DB) -> never stale.
run_selected_chroot() {
    local -a cmd=("$@")
    if [[ "${cmd[*]}" == *" -Qo "* ]]; then
        for a in "${cmd[@]}"; do
            [[ "$a" == /* ]] || continue
            case "$a" in
                */6.1.1-arch1-1*) printf '%s is owned by linux 6.1.1.arch1-1\n' "$a" ;;
                */6.1.2-lts-1*)   printf '%s is owned by linux-lts 6.1.2.lts-1\n' "$a" ;;
            esac
        done
        return 0
    fi
    if [[ "${cmd[*]}" == *" -Si "* ]]; then
        return 1
    fi
    return 0
}
kernel_inventory_collect
[[ "${#KERNEL_STALE[@]}" -eq 0 ]] \
    || fail "pacman must never classify stale on a missing sync DB: ${KERNEL_STALE[*]}"

# ---- pacman kernel_headers_installed: <pkg>-headers present/absent ---------
# kernel_headers_installed resolves the owning package via `pacman -Qo` of
# /usr/lib/modules/<v> and returns 0 when <pkg>-headers is installed (read-only
# `pacman -Q` succeeds) and 1 otherwise.
TARGET_PACKAGE_MANAGER=pacman
run_selected_chroot() {
    local -a cmd=("$@")
    if [[ "${cmd[*]}" == *" -Qo "* ]]; then
        printf '/usr/lib/modules/6.1.1-arch1-1 is owned by linux 6.1.1.arch1-1\n'
        return 0
    fi
    if [[ "${cmd[*]}" == *" -Q "* ]]; then
        [[ "${cmd[${#cmd[@]}-1]}" == linux-headers ]] && return 0
        return 1
    fi
    return 0
}
kernel_headers_installed 6.1.1-arch1-1 \
    || fail 'pacman kernel_headers_installed must return 0 when linux-headers is installed'
run_selected_chroot() {
    local -a cmd=("$@")
    if [[ "${cmd[*]}" == *" -Qo "* ]]; then
        printf '/usr/lib/modules/6.1.1-arch1-1 is owned by linux 6.1.1.arch1-1\n'
        return 0
    fi
    if [[ "${cmd[*]}" == *" -Q "* ]]; then
        return 1
    fi
    return 0
}
kernel_headers_installed 6.1.1-arch1-1 \
    && fail 'pacman kernel_headers_installed must return 1 when linux-headers is absent'

# ---- pacman kernel repo-latest: read-only pacman -Si, no sandbox/-Sy -------
# pacman_kernel_repo_latest is invoked via $(pacman_kernel_repo_latest) and must
# be READ-ONLY: it resolves the linux/linux-lts package version from the
# target's already-present sync DB (`pacman -Si`, never a `-Sy` refresh) and maps
# it to the module-tree string.  A sandbox prep or a -Sy transaction writes into
# $TARGET_ROOT/tmp and fails on a read-only diagnostic mount, so neither may be
# called; stdout stays exactly the mapped version (never a `msg:` leak).
reset_fixture
TARGET_PACKAGE_MANAGER=pacman
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"
PACMAN_SANDBOX_CALLED=0
arch_pacman_prepare_sandbox() { PACMAN_SANDBOX_CALLED=1; }
arch_pacman_transaction_try() { PACMAN_SANDBOX_CALLED=1; }
run_selected_chroot() {
    local last="${!#}"
    [[ "$last" == linux ]] && printf 'Version         : 7.2.9.arch1-1\n'
    return 0
}
captured="$(pacman_kernel_repo_latest)"
[[ "$captured" == "7.2.9-arch1-1" ]] \
    || fail "pacman_kernel_repo_latest stdout must be only the mapped version, got: $captured"
[[ "$PACMAN_SANDBOX_CALLED" -eq 0 ]] \
    || fail 'pacman_kernel_repo_latest must not prepare a sandbox or run -Sy'
! grep -q 'msg:' <<<"$captured" \
    || fail "pacman_kernel_repo_latest stdout must not leak msg_log output: $captured"
# Fail closed: no Version: line (sync DB absent) yields empty, never a guess.
run_selected_chroot() { return 1; }
captured="$(pacman_kernel_repo_latest || true)"
[[ -z "$captured" ]] \
    || fail "pacman_kernel_repo_latest must fail closed on a missing sync DB: $captured"

# ---- pacman host: refuse remove-newest when the running kernel is stale -----
# On Arch a `pacman -Syu` upgrade removes the old kernel's files, so the
# still-running kernel has no module tree and is absent from
# KERNEL_INVENTORY_VERSIONS; the newest is then the only inventory kernel.
# Removing it would leave zero bootable kernels, so kernel_newest_is_removable
# must fail closed (the running kernel is not a valid fallback).  On apt/dpkg,
# rpm and apk the running kernel is always in the inventory, so the new
# condition is a no-op there.
RUNNING_HOST_MODE=1
KERNEL_RUNNING=7.2.8-arch1-2          # stale: upgraded, not rebooted (no module tree)
KERNEL_LATEST=7.2.9-arch1-1
KERNEL_INVENTORY_VERSIONS=(7.2.9-arch1-1)   # newest is the ONLY inventory kernel
kernel_newest_is_removable \
    && fail 'pacman host must refuse newest removal when the running kernel is stale (absent from the inventory)'
# With the running kernel present in the inventory (the non-pacman shape), the
# host condition is unchanged: running != latest still allows removal.
KERNEL_INVENTORY_VERSIONS=(7.2.8-arch1-2 7.2.9-arch1-1)
kernel_newest_is_removable \
    || fail 'host with the running kernel in the inventory must still allow newest removal'
RUNNING_HOST_MODE=0

# ---- pacman verify: flavour-named vmlinuz resolved from the pkgbase file ---
# On Arch the boot image is flavour-named (vmlinuz-linux) and its name is the
# module tree's pkgbase file content, NOT `pacman -Qo` of the module-tree
# DIRECTORY: that query returns EVERY package owning the directory (linux AND
# linux-headers both own /usr/lib/modules/<v>/), so a scalar capture would be a
# newline-joined "linux\nlinux-headers" that never matches a vmlinuz path and
# wrongly falls through to the reinstall path.  A stale running kernel
# (upgraded, not yet rebooted) legitimately has no module tree and is skipped.
# Scenario A (repair TARGET): no running kernel, single latest 7.2.8-arch1-2.
reset_fixture
TARGET_PACKAGE_MANAGER=pacman
KERNEL_BASELINE_B64=""
KERNEL_RUNNING=""
KERNEL_LATEST=7.2.8-arch1-2
KERNEL_PARTIAL=()
KERNEL_ORPHANED_KERNELS=()
KERNEL_ORPHANED_HEADERS=()
KERNEL_ORPHANED_SUPPORT_VERSIONS=()
KERNEL_REPO_LATEST=""
mkdir -p "$fake_root/usr/lib/modules/7.2.8-arch1-2" "$fake_root/boot"
printf 'linux\n' > "$fake_root/usr/lib/modules/7.2.8-arch1-2/pkgbase"
touch "$fake_root/boot/vmlinuz-linux"
kernel_inventory_collect() { :; }
# Emulate the real `pacman -Qo` on the module-tree DIRECTORY (linux AND
# linux-headers both own it), which the verify must no longer rely on.
run_selected_chroot() {
    printf '%s\n' \
        '/usr/lib/modules/7.2.8-arch1-2/ is owned by linux 7.2.8.arch1-2' \
        '/usr/lib/modules/7.2.8-arch1-2/ is owned by linux-headers 7.2.8.arch1-2'
    return 0
}
PACMAN_REINSTALL_FLAG="$sandbox/pacman-verify-reinstall"
rm -f "$PACMAN_REINSTALL_FLAG"
repair_kernel_reinstall() { : > "$PACMAN_REINSTALL_FLAG"; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_verify)"
grep -q '^Repair change status kernels:verify: unchanged|reason:kernel-verified$' <<<"$status_line" \
    || fail "pacman verify must report kernel-verified for a healthy kernel on a target: $status_line"
[[ ! -e "$PACMAN_REINSTALL_FLAG" ]] \
    || fail 'pacman verify must not reinstall when the latest kernel is healthy'

# Scenario B (HOST): a stale running kernel (no module tree, so absent from the
# inventory) is skipped and the latest kernel is verified via vmlinuz-linux.
# The summary must name ONLY the installed latest kernel and flag the stale
# running kernel separately with a reboot hint (never list it as installed).
reset_fixture
TARGET_PACKAGE_MANAGER=pacman
KERNEL_BASELINE_B64=""
KERNEL_RUNNING=7.2.8-arch1-2
KERNEL_LATEST=7.2.9-arch1-1
KERNEL_INVENTORY_VERSIONS=(7.2.9-arch1-1)   # the stale running kernel is NOT installed
KERNEL_PARTIAL=()
KERNEL_ORPHANED_KERNELS=()
KERNEL_ORPHANED_HEADERS=()
KERNEL_ORPHANED_SUPPORT_VERSIONS=()
KERNEL_REPO_LATEST=""
mkdir -p "$fake_root/usr/lib/modules/7.2.9-arch1-1" "$fake_root/boot"
printf 'linux\n' > "$fake_root/usr/lib/modules/7.2.9-arch1-1/pkgbase"
touch "$fake_root/boot/vmlinuz-linux"
kernel_inventory_collect() { :; }
rm -f "$PACMAN_REINSTALL_FLAG"
repair_kernel_reinstall() { : > "$PACMAN_REINSTALL_FLAG"; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_verify)"
grep -q '^Repair change status kernels:verify: unchanged|reason:kernel-verified$' <<<"$status_line" \
    || fail "pacman verify must report kernel-verified with a stale running kernel: $status_line"
[[ ! -e "$PACMAN_REINSTALL_FLAG" ]] \
    || fail 'pacman verify must not reinstall when the latest kernel is healthy'
grep -qF 'verified: kernel 7.2.9-arch1-1 is installed and working (running kernel 7.2.8-arch1-2 is stale — reboot to activate 7.2.9-arch1-1)' <<<"$status_line" \
    || fail "pacman verify must name only the installed latest and flag the stale running kernel with a reboot hint: $status_line"
! grep -qF '7.2.8-arch1-2,7.2.9-arch1-1 are installed and working' <<<"$status_line" \
    || fail 'pacman verify must not list a stale running kernel as installed: '"$status_line"

# Scenario C (HOST, normal): the running kernel is present in the inventory, so
# verify names BOTH the running and latest kernels (byte-identical to the
# pre-change summary) and emits no stale-running note.
reset_fixture
TARGET_PACKAGE_MANAGER=pacman
KERNEL_BASELINE_B64=""
KERNEL_RUNNING=7.2.8-arch1-2
KERNEL_LATEST=7.2.9-arch1-1
KERNEL_INVENTORY_VERSIONS=(7.2.8-arch1-2 7.2.9-arch1-1)
KERNEL_PARTIAL=()
KERNEL_ORPHANED_KERNELS=()
KERNEL_ORPHANED_HEADERS=()
KERNEL_ORPHANED_SUPPORT_VERSIONS=()
KERNEL_REPO_LATEST=""
mkdir -p "$fake_root/usr/lib/modules/7.2.8-arch1-2" \
    "$fake_root/usr/lib/modules/7.2.9-arch1-1" "$fake_root/boot"
printf 'linux\n' > "$fake_root/usr/lib/modules/7.2.8-arch1-2/pkgbase"
printf 'linux\n' > "$fake_root/usr/lib/modules/7.2.9-arch1-1/pkgbase"
touch "$fake_root/boot/vmlinuz-linux"
kernel_inventory_collect() { :; }
rm -f "$PACMAN_REINSTALL_FLAG"
repair_kernel_reinstall() { : > "$PACMAN_REINSTALL_FLAG"; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_verify)"
grep -q '^Repair change status kernels:verify: unchanged|reason:kernel-verified$' <<<"$status_line" \
    || fail "pacman verify must report kernel-verified with the running kernel in the inventory: $status_line"
grep -qF 'verified: kernels 7.2.8-arch1-2,7.2.9-arch1-1 are installed and working' <<<"$status_line" \
    || fail "pacman verify must name both the running and latest kernels when the running kernel is installed: $status_line"
! grep -qF 'is stale' <<<"$status_line" \
    || fail 'pacman verify must not emit a stale-running note when the running kernel is in the inventory: '"$status_line"

# Scenario D (HOST, empty inventory): a stale running kernel with no installed
# inventory kernels and no latest must not crash, must not name the stale
# kernel as installed, and must hint at "the newest kernel".
reset_fixture
TARGET_PACKAGE_MANAGER=pacman
KERNEL_BASELINE_B64=""
KERNEL_RUNNING=7.2.8-arch1-2
KERNEL_LATEST=""
KERNEL_INVENTORY_VERSIONS=()
KERNEL_PARTIAL=()
KERNEL_ORPHANED_KERNELS=()
KERNEL_ORPHANED_HEADERS=()
KERNEL_ORPHANED_SUPPORT_VERSIONS=()
KERNEL_REPO_LATEST=""
kernel_inventory_collect() { :; }
rm -f "$PACMAN_REINSTALL_FLAG"
repair_kernel_reinstall() { : > "$PACMAN_REINSTALL_FLAG"; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_verify)"
grep -qF 'verified: kernel none is installed and working (running kernel 7.2.8-arch1-2 is stale — reboot to activate the newest kernel)' <<<"$status_line" \
    || fail "pacman verify must report no installed kernels and flag the stale running kernel: $status_line"
! grep -qF '7.2.8-arch1-2 is installed and working' <<<"$status_line" \
    || fail 'pacman verify must not name a stale running kernel as installed: '"$status_line"


# ---- pacman install-repo-latest: reinstall linux + linux-headers ------------
# repair_kernel_install_repo_latest on pacman installs the whole unversioned
# chain (linux + linux-headers) through guarded_pacman_install_packages, then
# re-collects + initramfs + bootloader regen and reports changed.  This recovers
# the total=0 inventory left by a `pacman -Syu` upgrade (the stale running
# kernel has no module tree).
PACMAN_REPO_LATEST_LOG="$sandbox/pacman-repo-latest.log"
PACMAN_FOLLOWON_LOG="$sandbox/pacman-repo-latest-followon.log"
: > "$PACMAN_REPO_LATEST_LOG"
: > "$PACMAN_FOLLOWON_LOG"
RUNNING_HOST_MODE=0
TARGET_PACKAGE_MANAGER=pacman
TARGET_BOOTLOADER_BACKEND=grub
KERNEL_BASELINE_B64=""
kernel_inventory_collect() {
    KERNEL_RUNNING=""
    KERNEL_LATEST=7.2.8-arch1-2
    KERNEL_REPO_LATEST=""
    KERNEL_INVENTORY_VERSIONS=()
    KERNEL_INVENTORY_PKGS=()
    KERNEL_INVENTORY_SIZES=()
    KERNEL_REMOVABLE=()
    KERNEL_PARTIAL=()
    KERNEL_ORPHANED_HEADERS=()
    KERNEL_ORPHANED_KERNELS=()
    KERNEL_ORPHANED_SUPPORT_VERSIONS=()
}
pacman_kernel_repo_latest() { printf '%s\n' '7.2.9-arch1-1'; }
guarded_pacman_install_packages() {
    printf '%s\n' "$*" >> "$PACMAN_REPO_LATEST_LOG"
    return 0
}
adaptive_initramfs_repair() { printf 'initramfs\n' >> "$PACMAN_FOLLOWON_LOG"; }
adaptive_grub_stage() { printf 'grub\n' >> "$PACMAN_FOLLOWON_LOG"; }
adaptive_extlinux_repair() { printf 'extlinux\n' >> "$PACMAN_FOLLOWON_LOG"; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_install_repo_latest)"
grep -q '^Repair change status kernels:install-repo-latest: changed$' <<<"$status_line" \
    || fail "pacman install-repo-latest must report changed: $status_line"
grep -qx 'linux linux-headers' "$PACMAN_REPO_LATEST_LOG" \
    || fail "pacman install-repo-latest must install linux + linux-headers, got: $(tr '\n' ',' < "$PACMAN_REPO_LATEST_LOG")"
[[ "$(cat "$PACMAN_FOLLOWON_LOG")" == $'initramfs\ngrub' ]] \
    || fail "pacman install-repo-latest follow-on order wrong: $(tr '\n' ',' < "$PACMAN_FOLLOWON_LOG")"

# ---- kernels gate: empty inventory + known repo-latest stays available -------
# A stale running kernel (a pacman -Syu upgrade pruned the still-booted
# kernel's module tree) leaves an empty inventory; when a repository-latest
# kernel IS known the kernels capability must stay available so the menu can
# offer install-repo-latest, and it must still fail closed with
# no-kernel-inventory when no repository-latest kernel is known.
mkdir -p "$fake_root/usr/bin" "$fake_root/etc" "$fake_root/var/lib/pacman" "$fake_root/boot"
touch "$fake_root/usr/bin/pacman"
chmod +x "$fake_root/usr/bin/pacman"
printf '[options]\n' > "$fake_root/etc/pacman.conf"
# (TARGET_PACKAGE_MANAGER=pacman, the pacman profile_target_backends,
#  kernel_inventory_collect -> empty inventory, and pacman_kernel_repo_latest
#  stubs from the section above are still active here.)
if ! reason="$(kernels_unavailable_reason)"; then
    fail "kernels must stay available with an empty inventory when a repo-latest kernel is known; got $reason"
fi
pacman_kernel_repo_latest() { return 1; }
if reason="$(kernels_unavailable_reason)"; then
    fail "kernels must fail closed when the inventory and repo-latest are both empty"
else
    grep -q 'reason:no-kernel-inventory' <<<"$reason" \
        || fail "kernels empty-inventory failure must name no-kernel-inventory; got $reason"
fi

# ---- rpm install-repo-latest: install explicit kernel/kernel-devel NEVRAs ----
# repair_kernel_install_repo_latest on rpm installs the EXPLICIT NEVRAs
# kernel-<repo_latest> + kernel-devel-<repo_latest> through
# guarded_rpm_install_packages — never the unversioned `kernel`, which dnf5
# resolves to the already-installed OLDER version and leaves only
# kernel-devel-<latest> installed with no matching kernel — then re-collects +
# initramfs + bootloader regen (Fedora BIOS uses GRUB2) and reports changed.
RPM_REPO_LATEST_LOG="$sandbox/rpm-repo-latest.log"
RPM_FOLLOWON_LOG="$sandbox/rpm-repo-latest-followon.log"
: > "$RPM_REPO_LATEST_LOG"
: > "$RPM_FOLLOWON_LOG"
RUNNING_HOST_MODE=0
TARGET_PACKAGE_MANAGER=rpm
TARGET_BOOTLOADER_BACKEND=grub
KERNEL_BASELINE_B64=""
kernel_inventory_collect() {
    KERNEL_RUNNING=""
    KERNEL_LATEST=6.8.1-200.fc44.x86_64
    KERNEL_REPO_LATEST=""
    KERNEL_INVENTORY_VERSIONS=()
    KERNEL_INVENTORY_PKGS=()
    KERNEL_INVENTORY_SIZES=()
    KERNEL_REMOVABLE=()
    KERNEL_PARTIAL=()
    KERNEL_ORPHANED_HEADERS=()
    KERNEL_ORPHANED_KERNELS=()
    KERNEL_ORPHANED_SUPPORT_VERSIONS=()
}
rpm_kernel_repo_latest() { printf '%s\n' '6.8.2-300.fc44.x86_64'; }
guarded_rpm_install_packages() {
    printf '%s\n' "$*" >> "$RPM_REPO_LATEST_LOG"
    return 0
}
adaptive_initramfs_repair() { printf 'initramfs\n' >> "$RPM_FOLLOWON_LOG"; }
adaptive_grub_stage() { printf 'grub\n' >> "$RPM_FOLLOWON_LOG"; }
adaptive_extlinux_repair() { printf 'extlinux\n' >> "$RPM_FOLLOWON_LOG"; }
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(repair_kernel_install_repo_latest)"
grep -q '^Repair change status kernels:install-repo-latest: changed$' <<<"$status_line" \
    || fail "rpm install-repo-latest must report changed: $status_line"
grep -qx 'kernel-6.8.2-300.fc44.x86_64 kernel-devel-6.8.2-300.fc44.x86_64' "$RPM_REPO_LATEST_LOG" \
    || fail "rpm install-repo-latest must install the explicit kernel/kernel-devel NEVRAs, got: $(tr '\n' ',' < "$RPM_REPO_LATEST_LOG")"
grep -qx 'kernel kernel-devel' "$RPM_REPO_LATEST_LOG" \
    && fail "rpm install-repo-latest must never install the unversioned kernel: $(tr '\n' ',' < "$RPM_REPO_LATEST_LOG")"
[[ "$(cat "$RPM_FOLLOWON_LOG")" == $'initramfs\ngrub' ]] \
    || fail "rpm install-repo-latest follow-on order wrong: $(tr '\n' ',' < "$RPM_FOLLOWON_LOG")"


# ===========================================================================
# Backend-independent newest-kernel removability: a repair target has NO running
# kernel, so "remove newest" must never leave the target with zero kernels.
# ===========================================================================
# Re-source the helper to restore the real collect/menu/parse definitions after
# the apk section's stubs, then re-establish the harness environment.
source <(sed '/^main "$@"/d' "$HELPER")
TARGET_ROOT="$fake_root"
RUNNING_HOST_MODE=0
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"
rm -rf "$fake_root/lib/apk" "$fake_root/etc/apk" "$fake_root/sbin" \
       "$fake_root/var/lib/rpm" "$fake_root/etc/yum.repos.d"
rm -f "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5" "$fake_root/usr/bin/akmods"
DPKG_PATH_OWNER_INDEX=""

# ---- apt/dpkg: target with ONE installed kernel ------------------------------
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

# Drop the old generic kernel so only the newest remains (single-kernel target).
write_generic_fixture
FAKE_RUNNING=6.5.0-10038-generic
rm -rf "$fake_root/usr/lib/modules/6.2.0-1000-generic" \
       "$fake_root/usr/src/linux-headers-6.2.0-1000-generic" \
       "$fake_root/usr/src/linux-headers-6.2.0-1000"
rm -f "$fake_root/boot/vmlinuz-6.2.0-1000-generic"
awk 'BEGIN { RS = ""; ORS = "\n\n" }
     $2 != "linux-image-6.2.0-1000-generic" \
     && $2 != "linux-modules-6.2.0-1000-generic" \
     && $2 != "linux-headers-6.2.0-1000-generic" \
     && $2 != "linux-headers-6.2.0-1000" { print }' \
    "$fake_root/var/lib/dpkg/status" > "$fake_root/var/lib/dpkg/status.new"
mv "$fake_root/var/lib/dpkg/status.new" "$fake_root/var/lib/dpkg/status"
kernel_inventory_collect
[[ -z "$KERNEL_RUNNING" ]] \
    || fail "apt single-kernel target must have an empty KERNEL_RUNNING: $KERNEL_RUNNING"
[[ "${#KERNEL_REMOVABLE[@]}" -eq 0 ]] \
    || fail "apt single-kernel target must have an empty removable set: ${KERNEL_REMOVABLE[*]}"
kernel_newest_is_removable \
    && fail 'apt single-kernel target must not offer newest removal (no fallback kernel)'
out="$(diagnostic_kernel_inventory || true)"
grep -q '^Kernel running: none$' <<<"$out" \
    || fail "apt single-kernel diagnostic must emit 'Kernel running: none': $out"

# parse_cached_kernel_inventory round-trips "Kernel running: none" -> empty.
KERNEL_BASELINE_B64="$(printf '%s' "$out" | base64 | tr -d '\n')"
KERNEL_RUNNING="sentinel"
parse_cached_kernel_inventory
[[ -z "$KERNEL_RUNNING" ]] \
    || fail "parsing 'Kernel running: none' must yield an empty KERNEL_RUNNING: $KERNEL_RUNNING"
KERNEL_BASELINE_B64=""

# The interactive menu must NOT offer "Remove newest kernel" for a single-kernel
# target even when another action (repo-latest install) is offered: add a newer
# repository kernel so the menu actually prompts, then assert the remove-newest
# entry is absent.
mkdir -p "$fake_root/var/lib/apt/lists"
cat > "$fake_root/var/lib/apt/lists/ubuntu_Packages" <<'PACKAGES'
Package: linux-image-generic
Version: 6.5.0.10040.1
Depends: linux-image-6.5.0-10040-generic (= 6.5.0-10040.1)

PACKAGES
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
BOOT_REPAIR_SESSION_PROTOCOL=1
BOOT_REPAIR_SESSION_REQUEST=7
SINGLE_MENU_FILE="$sandbox/single-kernel-menu.txt"
: > "$SINGLE_MENU_FILE"
helper_menu_prompt() { printf '%s\n' "$1" >> "$SINGLE_MENU_FILE"; HELPER_MENU_ANSWER="0"; return 0; }
# The repo-latest install is now pre-flighted: the image AND headers must be
# provably installable before the action is offered.  Stub both probes as
# installable so this test exercises the offer path (the suppression path is
# covered below).
apt_headers_installable() { return 0; }
apt_image_installable() { return 0; }
kernel_interactive_menu >"$sandbox/single-kernel-status.txt" 2>&1 || true
grep -q 'Install repository-latest kernel' "$SINGLE_MENU_FILE" \
    || fail 'single-kernel target with a newer repo kernel must offer install-repo-latest'
grep -q 'Remove newest kernel' "$SINGLE_MENU_FILE" \
    && fail 'single-kernel target must never offer "Remove newest kernel"'
BOOT_REPAIR_SESSION_PROTOCOL=0
KERNEL_BASELINE_B64=""

# ---- apt/dpkg: repo-latest image uninstallable (headers installable) ---------
# The backports scenario: the image has a stricter dependency (e.g. PreDepends:
# linux-base (>= 4.17~)) that plain apt-get refuses without -t <suite>-backports,
# so the action must NOT be offered even though the headers are installable.
write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
mkdir -p "$fake_root/var/lib/apt/lists"
cat > "$fake_root/var/lib/apt/lists/tuxedo_Packages" <<'PACKAGES'
Package: linux-image-tuxedo
Version: 7.1.10.1.1
Depends: linux-image-7.1.10.1-tuxedo-amd64 (= 7.1.10.1-1)

PACKAGES
kernel_inventory_collect
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
BOOT_REPAIR_SESSION_PROTOCOL=1
BOOT_REPAIR_SESSION_REQUEST=7
SUPPRESS_MENU_FILE="$sandbox/suppress-repo-latest-menu.txt"
: > "$SUPPRESS_MENU_FILE"
helper_menu_prompt() { printf '%s\n' "$1" >> "$SUPPRESS_MENU_FILE"; HELPER_MENU_ANSWER="0"; return 0; }
# Headers installable, image uninstallable: the exact backports split.  The
# menu must suppress the action on the image probe's failure.
apt_headers_installable() { return 0; }
apt_image_installable() { return 1; }
kernel_interactive_menu >"$sandbox/suppress-repo-latest-status.txt" 2>&1 || true
grep -q 'Install repository-latest kernel' "$SUPPRESS_MENU_FILE" \
    && fail 'uninstallable repo-latest image must suppress install-repo-latest (headers installable)'
BOOT_REPAIR_SESSION_PROTOCOL=0
KERNEL_BASELINE_B64=""

# ---- apt/dpkg: target with TWO kernels (older + newest) ----------------------
write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
kernel_inventory_collect
[[ -z "$KERNEL_RUNNING" ]] || fail "apt two-kernel target must have an empty KERNEL_RUNNING: $KERNEL_RUNNING"
[[ "${#KERNEL_REMOVABLE[@]}" -eq 1 ]] || fail "apt two-kernel target removable count: ${#KERNEL_REMOVABLE[@]}"
kernel_newest_is_removable \
    || fail 'apt two-kernel target must allow newest removal (an older kernel remains)'
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
BOOT_REPAIR_SESSION_PROTOCOL=1
BOOT_REPAIR_SESSION_REQUEST=7
TWO_MENU_FILE="$sandbox/two-kernel-menu.txt"
: > "$TWO_MENU_FILE"
# Select kernel 2 (the newest) in the kernel-selection prompt, then answer 0
# (Back) in its per-kernel action prompt, so both prompts are recorded.
TWO_MENU_ANSWERS=("2" "0")
helper_menu_prompt() {
    local n=0
    n="$(cat "$sandbox/two-kernel-prompt-count.txt" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$sandbox/two-kernel-prompt-count.txt"
    printf '%s\n' "$1" >> "$TWO_MENU_FILE"
    if (( n <= ${#TWO_MENU_ANSWERS[@]} )); then
        HELPER_MENU_ANSWER="${TWO_MENU_ANSWERS[$((n - 1))]}"
    else
        HELPER_MENU_ANSWER="0"
    fi
    return 0
}
: > "$sandbox/two-kernel-prompt-count.txt"
printf '0' > "$sandbox/two-kernel-prompt-count.txt"
kernel_interactive_menu >"$sandbox/two-kernel-status.txt" 2>&1 || true
grep -q 'Remove newest kernel (7.1.9.1-tuxedo-amd64)' "$TWO_MENU_FILE" \
    || fail 'two-kernel target menu must offer "Remove newest kernel (7.1.9.1-tuxedo-amd64)"'
grep -q 'Kernel 7.1.9.1-tuxedo-amd64 actions:' "$TWO_MENU_FILE" \
    || fail 'selecting the newest kernel must present its per-kernel action prompt'
grep -q '0) Back' "$TWO_MENU_FILE" \
    || fail 'the per-kernel action prompt must use the 0) Back back line'
grep -q 'newest installed kernel: 7.1.9.1-tuxedo-amd64' "$TWO_MENU_FILE" \
    || fail 'the menu summary must always report the newest installed kernel'
grep -q 'running kernel:' "$TWO_MENU_FILE" \
    && fail 'a repair target menu must never report a running kernel line'
grep -q 'pkgs=' "$TWO_MENU_FILE" \
    && fail 'the level-1 menu must not repeat the full pkgs= package lists'
BOOT_REPAIR_SESSION_PROTOCOL=0
KERNEL_BASELINE_B64=""

# ---- host mode: newest removable iff running != latest ------------------------
RUNNING_HOST_MODE=1
write_tuxedo_fixture
FAKE_RUNNING=7.1.8.2-tuxedo-amd64   # running (older) != latest (7.1.9.1)
kernel_inventory_collect
[[ "$KERNEL_RUNNING" == 7.1.8.2-tuxedo-amd64 ]] || fail "host running kernel: $KERNEL_RUNNING"
kernel_newest_is_removable || fail 'host running!=latest must allow newest removal'
FAKE_RUNNING=7.1.9.1-tuxedo-amd64   # running == latest
kernel_inventory_collect
kernel_newest_is_removable && fail 'host running==latest must refuse newest removal'
RUNNING_HOST_MODE=0

# ---- rpm: target with TWO kernels then ONE -----------------------------------
profile_target_backends() {
    TARGET_DISTRO_FAMILY=fedora
    TARGET_PACKAGE_MANAGERS=(rpm)
    TARGET_PACKAGE_MANAGER=rpm
    TARGET_SERVICE_MANAGERS=(systemd)
    TARGET_SERVICE_MANAGER=systemd
    TARGET_DISPLAY_BACKEND=systemd
    TARGET_INITRAMFS_BACKENDS=(dracut)
    TARGET_INITRAMFS_BACKEND=dracut
    TARGET_LOGGING_BACKEND=journald
    TARGET_BOOTLOADER_BACKEND=grub
    TARGET_ESP_MOUNT=unresolved
    TARGET_KERNEL_LAYOUT='versioned vmlinuz-* kernels'
    TARGET_REPAIR_BACKEND='Fedora/rpm'
}
DPKG_PATH_OWNER_INDEX=""
rm -rf "$fake_root/usr/lib/modules" "$fake_root/lib/modules" "$fake_root/boot" "$fake_root/usr/src" \
       "$fake_root/var/lib/dpkg/info"
mkdir -p "$fake_root/var/lib/dpkg/info" "$fake_root/boot"
: > "$fake_root/var/lib/dpkg/status"
touch "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5"
chmod +x "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5"
mkdir -p "$fake_root/var/lib/rpm" "$fake_root/etc/yum.repos.d"
printf 'sqlite' > "$fake_root/var/lib/rpm/rpmdb.sqlite"
printf '[fedora]\nenabled=1\n' > "$fake_root/etc/yum.repos.d/fedora.repo"
rpm_old=6.8.0-100.fc44.x86_64
rpm_new=6.8.1-200.fc44.x86_64
mkdir -p "$fake_root/usr/lib/modules/$rpm_old" "$fake_root/usr/lib/modules/$rpm_new" \
         "$fake_root/usr/src/kernels/$rpm_old" "$fake_root/usr/src/kernels/$rpm_new"
touch "$fake_root/boot/vmlinuz-$rpm_old" "$fake_root/boot/vmlinuz-$rpm_new"
run_selected_chroot() {
    local arg path ver
    for arg in "$@"; do
        [[ "$arg" == /* ]] || continue
        path="$arg"
        case "$path" in
            /boot/vmlinuz-*) ver="${path#/boot/vmlinuz-}"; printf 'kernel-core-%s\n' "$ver" ;;
            /usr/lib/modules/*|/lib/modules/*) ver="${path#/usr/lib/modules/}"; ver="${ver#/lib/modules/}"; printf 'kernel-modules-%s\n' "$ver" ;;
            /usr/src/kernels/*) ver="${path#/usr/src/kernels/}"; printf 'kernel-devel-%s\n' "$ver" ;;
        esac
    done
    return 0
}
kernel_inventory_collect
[[ -z "$KERNEL_RUNNING" ]] || fail "rpm two-kernel target must have an empty KERNEL_RUNNING: $KERNEL_RUNNING"
[[ "${#KERNEL_REMOVABLE[@]}" -eq 1 ]] || fail "rpm two-kernel target removable count: ${#KERNEL_REMOVABLE[@]}"
kernel_newest_is_removable || fail 'rpm two-kernel target must allow newest removal'
out="$(diagnostic_kernel_inventory || true)"
grep -q '^Kernel running: none$' <<<"$out" \
    || fail "rpm diagnostic must emit 'Kernel running: none': $out"

# Drop the older rpm kernel -> single kernel, newest removal refused.
rm -rf "$fake_root/usr/lib/modules/$rpm_old" "$fake_root/usr/src/kernels/$rpm_old"
rm -f "$fake_root/boot/vmlinuz-$rpm_old"
kernel_inventory_collect
[[ "${#KERNEL_REMOVABLE[@]}" -eq 0 ]] || fail "rpm single-kernel target removable count: ${#KERNEL_REMOVABLE[@]}"
kernel_newest_is_removable && fail 'rpm single-kernel target must refuse newest removal'

# ---- apk: target with TWO kernels then ONE ------------------------------------
profile_target_backends() {
    TARGET_DISTRO_FAMILY=alpine
    TARGET_PACKAGE_MANAGERS=(apk)
    TARGET_PACKAGE_MANAGER=apk
    TARGET_SERVICE_MANAGERS=(openrc)
    TARGET_SERVICE_MANAGER=openrc
    TARGET_DISPLAY_BACKEND=openrc
    TARGET_INITRAMFS_BACKENDS=(mkinitfs)
    TARGET_INITRAMFS_BACKEND=mkinitfs
    TARGET_LOGGING_BACKEND=syslog
    TARGET_BOOTLOADER_BACKEND=syslinux/extlinux
    TARGET_ESP_MOUNT=unresolved
    TARGET_KERNEL_LAYOUT='flavor vmlinuz-lts/virt kernels'
    TARGET_REPAIR_BACKEND='Alpine/apk'
}
DPKG_PATH_OWNER_INDEX=""
rm -rf "$fake_root/usr/lib/modules" "$fake_root/lib/modules" "$fake_root/boot" "$fake_root/usr/src" \
       "$fake_root/var/lib/rpm" "$fake_root/etc/yum.repos.d"
rm -f "$fake_root/usr/bin/rpm" "$fake_root/usr/bin/dnf5"
mkdir -p "$fake_root/lib/modules/6.18.55-0-lts" "$fake_root/lib/modules/6.18.56-0-virt" \
         "$fake_root/boot" "$fake_root/lib/apk/db" "$fake_root/etc/apk" "$fake_root/sbin"
printf '%s\n' '-lts' > "$fake_root/lib/modules/6.18.55-0-lts/kernel-suffix"
printf '%s\n' '-virt' > "$fake_root/lib/modules/6.18.56-0-virt/kernel-suffix"
touch "$fake_root/boot/vmlinuz-lts" "$fake_root/boot/vmlinuz-virt" \
      "$fake_root/boot/initramfs-lts" "$fake_root/boot/initramfs-virt"
printf 'https://dl-cdn.alpinelinux.org/alpine/v3.20/main\n' > "$fake_root/etc/apk/repositories"
: > "$fake_root/lib/apk/db/installed"
: > "$fake_root/etc/apk/world"
touch "$fake_root/sbin/apk"
chmod +x "$fake_root/sbin/apk"
run_selected_chroot() {
    local arg path
    for arg in "$@"; do
        [[ "$arg" == /* ]] || continue
        path="$arg"
        case "$path" in
            /lib/modules/6.18.55-0-lts*|/usr/lib/modules/6.18.55-0-lts*) printf '%s is owned by linux-lts-6.18.55-r0\n' "$path" ;;
            /lib/modules/6.18.56-0-virt*|/usr/lib/modules/6.18.56-0-virt*) printf '%s is owned by linux-virt-6.18.56-r0\n' "$path" ;;
            /boot/vmlinuz-lts*) printf '%s is owned by linux-lts-6.18.55-r0\n' "$path" ;;
            /boot/vmlinuz-virt*) printf '%s is owned by linux-virt-6.18.56-r0\n' "$path" ;;
        esac
    done
    return 0
}
target_apk_package_installed() { return 0; }
kernel_inventory_collect
[[ -z "$KERNEL_RUNNING" ]] || fail "apk two-kernel target must have an empty KERNEL_RUNNING: $KERNEL_RUNNING"
[[ "${#KERNEL_REMOVABLE[@]}" -eq 1 ]] || fail "apk two-kernel target removable count: ${#KERNEL_REMOVABLE[@]}"
kernel_newest_is_removable || fail 'apk two-kernel target must allow newest removal'
out="$(diagnostic_kernel_inventory || true)"
grep -q '^Kernel running: none$' <<<"$out" \
    || fail "apk diagnostic must emit 'Kernel running: none': $out"

# Drop the older apk kernel -> single kernel, newest removal refused.
rm -rf "$fake_root/lib/modules/6.18.55-0-lts"
rm -f "$fake_root/boot/vmlinuz-lts"
kernel_inventory_collect
[[ "${#KERNEL_REMOVABLE[@]}" -eq 0 ]] || fail "apk single-kernel target removable count: ${#KERNEL_REMOVABLE[@]}"
kernel_newest_is_removable && fail 'apk single-kernel target must refuse newest removal'

# ---- no-op sub-action with a remaining option still re-prompts --------------
# A no-op sub-action (unchanged) leaves the menu byte-identical, but a genuinely
# actionable option ("Remove newest kernel") may remain. The re-prompt guard
# must NOT terminate on byte-identity alone: the menu re-prompts so the user can
# still pick the remaining option.
source <(sed '/^main "$@"/d' "$HELPER")
TARGET_ROOT="$fake_root"
RUNNING_HOST_MODE=0
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"
running_kernel_version() { printf '%s\n' "${FAKE_RUNNING-7.1.9.1-tuxedo-amd64}"; }
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
write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
kernel_inventory_collect
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
BOOT_REPAIR_SESSION_PROTOCOL=1
BOOT_REPAIR_SESSION_REQUEST=7
REPROMPT_MENU_FILE="$sandbox/reprompt-menu.txt"
REPROMPT_COUNT_FILE="$sandbox/reprompt-count.txt"
: > "$REPROMPT_MENU_FILE"
printf '0' > "$REPROMPT_COUNT_FILE"
# Two-level flow: select the newest kernel (2), then its per-kernel "remove
# newest" action (1); that sub-action is a no-op, so the menu re-collects and
# re-prompts the kernel-selection menu, which is answered 0 (Abort).
REPROMPT_ANSWERS=("2" "1" "0")
helper_menu_prompt() {
    local n=0
    n="$(cat "$REPROMPT_COUNT_FILE" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$REPROMPT_COUNT_FILE"
    printf '%s\n' "$1" >> "$REPROMPT_MENU_FILE"
    if (( n <= ${#REPROMPT_ANSWERS[@]} )); then
        HELPER_MENU_ANSWER="${REPROMPT_ANSWERS[$((n - 1))]}"
    else
        HELPER_MENU_ANSWER="0"
    fi
    return 0
}
# A no-op remove-newest: reports unchanged and mutates nothing, so the
# re-collected inventory rebuilds the byte-identical kernel-selection menu.
repair_kernel_remove_newest() {
    repair_change_status "kernels:remove-newest" "unchanged|$(reason no-removable-kernels)"
    return 0
}
kernel_interactive_menu >"$sandbox/reprompt-status.txt" 2>&1 || true
[[ "$(cat "$REPROMPT_COUNT_FILE")" -eq 3 ]] \
    || fail "the menu must re-prompt after a no-op sub-action with a remaining option (prompts: $(cat "$REPROMPT_COUNT_FILE"))"
grep -q 'Remove newest kernel (7.1.9.1-tuxedo-amd64)' "$REPROMPT_MENU_FILE" \
    || fail 'the per-kernel action prompt must offer "Remove newest kernel (7.1.9.1-tuxedo-amd64)"'
grep -q 'remove newest' "$REPROMPT_MENU_FILE" \
    || fail 'the re-prompted kernel-selection menu must still annotate the newest kernel as removable'
grep -q '^Repair change status kernels: unchanged|reason:kernels-aborted$' "$sandbox/reprompt-status.txt" \
    || fail 'an aborted kernel menu must report kernels-aborted (unchanged)'
BOOT_REPAIR_SESSION_PROTOCOL=0
KERNEL_BASELINE_B64=""

# ---- menu-size guard: level-1 menu stays concise with many kernels ----------
# The two-level menu must never blow the GUI's PROMPT payload bound: the old
# flat menu carried every removable kernel's full pkgs= list, whose base64
# exceeded 1024 bytes at 3+ kernels.  A 5-kernel fixture's level-1 menu must
# stay well under 2048 raw bytes (=> base64 far under the 16384-byte bound) and
# must never repeat the full package list.
write_five_kernel_fixture
FAKE_RUNNING=6.4.0-5-generic
kernel_inventory_collect
[[ "${#KERNEL_INVENTORY_VERSIONS[@]}" -eq 5 ]] \
    || fail "five-kernel fixture must map 5 kernels, got: ${KERNEL_INVENTORY_VERSIONS[*]}"
baseline_text="$(diagnostic_kernel_inventory || true)"
KERNEL_BASELINE_B64="$(printf '%s' "$baseline_text" | base64 | tr -d '\n')"
BOOT_REPAIR_SESSION_PROTOCOL=1
BOOT_REPAIR_SESSION_REQUEST=7
MENU_SIZE_FILE="$sandbox/five-kernel-menu-size.txt"
MENU_TEXT_FILE="$sandbox/five-kernel-menu.txt"
MENU_PROMPT_COUNT_FILE="$sandbox/five-kernel-menu-count.txt"
: > "$MENU_TEXT_FILE"
printf '0' > "$MENU_PROMPT_COUNT_FILE"
helper_menu_prompt() {
    local n=0
    n="$(cat "$MENU_PROMPT_COUNT_FILE" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$MENU_PROMPT_COUNT_FILE"
    if (( n == 1 )); then
        printf '%s\n' "$1" >> "$MENU_TEXT_FILE"
        printf '%s\n' "$(printf '%s' "$1" | wc -c | tr -d '[:space:]')" > "$MENU_SIZE_FILE"
    fi
    HELPER_MENU_ANSWER="0"
    return 0
}
kernel_interactive_menu >"$sandbox/five-kernel-status.txt" 2>&1 || true
menu_bytes="$(cat "$MENU_SIZE_FILE")"
(( menu_bytes < 2048 )) \
    || fail "level-1 kernel menu must stay under 2048 bytes with 5 kernels, got $menu_bytes bytes"
grep -q 'pkgs=' "$MENU_TEXT_FILE" \
    && fail 'the level-1 kernel menu must not repeat the full pkgs= package lists'
for v in 6.0.0-1-generic 6.1.0-2-generic 6.2.0-3-generic 6.3.0-4-generic 6.4.0-5-generic; do
    grep -q "$v" "$MENU_TEXT_FILE" \
        || fail "the 5-kernel menu must list $v"
done
BOOT_REPAIR_SESSION_PROTOCOL=0
KERNEL_BASELINE_B64=""

# ---- metapackage-downgrade guard: removing newest cascades to metapackages ---
# Removing the newest kernel cascades apt's purge to the managed metapackages
# that pin it.  The stage must (a) detect that cascade, (b) perform a combined
# remove+install that downgrades each metapackage to the older version pinning
# the surviving kernel, (c) fail closed with
# reason:managed-default-metapackage-cascade when no older pinning version
# exists, and (d) never remove a protected kernel package.  This uses the
# write_tuxedo_fixture shape (linux-image-tuxedo + linux-headers-tuxedo-amd64
# pin the newest 7.1.9.1; the survivor is 7.1.8.2).
write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
kernel_inventory_collect
KERNEL_REMOVE_MODE=newest
KERNEL_STAGE_STATUS_KEY="kernels:remove-newest"
: > "$sandbox/apply.log"
: > "$sandbox/rollback.log"
run_selected_chroot() {
    local arg joined
    local -a cmd=()
    for arg in "$@"; do cmd+=("$arg"); done
    joined="${cmd[*]}"
    case "$joined" in
        *" apt-get -s -o Debug::NoLocking=1 remove --purge"*)
            printf 'Reading package lists...\n'
            printf 'Remv linux-image-7.1.9.1-tuxedo-amd64 [7.1.9.1]\n'
            printf 'Remv linux-modules-7.1.9.1-tuxedo-amd64 [7.1.9.1]\n'
            printf 'Remv linux-headers-7.1.9.1-tuxedo-amd64 [7.1.9.1]\n'
            printf 'Remv linux-headers-7.1.9.1-common-tuxedo [7.1.9.1]\n'
            printf 'Remv linux-image-tuxedo [7.1.9.1.2]\n'
            printf 'Remv linux-headers-tuxedo-amd64 [7.1.9.1.2]\n'
            printf '0 upgraded, 0 newly installed, 6 to remove and 0 not upgraded.\n'
            ;;
        *" apt-cache madison linux-image-tuxedo"*)
            printf 'linux-image-tuxedo | 7.1.9.1.2 | https://deb.tuxedocomputers.com jammy/main amd64 Packages\n'
            printf 'linux-image-tuxedo | 7.1.8.2.3 | https://deb.tuxedocomputers.com jammy/main amd64 Packages\n'
            ;;
        *" apt-cache show linux-image-tuxedo=7.1.9.1.2"*)
            printf 'Package: linux-image-tuxedo\nVersion: 7.1.9.1.2\nDepends: linux-image-7.1.9.1-tuxedo-amd64 (= 7.1.9.1-1)\n'
            ;;
        *" apt-cache show linux-image-tuxedo=7.1.8.2.3"*)
            printf 'Package: linux-image-tuxedo\nVersion: 7.1.8.2.3\nDepends: linux-image-7.1.8.2-tuxedo-amd64 (= 7.1.8.2-1)\n'
            ;;
        *" apt-cache madison linux-headers-tuxedo-amd64"*)
            printf 'linux-headers-tuxedo-amd64 | 7.1.9.1.2 | https://deb.tuxedocomputers.com jammy/main amd64 Packages\n'
            printf 'linux-headers-tuxedo-amd64 | 7.1.8.2.3 | https://deb.tuxedocomputers.com jammy/main amd64 Packages\n'
            ;;
        *" apt-cache show linux-headers-tuxedo-amd64=7.1.9.1.2"*)
            printf 'Package: linux-headers-tuxedo-amd64\nVersion: 7.1.9.1.2\nDepends: linux-headers-7.1.9.1-tuxedo-amd64 (= 7.1.9.1-1)\n'
            ;;
        *" apt-cache show linux-headers-tuxedo-amd64=7.1.8.2.3"*)
            printf 'Package: linux-headers-tuxedo-amd64\nVersion: 7.1.8.2.3\nDepends: linux-headers-7.1.8.2-tuxedo-amd64 (= 7.1.8.2-1)\n'
            ;;
        *" apt-get -s -o Debug::NoLocking=1 --allow-downgrades install"*)
            printf 'Reading package lists...\n'
            printf 'Inst linux-image-tuxedo [7.1.9.1.2] (7.1.8.2.3 jammy [amd64])\n'
            printf 'Inst linux-headers-tuxedo-amd64 [7.1.9.1.2] (7.1.8.2.3 jammy [amd64])\n'
            printf 'Remv linux-image-7.1.9.1-tuxedo-amd64 [7.1.9.1]\n'
            printf 'Remv linux-modules-7.1.9.1-tuxedo-amd64 [7.1.9.1]\n'
            printf 'Remv linux-headers-7.1.9.1-tuxedo-amd64 [7.1.9.1]\n'
            printf 'Remv linux-headers-7.1.9.1-common-tuxedo [7.1.9.1]\n'
            printf '2 upgraded, 0 newly installed, 4 to remove and 0 not upgraded.\n'
            ;;
    esac
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
    printf '%s\n' "$@" >> "$sandbox/apply.log"
    # Simulate the real apt apply: re-point the metapackages at the survivor.
    sed -i \
        -e 's/^Depends: linux-image-7\.1\.9\.1-tuxedo-amd64 /Depends: linux-image-7.1.8.2-tuxedo-amd64 /' \
        -e 's/^Depends: linux-headers-7\.1\.9\.1-tuxedo-amd64 /Depends: linux-headers-7.1.8.2-tuxedo-amd64 /' \
        "$fake_root/var/lib/dpkg/status"
    CHROOT_TRY_RC=0
    CHROOT_TRY_OUTPUT=''
    return 0
}
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(apt_kernel_cleanup_stage)"
grep -q '^Repair change status kernels:remove-newest: changed$' <<<"$status_line" \
    || fail "metapackage downgrade must report changed (got: $status_line)"
grep -q 'linux-image-tuxedo=7.1.8.2.3' "$sandbox/apply.log" \
    || fail 'the downgrade apply must re-point the image metapackage at the survivor pinning version'
grep -q 'linux-headers-tuxedo-amd64=7.1.8.2.3' "$sandbox/apply.log" \
    || fail 'the downgrade apply must re-point the headers metapackage'
grep -q 'linux-image-7.1.9.1-tuxedo-amd64-' "$sandbox/apply.log" \
    || fail 'the downgrade apply must remove the newest kernel versioned package (trailing -)'
if grep -q 'linux-image-7.1.8.2-tuxedo-amd64' "$sandbox/apply.log"; then
    fail 'the protected surviving kernel package must never be in the removal/install list'
fi
[[ ! -s "$sandbox/rollback.log" ]] || fail 'no rollback must run on a clean metapackage downgrade'

# ---- metapackage-downgrade failure: no older pinning -> fail closed ---------
# With no older metapackage version available to pin the surviving kernel, the
# downgrade cannot be resolved; the stage must fail closed with the specific
# reason naming the cascaded metapackages and the surviving kernel.
write_tuxedo_fixture
FAKE_RUNNING=7.1.9.1-tuxedo-amd64
kernel_inventory_collect
KERNEL_REMOVE_MODE=newest
KERNEL_STAGE_STATUS_KEY="kernels:remove-newest"
run_selected_chroot() {
    local arg joined
    local -a cmd=()
    for arg in "$@"; do cmd+=("$arg"); done
    joined="${cmd[*]}"
    case "$joined" in
        *" apt-get -s -o Debug::NoLocking=1 remove --purge"*)
            printf 'Remv linux-image-7.1.9.1-tuxedo-amd64 [7.1.9.1]\n'
            printf 'Remv linux-image-tuxedo [7.1.9.1.2]\n'
            printf 'Remv linux-headers-tuxedo-amd64 [7.1.9.1.2]\n'
            ;;
        *" apt-cache madison linux-image-tuxedo"*)
            printf 'linux-image-tuxedo | 7.1.9.1.2 | https://deb.tuxedocomputers.com jammy/main amd64 Packages\n'
            ;;
        *" apt-cache madison linux-headers-tuxedo-amd64"*)
            printf 'linux-headers-tuxedo-amd64 | 7.1.9.1.2 | https://deb.tuxedocomputers.com jammy/main amd64 Packages\n'
            ;;
    esac
    return 0
}
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()
status_line="$(apt_kernel_cleanup_stage)"
grep -q '^Repair change status kernels:remove-newest: unchanged|reason:managed-default-metapackage-cascade' <<<"$status_line" \
    || fail "unresolvable downgrade must fail closed with reason:managed-default-metapackage-cascade (got: $status_line)"
grep -q 'linux-image-tuxedo' <<<"$status_line" \
    || fail 'the reason must name the cascaded image metapackage'
grep -q 'linux-headers-tuxedo-amd64' <<<"$status_line" \
    || fail 'the reason must name the cascaded headers metapackage'
grep -q '7.1.8.2-tuxedo-amd64' <<<"$status_line" \
    || fail 'the reason must name the surviving kernel'
KERNEL_REMOVE_MODE=""


echo 'kernels-cleanup-contract: PASS'
