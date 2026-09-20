#!/usr/bin/env bash
# This contract test sources the helper under test dynamically and sets the
# helper's globals directly so ShellCheck cannot track their use.
# shellcheck disable=SC1090,SC2034
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="${HELPER:-$ROOT_DIR/scripts/boot-repair-helper.sh}"

[[ -x "$HELPER" ]] || { echo "FAIL: helper is not executable" >&2; exit 1; }
bash -n "$HELPER"

# Keep the probe layer visible so backend availability can only be decided by
# read-only target evidence, never by a distribution-family prefilter.
grep -q '^is_debian_family()' "$HELPER"
grep -q '^is_arch_family()' "$HELPER"
grep -q '^is_alpine_family()' "$HELPER"
grep -q '^profile_target_backends()' "$HELPER"
grep -q '^target_dpkg_detected()' "$HELPER"
grep -q '^target_apt_detected()' "$HELPER"
grep -q '^target_apk_detected()' "$HELPER"
grep -q '^target_pacman_detected()' "$HELPER"
grep -q '^target_rpm_detected()' "$HELPER"
grep -q '^target_systemd_present()' "$HELPER"
grep -q '^openrc_present()' "$HELPER"
grep -q '^target_display_manager_backend()' "$HELPER"
grep -q '^target_logging_backend()' "$HELPER"
grep -q '^target_initramfs_backend_detected()' "$HELPER"
grep -q '^package_stage_backends()' "$HELPER"
grep -q '^run_package_stage()' "$HELPER"
grep -q '^validate_repair_stages_against_backends()' "$HELPER"
# Package-manager feedback (held-back/skipped/ignored/masked/pinned packages)
# is parsed per backend and published as stable evidence lines plus a summary
# that the change-status reason and the GUI result summary consume.
grep -q '^package_feedback_reset()' "$HELPER"
grep -q '^package_feedback_report()' "$HELPER"
grep -q '^package_feedback_publish()' "$HELPER"
grep -q '^apt_package_feedback_report()' "$HELPER"
grep -q '^rpm_package_feedback_report()' "$HELPER"
grep -q '^pacman_package_feedback_report()' "$HELPER"
grep -q '^apk_package_feedback_report()' "$HELPER"
grep -q 'TARGET_PACKAGE_MANAGERS=()' "$HELPER"
grep -q 'TARGET_INITRAMFS_BACKENDS=()' "$HELPER"
grep -q 'TARGET_SERVICE_MANAGERS=()' "$HELPER"
grep -q 'TARGET_BOOTLOADER_BACKEND="systemd-boot' "$HELPER"
grep -q '^alpine_openrc_present()' "$HELPER"
grep -q '^alpine_display_manager_present()' "$HELPER"
grep -q '^alpine_kernel_images()' "$HELPER"
grep -q '^alpine_initramfs_images()' "$HELPER"
grep -q '/lib/apk/db/lock' "$HELPER"
grep -q '^target_apk_package_installed()' "$HELPER"
grep -q '^alpine_kernel_pairs()' "$HELPER"
grep -q '^alpine_apk_preflight()' "$HELPER"
grep -q '^alpine_apk_audit_missing_paths()' "$HELPER"
grep -q '^alpine_apk_missing_file_packages()' "$HELPER"
grep -q '^alpine_apk_missing_files_fingerprint()' "$HELPER"
grep -q '^alpine_apk_transaction_try()' "$HELPER"
grep -q '^alpine_apk_simulation_is_safe()' "$HELPER"
grep -q '^alpine_apk_transaction_reported_no_changes()' "$HELPER"
grep -q '^adaptive_alpine_apk_apply()' "$HELPER"
grep -q '^adaptive_alpine_apk_fix_broken()' "$HELPER"
grep -q '^adaptive_alpine_apk_upgrade()' "$HELPER"
grep -q '^preflight_alpine_initramfs()' "$HELPER"
grep -q '^adaptive_alpine_initramfs_repair()' "$HELPER"
grep -q '^preflight_extlinux()' "$HELPER"
grep -q '^adaptive_extlinux_repair()' "$HELPER"
grep -q '^guard_extlinux_candidate_preserves_entries()' "$HELPER"
grep -q '^detect_alpine_display_manager()' "$HELPER"
grep -q '^preflight_alpine_display_manager()' "$HELPER"
grep -q '^adaptive_alpine_display_manager_repair()' "$HELPER"
grep -q 'Arch profile — guarded pacman/mkinitcpio/GRUB/EFI repairs' "$HELPER"
grep -q 'diagnostic_backend_profile()' "$HELPER"
grep -q 'mount_target_boot_entry "/efi" ro' "$HELPER"
grep -q '^preflight_arch_pacman_transaction()' "$HELPER"
grep -q '^arch_pacman_transaction_reported_no_changes()' "$HELPER"
grep -q '^adaptive_arch_pacman_repair()' "$HELPER"
grep -q '^preflight_arch_initramfs()' "$HELPER"
grep -q '^adaptive_arch_initramfs_repair()' "$HELPER"
grep -q 'grub-mkconfig with an isolated output path' "$HELPER"
grep -q '^realpath_existing()' "$HELPER"
grep -q '^target_path()' "$HELPER"
grep -q '^validate_selected_esp()' "$HELPER"
grep -q '^efi_selected_generic_loader()' "$HELPER"
grep -q '^efi_ensure_selected_generic_entry()' "$HELPER"
grep -q 'efi_ensure_selected_generic_entry' "$HELPER"
grep -q '^alpine_efi_firmware_available()' "$HELPER"
grep -q '^alpine_efi_backend()' "$HELPER"
grep -q '^alpine_grub_install_present()' "$HELPER"
grep -q '^alpine_grub_module_dir_present()' "$HELPER"
grep -q '^alpine_grub_config_tool_present()' "$HELPER"
grep -q '^alpine_efi_esp_kernel_images()' "$HELPER"
grep -q '^alpine_efi_esp_initramfs_images()' "$HELPER"
grep -q '^alpine_efi_stub_entry_ids_for_partuuid()' "$HELPER"
grep -q '^alpine_efi_backup_state()' "$HELPER"
grep -q '^alpine_efi_restore_backup()' "$HELPER"
grep -q '^alpine_efi_refresh_fallback_loader()' "$HELPER"
grep -q '^alpine_efi_stub_preflight()' "$HELPER"
grep -q '^alpine_efi_stub_reconcile()' "$HELPER"
grep -q '^alpine_grub_efi_apply()' "$HELPER"
grep -q '^alpine_grub_efi_repair()' "$HELPER"
grep -q '^adaptive_alpine_efi_repair()' "$HELPER"
grep -q -- '--boot-directory=/boot' "$HELPER"
grep -q -- '--no-nvram' "$HELPER"
grep -q '^target_rpm_ready()' "$HELPER"
grep -q '^target_dnf5_ready()' "$HELPER"
grep -q '^rpm_database_present()' "$HELPER"
grep -q '^rpm_repositories_present()' "$HELPER"
grep -q '^rpm_dnf_tool()' "$HELPER"
grep -q '^rpm_lock_probe_available()' "$HELPER"
grep -q '^rpm_lock_held()' "$HELPER"
grep -q '^rpm_database_fingerprint()' "$HELPER"
grep -q '^rpm_preflight()' "$HELPER"
grep -q '^rpm_transaction_try()' "$HELPER"
grep -q '^rpm_simulation_is_safe()' "$HELPER"
grep -q '^rpm_transaction_reported_no_changes()' "$HELPER"
grep -q '^rpm_verify_missing_paths()' "$HELPER"
grep -q '^rpm_missing_file_packages()' "$HELPER"
grep -q '^adaptive_rpm_apply()' "$HELPER"
grep -q '^adaptive_rpm_stage()' "$HELPER"
grep -q '^adaptive_rpm_fix_broken()' "$HELPER"
grep -q '^adaptive_rpm_metadata_refresh()' "$HELPER"
grep -q '^adaptive_rpm_upgrade()' "$HELPER"
grep -q '^rpm_kernel_pairs()' "$HELPER"
grep -q '^rpm_kernel_pairs_readonly()' "$HELPER"
grep -q '^dracut_initramfs_verify()' "$HELPER"
grep -q '^dracut_initramfs_verify_rc()' "$HELPER"
grep -q '^preflight_dracut_initramfs()' "$HELPER"
grep -q '^adaptive_dracut_initramfs_repair()' "$HELPER"
grep -q 'Fedora profile — guarded rpm/dnf5/dracut repairs' "$HELPER"
grep -q 'Fedora policy: guarded rpm/dnf5 package transactions and dracut initramfs rebuilds' "$HELPER"
grep -q '^grub_generator_tool()' "$HELPER"
grep -q '^grub_config_path()' "$HELPER"
grep -q '^grub_script_check_tool()' "$HELPER"
grep -q '^grub_install_tool()' "$HELPER"
grep -q '^grub_editenv_tool()' "$HELPER"
grep -q '^grub_env_path()' "$HELPER"
grep -q '^grub_env_block_valid()' "$HELPER"
grep -q '^grub2_layout_detected()' "$HELPER"
grep -q '^grub_artifact_fingerprint()' "$HELPER"
grep -q '^fedora_bls_entry_keys()' "$HELPER"
grep -q '^guard_fedora_bls_entries_preserved()' "$HELPER"
grep -q '^preflight_fedora_grub()' "$HELPER"
grep -q '^adaptive_fedora_grub_repair()' "$HELPER"
grep -q '^adaptive_grub_stage()' "$HELPER"
grep -q '^fedora_bios_grub_partition()' "$HELPER"
grep -q '^fedora_grub_boot_code_broken()' "$HELPER"
grep -q '^partition_table_fingerprint()' "$HELPER"
grep -q '^fedora_grub_boot_fingerprint()' "$HELPER"
grep -q '^preflight_fedora_grub_reinstall()' "$HELPER"
grep -q '^fedora_grub_reinstall_boot_code()' "$HELPER"
grep -q '^fedora_grub_backup_boot_state()' "$HELPER"
grep -q '^fedora_grub_restore_boot_backup()' "$HELPER"
grep -q '^fedora_bootstack_pairing_check()' "$HELPER"
grep -q -- '--no-grubenv-update' "$HELPER"
grep -q -- '--target=i386-pc --boot-directory=/boot --recheck' "$HELPER"
grep -q '^diagnostic_package_manager_logs()' "$HELPER"
grep -q 'dnf5 package-manager log errors' "$HELPER"
grep -q '^package_log_filter()' "$HELPER"
grep -q '^target_journal_evidence_present()' "$HELPER"
grep -q '^apt_lists_fingerprint()' "$HELPER"
grep -q '^rpm_metadata_cache_fingerprint()' "$HELPER"
grep -q '^grub_entry_is_foreign()' "$HELPER"
grep -q '/etc/gdm/custom.conf' "$HELPER"
grep -q 'legacy BIOS target; no EFI boot path is available' "$HELPER"

# The guarded Fedora GRUB2 reinstall must never pass a policy-relaxing
# grub2-install flag, never call efibootmgr and never touch a partition path.
fedora_reinstall_body="$(sed -n '/^fedora_grub_reinstall_boot_code()/,/^}/p' "$HELPER")"
[[ -n "$fedora_reinstall_body" ]] || { echo 'FAIL: fedora_grub_reinstall_boot_code is missing' >&2; exit 1; }
if grep -Eq -- '--force|--allow-floppy|--skip-fs-probe|--removable|efibootmgr|efivar' <<<"$fedora_reinstall_body"; then
    echo 'FAIL: the Fedora GRUB2 reinstall passes a forbidden flag or touches EFI variables' >&2
    exit 1
fi
grep -q -- '--target=i386-pc --boot-directory=/boot --recheck "\$TARGET_DISK"' <<<"$fedora_reinstall_body" \
    || { echo 'FAIL: the Fedora GRUB2 reinstall does not target the selected disk' >&2; exit 1; }

# Boot-stack reconciliation must be dracut -> GRUB2 config-only on Fedora BIOS:
# the fail-closed pairing check runs before the non-destructive GRUB stage.
bootstack_body="$(sed -n '/^repair_boot_stack()/,/^}/p' "$HELPER")"
grep -q 'fedora_bootstack_pairing_check' <<<"$bootstack_body" \
    || { echo 'FAIL: Fedora boot-stack pairing check is not wired' >&2; exit 1; }
grep -q 'adaptive_grub_stage config-only' <<<"$bootstack_body" \
    || { echo 'FAIL: Fedora boot-stack is not config-only for GRUB2' >&2; exit 1; }
pairing_line="$(grep -n 'fedora_bootstack_pairing_check' <<<"$bootstack_body" | head -n1 | cut -d: -f1)"
grub_line="$(grep -n 'adaptive_grub_stage config-only' <<<"$bootstack_body" | head -n1 | cut -d: -f1)"
[[ -n "$pairing_line" && -n "$grub_line" && "$pairing_line" -lt "$grub_line" ]] \
    || { echo 'FAIL: Fedora boot-stack does not pair kernels before GRUB reconciliation' >&2; exit 1; }
grep -q 'grub2_layout_detected && bios_firmware_mode' <<<"$bootstack_body" \
    || { echo 'FAIL: Fedora BIOS boot-stack branch does not probe layout + firmware' >&2; exit 1; }

# Exercise the profiler against a synthetic Arch root without touching a real
# filesystem or invoking a package manager.
fake_root="$(mktemp -d)"
mkdir -p "$fake_root"/usr/bin "$fake_root"/etc/mkinitcpio.d \
    "$fake_root"/boot/efi/EFI/Linux "$fake_root"/boot/loader
touch "$fake_root/usr/bin/pacman" "$fake_root/usr/bin/mkinitcpio" "$fake_root/usr/bin/bootctl" \
    "$fake_root/boot/vmlinuz-linux" "$fake_root/boot/initramfs-linux.img"
