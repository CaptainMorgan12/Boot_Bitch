#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="$ROOT_DIR/scripts/boot-repair-helper.sh"

[[ -x "$HELPER" ]] || { echo "FAIL: helper is not executable" >&2; exit 1; }
bash -n "$HELPER"

# Keep the profile contracts visible so Arch modifying stages can only be
# reached through their transaction-specific preflight gates.
grep -q '^is_debian_family()' "$HELPER"
grep -q '^is_arch_family()' "$HELPER"
grep -q '^profile_target_backends()' "$HELPER"
grep -q 'TARGET_PACKAGE_MANAGER="pacman"' "$HELPER"
grep -q 'TARGET_INITRAMFS_BACKEND="mkinitcpio"' "$HELPER"
grep -q 'TARGET_INITRAMFS_BACKEND="dracut"' "$HELPER"
grep -q 'TARGET_BOOTLOADER_BACKEND="systemd-boot' "$HELPER"
grep -q 'Arch profile — guarded pacman/mkinitcpio/GRUB/EFI repairs' "$HELPER"
grep -q 'diagnostic_backend_profile()' "$HELPER"
grep -q 'mount_target_boot_entry "/efi" ro' "$HELPER"
grep -q '^preflight_arch_pacman_transaction()' "$HELPER"
grep -q '^adaptive_arch_pacman_repair()' "$HELPER"
grep -q '^preflight_arch_initramfs()' "$HELPER"
grep -q '^adaptive_arch_initramfs_repair()' "$HELPER"
grep -q 'grub-mkconfig with an isolated output path' "$HELPER"
grep -q '^validate_selected_esp()' "$HELPER"
grep -q '^efi_selected_generic_loader()' "$HELPER"
grep -q '^efi_ensure_selected_generic_entry()' "$HELPER"
grep -q 'efi_ensure_selected_generic_entry' "$HELPER"

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

rm -rf -- "$fake_root/usr/bin"/* "$fake_root/etc/mkinitcpio.d" "$fake_root/boot/loader"
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
if arch_pacman_transaction_is_safe $'error: failed retrieving file extra.db from mirror'; then
    echo "FAIL: pacman repository failure was accepted" >&2
    exit 1
fi

echo "PASS: distribution and boot backend profile contract is wired and read-only."
