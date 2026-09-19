#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="${HELPER:-$ROOT_DIR/scripts/boot-repair-helper.sh}"

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
grep -q '^arch_pacman_transaction_reported_no_changes()' "$HELPER"
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
    [[ "$(grep -c '^Repair tool ' <<<"$output")" -eq 12 ]]
    [[ "$(grep -c '^Repair capability evidence [a-z]*: ' <<<"$output")" -eq 12 ]]
    grep -Fqx 'Repair capability evidence (read-only):' <<<"$output"
    local -a keys=(validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub bootstack)
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
    'fixbroken: unavailable|pacman is not installed in the target' \
    'aptupdate: unavailable|Standalone APT metadata refresh is not available on Arch; use Upgrade installed packages for one full pacman transaction' \
    'upgrade: unavailable|pacman is not installed in the target' \
    'dkms: unavailable|DKMS is not installed in the Arch target system' \
    'display: unavailable|graphical.target is missing from the Arch target system' \
    'initramfs: unavailable|mkinitcpio is not installed in the target system' \
    'efi: unavailable|grub-mkconfig/update-grub is not installed in the target system' \
    'grub: unavailable|Neither grub-mkconfig nor update-grub is installed in the target system' \
    'bootstack: unavailable|Arch boot-stack reconciliation requires available initramfs, GRUB and EFI repair prerequisites'

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
[[ "$(grep -c '^Repair tool ' <<<"$individual_report")" -eq 12 ]] \
    || { echo 'FAIL: individual diagnostic lost capability key lines' >&2; exit 1; }
[[ "$(grep -c '^Repair capability evidence [a-z]*: ' <<<"$individual_report")" -eq 12 ]] \
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
    [[ "$(grep -c '^Repair tool ' <<<"$combined_report")" -eq 12 ]] \
        || { echo "FAIL: $label combined report lost capability key lines" >&2; exit 1; }
    [[ "$(grep -c '^Repair capability evidence [a-z]*: ' <<<"$combined_report")" -eq 12 ]] \
        || { echo "FAIL: $label combined report lost capability evidence lines" >&2; exit 1; }
    [[ "$(grep -c '^SECTION ' <<<"$combined_report")" -eq 14 ]] \
        || { echo "FAIL: $label combined report did not run every diagnostic section" >&2; exit 1; }
    [[ "$(grep -c '^Diagnostic: ' <<<"$combined_report")" -eq 14 ]] \
        || { echo "FAIL: $label combined report diagnostic section headers changed" >&2; exit 1; }
    for cap_key in validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub bootstack; do
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
    'display: unavailable|No supported display manager is installed in the Arch target system' \
    'initramfs: unavailable|mkinitcpio is not installed in the target system' \
    'efi: unavailable|grub-mkconfig/update-grub is not installed in the target system'

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