chmod +x "$fake_root/usr/bin"/*
source <(sed '/^main "\$@"/d' "$HELPER")
trap 'rm -rf -- "$fake_root"' EXIT
TARGET_ROOT="$fake_root"
TARGET_OS_ID="arch"
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=""
profile_target_backends
[[ "$TARGET_DISTRO_FAMILY" == arch ]]
[[ "$TARGET_PACKAGE_MANAGER" == pacman ]]
[[ "$TARGET_INITRAMFS_BACKEND" == mkinitcpio ]]
[[ "$TARGET_BOOTLOADER_BACKEND" == 'systemd-boot + UKI' ]]
[[ "$TARGET_ESP_MOUNT" == /boot/efi ]]
[[ "$TARGET_KERNEL_LAYOUT" == 'Arch-style named kernels (vmlinuz-linux*)' ]]
[[ "$TARGET_REPAIR_BACKEND" == 'Arch profile — guarded pacman/mkinitcpio/GRUB/EFI repairs when transaction preflights pass' ]]

# realpath_existing must canonicalize existing paths on GNU hosts and fall back
# to readlink -f on BusyBox hosts (base Alpine has no realpath -e).
(
    realpath() { return 1; }
    resolved="$(realpath_existing "$fake_root")"
    [[ "$resolved" == "$(readlink -f -- "$fake_root")" ]] \
        || { echo 'FAIL: realpath_existing BusyBox fallback did not canonicalize an existing path' >&2; exit 1; }
    if realpath_existing "$fake_root/does-not-exist" >/dev/null 2>&1; then
        echo 'FAIL: realpath_existing accepted a missing path' >&2
        exit 1
    fi
)

# target_path must never produce a double slash for the running-host root,
# which BusyBox mountpoint rejects as "not a mountpoint".
(
    TARGET_ROOT=/
    [[ "$(target_path /boot/efi)" == /boot/efi ]] \
        || { echo 'FAIL: target_path produced a non-canonical running-host path' >&2; exit 1; }
    TARGET_ROOT=/mnt/root
    [[ "$(target_path /boot/efi)" == /mnt/root/boot/efi ]] \
        || { echo 'FAIL: target_path did not join a mounted target root' >&2; exit 1; }
)

# Btrfs root discovery must accept a symlinked /etc/os-release (Fedora links it
# to ../usr/lib/os-release) while still requiring readable os-release evidence.
btrfs_probe_root="$(mktemp -d)"
mkdir -p "$btrfs_probe_root/root/etc" "$btrfs_probe_root/root/usr/lib" "$btrfs_probe_root/home"
printf 'ID=fedora\n' > "$btrfs_probe_root/root/usr/lib/os-release"
ln -s ../usr/lib/os-release "$btrfs_probe_root/root/etc/os-release"
[[ "$(find_btrfs_root "$btrfs_probe_root")" == "$btrfs_probe_root/root" ]] \
    || { echo 'FAIL: symlinked btrfs os-release root was not resolved' >&2; exit 1; }
rm -f "$btrfs_probe_root/root/etc/os-release"
if find_btrfs_root "$btrfs_probe_root" >/dev/null 2>&1; then
    echo 'FAIL: a btrfs tree without os-release evidence was accepted' >&2
    exit 1
fi
rm -rf -- "$btrfs_probe_root"

rm -rf -- "${fake_root:?}/usr/bin"/* "${fake_root:?}/etc/mkinitcpio.d" "${fake_root:?}/boot/loader"
mkdir -p "$fake_root/etc/initramfs-tools" "$fake_root/boot/grub"
touch "$fake_root/usr/bin/apt-get" "$fake_root/usr/bin/update-initramfs" \
    "$fake_root/usr/bin/grub-mkconfig" "$fake_root/boot/vmlinuz-6.1-test" \
    "$fake_root/boot/initrd.img-6.1-test" "$fake_root/boot/grub/grub.cfg"
chmod +x "$fake_root/usr/bin"/*
TARGET_OS_ID="tuxedo"
TARGET_OS_LIKE="debian"
TARGET_DISTRO_FAMILY=""
profile_target_backends
[[ "$TARGET_DISTRO_FAMILY" == debian ]]
[[ "$TARGET_PACKAGE_MANAGER" == 'apt/dpkg' ]]
[[ "$TARGET_INITRAMFS_BACKEND" == initramfs-tools ]]
[[ "$TARGET_BOOTLOADER_BACKEND" == grub ]]
[[ "$TARGET_REPAIR_BACKEND" == 'Debian/APT profile — existing guarded modifying backend' ]]

# A stray dracut executable (TUXEDO OS ships dracut-core leftovers next to a
# real initramfs-tools installation) must not override the installed backend.
touch "$fake_root/usr/bin/dracut"
chmod +x "$fake_root/usr/bin/dracut"
TARGET_OS_ID="tuxedo"
TARGET_OS_LIKE="debian"
TARGET_DISTRO_FAMILY=""
profile_target_backends
[[ "$TARGET_INITRAMFS_BACKEND" == initramfs-tools ]]
rm -f "$fake_root/usr/bin/dracut"

# The same holds for dracut-core leftovers (generator directory and
# configuration without the dracut metapackage): initramfs-tools stays active.
mkdir -p "$fake_root/usr/lib/dracut" "$fake_root/etc/dracut.conf.d"
: > "$fake_root/etc/dracut.conf"
profile_target_backends
[[ "$TARGET_INITRAMFS_BACKEND" == initramfs-tools ]]
rm -rf "$fake_root/usr/lib/dracut" "$fake_root/etc/dracut.conf" "$fake_root/etc/dracut.conf.d"

# A genuine dracut installation (generator directory plus configuration)
# selects dracut when initramfs-tools is absent.
dracut_root="$(mktemp -d)"
trap 'rm -rf -- "$fake_root" "$dracut_root"' EXIT
mkdir -p "$dracut_root/usr/bin" "$dracut_root/usr/lib/dracut" "$dracut_root/etc/dracut.conf.d"
: > "$dracut_root/usr/bin/dracut"; chmod +x "$dracut_root/usr/bin/dracut"
: > "$dracut_root/etc/dracut.conf"
TARGET_ROOT="$dracut_root"
TARGET_OS_ID="tuxedo"
TARGET_OS_LIKE="debian"
TARGET_DISTRO_FAMILY=""
profile_target_backends
[[ "$TARGET_DISTRO_FAMILY" == debian ]]
[[ "$TARGET_INITRAMFS_BACKEND" == dracut ]]

# dpkg package state alone is enough to recognise an installed dracut.
dracut_pkg_root="$(mktemp -d)"
trap 'rm -rf -- "$fake_root" "$dracut_root" "$dracut_pkg_root"' EXIT
mkdir -p "$dracut_pkg_root/usr/bin" "$dracut_pkg_root/var/lib/dpkg"
: > "$dracut_pkg_root/usr/bin/dracut"; chmod +x "$dracut_pkg_root/usr/bin/dracut"
printf 'Package: dracut\nStatus: install ok installed\nVersion: 060+5-1\n\n' > "$dracut_pkg_root/var/lib/dpkg/status"
TARGET_ROOT="$dracut_pkg_root"
TARGET_OS_ID="debian"
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=""
profile_target_backends
[[ "$TARGET_INITRAMFS_BACKEND" == dracut ]]
TARGET_ROOT="$fake_root"

# Exercise the transaction safety policy independently of pacman itself.  A
# successful no-op transaction is accepted; removals, dependency failures and
# an unexpectedly large transaction are rejected before any target write.
SESSION_LOG="$fake_root/contract.log"
arch_pacman_transaction_is_safe $'there is nothing to do' \
    || { echo "FAIL: safe pacman transaction was rejected" >&2; exit 1; }
if arch_pacman_transaction_is_safe $'Packages (1001):\n removing unsafe-package'; then
    echo "FAIL: unsafe pacman transaction was accepted" >&2
    exit 1
fi
if arch_pacman_transaction_is_safe $'cannot resolve dependencies'; then
    echo "FAIL: unresolved pacman transaction was accepted" >&2
    exit 1
fi
mirror_log=$'error: failed retrieving file \'extra.db\' from fastly.mirror.pkgbuild.com : OpenSSL EOF'
arch_pacman_transaction_is_safe "$mirror_log" 0 || { echo 'FAIL: recoverable mirror failure rejected' >&2; exit 1; }
arch_pacman_transaction_is_safe "$mirror_log" 1 && { echo 'FAIL: nonzero pacman rc accepted' >&2; exit 1; }
grep -Fq 'WARN: pacman encountered 1 recoverable mirror retrieval failure(s).' "$SESSION_LOG"
grep -Fq 'WARN: failing mirror: fastly.mirror.pkgbuild.com' "$SESSION_LOG"
for fatal in 'error: failed to synchronize all databases' 'error: invalid or corrupted package' 'error: invalid or corrupted database' 'error: required key missing from keyring' 'error: invalid signature' 'error: unknown trust' 'error: conflicting files' 'error: could not satisfy dependencies' 'error: failed to init transaction' 'error: failed to prepare transaction' 'error: failed to commit transaction' 'error: unexpected failure'; do
    arch_pacman_transaction_is_safe "$fatal" 0 && { echo "FAIL: fatal pacman error accepted: $fatal" >&2; exit 1; }
done
multiple_mirrors=$'error: failed retrieving file \'extra.db\' from fastly.mirror.pkgbuild.com : EOF\nerror: failed retrieving file \'core.db\' from another.mirror.example : EOF'
arch_pacman_transaction_is_safe "$multiple_mirrors" 0 || { echo 'FAIL: multiple recoverable mirror failures rejected' >&2; exit 1; }
grep -Fq 'WARN: pacman encountered 2 recoverable mirror retrieval failure(s).' "$SESSION_LOG"
grep -Fq 'WARN: failing mirror: another.mirror.example' "$SESSION_LOG"
combined=$'error: failed retrieving file \'extra.db\' from fastly.mirror.pkgbuild.com : EOF\nerror: failed to synchronize all databases'
arch_pacman_transaction_is_safe "$combined" 0 && { echo 'FAIL: mirror plus fatal sync error accepted' >&2; exit 1; }

# No-op detection: only a transaction that printed "there is nothing to do"
# with no package action may report unchanged.
arch_pacman_transaction_reported_no_changes $':: Synchronizing package databases...\n core is up to date\n:: Starting full system upgrade...\n there is nothing to do\n' \
    || { echo 'FAIL: pacman no-op transaction was not recognized' >&2; exit 1; }
arch_pacman_transaction_reported_no_changes $'Packages (1) example-1.0-1 -> 2.0-1\nTotal Installed Size: 1.00 MiB\n' \
    && { echo 'FAIL: pacman package upgrade was treated as unchanged' >&2; exit 1; }
arch_pacman_transaction_reported_no_changes $'Packages (2) example-1.0-1 -> 2.0-1  other-1.0-1\n(1/2) upgrading example\n(2/2) installing other\n' \
    && { echo 'FAIL: pacman package actions were treated as unchanged' >&2; exit 1; }
arch_pacman_transaction_reported_no_changes $'Packages (1) example-1.0-1\n(1/1) removing example\n' \
    && { echo 'FAIL: pacman package removal was treated as unchanged' >&2; exit 1; }
arch_pacman_transaction_reported_no_changes $'there is nothing to do\nPackages (1) example-1.0-1 -> 2.0-1\n' \
    && { echo 'FAIL: a transaction with package changes plus no-op text was treated as unchanged' >&2; exit 1; }

mkdir -p "$fake_root/var/lib/pacman/local/example-1" "$fake_root/var/lib/pacman/sync"
printf 'installed package state\n' > "$fake_root/var/lib/pacman/local/example-1/desc"
printf 'stale repository\n' > "$fake_root/var/lib/pacman/sync/core.db"
touch "$fake_root/etc/pacman.conf" "$fake_root/usr/bin/pacman"
chmod +x "$fake_root/usr/bin/pacman"
TARGET_DISTRO_FAMILY=arch
TARGET_PACKAGE_MANAGER=pacman
SESSION_DIR="$fake_root/session.contract"
arch_pacman_prepare_sandbox
cmp "$fake_root/var/lib/pacman/local/example-1/desc" "$TARGET_ROOT$ARCH_PACMAN_DB_REL/local/example-1/desc"
[[ -d "$TARGET_ROOT$ARCH_PACMAN_DB_REL/sync" ]]
[[ -z "$(ls -A "$TARGET_ROOT$ARCH_PACMAN_DB_REL/sync")" ]]
[[ -s "$fake_root/var/lib/pacman/sync/core.db" ]]
grep -q '"Arch pacman full transaction preflight" -Syyuw --noconfirm' "$HELPER"

grep -q '^diagnostic_repair_capabilities()' "$HELPER"
grep -q 'diagnostic_repair_capabilities$' "$HELPER"
grep -Fq "printf 'Repair tool %s: available\\n' \"\$key\"" "$HELPER"
grep -Fq "printf 'Repair tool %s: unavailable|%s\\n' \"\$key\"" "$HELPER"

# Combined Run All reports share one capability preamble at the top; only the
# individual diagnostic path emits it inline.
grep -q '^REPORT_SHARED_PREAMBLE=0$' "$HELPER"
grep -q 'if (( REPORT_SHARED_PREAMBLE == 0 )); then' "$HELPER"
for report_fn in run_target_diagnostic run_host_diagnostic; do
    report_body="$(sed -n "/^$report_fn()/,/^}/p" "$HELPER")"
    [[ -n "$report_body" ]] || { echo "FAIL: $report_fn is missing" >&2; exit 1; }
    grep -q 'REPORT_SHARED_PREAMBLE=1' <<<"$report_body" \
        || { echo "FAIL: $report_fn does not mark the combined report preamble" >&2; exit 1; }
    grep -q 'diagnostic_repair_capabilities' <<<"$report_body" \
        || { echo "FAIL: $report_fn does not emit the shared capability preamble" >&2; exit 1; }
    grep -q 'REPORT_SHARED_PREAMBLE=0' <<<"$report_body" \
        || { echo "FAIL: $report_fn does not restore per-section capability output" >&2; exit 1; }
done

cap_assert_lines()
{
    local output
    output="$(cat)"
    [[ "$(sed -n '1p' <<<"$output")" == 'Repair capability probes (read-only, selected target):' ]]
    [[ "$(grep -c '^Repair tool ' <<<"$output")" -eq 13 ]]
    [[ "$(grep -c '^Repair capability evidence [a-z]*: ' <<<"$output")" -eq 13 ]]
    grep -Fqx 'Repair capability evidence (read-only):' <<<"$output"
    local -a keys=(validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub extlinux bootstack)
    local idx
    for idx in "${!keys[@]}"; do
        sed -n "$((idx + 2))p" <<<"$output" | grep -Fqx "Repair tool ${keys[$idx]}: available" \
            || sed -n "$((idx + 2))p" <<<"$output" | grep -Eq "^Repair tool ${keys[$idx]}: unavailable\|[^|]+\$"
    done
}

cap_expect()
{
    local output="$1"; shift
    local line
    for line in "$@"; do
        grep -Fqx "Repair tool $line" <<<"$output" \
            || { echo "FAIL: missing capability line 'Repair tool $line'" >&2; exit 1; }
    done
}

cap_root="$(mktemp -d)"
trap 'rm -rf -- "$fake_root" "$dracut_root" "$dracut_pkg_root" "$cap_root"' EXIT

cap_expect \
    "$(TARGET_ROOT="$cap_root" TARGET_OS_ID=arch TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=arch \
        TARGET_INITRAMFS_BACKEND=mkinitcpio TARGET_BOOTLOADER_BACKEND=grub \
        diagnostic_repair_capabilities)" \
    'validate: available' \
    'dpkg: unavailable|dpkg configuration is not available on Arch; use the Arch package transaction stages instead' \
    'fixbroken: unavailable|No guarded package-manager backend was detected (detected: none)' \
    'aptupdate: unavailable|Standalone APT metadata refresh is not available on Arch; use Upgrade installed packages for one full pacman transaction' \
    'upgrade: unavailable|No guarded package-manager backend was detected (detected: none)' \
    'dkms: unavailable|DKMS is not installed in the Arch target system' \
    'display: unavailable|No supported service manager (systemd or OpenRC) was detected in the target' \
    'initramfs: unavailable|No supported initramfs backend (mkinitfs, mkinitcpio, dracut or initramfs-tools) was detected' \
    'efi: unavailable|requires GRUB configuration tooling' \
    'grub: unavailable|Neither grub-mkconfig nor update-grub is installed in the target system' \
    'extlinux: unavailable|update-extlinux is not installed in the target' \
    'bootstack: unavailable|Requires available initramfs and GRUB repair prerequisites'

mkdir -p "$cap_root"/usr/bin "$cap_root"/usr/lib/modules/6.12.1-arch1-1/build \
    "$cap_root"/usr/lib/systemd/system "$cap_root"/boot/grub "$cap_root"/etc \
    "$cap_root"/var/lib/pacman/local "$cap_root"/boot/efi/EFI
for cap_tool in pacman mkinitcpio dkms grub-mkconfig grub-install; do
    : > "$cap_root/usr/bin/$cap_tool"; chmod +x "$cap_root/usr/bin/$cap_tool"
done
: > "$cap_root/usr/lib/systemd/system/graphical.target"
: > "$cap_root/usr/lib/systemd/system/lightdm.service"
: > "$cap_root/etc/pacman.conf"
: > "$cap_root/boot/vmlinuz-linux"
cap_expect \
    "$(TARGET_ROOT="$cap_root" TARGET_OS_ID=arch TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=arch \
        TARGET_INITRAMFS_BACKEND=mkinitcpio TARGET_BOOTLOADER_BACKEND=grub \
        diagnostic_repair_capabilities)" \
    'validate: available' \
    'fixbroken: available' \
    'upgrade: available' \
    'dkms: available' \
    'display: available' \
    'initramfs: available' \
    'efi: available' \
    'grub: available' \
    'bootstack: available'

full_cap="$(TARGET_ROOT="$cap_root" TARGET_OS_ID=arch TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=arch \
    TARGET_INITRAMFS_BACKEND=mkinitcpio TARGET_BOOTLOADER_BACKEND=grub diagnostic_repair_capabilities)"
grep -Fqx 'Repair capability evidence dkms: dkms executable present; kernel 6.12.1-arch1-1 build tree present' <<<"$full_cap"
grep -Fqx 'Repair capability evidence display: graphical.target present; display manager unit lightdm.service' <<<"$full_cap"
grep -Fqx 'Repair capability evidence efi: grub-install present; ESP mount candidate: /boot/efi' <<<"$full_cap"
grep -Fqx 'Repair capability evidence initramfs: mkinitcpio executable present; kernel module directories: 6.12.1-arch1-1' <<<"$full_cap"
grep -Fqx 'Repair capability evidence fixbroken: pacman executable present; /etc/pacman.conf present; /var/lib/pacman present' <<<"$full_cap"
grep -Fqx 'Repair capability evidence extlinux: update-extlinux is not installed in the target' <<<"$full_cap"

cap_expect \
    "$(TARGET_ROOT="$cap_root" TARGET_OS_ID=arch TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY="" \
        TARGET_INITRAMFS_BACKEND=mkinitcpio TARGET_BOOTLOADER_BACKEND=grub \
        diagnostic_repair_capabilities)" \
    'fixbroken: available' \
    'upgrade: available' \
    'dpkg: unavailable|dpkg configuration is not available on Arch; use the Arch package transaction stages instead'

# A single diagnostic stays self-contained: exactly one capability preamble
# with every key line present.
individual_report="$(TARGET_ROOT="$cap_root" TARGET_OS_ID=arch TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY="" \
    TARGET_INITRAMFS_BACKEND=mkinitcpio TARGET_BOOTLOADER_BACKEND=grub \
    run_one_diagnostic backend "Repair Target")"
[[ "$(grep -c '^Repair capability probes (read-only, selected target):$' <<<"$individual_report")" -eq 1 ]] \
    || { echo 'FAIL: individual diagnostic did not emit exactly one capability preamble' >&2; exit 1; }
[[ "$(grep -c '^Repair capability evidence (read-only):$' <<<"$individual_report")" -eq 1 ]] \
    || { echo 'FAIL: individual diagnostic did not emit exactly one capability evidence header' >&2; exit 1; }
[[ "$(grep -c '^Repair tool ' <<<"$individual_report")" -eq 13 ]] \
    || { echo 'FAIL: individual diagnostic lost capability key lines' >&2; exit 1; }
[[ "$(grep -c '^Repair capability evidence [a-z]*: ' <<<"$individual_report")" -eq 13 ]] \
    || { echo 'FAIL: individual diagnostic lost capability evidence lines' >&2; exit 1; }

# A combined Run All report emits the shared capability preamble once at the
# top and omits it from every diagnostic section. Section bodies are stubbed so
# the composition contract is independent of the individual diagnostics, and
# both the target and running-host combined paths are covered.
stub_combined_diagnostic_sections()
{
    prepare_target() { :; }
    prepare_running_host() { :; }
    mount_target_boot_entry() { :; }
    diagnostic_environment() { printf 'SECTION environment\n'; }
    diagnostic_backend_profile() { printf 'SECTION backend\n'; }
    diagnostic_boot() { printf 'SECTION boot\n'; }
    diagnostic_boot_evidence() { printf 'SECTION boot-evidence\n'; }
    diagnostic_kernel() { printf 'SECTION kernel\n'; }
    diagnostic_grub() { printf 'SECTION grub\n'; }
    diagnostic_uki() { printf 'SECTION uki\n'; }
    diagnostic_display() { printf 'SECTION display\n'; }
    diagnostic_errors() { printf 'SECTION errors\n'; }
    diagnostic_usage() { printf 'SECTION usage\n'; }
    diagnostic_fstab() { printf 'SECTION fstab\n'; }
    diagnostic_btrfs() { printf 'SECTION btrfs\n'; }
    diagnostic_mapper() { printf 'SECTION mapper\n'; }
    diagnostic_luks() { printf 'SECTION luks\n'; }
}

assert_combined_report()
{
    local label="$1" combined_report="$2" cap_key
    local probes_header_line evidence_header_line first_divider_line
    probes_header_line="$(grep -n '^Repair capability probes (read-only, selected target):$' <<<"$combined_report" | head -n1 | cut -d: -f1 || true)"
    evidence_header_line="$(grep -n '^Repair capability evidence (read-only):$' <<<"$combined_report" | head -n1 | cut -d: -f1 || true)"
    first_divider_line="$(grep -n '^========================================$' <<<"$combined_report" | head -n1 | cut -d: -f1 || true)"
    [[ -n "$probes_header_line" && -n "$evidence_header_line" && -n "$first_divider_line" \
        && "$probes_header_line" -lt "$evidence_header_line" && "$evidence_header_line" -lt "$first_divider_line" ]] \
        || { echo "FAIL: $label combined report capability preamble is not before the first section" >&2; exit 1; }
    [[ "$(grep -c '^Repair capability probes (read-only, selected target):$' <<<"$combined_report")" -eq 1 ]] \
        || { echo "FAIL: $label combined report emitted duplicate capability preambles" >&2; exit 1; }
    [[ "$(grep -c '^Repair capability evidence (read-only):$' <<<"$combined_report")" -eq 1 ]] \
        || { echo "FAIL: $label combined report emitted duplicate capability evidence headers" >&2; exit 1; }
    [[ "$(grep -c '^Repair tool ' <<<"$combined_report")" -eq 13 ]] \
        || { echo "FAIL: $label combined report lost capability key lines" >&2; exit 1; }
    [[ "$(grep -c '^Repair capability evidence [a-z]*: ' <<<"$combined_report")" -eq 13 ]] \
        || { echo "FAIL: $label combined report lost capability evidence lines" >&2; exit 1; }
    [[ "$(grep -c '^SECTION ' <<<"$combined_report")" -eq 14 ]] \
        || { echo "FAIL: $label combined report did not run every diagnostic section" >&2; exit 1; }
    [[ "$(grep -c '^Diagnostic: ' <<<"$combined_report")" -eq 14 ]] \
        || { echo "FAIL: $label combined report diagnostic section headers changed" >&2; exit 1; }
    for cap_key in validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub extlinux bootstack; do
        [[ "$(grep -c "^Repair tool $cap_key: " <<<"$combined_report")" -eq 1 ]] \
            || { echo "FAIL: $label combined report does not contain exactly one 'Repair tool $cap_key' line" >&2; exit 1; }
    done
}

combined_reports="$(
    TARGET_ROOT="$cap_root" TARGET_OS_ID=arch TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=arch \
    TARGET_INITRAMFS_BACKEND=mkinitcpio TARGET_BOOTLOADER_BACKEND=grub \
    TARGET_DISK=/dev/test-disk ROOT_DEVICE=/dev/test-root \
    REPORT_SHARED_PREAMBLE=0
    stub_combined_diagnostic_sections
    run_target_diagnostic all
    printf '\n=====HOST COMBINED REPORT=====\n'
    run_host_diagnostic all
)"
target_combined="${combined_reports%%=====HOST COMBINED REPORT=====*}"
host_combined="${combined_reports#*=====HOST COMBINED REPORT=====}"
assert_combined_report target "$target_combined"
assert_combined_report host "$host_combined"

cap_arch_min="$(mktemp -d)"
trap 'rm -rf -- "$fake_root" "$dracut_root" "$dracut_pkg_root" "$cap_root" "$cap_arch_min"' EXIT
mkdir -p "$cap_arch_min"/usr/bin "$cap_arch_min"/etc "$cap_arch_min"/usr/lib/systemd/system "$cap_arch_min"/boot
: > "$cap_arch_min/usr/bin/pacman"; chmod +x "$cap_arch_min/usr/bin/pacman"
: > "$cap_arch_min/etc/pacman.conf"
: > "$cap_arch_min/usr/lib/systemd/system/graphical.target"
cap_arch_probe()
{
    TARGET_ROOT="$cap_arch_min" TARGET_OS_ID=arch TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=arch \
        TARGET_INITRAMFS_BACKEND=mkinitcpio TARGET_BOOTLOADER_BACKEND=grub \
        diagnostic_repair_capabilities
}
cap_expect "$(cap_arch_probe)" \
    'fixbroken: unavailable|The target pacman database directory is missing' \
    'upgrade: unavailable|The target pacman database directory is missing' \
    'display: unavailable|No supported service manager (systemd or OpenRC) was detected in the target' \
    'initramfs: unavailable|No supported initramfs backend (mkinitfs, mkinitcpio, dracut or initramfs-tools) was detected' \
    'efi: unavailable|requires GRUB configuration tooling'

mkdir -p "$cap_arch_min"/var/lib/pacman/local
cap_expect "$(cap_arch_probe)" 'fixbroken: available' 'upgrade: available'

: > "$cap_arch_min/usr/lib/systemd/system/lightdm.service"
cap_expect "$(cap_arch_probe)" 'display: available'

mkdir -p "$cap_arch_min"/usr/lib/modules/6.12.1-arch1-1
: > "$cap_arch_min/usr/bin/mkinitcpio"; chmod +x "$cap_arch_min/usr/bin/mkinitcpio"
cap_expect "$(cap_arch_probe)" 'initramfs: available'

for cap_tool in grub-mkconfig grub-install; do
    : > "$cap_arch_min/usr/bin/$cap_tool"; chmod +x "$cap_arch_min/usr/bin/$cap_tool"
done
cap_expect "$(cap_arch_probe)" \
    'grub: available' \
    'efi: unavailable|No EFI System Partition was identified for the selected Arch target' \
    'bootstack: unavailable|Arch boot-stack reconciliation requires available initramfs, GRUB and EFI repair prerequisites'

mkdir -p "$cap_arch_min"/boot/efi/EFI
cap_expect "$(cap_arch_probe)" 'efi: available' 'bootstack: available'

rm -rf -- "${cap_root:?}"/usr/bin/* "${cap_root:?}"/usr/lib/modules "${cap_root:?}"/boot/grub \
    "${cap_root:?}"/etc/pacman.conf "${cap_root:?}"/var/lib/pacman
mkdir -p "$cap_root"/usr/bin "$cap_root"/usr/lib/systemd/system "$cap_root"/boot \
    "$cap_root"/etc/apt "$cap_root"/etc/initramfs-tools
for cap_tool in apt-get dpkg-query; do
    : > "$cap_root/usr/bin/$cap_tool"; chmod +x "$cap_root/usr/bin/$cap_tool"
done
: > "$cap_root/usr/lib/systemd/system/graphical.target"
printf 'deb http://deb.example.invalid/ stable main\n' > "$cap_root/etc/apt/sources.list"
cap_expect \
    "$(TARGET_ROOT="$cap_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
        TARGET_INITRAMFS_BACKEND=initramfs-tools diagnostic_repair_capabilities)" \
    'validate: available' \
    'aptupdate: available' \
    'dpkg: unavailable|dpkg is not installed in the target' \
    'fixbroken: unavailable|dpkg is not installed in the target' \
    'upgrade: unavailable|dpkg is not installed in the target' \
    'dkms: unavailable|DKMS is not installed in the target' \
    'display: available' \
    'initramfs: unavailable|update-initramfs/mkinitramfs are not installed in the target' \
    'grub: unavailable|Neither grub-mkconfig nor update-grub is installed in the target system' \
    'efi: unavailable|TUXEDO UKI builder create_boot_uki_base.sh is not installed in the target' \
    'extlinux: unavailable|update-extlinux is not installed in the target' \
    'bootstack: unavailable|Requires available initramfs and GRUB repair prerequisites'

mkdir -p "$cap_root"/usr/bin "$cap_root"/usr/lib/systemd/system "$cap_root"/usr/sbin "$cap_root"/boot/grub \
    "$cap_root"/var/lib/dpkg
for cap_tool in dpkg apt-get dkms grub-mkconfig grub-install; do
    : > "$cap_root/usr/bin/$cap_tool"; chmod +x "$cap_root/usr/bin/$cap_tool"
done
printf 'Package: base-files\nStatus: install ok installed\nVersion: 1\n\n' > "$cap_root/var/lib/dpkg/status"
: > "$cap_root/usr/sbin/update-initramfs"; chmod +x "$cap_root/usr/sbin/update-initramfs"
: > "$cap_root/usr/sbin/mkinitramfs"; chmod +x "$cap_root/usr/sbin/mkinitramfs"
: > "$cap_root/usr/lib/systemd/system/graphical.target"
: > "$cap_root/boot/grub/grub.cfg"

# Without the vendor UKI builder, TUXEDO OS must not advertise EFI repair.
cap_tuxedo_no_builder="$(TARGET_ROOT="$cap_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
    TARGET_INITRAMFS_BACKEND=initramfs-tools diagnostic_repair_capabilities)"
cap_expect "$cap_tuxedo_no_builder" \
    'validate: available' 'dpkg: available' 'fixbroken: available' 'aptupdate: available' \
    'upgrade: available' 'dkms: available' 'display: available' 'initramfs: available' \
    'grub: available' \
    'efi: unavailable|TUXEDO UKI builder create_boot_uki_base.sh is not installed in the target' \
    'bootstack: available'
grep -Fqx 'Repair capability evidence initramfs: initramfs backend: initramfs-tools; update-initramfs and mkinitramfs present' <<<"$cap_tuxedo_no_builder"
grep -Fqx 'Repair capability evidence efi: TUXEDO UKI builder create_boot_uki_base.sh missing' <<<"$cap_tuxedo_no_builder"

# Adding the vendor builder makes EFI repair available and names it in evidence.
: > "$cap_root/usr/sbin/create_boot_uki_base.sh"; chmod +x "$cap_root/usr/sbin/create_boot_uki_base.sh"
cap_tuxedo_uki="$(TARGET_ROOT="$cap_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
    TARGET_INITRAMFS_BACKEND=initramfs-tools diagnostic_repair_capabilities)"
cap_expect "$cap_tuxedo_uki" 'efi: available' 'bootstack: available'
grep -Fqx 'Repair capability evidence efi: grub-install present; TUXEDO UKI builder create_boot_uki_base.sh present; ESP mount candidate: /boot/efi' <<<"$cap_tuxedo_uki"

# A configured dracut backend must never claim Debian initramfs repair.  The
# initramfs-tools evidence is removed first so the dracut probe is the one
# detected (native-first ordering would otherwise select initramfs-tools).
# Without lsinitrd the stage cannot verify images, so it fails closed with the
# exact missing prerequisite instead of the old "not implemented" refusal.
rm -rf -- "$cap_root/etc/initramfs-tools"
mkdir -p "$cap_root/usr/bin" "$cap_root/usr/lib/dracut"
: > "$cap_root/usr/bin/dracut"; chmod +x "$cap_root/usr/bin/dracut"
cap_dracut="$(TARGET_ROOT="$cap_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
    TARGET_INITRAMFS_BACKEND=dracut diagnostic_repair_capabilities)"
cap_expect "$cap_dracut" \
    'initramfs: unavailable|lsinitrd is not installed in the target system; dracut image verification is unavailable' \
    'bootstack: unavailable|Requires available initramfs and GRUB repair prerequisites'
grep -Fqx 'Repair capability evidence initramfs: initramfs backend: dracut; dracut executable and /usr/lib/dracut present; lsinitrd missing' <<<"$cap_dracut"

rm -rf -- "$cap_root/usr/lib/dracut"
cap_expect \
    "$(TARGET_ROOT="$cap_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
        TARGET_INITRAMFS_BACKEND=dracut diagnostic_repair_capabilities)" \
    'initramfs: unavailable|the dracut generator directory is missing from the target'

rm -f "$cap_root/usr/bin/dracut"
mkdir -p "$cap_root/usr/lib/dracut"
cap_expect \
    "$(TARGET_ROOT="$cap_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
        TARGET_INITRAMFS_BACKEND=dracut diagnostic_repair_capabilities)" \
    'initramfs: unavailable|dracut is not installed in the target system'

# A complete dracut installation without an installed kernel pair fails closed
# on the empty kernel inventory; adding the module/vmlinuz pair makes the
# guarded dracut rebuild available and names kernels/images in evidence.
: > "$cap_root/usr/bin/dracut"; chmod +x "$cap_root/usr/bin/dracut"
: > "$cap_root/usr/bin/lsinitrd"; chmod +x "$cap_root/usr/bin/lsinitrd"
cap_expect \
    "$(TARGET_ROOT="$cap_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
        TARGET_INITRAMFS_BACKEND=dracut diagnostic_repair_capabilities)" \
    'initramfs: unavailable|No installed dracut kernels were found under target /boot'
mkdir -p "$cap_root/lib/modules/6.1-test"
: > "$cap_root/boot/vmlinuz-6.1-test"
: > "$cap_root/boot/initramfs-6.1-test.img"
cap_dracut_ready="$(TARGET_ROOT="$cap_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
    TARGET_INITRAMFS_BACKEND=dracut diagnostic_repair_capabilities)"
cap_expect "$cap_dracut_ready" 'initramfs: available'
grep -Fqx 'Repair capability evidence initramfs: initramfs backend: dracut; dracut executable and /usr/lib/dracut present; kernels: 6.1-test; images: initramfs-6.1-test.img' <<<"$cap_dracut_ready"

rm -rf -- "$cap_root"
TARGET_ROOT="$cap_root" TARGET_OS_ID=fedora TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=fedora \
    TARGET_INITRAMFS_BACKEND=dracut diagnostic_repair_capabilities > "$fake_root/cap-unsupported.log"
cap_assert_lines < "$fake_root/cap-unsupported.log"
grep -Fqx 'Repair tool validate: available' "$fake_root/cap-unsupported.log"
# An unknown-family tree with no detected backend gets probe-based reasons
# instead of a blanket "unsupported distribution" message.
while read -r cap_key cap_reason; do
    [[ -n "$cap_key" ]] || continue
    grep -Fqx "Repair tool $cap_key: unavailable|$cap_reason" "$fake_root/cap-unsupported.log" \
        || { echo "FAIL: unsupported-family $cap_key reason changed" >&2; exit 1; }
done <<'UNSUPPORTED'
dpkg Fedora uses rpm/dnf; dpkg configuration is not available
fixbroken No guarded package-manager backend was detected (detected: none)
aptupdate dnf5 is not installed in the target
upgrade No guarded package-manager backend was detected (detected: none)
dkms DKMS is not installed in the target
display No supported service manager (systemd or OpenRC) was detected in the target
initramfs No supported initramfs backend (mkinitfs, mkinitcpio, dracut or initramfs-tools) was detected
efi requires GRUB configuration tooling
grub Neither grub-mkconfig nor update-grub is installed in the target system
extlinux update-extlinux is not installed in the target
bootstack Requires available initramfs and GRUB repair prerequisites
UNSUPPORTED

# ---------------------------------------------------------------------------
# Alpine is its own guarded-repair family: apk transactions, OpenRC runlevel
# repair, mkinitfs initramfs rebuilds and extlinux configuration regeneration
# are offered when their prerequisites are present, while dpkg/apt metadata,
# EFI and GRUB stages fail closed with Alpine-specific reasons.
# ---------------------------------------------------------------------------
cap_alpine="$(mktemp -d)"
trap 'rm -rf -- "$fake_root" "$dracut_root" "$dracut_pkg_root" "$cap_root" "$cap_arch_min" "$cap_alpine"' EXIT
mkdir -p "$cap_alpine"/sbin "$cap_alpine"/usr/bin "$cap_alpine"/etc/init.d "$cap_alpine"/etc/conf.d \
    "$cap_alpine"/etc/mkinitfs "$cap_alpine"/etc/runlevels/default "$cap_alpine"/boot \
    "$cap_alpine"/lib/apk/db "$cap_alpine"/lib/modules/6.18.52-0-lts "$cap_alpine"/var/log \
    "$cap_alpine"/etc/apk
: > "$cap_alpine/sbin/apk"
: > "$cap_alpine/sbin/openrc"
: > "$cap_alpine/sbin/mkinitfs"
: > "$cap_alpine/sbin/update-extlinux"
: > "$cap_alpine/usr/bin/lightdm"
printf 'overwrite=1\ndefault=lts\n' > "$cap_alpine/etc/update-extlinux.conf"
printf 'http://dl-cdn.alpinelinux.org/alpine/v3.24/main\n' > "$cap_alpine/etc/apk/repositories"
printf 'alpine-base\nlightdm\n' > "$cap_alpine/etc/apk/world"
# Real Alpine kernel packages write the flavor with a leading dash; the
# fixture must match the real layout or it masks the pairing normalization.
printf '%s\n' '-lts' > "$cap_alpine/lib/modules/6.18.52-0-lts/kernel-suffix"
printf '#!/sbin/openrc-run\nprovide display-manager\ncommand=/usr/bin/lightdm\n' > "$cap_alpine/etc/init.d/lightdm"
ln -sfn /etc/init.d/lightdm "$cap_alpine/etc/runlevels/default/lightdm"
printf 'P:lightdm\nV:1.32.0-r3\n\nP:mkinitfs\nV:3.10.4-r0\n\nP:syslinux\nV:6.04_pre1-r19\n\n' \
    > "$cap_alpine/lib/apk/db/installed"
: > "$cap_alpine/boot/vmlinuz-lts"
: > "$cap_alpine/boot/vmlinuz-virt"
: > "$cap_alpine/boot/initramfs-lts"
: > "$cap_alpine/boot/initramfs-virt"
cat > "$cap_alpine/var/log/messages" <<'SYSLOG'
Sep 19 08:14:51 alpine daemon.info init: starting pid 2526, tty '': '/sbin/openrc default'
Sep 19 08:33:14 alpine daemon.warn supervise-daemon[3289]: /usr/bin/lightdm, pid 3290, exited with return code 1
Sep 19 08:33:26 alpine daemon.warn supervise-daemon[3289]: respawned "/usr/bin/lightdm" too many times, exiting
Sep 19 08:35:00 alpine daemon.info crond[2560]: USER root pid 2614 cmd run-parts /etc/periodic/15min
Sep 19 08:36:00 alpine user.info : info: guest-exec called: "/bin/sh -c lightdm failure from user session"
Sep 19 08:36:01 alpine user.info some-app[999]: unrelated application failure
Sep 19 08:36:02 alpine daemon.info cryptsetup[123]: Failed to unlock device root
SYSLOG
cat > "$cap_alpine/var/log/dmesg" <<'DMESG'
[    0.000000] Linux version 6.18.52-0-lts (buildozer@build) #1-Alpine SMP
[    0.035802] Kernel command line: BOOT_IMAGE=vmlinuz-lts root=UUID=1111-2222 quiet
[    0.319835] Mounting root...
DMESG
cat > "$cap_alpine/var/log/apk.log" <<'APKLOG'
(1/2) Installing lightdm
ERROR: unable to select packages:
  lightdm-gtk-greeter (missing):
APKLOG
chmod +x "$cap_alpine/sbin/apk" "$cap_alpine/sbin/openrc" "$cap_alpine/sbin/mkinitfs" \
    "$cap_alpine/sbin/update-extlinux" "$cap_alpine/usr/bin/lightdm" \
    "$cap_alpine/etc/init.d/lightdm"

TARGET_ROOT="$cap_alpine"
TARGET_OS_ID="alpine"
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=""
profile_target_backends
[[ "$TARGET_DISTRO_FAMILY" == alpine ]] || { echo 'FAIL: Alpine ID was not detected as the alpine family' >&2; exit 1; }
[[ "$TARGET_PACKAGE_MANAGER" == apk ]] || { echo 'FAIL: Alpine apk was not detected' >&2; exit 1; }
[[ "$TARGET_INITRAMFS_BACKEND" == mkinitfs ]] || { echo 'FAIL: Alpine mkinitfs was not detected' >&2; exit 1; }
[[ "$TARGET_SERVICE_MANAGER" == OpenRC ]] || { echo 'FAIL: Alpine OpenRC was not detected' >&2; exit 1; }
[[ "$TARGET_KERNEL_LAYOUT" == 'Alpine named kernels (vmlinuz-lts/virt/edge)' ]] \
    || { echo "FAIL: unexpected Alpine kernel layout: $TARGET_KERNEL_LAYOUT" >&2; exit 1; }
[[ "$TARGET_REPAIR_BACKEND" == 'Alpine profile — guarded apk/OpenRC/mkinitfs/extlinux repairs when their stage-specific preflights pass' ]] \
    || { echo "FAIL: unexpected Alpine repair capability: $TARGET_REPAIR_BACKEND" >&2; exit 1; }
[[ "$(alpine_display_manager_present)" == lightdm ]] \
    || { echo 'FAIL: Alpine LightDM init script was not detected' >&2; exit 1; }

# Versioned Alpine kernel names are recognized alongside the flavor names.
: > "$cap_alpine/boot/vmlinuz-6.6.4-0-lts"
[[ "$(alpine_kernel_images | paste -sd' ' -)" == 'vmlinuz-6.6.4-0-lts vmlinuz-lts vmlinuz-virt' ]] \
    || { echo 'FAIL: versioned Alpine kernel name was not detected' >&2; exit 1; }
rm -f "$cap_alpine/boot/vmlinuz-6.6.4-0-lts"

# syslinux/extlinux is the Alpine BIOS bootloader and must not fall through to
# the unknown-EFI-loader label; /etc/update-extlinux.conf alone is evidence,
# and a real GRUB configuration still wins.
[[ "$TARGET_BOOTLOADER_BACKEND" == 'syslinux/extlinux' ]] \
    || { echo "FAIL: Alpine /etc/update-extlinux.conf was not detected as syslinux/extlinux: $TARGET_BOOTLOADER_BACKEND" >&2; exit 1; }
cat > "$cap_alpine/boot/extlinux.conf" <<'EXTLINUX'
DEFAULT menu.c32
PROMPT 0
MENU TITLE Alpine/Linux Boot Menu
TIMEOUT 10
LABEL lts
  MENU DEFAULT
  MENU LABEL Linux lts
  LINUX vmlinuz-lts
  INITRD initramfs-lts
  APPEND root=UUID=1111-2222 modules=sd-mod,usb-storage,ext4 quiet cryptroot=/dev/sda3

LABEL virt
  LINUX vmlinuz-virt
  INITRD initramfs-virt
  APPEND root=UUID=3333-4444 modules=sd-mod,usb-storage,ext4
EXTLINUX
profile_target_backends
[[ "$TARGET_BOOTLOADER_BACKEND" == 'syslinux/extlinux' ]] \
    || { echo "FAIL: Alpine syslinux/extlinux was not detected: $TARGET_BOOTLOADER_BACKEND" >&2; exit 1; }
mkdir -p "$cap_alpine/boot/grub"
: > "$cap_alpine/boot/grub/grub.cfg"
profile_target_backends
[[ "$TARGET_BOOTLOADER_BACKEND" == grub ]] \
    || { echo 'FAIL: Alpine GRUB did not take precedence over syslinux/extlinux' >&2; exit 1; }
rm -rf "$cap_alpine/boot/grub"

# ID_LIKE=alpine (for example postmarketOS) maps to the same family.
TARGET_OS_ID="postmarketos"
TARGET_OS_LIKE="alpine"
TARGET_DISTRO_FAMILY=""
profile_target_backends
[[ "$TARGET_DISTRO_FAMILY" == alpine ]] \
    || { echo 'FAIL: ID_LIKE=alpine was not detected as the alpine family' >&2; exit 1; }
TARGET_OS_ID="alpine"
TARGET_OS_LIKE=""

# OpenRC display managers are discovered through /etc/init.d and /etc/conf.d.
rm -f "$cap_alpine/etc/init.d/lightdm"
: > "$cap_alpine/etc/conf.d/slim"
[[ "$(alpine_display_manager_present)" == slim ]] \
    || { echo 'FAIL: Alpine SLiM /etc/conf.d entry was not detected' >&2; exit 1; }
rm -f "$cap_alpine/etc/conf.d/slim"
: > "$cap_alpine/etc/init.d/gdm"; chmod +x "$cap_alpine/etc/init.d/gdm"
[[ "$(alpine_display_manager_present)" == gdm ]] \
    || { echo 'FAIL: Alpine GDM init script was not detected' >&2; exit 1; }
printf '#!/sbin/openrc-run\nprovide display-manager\ncommand=/usr/bin/lightdm\n' > "$cap_alpine/etc/init.d/lightdm"
chmod +x "$cap_alpine/etc/init.d/lightdm"
rm -f "$cap_alpine/etc/init.d/gdm"

# The read-only backend profile names the detected backends and the logging
# source instead of assuming a family-specific journal.
cap_alpine_profile="$(TARGET_ROOT="$cap_alpine" TARGET_OS_ID=alpine TARGET_OS_LIKE="" \
    TARGET_DISTRO_FAMILY="" TARGET_PRETTY="Alpine Linux v3.20" diagnostic_backend_profile)"
for profile_line in \
    'Distribution ID: alpine' \
    'Distribution family: alpine' \
    'Distribution name: Alpine Linux v3.20' \
    'Package manager backend: apk' \
    'Package manager backends: apk' \
    'Initramfs backend: mkinitfs' \
    'Initramfs backends: mkinitfs' \
    'Service manager: OpenRC' \
    'Display manager backend: OpenRC' \
    'OpenRC display manager: lightdm' \
    'Kernel layout: Alpine named kernels (vmlinuz-lts/virt/edge)' \
    'Logging backend: messages' \
    'Logging source: /var/log/messages or /var/log/syslog' \
    'Alpine status: guarded repairs are selected per detected backend; EFI/GRUB availability follows the detected bootloader backend.'; do
    grep -Fqx "$profile_line" <<<"$cap_alpine_profile" \
        || { echo "FAIL: Alpine backend profile line missing: $profile_line" >&2; exit 1; }
done

# The Alpine fixture is a directory, not a mountable device, so the
# backend-independent filesystem scope is stubbed to prove Alpine keeps the
# read-only filesystem capability while every guarded repair fails closed.
cap_alpine_probe()
{
    (
        filesystem_scope_resolve()
        {
            FS_SCOPE_DEVICES=(/dev/contract-alpine-root)
            FS_SCOPE_MOUNTS=("")
            FS_SCOPE_FSTYPES=(ext4)
            FS_SCOPE_UUIDS=(none)
            FS_SCOPE_TOOLS=(/usr/sbin/e2fsck)
            FS_SCOPE_DEVICE_INDEX=( [/dev/contract-alpine-root]=0 )
        }
        filesystem_scope_tools() { FS_SCOPE_TOOLS=(/usr/sbin/e2fsck); }
        TARGET_ROOT="$cap_alpine" TARGET_OS_ID=alpine TARGET_OS_LIKE="" \
            TARGET_DISTRO_FAMILY=alpine TARGET_INITRAMFS_BACKEND=mkinitfs \
            TARGET_BOOTLOADER_BACKEND=grub diagnostic_repair_capabilities
    )
}

cap_alpine_output="$(cap_alpine_probe)"
cap_expect "$cap_alpine_output" \
    'validate: available' \
    'filesystem: available' \
    'dpkg: unavailable|Alpine uses apk; dpkg configuration is not available on Alpine' \
    'fixbroken: available' \
    'aptupdate: unavailable|Standalone APK metadata refresh is not available on Alpine; use Upgrade installed packages for one guarded apk transaction' \
    'upgrade: available' \
    'dkms: unavailable|DKMS is not installed in the Alpine target system' \
    'display: available' \
    'initramfs: available' \
    'extlinux: available' \
    'efi: unavailable|syslinux/extlinux (BIOS) boot detected; no EFI boot path is available' \
    'grub: unavailable|The detected bootloader is syslinux/extlinux; GRUB is not the selected bootloader' \
    'bootstack: unavailable|Alpine uses OpenRC, mkinitfs and syslinux/extlinux; boot-stack reconciliation is not enabled (run the initramfs and extlinux stages separately)'
if grep -q 'Modifying repairs require a supported Debian/Ubuntu' <<<"$cap_alpine_output"; then
    echo 'FAIL: Alpine capability reasons fell back to the unsupported-family message' >&2
    exit 1
fi
grep -Fqx 'Repair capability evidence filesystem: scope filesystems resolved; tools: e2fsck(ext4)' <<<"$cap_alpine_output" \
    || { echo 'FAIL: Alpine filesystem evidence changed' >&2; exit 1; }
grep -Fqx 'Repair capability evidence dpkg: /usr/bin/dpkg missing' <<<"$cap_alpine_output" \
    || { echo 'FAIL: Alpine dpkg evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence fixbroken: apk executable present; /etc/apk/repositories present; /lib/apk/db/installed present; /etc/apk/world present' <<<"$cap_alpine_output" \
    || { echo 'FAIL: Alpine apk evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence display: OpenRC display-manager service lightdm present' <<<"$cap_alpine_output" \
    || { echo 'FAIL: Alpine display-manager evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence initramfs: Alpine mkinitfs backend; kernels: vmlinuz-lts vmlinuz-virt; initramfs images: initramfs-lts initramfs-virt' <<<"$cap_alpine_output" \
    || { echo 'FAIL: Alpine initramfs evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence extlinux: update-extlinux present; /boot/extlinux.conf present' <<<"$cap_alpine_output" \
    || { echo 'FAIL: Alpine extlinux evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence bootstack: Alpine uses OpenRC, mkinitfs and syslinux/extlinux; boot-stack reconciliation is not enabled (run the initramfs and extlinux stages separately)' <<<"$cap_alpine_output" \
    || { echo 'FAIL: Alpine boot-stack evidence is missing' >&2; exit 1; }

# Missing apk prerequisites must fail closed with Alpine-specific reasons.
mv "$cap_alpine/etc/apk/repositories" "$cap_alpine/etc/apk/repositories.disabled"
cap_expect "$(cap_alpine_probe)" \
    'fixbroken: unavailable|The target has no configured apk repositories' \
    'upgrade: unavailable|The target has no configured apk repositories'
mv "$cap_alpine/etc/apk/repositories.disabled" "$cap_alpine/etc/apk/repositories"
mv "$cap_alpine/etc/apk/world" "$cap_alpine/etc/apk/world.disabled"
cap_expect "$(cap_alpine_probe)" \
    'fixbroken: unavailable|The target apk world file is missing' \
    'upgrade: unavailable|The target apk world file is missing'
mv "$cap_alpine/etc/apk/world.disabled" "$cap_alpine/etc/apk/world"
mv "$cap_alpine/lib/apk/db/installed" "$cap_alpine/lib/apk/db/installed.disabled"
cap_expect "$(cap_alpine_probe)" \
    'fixbroken: unavailable|The target apk installed database is missing' \
    'upgrade: unavailable|The target apk installed database is missing'
mv "$cap_alpine/lib/apk/db/installed.disabled" "$cap_alpine/lib/apk/db/installed"

# Missing mkinitfs prerequisites and update-extlinux fail closed too.
mv "$cap_alpine/sbin/mkinitfs" "$cap_alpine/sbin/mkinitfs.disabled"
cap_expect "$(cap_alpine_probe)" 'initramfs: unavailable|mkinitfs is not installed in the target system'
mv "$cap_alpine/sbin/mkinitfs.disabled" "$cap_alpine/sbin/mkinitfs"
mv "$cap_alpine/sbin/update-extlinux" "$cap_alpine/sbin/update-extlinux.disabled"
cap_expect "$(cap_alpine_probe)" 'extlinux: unavailable|update-extlinux is not installed in the target'
mv "$cap_alpine/sbin/update-extlinux.disabled" "$cap_alpine/sbin/update-extlinux"
mv "$cap_alpine/boot/extlinux.conf" "$cap_alpine/boot/extlinux.conf.disabled"
mv "$cap_alpine/etc/update-extlinux.conf" "$cap_alpine/etc/update-extlinux.conf.disabled"
cap_expect "$(cap_alpine_probe)" 'extlinux: unavailable|No extlinux/syslinux configuration was detected in the target'
mv "$cap_alpine/boot/extlinux.conf.disabled" "$cap_alpine/boot/extlinux.conf"
mv "$cap_alpine/etc/update-extlinux.conf.disabled" "$cap_alpine/etc/update-extlinux.conf"
mv "$cap_alpine/lib/modules/6.18.52-0-lts/kernel-suffix" "$cap_alpine/lib/modules/6.18.52-0-lts/kernel-suffix.disabled"
cap_expect "$(cap_alpine_probe)" 'initramfs: unavailable|No installed Alpine kernels were found under target /boot'
mv "$cap_alpine/lib/modules/6.18.52-0-lts/kernel-suffix.disabled" "$cap_alpine/lib/modules/6.18.52-0-lts/kernel-suffix"

# DKMS on Alpine is available when dkms is installed and the headers/build
# tree exists for every installed kernel, exactly like the Arch preflight.
: > "$cap_alpine/usr/bin/dkms"; chmod +x "$cap_alpine/usr/bin/dkms"
cap_expect "$(cap_alpine_probe)" \
    'dkms: unavailable|Alpine DKMS preflight found no build tree for installed kernel 6.18.52-0-lts; install the matching headers and retry'
mkdir -p "$cap_alpine/lib/modules/6.18.52-0-lts/build"
cap_expect "$(cap_alpine_probe)" 'dkms: available'
rm -rf "$cap_alpine/lib/modules/6.18.52-0-lts/build" "$cap_alpine/usr/bin/dkms"

# ---------------------------------------------------------------------------
# Alpine/OpenRC runtime evidence: the extlinux boot chain, the OpenRC display
# manager branch and the syslog fallback must describe the real system instead
# of claiming systemd targets, display-manager.service, journald or Debian
# initramfs tooling.
# ---------------------------------------------------------------------------

# Alpine capability evidence must name the OpenRC/mkinitfs/extlinux reality.
cap_alpine_extlinux="$(TARGET_ROOT="$cap_alpine" TARGET_OS_ID=alpine TARGET_OS_LIKE="" \
    TARGET_DISTRO_FAMILY=alpine TARGET_INITRAMFS_BACKEND=mkinitfs \
    TARGET_BOOTLOADER_BACKEND=syslinux/extlinux diagnostic_repair_capabilities)"
grep -Fqx 'Repair capability evidence efi: Alpine syslinux/extlinux (BIOS) backend; no EFI boot path' <<<"$cap_alpine_extlinux" \
    || { echo 'FAIL: Alpine extlinux EFI evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence grub: syslinux/extlinux is the selected bootloader; GRUB is not detected' <<<"$cap_alpine_extlinux" \
    || { echo 'FAIL: Alpine extlinux GRUB evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence bootstack: Alpine uses OpenRC, mkinitfs and syslinux/extlinux; boot-stack reconciliation is not enabled (run the initramfs and extlinux stages separately)' <<<"$cap_alpine_extlinux" \
    || { echo 'FAIL: Alpine extlinux boot-stack evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence extlinux: update-extlinux present; /boot/extlinux.conf present' <<<"$cap_alpine_extlinux" \
    || { echo 'FAIL: Alpine extlinux evidence is missing' >&2; exit 1; }
if grep -Fq 'requires GRUB configuration tooling' <<<"$cap_alpine_extlinux"; then
    echo 'FAIL: Alpine EFI evidence fell back to the Debian GRUB wording' >&2
    exit 1
fi
for forbidden in 'systemd' 'display-manager.service' 'dpkg-query'; do
    if grep -Fq "$forbidden" <<<"$cap_alpine_extlinux"; then
        echo "FAIL: Alpine capability evidence still claims Debian/systemd state: $forbidden" >&2
        exit 1
    fi
done

# extlinux parsing: LABEL/LINUX/INITRD/APPEND in menu order, default marker
# and command-line secret redaction.
extlinux_entries="$(TARGET_ROOT="$cap_alpine" extlinux_menu_entries)"
grep -Fqx 'LABEL lts [default]' <<<"$extlinux_entries" \
    || { echo 'FAIL: default extlinux label was not marked' >&2; exit 1; }
grep -Fqx '  LINUX vmlinuz-lts' <<<"$extlinux_entries" \
    || { echo 'FAIL: extlinux LINUX entry was not parsed' >&2; exit 1; }
grep -Fqx '  INITRD initramfs-lts' <<<"$extlinux_entries" \
    || { echo 'FAIL: extlinux INITRD entry was not parsed' >&2; exit 1; }
grep -Fqx 'LABEL virt' <<<"$extlinux_entries" \
    || { echo 'FAIL: secondary extlinux label was not parsed' >&2; exit 1; }
grep -Fq 'cryptroot=[REDACTED]' <<<"$extlinux_entries" \
    || { echo 'FAIL: extlinux APPEND secrets were not redacted' >&2; exit 1; }
if grep -Fq 'cryptroot=/dev/sda3' <<<"$extlinux_entries"; then
    echo 'FAIL: extlinux APPEND leaked the crypt root device' >&2
    exit 1
fi
[[ "$(TARGET_ROOT="$cap_alpine" extlinux_default_entry)" == $'lts\tvmlinuz-lts\tinitramfs-lts' ]] \
    || { echo 'FAIL: default extlinux entry was not resolved' >&2; exit 1; }

# ---------------------------------------------------------------------------
# Alpine apk simulation/apply policy: the safety parser must fail closed on
# removals, downgrades, untrusted signatures, locked databases, unresolved
# packages, incomplete metadata, unknown errors and an oversized transaction,
# while accepting a same-package replacement.
# ---------------------------------------------------------------------------
apk_policy_log="$fake_root/apk-policy.log"
SESSION_LOG="$apk_policy_log"
: > "$apk_policy_log"

alpine_apk_simulation_is_safe $'OK: 1734.0 MiB in 658 packages' 0 \
    || { echo 'FAIL: apk no-op simulation rejected' >&2; exit 1; }
alpine_apk_simulation_is_safe $'(1/1) Installing nano (9.2-r0)\nOK: 1734.3 MiB in 659 packages' 0 \
    || { echo 'FAIL: apk install simulation rejected' >&2; exit 1; }
alpine_apk_simulation_is_safe $'(1/1) Upgrading foo (1.0-r0 -> 2.0-r0)' 0 \
    || { echo 'FAIL: apk upgrade simulation rejected' >&2; exit 1; }
alpine_apk_simulation_is_safe $'(1/2) Installing foo (2.0-r0)\n(2/2) Purging foo (1.0-r0)' 0 \
    || { echo 'FAIL: same-package apk replacement rejected' >&2; exit 1; }
for apk_unsafe in \
    $'(1/1) Purging foo (1.0-r0)' \
    $'(1/1) Downgrading foo (2.0-r0 -> 1.0-r0)' \
    $'ERROR: foo: UNTRUSTED signature' \
    $'ERROR: Unable to lock database' \
    $'ERROR: unable to select packages:\n  foo (missing):' \
    $'WARNING: temporary error' \
    $'unrecognized error: bad' \
    $'ERROR: bad package'; do
    if alpine_apk_simulation_is_safe "$apk_unsafe" 0; then
        echo "FAIL: unsafe apk simulation accepted: $apk_unsafe" >&2
        exit 1
    fi
done
alpine_apk_simulation_is_safe $'OK: 1 package' 1 \
    && { echo 'FAIL: nonzero apk simulation exit accepted' >&2; exit 1; }
apk_cap_fixture="$(for i in $(seq 1 1001); do printf '(1/1) Installing pkg%s (1.0-r0)\n' "$i"; done)"
alpine_apk_simulation_is_safe "$apk_cap_fixture" 0 \
    && { echo 'FAIL: apk package-count cap overflow accepted' >&2; exit 1; }
grep -Fq 'safety limit: 1000' "$apk_policy_log" \
    || { echo 'FAIL: apk cap refusal was not logged' >&2; exit 1; }

# apk has no "nothing to do" summary: no action line is the no-change proof.
alpine_apk_transaction_reported_no_changes $'OK: 1734.0 MiB in 658 packages' \
    || { echo 'FAIL: apk no-op simulation was not recognized' >&2; exit 1; }
alpine_apk_transaction_reported_no_changes $'(1/1) Installing nano (9.2-r0)' \
    && { echo 'FAIL: apk package action was treated as unchanged' >&2; exit 1; }
alpine_apk_transaction_reported_no_changes $'(1/1) Purging foo (1.0-r0)' \
    && { echo 'FAIL: apk removal was treated as unchanged' >&2; exit 1; }

# The apk backend must simulate exactly once, apply the exact simulated
# command and never pass force/overwrite/untrusted flags.
grep -Fq 'adaptive_alpine_apk_apply "Repair Alpine package dependencies (apk fix --depends)" fixbroken fix --depends' "$HELPER" \
    || { echo 'FAIL: apk fix-broken stage is not wired' >&2; exit 1; }
grep -Fq 'adaptive_alpine_apk_apply "Repair Alpine package dependencies and missing files (apk fix --depends ${missing_packages[*]})" fixbroken fix --depends "${missing_packages[@]}"' "$HELPER" \
    || { echo 'FAIL: apk targeted missing-file repair is not wired' >&2; exit 1; }
grep -Fq 'adaptive_alpine_apk_stage "Upgrade installed Alpine packages (apk upgrade)" upgrade upgrade' "$HELPER" \
    || { echo 'FAIL: apk upgrade stage is not wired' >&2; exit 1; }
grep -Fq 'alpine_apk_transaction_try "Alpine apk ${apk_command[*]} simulation" "${apk_command[@]}" --simulate' "$HELPER" \
    || { echo 'FAIL: apk simulation is not wired' >&2; exit 1; }
grep -Fq 'run_chroot_try "$label" apk "${apk_command[@]}"' "$HELPER" \
    || { echo 'FAIL: apk apply is not wired' >&2; exit 1; }
grep -Fq 'flock -n "$lock" true' "$HELPER" \
    || { echo 'FAIL: apk database lock probe is not wired' >&2; exit 1; }
grep -Fq 'apk audit --system' "$HELPER" \
    || { echo 'FAIL: apk missing-file audit is not wired' >&2; exit 1; }
grep -Fq 'apk info --who-owns' "$HELPER" \
    || { echo 'FAIL: apk missing-file owner mapping is not wired' >&2; exit 1; }
grep -Fq 'APK_MAX_MISSING_FILES=1000' "$HELPER" \
    || { echo 'FAIL: apk missing-file cap is not wired' >&2; exit 1; }
grep -Fq 'APK_MAX_MISSING_PACKAGES=1000' "$HELPER" \
    || { echo 'FAIL: apk missing-package cap is not wired' >&2; exit 1; }
if grep -Fq -- '--force' "$HELPER" || grep -Fq -- '--overwrite' "$HELPER" || grep -Fq -- '--allow-untrusted' "$HELPER"; then
    echo 'FAIL: apk backend must never pass force/overwrite/untrusted flags' >&2
    exit 1
fi

# Change status: a no-op simulation plus identical fingerprints is unchanged;
# a simulated package change is always changed.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$fake_root/apk-status.log"
    SESSION_DIR="$fake_root/apk-status-session"
    mkdir -p "$SESSION_DIR"
    : > "$SESSION_LOG"
    TARGET_DISTRO_FAMILY=alpine
    # Sourcing the helper resets TARGET_ROOT; keep the world-pin feedback probe
    # on the isolated fixture instead of the build host's /etc/apk/world.
    TARGET_ROOT="$fake_root"
    alpine_apk_preflight() { :; }
    alpine_apk_fingerprint() { printf 'stable\n'; }
    alpine_apk_transaction_try() { APK_SIM_RC=0; APK_SIM_OUTPUT=$'OK: 1734.0 MiB in 658 packages'; }
    run_chroot_try() { CHROOT_TRY_RC=0; CHROOT_TRY_OUTPUT=''; }
    apk_status_out="$(adaptive_alpine_apk_fix_broken)"
    grep -Fqx 'Repair change status fixbroken: unchanged|apk simulated no package changes and the package state is byte-identical' <<<"$apk_status_out" \
        || { echo 'FAIL: apk no-op change status is wrong' >&2; printf '%s\n' "$apk_status_out" >&2; exit 1; }
    alpine_apk_transaction_try() { APK_SIM_RC=0; APK_SIM_OUTPUT=$'(1/1) Upgrading foo (1.0-r0 -> 2.0-r0)'; }
    apk_status_out="$(adaptive_alpine_apk_upgrade)"
    grep -Fqx 'Repair change status upgrade: changed' <<<"$apk_status_out" \
        || { echo 'FAIL: apk changed status is wrong' >&2; printf '%s\n' "$apk_status_out" >&2; exit 1; }
)

# Missing-file fixture: apk audit --system lists deleted package files, the
# read-only who-owns mapping turns them into packages, and fix-broken injects
# exactly one combined `apk fix --depends <pkgs>` transaction that is simulated
# before it is applied.  The empty-list path must stay today's exact
# `apk fix --depends` transaction.
apk_missing_log="$fake_root/apk-missing.log"
apk_missing_commands="$fake_root/apk-missing-commands.log"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_DISTRO_FAMILY=alpine
    TARGET_ROOT="$fake_root"
    SESSION_LOG="$apk_missing_log"
    SESSION_DIR="$fake_root/apk-missing-session"
    mkdir -p "$SESSION_DIR"
    : > "$SESSION_LOG"
    : > "$apk_missing_commands"

    run_chroot_try()
    {
        printf 'CHROOT %s\n' "$*" >> "$apk_missing_commands"
        case "$*" in
            *'apk audit --system'*)
                CHROOT_TRY_RC=0
                CHROOT_TRY_OUTPUT=$'X usr/bin/lightdm\nX boot/vmlinuz-lts' ;;
            *'apk info --who-owns'*)
                CHROOT_TRY_RC=0
                CHROOT_TRY_OUTPUT=$'/usr/bin/lightdm is owned by lightdm-1.32.0-r12\n/boot/vmlinuz-lts is owned by linux-lts-6.18.52-r0' ;;
            *)
                CHROOT_TRY_RC=0
                CHROOT_TRY_OUTPUT='' ;;
        esac
    }
    alpine_apk_preflight() { :; }
    alpine_apk_fingerprint() { printf 'fingerprint\n'; }
    alpine_apk_transaction_try()
    {
        printf 'SIMULATE %s\n' "$*" >> "$apk_missing_commands"
        APK_SIM_RC=0
        APK_SIM_OUTPUT=$'(1/2) Reinstalling lightdm (1.32.0-r12)\n(2/2) Reinstalling linux-lts (6.18.52-r0)'
    }

    detected="$(alpine_apk_missing_file_packages)"
    [[ "$detected" == $'lightdm\nlinux-lts' ]] \
        || { echo "FAIL: missing-file detection produced '$detected'" >&2; exit 1; }

    fixbroken_out="$(adaptive_alpine_apk_fix_broken)"
    grep -Fqx 'Repair change status fixbroken: changed' <<<"$fixbroken_out" \
        || { echo 'FAIL: targeted missing-file repair did not report changed' >&2; printf '%s\n' "$fixbroken_out" >&2; exit 1; }

    # Empty detection keeps the previous exact command and no-op status.
    run_chroot_try()
    {
        printf 'CHROOT %s\n' "$*" >> "$apk_missing_commands"
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=''
    }
    alpine_apk_transaction_try()
    {
        printf 'SIMULATE %s\n' "$*" >> "$apk_missing_commands"
        APK_SIM_RC=0
        APK_SIM_OUTPUT='OK: 1734.0 MiB in 658 packages'
    }
    fixbroken_noop_out="$(adaptive_alpine_apk_fix_broken)"
    grep -Fqx 'Repair change status fixbroken: unchanged|apk simulated no package changes and the package state is byte-identical' <<<"$fixbroken_noop_out" \
        || { echo 'FAIL: empty missing-file list changed the no-op status' >&2; printf '%s\n' "$fixbroken_noop_out" >&2; exit 1; }
) || exit 1
grep -Fq 'apk info --who-owns /boot/vmlinuz-lts /usr/bin/lightdm' "$apk_missing_commands" \
    || { echo 'FAIL: missing files were not mapped to owners with absolute paths' >&2; exit 1; }
grep -Fq 'SIMULATE Alpine apk fix --depends lightdm linux-lts simulation fix --depends lightdm linux-lts --simulate' "$apk_missing_commands" \
    || { echo 'FAIL: targeted repair was not simulated first' >&2; exit 1; }
grep -Fq 'CHROOT Repair Alpine package dependencies and missing files (apk fix --depends lightdm linux-lts) apk fix --depends lightdm linux-lts' "$apk_missing_commands" \
    || { echo 'FAIL: targeted repair was not applied with the exact simulated command' >&2; exit 1; }
grep -Fq 'SIMULATE Alpine apk fix --depends simulation fix --depends --simulate' "$apk_missing_commands" \
    || { echo 'FAIL: the empty missing-file list did not use apk fix --depends' >&2; exit 1; }
grep -Fq 'CHROOT Repair Alpine package dependencies (apk fix --depends) apk fix --depends' "$apk_missing_commands" \
    || { echo 'FAIL: the empty missing-file list did not apply apk fix --depends' >&2; exit 1; }
sim_line="$(grep -n 'SIMULATE Alpine apk fix --depends lightdm linux-lts simulation' "$apk_missing_commands" | head -n1 | cut -d: -f1)"
apply_line="$(grep -n 'CHROOT Repair Alpine package dependencies and missing files .* apk fix --depends lightdm linux-lts$' "$apk_missing_commands" | head -n1 | cut -d: -f1)"
[[ -n "$sim_line" && -n "$apply_line" && "$sim_line" -lt "$apply_line" ]] \
    || { echo 'FAIL: targeted missing-file repair applied before its simulation' >&2; exit 1; }

# Missing-file detection fails closed: an oversized list is refused before the
# owner mapping, an unowned file is refused, and an audit error is refused.
apk_missing_cap_err="$fake_root/apk-missing-cap.err"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_DISTRO_FAMILY=alpine
    TARGET_ROOT="$fake_root"
    SESSION_LOG="$fake_root/apk-missing-cap.log"
    SESSION_DIR="$fake_root/apk-missing-session"
    mkdir -p "$SESSION_DIR"
    : > "$SESSION_LOG"

    run_chroot_try()
    {
        CHROOT_TRY_RC=0
        case "$*" in
            *'apk audit --system'*)
                CHROOT_TRY_OUTPUT="$(for i in $(seq 1 1001); do printf 'X usr/bin/file%s\n' "$i"; done)" ;;
            *'apk info --who-owns'*)
                CHROOT_TRY_OUTPUT='' ;;
            *)
                CHROOT_TRY_OUTPUT='' ;;
        esac
    }
    if ( alpine_apk_missing_file_packages ) >/dev/null 2>"$apk_missing_cap_err"; then
        echo 'FAIL: an oversized missing-file list was accepted' >&2
        exit 1
    fi
    grep -Fq 'missing files (safety limit: 1000)' "$apk_missing_cap_err" \
        || { echo 'FAIL: the missing-file cap refusal was not reported' >&2; exit 1; }

    run_chroot_try()
    {
        CHROOT_TRY_RC=0
        case "$*" in
            *'apk audit --system'*)
                CHROOT_TRY_OUTPUT='X usr/bin/lightdm' ;;
            *'apk info --who-owns'*)
                CHROOT_TRY_RC=1
                CHROOT_TRY_OUTPUT='ERROR: /usr/bin/lightdm: Could not find owner package' ;;
            *)
                CHROOT_TRY_OUTPUT='' ;;
        esac
    }
    if ( alpine_apk_missing_file_packages ) >/dev/null 2>"$apk_missing_cap_err"; then
        echo 'FAIL: a missing file without an owner was accepted' >&2
        exit 1
    fi
    grep -Fq 'could not map every missing file' "$apk_missing_cap_err" \
        || { echo 'FAIL: the unowned missing-file refusal was not reported' >&2; exit 1; }

    run_chroot_try()
    {
        CHROOT_TRY_RC=1
        CHROOT_TRY_OUTPUT='ERROR: audit failed'
    }
    if ( alpine_apk_missing_file_packages ) >/dev/null 2>"$apk_missing_cap_err"; then
        echo 'FAIL: a failed apk audit was accepted' >&2
        exit 1
    fi
    grep -Fq 'apk audit --system failed with exit code 1' "$apk_missing_cap_err" \
        || { echo 'FAIL: the audit failure was not reported' >&2; exit 1; }
) || exit 1

# The package-state fingerprint includes the missing-file situation: changing
# only the audit result changes the fingerprint, while a stable audit result
# keeps it identical.
apk_fingerprint_root="$fake_root/apk-fingerprint"
mkdir -p "$apk_fingerprint_root/etc/apk" "$apk_fingerprint_root/lib/apk/db"
printf 'alpine-base\n' > "$apk_fingerprint_root/etc/apk/world"
printf 'P:lightdm\nV:1.32.0-r12\n\n' > "$apk_fingerprint_root/lib/apk/db/installed"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_ROOT="$apk_fingerprint_root"
    SESSION_LOG="$fake_root/apk-fingerprint.log"
    SESSION_DIR="$fake_root/apk-missing-session"
    mkdir -p "$SESSION_DIR"
    : > "$SESSION_LOG"
    audit_output='X usr/bin/lightdm'
    run_chroot_try() { CHROOT_TRY_RC=0; CHROOT_TRY_OUTPUT="$audit_output"; }
    fingerprint_with_missing="$(alpine_apk_fingerprint)"
    audit_output=''
    fingerprint_clean="$(alpine_apk_fingerprint)"
    [[ "$fingerprint_with_missing" != "$fingerprint_clean" ]] \
        || { echo 'FAIL: the package fingerprint ignored the missing-file situation' >&2; exit 1; }
    [[ "$fingerprint_clean" == "$(alpine_apk_fingerprint)" ]] \
        || { echo 'FAIL: the package fingerprint is not stable' >&2; exit 1; }
) || exit 1

# ---------------------------------------------------------------------------
# Alpine kernel pairing, mkinitfs trial/apply/verify and extlinux guard
# wiring.  BusyBox-safe probes only; no GNU find -printf on the Alpine path.
# ---------------------------------------------------------------------------
pair_root="$(mktemp -d)"
trap 'rm -rf -- "$fake_root" "$dracut_root" "$dracut_pkg_root" "$cap_root" "$cap_arch_min" "$cap_alpine" "$pair_root"' EXIT
mkdir -p "$pair_root/lib/modules/6.18.52-0-lts" "$pair_root/lib/modules/7.0.0-custom" "$pair_root/lib/modules/8.0.0-orphan" \
    "$pair_root/lib/modules/10.0.0-empty" "$pair_root/usr/lib/modules/9.0.0-fallback" "$pair_root/boot"
# Real Alpine linux-lts/linux-virt packages write "-lts\n"/"-virt\n" into
# kernel-suffix; the leading dash must be stripped to reach vmlinuz-lts.
printf '%s\n' '-lts' > "$pair_root/lib/modules/6.18.52-0-lts/kernel-suffix"
printf 'custom\n' > "$pair_root/lib/modules/7.0.0-custom/kernel-suffix"
# An empty kernel-suffix still falls back to the module directory name.
: > "$pair_root/lib/modules/10.0.0-empty/kernel-suffix"
: > "$pair_root/boot/vmlinuz-lts"
: > "$pair_root/boot/vmlinuz-custom"
: > "$pair_root/boot/vmlinuz-9.0.0-fallback"
: > "$pair_root/boot/vmlinuz-10.0.0-empty"
TARGET_ROOT="$pair_root"
alpine_pair_output="$(alpine_kernel_pairs)"
grep -Fqx '6.18.52-0-lts lts /boot/vmlinuz-lts /boot/initramfs-lts' <<<"$alpine_pair_output" \
    || { echo 'FAIL: leading-dash kernel-suffix pairing is wrong' >&2; exit 1; }
if grep -Fq 'vmlinuz--lts' <<<"$alpine_pair_output"; then
    echo 'FAIL: leading-dash kernel-suffix was not normalized before pairing' >&2
    exit 1
fi
grep -Fqx '7.0.0-custom custom /boot/vmlinuz-custom /boot/initramfs-custom' <<<"$alpine_pair_output" \
    || { echo 'FAIL: custom kernel-suffix pairing is wrong' >&2; exit 1; }
grep -Fqx '10.0.0-empty 10.0.0-empty /boot/vmlinuz-10.0.0-empty /boot/initramfs-10.0.0-empty' <<<"$alpine_pair_output" \
    || { echo 'FAIL: empty kernel-suffix fallback pairing is wrong' >&2; exit 1; }
grep -Fqx '9.0.0-fallback 9.0.0-fallback /boot/vmlinuz-9.0.0-fallback /boot/initramfs-9.0.0-fallback' <<<"$alpine_pair_output" \
    || { echo 'FAIL: kernel fallback pairing is wrong' >&2; exit 1; }
if grep -Fq '8.0.0-orphan' <<<"$alpine_pair_output"; then
    echo 'FAIL: kernel module directory without boot image was paired' >&2
    exit 1
fi
grep -Fq 'alpine_kernel_pairs' "$HELPER"
grep -Fq 'sim_path="/tmp/boot-repair-initramfs-preflight-${kver}"' "$HELPER" \
    || { echo 'FAIL: mkinitfs trial output path is not wired' >&2; exit 1; }
grep -Fq 'mkinitfs -o "$sim_path" "$kver"' "$HELPER" \
    || { echo 'FAIL: mkinitfs trial build is not wired' >&2; exit 1; }
grep -Fq 'mkinitfs -l "$kver"' "$HELPER" \
    || { echo 'FAIL: mkinitfs -l verification fallback is not wired' >&2; exit 1; }
grep -Fq "zcat '\$image' | cpio -t" "$HELPER" \
    || { echo 'FAIL: zcat/cpio initramfs verification is not wired' >&2; exit 1; }
grep -Fq 'run_chroot_try "Rebuild Alpine initramfs for $kver with mkinitfs" mkinitfs "$kver"' "$HELPER" \
    || { echo 'FAIL: mkinitfs per-kernel apply is not wired' >&2; exit 1; }
if grep -Fq 'mkinitcpio -P' <<<"$(sed -n '/^adaptive_alpine_initramfs_repair()/,/^}/p' "$HELPER")"; then
    echo 'FAIL: Alpine initramfs repair must not use mkinitcpio -P' >&2
    exit 1
fi

# extlinux entry keys and the entry-preservation guard.
SESSION_DIR="$pair_root"
extlinux_existing="$pair_root/extlinux.conf"
extlinux_candidate="$pair_root/extlinux.new"
cat > "$extlinux_existing" <<'EXTLINUX'
DEFAULT menu.c32
LABEL lts
  LINUX vmlinuz-lts
  INITRD initramfs-lts
LABEL virt
  LINUX vmlinuz-virt
  INITRD initramfs-virt
EXTLINUX
cp "$extlinux_existing" "$extlinux_candidate"
guard_extlinux_candidate_preserves_entries "$extlinux_existing" "$extlinux_candidate" \
    || { echo 'FAIL: preserved extlinux candidate was rejected' >&2; exit 1; }
extlinux_keys="$(extlinux_entry_keys "$extlinux_existing")"
grep -Fqx $'LABEL\tlts' <<<"$extlinux_keys" || { echo 'FAIL: extlinux LABEL key missing' >&2; exit 1; }
grep -Fqx $'LINUX\tlts\tvmlinuz-lts' <<<"$extlinux_keys" || { echo 'FAIL: extlinux LINUX key missing' >&2; exit 1; }
grep -Fqx $'INITRD\tvirt\tinitramfs-virt' <<<"$extlinux_keys" || { echo 'FAIL: extlinux INITRD key missing' >&2; exit 1; }
sed -i '/LABEL virt/,/INITRD initramfs-virt/d' "$extlinux_candidate"
if guard_extlinux_candidate_preserves_entries "$extlinux_existing" "$extlinux_candidate" >/dev/null 2>&1; then
    echo 'FAIL: extlinux entry loss was accepted' >&2
    exit 1
fi

# extlinux repair must use the overwrite=0 trial pattern, install only the
# guarded candidate and never touch the boot sector or the extlinux installer.
extlinux_body="$(sed -n '/^adaptive_extlinux_repair()/,/^}/p' "$HELPER")"
grep -Fq 'overwrite=0' <<<"$extlinux_body" \
    || { echo 'FAIL: extlinux overwrite=0 trial pattern is not wired' >&2; exit 1; }
grep -Fq 'guard_extlinux_candidate_preserves_entries' <<<"$extlinux_body" \
    || { echo 'FAIL: extlinux entry guard is not wired' >&2; exit 1; }
grep -Fq 'extlinux_restore_backup' <<<"$extlinux_body" \
    || { echo 'FAIL: extlinux rollback is not wired' >&2; exit 1; }
if grep -Eq 'extlinux --(install|update)|dd if=|mbr\.bin|gptmbr' <<<"$extlinux_body"; then
    echo 'FAIL: extlinux repair must not write the boot sector or reinstall the loader' >&2
    exit 1
fi

# Full extlinux repair behaviour with a stubbed chroot runner: a changed
# candidate is installed and reported changed, a byte-identical trial reports
# unchanged, and an entry-losing candidate is rolled back.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    extlinux_root="$(mktemp -d)"
    trap 'rm -rf -- "$extlinux_root"' EXIT
    SESSION_LOG="$extlinux_root/session.log"
    SESSION_DIR="$extlinux_root/session"
    mkdir -p "$SESSION_DIR" "$extlinux_root/boot" "$extlinux_root/etc" "$extlinux_root/sbin"
    : > "$SESSION_LOG"
    printf 'overwrite=1\ndefault=lts\n' > "$extlinux_root/etc/update-extlinux.conf"
    : > "$extlinux_root/sbin/update-extlinux"; chmod +x "$extlinux_root/sbin/update-extlinux"
    : > "$extlinux_root/boot/vmlinuz-lts"
    : > "$extlinux_root/boot/initramfs-lts"
    cat > "$extlinux_root/boot/extlinux.conf" <<'EXTLINUX'
DEFAULT menu.c32
LABEL lts
  LINUX vmlinuz-lts
  INITRD initramfs-lts
EXTLINUX
    TARGET_ROOT="$extlinux_root"
    TARGET_DISTRO_FAMILY=alpine
    preflight_extlinux() { :; }
    run_chroot_try() {
        CHROOT_TRY_RC=0
        printf 'DEFAULT menu.c32\nLABEL lts\n  LINUX vmlinuz-lts\n  INITRD initramfs-lts\nLABEL recovery\n  LINUX vmlinuz-lts\n' \
            > "$TARGET_ROOT/boot/extlinux.conf.new"
    }
    extlinux_out="$(adaptive_extlinux_repair)"
    grep -Fqx 'Repair change status extlinux: changed' <<<"$extlinux_out" \
        || { echo 'FAIL: extlinux repair change status is wrong' >&2; printf '%s\n' "$extlinux_out" >&2; exit 1; }
    grep -Fqx 'overwrite=1' "$extlinux_root/etc/update-extlinux.conf" \
        || { echo 'FAIL: extlinux config was not restored after the trial' >&2; exit 1; }
    grep -Fq 'LABEL recovery' "$extlinux_root/boot/extlinux.conf" \
        || { echo 'FAIL: extlinux candidate was not installed' >&2; exit 1; }
    [[ ! -e "$extlinux_root/boot/extlinux.conf.new" ]] \
        || { echo 'FAIL: extlinux candidate was left behind' >&2; exit 1; }
    run_chroot_try() { CHROOT_TRY_RC=0; }
    extlinux_out="$(adaptive_extlinux_repair)"
    grep -Fqx 'Repair change status extlinux: unchanged|update-extlinux generated a byte-identical configuration' <<<"$extlinux_out" \
        || { echo 'FAIL: extlinux no-op change status is wrong' >&2; printf '%s\n' "$extlinux_out" >&2; exit 1; }
    run_chroot_try() {
        CHROOT_TRY_RC=0
        printf 'DEFAULT menu.c32\nLABEL other\n  LINUX vmlinuz-lts\n' > "$TARGET_ROOT/boot/extlinux.conf.new"
    }
    if extlinux_out="$(adaptive_extlinux_repair 2>&1)"; then
        echo 'FAIL: extlinux entry loss did not fail' >&2
        exit 1
    fi
    grep -Fq 'LABEL lts' "$extlinux_root/boot/extlinux.conf" \
        || { echo 'FAIL: extlinux rollback did not restore the configuration' >&2; exit 1; }
    grep -Fqx 'overwrite=1' "$extlinux_root/etc/update-extlinux.conf" \
        || { echo 'FAIL: extlinux config was not restored after rollback' >&2; exit 1; }
)

# The BIOS/extlinux boot chain must never claim a firmware EFI entry.
boot_chain_out="$(TARGET_ROOT="$cap_alpine" TARGET_OS_ID=alpine TARGET_OS_LIKE="" \
    TARGET_DISTRO_FAMILY=alpine TARGET_INITRAMFS_BACKEND=mkinitfs \
    TARGET_BOOTLOADER_BACKEND=syslinux/extlinux diagnostic_boot_chain 2>&1)"
grep -Fq 'Primary: BIOS/legacy firmware -> extlinux menu -> initramfs -> root filesystem -> graphical login.' <<<"$boot_chain_out" \
    || { echo 'FAIL: BIOS/extlinux boot chain was not described' >&2; exit 1; }
grep -Fq 'Default extlinux entry: LABEL lts -> LINUX vmlinuz-lts + INITRD initramfs-lts.' <<<"$boot_chain_out" \
    || { echo 'FAIL: default extlinux entry was not named in the boot chain' >&2; exit 1; }
if grep -Fq 'firmware EFI entry' <<<"$boot_chain_out"; then
    echo 'FAIL: BIOS/extlinux boot chain still claims a firmware EFI entry' >&2
    exit 1
fi

# Alpine kernel/initramfs pairing: initramfs-* is the real initramfs name.
kernel_out="$(TARGET_ROOT="$cap_alpine" TARGET_OS_ID=alpine TARGET_OS_LIKE="" \
    TARGET_DISTRO_FAMILY=alpine TARGET_INITRAMFS_BACKEND=mkinitfs diagnostic_kernel 2>&1 || true)"
grep -Fq 'PASS: vmlinuz-lts has matching mkinitfs initramfs initramfs-lts.' <<<"$kernel_out" \
    || { echo 'FAIL: Alpine lts initramfs pairing was not detected' >&2; exit 1; }
grep -Fq 'PASS: vmlinuz-virt has matching mkinitfs initramfs initramfs-virt.' <<<"$kernel_out" \
    || { echo 'FAIL: Alpine virt initramfs pairing was not detected' >&2; exit 1; }
: > "$cap_alpine/boot/vmlinuz-edge"
kernel_out="$(TARGET_ROOT="$cap_alpine" TARGET_OS_ID=alpine TARGET_OS_LIKE="" \
    TARGET_DISTRO_FAMILY=alpine TARGET_INITRAMFS_BACKEND=mkinitfs diagnostic_kernel 2>&1 || true)"
grep -Fq 'FAIL: vmlinuz-edge has no matching initramfs.' <<<"$kernel_out" \
    || { echo 'FAIL: Alpine kernel without initramfs was not reported' >&2; exit 1; }
rm -f "$cap_alpine/boot/vmlinuz-edge"

# The plain-text syslog filter keeps the same boot identifiers as the journald
# filter and drops user-session application noise.
syslog_filter_input="$(cat <<'SYSLOG'
Sep 19 08:14:51 alpine kernel: [    0.000000] Linux version 6.18.52-0-lts
Sep 19 08:14:51 alpine daemon.info init: starting pid 2526, tty '': '/sbin/openrc default'
Sep 19 08:33:14 alpine daemon.warn supervise-daemon[3289]: /usr/bin/lightdm, pid 3290, exited with return code 1
Sep 19 08:35:00 alpine daemon.info crond[2560]: USER root pid 2614 cmd run-parts /etc/periodic/15min
Sep 19 08:35:01 alpine user.info some-app[999]: unrelated application failure
Sep 19 08:35:02 alpine daemon.info cryptsetup[123]: Failed to unlock device root
Sep 19 08:35:03 alpine daemon.info fsck.ext4[77]: fsck failed on /dev/sda1
Sep 19 08:35:04 alpine daemon.info tuxedo-tomte[88]: tuxedo service failure
SYSLOG
)"
syslog_filter_output="$(syslog_boot_relevant_filter <<<"$syslog_filter_input")"
grep -Fq 'kernel: [    0.000000] Linux version' <<<"$syslog_filter_output" \
    || { echo 'FAIL: syslog filter dropped the kernel line' >&2; exit 1; }
grep -Fq 'cryptsetup[123]: Failed to unlock device root' <<<"$syslog_filter_output" \
    || { echo 'FAIL: syslog filter dropped the cryptsetup line' >&2; exit 1; }
grep -Fq 'fsck.ext4[77]: fsck failed on /dev/sda1' <<<"$syslog_filter_output" \
    || { echo 'FAIL: syslog filter dropped the fsck line' >&2; exit 1; }
grep -Fq 'tuxedo-tomte[88]: tuxedo service failure' <<<"$syslog_filter_output" \
    || { echo 'FAIL: syslog filter dropped the tuxedo line' >&2; exit 1; }
for syslog_noise in 'starting pid' 'supervise-daemon' 'crond' 'unrelated application failure'; do
    if grep -Fq "$syslog_noise" <<<"$syslog_filter_output"; then
        echo "FAIL: syslog filter kept unrelated line: $syslog_noise" >&2
        exit 1
    fi
done

# The OpenRC display branch must name the provide display-manager service, its
# runlevel and its apk package, and fall back to OpenRC syslog.  It must never
# print the systemd-only failures from the baseline.
display_out="$(TARGET_ROOT="$cap_alpine" TARGET_OS_ID=alpine TARGET_OS_LIKE="" \
    TARGET_DISTRO_FAMILY=alpine TARGET_INITRAMFS_BACKEND=mkinitfs \
    TARGET_BOOTLOADER_BACKEND=syslinux/extlinux RUNNING_HOST_MODE=0 diagnostic_display 2>&1)"
grep -Fq 'OpenRC service manager:' <<<"$display_out" \
    || { echo 'FAIL: OpenRC display branch did not run' >&2; exit 1; }
grep -Fq 'LightDM: service lightdm provides display-manager; enabled in runlevel(s): default; package lightdm: installed' <<<"$display_out" \
    || { echo 'FAIL: OpenRC configured display manager was not reported' >&2; exit 1; }
grep -Fq 'LightDM: package=lightdm (installed); init script=/etc/init.d/lightdm; enabled runlevels=default [configured]' <<<"$display_out" \
    || { echo 'FAIL: OpenRC installed display manager was not reported' >&2; exit 1; }
grep -Fq 'Recent target display-manager boot evidence (syslog log, latest entries):' <<<"$display_out" \
    || { echo 'FAIL: OpenRC syslog fallback label is missing' >&2; exit 1; }
grep -Fq 'supervise-daemon[3289]: /usr/bin/lightdm, pid 3290, exited with return code 1' <<<"$display_out" \
    || { echo 'FAIL: OpenRC display-manager failure evidence is missing' >&2; exit 1; }
for forbidden in 'display-manager.service is not enabled' 'Unable to determine default target' \
    'dpkg-query is not available' 'graphical.target' 'guest-exec called'; do
    if grep -Fq "$forbidden" <<<"$display_out"; then
        echo "FAIL: OpenRC display output contains systemd/user-session noise: $forbidden" >&2
        exit 1
    fi
done

# The syslog fallback also covers boot evidence and error diagnostics, and it
# includes the Alpine apk log where package failures matter.
mkdir -p "$SESSION_DIR"
evidence_out="$(TARGET_ROOT="$cap_alpine" TARGET_OS_ID=alpine TARGET_OS_LIKE="" \
    TARGET_DISTRO_FAMILY=alpine TARGET_INITRAMFS_BACKEND=mkinitfs \
    TARGET_BOOTLOADER_BACKEND=syslinux/extlinux RUNNING_HOST_MODE=0 diagnostic_boot_evidence 2>&1 || true)"
grep -Fq 'Target extlinux configuration (/boot/extlinux.conf):' <<<"$evidence_out" \
    || { echo 'FAIL: extlinux configuration missing from boot evidence' >&2; exit 1; }
grep -Fq 'LABEL lts [default]' <<<"$evidence_out" \
    || { echo 'FAIL: extlinux menu entries missing from boot evidence' >&2; exit 1; }
grep -Fq 'Boot-selection and kernel messages (syslog log, latest entries):' <<<"$evidence_out" \
    || { echo 'FAIL: boot-evidence syslog fallback label is missing' >&2; exit 1; }
grep -Fq 'Source: syslog log (latest entries).' <<<"$evidence_out" \
    || { echo 'FAIL: unlock-evidence syslog fallback label is missing' >&2; exit 1; }
grep -Fq 'initramfs-lts' <<<"$evidence_out" \
    || { echo 'FAIL: initramfs-* file missing from boot-selection candidates' >&2; exit 1; }
if grep -Fq 'No persistent target journal is available.' <<<"$evidence_out"; then
    echo 'FAIL: boot evidence went dark despite OpenRC syslog files' >&2
    exit 1
fi

errors_out="$(TARGET_ROOT="$cap_alpine" TARGET_OS_ID=alpine TARGET_OS_LIKE="" \
    TARGET_DISTRO_FAMILY=alpine TARGET_INITRAMFS_BACKEND=mkinitfs \
    TARGET_BOOTLOADER_BACKEND=syslinux/extlinux RUNNING_HOST_MODE=0 diagnostic_errors 2>&1 || true)"
grep -Fq 'Recent target error-priority syslog entries (syslog log, latest entries):' <<<"$errors_out" \
    || { echo 'FAIL: error diagnostics syslog fallback label is missing' >&2; exit 1; }
grep -Fq 'Recent target apk package-manager log errors (/var/log/apk.log):' <<<"$errors_out" \
    || { echo 'FAIL: apk log fallback section is missing' >&2; exit 1; }
grep -Fq 'ERROR: unable to select packages:' <<<"$errors_out" \
    || { echo 'FAIL: apk log errors were not reported' >&2; exit 1; }
if grep -Fq 'no persistent /var/log/journal' <<<"$errors_out"; then
    echo 'FAIL: error diagnostics went dark despite OpenRC syslog files' >&2
    exit 1
fi

# Package-manager log filtering: known transient per-mirror retry noise is
# dropped, volatile fields (mount prefix, timestamp, pid, URL, mirror IP) are
# normalized so identical messages deduplicate, real errors are prioritized
# and the cap reports how many lines it suppressed.
package_log_input="$(cat <<'PKGLOG'
/run/mount/var/log/dnf5.log.1:2026-09-20T05:51:58+0000 [2392] INFO [librepo] Error during transfer: Status code: 404 for https://mirror-a.example/fedora/x.rpm (IP: 192.0.2.1)
/run/mount/var/log/dnf5.log.1:2026-09-20T05:51:58+0000 [2392] DEBUG [librepo] check_transfer_statuses: Ignore error - Try another mirror
/run/mount/var/log/dnf5.log.1:2026-09-20T05:51:59+0000 [2392] INFO [librepo] Error during transfer: Status code: 404 for http://mirror-b.example/fedora/x.rpm (IP: 192.0.2.2)
/run/mount/var/log/dnf5.log:2026-09-20T06:00:00+0000 [7] ERROR Command returned error: Failed to download packages
/run/mount/var/log/dnf5.log:2026-09-20T06:00:01+0000 [7] TRACE Sync check: failed for repo "updates", sha256 checksum mismatch
PKGLOG
)"
package_log_out="$(package_log_filter <<<"$package_log_input")"
grep -Fq 'dnf5.log:<time> [<pid>] ERROR Command returned error: Failed to download packages' <<<"$package_log_out" \
    || { echo 'FAIL: real package-manager error was dropped' >&2; printf '%s\n' "$package_log_out" >&2; exit 1; }
grep -Fq 'TRACE Sync check: failed for repo "updates", sha256 checksum mismatch' <<<"$package_log_out" \
    || { echo 'FAIL: checksum mismatch was dropped' >&2; printf '%s\n' "$package_log_out" >&2; exit 1; }
if grep -Fq 'Error during transfer' <<<"$package_log_out"; then
    echo 'FAIL: transient mirror-retry noise was kept' >&2
    printf '%s\n' "$package_log_out" >&2
    exit 1
fi
grep -Fq 'Suppressed: 3 transient mirror-retry line(s), 0 further unique message(s).' <<<"$package_log_out" \
    || { echo 'FAIL: package-log suppression note is missing or wrong' >&2; printf '%s\n' "$package_log_out" >&2; exit 1; }

# The cap keeps the report bounded and always states the suppressed count.
package_log_cap_input=""
for cap_i in $(seq 1 12); do
    package_log_cap_input+="dnf5.log:2026-01-01T00:00:00+0000 [1] ERROR Command returned error: failure $cap_i"$'\n'
done
for cap_i in $(seq 1 5); do
    package_log_cap_input+="dnf5.log:2026-01-01T00:00:00+0000 [1] DEBUG [librepo] check_transfer_statuses: Ignore error - Try another mirror"$'\n'
done
package_log_cap_out="$(package_log_filter <<<"$package_log_cap_input")"
[[ "$(grep -c 'Command returned error' <<<"$package_log_cap_out" || true)" -eq 10 ]] \
    || { echo 'FAIL: package-log cap did not keep exactly 10 lines' >&2; printf '%s\n' "$package_log_cap_out" >&2; exit 1; }
grep -Fq 'Suppressed: 5 transient mirror-retry line(s), 2 further unique message(s).' <<<"$package_log_cap_out" \
    || { echo 'FAIL: package-log cap suppression note is missing or wrong' >&2; printf '%s\n' "$package_log_cap_out" >&2; exit 1; }

# The dnf5 section on a noisy target reports the real error, not the flood of
# mirror retries, and names the suppression.
{
    for cap_i in $(seq 1 25); do
        printf '2026-09-20T05:52:%02d+0000 [%d] INFO [librepo] Error during transfer: Status code: 404 for https://mirror-%d.example/fedora/x.rpm (IP: 10.0.0.%d)\n' \
            "$cap_i" "$cap_i" "$cap_i" "$cap_i"
        printf '2026-09-20T05:52:%02d+0000 [%d] DEBUG [librepo] check_transfer_statuses: Ignore error - Try another mirror\n' \
            "$cap_i" "$cap_i"
    done
    printf '2026-09-20T06:00:00+0000 [7] ERROR Command returned error: Failed to download packages\n'
} > "$cap_alpine/var/log/dnf5.log"
dnf_log_out="$(TARGET_ROOT="$cap_alpine" TARGET_DISTRO_FAMILY=alpine diagnostic_package_manager_logs target)"
grep -Fq 'Recent target dnf5 package-manager log errors (/var/log/dnf5.log):' <<<"$dnf_log_out" \
    || { echo 'FAIL: dnf5 log section is missing' >&2; exit 1; }
grep -Fq 'Command returned error: Failed to download packages' <<<"$dnf_log_out" \
    || { echo 'FAIL: dnf5 real error was not reported' >&2; printf '%s\n' "$dnf_log_out" >&2; exit 1; }
if grep -Fq 'Error during transfer' <<<"$dnf_log_out"; then
    echo 'FAIL: dnf5 mirror-retry noise leaked into the diagnostics section' >&2
    printf '%s\n' "$dnf_log_out" >&2
    exit 1
fi
grep -Fq 'Suppressed: 50 transient mirror-retry line(s)' <<<"$dnf_log_out" \
    || { echo 'FAIL: dnf5 suppression count is missing' >&2; printf '%s\n' "$dnf_log_out" >&2; exit 1; }

# GAP-4 regression: a target with no journal, no syslog and no dmesg must
# report that missing evidence source instead of blaming the recovery host.
# An empty /var/log/journal directory is not journal evidence: systemd creates
# it even with no journal files, and journalctl then answers "No journal files
# were found."  The probe must fall through to the no-evidence message.
no_log_root="$(mktemp -d)"
trap 'rm -rf -- "$fake_root" "$dracut_root" "$dracut_pkg_root" "$cap_root" "$cap_arch_min" "$cap_alpine" "$pair_root" "$mixed_root" "$extlinux_debian_root" "$alpine_efi_root" "$rpm_root" "$no_log_root"' EXIT
mkdir -p "$no_log_root/etc" "$no_log_root/var/log/journal"
if TARGET_ROOT="$no_log_root" RUNNING_HOST_MODE=0 target_journal_evidence_present; then
    echo 'FAIL: an empty /var/log/journal directory was accepted as journal evidence' >&2
    exit 1
fi
no_log_out="$(TARGET_ROOT="$no_log_root" TARGET_OS_ID=alpine TARGET_OS_LIKE="" \
    TARGET_DISTRO_FAMILY=alpine RUNNING_HOST_MODE=0 diagnostic_errors 2>&1 || true)"
grep -Fq 'has no persistent journal, syslog or dmesg evidence source.' <<<"$no_log_out" \
    || { echo 'FAIL: GAP-4 no-log message is missing' >&2; printf '%s\n' "$no_log_out" >&2; exit 1; }
if grep -Fq 'journalctl is not installed in the recovery host.' <<<"$no_log_out"; then
    echo 'FAIL: GAP-4 regressed: the recovery host was blamed for a target without logs' >&2
    exit 1
fi
if grep -Fq 'No journal files were found' <<<"$no_log_out"; then
    echo 'FAIL: an empty target journal directory still reached journalctl' >&2
    printf '%s\n' "$no_log_out" >&2
    exit 1
fi
mkdir -p "$no_log_root/var/log/journal/machine-id"
: > "$no_log_root/var/log/journal/machine-id/system.journal"
TARGET_ROOT="$no_log_root" RUNNING_HOST_MODE=0 target_journal_evidence_present \
    || { echo 'FAIL: a target journal file was not accepted as journal evidence' >&2; exit 1; }

mkdir -p "$cap_root"/usr/bin "$cap_root"/usr/lib/modules/6.12.1-arch1-1 "$cap_root"/etc "$cap_root"/var/lib/pacman/local
: > "$cap_root/usr/bin/pacman"; chmod +x "$cap_root/usr/bin/pacman"
: > "$cap_root/usr/bin/dkms"; chmod +x "$cap_root/usr/bin/dkms"
: > "$cap_root/etc/pacman.conf"
cap_expect \
    "$(TARGET_ROOT="$cap_root" TARGET_OS_ID=arch TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=arch \
        TARGET_INITRAMFS_BACKEND=mkinitcpio diagnostic_repair_capabilities)" \
    'dkms: unavailable|Arch DKMS preflight found no build tree for installed kernel 6.12.1-arch1-1; install the matching headers and retry' \
    'upgrade: available'

gate_bin="$fake_root/gate-bin"
mkdir -p "$gate_bin"
cat > "$gate_bin/pgrep" <<'GATE'
#!/usr/bin/env bash
name="${2:-}"
[[ -n "${FAKE_PKG_GATE_PROC:-}" && "$name" == "$FAKE_PKG_GATE_PROC" ]]
GATE
cat > "$gate_bin/fuser" <<'GATE'
#!/usr/bin/env bash
[[ -n "${FAKE_PKG_GATE_LOCK:-}" ]] && exit 0
exit 1
GATE
chmod +x "$gate_bin/pgrep" "$gate_bin/fuser"
SESSION_LOG="$fake_root/gate.log"
FAKE_PKG_GATE_PROC=packagekitd PATH="$gate_bin:$PATH" host_package_manager_gate \
    || { echo 'FAIL: idle PackageKit daemon rejected' >&2; exit 1; }
grep -Fq 'PackageKit daemon is running' "$fake_root/gate.log"
if (FAKE_PKG_GATE_PROC=dpkg PATH="$gate_bin:$PATH" host_package_manager_gate); then
    echo 'FAIL: active dpkg process accepted' >&2
    exit 1
fi
if (FAKE_PKG_GATE_PROC=unattended-upgrade PATH="$gate_bin:$PATH" host_package_manager_gate); then
    echo 'FAIL: active unattended-upgrade accepted' >&2
    exit 1
fi
# The generic host gate is Debian/Arch-only: an apk process or database-lock
# conflict belongs to the modifying Alpine package-stage preflight, so
# read-only capability probes and the abuild check phase (an unprivileged user
# that cannot open the root-owned apk lock) are never refused here.
FAKE_PKG_GATE_PROC=apk PATH="$gate_bin:$PATH" host_package_manager_gate \
    || { echo 'FAIL: apk process was still refused by the generic host gate' >&2; exit 1; }
# Fedora host maintenance is refused while a dnf5/rpm process or a WRITE lock
# on the rpm/dnf5 lock files is active.
for rpm_gate_proc in dnf5 dnf dnf4 dnf-3 rpm rpmkeys dnf-automatic; do
    if (FAKE_PKG_GATE_PROC="$rpm_gate_proc" PATH="$gate_bin:$PATH" host_package_manager_gate); then
        echo "FAIL: active $rpm_gate_proc process accepted" >&2
        exit 1
    fi
done
if rpm_lock_fail="$( (rpm_lock_held() { return 0; }; host_package_manager_gate) 2>&1 )"; then
    echo 'FAIL: active rpm/dnf5 lock accepted' >&2
    exit 1
fi
grep -Fq 'Package manager lock on the rpm/dnf5 database is active' <<<"$rpm_lock_fail" \
    || { echo 'FAIL: rpm/dnf5 lock refusal reason changed' >&2; printf '%s\n' "$rpm_lock_fail" >&2; exit 1; }
if [[ -e /var/lib/dpkg/lock || -e /var/lib/dpkg/lock-frontend || -e /var/lib/apt/lists/lock \
    || -e /var/lib/pacman/db.lck ]]; then
    if (FAKE_PKG_GATE_PROC=packagekitd FAKE_PKG_GATE_LOCK=1 PATH="$gate_bin:$PATH" host_package_manager_gate); then
        echo 'FAIL: active package-manager lock accepted' >&2
        exit 1
    fi
fi

# A held apk database lock is evidence for read-only diagnostics, not a
# refusal: the capability probe must succeed and must name the lock.  The
# modifying fix-broken/upgrade preflight owns the fail-closed apk lock gate.
apk_lock="$cap_alpine/lib/apk/db/lock"
: > "$apk_lock"
flock "$apk_lock" -c 'sleep 60' &
apk_lock_holder=$!
apk_lock_held=false
for _ in 1 2 3 4 5; do
    if ! flock -n "$apk_lock" true 2>/dev/null; then
        apk_lock_held=true
        break
    fi
    sleep 1
done
[[ "$apk_lock_held" == true ]] \
    || { echo 'FAIL: could not hold the fixture apk database lock' >&2; exit 1; }

if ! cap_alpine_locked="$(cap_alpine_probe)"; then
    echo 'FAIL: held apk lock turned read-only capability output into a failure' >&2
    exit 1
fi
grep -Fqx 'Repair tool fixbroken: available' <<<"$cap_alpine_locked" \
    || { echo 'FAIL: held apk lock removed the available fixbroken capability' >&2; exit 1; }
grep -Fqx 'Repair capability evidence fixbroken: apk executable present; /etc/apk/repositories present; /lib/apk/db/installed present; /etc/apk/world present; apk database lock is held by another process' <<<"$cap_alpine_locked" \
    || { echo 'FAIL: held apk lock was not reported as capability evidence' >&2; exit 1; }

if apk_lock_refusal="$(TARGET_ROOT="$cap_alpine" TARGET_DISTRO_FAMILY=alpine alpine_apk_preflight 2>&1)"; then
    echo 'FAIL: Alpine apk preflight accepted a held database lock' >&2
    exit 1
fi
grep -Fq 'The target apk database is locked; refusing a concurrent transaction.' <<<"$apk_lock_refusal" \
    || { echo 'FAIL: Alpine apk lock refusal reason is missing' >&2; printf '%s\n' "$apk_lock_refusal" >&2; exit 1; }

# The host-side apk process check is part of the same modifying-stage
# preflight; a running apk must fail it closed.
if apk_proc_refusal="$(RUNNING_HOST_MODE=1 TARGET_ROOT="$cap_alpine" TARGET_DISTRO_FAMILY=alpine \
    PATH="$gate_bin:$PATH" FAKE_PKG_GATE_PROC=apk alpine_apk_preflight 2>&1)"; then
    echo 'FAIL: Alpine apk preflight accepted an active apk process' >&2
    exit 1
fi
grep -Fq "Package manager process 'apk' is already running; refusing a concurrent host package repair." <<<"$apk_proc_refusal" \
    || { echo 'FAIL: Alpine apk process refusal reason is missing' >&2; printf '%s\n' "$apk_proc_refusal" >&2; exit 1; }

kill "$apk_lock_holder" 2>/dev/null || true
wait "$apk_lock_holder" 2>/dev/null || true
rm -f "$apk_lock"

# Near-identical journal runs collapse to one line plus an occurrence count,
# while genuinely distinct lines and input order are preserved.
collapse_input="$(cat <<'COLLAPSE'
Sep 17 15:55:17 host proc[100]: Failed to find module for LoRA layer key: lora_transformer-layers.0.attention.out
Sep 17 15:55:17 host proc[100]: Failed to find module for LoRA layer key: lora_transformer-layers.0.attention.qkv
Sep 17 15:55:17 host proc[100]: Failed to find module for LoRA layer key: lora_transformer-layers.1.attention.out
Sep 17 15:55:18 host proc[100]: Failed to activate service 'org.bluez': timed out
Sep 17 15:55:19 host proc[100]: Failed to activate service 'org.bluez': timed out
Sep 17 15:55:20 host proc[100]: distinct failure at 0xdeadbeef
Sep 17 15:55:21 host proc[100]: distinct failure at 0xcafebabe
Sep 17 15:55:22 host proc[100]: another unrelated error: permission denied
COLLAPSE
)"
collapse_expected="$(cat <<'COLLAPSE'
Sep 17 15:55:17 host proc[100]: Failed to find module for LoRA layer key: lora_transformer-layers.0.attention.out (3 similar entries)
Sep 17 15:55:18 host proc[100]: Failed to activate service 'org.bluez': timed out (2 similar entries)
Sep 17 15:55:20 host proc[100]: distinct failure at 0xdeadbeef (2 similar entries)
Sep 17 15:55:22 host proc[100]: another unrelated error: permission denied
COLLAPSE
)"
collapse_output="$(collapse_similar_journal_lines <<<"$collapse_input")"
[[ "$collapse_output" == "$collapse_expected" ]] \
    || { echo 'FAIL: journal collapsing helper produced unexpected output' >&2; exit 1; }
grep -q 'collapse_similar_journal_lines' "$HELPER"

# Boot-relevant journal filtering must structurally keep kernel, system-unit
# and known boot-subsystem entries while dropping unrelated application and
# user-session noise.  Synthetic journalctl -o json lines only.
filter_input="$(cat <<'FILTER'
{"__REALTIME_TIMESTAMP":"1726578917000000","_HOSTNAME":"host","_TRANSPORT":"stdout","SYSLOG_IDENTIFIER":"AppImage","_COMM":"appimage","MESSAGE":"Failed to find module for LoRA layer key: lora_transformer-layers.0.attention.out"}
{"__REALTIME_TIMESTAMP":"1726578918000000","_HOSTNAME":"host","_TRANSPORT":"kernel","_COMM":"kernel","MESSAGE":"usb 1-2: device descriptor read/64, error -71"}
{"__REALTIME_TIMESTAMP":"1726578919000000","_HOSTNAME":"host","_TRANSPORT":"stdout","_SYSTEMD_UNIT":"systemd-logind.service","SYSLOG_IDENTIFIER":"systemd-logind","MESSAGE":"Failed to start session scope"}
{"__REALTIME_TIMESTAMP":"1726578920000000","_HOSTNAME":"host","_TRANSPORT":"stdout","SYSLOG_IDENTIFIER":"cryptsetup","MESSAGE":"Failed to unlock device root"}
{"__REALTIME_TIMESTAMP":"1726578921000000","_HOSTNAME":"host","_TRANSPORT":"stdout","_SYSTEMD_UNIT":"user@1000.service","_SYSTEMD_USER_UNIT":"app-gnome-org.gnome.Nautilus-1234.scope","SYSLOG_IDENTIFIER":"nautilus","MESSAGE":"user-session nautilus failure"}
{"__REALTIME_TIMESTAMP":"1726578922000000","_HOSTNAME":"host","_TRANSPORT":"stdout","_SYSTEMD_UNIT":"user@1000.service","SYSLOG_IDENTIFIER":"systemd","MESSAGE":"user-manager failure line"}
{"__REALTIME_TIMESTAMP":"1726578923000000","_HOSTNAME":"host","_TRANSPORT":"stdout","_SYSTEMD_USER_UNIT":"app-gnome-org.gnome.Shell-1234.scope","SYSLOG_IDENTIFIER":"gnome-shell","MESSAGE":"user-unit failure line"}
{"__REALTIME_TIMESTAMP":"1726578924000000","_HOSTNAME":"host","_TRANSPORT":"stdout","_SYSTEMD_UNIT":"session-2.scope","SYSLOG_IDENTIFIER":"systemd","MESSAGE":"system scope failure line"}
{"__REALTIME_TIMESTAMP":"1726578925000000","_HOSTNAME":"host","_TRANSPORT":"stdout","_SYSTEMD_UNIT":"init.scope","SYSLOG_IDENTIFIER":"systemd","MESSAGE":"init scope failure line"}
{"__REALTIME_TIMESTAMP":"1726578926000000","_HOSTNAME":"host","_TRANSPORT":"stdout","SYSLOG_IDENTIFIER":"fsck.ext4","MESSAGE":"fsck failed on /dev/sda1"}
{"__REALTIME_TIMESTAMP":"1726578927000000","_HOSTNAME":"host","_TRANSPORT":"stdout","SYSLOG_IDENTIFIER":"tuxedo-tomte","MESSAGE":"tuxedo service failure"}
{"__REALTIME_TIMESTAMP":"1726578928000000","_HOSTNAME":"host","_TRANSPORT":"stdout","SYSLOG_IDENTIFIER":"some-app","MESSAGE":"unrelated app failure"}
FILTER
)"
filter_output="$(journal_boot_relevant_filter <<<"$filter_input")"
grep -Fq 'kernel: usb 1-2: device descriptor read/64, error -71' <<<"$filter_output" \
    || { echo 'FAIL: kernel journal entry was filtered out' >&2; exit 1; }
grep -Fq 'systemd-logind: Failed to start session scope' <<<"$filter_output" \
    || { echo 'FAIL: systemd unit journal entry was filtered out' >&2; exit 1; }
grep -Fq 'cryptsetup: Failed to unlock device root' <<<"$filter_output" \
    || { echo 'FAIL: boot-subsystem journal entry was filtered out' >&2; exit 1; }
if grep -Fq 'LoRA layer key' <<<"$filter_output"; then
    echo 'FAIL: unitless application journal entry was kept' >&2
    exit 1
fi
if grep -Fq 'user-session nautilus failure' <<<"$filter_output"; then
    echo 'FAIL: user-session application journal entry was kept' >&2
    exit 1
fi
if grep -Fq 'user-manager failure line' <<<"$filter_output"; then
    echo 'FAIL: user@ session unit journal entry was kept' >&2
    exit 1
fi
if grep -Fq 'user-unit failure line' <<<"$filter_output"; then
    echo 'FAIL: _SYSTEMD_USER_UNIT journal entry was kept' >&2
    exit 1
fi
grep -Fq 'init scope failure line' <<<"$filter_output" \
    || { echo 'FAIL: init.scope journal entry was filtered out' >&2; exit 1; }
grep -Fq 'fsck.ext4: fsck failed on /dev/sda1' <<<"$filter_output" \
    || { echo 'FAIL: fsck journal entry was filtered out' >&2; exit 1; }
grep -Fq 'tuxedo-tomte: tuxedo service failure' <<<"$filter_output" \
    || { echo 'FAIL: tuxedo journal entry was filtered out' >&2; exit 1; }
if grep -Fq 'system scope failure line' <<<"$filter_output"; then
    echo 'FAIL: non-init system scope journal entry was kept' >&2
    exit 1
fi
if grep -Fq 'unrelated app failure' <<<"$filter_output"; then
    echo 'FAIL: unknown unitless application journal entry was kept' >&2
    exit 1
fi
if grep -Eq '"_TRANSPORT"|"MESSAGE"' <<<"$filter_output"; then
    echo 'FAIL: journal filter did not emit plain text' >&2
    exit 1
fi

# The awk fallback used on recovery hosts without python3 must make the same
# structural decisions as the python3 parser.
no_python_bin="$(mktemp -d)"
ln -s "$(command -v awk)" "$no_python_bin/awk"
fallback_output="$(PATH="$no_python_bin" journal_boot_relevant_filter <<<"$filter_input")"
rm -rf -- "$no_python_bin"
grep -Fq 'kernel: usb 1-2: device descriptor read/64, error -71' <<<"$fallback_output" \
    || { echo 'FAIL: awk fallback dropped kernel journal entry' >&2; exit 1; }
grep -Fq 'cryptsetup: Failed to unlock device root' <<<"$fallback_output" \
    || { echo 'FAIL: awk fallback dropped boot-subsystem journal entry' >&2; exit 1; }
grep -Fq 'init scope failure line' <<<"$fallback_output" \
    || { echo 'FAIL: awk fallback dropped init.scope journal entry' >&2; exit 1; }
if grep -Eq 'LoRA layer key|user-session nautilus failure|user-manager failure line|user-unit failure line|system scope failure line|unrelated app failure' <<<"$fallback_output"; then
    echo 'FAIL: awk fallback kept unrelated journal noise' >&2
    exit 1
fi

# Both diagnostic_errors journal sections must run the filter on journalctl
# JSON before the actionability and collapsing stages.
awk '
    /^diagnostic_errors\(\)/ { in_errors = 1; next }
    in_errors && /^}/ { exit(seen == 2 && filtered == 2 ? 0 : 1) }
    in_errors && /journalctl --root=.*-o json/ { seen++; expect = 1; next }
    in_errors && expect && /journal_boot_relevant_filter/ { filtered++; expect = 0 }
' "$HELPER" \
    || { echo 'FAIL: journal_boot_relevant_filter is not wired into both diagnostic_errors sections' >&2; exit 1; }

# Every full-journal query in diagnostic_boot_evidence and diagnostic_display
# must request JSON, run through the boot-relevant filter, and collapse
# near-identical lines.  The OpenRC syslog fallback stages add their own
# collapse stages, so each query needs at least one collapse stage.  Unit
# scoped (-u) queries and --list-boots are exempt.
awk '
    /^diagnostic_boot_evidence\(\)/ { target = "diagnostic_boot_evidence"; functions_seen++; queries = 0; filtered = 0; collapsed = 0; expect_filter = 0; next }
    /^diagnostic_display\(\)/ { target = "diagnostic_display"; functions_seen++; queries = 0; filtered = 0; collapsed = 0; expect_filter = 0; next }
    target == "" { next }
    /^}/ {
        if (queries == 0 || filtered != queries || collapsed < queries) {
            printf "FAIL: %s has %d full-journal queries, %d boot-relevant filters, %d collapse stages\n", target, queries, filtered, collapsed
            failed = 1
        }
        target = ""; next
    }
    expect_filter {
        if (/journal_boot_relevant_filter/) { filtered++; expect_filter = 0; next }
        printf "FAIL: %s full-journal query is not piped through journal_boot_relevant_filter\n", target
        failed = 1
        expect_filter = 0
        next
    }
    /journalctl --root=.*-b 0/ && !/ -u / {
        queries++
        if (!/-o json/) {
            printf "FAIL: %s full-journal query does not request -o json\n", target
            failed = 1
        }
        expect_filter = 1
        next
    }
    /collapse_similar_journal_lines/ { collapsed++ }
    END {
        if (functions_seen != 2) {
            print "FAIL: diagnostic_boot_evidence/diagnostic_display functions were not found"
            failed = 1
        }
        exit failed ? 1 : 0
    }
' "$HELPER" \
    || { echo 'FAIL: journal_boot_relevant_filter/collapse_similar_journal_lines are not wired into all diagnostic_boot_evidence and diagnostic_display queries' >&2; exit 1; }

# Running-host shell contract: Host Maintenance mode uses a dedicated
# host-shell command that validates the running host identity, activates the
# native host command guard and executes directly in the live root without
# ever entering a chroot or mounting the running host read-write.
grep -q '^  \$PROGRAM_NAME host-shell' "$HELPER"
grep -q 'host-shell       Execute one reviewed command as root on the running host' "$HELPER"
grep -q 'host-validate|host-diagnose|host-repair|host-default|host-shell' "$HELPER"
grep -q '^        host-shell)' "$HELPER"
grep -q 'run_host_shell "\$TARGET_DISK" "\$ROOT_DEVICE" "\$1"' "$HELPER"
host_shell_body="$(sed -n '/^run_host_shell()/,/^}/p' "$HELPER")"
[[ -n "$host_shell_body" ]] || { echo 'FAIL: run_host_shell function is missing' >&2; exit 1; }
grep -q 'prepare_running_host "\$raw_disk" "\$raw_root" no' <<<"$host_shell_body"
grep -q 'RUNNING_HOST_MODE=1' <<<"$host_shell_body"
grep -q 'DIAGNOSTIC_SCOPE="Running Host"' <<<"$host_shell_body"
grep -q 'prepare_host_command_guard' <<<"$host_shell_body"
grep -q 'run_host_command_isolated' <<<"$host_shell_body"
grep -q 'HOME=/root' <<<"$host_shell_body"
grep -q '/bin/bash -lc' <<<"$host_shell_body"
grep -q 'DEBIAN_FRONTEND=noninteractive' <<<"$host_shell_body"
if grep -q 'chroot' <<<"$host_shell_body"; then
    echo 'FAIL: host-shell must not enter a chroot' >&2
    exit 1
fi
host_isolated_body="$(sed -n '/^run_host_command_isolated()/,/^}/p' "$HELPER")"
[[ -n "$host_isolated_body" ]] || { echo 'FAIL: run_host_command_isolated function is missing' >&2; exit 1; }
grep -q 'unshare --mount --propagation private' <<<"$host_isolated_body"
grep -q 'mount -o remount,bind,ro /sys/firmware/efi/efivars' <<<"$host_isolated_body"
if grep -q 'chroot' <<<"$host_isolated_body"; then
    echo 'FAIL: native host command isolation must not enter a chroot' >&2
    exit 1
fi

# APT release-info-change handling.  Vendor repository metadata changes
# (Origin/Label/Suite/Codename/Version) must trigger one explicit, warned
# retry with Acquire::AllowReleaseInfoChange=true, confined to the metadata
# refresh.  Every other apt failure must keep failing without the retry, and
# package install/upgrade transactions must never receive the option.
grep -q '^apt_update_allow_release_info_retry()' "$HELPER"
grep -q '^apt_update_release_info_change_only()' "$HELPER"
grep -q '^apt_update_release_info_change_repos()' "$HELPER"
grep -q '^apt_update_release_info_change_details()' "$HELPER"
grep -q '^run_apt_update()' "$HELPER"
run_apt_update_body="$(sed -n '/^run_apt_update()/,/^}/p' "$HELPER")"
grep -q 'apt_update_allow_release_info_retry' <<<"$run_apt_update_body" \
    || { echo 'FAIL: run_apt_update does not use the release-info retry path' >&2; exit 1; }
for apt_txn_fn in adaptive_apt_upgrade adaptive_fix_broken simulate_apt_upgrade_mode; do
    apt_txn_body="$(sed -n "/^$apt_txn_fn()/,/^}/p" "$HELPER")"
    [[ -n "$apt_txn_body" ]] || { echo "FAIL: $apt_txn_fn is missing" >&2; exit 1; }
    if grep -q 'AllowReleaseInfoChange' <<<"$apt_txn_body"; then
        echo "FAIL: package transaction $apt_txn_fn must not accept release-info changes" >&2
        exit 1
    fi
done
retry_body="$(sed -n '/^apt_update_allow_release_info_retry()/,/^}/p' "$HELPER")"
grep -q 'apt-get update -o Acquire::AllowReleaseInfoChange=true' <<<"$retry_body" \
    || { echo 'FAIL: retry does not pass Acquire::AllowReleaseInfoChange=true' >&2; exit 1; }

# The classifier must accept every documented release-info field and the
# paired apt-secure notice, while rejecting unrelated apt errors and integrity
# warnings even when a release-info error is also present.
for apt_field in Origin Label Suite Codename Version; do
    apt_update_release_info_change_only \
        "E: Repository 'https://txos.tuxedocomputers.com/debian-cache testing InRelease' changed its '$apt_field' value from 'old' to 'new'
N: This must be accepted explicitly before updates for this repository can be applied. See apt-secure(8) manpage for details." \
        || { echo "FAIL: release-info classifier rejected field $apt_field" >&2; exit 1; }
done
apt_update_release_info_change_only \
    "E: Repository 'https://txos.tuxedocomputers.com/debian-cache testing InRelease' changed its 'Origin' value from 'old' to 'new'
W: Key is stored in legacy trusted.gpg keyring (/etc/apt/trusted.gpg), see the DEPRECATION section in apt-key(8) for details." \
    || { echo 'FAIL: release-info classifier rejected a benign deprecation warning' >&2; exit 1; }
if apt_update_release_info_change_only \
    "E: Repository 'https://txos.tuxedocomputers.com/debian-cache testing InRelease' changed its 'Origin' value from 'old' to 'new'
W: GPG error: https://txos.tuxedocomputers.com/debian-cache testing InRelease: The following signatures were invalid: KEYEXPIRED"; then
    echo 'FAIL: release-info classifier accepted a GPG integrity error' >&2
    exit 1
fi
if apt_update_release_info_change_only $'E: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/noble/InRelease  Could not connect to archive.ubuntu.com:80'; then
    echo 'FAIL: release-info classifier accepted an unrelated fetch error' >&2
    exit 1
fi

# Drive the full retry path with a stubbed chroot runner: the first apt-get
# update fails with release-info changes only, the explicit retry succeeds.
apt_retry_calls="$fake_root/apt-retry-calls.log"
apt_retry_first_output=""
apt_retry_retry_output=""
apt_retry_first_rc=0
apt_retry_retry_rc=0
run_selected_chroot()
{
    local joined="$*"
    printf '%s\n' "$joined" >> "$apt_retry_calls"
    if [[ "$joined" == *'Acquire::AllowReleaseInfoChange=true'* ]]; then
        printf '%s\n' "$apt_retry_retry_output"
        return "$apt_retry_retry_rc"
    fi
    printf '%s\n' "$apt_retry_first_output"
    return "$apt_retry_first_rc"
}
SESSION_LOG="$fake_root/apt-retry.log"
: > "$apt_retry_calls"
apt_retry_first_rc=100
apt_retry_retry_rc=0
apt_retry_first_output="$(cat <<'APT'
Hit:1 http://archive.ubuntu.com/ubuntu noble InRelease
E: Repository 'https://txos.tuxedocomputers.com/debian-cache testing InRelease' changed its 'Origin' value from 'TUXEDO Computers' to 'TUXEDO'
N: This must be accepted explicitly before updates for this repository can be applied. See apt-secure(8) manpage for details.
E: Repository 'https://txos.tuxedocomputers.com/debian-security testing InRelease' changed its 'Origin' value from 'TUXEDO Computers' to 'TUXEDO'
N: This must be accepted explicitly before updates for this repository can be applied. See apt-secure(8) manpage for details.
E: Repository 'https://txos.tuxedocomputers.com/debian-cache testing InRelease' changed its 'Label' value from 'tuxedoos-testing' to 'tuxedoos-cache-testing'
N: This must be accepted explicitly before updates for this repository can be applied. See apt-secure(8) manpage for details.
APT
)"
apt_retry_retry_output="$(cat <<'APT'
Hit:1 http://archive.ubuntu.com/ubuntu noble InRelease
Reading package lists...
APT
)"
apt_update_allow_release_info_retry
[[ "$(wc -l < "$apt_retry_calls")" -eq 2 ]] \
    || { echo 'FAIL: release-info failure did not produce exactly one retry' >&2; exit 1; }
sed -n '1p' "$apt_retry_calls" | grep -Eq 'apt-get update$' \
    || { echo 'FAIL: initial apt-get update unexpectedly carried the release-info option' >&2; exit 1; }
sed -n '2p' "$apt_retry_calls" | grep -Fq 'apt-get update -o Acquire::AllowReleaseInfoChange=true' \
    || { echo 'FAIL: retry did not use Acquire::AllowReleaseInfoChange=true' >&2; exit 1; }
grep -Fq "WARNING: apt-get update refused a repository release metadata change for: https://txos.tuxedocomputers.com/debian-cache testing InRelease, https://txos.tuxedocomputers.com/debian-security testing InRelease" "$SESSION_LOG" \
    || { echo 'FAIL: warning does not name the changed repositories' >&2; exit 1; }
grep -Fq "WARN: accepted release metadata change for https://txos.tuxedocomputers.com/debian-cache testing InRelease, https://txos.tuxedocomputers.com/debian-security testing InRelease; metadata refreshed." "$SESSION_LOG" \
    || { echo 'FAIL: accepted release metadata change was not reported with the repository names' >&2; exit 1; }

# A release-info change mixed with any other apt error must fail without a
# retry, preserving the original failure.
: > "$apt_retry_calls"
apt_retry_first_rc=100
apt_retry_first_output="$(cat <<'APT'
E: Repository 'https://txos.tuxedocomputers.com/debian-cache testing InRelease' changed its 'Origin' value from 'TUXEDO Computers' to 'TUXEDO'
E: The repository 'https://txos.tuxedocomputers.com/debian-cache testing InRelease' is not signed.
APT
)"
if apt_retry_failure="$( (apt_update_allow_release_info_retry) 2>&1 )"; then
    echo 'FAIL: mixed release-info and signature error did not fail' >&2
    exit 1
fi
[[ "$(wc -l < "$apt_retry_calls")" -eq 1 ]] \
    || { echo 'FAIL: mixed apt error took the release-info retry path' >&2; exit 1; }
grep -Fq 'Refresh package metadata failed with exit code 100.' <<<"$apt_retry_failure" \
    || { echo 'FAIL: mixed apt error did not report the original failure' >&2; exit 1; }

# A retry that still fails must report the original error rather than pretend
# the metadata refresh succeeded.
: > "$apt_retry_calls"
apt_retry_first_rc=100
apt_retry_retry_rc=100
apt_retry_first_output="E: Repository 'https://txos.tuxedocomputers.com/debian-cache testing InRelease' changed its 'Origin' value from 'TUXEDO Computers' to 'TUXEDO'"
apt_retry_retry_output="E: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/noble/InRelease  Could not connect to archive.ubuntu.com:80"
if apt_retry_failure="$( (apt_update_allow_release_info_retry) 2>&1 )"; then
    echo 'FAIL: failing release-info retry did not fail' >&2
    exit 1
fi
[[ "$(wc -l < "$apt_retry_calls")" -eq 2 ]] \
    || { echo 'FAIL: failing release-info retry did not retry exactly once' >&2; exit 1; }
grep -Fq 'Original error (exit code 100): E: Repository' <<<"$apt_retry_failure" \
    || { echo 'FAIL: failing retry did not preserve the original apt error' >&2; exit 1; }

# APT metadata refresh change evidence: an all-Hit refresh that leaves
# /var/lib/apt/lists byte-identical is a proven no-op; a fetched or rewritten
# index reports changed with explicit evidence.
apt_lists_root="$(mktemp -d)"
mkdir -p "$apt_lists_root/var/lib/apt/lists"
printf 'Packages\n' > "$apt_lists_root/var/lib/apt/lists/archive.example_Packages"
(
    TARGET_ROOT="$apt_lists_root"
    SESSION_LOG="$fake_root/apt-lists.log"
    : > "$SESSION_LOG"
    run_selected_chroot() { printf 'Hit:1 http://archive.example stable InRelease\nReading package lists...\n'; }
    apt_update_allow_release_info_retry
) > "$apt_lists_root/unchanged.out" 2>&1
grep -Fqx 'Repair change status aptupdate: unchanged|APT package lists are byte-identical and no repository index was fetched' "$apt_lists_root/unchanged.out" \
    || { echo 'FAIL: byte-identical apt lists were not reported unchanged' >&2; cat "$apt_lists_root/unchanged.out" >&2; exit 1; }
(
    TARGET_ROOT="$apt_lists_root"
    SESSION_LOG="$fake_root/apt-lists.log"
    run_selected_chroot()
    {
        printf 'Get:1 http://archive.example stable InRelease\n'
        printf 'rewritten\n' > "$TARGET_ROOT/var/lib/apt/lists/archive.example_Packages"
    }
    apt_update_allow_release_info_retry
) > "$apt_lists_root/changed.out" 2>&1
grep -Fqx 'Repair change status aptupdate: changed' "$apt_lists_root/changed.out" \
    || { echo 'FAIL: a rewritten apt list did not report changed' >&2; cat "$apt_lists_root/changed.out" >&2; exit 1; }
grep -Fq 'APT package lists changed or a repository index was fetched' "$apt_lists_root/changed.out" \
    || { echo 'FAIL: apt changed evidence wording is missing' >&2; cat "$apt_lists_root/changed.out" >&2; exit 1; }
rm -rf -- "$apt_lists_root"

# ---------------------------------------------------------------------------
# Reconcile boot stack reuse. The UI passes the explicit --post-efi hint when
# the EFI/UKI stage already ran for the same plan/session; boot-stack then
# skips the second UKI rebuild and the duplicate GRUB regeneration and logs
# exactly what it skipped, while mapper/crypttab validation and initramfs
# reconciliation still run. Without the hint the complete reconciliation runs;
# the helper never infers reuse from the stage list.
# ---------------------------------------------------------------------------
grep -q '^parse_repair_arguments()' "$HELPER"
grep -Fq -- '--post-efi' "$HELPER"
grep -Fq 'REPAIR_POST_EFI' "$HELPER"
grep -Fq 'BOOT_STACK_POST_EFI' "$HELPER"
grep -Fq 'SKIP: boot-stack EFI/UKI rebuild skipped because the EFI / UKI bootloader stage already rebuilt and verified this layout in the same run (--post-efi).' "$HELPER"
grep -Fq 'SKIP: boot-stack GRUB regeneration skipped because the EFI / UKI bootloader stage already regenerated the GRUB configuration in the same run (--post-efi).' "$HELPER"

parse_repair_arguments efi boot-stack --post-efi
[[ "${REPAIR_STAGES[*]}" == 'efi boot-stack' ]] \
    || { echo 'FAIL: --post-efi leaked into the repair stage list' >&2; exit 1; }
[[ "$REPAIR_POST_EFI" == true ]] \
    || { echo 'FAIL: --post-efi did not set the explicit reuse hint' >&2; exit 1; }
parse_repair_arguments grub
[[ "$REPAIR_POST_EFI" == false ]] \
    || { echo 'FAIL: the reuse hint leaked across argument parses' >&2; exit 1; }
if (parse_repair_arguments efi --bogus-hint) >/dev/null 2>&1; then
    echo 'FAIL: an unknown repair mode hint was accepted' >&2
    exit 1
fi

bootstack_stage_log="$fake_root/boot-stack-skip.log"
bootstack_calls="$fake_root/bootstack-calls.log"
validate_mapper_crypttab() { printf 'mapper\n' >> "$bootstack_calls"; }
adaptive_initramfs_repair() { printf 'initramfs\n' >> "$bootstack_calls"; }
preflight_tuxedo_uki() { printf 'preflight-uki\n' >> "$bootstack_calls"; }
rebuild_tuxedo_uki() { printf 'rebuild-uki\n' >> "$bootstack_calls"; }
verify_tuxedo_uki_root_binding() { printf 'verify-uki\n' >> "$bootstack_calls"; }
reinstall_efi_bootloader() { printf 'reinstall-efi\n' >> "$bootstack_calls"; }
adaptive_grub_repair() { printf 'grub\n' >> "$bootstack_calls"; }
is_tuxedo_uki_layout() { return 0; }
SESSION_LOG="$bootstack_stage_log"

: > "$bootstack_calls"
: > "$bootstack_stage_log"
BOOT_STACK_POST_EFI=false
repair_boot_stack
grep -Fq 'rebuild-uki' "$bootstack_calls" \
    || { echo 'FAIL: boot-stack did not rebuild the UKI without --post-efi' >&2; exit 1; }
grep -Fq 'grub' "$bootstack_calls" \
    || { echo 'FAIL: boot-stack did not regenerate GRUB without --post-efi' >&2; exit 1; }

: > "$bootstack_calls"
: > "$bootstack_stage_log"
BOOT_STACK_POST_EFI=true
repair_boot_stack
if grep -Fq 'rebuild-uki' "$bootstack_calls"; then
    echo 'FAIL: --post-efi still rebuilt the UKI' >&2
    exit 1
fi
if grep -Fq 'grub' "$bootstack_calls"; then
    echo 'FAIL: --post-efi still regenerated GRUB' >&2
    exit 1
fi
grep -Fq 'mapper' "$bootstack_calls" \
    || { echo 'FAIL: --post-efi skipped mapper/crypttab validation' >&2; exit 1; }
grep -Fq 'initramfs' "$bootstack_calls" \
    || { echo 'FAIL: --post-efi skipped initramfs reconciliation' >&2; exit 1; }
grep -Fq 'SKIP: boot-stack EFI/UKI rebuild skipped' "$bootstack_stage_log" \
    || { echo 'FAIL: the skipped UKI rebuild was not logged' >&2; exit 1; }
grep -Fq 'SKIP: boot-stack GRUB regeneration skipped' "$bootstack_stage_log" \
    || { echo 'FAIL: the skipped GRUB regeneration was not logged' >&2; exit 1; }

# The UI is the only source of the hint, and its label states the reuse.
grep -q '^bool MainWindow::efiRepairReuseAvailable() const' "$ROOT_DIR/src/MainWindow.cpp"
grep -Fq 'stages.append(QStringLiteral("--post-efi"))' "$ROOT_DIR/src/MainWindow.cpp"
grep -Fq 'second UKI rebuild and duplicate GRUB regeneration are skipped' "$ROOT_DIR/src/MainWindow.cpp"

# ---------------------------------------------------------------------------
# Evidence-driven diagnostics invalidation. Every successful modifying repair
# action emits exactly one stable "Repair change status <tool-key>:" line; a
# proven no-op reports unchanged while real work reports changed. The GUI
# consumes these lines to skip cached-diagnostics regeneration.
# ---------------------------------------------------------------------------
grep -q '^repair_change_status()' "$HELPER"
grep -Fq "printf 'Repair change status %s: %s\\n' \"\$key\" \"\$state\"" "$HELPER"
grep -q '^repair_file_fingerprint()' "$HELPER"
grep -q '^initramfs_image_fingerprint()' "$HELPER"
grep -q '^efi_boot_artifact_fingerprint()' "$HELPER"
grep -q '^apt_transaction_reported_no_changes()' "$HELPER"
grep -q '^dpkg_configuration_pending()' "$HELPER"
grep -q '^dpkg_configure_stage()' "$HELPER"
grep -q '^efi_repair_emit_change_status()' "$HELPER"
grep -q '^display_manager_already_correct()' "$HELPER"

# Every modifying stage owns a function that emits the status line at its end.
while read -r change_fn change_token; do
    [[ -n "$change_fn" ]] || continue
    fn_body="$(sed -n "/^$change_fn()/,/^}/p" "$HELPER")"
    [[ -n "$fn_body" ]] || { echo "FAIL: $change_fn is missing" >&2; exit 1; }
    grep -q "$change_token" <<<"$fn_body" \
        || { echo "FAIL: $change_fn does not emit a repair change status" >&2; exit 1; }
done <<'CHANGE_FUNCTIONS'
dpkg_configure_stage repair_change_status
adaptive_fix_broken repair_change_status
apt_update_allow_release_info_retry repair_change_status
adaptive_apt_upgrade repair_change_status
adaptive_arch_pacman_repair repair_change_status
adaptive_dkms_repair repair_change_status
adaptive_display_manager_repair repair_change_status
adaptive_initramfs_repair repair_change_status
reinstall_efi_bootloader efi_repair_emit_change_status
adaptive_grub_repair repair_change_status
repair_boot_stack repair_change_status
run_host_default repair_change_status
fs_repair repair_change_status
validate_target repair_change_status
validate_running_host repair_change_status
CHANGE_FUNCTIONS

# The repair dispatchers route their stages through those functions.
grep -Fq 'run_package_stage dpkg-configure dpkg' "$HELPER"
grep -Fq 'run_package_stage fix-broken fixbroken' "$HELPER"
grep -Fq 'run_package_stage apt-update aptupdate' "$HELPER"
grep -Fq 'run_package_stage apt-upgrade upgrade' "$HELPER"
grep -Fq 'adaptive_arch_pacman_repair "Repair Arch package dependencies" fixbroken' "$HELPER"
grep -Fq 'adaptive_arch_pacman_repair "Upgrade installed Arch packages" upgrade' "$HELPER"

# The live harness runs in a subshell that re-sources the helper so the stub
# overrides installed by the earlier boot-stack section cannot leak in.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP

    SESSION_LOG="$fake_root/change-status.log"
    SESSION_DIR="$fake_root/change-status-session"
    mkdir -p "$SESSION_DIR"
    : > "$SESSION_LOG"

    assert_single_change_status() {
        local output="$1" key="$2" expected="$3"
        [[ "$(grep -c '^Repair change status ' <<<"$output" || true)" -eq 1 ]] \
            || { echo "FAIL: expected exactly one change status line for $key" >&2; printf '%s\n' "$output" >&2; exit 1; }
        grep -Fqx "Repair change status $key: $expected" <<<"$output" \
            || { echo "FAIL: unexpected $key change status (expected '$expected')" >&2; printf '%s\n' "$output" >&2; exit 1; }
    }
    assert_change_status() {
        local output="$1" key="$2" expected="$3"
        [[ "$(grep -c "^Repair change status $key: " <<<"$output" || true)" -eq 1 ]] \
            || { echo "FAIL: expected exactly one $key change status line" >&2; printf '%s\n' "$output" >&2; exit 1; }
        grep -Fqx "Repair change status $key: $expected" <<<"$output" \
            || { echo "FAIL: unexpected $key change status (expected '$expected')" >&2; printf '%s\n' "$output" >&2; exit 1; }
    }

    # dpkg-configure: nothing pending -> unchanged; an unpacked package -> changed.
    TARGET_DISTRO_FAMILY=debian
    TARGET_ROOT="$fake_root"
    run_chroot() { :; }
    run_selected_chroot() { printf 'ii \nii \n'; }
    change_out="$(dpkg_configure_stage)"
    assert_single_change_status "$change_out" dpkg 'unchanged|dpkg reported no packages pending configuration'
    run_selected_chroot() { printf 'iU \nii \n'; }
    change_out="$(dpkg_configure_stage)"
    assert_single_change_status "$change_out" dpkg 'changed'

    # fix-broken: a simulation with nothing to do -> unchanged; a real package
    # transaction -> changed.
    run_selected_chroot() { printf 'Reading package lists...\n0 upgraded, 0 newly installed, 0 to remove and 0 not upgraded.\n'; }
    change_out="$(adaptive_fix_broken)"
    assert_single_change_status "$change_out" fixbroken 'unchanged|simulated fix-broken transaction proposed no package changes'
    run_selected_chroot() { printf 'Inst broken-package [1.0] (2.0 example [amd64])\n1 upgraded, 0 newly installed, 0 to remove and 0 not upgraded.\n'; }
    change_out="$(adaptive_fix_broken)"
    assert_single_change_status "$change_out" fixbroken 'changed'

    # A metadata refresh always rewrites package lists and stays changed.
    run_selected_chroot() { printf 'Hit:1 http://example.invalid stable InRelease\nReading package lists...\n'; }
    change_out="$(run_apt_update)"
    assert_single_change_status "$change_out" aptupdate 'changed'

    # Arch pacman: a no-op transaction reports unchanged; real package
    # changes and removals report changed.
    preflight_arch_pacman_transaction() { :; }
    run_chroot_try() {
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=$':: Synchronizing package databases...\n core is up to date\n:: Starting full system upgrade...\n there is nothing to do\n'
    }
    change_out="$(adaptive_arch_pacman_repair 'Upgrade installed Arch packages' upgrade)"
    assert_single_change_status "$change_out" upgrade 'unchanged|pacman transaction reported no packages to install, upgrade or remove'
    run_chroot_try() {
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=$'Packages (1) example-1.0-1 -> 2.0-1\n(1/1) upgrading example\n'
    }
    change_out="$(adaptive_arch_pacman_repair 'Repair Arch package dependencies' fixbroken)"
    assert_single_change_status "$change_out" fixbroken 'changed'
    run_chroot_try() {
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=$'Packages (1) example-1.0-1\n(1/1) removing example\n'
    }
    change_out="$(adaptive_arch_pacman_repair 'Repair Arch package dependencies' fixbroken)"
    assert_single_change_status "$change_out" fixbroken 'changed'

    # DKMS rebuilds stay changed (fail safe).
    preflight_dkms() { :; }
    run_chroot_try() { CHROOT_TRY_RC=0; }
    change_out="$(adaptive_dkms_repair)"
    assert_single_change_status "$change_out" dkms 'changed'

    # GRUB: byte-identical grub.cfg -> unchanged; a regenerated file -> changed.
    mkdir -p "$fake_root/boot/grub"
    printf 'menuentry old\n' > "$fake_root/boot/grub/grub.cfg"
    preflight_grub() { :; }
    guard_grub_candidate_preserves_entries() { return 0; }
    run_chroot_try() { CHROOT_TRY_RC=0; }
    change_out="$(adaptive_grub_repair)"
    assert_single_change_status "$change_out" grub 'unchanged|grub.cfg is byte-identical'
    run_chroot_try() { CHROOT_TRY_RC=0; printf 'menuentry new\n' > "$TARGET_ROOT/boot/grub/grub.cfg"; }
    change_out="$(adaptive_grub_repair)"
    assert_single_change_status "$change_out" grub 'changed'

    # initramfs: byte-identical images -> unchanged; a rebuilt image -> changed.
    TARGET_DISTRO_FAMILY=debian
    TARGET_INITRAMFS_BACKEND=initramfs-tools
    installed_kernel_versions() { printf '6.1-test\n'; }
    mkdir -p "$fake_root/boot"
    printf 'initrd-old\n' > "$fake_root/boot/initrd.img-6.1-test"
    preflight_initramfs() { :; }
    run_chroot_try() { CHROOT_TRY_RC=0; }
    change_out="$(adaptive_initramfs_repair)"
    assert_single_change_status "$change_out" initramfs 'unchanged|rebuilt initramfs images are byte-identical'
    run_chroot_try() { CHROOT_TRY_RC=0; printf 'initrd-new\n' > "$TARGET_ROOT/boot/initrd.img-6.1-test"; }
    change_out="$(adaptive_initramfs_repair)"
    assert_single_change_status "$change_out" initramfs 'changed'

    # display-manager: already-correct offline links -> unchanged; a repaired
    # link -> changed.
    mkdir -p "$fake_root/etc/systemd/system" "$fake_root/usr/lib/systemd/system"
    : > "$fake_root/usr/lib/systemd/system/sddm.service"
    : > "$fake_root/usr/lib/systemd/system/graphical.target"
    ln -sfn /usr/lib/systemd/system/sddm.service "$fake_root/etc/systemd/system/display-manager.service"
    ln -sfn /usr/lib/systemd/system/graphical.target "$fake_root/etc/systemd/system/default.target"
    preflight_display_manager() {
        DISPLAY_MANAGER_SERVICE=sddm.service
        DISPLAY_MANAGER_PACKAGE=sddm
        DISPLAY_MANAGER_LABEL=SDDM
        DISPLAY_MANAGER_UNIT_REL=/usr/lib/systemd/system/sddm.service
        DISPLAY_MANAGER_PACKAGE_WORK=false
    }
    restore_display_manager() { :; }
    change_out="$(adaptive_display_manager_repair)"
    assert_single_change_status "$change_out" display 'unchanged|default.target and display-manager.service were already correct'
    rm -f "$fake_root/etc/systemd/system/display-manager.service"
    restore_display_manager() { ln -sfn /usr/lib/systemd/system/sddm.service "$TARGET_ROOT/etc/systemd/system/display-manager.service"; }
    change_out="$(adaptive_display_manager_repair)"
    assert_single_change_status "$change_out" display 'changed'

    # EFI/UKI: byte-identical artifacts -> unchanged; a rebuilt artifact -> changed.
    mkdir -p "$fake_root/boot/efi/EFI/testos"
    printf 'loader-old\n' > "$fake_root/boot/efi/EFI/testos/grubx64.efi"
    TARGET_ESP_MOUNT=/boot/efi
    is_tuxedo_uki_layout() { return 0; }
    preflight_tuxedo_uki() { :; }
    rebuild_tuxedo_uki() { :; }
    verify_tuxedo_uki_root_binding() { :; }
    change_out="$(reinstall_efi_bootloader)"
    assert_single_change_status "$change_out" efi 'unchanged|EFI boot artifacts and firmware entries are byte-identical'
    rebuild_tuxedo_uki() { printf 'loader-new\n' > "$TARGET_ROOT/boot/efi/EFI/testos/grubx64.efi"; }
    change_out="$(reinstall_efi_bootloader)"
    assert_single_change_status "$change_out" efi 'changed'

    # boot-stack aggregates its components from the artifact fingerprints; its
    # component lines are separate keys, so only the bootstack line is asserted.
    TARGET_DISTRO_FAMILY=debian
    adaptive_initramfs_repair() { repair_change_status initramfs unchanged; }
    adaptive_grub_repair() { repair_change_status grub unchanged; }
    validate_mapper_crypttab() { :; }
    change_out="$(repair_boot_stack)"
    assert_change_status "$change_out" bootstack 'unchanged|initramfs, EFI/UKI and GRUB artifacts are byte-identical'
    rebuild_tuxedo_uki() { printf 'loader-newer\n' > "$TARGET_ROOT/boot/efi/EFI/testos/grubx64.efi"; }
    change_out="$(repair_boot_stack)"
    assert_change_status "$change_out" bootstack 'changed'
)

# ---------------------------------------------------------------------------
# Package-manager feedback: held-back, skipped, ignored, masked and pinned
# packages are surfaced as stable evidence lines plus one summary and appended
# to the change-status reason without failing the stage.  The raw simulate and
# apply blocks stay in the repair log.
# ---------------------------------------------------------------------------
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_DISTRO_FAMILY=debian
    TARGET_ROOT="$fake_root"
    SESSION_LOG="$fake_root/apt-feedback.log"
    : > "$SESSION_LOG"

    # A kept-back package with no other transaction work: the stage stays
    # unchanged and the status names the package.
    run_selected_chroot()
    {
        printf 'Reading package lists...\n'
        printf 'The following packages have been kept back:\n'
        printf '  firmware-mediatek\n'
        printf '0 upgraded, 0 newly installed, 0 to remove and 1 not upgraded.\n'
    }
    apt_feedback_out="$(adaptive_apt_upgrade)"
    grep -Fqx 'Package kept back: firmware-mediatek' <<<"$apt_feedback_out" \
        || { echo 'FAIL: apt kept-back evidence line is missing' >&2; printf '%s\n' "$apt_feedback_out" >&2; exit 1; }
    grep -Fqx 'Package manager feedback: apt/dpkg: 1 package kept back: firmware-mediatek' <<<"$apt_feedback_out" \
        || { echo 'FAIL: apt kept-back summary is missing' >&2; printf '%s\n' "$apt_feedback_out" >&2; exit 1; }
    grep -Fqx 'Repair change status upgrade: unchanged|simulated upgrade transaction proposed no package changes; 1 package kept back: firmware-mediatek' <<<"$apt_feedback_out" \
        || { echo 'FAIL: apt kept-back unchanged status is wrong' >&2; printf '%s\n' "$apt_feedback_out" >&2; exit 1; }
    grep -Fq 'The following packages have been kept back:' "$SESSION_LOG" \
        || { echo 'FAIL: the raw apt kept-back block was not logged' >&2; exit 1; }

    # Other packages upgraded while one stays kept back: the stage is changed
    # and the status still names the held-back package.
    run_selected_chroot()
    {
        printf 'Reading package lists...\n'
        printf 'Inst linux-image [1.0] (2.0 example [amd64])\n'
        printf 'The following packages have been kept back:\n'
        printf '  firmware-mediatek\n'
        printf '1 upgraded, 0 newly installed, 0 to remove and 1 not upgraded.\n'
    }
    apt_feedback_out="$(adaptive_apt_upgrade)"
    grep -Fqx 'Repair change status upgrade: changed|1 package kept back: firmware-mediatek' <<<"$apt_feedback_out" \
        || { echo 'FAIL: apt changed status does not name the kept-back package' >&2; printf '%s\n' "$apt_feedback_out" >&2; exit 1; }
)

# dnf5: a skipped package is non-fatal and surfaced; the changed stage keeps
# its package feedback in the status reason.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_ROOT="$fake_root"
    SESSION_LOG="$fake_root/rpm-feedback.log"
    SESSION_DIR="$fake_root/rpm-feedback-session"
    mkdir -p "$SESSION_DIR"
    : > "$SESSION_LOG"
    rpm_preflight() { :; }
    rpm_database_fingerprint() { printf 'stable\n'; }
    rpm_transaction_try()
    {
        RPM_SIM_RC=1
        RPM_SIM_OUTPUT=$'Skipping packages with broken dependencies:\n broken-pkg\nOperation aborted by the user.'
    }
    run_chroot_try()
    {
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=$'Skipping packages with broken dependencies:\n broken-pkg'
    }
    rpm_feedback_out="$(adaptive_rpm_upgrade)"
    grep -Fqx 'Package skipped: broken-pkg' <<<"$rpm_feedback_out" \
        || { echo 'FAIL: dnf5 skipped evidence line is missing' >&2; printf '%s\n' "$rpm_feedback_out" >&2; exit 1; }
    grep -Fqx 'Package manager feedback: rpm: 1 package skipped: broken-pkg' <<<"$rpm_feedback_out" \
        || { echo 'FAIL: dnf5 skipped summary is missing' >&2; printf '%s\n' "$rpm_feedback_out" >&2; exit 1; }
    grep -Fqx 'Repair change status upgrade: changed|1 package skipped: broken-pkg' <<<"$rpm_feedback_out" \
        || { echo 'FAIL: dnf5 skipped status is wrong' >&2; printf '%s\n' "$rpm_feedback_out" >&2; exit 1; }
)

# pacman: an ignored package upgrade is non-fatal and surfaced in the
# unchanged status reason.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_ROOT="$fake_root"
    SESSION_LOG="$fake_root/pacman-feedback.log"
    : > "$SESSION_LOG"
    preflight_arch_pacman_transaction()
    {
        ARCH_PACMAN_TRY_OUTPUT=$'warning: linux: ignoring package upgrade (6.12.1-1 => 6.12.2-1)'
    }
    run_chroot_try()
    {
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=$'warning: linux: ignoring package upgrade (6.12.1-1 => 6.12.2-1)\n there is nothing to do'
    }
    pacman_feedback_out="$(adaptive_arch_pacman_repair 'Upgrade installed Arch packages' upgrade)"
    grep -Fqx 'Package ignored: linux' <<<"$pacman_feedback_out" \
        || { echo 'FAIL: pacman ignored evidence line is missing' >&2; printf '%s\n' "$pacman_feedback_out" >&2; exit 1; }
    grep -Fqx 'Package manager feedback: pacman: 1 package ignored: linux' <<<"$pacman_feedback_out" \
        || { echo 'FAIL: pacman ignored summary is missing' >&2; printf '%s\n' "$pacman_feedback_out" >&2; exit 1; }
    grep -Fqx 'Repair change status upgrade: unchanged|pacman transaction reported no packages to install, upgrade or remove; 1 package ignored: linux' <<<"$pacman_feedback_out" \
        || { echo 'FAIL: pacman ignored unchanged status is wrong' >&2; printf '%s\n' "$pacman_feedback_out" >&2; exit 1; }
)

# apk: a masked warning and an /etc/apk/world version pin are non-fatal and
# surfaced in the changed status reason.
apk_feedback_root="$(mktemp -d)"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_ROOT="$apk_feedback_root"
    SESSION_LOG="$apk_feedback_root/session.log"
    : > "$SESSION_LOG"
    mkdir -p "$TARGET_ROOT/etc/apk"
    printf 'foo=1.2.3-r0\nbar\n' > "$TARGET_ROOT/etc/apk/world"
    alpine_apk_preflight() { :; }
    alpine_apk_fingerprint() { printf 'stable\n'; }
    alpine_apk_transaction_try()
    {
        APK_SIM_RC=0
        APK_SIM_OUTPUT=$'WARNING: masked-pkg: package is masked\n(1/1) Upgrading foo (1.0-r0 -> 1.1-r0)'
    }
    run_chroot_try() { CHROOT_TRY_RC=0; CHROOT_TRY_OUTPUT=''; }
    apk_feedback_out="$(adaptive_alpine_apk_upgrade)"
    grep -Fqx 'Package masked: masked-pkg' <<<"$apk_feedback_out" \
        || { echo 'FAIL: apk masked evidence line is missing' >&2; printf '%s\n' "$apk_feedback_out" >&2; exit 1; }
    grep -Fqx 'Package pinned: foo' <<<"$apk_feedback_out" \
        || { echo 'FAIL: apk world-pin evidence line is missing' >&2; printf '%s\n' "$apk_feedback_out" >&2; exit 1; }
    grep -Fqx 'Package manager feedback: apk: 1 package masked: masked-pkg; 1 package pinned: foo' <<<"$apk_feedback_out" \
        || { echo 'FAIL: apk feedback summary is missing' >&2; printf '%s\n' "$apk_feedback_out" >&2; exit 1; }
    grep -Fqx 'Repair change status upgrade: changed|1 package masked: masked-pkg; 1 package pinned: foo' <<<"$apk_feedback_out" \
        || { echo 'FAIL: apk changed status does not name the feedback' >&2; printf '%s\n' "$apk_feedback_out" >&2; exit 1; }
)
rm -rf -- "$apk_feedback_root"

# The multi-backend dispatcher names each backend's feedback in the combined
# status while keeping exactly one status line for the stage.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$fake_root/multi-feedback.log"
    SESSION_DIR="$fake_root/multi-feedback-session"
    mkdir -p "$SESSION_DIR"
    : > "$SESSION_LOG"
    TARGET_ROOT="$fake_root"
    TARGET_PACKAGE_MANAGERS=(apt/dpkg apk)
    package_backend_unavailable_reason() { return 0; }
    adaptive_fix_broken()
    {
        package_feedback_add apt/dpkg "1 package kept back: firmware-mediatek"
        repair_change_status fixbroken changed
    }
    adaptive_alpine_apk_fix_broken() { repair_change_status fixbroken "unchanged|apk no-op"; }
    run_package_stage fix-broken fixbroken
) > "$fake_root/multi-feedback.out" 2>&1
grep -Fqx 'Repair change status fixbroken: changed|backends: apt/dpkg changed (1 package kept back: firmware-mediatek); apk unchanged' "$fake_root/multi-feedback.out" \
    || { echo 'FAIL: combined multi-backend status does not name the package feedback' >&2; cat "$fake_root/multi-feedback.out" >&2; exit 1; }

# host-default compares the pre-change NVRAM capture with the final state.
grep -Fq 'cmp -s "$pre" <(efibootmgr -v 2>/dev/null || true)' "$HELPER"
grep -q '^run_host_default()' "$HELPER"

# validate is read-only and always reports unchanged.
validate_target_body="$(sed -n '/^validate_target()/,/^}/p' "$HELPER")"
validate_host_body="$(sed -n '/^validate_running_host()/,/^}/p' "$HELPER")"
grep -Fq 'repair_change_status validate "unchanged|validation is read-only"' <<<"$validate_target_body"
grep -Fq 'repair_change_status validate "unchanged|validation is read-only"' <<<"$validate_host_body"

# A restored firmware entry's ID is captured from the function's stdout, so the
# efibootmgr warning about another ESP carrying the same label must stay on
# stderr, and the captured ID must be validated before it reaches BootOrder.
create_entry_body="$(sed -n '/^efi_create_entry_from_definition()/,/^}/p' "$HELPER")"
grep -Fq -- '--label "$label" --loader "$loader" 2>&1 | tee -a "$SESSION_LOG" >&2' <<<"$create_entry_body" \
    || { echo "FAIL: efibootmgr create output must not leak into the captured Boot#### ID" >&2; exit 1; }
grep -Fq 'refusing to use an unverified firmware ID' <<<"$create_entry_body" \
    || { echo "FAIL: a restored EFI entry ID must be validated before use" >&2; exit 1; }

# The BootOrder restore fails closed with a specific reason when a pre-existing
# entry maps to a polluted/invalid firmware ID (the regression that captured an
# efibootmgr warning as the restored ID), and still restores a valid mapping.
(
    efi_order_root="$(mktemp -d)"
    trap 'rm -rf -- "$efi_order_root"' EXIT
    SESSION_DIR="$efi_order_root"
    SESSION_LOG="$efi_order_root/session.log"
    : > "$SESSION_LOG"
    cat > "$efi_order_root/before.txt" <<'EOF'
BootOrder: 0004,0003
Boot0003* arch	HD(1,GPT,11111111-2222-3333-4444-555555555555,0x800,0x200000)/\EFI\arch\grubx64.efi
Boot0004* arch	HD(1,GPT,66666666-7777-8888-9999-aaaaaaaaaaaa,0x800,0x200000)/\EFI\arch\grubx64.efi
EOF
    printf '0004\tefibootmgr: ** Warning ** : Boot0003 has same label arch\tkey\n' \
        > "$efi_order_root/polluted-map.tsv"
    efibootmgr() {
        case "$1" in
            -v) printf 'BootCurrent: 0002\nBootOrder: 0003,0004\n'
                printf 'Boot0003* arch\tHD(1,GPT,11111111-2222-3333-4444-555555555555,0x800,0x200000)/\\EFI\\arch\\grubx64.efi\n'
                printf 'Boot0004* arch\tHD(1,GPT,66666666-7777-8888-9999-aaaaaaaaaaaa,0x800,0x200000)/\\EFI\\arch\\grubx64.efi\n'
                ;;
            -o) printf '%s\n' "$*" >> "$efi_order_root/order-calls" ;;
        esac
        return 0
    }
    if efi_restore_reconciled_order "$efi_order_root/before.txt" \
            "$efi_order_root/polluted-map.tsv" 2>/dev/null; then
        echo "FAIL: a polluted EFI ID mapping must fail closed" >&2
        exit 1
    fi
    grep -Fq 'mapped to invalid firmware ID' "$SESSION_LOG" \
        || { echo "FAIL: the invalid EFI ID must be named in the failure reason" >&2; exit 1; }
    [[ ! -e "$efi_order_root/order-calls" ]] \
        || { echo "FAIL: a polluted mapping must not change BootOrder" >&2; exit 1; }

    printf '0004\t0004\tkey\n0003\t0003\tkey\n' > "$efi_order_root/valid-map.tsv"
    efi_restore_reconciled_order "$efi_order_root/before.txt" \
        "$efi_order_root/valid-map.tsv" >/dev/null 2>&1 \
        || { echo "FAIL: a valid EFI ID mapping must restore BootOrder" >&2; exit 1; }
    grep -Fqx -- '-o 0004,0003' "$efi_order_root/order-calls" \
        || { echo "FAIL: the reconciled BootOrder must be applied in the pre-state order" >&2; exit 1; }
)

# ---------------------------------------------------------------------------
# Probe-based backend detection: the distribution family is never an
# availability gate.  The fixtures below cover mixed package managers, a
# Debian tree with extlinux under systemd vs OpenRC, multi-backend stage
# semantics and detection-accurate Alpine EFI reasons.
# ---------------------------------------------------------------------------
mixed_root="$(mktemp -d)"
extlinux_debian_root="$(mktemp -d)"
alpine_efi_root="$(mktemp -d)"
rpm_root="$(mktemp -d)"
rpm_preflight_root="$(mktemp -d)"
dracut_pair_root="$(mktemp -d)"
trap 'rm -rf -- "$fake_root" "$dracut_root" "$dracut_pkg_root" "$cap_root" "$cap_arch_min" "$cap_alpine" "$pair_root" "$mixed_root" "$extlinux_debian_root" "$alpine_efi_root" "$rpm_root" "$rpm_preflight_root" "$dracut_pair_root" "$no_log_root"' EXIT

# (a) A Debian-family tree that also carries a usable apk database: both the
# native apt/dpkg backend and the apk backend are detected, advertised and run.
mkdir -p "$mixed_root"/usr/bin "$mixed_root"/var/lib/dpkg "$mixed_root"/etc/apt \
    "$mixed_root"/sbin "$mixed_root"/etc/apk "$mixed_root"/lib/apk/db \
    "$mixed_root"/boot "$mixed_root"/lib/modules/6.1.0-test
for mixed_tool in dpkg apt-get; do
    : > "$mixed_root/usr/bin/$mixed_tool"; chmod +x "$mixed_root/usr/bin/$mixed_tool"
done
printf 'Package: base-files\nStatus: install ok installed\nVersion: 1\n\n' > "$mixed_root/var/lib/dpkg/status"
printf 'deb http://deb.example.invalid/ stable main\n' > "$mixed_root/etc/apt/sources.list"
: > "$mixed_root/sbin/apk"; chmod +x "$mixed_root/sbin/apk"
printf 'http://apk.example.invalid/main\n' > "$mixed_root/etc/apk/repositories"
printf 'alpine-base\n' > "$mixed_root/etc/apk/world"
printf 'P:syslinux\nV:6.04-r0\n\n' > "$mixed_root/lib/apk/db/installed"
printf '%s\n' '-lts' > "$mixed_root/lib/modules/6.1.0-test/kernel-suffix"
: > "$mixed_root/boot/vmlinuz-lts"; : > "$mixed_root/boot/initramfs-lts"

mixed_caps="$(TARGET_ROOT="$mixed_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
    diagnostic_repair_capabilities)"
cap_expect "$mixed_caps" \
    'fixbroken: available' \
    'upgrade: available' \
    'aptupdate: available' \
    'dpkg: available'
grep -Fqx 'Repair capability evidence fixbroken: apt/dpkg: /usr/bin/dpkg and /usr/bin/apt-get present; apk: apk executable present; /etc/apk/repositories present; /lib/apk/db/installed present; /etc/apk/world present' <<<"$mixed_caps" \
    || { echo 'FAIL: mixed-manager fixbroken evidence does not name both backends' >&2; exit 1; }
mixed_backends="$(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_ROOT="$mixed_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian profile_target_backends
    package_stage_backends fix-broken
)"
[[ "$mixed_backends" == $'apt/dpkg\napk' ]] \
    || { echo "FAIL: mixed package backends are not native-first: $mixed_backends" >&2; exit 1; }

mixed_profile="$(TARGET_ROOT="$mixed_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
    TARGET_PRETTY="TUXEDO OS" diagnostic_backend_profile)"
grep -Fqx 'Package manager backends: apt/dpkg, apk' <<<"$mixed_profile" \
    || { echo 'FAIL: backend profile does not list both detected managers' >&2; exit 1; }
grep -Fqx 'Backend note: Debian-family system with apk detected — package stages run every detected backend (apt/dpkg first, then apk).' <<<"$mixed_profile" \
    || { echo 'FAIL: non-native backend note is missing' >&2; exit 1; }

# Both backends run for one stage and the combined status is changed if any
# backend changed; unchanged only when every backend proved unchanged.
mixed_calls="$mixed_root/calls.log"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$mixed_root/session.log"
    SESSION_DIR="$mixed_root/session"
    mkdir -p "$SESSION_DIR"
    : > "$SESSION_LOG"
    : > "$mixed_calls"
    TARGET_ROOT="$mixed_root"
    TARGET_DISTRO_FAMILY=debian
    TARGET_PACKAGE_MANAGERS=(apt/dpkg apk)
    adaptive_fix_broken() { printf 'apt\n' >> "$mixed_calls"; repair_change_status fixbroken "unchanged|apt no-op"; }
    adaptive_alpine_apk_fix_broken() { printf 'apk\n' >> "$mixed_calls"; repair_change_status fixbroken changed; }
    run_package_stage fix-broken fixbroken
) > "$mixed_root/mixed-changed.out"
[[ "$(cat "$mixed_calls")" == $'apt\napk' ]] \
    || { echo 'FAIL: mixed backends did not run in native-first order' >&2; exit 1; }
[[ "$(grep -c '^Repair change status fixbroken: ' "$mixed_root/mixed-changed.out")" -eq 1 ]] \
    || { echo 'FAIL: mixed stage emitted more than one change status line' >&2; exit 1; }
grep -Fqx 'Repair change status fixbroken: changed|backends: apt/dpkg unchanged; apk changed' "$mixed_root/mixed-changed.out" \
    || { echo 'FAIL: mixed changed status is not combined' >&2; cat "$mixed_root/mixed-changed.out" >&2; exit 1; }

(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$mixed_root/session.log"
    SESSION_DIR="$mixed_root/session"
    TARGET_ROOT="$mixed_root"
    TARGET_DISTRO_FAMILY=debian
    TARGET_PACKAGE_MANAGERS=(apt/dpkg apk)
    adaptive_fix_broken() { repair_change_status fixbroken "unchanged|apt no-op"; }
    adaptive_alpine_apk_fix_broken() { repair_change_status fixbroken "unchanged|apk no-op"; }
    run_package_stage fix-broken fixbroken
) > "$mixed_root/mixed-unchanged.out"
grep -Fqx 'Repair change status fixbroken: unchanged|backends: apt/dpkg unchanged; apk unchanged' "$mixed_root/mixed-unchanged.out" \
    || { echo 'FAIL: mixed unchanged status is not combined' >&2; cat "$mixed_root/mixed-unchanged.out" >&2; exit 1; }

# (c) A failing backend aborts the stage and is named in the stage context.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$mixed_root/session.log"
    SESSION_DIR="$mixed_root/session"
    TARGET_ROOT="$mixed_root"
    TARGET_DISTRO_FAMILY=debian
    TARGET_PACKAGE_MANAGERS=(apt/dpkg apk)
    adaptive_fix_broken() { printf 'apt\n' >> "$mixed_calls"; repair_change_status fixbroken unchanged; }
    adaptive_alpine_apk_fix_broken() { fail "simulated apk backend failure"; }
    run_package_stage fix-broken fixbroken
) > "$mixed_root/mixed-fail.out" 2>&1 && { echo 'FAIL: a failing backend did not fail the stage' >&2; exit 1; }
grep -Fq "stage 'fix-broken (apk)' failed: simulated apk backend failure" "$mixed_root/mixed-fail.out" \
    || { echo 'FAIL: failing backend attribution is missing' >&2; cat "$mixed_root/mixed-fail.out" >&2; exit 1; }

# ---------------------------------------------------------------------------
# (e) Fedora/rpm-dnf5 + dracut + GRUB2 BIOS: a complete fixture must advertise
# the guarded rpm package stages (fix-broken/apt-update/upgrade), the dracut
# initramfs rebuild and the GRUB2 configuration/boot-code paths from probe
# evidence, while EFI stays unavailable on the BIOS layout with a probe reason.
# ---------------------------------------------------------------------------
mkdir -p "$rpm_root"/usr/bin "$rpm_root"/usr/sbin "$rpm_root"/usr/lib/sysimage/rpm/pubkeys \
    "$rpm_root"/etc/yum.repos.d "$rpm_root"/etc/kernel "$rpm_root"/etc/default \
    "$rpm_root"/usr/lib/dracut "$rpm_root"/boot/grub2/i386-pc \
    "$rpm_root"/boot/loader/entries \
    "$rpm_root"/lib/modules/6.19.10-300.fc44.x86_64 \
    "$rpm_root"/usr/lib/systemd/system "$rpm_root"/var/log/journal
for rpm_tool in rpm dnf5 dracut lsinitrd grub2-mkconfig grub2-install grub2-script-check grub2-editenv; do
    : > "$rpm_root/usr/bin/$rpm_tool"; chmod +x "$rpm_root/usr/bin/$rpm_tool"
done
printf 'sqlite rpmdb\n' > "$rpm_root/usr/lib/sysimage/rpm/rpmdb.sqlite"
printf 'history\n' > "$rpm_root/usr/lib/sysimage/rpm/history.sqlite"
printf 'key\n' > "$rpm_root/usr/lib/sysimage/rpm/pubkeys/RPM-GPG-KEY-fedora-44"
cat > "$rpm_root/etc/yum.repos.d/fedora.repo" <<'REPO'
[fedora]
name=Fedora
enabled=1

[disabled]
name=Disabled
enabled=0
REPO
: > "$rpm_root/boot/vmlinuz-6.19.10-300.fc44.x86_64"
printf 'initramfs\n' > "$rpm_root/boot/initramfs-6.19.10-300.fc44.x86_64.img"
: > "$rpm_root/usr/lib/systemd/system/graphical.target"
: > "$rpm_root/usr/lib/systemd/system/gdm.service"
# Fedora GRUB2/BLS layout: grub.cfg with blscfg, a valid 1024-byte grubenv,
# machine-id-named BLS entries and the i386-pc module set.
printf 'insmod blscfg\nblscfg\n' > "$rpm_root/boot/grub2/grub.cfg"
{
    printf '# GRUB Environment Block\n'
    printf 'saved_entry=cfe2564d1eaf4bd88faaff5dd35b5e31-6.19.10-300.fc44.x86_64\n'
    printf 'blsdir=/boot/loader/entries\n'
    printf 'menu_auto_hide=1\n'
    printf 'boot_success=1\n'
} > "$rpm_root/boot/grub2/grubenv"
truncate -s 1024 "$rpm_root/boot/grub2/grubenv"
printf 'cfe2564d1eaf4bd88faaff5dd35b5e31\n' > "$rpm_root/etc/machine-id"
printf 'GRUB_TIMEOUT=5\nGRUB_ENABLE_BLSCFG=true\n' > "$rpm_root/etc/default/grub"
printf 'title Fedora Linux\nversion 6.19.10-300.fc44.x86_64\nlinux /vmlinuz-6.19.10-300.fc44.x86_64\ninitrd /initramfs-6.19.10-300.fc44.x86_64.img\n' \
    > "$rpm_root/boot/loader/entries/cfe2564d1eaf4bd88faaff5dd35b5e31-6.19.10-300.fc44.x86_64.conf"
printf 'loading\nGeom\n' > "$rpm_root/boot/grub2/i386-pc/boot.img"
printf 'loading\nGeom\n' > "$rpm_root/boot/grub2/i386-pc/core.img"

# The Fedora fixture pins the firmware probe to legacy BIOS so the capability
# reasons are deterministic on a UEFI test host; the probe itself is covered by
# bios_firmware_mode's /sys/firmware/efi evidence.
rpm_probe()
{
    (
        bios_firmware_mode() { return 0; }
        filesystem_scope_resolve() { :; }
        filesystem_scope_tools() { :; }
        TARGET_ROOT="$rpm_root" TARGET_OS_ID=fedora TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=fedora \
            TARGET_INITRAMFS_BACKEND=dracut TARGET_BOOTLOADER_BACKEND=grub \
            diagnostic_repair_capabilities
    )
}

rpm_caps="$(rpm_probe)"
cap_expect "$rpm_caps" \
    'validate: available' \
    'dpkg: unavailable|Fedora uses rpm/dnf; dpkg configuration is not available' \
    'fixbroken: available' \
    'aptupdate: available' \
    'upgrade: available' \
    'dkms: unavailable|DKMS is not installed in the target' \
    'display: available' \
    'initramfs: available' \
    'efi: unavailable|legacy BIOS target; no EFI boot path is available' \
    'grub: available' \
    'extlinux: unavailable|update-extlinux is not installed in the target' \
    'bootstack: available'
grep -Fqx 'Repair capability evidence dpkg: /usr/bin/dpkg missing' <<<"$rpm_caps" \
    || { echo 'FAIL: Fedora dpkg evidence changed' >&2; exit 1; }
grep -Fqx 'Repair capability evidence fixbroken: rpm and dnf5 present; sqlite RPM database present; enabled dnf repositories: 1' <<<"$rpm_caps" \
    || { echo 'FAIL: Fedora fix-broken evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence aptupdate: dnf5 present; enabled dnf repositories: 1' <<<"$rpm_caps" \
    || { echo 'FAIL: Fedora aptupdate evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence upgrade: rpm and dnf5 present; sqlite RPM database present; enabled dnf repositories: 1' <<<"$rpm_caps" \
    || { echo 'FAIL: Fedora upgrade evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence initramfs: initramfs backend: dracut; dracut executable and /usr/lib/dracut present; kernels: 6.19.10-300.fc44.x86_64; images: initramfs-6.19.10-300.fc44.x86_64.img' <<<"$rpm_caps" \
    || { echo 'FAIL: Fedora dracut evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence display: graphical.target present; display manager unit gdm.service' <<<"$rpm_caps" \
    || { echo 'FAIL: Fedora GDM evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence efi: legacy BIOS target; no EFI boot path is available' <<<"$rpm_caps" \
    || { echo 'FAIL: Fedora BIOS EFI evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence grub: grub2-mkconfig present; /boot/grub2/grub.cfg present; grubenv present; BIOS boot partition missing' <<<"$rpm_caps" \
    || { echo 'FAIL: Fedora GRUB2 evidence is missing' >&2; exit 1; }
grep -Fqx 'Repair capability evidence bootstack: initramfs backend: dracut; GRUB2 BIOS prerequisites available; no EFI/UKI artifacts' <<<"$rpm_caps" \
    || { echo 'FAIL: Fedora boot-stack evidence is missing' >&2; exit 1; }

rpm_profile="$(TARGET_ROOT="$rpm_root" TARGET_OS_ID=fedora TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=fedora \
    TARGET_PRETTY="Fedora Linux 44" diagnostic_backend_profile)"
for rpm_profile_line in \
    'Distribution family: fedora' \
    'Package manager backend: rpm' \
    'Package manager backends: rpm' \
    'Bootloader backend: grub' \
    'GRUB tools: grub2-mkconfig present; /boot/grub2/grub.cfg present; grubenv present; BIOS boot partition missing' \
    'Initramfs backend: dracut' \
    'Initramfs backends: dracut' \
    'Fedora policy: guarded rpm/dnf5 package transactions and dracut initramfs rebuilds are selected when their stage-specific preflights pass.' \
    'Repair capability: Fedora profile — guarded rpm/dnf5/dracut repairs when their stage-specific preflights pass'; do
    grep -Fqx "$rpm_profile_line" <<<"$rpm_profile" \
        || { echo "FAIL: Fedora backend profile line missing: $rpm_profile_line" >&2; exit 1; }
done

# A stale ESP on a legacy BIOS target must be described as informational: the
# EFI fallback binary and the ESP section are not the active boot path there.
mkdir -p "$rpm_root/boot/efi/EFI/BOOT"
printf 'efi\n' > "$rpm_root/boot/efi/EFI/BOOT/BOOTX64.EFI"
(
    TARGET_ROOT="$rpm_root"
    TARGET_OS_ID=fedora
    TARGET_DISTRO_FAMILY=fedora
    TARGET_BOOTLOADER_BACKEND=grub
    bios_firmware_mode() { return 0; }
    diagnostic_grub
) > "$rpm_root/bios-grub-diag.out" 2>&1
grep -Fq 'EFI fallback binary (present, not the active boot path for this legacy BIOS boot):' "$rpm_root/bios-grub-diag.out" \
    || { echo 'FAIL: the BIOS EFI fallback wording is missing' >&2; cat "$rpm_root/bios-grub-diag.out" >&2; exit 1; }
(
    TARGET_ROOT="$rpm_root"
    TARGET_OS_ID=fedora
    TARGET_DISTRO_FAMILY=fedora
    TARGET_BOOTLOADER_BACKEND=grub
    bios_firmware_mode() { return 0; }
    diagnostic_uki
) > "$rpm_root/bios-uki-diag.out" 2>&1
grep -Fq 'Selected target EFI System Partition (informational only: this legacy BIOS boot has no active EFI boot path):' "$rpm_root/bios-uki-diag.out" \
    || { echo 'FAIL: the BIOS ESP wording is missing' >&2; cat "$rpm_root/bios-uki-diag.out" >&2; exit 1; }

# Fedora's /boot/loader is the GRUB2 BLS directory, never systemd-boot, and
# the resolved GRUB path/grubenv drive diagnostics and the artifact fingerprint.
(
    TARGET_ROOT="$rpm_root"
    TARGET_OS_ID=fedora
    TARGET_OS_LIKE=""
    TARGET_DISTRO_FAMILY=fedora
    profile_target_backends
    [[ "$TARGET_BOOTLOADER_BACKEND" == grub ]] \
        || { echo "FAIL: Fedora BLS tree was misclassified as $TARGET_BOOTLOADER_BACKEND" >&2; exit 1; }
    [[ "$(grub_config_path)" == /boot/grub2/grub.cfg ]] \
        || { echo 'FAIL: Fedora GRUB configuration path was not resolved' >&2; exit 1; }
    [[ "$(grub_env_path)" == /boot/grub2/grubenv ]] \
        || { echo 'FAIL: Fedora grubenv path was not resolved' >&2; exit 1; }
    [[ "$(grub_generator_tool)" == /usr/bin/grub2-mkconfig ]] \
        || { echo 'FAIL: Fedora grub2-mkconfig was not resolved' >&2; exit 1; }
    grub_env_block_valid \
        || { echo 'FAIL: the Fedora grubenv fixture was rejected' >&2; exit 1; }
    fedora_grub_fingerprint="$(grub_artifact_fingerprint)"
    grep -Fq "/boot/grub2/grub.cfg $(repair_file_fingerprint "$rpm_root/boot/grub2/grub.cfg")" <<<"$fedora_grub_fingerprint" \
        || { echo 'FAIL: the Fedora GRUB artifact fingerprint omits grub.cfg' >&2; exit 1; }
    grep -Fq "/boot/grub2/grubenv $(repair_file_fingerprint "$rpm_root/boot/grub2/grubenv")" <<<"$fedora_grub_fingerprint" \
        || { echo 'FAIL: the Fedora GRUB artifact fingerprint omits grubenv' >&2; exit 1; }
    grep -Fq '/boot/loader/entries/cfe2564d1eaf4bd88faaff5dd35b5e31-6.19.10-300.fc44.x86_64.conf' <<<"$fedora_grub_fingerprint" \
        || { echo 'FAIL: the Fedora GRUB artifact fingerprint omits BLS entries' >&2; exit 1; }
    bls_keys="$(fedora_bls_entry_keys)"
    grep -Fq $'cfe2564d1eaf4bd88faaff5dd35b5e31-6.19.10-300.fc44.x86_64.conf\t6.19.10-300.fc44.x86_64\t/vmlinuz-6.19.10-300.fc44.x86_64\t/initramfs-6.19.10-300.fc44.x86_64.img' <<<"$bls_keys" \
        || { echo 'FAIL: the Fedora BLS entry identity is wrong' >&2; exit 1; }
)
fedora_grub_diag="$(
    TARGET_ROOT="$rpm_root" TARGET_OS_ID=fedora TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=fedora \
        diagnostic_grub
)"
grep -Fqx "GRUB configuration: $rpm_root/boot/grub2/grub.cfg" <<<"$fedora_grub_diag" \
    || { echo 'FAIL: Fedora diagnostic_grub did not resolve /boot/grub2/grub.cfg' >&2; exit 1; }
grep -Fq 'GRUB environment: ' <<<"$fedora_grub_diag" \
    || { echo 'FAIL: Fedora diagnostic_grub did not resolve grubenv' >&2; exit 1; }

# A missing/invalid grubenv must fail the GRUB2 capability closed, and a
# Fedora BIOS tree with an invalid grubenv keeps a probe-based reason.
cp -a "$rpm_root/boot/grub2/grubenv" "$rpm_root/boot/grub2/grubenv.valid"
truncate -s 100 "$rpm_root/boot/grub2/grubenv"
cap_expect "$(rpm_probe)" \
    'grub: unavailable|grubenv is missing or not a valid GRUB environment block' \
    'bootstack: unavailable|Requires available initramfs and GRUB repair prerequisites'
mv "$rpm_root/boot/grub2/grubenv.valid" "$rpm_root/boot/grub2/grubenv"
rm -f "$rpm_root/boot/grub2/grub.cfg"
cap_expect "$(rpm_probe)" 'grub: unavailable|/boot/grub2/grub.cfg is missing'
printf 'insmod blscfg\nblscfg\n' > "$rpm_root/boot/grub2/grub.cfg"

# The rpm probe and the native-first ordering are evidence-based.
( TARGET_ROOT="$rpm_root" TARGET_OS_ID=fedora TARGET_DISTRO_FAMILY=fedora target_rpm_ready ) \
    || { echo 'FAIL: complete Fedora fixture was not rpm-ready' >&2; exit 1; }
( TARGET_ROOT="$rpm_root" TARGET_OS_ID=fedora TARGET_DISTRO_FAMILY=fedora target_dnf5_ready ) \
    || { echo 'FAIL: complete Fedora fixture was not dnf5-ready' >&2; exit 1; }
rpm_stage_backends="$(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_ROOT="$rpm_root" TARGET_OS_ID=fedora TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=fedora profile_target_backends
    package_stage_backends fix-broken
)"
[[ "$rpm_stage_backends" == 'rpm' ]] \
    || { echo "FAIL: Fedora fix-broken backends are not native-first: $rpm_stage_backends" >&2; exit 1; }
[[ "$(TARGET_ROOT="$rpm_root" TARGET_OS_ID=fedora TARGET_DISTRO_FAMILY=fedora rpm_enabled_repo_count)" == '1' ]] \
    || { echo 'FAIL: disabled dnf repository was counted as enabled' >&2; exit 1; }
( TARGET_ROOT="$rpm_root" TARGET_OS_ID=fedora TARGET_DISTRO_FAMILY=fedora rpm_database_present ) \
    || { echo 'FAIL: sqlite rpmdb was not detected' >&2; exit 1; }
[[ "$(TARGET_ROOT="$rpm_root" TARGET_OS_ID=fedora TARGET_DISTRO_FAMILY=fedora rpm_dnf_tool)" == 'dnf5' ]] \
    || { echo 'FAIL: dnf5 was not selected as the guarded command' >&2; exit 1; }

# A held rpm/dnf5 WRITE lock is evidence for read-only diagnostics, never a
# refusal; the modifying preflight owns the fail-closed lock gate.
rpm_locked_evidence="$(
    rpm_lock_held() { return 0; }
    TARGET_ROOT="$rpm_root"
    TARGET_OS_ID=fedora
    TARGET_DISTRO_FAMILY=fedora
    TARGET_PACKAGE_MANAGERS=()
    profile_target_backends
    repair_capability_evidence fixbroken
)"
[[ "$rpm_locked_evidence" == *'; RPM database lock is held by another process' ]] \
    || { echo "FAIL: held rpm lock was not reported as capability evidence: $rpm_locked_evidence" >&2; exit 1; }
grep -Fq 'lslocks -n -o MODE,PATH' "$HELPER" \
    || { echo 'FAIL: rpm lock probing does not use lslocks' >&2; exit 1; }
if grep -Eq 'flock -n.*rpm|rpm.*flock -n' "$HELPER"; then
    echo 'FAIL: rpm fcntl locks must not be probed with flock' >&2
    exit 1
fi

# Missing rpm/dnf5 prerequisites fail closed with probe-specific reasons.
mv "$rpm_root/usr/lib/sysimage/rpm/rpmdb.sqlite" "$rpm_root/usr/lib/sysimage/rpm/rpmdb.sqlite.disabled"
cap_expect "$(rpm_probe)" \
    'fixbroken: unavailable|the target RPM database is missing' \
    'upgrade: unavailable|the target RPM database is missing'
mv "$rpm_root/usr/lib/sysimage/rpm/rpmdb.sqlite.disabled" "$rpm_root/usr/lib/sysimage/rpm/rpmdb.sqlite"

mv "$rpm_root/etc/yum.repos.d/fedora.repo" "$rpm_root/etc/yum.repos.d/fedora.repo.disabled"
printf '[disabled]\nenabled=0\n' > "$rpm_root/etc/yum.repos.d/disabled.repo"
cap_expect "$(rpm_probe)" \
    'fixbroken: unavailable|The target has no enabled dnf repositories' \
    'aptupdate: unavailable|The target has no enabled dnf repositories to refresh' \
    'upgrade: unavailable|The target has no enabled dnf repositories'
rm -f "$rpm_root/etc/yum.repos.d/disabled.repo"
mv "$rpm_root/etc/yum.repos.d/fedora.repo.disabled" "$rpm_root/etc/yum.repos.d/fedora.repo"

mv "$rpm_root/usr/bin/dnf5" "$rpm_root/usr/bin/dnf5.disabled"
: > "$rpm_root/usr/bin/dnf4"; chmod +x "$rpm_root/usr/bin/dnf4"
cap_expect "$(rpm_probe)" \
    'fixbroken: unavailable|dnf4 is not supported by the guarded rpm backend' \
    'upgrade: unavailable|dnf4 is not supported by the guarded rpm backend'
grep -Fqx 'Repair capability evidence fixbroken: rpm executable present; dnf5 missing (dnf4 is not supported)' <<<"$(rpm_probe)" \
    || { echo 'FAIL: dnf4 evidence is missing' >&2; exit 1; }
rm -f "$rpm_root/usr/bin/dnf4"
cap_expect "$(rpm_probe)" \
    'fixbroken: unavailable|dnf5 is not installed in the target system' \
    'upgrade: unavailable|dnf5 is not installed in the target system'
mv "$rpm_root/usr/bin/dnf5.disabled" "$rpm_root/usr/bin/dnf5"

# Missing dracut prerequisites fail closed with the exact probe reason.
mv "$rpm_root/usr/bin/lsinitrd" "$rpm_root/usr/bin/lsinitrd.disabled"
cap_expect "$(rpm_probe)" \
    'initramfs: unavailable|lsinitrd is not installed in the target system; dracut image verification is unavailable'
mv "$rpm_root/usr/bin/lsinitrd.disabled" "$rpm_root/usr/bin/lsinitrd"
mv "$rpm_root/usr/lib/dracut" "$rpm_root/usr/lib/dracut.disabled"
cap_expect "$(rpm_probe)" 'initramfs: unavailable|the dracut generator directory is missing from the target'
mv "$rpm_root/usr/lib/dracut.disabled" "$rpm_root/usr/lib/dracut"
mv "$rpm_root/boot/vmlinuz-6.19.10-300.fc44.x86_64" "$rpm_root/boot/vmlinuz.disabled"
cap_expect "$(rpm_probe)" 'initramfs: unavailable|No installed dracut kernels were found under target /boot'
mv "$rpm_root/boot/vmlinuz.disabled" "$rpm_root/boot/vmlinuz-6.19.10-300.fc44.x86_64"

# ---------------------------------------------------------------------------
# Fedora rpm/dnf5 simulation policy: the safety parser must fail closed on
# removals, downgrades, untrusted signatures, held locks, unresolved
# transactions, incomplete metadata, unknown errors, critical obsoletes and an
# oversized transaction, while accepting the write-free --assumeno abort, a
# no-op simulation, normal same-name version upgrades of critical packages
# (systemd, glibc, dnf5, grub2, dracut, btrfs-progs) and skipped packages,
# which stay non-fatal feedback surfaced by the backend.
# ---------------------------------------------------------------------------
rpm_policy_log="$fake_root/rpm-policy.log"
SESSION_LOG="$rpm_policy_log"
: > "$rpm_policy_log"

rpm_simulation_is_safe $'Nothing to do.' 0 \
    || { echo 'FAIL: dnf5 no-op simulation rejected' >&2; exit 1; }
rpm_simulation_is_safe $'Transaction Summary:\n Installing: 2 packages\nOperation aborted by the user.' 1 \
    || { echo 'FAIL: dnf5 --assumeno simulation rejected' >&2; exit 1; }
rpm_simulation_is_safe $'Transaction Summary:\n Upgrading: 1 package\n   replacing foo-2.0-1\nOperation aborted by the user.' 1 \
    || { echo 'FAIL: non-critical dnf5 obsoletes rejected' >&2; exit 1; }
rpm_simulation_is_safe $'Transaction Summary:\n Reinstalling: 1 package\n   replacing foo-1.0-1\nOperation aborted by the user.' 1 foo \
    || { echo 'FAIL: same-name dnf5 reinstall replacement rejected' >&2; exit 1; }

# Defect regression: a same-name version upgrade of a critical package is a
# normal Fedora update and must be accepted, in the aligned dnf5 5.4.1 table
# format (package row followed by the indented replacing sub-line).  The one
# different-name replacement in the set (libheif) is a non-critical obsoletes.
rpm_simulation_is_safe $'Upgrading:\n systemd                                               x86_64 0:258-1.fc44                         updates                           12.0 MiB\n   replacing systemd                                   x86_64 0:257-1.fc44                         19278be6a81040f5b6cbc7bacea5148e  12.0 MiB\n glibc                                                 x86_64 0:2.43-9.fc44                        updates                            6.9 MiB\n   replacing glibc                                     x86_64 0:2.43-8.fc44                        19278be6a81040f5b6cbc7bacea5148e   6.9 MiB\n dnf5                                                  x86_64 0:5.4.5.0-1.fc44                      updates                            3.4 MiB\n   replacing dnf5                                      x86_64 0:5.4.1.0-1.fc44                      19278be6a81040f5b6cbc7bacea5148e   3.1 MiB\n grub2-tools                                           x86_64 1:2.12-30.fc44                       updates                            2.0 MiB\n   replacing grub2-tools                               x86_64 1:2.12-29.fc44                       19278be6a81040f5b6cbc7bacea5148e   2.0 MiB\n btrfs-progs                                           x86_64 0:7.1-1.fc44                          updates                            6.5 MiB\n   replacing btrfs-progs                               x86_64 0:6.19.1-1.fc44                       19278be6a81040f5b6cbc7bacea5148e   6.4 MiB\nInstalling dependencies:\n libheif-ffmpeg                                        x86_64 0:1.23.4-6.fc44                       updates                           32.0 KiB\n   replacing libheif                                   x86_64 0:1.21.2-1.fc44                       19278be6a81040f5b6cbc7bacea5148e   1.8 MiB\nTransaction Summary:\n Installing:         1 package\n Upgrading:          5 packages\n Replacing:          6 packages\nOperation aborted by the user.' 1 \
    || { echo 'FAIL: same-name critical dnf5 upgrades were rejected' >&2; exit 1; }

# The observed normal Fedora update set (21 install + 774 upgrade + 776
# replacing) stays under the cap: Replacing counts the old versions superseded
# by the incoming rows and must not be added to the transaction total.
rpm_cap_observed="$(for i in $(seq 1 21); do printf ' Installing: 1 package\n'; done; for i in $(seq 1 774); do printf ' Upgrading: 1 package\n'; done; printf ' Replacing: 776 packages\n')"
rpm_simulation_is_safe "$rpm_cap_observed" 0 \
    || { echo 'FAIL: normal Fedora update set was rejected by the cap' >&2; exit 1; }
rpm_replacing_cap="$(for i in $(seq 1 1001); do printf ' Replacing: 1 package\n'; done)"
rpm_simulation_is_safe "$rpm_replacing_cap" 0 \
    && { echo 'FAIL: dnf5 replacing-count cap overflow accepted' >&2; exit 1; }
grep -Fq 'would replace 1001 packages (safety limit: 1000)' "$rpm_policy_log" \
    || { echo 'FAIL: dnf5 replacing cap refusal was not logged' >&2; exit 1; }

# Skipped packages are non-fatal: the transaction continues for every other
# package and the backend surfaces the skip as package-manager feedback.
rpm_simulation_is_safe $'Skipping packages with conflicts:\n foo\nOperation aborted by the user.' 1 \
    || { echo 'FAIL: a skipped conflicted package was treated as a failure' >&2; exit 1; }
rpm_simulation_is_safe $'Skipping packages with broken dependencies:\n foo\nOperation aborted by the user.' 1 \
    || { echo 'FAIL: a skipped dependency-broken package was treated as a failure' >&2; exit 1; }
rpm_simulation_is_safe $'Transaction Summary:\n Installing: 1 package\n Skipping: 1 package\nOperation aborted by the user.' 1 \
    || { echo 'FAIL: a dnf5 Skipping summary count was treated as a failure' >&2; exit 1; }

for rpm_unsafe in \
    $'Transaction Summary:\n Removing: 2 packages\nOperation aborted by the user.' \
    $'Removing:\n foo\nOperation aborted by the user.' \
    $'Removing dependent packages:\n foo\nOperation aborted by the user.' \
    $'Downgrading:\n foo\nOperation aborted by the user.' \
    $'Public key is not installed.\nOperation aborted by the user.' \
    $'signature is valid, but the key is not trusted.\nOperation aborted by the user.' \
    $'Failed to import OpenPGP keys\nOperation aborted by the user.' \
    $'Waiting for a lock on the system repository\nOperation aborted by the user.' \
    $'Failed to obtain rpm transaction lock\nOperation aborted by the user.' \
    $'Failed to resolve the transaction:\n Problem: nothing provides bar\nOperation aborted by the user.' \
    $'conflicting requests\nOperation aborted by the user.' \
    $'Failed to download metadata\nOperation aborted by the user.' \
    $'repomd.xml signature check failed\nOperation aborted by the user.' \
    $'Error: unknown failure\nOperation aborted by the user.' \
    $'Transaction failed: scriptlet failed\nOperation aborted by the user.'; do
    if rpm_simulation_is_safe "$rpm_unsafe" 1; then
        echo "FAIL: unsafe dnf5 simulation accepted: $rpm_unsafe" >&2
        exit 1
    fi
done
rpm_simulation_is_safe $'Nothing to do.' 1 \
    && { echo 'FAIL: dnf5 rc 1 without the --assumeno abort token accepted' >&2; exit 1; }
rpm_simulation_is_safe $'Nothing to do.' 2 \
    && { echo 'FAIL: dnf5 rc 2 accepted' >&2; exit 1; }
# A different-name replacement is an obsoletes removal; it may refuse a
# critical package even in the upgrade mode, while the same-name kernel
# upgrade above stays accepted.
rpm_simulation_is_safe $'Installing:\n systemd-boot\n   replacing systemd-resolved\nOperation aborted by the user.' 1 \
    && { echo 'FAIL: dnf5 upgrade obsoleting a critical package accepted' >&2; exit 1; }
rpm_simulation_is_safe $'Installing:\n kernel-uki\n   replacing kernel-core-6.19\nOperation aborted by the user.' 1 \
    && { echo 'FAIL: dnf5 upgrade obsoleting a critical kernel package accepted' >&2; exit 1; }
rpm_simulation_is_safe $'Transaction Summary:\n Reinstalling: 1 package\n   replacing bar-1.0\nOperation aborted by the user.' 1 foo \
    && { echo 'FAIL: dnf5 reinstall replacement outside the reinstalled set accepted' >&2; exit 1; }
rpm_cap_fixture="$(for i in $(seq 1 1001); do printf ' Installing: 1 package\n'; done)"
rpm_simulation_is_safe "$rpm_cap_fixture" 0 \
    && { echo 'FAIL: dnf5 package-count cap overflow accepted' >&2; exit 1; }
grep -Fq 'safety limit: 1000' "$rpm_policy_log" \
    || { echo 'FAIL: dnf5 cap refusal was not logged' >&2; exit 1; }

# A cold target's first rpmdb access creates/truncates the sqlite -shm/-wal
# sidecars; they must stay out of the byte fingerprint or the first
# simulation fails closed as a false "modified the rpm database".
if grep -Fq 'rpmdb.sqlite-wal' "$HELPER" || grep -Fq 'rpmdb.sqlite-shm' "$HELPER"; then
    echo 'FAIL: rpm fingerprint still hashes the transient sqlite sidecars' >&2
    exit 1
fi

rpm_transaction_reported_no_changes $'Nothing to do.' \
    || { echo 'FAIL: dnf5 no-op simulation was not recognized' >&2; exit 1; }
rpm_transaction_reported_no_changes $'Installing:\n foo\nNothing to do.' \
    && { echo 'FAIL: dnf5 install action was treated as unchanged' >&2; exit 1; }
rpm_transaction_reported_no_changes $'Transaction Summary:\n Installing: 1 package' \
    && { echo 'FAIL: dnf5 summary action was treated as unchanged' >&2; exit 1; }

# The guarded rpm backend must wire the write-free --assumeno simulation, the
# exact simulated apply and never pass a policy-relaxing dnf5 flag.
grep -Fq 'rpm_transaction_try "dnf5 ${rpm_command[*]} simulation" "${rpm_command[@]}" --assumeno' "$HELPER" \
    || { echo 'FAIL: dnf5 write-free simulation is not wired' >&2; exit 1; }
grep -Fq 'run_chroot_try "$label" "${RPM_DNF_TOOL:-dnf5}" "${rpm_command[@]}" -y' "$HELPER" \
    || { echo 'FAIL: dnf5 apply is not wired' >&2; exit 1; }
grep -Fq 'adaptive_rpm_apply "Repair RPM package files (dnf5 reinstall ${missing_packages[*]})" fixbroken reinstall "${missing_packages[@]}"' "$HELPER" \
    || { echo 'FAIL: rpm fix-broken reinstall is not wired' >&2; exit 1; }
grep -Fq 'adaptive_rpm_stage "Upgrade installed RPM packages (dnf5 upgrade)" upgrade upgrade' "$HELPER" \
    || { echo 'FAIL: rpm upgrade stage is not wired' >&2; exit 1; }
grep -Fq 'Refresh dnf5 package metadata (dnf5 makecache)' "$HELPER" \
    || { echo 'FAIL: dnf5 makecache stage is not wired' >&2; exit 1; }

# dnf5 prints "Metadata cache created." even when the repository metadata cache
# is byte-identical, so the metadata/solv fingerprint is the accurate change
# signal: identical -> unchanged, rewritten -> changed with explicit evidence,
# no cache -> changed (a no-op cannot be proven).
rpm_cache_root="$(mktemp -d)"
mkdir -p "$rpm_cache_root/var/cache/libdnf5/fedora-abc/repodata" \
    "$rpm_cache_root/var/cache/libdnf5/fedora-abc/solv" \
    "$rpm_cache_root/var/cache/libdnf5/fedora-abc/packages"
printf 'repomd\n' > "$rpm_cache_root/var/cache/libdnf5/fedora-abc/repodata/repomd.xml"
printf 'solv\n' > "$rpm_cache_root/var/cache/libdnf5/fedora-abc/solv/fedora.solv"
printf 'payload\n' > "$rpm_cache_root/var/cache/libdnf5/fedora-abc/packages/foo.rpm"
rpm_cache_fp="$(TARGET_ROOT="$rpm_cache_root" rpm_metadata_cache_fingerprint)"
[[ "$rpm_cache_fp" != "cache missing" && "$rpm_cache_fp" != "no-sha256sum" ]] \
    || { echo 'FAIL: dnf metadata cache fingerprint was not computed' >&2; exit 1; }
grep -Fq 'repodata/repomd.xml' <<<"$rpm_cache_fp" \
    || { echo 'FAIL: dnf repodata is not fingerprinted' >&2; exit 1; }
if grep -Fq 'packages/foo.rpm' <<<"$rpm_cache_fp"; then
    echo 'FAIL: dnf package payloads must stay out of the metadata fingerprint' >&2
    exit 1
fi
(
    TARGET_ROOT="$rpm_cache_root"
    SESSION_LOG="$rpm_cache_root/session.log"
    : > "$SESSION_LOG"
    rpm_preflight() { :; }
    run_chroot_try() { CHROOT_TRY_RC=0; CHROOT_TRY_OUTPUT='Metadata cache created.'; }
    adaptive_rpm_metadata_refresh
) > "$rpm_cache_root/unchanged.out" 2>&1
grep -Fqx 'Repair change status aptupdate: unchanged|dnf5 repository metadata cache is byte-identical' "$rpm_cache_root/unchanged.out" \
    || { echo 'FAIL: byte-identical dnf metadata cache was not reported unchanged' >&2; cat "$rpm_cache_root/unchanged.out" >&2; exit 1; }
(
    TARGET_ROOT="$rpm_cache_root"
    SESSION_LOG="$rpm_cache_root/session.log"
    rpm_preflight() { :; }
    run_chroot_try()
    {
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT='Metadata cache created.'
        printf 'rewritten\n' > "$TARGET_ROOT/var/cache/libdnf5/fedora-abc/repodata/repomd.xml"
    }
    adaptive_rpm_metadata_refresh
) > "$rpm_cache_root/changed.out" 2>&1
grep -Fqx 'Repair change status aptupdate: changed' "$rpm_cache_root/changed.out" \
    || { echo 'FAIL: a rewritten dnf metadata cache did not report changed' >&2; cat "$rpm_cache_root/changed.out" >&2; exit 1; }
grep -Fq 'dnf5 repository metadata cache rewritten (cache fingerprint changed)' "$rpm_cache_root/changed.out" \
    || { echo 'FAIL: dnf changed evidence wording is missing' >&2; cat "$rpm_cache_root/changed.out" >&2; exit 1; }
(
    TARGET_ROOT="$rpm_cache_root"
    SESSION_LOG="$rpm_cache_root/session.log"
    rpm_preflight() { :; }
    run_chroot_try() { CHROOT_TRY_RC=0; CHROOT_TRY_OUTPUT='Metadata cache created.'; }
    rm -rf -- "$TARGET_ROOT/var/cache"
    adaptive_rpm_metadata_refresh
) > "$rpm_cache_root/missing.out" 2>&1
grep -Fqx 'Repair change status aptupdate: changed' "$rpm_cache_root/missing.out" \
    || { echo 'FAIL: a missing dnf metadata cache did not report changed' >&2; cat "$rpm_cache_root/missing.out" >&2; exit 1; }
rm -rf -- "$rpm_cache_root"

grep -Fq 'rpm -Va --nofiledigest' "$HELPER" \
    || { echo 'FAIL: rpm missing-file verification is not wired' >&2; exit 1; }
grep -Fq 'rpm -qf --qf' "$HELPER" \
    || { echo 'FAIL: rpm file ownership mapping is not wired' >&2; exit 1; }
if grep -En 'dnf5.*(--allowerasing|--skip-broken|--skip-unavailable|--nogpgcheck|--no-gpgchecks|--nodeps|--noscripts|--downloadonly|--refresh|tsflags=test)' "$HELPER"; then
    echo 'FAIL: guarded rpm backend passes a policy-relaxing dnf5 flag' >&2
    exit 1
fi

# Runtime directories under /run, /var/run and /var/lock are never repair
# input: the repair chroot mounts a fresh tmpfs on /run, so those rpmdb-owned
# runtime dirs are always absent there and would make fix-broken non-idempotent.
# Real files, config files and /boot entries stay in the missing-file set.
(
    run_chroot_try()
    {
        CHROOT_TRY_RC=1
        CHROOT_TRY_OUTPUT=$'missing     /run/setrans\nmissing     /var/run/faillock\nmissing     /var/lock/subdir\nmissing   c /etc/foo.conf\nmissing   c /boot/grub2/grubenv\nmissing     /usr/bin/foo\n'
    }
    rpm_missing_paths="$(rpm_verify_missing_paths)"
    [[ "$rpm_missing_paths" == $'/boot/grub2/grubenv\n/etc/foo.conf\n/usr/bin/foo' ]] \
        || { echo "FAIL: runtime paths were not filtered from rpm verify input: $rpm_missing_paths" >&2; exit 1; }
) || exit 1

# rpmdb-owned symlinks are excluded from the reinstall package set: rpm's
# reinstall erase phase removes %config(noreplace) symlinks again, so the
# stage could never converge; regular files keep their owning packages.
(
    run_chroot_try()
    {
        case "$*" in
            *'rpm -Va --nofiledigest'*)
                CHROOT_TRY_RC=1
                CHROOT_TRY_OUTPUT=$'missing   c /etc/nfsmount.conf.d/10-nfsv3.conf\nmissing   c /etc/real.conf\n' ;;
            *'rpm -qf'*)
                CHROOT_TRY_RC=0
                CHROOT_TRY_OUTPUT=$'nfsv3-client-utils\t41471\nreal-pkg\t33188' ;;
        esac
    }
    rpm_detected_packages="$(rpm_missing_file_packages)"
    [[ "$rpm_detected_packages" == 'real-pkg' ]] \
        || { echo "FAIL: rpm missing-file mapping kept a symlink owner: $rpm_detected_packages" >&2; exit 1; }
) || exit 1

# ---------------------------------------------------------------------------
# rpm_preflight fail-closed drills: each missing prerequisite names itself and
# no simulation or apply command is reached.
# ---------------------------------------------------------------------------
mkdir -p "$rpm_preflight_root"/usr/bin "$rpm_preflight_root"/usr/lib/sysimage/rpm \
    "$rpm_preflight_root"/etc/yum.repos.d "$rpm_preflight_root"/boot
printf 'db\n' > "$rpm_preflight_root/usr/lib/sysimage/rpm/rpmdb.sqlite"
printf '[fedora]\nenabled=1\n' > "$rpm_preflight_root/etc/yum.repos.d/fedora.repo"
: > "$rpm_preflight_root/usr/bin/rpm"; chmod +x "$rpm_preflight_root/usr/bin/rpm"
: > "$rpm_preflight_root/usr/bin/dnf5"; chmod +x "$rpm_preflight_root/usr/bin/dnf5"
printf 'img\n' > "$rpm_preflight_root/boot/initramfs-test.img"

rpm_preflight_probe()
{
    local mode="${1:-ok}"
    (
        TARGET_ROOT="$rpm_preflight_root"
        TARGET_OS_ID=fedora
        TARGET_DISTRO_FAMILY=fedora
        RUNNING_HOST_MODE=0
        rpm_lock_probe_available() { return 0; }
        rpm_lock_held() { return 1; }
        target_path_is_mounted_rw() { return 0; }
        run_chroot_try()
        {
            CHROOT_TRY_RC=0
            case "$*" in
                *'dnf5 --version'*)
                    if [[ "$mode" == bad-version ]]; then
                        CHROOT_TRY_OUTPUT='dnf5 version 4.9.0'
                    else
                        CHROOT_TRY_OUTPUT='dnf5 version 5.4.1.0'
                    fi
                    ;;
                *'rpm --version'*) CHROOT_TRY_OUTPUT='RPM version 6.0.1' ;;
                *) CHROOT_TRY_OUTPUT='' ;;
            esac
        }
        case "$mode" in
            locked) rpm_lock_held() { return 0; } ;;
            no-lock-probe) rpm_lock_probe_available() { return 1; } ;;
            boot-ro) target_path_is_mounted_rw() { return 1; } ;;
        esac
        rpm_preflight
    )
}

rpm_preflight_expect_fail()
{
    local reason="$1" mode="${2:-ok}"
    local err
    if err="$(rpm_preflight_probe "$mode" 2>&1)"; then
        echo "FAIL: rpm preflight accepted an invalid fixture ($mode)" >&2
        exit 1
    fi
    grep -Fq "$reason" <<<"$err" \
        || { echo "FAIL: rpm preflight reason missing ($mode): $reason" >&2; printf '%s\n' "$err" >&2; exit 1; }
}

rpm_preflight_probe >/dev/null 2>&1 \
    || { echo 'FAIL: rpm preflight rejected a complete fixture' >&2; exit 1; }
mv "$rpm_preflight_root/usr/bin/rpm" "$rpm_preflight_root/usr/bin/rpm.disabled"
rpm_preflight_expect_fail 'rpm is not installed in the target system.'
mv "$rpm_preflight_root/usr/bin/rpm.disabled" "$rpm_preflight_root/usr/bin/rpm"
mv "$rpm_preflight_root/usr/bin/dnf5" "$rpm_preflight_root/usr/bin/dnf5.disabled"
: > "$rpm_preflight_root/usr/bin/dnf4"; chmod +x "$rpm_preflight_root/usr/bin/dnf4"
rpm_preflight_expect_fail 'dnf4 is not supported by the guarded rpm backend.'
rm -f "$rpm_preflight_root/usr/bin/dnf4"
rpm_preflight_expect_fail 'dnf5 is not installed in the target system.'
mv "$rpm_preflight_root/usr/bin/dnf5.disabled" "$rpm_preflight_root/usr/bin/dnf5"
mv "$rpm_preflight_root/usr/lib/sysimage/rpm/rpmdb.sqlite" "$rpm_preflight_root/usr/lib/sysimage/rpm/rpmdb.sqlite.disabled"
rpm_preflight_expect_fail 'The target RPM database is missing'
mv "$rpm_preflight_root/usr/lib/sysimage/rpm/rpmdb.sqlite.disabled" "$rpm_preflight_root/usr/lib/sysimage/rpm/rpmdb.sqlite"
mv "$rpm_preflight_root/etc/yum.repos.d/fedora.repo" "$rpm_preflight_root/etc/yum.repos.d/fedora.repo.disabled"
printf '[disabled]\nenabled=0\n' > "$rpm_preflight_root/etc/yum.repos.d/disabled.repo"
rpm_preflight_expect_fail 'The target has no enabled dnf repositories.'
rm -f "$rpm_preflight_root/etc/yum.repos.d/disabled.repo"
mv "$rpm_preflight_root/etc/yum.repos.d/fedora.repo.disabled" "$rpm_preflight_root/etc/yum.repos.d/fedora.repo"
rpm_preflight_expect_fail 'The RPM database is locked; refusing a concurrent transaction.' locked
rpm_preflight_expect_fail 'The RPM database lock state cannot be probed' no-lock-probe
rpm_preflight_expect_fail 'The target /boot is not mounted read-write' boot-ro
rpm_preflight_expect_fail 'not a supported dnf5 5.x release' bad-version

# ---------------------------------------------------------------------------
# Fedora kernel pairing and dracut trial/apply/rollback.  The rescue pair is
# excluded, an orphan module directory fails the strict pairing, a trial must
# leave /boot byte-identical and a post-apply lsinitrd failure must restore the
# previous image before the stage fails.
# ---------------------------------------------------------------------------
mkdir -p "$dracut_pair_root"/lib/modules/6.19.10-300.fc44.x86_64 \
    "$dracut_pair_root"/lib/modules/0-rescue-deadbeef \
    "$dracut_pair_root"/usr/lib/modules/5.0.0-orphan \
    "$dracut_pair_root"/usr/lib/dracut \
    "$dracut_pair_root"/usr/bin \
    "$dracut_pair_root"/boot "$dracut_pair_root"/tmp
for dracut_tool in dracut lsinitrd; do
    : > "$dracut_pair_root/usr/bin/$dracut_tool"; chmod +x "$dracut_pair_root/usr/bin/$dracut_tool"
done
: > "$dracut_pair_root/boot/vmlinuz-6.19.10-300.fc44.x86_64"
printf 'initramfs\n' > "$dracut_pair_root/boot/initramfs-6.19.10-300.fc44.x86_64.img"
rpm_pairs="$(TARGET_ROOT="$dracut_pair_root" rpm_kernel_pairs_readonly)"
[[ "$rpm_pairs" == '6.19.10-300.fc44.x86_64 /boot/vmlinuz-6.19.10-300.fc44.x86_64 /boot/initramfs-6.19.10-300.fc44.x86_64.img' ]] \
    || { echo "FAIL: rpm kernel pairing is wrong: $rpm_pairs" >&2; exit 1; }
if (TARGET_ROOT="$dracut_pair_root" rpm_kernel_pairs) >/dev/null 2>&1; then
    echo 'FAIL: an orphan kernel module directory was paired' >&2
    exit 1
fi
if grep -Fq '0-rescue' <<<"$rpm_pairs"; then
    echo 'FAIL: the rescue pseudo-kernel was paired' >&2
    exit 1
fi
rm -rf "$dracut_pair_root/usr/lib/modules/5.0.0-orphan"
rpm_fingerprint="$(TARGET_ROOT="$dracut_pair_root" TARGET_INITRAMFS_BACKEND=dracut initramfs_image_fingerprint)"
grep -Fqx '/boot/initramfs-6.19.10-300.fc44.x86_64.img '"$(sha256sum "$dracut_pair_root/boot/initramfs-6.19.10-300.fc44.x86_64.img" | awk '{print $1}')" <<<"$rpm_fingerprint" \
    || { echo "FAIL: dracut image fingerprint is wrong: $rpm_fingerprint" >&2; exit 1; }
printf 'changed\n' > "$dracut_pair_root/boot/initramfs-6.19.10-300.fc44.x86_64.img"
[[ "$(TARGET_ROOT="$dracut_pair_root" TARGET_INITRAMFS_BACKEND=dracut initramfs_image_fingerprint)" != "$rpm_fingerprint" ]] \
    || { echo 'FAIL: dracut image fingerprint ignored a changed image' >&2; exit 1; }
rm -f "$dracut_pair_root/boot/initramfs-6.19.10-300.fc44.x86_64.img"
grep -Fqx '/boot/initramfs-6.19.10-300.fc44.x86_64.img missing' <<<"$(TARGET_ROOT="$dracut_pair_root" TARGET_INITRAMFS_BACKEND=dracut initramfs_image_fingerprint)" \
    || { echo 'FAIL: dracut image fingerprint did not report a missing image' >&2; exit 1; }

(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_ROOT="$dracut_pair_root"
    TARGET_INITRAMFS_BACKEND=dracut
    SESSION_LOG="$dracut_pair_root/session.log"
    SESSION_DIR="$dracut_pair_root/session"
    mkdir -p "$SESSION_DIR" "$TARGET_ROOT/tmp"
    : > "$SESSION_LOG"
    dracut_image="$TARGET_ROOT/boot/initramfs-6.19.10-300.fc44.x86_64.img"
    dracut_kver=6.19.10-300.fc44.x86_64
    validate_mapper_crypttab() { :; }
    target_path_is_mounted_rw() { return 0; }
    run_selected_chroot() { return 0; }
    dracut_mode=success
    run_chroot_try()
    {
        CHROOT_TRY_RC=0
        case "$*" in
            *'dracut --version'*) CHROOT_TRY_OUTPUT='dracut 108-6.fc44' ;;
            *'dracut -f --kver'*)
                local last="${!#}"
                if [[ "$last" == /tmp/* ]]; then
                    printf 'trial\n' > "$TARGET_ROOT$last"
                    if [[ "$dracut_mode" == touches-boot ]]; then
                        printf 'tampered\n' > "$dracut_image"
                    fi
                elif [[ "$dracut_mode" == unchanged ]]; then
                    printf 'original\n' > "$TARGET_ROOT$last"
                else
                    printf 'rebuilt\n' > "$TARGET_ROOT$last"
                fi
                CHROOT_TRY_OUTPUT='' ;;
            *) CHROOT_TRY_OUTPUT='' ;;
        esac
    }

    printf 'original\n' > "$dracut_image"
    preflight_dracut_initramfs || exit 1
    [[ ! -e "$TARGET_ROOT/tmp/boot-repair-initramfs-preflight-$dracut_kver.img" ]] \
        || { echo 'FAIL: dracut trial output was left behind' >&2; exit 1; }

    printf 'original\n' > "$dracut_image"
    dracut_status="$(adaptive_initramfs_repair)"
    grep -Fqx 'Repair change status initramfs: changed' <<<"$dracut_status" \
        || { echo 'FAIL: dracut rebuild did not report changed' >&2; printf '%s\n' "$dracut_status" >&2; exit 1; }
    [[ "$(cat "$dracut_image")" == rebuilt ]] \
        || { echo 'FAIL: dracut rebuild did not install the new image' >&2; exit 1; }

    printf 'original\n' > "$dracut_image"
    dracut_mode=unchanged
    dracut_status="$(adaptive_initramfs_repair)"
    grep -Fqx 'Repair change status initramfs: unchanged|rebuilt initramfs images are byte-identical' <<<"$dracut_status" \
        || { echo 'FAIL: byte-identical dracut rebuild was not unchanged' >&2; printf '%s\n' "$dracut_status" >&2; exit 1; }

    printf 'original\n' > "$dracut_image"
    dracut_mode=touches-boot
    if (preflight_dracut_initramfs) >"$SESSION_DIR/trial-touch.out" 2>&1; then
        echo 'FAIL: a dracut trial that touched /boot was accepted' >&2
        exit 1
    fi
    grep -Fq 'The dracut trial build modified a /boot initramfs image' "$SESSION_DIR/trial-touch.out" \
        || { echo 'FAIL: dracut /boot-unchanged proof reason changed' >&2; cat "$SESSION_DIR/trial-touch.out" >&2; exit 1; }

    printf 'original\n' > "$dracut_image"
    dracut_mode=success
    dracut_initramfs_verify_rc() { [[ "$1" == /tmp/* ]]; }
    if (adaptive_initramfs_repair) >"$SESSION_DIR/verify-fail.out" 2>&1; then
        echo 'FAIL: a dracut verification failure did not fail the stage' >&2
        exit 1
    fi
    grep -Fq 'the previous initramfs was restored' "$SESSION_DIR/verify-fail.out" \
        || { echo 'FAIL: dracut rollback reason is missing' >&2; cat "$SESSION_DIR/verify-fail.out" >&2; exit 1; }
    [[ "$(cat "$dracut_image")" == original ]] \
        || { echo 'FAIL: dracut rollback did not restore the previous image' >&2; exit 1; }
) || exit 1

# Dracut repair must never pass a hostonly/rescue override and must never
# touch the rescue image, BLS entries, grubenv or grub.cfg.
dracut_apply_body="$(sed -n '/^adaptive_dracut_initramfs_repair()/,/^}/p' "$HELPER")"
grep -Fq 'dracut -f --kver "$kver" "$image"' <<<"$dracut_apply_body" \
    || { echo 'FAIL: dracut per-kernel apply is not wired' >&2; exit 1; }
if grep -Eq -- '--regenerate-all|--no-hostonly|--uefi|--noimageifnotneeded|--no-kernel|restorecon|setenforce|fixfiles' <<<"$dracut_apply_body"; then
    echo 'FAIL: dracut apply passes a forbidden override or relabels SELinux' >&2
    exit 1
fi
if grep -Fq '0-rescue' <<<"$dracut_apply_body"; then
    echo 'FAIL: dracut apply references the rescue pseudo-kernel' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# Fedora GRUB2 config regeneration: --no-grubenv-update is mandatory, the
# trial enforces the menu-entry and BLS entry-set guards, and an apply-time
# entry loss or grubenv mutation restores the session backup byte-identical.
# ---------------------------------------------------------------------------
fedora_grub_root="$(mktemp -d)"
fedora_reinstall_root="$(mktemp -d)"
trap 'rm -rf -- "$fake_root" "$dracut_root" "$dracut_pkg_root" "$cap_root" "$cap_arch_min" "$cap_alpine" "$pair_root" "$mixed_root" "$extlinux_debian_root" "$alpine_efi_root" "$rpm_root" "$rpm_preflight_root" "$dracut_pair_root" "$fedora_grub_root" "$fedora_reinstall_root" "$no_log_root"' EXIT
mkdir -p "$fedora_grub_root"/usr/bin "$fedora_grub_root"/boot/grub2/i386-pc \
    "$fedora_grub_root"/boot/loader/entries "$fedora_grub_root"/etc/kernel "$fedora_grub_root"/etc/default \
    "$fedora_grub_root"/run "$fedora_grub_root"/session
for fedora_tool in grub2-mkconfig grub2-script-check grub2-install grub2-editenv; do
    : > "$fedora_grub_root/usr/bin/$fedora_tool"; chmod +x "$fedora_grub_root/usr/bin/$fedora_tool"
done
printf 'menuentry "keep" { linux /vmlinuz; }\n' > "$fedora_grub_root/boot/grub2/grub.cfg"
{
    printf '# GRUB Environment Block\n'
    printf 'saved_entry=keep\n'
    printf 'blsdir=/boot/loader/entries\n'
} > "$fedora_grub_root/boot/grub2/grubenv"
truncate -s 1024 "$fedora_grub_root/boot/grub2/grubenv"
printf 'title Fedora\nversion 6.19.10-300.fc44.x86_64\nlinux /vmlinuz-6.19.10-300.fc44.x86_64\ninitrd /initramfs-6.19.10-300.fc44.x86_64.img\n' \
    > "$fedora_grub_root/boot/loader/entries/cfe2564d1eaf4bd88faaff5dd35b5e31-6.19.10-300.fc44.x86_64.conf"
printf 'loading\nGeom\n' > "$fedora_grub_root/boot/grub2/i386-pc/boot.img"
printf 'loading\nGeom\n' > "$fedora_grub_root/boot/grub2/i386-pc/core.img"
cp -a "$fedora_grub_root/boot/grub2/grub.cfg" "$fedora_grub_root/grub.cfg.orig"
cp -a "$fedora_grub_root/boot/grub2/grubenv" "$fedora_grub_root/grubenv.orig"

fedora_grub_stub_env()
{
    SESSION_DIR="$fedora_grub_root/session"
    SESSION_LOG="$SESSION_DIR/session.log"
    : > "$SESSION_LOG"
    TARGET_ROOT="$fedora_grub_root"
    TARGET_DISTRO_FAMILY=fedora
    TARGET_INITRAMFS_BACKEND=dracut
    TARGET_BOOTLOADER_BACKEND=grub
    RUNNING_HOST_MODE=0
    target_path_is_mounted_rw() { return 0; }
    validate_mapper_crypttab() { :; }
    fedora_grub_boot_code_broken() { return 1; }
    run_selected_chroot()
    {
        local last="${!#}"
        printf 'menuentry "keep" { linux /vmlinuz; }\n' > "$TARGET_ROOT$last"
        return 0
    }
    run_chroot_try()
    {
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=''
        case "$*" in
            *'--no-grubenv-update -o /boot/grub2/grub.cfg'*)
                printf 'menuentry "keep" { linux /vmlinuz; }\n' > "$TARGET_ROOT/boot/grub2/grub.cfg"
                ;;
        esac
    }
}

(
    trap - EXIT INT TERM HUP
    fedora_grub_stub_env
    cp -a "$fedora_grub_root/grub.cfg.orig" "$TARGET_ROOT/boot/grub2/grub.cfg"
    cp -a "$fedora_grub_root/grubenv.orig" "$TARGET_ROOT/boot/grub2/grubenv"
    grub_status="$(adaptive_fedora_grub_repair config-only)"
    grep -Fqx 'Repair change status grub: unchanged|grub.cfg and grubenv are byte-identical' <<<"$grub_status" \
        || { echo 'FAIL: an idempotent Fedora GRUB2 regeneration was not unchanged' >&2; printf '%s\n' "$grub_status" >&2; exit 1; }

    # Foreign-OS entries added or dropped by os-prober are reported as
    # evidence but never silently: additions are named and a disappearing
    # foreign entry is recorded without blocking the repair, while an existing
    # native entry must still be preserved or the guard fails closed.
    foreign_existing="$SESSION_DIR/guard-existing.cfg"
    foreign_candidate="$SESSION_DIR/guard-candidate.cfg"
    {
        printf 'menuentry "keep" { linux /vmlinuz; }\n'
        printf "menuentry 'Fedora Linux 44 (Workstation Edition) (on /dev/vda3)' --class fedora { linux /vmlinuz; }\n"
    } > "$foreign_existing"
    {
        printf 'menuentry "keep" { linux /vmlinuz; }\n'
        printf "menuentry 'Fedora Linux 44 (Workstation Edition) (on /dev/vda3)' --class fedora { linux /vmlinuz; }\n"
        printf "submenu 'Ubuntu 26.04 (on /dev/vdb1)' \$menuentry_id_option 'osprober-gnulinux-simple-abc' { menuentry 'Ubuntu' { linux /vmlinuz; } }\n"
    } > "$foreign_candidate"
    guard_grub_candidate_preserves_entries "$foreign_existing" "$foreign_candidate" \
        || { echo 'FAIL: a foreign-entry addition blocked the GRUB guard' >&2; exit 1; }
    grep -Fq 'foreign-entry-added:' "$SESSION_LOG" \
        || { echo 'FAIL: the added foreign entry was not reported as evidence' >&2; cat "$SESSION_LOG" >&2; exit 1; }
    grep -Fq 'Ubuntu 26.04 (on /dev/vdb1)' "$SESSION_LOG" \
        || { echo 'FAIL: the added foreign entry was not named' >&2; cat "$SESSION_LOG" >&2; exit 1; }
    : > "$SESSION_LOG"
    guard_grub_candidate_preserves_entries "$foreign_candidate" "$foreign_existing" \
        || { echo 'FAIL: a foreign-entry removal blocked the GRUB guard' >&2; exit 1; }
    grep -Fq 'foreign-entry-removed:' "$SESSION_LOG" \
        || { echo 'FAIL: the removed foreign entry was not reported as evidence' >&2; cat "$SESSION_LOG" >&2; exit 1; }
    : > "$SESSION_LOG"
    printf 'menuentry "other" { linux /vmlinuz; }\n' > "$foreign_candidate"
    if guard_grub_candidate_preserves_entries "$foreign_existing" "$foreign_candidate"; then
        echo 'FAIL: a native GRUB menu entry loss was accepted' >&2
        exit 1
    fi
    grep -Fq 'preserved-entry-required: menuentry "keep"' "$SESSION_LOG" \
        || { echo 'FAIL: the native entry-preservation refusal was not reported' >&2; cat "$SESSION_LOG" >&2; exit 1; }

    # Apply-time entry loss: the trial preserves entries, the apply drops one.
    run_chroot_try()
    {
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=''
        case "$*" in
            *'--no-grubenv-update -o /boot/grub2/grub.cfg'*)
                printf 'insmod blscfg\nblscfg\n' > "$TARGET_ROOT/boot/grub2/grub.cfg"
                ;;
        esac
    }
    if (adaptive_fedora_grub_repair config-only) >"$SESSION_DIR/entry-loss.out" 2>&1; then
        echo 'FAIL: an apply-time GRUB menu entry loss was accepted' >&2
        exit 1
    fi
    grep -Fq 'rolled back because it removed an existing boot entry' "$SESSION_DIR/entry-loss.out" \
        || { echo 'FAIL: the entry-loss rollback reason is missing' >&2; cat "$SESSION_DIR/entry-loss.out" >&2; exit 1; }
    cmp -s "$fedora_grub_root/grub.cfg.orig" "$TARGET_ROOT/boot/grub2/grub.cfg" \
        || { echo 'FAIL: the entry-loss rollback did not restore grub.cfg byte-identical' >&2; exit 1; }
    cmp -s "$fedora_grub_root/grubenv.orig" "$TARGET_ROOT/boot/grub2/grubenv" \
        || { echo 'FAIL: the entry-loss rollback did not restore grubenv byte-identical' >&2; exit 1; }

    # A grubenv mutation despite --no-grubenv-update must also roll back.
    run_chroot_try()
    {
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=''
        case "$*" in
            *'--no-grubenv-update -o /boot/grub2/grub.cfg'*)
                printf 'menuentry "keep" { linux /vmlinuz; }\n' > "$TARGET_ROOT/boot/grub2/grub.cfg"
                printf 'mutated-grubenv\n' > "$TARGET_ROOT/boot/grub2/grubenv"
                ;;
        esac
    }
    if (adaptive_fedora_grub_repair config-only) >"$SESSION_DIR/grubenv-mutation.out" 2>&1; then
        echo 'FAIL: a grubenv mutation during regeneration was accepted' >&2
        exit 1
    fi
    grep -Fq 'modified grubenv despite --no-grubenv-update' "$SESSION_DIR/grubenv-mutation.out" \
        || { echo 'FAIL: the grubenv mutation rollback reason is missing' >&2; cat "$SESSION_DIR/grubenv-mutation.out" >&2; exit 1; }
    cmp -s "$fedora_grub_root/grubenv.orig" "$TARGET_ROOT/boot/grub2/grubenv" \
        || { echo 'FAIL: the grubenv rollback did not restore byte-identical' >&2; exit 1; }

    # Trial-time entry loss is refused before any apply.
    run_selected_chroot()
    {
        local last="${!#}"
        printf 'insmod blscfg\nblscfg\n' > "$TARGET_ROOT$last"
        return 0
    }
    if (preflight_fedora_grub) >"$SESSION_DIR/trial-entry-loss.out" 2>&1; then
        echo 'FAIL: a trial candidate that dropped a menu entry was accepted' >&2
        exit 1
    fi
    grep -Fq 'existing menu entries are absent from the generated candidate' "$SESSION_DIR/trial-entry-loss.out" \
        || { echo 'FAIL: the trial entry-preservation reason is missing' >&2; cat "$SESSION_DIR/trial-entry-loss.out" >&2; exit 1; }
    cmp -s "$fedora_grub_root/grub.cfg.orig" "$TARGET_ROOT/boot/grub2/grub.cfg" \
        || { echo 'FAIL: a refused trial changed grub.cfg' >&2; exit 1; }
) || exit 1

# The Fedora regeneration must always pass --no-grubenv-update and must never
# call grub2-set-default/grub2-reboot/grubby/restorecon.
fedora_regen_body="$(sed -n '/^fedora_grub_regenerate_config()/,/^}/p' "$HELPER")"
grep -q -- '--no-grubenv-update' <<<"$fedora_regen_body" \
    || { echo 'FAIL: Fedora GRUB2 regeneration does not pass --no-grubenv-update' >&2; exit 1; }
if grep -Eq 'grub2-set-default|grub2-reboot|grubby|restorecon|setenforce|fixfiles' <<<"$fedora_regen_body"; then
    echo 'FAIL: Fedora GRUB2 regeneration calls a forbidden boot-state or SELinux command' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# Fedora GRUB2 BIOS boot-code reinstall: MBR/bios_grub/i386-pc backup,
# rollback and byte-identity proof, plus the partition-table fail-closed check.
# ---------------------------------------------------------------------------
mkdir -p "$fedora_reinstall_root"/boot/grub2/i386-pc "$fedora_reinstall_root"/session
dd if=/dev/zero of="$fedora_reinstall_root/disk.img" bs=1M count=2 status=none
printf '\xeb\x63\x90GRUB Geom Hard Disk Read Error' | dd of="$fedora_reinstall_root/disk.img" bs=1 conv=notrunc status=none
printf 'EFI PART' | dd of="$fedora_reinstall_root/disk.img" bs=1 seek=512 conv=notrunc status=none
dd if=/dev/zero of="$fedora_reinstall_root/bios-grub.img" bs=1M count=1 status=none
printf 'GRUB loading Geom' | dd of="$fedora_reinstall_root/bios-grub.img" bs=1 conv=notrunc status=none
printf 'loading\nGeom\n' > "$fedora_reinstall_root/boot/grub2/i386-pc/boot.img"
printf 'loading\nGeom\n' > "$fedora_reinstall_root/boot/grub2/i386-pc/core.img"
dd if="$fedora_reinstall_root/disk.img" of="$fedora_reinstall_root/disk.orig" bs=1M count=2 status=none
dd if="$fedora_reinstall_root/bios-grub.img" of="$fedora_reinstall_root/bios.orig" bs=1M count=1 status=none

fedora_reinstall_stub_env()
{
    SESSION_DIR="$fedora_reinstall_root/session"
    SESSION_LOG="$SESSION_DIR/session.log"
    : > "$SESSION_LOG"
    TARGET_ROOT="$fedora_reinstall_root"
    TARGET_DISK="$fedora_reinstall_root/disk.img"
    TARGET_DISTRO_FAMILY=fedora
    TARGET_INITRAMFS_BACKEND=dracut
    TARGET_BOOTLOADER_BACKEND=grub
    RUNNING_HOST_MODE=0
    fedora_bios_grub_partition() { printf '%s\n' "$fedora_reinstall_root/bios-grub.img"; }
    lsblk()
    {
        case "$*" in
            *'NAME,PARTUUID,PARTTYPE,SIZE'*) printf 'disk.img\nbios 1111-2222 21686148-6449-6e6f-744e-656564454649 1M\n' ;;
            *PTTYPE*) printf 'gpt\n' ;;
            *SIZE*) printf '1048576\n' ;;
            *) printf '\n' ;;
        esac
    }
    findmnt() { return 1; }
    same_single_top_disk() { return 0; }
    validate_mapper_crypttab() { :; }
    bios_firmware_mode() { return 0; }
    preflight_fedora_grub_reinstall() { :; }
    run_chroot_try()
    {
        CHROOT_TRY_RC=1
        CHROOT_TRY_OUTPUT='simulated grub2-install failure'
        printf 'corrupted-boot-code' | dd of="$TARGET_DISK" bs=1 conv=notrunc status=none
        printf 'corrupted-boot-code' | dd of="$fedora_reinstall_root/bios-grub.img" bs=1 conv=notrunc status=none
        printf 'tampered\n' > "$TARGET_ROOT/boot/grub2/i386-pc/core.img"
    }
}

(
    trap - EXIT INT TERM HUP
    fedora_reinstall_stub_env
    if (fedora_grub_reinstall_boot_code) >"$SESSION_DIR/reinstall-fail.out" 2>&1; then
        echo 'FAIL: a failing grub2-install must fail the boot-code reinstall' >&2
        exit 1
    fi
    grep -Fq 'restored byte-identical' "$SESSION_DIR/reinstall-fail.out" \
        || { echo 'FAIL: the boot-code rollback reason is missing' >&2; cat "$SESSION_DIR/reinstall-fail.out" >&2; exit 1; }
    cmp -s "$fedora_reinstall_root/disk.orig" "$TARGET_DISK" \
        || { echo 'FAIL: the failed reinstall did not restore the MBR byte-identical' >&2; exit 1; }
    cmp -s "$fedora_reinstall_root/bios.orig" "$fedora_reinstall_root/bios-grub.img" \
        || { echo 'FAIL: the failed reinstall did not restore the bios_grub partition byte-identical' >&2; exit 1; }
    cmp -s <(printf 'loading\nGeom\n') "$TARGET_ROOT/boot/grub2/i386-pc/core.img" \
        || { echo 'FAIL: the failed reinstall did not restore the i386-pc modules byte-identical' >&2; exit 1; }
) || exit 1

(
    trap - EXIT INT TERM HUP
    fedora_reinstall_stub_env
    run_chroot_try()
    {
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=''
        printf 'table-tamper' | dd of="$TARGET_DISK" bs=1 seek=446 conv=notrunc status=none
    }
    if (fedora_grub_reinstall_boot_code) >"$SESSION_DIR/table-change.out" 2>&1; then
        echo 'FAIL: a changed partition table was accepted' >&2
        exit 1
    fi
    grep -Fq 'partition table changed during the GRUB2 boot-code reinstall' "$SESSION_DIR/table-change.out" \
        || { echo 'FAIL: the partition-table change reason is missing' >&2; cat "$SESSION_DIR/table-change.out" >&2; exit 1; }
    if cmp -s "$fedora_reinstall_root/disk.orig" "$TARGET_DISK"; then
        echo 'FAIL: the changed partition table was restored instead of failing closed' >&2
        exit 1
    fi
) || exit 1

# Fedora BIOS boot-stack pairing is fail-closed: a kernel without a BLS entry
# or without a non-empty initramfs image blocks the reconciliation.
(
    trap - EXIT INT TERM HUP
    TARGET_ROOT="$rpm_root"
    TARGET_DISTRO_FAMILY=fedora
    TARGET_INITRAMFS_BACKEND=dracut
    fedora_bootstack_pairing_check
)
mv "$rpm_root/boot/loader/entries/cfe2564d1eaf4bd88faaff5dd35b5e31-6.19.10-300.fc44.x86_64.conf" \
    "$rpm_root/boot/loader/entries/entry.disabled"
if (TARGET_ROOT="$rpm_root" TARGET_DISTRO_FAMILY=fedora TARGET_INITRAMFS_BACKEND=dracut \
    fedora_bootstack_pairing_check) >"$rpm_root/pairing-missing-bls.out" 2>&1; then
    echo 'FAIL: a kernel without a BLS entry passed the Fedora pairing check' >&2
    exit 1
fi
grep -Fq 'requires /boot/loader/entries/cfe2564d1eaf4bd88faaff5dd35b5e31-6.19.10-300.fc44.x86_64.conf' "$rpm_root/pairing-missing-bls.out" \
    || { echo 'FAIL: the missing-BLS pairing reason is missing' >&2; cat "$rpm_root/pairing-missing-bls.out" >&2; exit 1; }
mv "$rpm_root/boot/loader/entries/entry.disabled" \
    "$rpm_root/boot/loader/entries/cfe2564d1eaf4bd88faaff5dd35b5e31-6.19.10-300.fc44.x86_64.conf"
mv "$rpm_root/boot/initramfs-6.19.10-300.fc44.x86_64.img" "$rpm_root/boot/initramfs.disabled"
if (TARGET_ROOT="$rpm_root" TARGET_DISTRO_FAMILY=fedora TARGET_INITRAMFS_BACKEND=dracut \
    fedora_bootstack_pairing_check) >"$rpm_root/pairing-missing-initramfs.out" 2>&1; then
    echo 'FAIL: a kernel without an initramfs image passed the Fedora pairing check' >&2
    exit 1
fi
grep -Fq 'requires /boot/initramfs-6.19.10-300.fc44.x86_64.img' "$rpm_root/pairing-missing-initramfs.out" \
    || { echo 'FAIL: the missing-initramfs pairing reason is missing' >&2; cat "$rpm_root/pairing-missing-initramfs.out" >&2; exit 1; }
mv "$rpm_root/boot/initramfs.disabled" "$rpm_root/boot/initramfs-6.19.10-300.fc44.x86_64.img"

# ---------------------------------------------------------------------------
# Multi-backend dispatch with rpm first: one combined status line and a
# backend-named stage failure.
# ---------------------------------------------------------------------------
rpm_mixed_calls="$rpm_root/rpm-mixed-calls.log"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$rpm_root/session.log"
    SESSION_DIR="$rpm_root/session"
    mkdir -p "$SESSION_DIR"
    : > "$SESSION_LOG"
    : > "$rpm_mixed_calls"
    TARGET_ROOT="$rpm_root"
    TARGET_DISTRO_FAMILY=fedora
    TARGET_PACKAGE_MANAGERS=(rpm apk)
    package_backend_unavailable_reason() { return 0; }
    adaptive_rpm_fix_broken() { printf 'rpm\n' >> "$rpm_mixed_calls"; repair_change_status fixbroken "unchanged|rpm no-op"; }
    adaptive_alpine_apk_fix_broken() { printf 'apk\n' >> "$rpm_mixed_calls"; repair_change_status fixbroken changed; }
    run_package_stage fix-broken fixbroken
) > "$rpm_root/rpm-mixed.out"
[[ "$(cat "$rpm_mixed_calls")" == $'rpm\napk' ]] \
    || { echo 'FAIL: mixed rpm/apk backends did not run in native-first order' >&2; exit 1; }
grep -Fqx 'Repair change status fixbroken: changed|backends: rpm unchanged; apk changed' "$rpm_root/rpm-mixed.out" \
    || { echo 'FAIL: mixed rpm/apk changed status is not combined' >&2; cat "$rpm_root/rpm-mixed.out" >&2; exit 1; }
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$rpm_root/session.log"
    SESSION_DIR="$rpm_root/session"
    TARGET_ROOT="$rpm_root"
    TARGET_DISTRO_FAMILY=fedora
    TARGET_PACKAGE_MANAGERS=(rpm apk)
    package_backend_unavailable_reason() { return 0; }
    adaptive_rpm_fix_broken() { fail "simulated rpm backend failure"; }
    adaptive_alpine_apk_fix_broken() { repair_change_status fixbroken changed; }
    run_package_stage fix-broken fixbroken
) > "$rpm_root/rpm-mixed-fail.out" 2>&1 && { echo 'FAIL: a failing rpm backend did not fail the stage' >&2; exit 1; }
grep -Fq "stage 'fix-broken (rpm)' failed: simulated rpm backend failure" "$rpm_root/rpm-mixed-fail.out" \
    || { echo 'FAIL: failing rpm backend attribution is missing' >&2; cat "$rpm_root/rpm-mixed-fail.out" >&2; exit 1; }

# A detected-but-not-runnable backend must not abort a stage another backend
# can complete: a Debian-family tree that merely ships the rpm package runs
# the apt backend and names the skipped rpm backend in the combined status.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$mixed_root/session.log"
    SESSION_DIR="$mixed_root/session"
    TARGET_ROOT="$mixed_root"
    TARGET_DISTRO_FAMILY=debian
    TARGET_PACKAGE_MANAGERS=(apt/dpkg rpm)
    : > "$mixed_calls"
    adaptive_fix_broken() { printf 'apt\n' >> "$mixed_calls"; repair_change_status fixbroken "unchanged|apt no-op"; }
    adaptive_rpm_fix_broken() { printf 'rpm\n' >> "$mixed_calls"; repair_change_status fixbroken changed; }
    package_backend_unavailable_reason()
    {
        case "$2" in
            rpm) printf 'dnf5 is not installed in the target system'; return 1 ;;
            *) return 0 ;;
        esac
    }
    run_package_stage fix-broken fixbroken
) > "$mixed_root/mixed-skip.out"
[[ "$(cat "$mixed_calls")" == 'apt' ]] \
    || { echo 'FAIL: a not-runnable rpm backend still ran' >&2; cat "$mixed_calls" >&2; exit 1; }
grep -Fqx 'Repair change status fixbroken: unchanged|backends: apt/dpkg unchanged; rpm skipped (dnf5 is not installed in the target system)' "$mixed_root/mixed-skip.out" \
    || { echo 'FAIL: skipped backend is not named in the combined status' >&2; cat "$mixed_root/mixed-skip.out" >&2; exit 1; }
grep -Fq 'SKIP: package backend rpm is not runnable for stage' "$mixed_root/session.log" \
    || { echo 'FAIL: skipped backend was not logged' >&2; exit 1; }

# (b) A Debian tree that boots extlinux: systemd vs OpenRC variants must be
# distinguished by the detected service manager, not by the family.
mkdir -p "$extlinux_debian_root"/usr/bin "$extlinux_debian_root"/usr/sbin "$extlinux_debian_root"/var/lib/dpkg \
    "$extlinux_debian_root"/etc/apt "$extlinux_debian_root"/etc "$extlinux_debian_root"/boot \
    "$extlinux_debian_root"/lib/modules/6.1.0-test
for extlinux_tool in dpkg dpkg-query apt-get update-initramfs mkinitramfs update-extlinux; do
    : > "$extlinux_debian_root/usr/bin/$extlinux_tool"; chmod +x "$extlinux_debian_root/usr/bin/$extlinux_tool"
done
printf 'Package: syslinux\nStatus: install ok installed\nVersion: 6.04\n\n' > "$extlinux_debian_root/var/lib/dpkg/status"
printf 'deb http://deb.example.invalid/ stable main\n' > "$extlinux_debian_root/etc/apt/sources.list"
printf 'overwrite=1\ndefault=lts\n' > "$extlinux_debian_root/etc/update-extlinux.conf"
printf 'DEFAULT menu.c32\nLABEL lts\n  LINUX vmlinuz-lts\n  INITRD initramfs-lts\n' > "$extlinux_debian_root/boot/extlinux.conf"
printf '%s\n' '-lts' > "$extlinux_debian_root/lib/modules/6.1.0-test/kernel-suffix"
: > "$extlinux_debian_root/boot/vmlinuz-lts"; : > "$extlinux_debian_root/boot/initramfs-lts"

extlinux_debian_probe()
{
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        run_selected_chroot()
        {
            [[ "$*" == *dpkg-query* && "$*" == *syslinux* ]] && printf 'installed'
            return 0
        }
        filesystem_scope_resolve() { :; }
        filesystem_scope_tools() { :; }
        TARGET_ROOT="$extlinux_debian_root" TARGET_OS_ID=debian TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=debian \
            diagnostic_repair_capabilities
    )
}

# systemd variant: a graphical.target plus a supported display-manager unit.
mkdir -p "$extlinux_debian_root"/usr/lib/systemd/system
: > "$extlinux_debian_root/usr/lib/systemd/system/graphical.target"
: > "$extlinux_debian_root/usr/lib/systemd/system/lightdm.service"
: > "$extlinux_debian_root/usr/bin/systemctl"; chmod +x "$extlinux_debian_root/usr/bin/systemctl"
extlinux_debian_systemd="$(extlinux_debian_probe)"
cap_expect "$extlinux_debian_systemd" 'display: available' 'extlinux: available'
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    run_selected_chroot() { [[ "$*" == *dpkg-query* && "$*" == *syslinux* ]] && printf 'installed'; return 0; }
    TARGET_ROOT="$extlinux_debian_root" TARGET_OS_ID=debian TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=debian \
        profile_target_backends
    [[ "$TARGET_SERVICE_MANAGER" == systemd && "$TARGET_DISPLAY_BACKEND" == systemd ]] \
        || { echo "FAIL: Debian+systemd backends were not detected (service=$TARGET_SERVICE_MANAGER display=$TARGET_DISPLAY_BACKEND)" >&2; exit 1; }
    [[ "$TARGET_BOOTLOADER_BACKEND" == syslinux/extlinux ]] \
        || { echo "FAIL: Debian extlinux bootloader was not detected: $TARGET_BOOTLOADER_BACKEND" >&2; exit 1; }
)

# OpenRC variant: remove the systemd evidence and provide an OpenRC manager.
rm -rf "$extlinux_debian_root/usr/lib/systemd" "$extlinux_debian_root/usr/bin/systemctl"
mkdir -p "$extlinux_debian_root"/sbin "$extlinux_debian_root"/etc/init.d "$extlinux_debian_root"/etc/runlevels/default
: > "$extlinux_debian_root/sbin/openrc"; chmod +x "$extlinux_debian_root/sbin/openrc"
printf '#!/sbin/openrc-run\nprovide display-manager\ncommand=/usr/bin/lightdm\n' > "$extlinux_debian_root/etc/init.d/lightdm"
chmod +x "$extlinux_debian_root/etc/init.d/lightdm"
extlinux_debian_openrc="$(extlinux_debian_probe)"
cap_expect "$extlinux_debian_openrc" 'display: available' 'extlinux: available'
extlinux_debian_display="$(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_ROOT="$extlinux_debian_root" TARGET_OS_ID=debian TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=debian \
        RUNNING_HOST_MODE=0 diagnostic_display 2>&1
)"
grep -Fq 'OpenRC service manager:' <<<"$extlinux_debian_display" \
    || { echo 'FAIL: Debian+OpenRC did not use the OpenRC display branch' >&2; exit 1; }
if grep -Fq 'Systemd default target:' <<<"$extlinux_debian_display"; then
    echo 'FAIL: Debian+OpenRC fell back to the systemd display branch' >&2
    exit 1
fi
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_ROOT="$extlinux_debian_root" TARGET_OS_ID=debian TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=debian \
        profile_target_backends
    [[ "$TARGET_SERVICE_MANAGER" == OpenRC && "$TARGET_DISPLAY_BACKEND" == OpenRC ]] \
        || { echo "FAIL: Debian+OpenRC backends were not detected (service=$TARGET_SERVICE_MANAGER display=$TARGET_DISPLAY_BACKEND)" >&2; exit 1; }
)

# (d) Alpine UEFI GRUB repair: capability, evidence, stage policy and the
# read-only preflight failures are probe-based.  The detected GRUB EFI backend
# is available when UEFI firmware, grub-install, the grub/grub-efi packages,
# the x86_64-efi module directory and an ESP candidate are present.
mkdir -p "$alpine_efi_root"/sbin "$alpine_efi_root"/usr/sbin "$alpine_efi_root"/usr/lib/grub/x86_64-efi \
    "$alpine_efi_root"/etc/apk "$alpine_efi_root"/lib/apk/db "$alpine_efi_root"/etc/mkinitfs \
    "$alpine_efi_root"/boot "$alpine_efi_root"/boot/grub "$alpine_efi_root"/boot/efi/EFI/alpine \
    "$alpine_efi_root"/boot/efi/EFI/boot "$alpine_efi_root"/lib/modules/6.18.52-0-lts
: > "$alpine_efi_root/sbin/apk"; chmod +x "$alpine_efi_root/sbin/apk"
: > "$alpine_efi_root/usr/sbin/grub-mkconfig"; chmod +x "$alpine_efi_root/usr/sbin/grub-mkconfig"
: > "$alpine_efi_root/usr/sbin/grub-install"; chmod +x "$alpine_efi_root/usr/sbin/grub-install"
: > "$alpine_efi_root/sbin/mkinitfs"; chmod +x "$alpine_efi_root/sbin/mkinitfs"
printf 'http://apk.example.invalid/main\n' > "$alpine_efi_root/etc/apk/repositories"
printf 'alpine-base\n' > "$alpine_efi_root/etc/apk/world"
printf 'P:grub\nV:2.14-r0\n\nP:grub-efi\nV:2.14-r0\n\n' > "$alpine_efi_root/lib/apk/db/installed"
printf '%s\n' '-lts' > "$alpine_efi_root/lib/modules/6.18.52-0-lts/kernel-suffix"
: > "$alpine_efi_root/boot/vmlinuz-lts"; : > "$alpine_efi_root/boot/initramfs-lts"
: > "$alpine_efi_root/boot/grub/grub.cfg"
printf 'loader\n' > "$alpine_efi_root/boot/efi/EFI/alpine/grubx64.efi"
printf 'fallback\n' > "$alpine_efi_root/boot/efi/EFI/boot/bootx64.efi"

# Deterministic firmware-mode probe: the default is UEFI (0); setting
# CONTRACT_EFI_FIRMWARE=1 selects the legacy-BIOS reason paths.
alpine_efi_caps()
{
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        alpine_efi_firmware_available() { return "${CONTRACT_EFI_FIRMWARE:-0}"; }
        filesystem_scope_resolve() { :; }
        filesystem_scope_tools() { :; }
        TARGET_ROOT="$alpine_efi_root" TARGET_OS_ID=alpine TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=alpine \
            diagnostic_repair_capabilities
    )
}

alpine_efi_uefi_caps="$(CONTRACT_EFI_FIRMWARE=0 alpine_efi_caps)"
cap_expect "$alpine_efi_uefi_caps" \
    'efi: available' \
    'grub: available' \
    'fixbroken: available'
grep -Fqx 'Repair capability evidence efi: Alpine GRUB EFI backend; grub-install present; grub and grub-efi packages installed; x86_64-efi module directory present; ESP candidate: /boot/efi; loader: /boot/efi/EFI/alpine/grubx64.efi' <<<"$alpine_efi_uefi_caps" \
    || { echo 'FAIL: Alpine GRUB EFI evidence does not cite the probes' >&2; exit 1; }
grep -Fqx 'Repair capability evidence grub: grub-mkconfig present; /boot/grub/grub.cfg present' <<<"$alpine_efi_uefi_caps" \
    || { echo 'FAIL: Alpine GRUB evidence does not cite the probes' >&2; exit 1; }

# The runtime stage gate accepts efi/grub on the UEFI fixture and refuses the
# legacy-BIOS GRUB backend with the same probe reason as the capability line.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    alpine_efi_firmware_available() { return 0; }
    TARGET_ROOT="$alpine_efi_root" TARGET_OS_ID=alpine TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=alpine
    validate_repair_stages_against_backends efi grub
)
alpine_efi_bios_caps="$(CONTRACT_EFI_FIRMWARE=1 alpine_efi_caps)"
cap_expect "$alpine_efi_bios_caps" \
    'efi: unavailable|GRUB detected on legacy BIOS; no EFI boot path is available' \
    'grub: available'
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    alpine_efi_firmware_available() { return 1; }
    TARGET_ROOT="$alpine_efi_root" TARGET_OS_ID=alpine TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=alpine
    if (validate_repair_stages_against_backends efi) >"$alpine_efi_root/bios-gate.out" 2>&1; then
        echo 'FAIL: the runtime efi stage gate accepted a legacy-BIOS Alpine target' >&2
        exit 1
    fi
    grep -Fq 'GRUB detected on legacy BIOS; no EFI boot path is available' "$alpine_efi_root/bios-gate.out" \
        || { echo 'FAIL: legacy-BIOS efi gate reason changed' >&2; cat "$alpine_efi_root/bios-gate.out" >&2; exit 1; }
    validate_repair_stages_against_backends grub
)

# Missing prerequisites fail closed with probe-specific reasons: no ESP,
# grub-install missing, x86_64-efi modules missing, grub-efi package missing
# and grub configuration tooling missing.
mv "$alpine_efi_root/boot/efi" "$alpine_efi_root/boot/efi.disabled"
cap_expect "$(CONTRACT_EFI_FIRMWARE=0 alpine_efi_caps)" \
    'efi: unavailable|no EFI System Partition candidate on the selected disk'
mv "$alpine_efi_root/boot/efi.disabled" "$alpine_efi_root/boot/efi"

mv "$alpine_efi_root/usr/sbin/grub-install" "$alpine_efi_root/usr/sbin/grub-install.disabled"
cap_expect "$(CONTRACT_EFI_FIRMWARE=0 alpine_efi_caps)" \
    'efi: unavailable|grub-install is not installed in the Alpine target'
mv "$alpine_efi_root/usr/sbin/grub-install.disabled" "$alpine_efi_root/usr/sbin/grub-install"

mv "$alpine_efi_root/usr/lib/grub/x86_64-efi" "$alpine_efi_root/usr/lib/grub/x86_64-efi.disabled"
cap_expect "$(CONTRACT_EFI_FIRMWARE=0 alpine_efi_caps)" \
    'efi: unavailable|the x86_64-efi GRUB module directory is missing from the Alpine target'
mv "$alpine_efi_root/usr/lib/grub/x86_64-efi.disabled" "$alpine_efi_root/usr/lib/grub/x86_64-efi"

printf 'P:grub\nV:2.14-r0\n\n' > "$alpine_efi_root/lib/apk/db/installed"
cap_expect "$(CONTRACT_EFI_FIRMWARE=0 alpine_efi_caps)" \
    'efi: unavailable|grub-efi is not installed in the Alpine target'
printf 'P:grub\nV:2.14-r0\n\nP:grub-efi\nV:2.14-r0\n\n' > "$alpine_efi_root/lib/apk/db/installed"

mv "$alpine_efi_root/usr/sbin/grub-mkconfig" "$alpine_efi_root/usr/sbin/grub-mkconfig.disabled"
cap_expect "$(CONTRACT_EFI_FIRMWARE=0 alpine_efi_caps)" \
    'grub: unavailable|Neither grub-mkconfig nor update-grub is installed in the target system'
mv "$alpine_efi_root/usr/sbin/grub-mkconfig.disabled" "$alpine_efi_root/usr/sbin/grub-mkconfig"

# EFI-stub: detected from vmlinuz-*/initramfs-* on the ESP root with no GRUB.
# The MVP stage reconciles firmware entries only; no file or cmdline synthesis.
mv "$alpine_efi_root/boot/grub/grub.cfg" "$alpine_efi_root/boot/grub/grub.cfg.disabled"
mv "$alpine_efi_root/usr/sbin/grub-mkconfig" "$alpine_efi_root/usr/sbin/grub-mkconfig.disabled"
mv "$alpine_efi_root/usr/sbin/grub-install" "$alpine_efi_root/usr/sbin/grub-install.disabled"
: > "$alpine_efi_root/boot/efi/vmlinuz-lts"
: > "$alpine_efi_root/boot/efi/initramfs-lts"
alpine_efi_stub_caps="$(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    alpine_efi_firmware_available() { return 0; }
    filesystem_scope_resolve() { :; }
    filesystem_scope_tools() { :; }
    TARGET_ROOT="$alpine_efi_root" TARGET_OS_ID=alpine TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=alpine \
        diagnostic_repair_capabilities
)"
cap_expect "$alpine_efi_stub_caps" \
    'efi: available' \
    'grub: unavailable|The detected bootloader is unknown EFI loader; GRUB is not the selected bootloader'
grep -Fqx 'Repair capability evidence efi: Alpine EFI-stub backend; ESP candidate: /boot/efi; kernel images: vmlinuz-lts; initramfs images: initramfs-lts; efibootmgr present' <<<"$alpine_efi_stub_caps" \
    || { echo 'FAIL: Alpine EFI-stub evidence does not cite the ESP probes' >&2; exit 1; }

mv "$alpine_efi_root/boot/efi/initramfs-lts" "$alpine_efi_root/initramfs-lts.removed"
cap_expect "$(CONTRACT_EFI_FIRMWARE=0 alpine_efi_caps)" \
    'efi: unavailable|no EFI-stub initramfs image (initramfs-*) is present at the EFI System Partition root'
mv "$alpine_efi_root/initramfs-lts.removed" "$alpine_efi_root/boot/efi/initramfs-lts"

cap_expect "$(CONTRACT_EFI_FIRMWARE=1 alpine_efi_caps)" \
    'efi: unavailable|EFI-stub boot requires UEFI firmware; the recovery host booted in legacy BIOS mode'

# efibootmgr is required because the stub stage has no files to reinstall.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    alpine_efi_firmware_available() { return 0; }
    command() {
        if [[ "${1:-}" == "-v" && "${2:-}" == "efibootmgr" ]]; then
            return 1
        fi
        builtin command "$@"
    }
    filesystem_scope_resolve() { :; }
    filesystem_scope_tools() { :; }
    TARGET_ROOT="$alpine_efi_root" TARGET_OS_ID=alpine TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=alpine \
        diagnostic_repair_capabilities
) > "$alpine_efi_root/stub-no-efibootmgr.out"
cap_expect "$(cat "$alpine_efi_root/stub-no-efibootmgr.out")" \
    'efi: unavailable|EFI-stub entry repair requires efibootmgr in the recovery host'