rm -rf -- "${cap_root:?}"/usr/bin/* "${cap_root:?}"/usr/lib/modules "${cap_root:?}"/boot/grub
mkdir -p "$cap_root"/usr/bin "$cap_root"/usr/lib/systemd/system "$cap_root"/boot
for cap_tool in apt-get dpkg-query; do
    : > "$cap_root/usr/bin/$cap_tool"; chmod +x "$cap_root/usr/bin/$cap_tool"
done
: > "$cap_root/usr/lib/systemd/system/graphical.target"
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
    'efi: unavailable|grub-mkconfig/update-grub is not installed in the target system' \
    'bootstack: unavailable|Requires available initramfs and GRUB repair prerequisites'

mkdir -p "$cap_root"/usr/bin "$cap_root"/usr/lib/systemd/system "$cap_root"/usr/sbin "$cap_root"/boot/grub
for cap_tool in dpkg apt-get dkms grub-mkconfig grub-install; do
    : > "$cap_root/usr/bin/$cap_tool"; chmod +x "$cap_root/usr/bin/$cap_tool"
done
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

# A configured dracut backend must never claim Debian initramfs repair.
mkdir -p "$cap_root/usr/bin" "$cap_root/usr/lib/dracut"
: > "$cap_root/usr/bin/dracut"; chmod +x "$cap_root/usr/bin/dracut"
cap_dracut="$(TARGET_ROOT="$cap_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
    TARGET_INITRAMFS_BACKEND=dracut diagnostic_repair_capabilities)"
cap_expect "$cap_dracut" \
    'initramfs: unavailable|initramfs backend is dracut, which the current Debian repair implementation does not handle' \
    'bootstack: unavailable|Requires available initramfs and GRUB repair prerequisites'
grep -Fqx 'Repair capability evidence initramfs: initramfs backend: dracut; dracut executable and /usr/lib/dracut present' <<<"$cap_dracut"

rm -rf -- "$cap_root/usr/lib/dracut"
cap_expect \
    "$(TARGET_ROOT="$cap_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
        TARGET_INITRAMFS_BACKEND=dracut diagnostic_repair_capabilities)" \
    'initramfs: unavailable|initramfs backend is dracut but /usr/lib/dracut is missing from the target'

rm -f "$cap_root/usr/bin/dracut"
mkdir -p "$cap_root/usr/lib/dracut"
cap_expect \
    "$(TARGET_ROOT="$cap_root" TARGET_OS_ID=tuxedo TARGET_OS_LIKE=debian TARGET_DISTRO_FAMILY=debian \
        TARGET_INITRAMFS_BACKEND=dracut diagnostic_repair_capabilities)" \
    'initramfs: unavailable|initramfs backend is dracut but the dracut executable is missing from the target'

rm -rf -- "$cap_root"
TARGET_ROOT="$cap_root" TARGET_OS_ID=fedora TARGET_OS_LIKE="" TARGET_DISTRO_FAMILY=fedora \
    TARGET_INITRAMFS_BACKEND=dracut diagnostic_repair_capabilities > "$fake_root/cap-unsupported.log"
cap_assert_lines < "$fake_root/cap-unsupported.log"
grep -Fqx 'Repair tool validate: available' "$fake_root/cap-unsupported.log"
for cap_key in dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub bootstack; do
    grep -Eq "^Repair tool $cap_key: unavailable\|Modifying repairs require a supported Debian/Ubuntu or Arch backend\$" "$fake_root/cap-unsupported.log"
done

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
if [[ -e /var/lib/dpkg/lock || -e /var/lib/dpkg/lock-frontend || -e /var/lib/apt/lists/lock || -e /var/lib/pacman/db.lck ]]; then
    if (FAKE_PKG_GATE_PROC=packagekitd FAKE_PKG_GATE_LOCK=1 PATH="$gate_bin:$PATH" host_package_manager_gate); then
        echo 'FAIL: active package-manager lock accepted' >&2
        exit 1
    fi
fi

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
# near-identical lines.  Unit-scoped (-u) queries and --list-boots are exempt.
awk '
    /^diagnostic_boot_evidence\(\)/ { target = "diagnostic_boot_evidence"; functions_seen++; queries = 0; filtered = 0; collapsed = 0; expect_filter = 0; next }
    /^diagnostic_display\(\)/ { target = "diagnostic_display"; functions_seen++; queries = 0; filtered = 0; collapsed = 0; expect_filter = 0; next }
    target == "" { next }
    /^}/ {
        if (queries == 0 || filtered != queries || collapsed != queries) {
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
grep -q 'prepare_running_host "\$raw_disk" "\$raw_root" no no' <<<"$host_shell_body"
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
grep -Fq 'dpkg-configure) dpkg_configure_stage' "$HELPER"
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
Boot0003* arch	HD(1,GPT,73bd4280-6253-46c5-9a5b-23259be97abc,0x800,0x200000)/\EFI\arch\grubx64.efi
Boot0004* arch	HD(1,GPT,2e49c42a-d8db-4ff0-b147-628c8fa47b2f,0x800,0x200000)/\EFI\arch\grubx64.efi
EOF
    printf '0004\tefibootmgr: ** Warning ** : Boot0003 has same label arch\tkey\n' \
        > "$efi_order_root/polluted-map.tsv"
    efibootmgr() {
        case "$1" in
            -v) printf 'BootCurrent: 0002\nBootOrder: 0003,0004\n'
                printf 'Boot0003* arch\tHD(1,GPT,73bd4280-6253-46c5-9a5b-23259be97abc,0x800,0x200000)/\\EFI\\arch\\grubx64.efi\n'
                printf 'Boot0004* arch\tHD(1,GPT,2e49c42a-d8db-4ff0-b147-628c8fa47b2f,0x800,0x200000)/\\EFI\\arch\\grubx64.efi\n'
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

echo "PASS: distribution and boot backend profile contract is wired and read-only."