# Read-only firmware variables fail the EFI-stub preflight closed: there is no
# file-level fallback that could repair a missing stub entry.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    alpine_efi_backend() { printf 'efi-stub\n'; }
    alpine_efi_firmware_available() { return 0; }
    validate_selected_esp() { EFI_ESP_SOURCE=/dev/contract-esp; EFI_ESP_FSTYPE=vfat; }
    uefi_nvram_writable() { return 1; }
    TARGET_ROOT="$alpine_efi_root" TARGET_OS_ID=alpine TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=alpine
    if (alpine_efi_stub_preflight) >"$alpine_efi_root/stub-preflight.out" 2>&1; then
        echo 'FAIL: the EFI-stub preflight accepted read-only firmware variables' >&2
        exit 1
    fi
    grep -Fq 'EFI-stub entry repair requires writable UEFI variables' "$alpine_efi_root/stub-preflight.out" \
        || { echo 'FAIL: EFI-stub read-only-vars reason changed' >&2; cat "$alpine_efi_root/stub-preflight.out" >&2; exit 1; }
)

# A stub system without a captured entry definition fails closed: entry
# synthesis is explicitly out of MVP scope.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    stub_session="$(mktemp -d)"
    trap 'rm -rf -- "$stub_session"' EXIT
    SESSION_DIR="$stub_session"
    SESSION_LOG="$stub_session/session.log"
    : > "$SESSION_LOG"
    TARGET_ROOT="$alpine_efi_root"
    TARGET_OS_ID=alpine
    TARGET_OS_LIKE=""
    TARGET_DISTRO_FAMILY=alpine
    alpine_efi_stub_preflight() { :; }
    efi_set_inventory_esp_ids() { EFI_TARGET_ESP_PARTUUID=deadbeef-0000-0000-0000-000000000000; }
    efibootmgr() {
        case "$1" in
            -v) printf 'BootCurrent: 0002\nBootOrder: 0002\n' ;;
        esac
        return 0
    }
    if (alpine_efi_stub_reconcile) >"$stub_session/no-entry.out" 2>&1; then
        echo 'FAIL: EFI-stub reconciliation accepted a target without a stub entry definition' >&2
        exit 1
    fi
    grep -Fq 'EFI-stub entry synthesis is not implemented' "$stub_session/no-entry.out" \
        || { echo 'FAIL: EFI-stub synthesis refusal reason changed' >&2; cat "$stub_session/no-entry.out" >&2; exit 1; }
)

# syslinux-EFI is detected and reported, never repaired.
mkdir -p "$alpine_efi_root/boot/efi/EFI/syslinux"
: > "$alpine_efi_root/boot/efi/EFI/syslinux/syslinux.efi"
cap_expect "$(CONTRACT_EFI_FIRMWARE=0 alpine_efi_caps)" \
    'efi: unavailable|Alpine syslinux-EFI boot detected (EFI/syslinux/syslinux.efi); guarded repair is not implemented'

# A failed Alpine grub-install restores the ESP loader files from the session
# backup; read-only firmware variables keep the stage file-only with no NVRAM
# mutation.  Both run against a synthetic ESP tree.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    rollback_root="$(mktemp -d)"
    trap 'rm -rf -- "$rollback_root"' EXIT

    prepare_alpine_grub_stage()
    {
        SESSION_DIR="$rollback_root/session"
        mkdir -p "$SESSION_DIR"
        SESSION_LOG="$SESSION_DIR/session.log"
        : > "$SESSION_LOG"
        TARGET_ROOT="$rollback_root"
        TARGET_ESP_MOUNT=/boot/efi
        EFI_BOOTLOADER_ID=alpine
        EFI_ESP_SOURCE=/dev/contract-esp
        EFI_ESP_FSTYPE=vfat
        mkdir -p "$TARGET_ROOT/boot/efi/EFI/alpine" "$TARGET_ROOT/boot/efi/EFI/boot"
        printf 'loader-original\n' > "$TARGET_ROOT/boot/efi/EFI/alpine/grubx64.efi"
        printf 'fallback-original\n' > "$TARGET_ROOT/boot/efi/EFI/boot/bootx64.efi"
        alpine_efi_backend() { printf 'grub\n'; }
        alpine_efi_firmware_available() { return 0; }
        alpine_grub_install_present() { return 0; }
        alpine_grub_module_dir_present() { return 0; }
        alpine_grub_config_tool_present() { return 0; }
        target_apk_package_installed() { return 0; }
        validate_mapper_crypttab() { :; }
        validate_efi_bootloader_target()
        {
            EFI_GRUB_INSTALL_PATH=/usr/sbin/grub-install
            EFI_BOOTLOADER_ID=alpine
            EFI_ESP_SOURCE=/dev/contract-esp
            EFI_ESP_FSTYPE=vfat
        }
        uefi_nvram_writable() { return 1; }
    }

    prepare_alpine_grub_stage
    run_chroot_try()
    {
        if [[ "$*" == *--version* ]]; then
            CHROOT_TRY_RC=0
            CHROOT_TRY_OUTPUT=""
            return 0
        fi
        # Simulate a partially written loader before the failing install.
        printf 'loader-partial\n' > "$TARGET_ROOT/boot/efi/EFI/alpine/grubx64.efi"
        CHROOT_TRY_RC=1
        CHROOT_TRY_OUTPUT="simulated grub-install failure"
        return 0
    }
    if (alpine_grub_efi_repair) >"$rollback_root/rollback.out" 2>&1; then
        echo 'FAIL: a failing Alpine grub-install must fail the EFI stage' >&2
        exit 1
    fi
    grep -Fq 'the ESP loader files were restored from the session backup' "$rollback_root/rollback.out" \
        || { echo 'FAIL: the Alpine EFI rollback reason is missing' >&2; cat "$rollback_root/rollback.out" >&2; exit 1; }
    [[ "$(cat "$TARGET_ROOT/boot/efi/EFI/alpine/grubx64.efi")" == 'loader-original' ]] \
        || { echo 'FAIL: the Alpine EFI vendor loader was not restored after rollback' >&2; exit 1; }
    [[ "$(cat "$TARGET_ROOT/boot/efi/EFI/boot/bootx64.efi")" == 'fallback-original' ]] \
        || { echo 'FAIL: the Alpine EFI fallback loader was not restored after rollback' >&2; exit 1; }

    rm -rf -- "${rollback_root:?}/boot"
    prepare_alpine_grub_stage
    # The file-only path also recreates a firmware fallback that was deleted.
    rm -f "$TARGET_ROOT/boot/efi/EFI/boot/bootx64.efi"
    efibootmgr()
    {
        printf '%s\n' "$*" >> "$rollback_root/efibootmgr-calls"
        return 1
    }
    run_chroot_try()
    {
        if [[ "$*" == *--version* ]]; then
            CHROOT_TRY_RC=0
            CHROOT_TRY_OUTPUT=""
            return 0
        fi
        printf 'loader-reinstalled\n' > "$TARGET_ROOT/boot/efi/EFI/alpine/grubx64.efi"
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=""
        return 0
    }
    (alpine_grub_efi_repair) >"$rollback_root/file-only.out" 2>&1
    grep -Fq 'update loader files and the fallback copy only' "$rollback_root/file-only.out" \
        || { echo 'FAIL: read-only firmware variables must select the file-only Alpine path' >&2; cat "$rollback_root/file-only.out" >&2; exit 1; }
    grep -Fqx 'Repair change status efi: changed' "$rollback_root/file-only.out" \
        || { echo 'FAIL: the file-only Alpine EFI stage must report changed' >&2; cat "$rollback_root/file-only.out" >&2; exit 1; }
    [[ -f "$rollback_root/efibootmgr-calls" ]] \
        || { echo 'FAIL: the file-only Alpine EFI stage never read the firmware state' >&2; exit 1; }
    if grep -vqx -- '-v' "$rollback_root/efibootmgr-calls"; then
        echo 'FAIL: read-only firmware variables must not mutate NVRAM' >&2
        cat "$rollback_root/efibootmgr-calls" >&2
        exit 1
    fi
    [[ "$(cat "$TARGET_ROOT/boot/efi/EFI/boot/bootx64.efi")" == 'loader-reinstalled' ]] \
        || { echo 'FAIL: the Alpine firmware fallback loader was not refreshed' >&2; exit 1; }

    # A firmware reconciliation failure after a successful install restores the
    # ESP backup instead of leaving the reinstalled loader in place.
    rm -rf -- "${rollback_root:?}/boot"
    prepare_alpine_grub_stage
    uefi_nvram_writable() { return 0; }
    efibootmgr()
    {
        case "$1" in
            -v) printf 'BootCurrent: 0002\nBootOrder: 0002\n' ;;
        esac
        return 0
    }
    efi_reconcile_firmware_inventory() { return 1; }
    run_chroot_try()
    {
        if [[ "$*" == *--version* ]]; then
            CHROOT_TRY_RC=0
            CHROOT_TRY_OUTPUT=""
            return 0
        fi
        printf 'loader-reinstalled\n' > "$TARGET_ROOT/boot/efi/EFI/alpine/grubx64.efi"
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT=""
        return 0
    }
    if (alpine_grub_efi_repair) >"$rollback_root/reconcile-fail.out" 2>&1; then
        echo 'FAIL: a firmware reconciliation failure must fail the Alpine EFI stage' >&2
        exit 1
    fi
    grep -Fq 'firmware reconciliation failed; the ESP loader files were restored from the session backup' "$rollback_root/reconcile-fail.out" \
        || { echo 'FAIL: the Alpine reconcile-failure rollback reason is missing' >&2; cat "$rollback_root/reconcile-fail.out" >&2; exit 1; }
    [[ "$(cat "$TARGET_ROOT/boot/efi/EFI/alpine/grubx64.efi")" == 'loader-original' ]] \
        || { echo 'FAIL: the Alpine EFI vendor loader was not restored after a reconcile failure' >&2; exit 1; }
    [[ "$(cat "$TARGET_ROOT/boot/efi/EFI/boot/bootx64.efi")" == 'fallback-original' ]] \
        || { echo 'FAIL: the Alpine EFI fallback loader was not restored after a reconcile failure' >&2; exit 1; }
)

echo "PASS: distribution and boot backend profile contract is wired and read-only."
