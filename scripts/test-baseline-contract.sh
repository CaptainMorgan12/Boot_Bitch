#!/usr/bin/env bash
# Consolidated baseline evidence contract.
#
# One harness that covers the four (now archived) baseline contracts in a single
# ctest case:
#   * the generic `--evidence <section> <base64>` parse/dispatch/reset over the
#     full key matrix (kernel, gpu, display, report, uki, grub, boot-evidence)
#     plus the `boot` -> fail-closed remap and the unknown-key/unconsumed-section
#     fail-closed refusals, tested ONCE;
#   * the per-section parse round-trip + fail-closed blocks for gpu / display /
#     uki / grub / extlinux with the poison-stub "repair consumes the cache,
#     never re-derives fresh, keeps fresh guards" checks;
#   * the gpu fresh-recheck divergence + interactive-menu re-collect semantics
#     and the extlinux boot-stack serialization assertions.
#
# Static wiring plus a behavioural harness that sources the helper without main
# and stubs only the read-only target probes and the package-manager/bootloader
# transcript plumbing.  No real block device, package manager, GPU, DKMS, chroot
# or firmware is ever touched.
# shellcheck disable=SC1090,SC2034,SC2317
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="${HELPER:-$ROOT_DIR/scripts/boot-repair-helper.sh}"

[[ -x "$HELPER" ]] || { echo 'FAIL: helper is not executable' >&2; exit 1; }
bash -n "$HELPER"

fail() { printf 'FAIL: %s\n' "$*" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Static wiring contracts (merged from the four archived baseline contracts)
# ---------------------------------------------------------------------------
for fn in \
    store_evidence_baseline parse_cached_evidence_section \
    parse_cached_gpu_inventory gpu_repair_prepare_evidence \
    repair_gpu_purge_wrong_driver repair_gpu_install_correct_driver \
    repair_gpu_blacklist_module repair_gpu_rebuild_dkms repair_gpu_rebuild_boot \
    repair_gpu_remove_older_kernel repair_gpu_replace_kernel \
    repair_kernel_prepare_inventory repair_kernel_versions \
    parse_cached_display_inventory resolve_display_manager resolve_alpine_display_manager \
    parse_cached_uki_inventory parse_cached_grub_inventory \
    efi_loader_layout_classification resolve_efi_loader_layout \
    grub_resolved_layout grub_resolved_config_path grub_resolved_generator \
    grub_resolved_boot_code_broken \
    parse_cached_extlinux_inventory extlinux_resolved_config_path \
    extlinux_inventory_collect extlinux_configured_overwrite \
    extlinux_config_path extlinux_entry_keys preflight_extlinux \
    adaptive_extlinux_repair guard_extlinux_candidate_preserves_entries; do
    grep -q "^${fn}()" "$HELPER" || fail "helper is missing ${fn}()"
done

# --- GPU baseline wiring -----------------------------------------------------
grep -q -- '--gpu-baseline)' "$HELPER" \
    || fail 'the --gpu-baseline GPU baseline mode hint is not parsed'
grep -q 'GPU_BASELINE_B64=' "$HELPER" \
    || fail 'the gpu baseline state variable is missing'
grep -q 'GPU_BASELINE_B64=""' "$HELPER" \
    || fail 'parse_repair_arguments must reset GPU_BASELINE_B64'
grep -q -- '--baseline)' "$HELPER" \
    || fail 'the --baseline kernel baseline mode hint is not parsed'
grep -q 'for i in "${!GPU_HARDWARE_DEVICES\[@\]}"' "$HELPER" \
    || fail 'diagnostic_gpu must iterate GPU_HARDWARE_DEVICES to emit every device'
for fn in repair_gpu_purge_wrong_driver repair_gpu_install_correct_driver \
    repair_gpu_blacklist_module repair_gpu_rebuild_dkms repair_gpu_rebuild_boot; do
    body="$(grep -A2 "^${fn}()" "$HELPER")"
    grep -q 'gpu_repair_prepare_evidence' <<<"$body" \
        || fail "${fn} must re-run the fresh safety re-check via gpu_repair_prepare_evidence"
done
for fn in repair_gpu_remove_older_kernel repair_gpu_replace_kernel; do
    body="$(grep -A3 "^${fn}()" "$HELPER")"
    grep -q 'repair_kernel_prepare_inventory' <<<"$body" \
        || fail "${fn} must route through repair_kernel_prepare_inventory"
done
grep -q 'no cached GPU baseline' "$HELPER" \
    || fail 'the missing-baseline refusal message is missing'
grep -q 'could not be decoded' "$HELPER" \
    || fail 'the undecodable-baseline refusal message is missing'
grep -q 'is empty' "$HELPER" \
    || fail 'the empty-baseline refusal message is missing'
grep -q 'expected GPU driver changed' "$HELPER" \
    || fail 'the stale-expected-driver refusal message is missing'
grep -q '^boot_failure_classification()' "$HELPER" \
    || fail 'the shared boot_failure_classification() classifier is missing'
if grep -q '^gpu_boot_reached()' "$HELPER"; then
    fail 'gpu_boot_reached() must be removed; the GPU gate consumes boot_failure_classification'
fi
grep -q "GPU_BOOT_REACHED" "$HELPER" \
    && fail 'GPU_BOOT_REACHED global must be removed' || true
grep -q 'GPU boot reached:' "$HELPER" \
    && fail 'the GPU diagnostic must no longer emit a GPU boot reached: line' || true

# --- display baseline wiring -------------------------------------------------
grep -q -- '--evidence)' "$HELPER" \
    || fail 'the --evidence baseline flag is not parsed'
grep -q 'DISPLAY_BASELINE_B64=""' "$HELPER" \
    || fail 'parse_repair_arguments must reset DISPLAY_BASELINE_B64'
grep -q 'REPORT_BASELINE_B64=""' "$HELPER" \
    || fail 'parse_repair_arguments must reset REPORT_BASELINE_B64'
grep -q 'echo "Display manager decision:' "$HELPER" \
    || fail 'the display diagnostic must emit a Display manager decision: line'
[[ "$(grep -c 'Display manager decision:' "$HELPER")" -ge 2 ]] \
    || fail 'both diagnostic_display and diagnostic_display_openrc must emit the decision line'
for fn in preflight_dkms preflight_initramfs adaptive_initramfs_repair; do
    body="$(grep -A40 "^${fn}()" "$HELPER")"
    grep -q 'repair_kernel_versions' <<<"$body" \
        || fail "${fn} must enumerate through repair_kernel_versions"
done
body="$(grep -A8 '^adaptive_display_manager_repair()' "$HELPER")"
grep -q 'parse_cached_display_inventory' <<<"$body" \
    || fail 'adaptive_display_manager_repair must parse the cached display baseline'
for fn in diagnostic_display diagnostic_display_openrc; do
    body="$(sed -n "/^${fn}()/,/^}/p" "$HELPER")"
    grep -q 'boot_failure_classification' <<<"$body" \
        || fail "${fn} must call boot_failure_classification"
    grep -q "printf 'Boot reached: %s" <<<"$body" \
        || fail "${fn} must emit a Boot reached: line"
done
grep -q 'reason display-already-correct-boot-tty' "$HELPER" \
    || fail 'adaptive_display_manager_repair must emit the display-already-correct-boot-tty reason'

# --- uki/grub baseline wiring ------------------------------------------------
grep -q 'UKI_BASELINE_B64=""' "$HELPER" \
    || fail 'parse_repair_arguments must reset UKI_BASELINE_B64'
grep -q 'GRUB_BASELINE_B64=""' "$HELPER" \
    || fail 'parse_repair_arguments must reset GRUB_BASELINE_B64'
grep -qE 'uki\)[[:space:]]+UKI_BASELINE_B64' "$HELPER" \
    || fail 'store_evidence_baseline must map uki to UKI_BASELINE_B64'
grep -qE 'grub\)[[:space:]]+GRUB_BASELINE_B64' "$HELPER" \
    || fail 'store_evidence_baseline must map grub to GRUB_BASELINE_B64'
grep -qE 'uki\)[[:space:]]+parse_cached_uki_inventory' "$HELPER" \
    || fail 'parse_cached_evidence_section must route uki'
grep -qE 'grub\)[[:space:]]+parse_cached_grub_inventory' "$HELPER" \
    || fail 'parse_cached_evidence_section must route grub'
grep -q 'echo "Uki decision:' "$HELPER" \
    || fail 'diagnostic_uki must emit the Uki decision: line'
grep -q 'echo "Grub decision:' "$HELPER" \
    || fail 'diagnostic_grub must emit the Grub decision: line'
grep -q 'Grub menu entry keys (normalized):' "$HELPER" \
    || fail 'diagnostic_grub must emit the normalized menu-entry key set'
grep -q 'resolve_efi_loader_layout' <<<"$(sed -n '/^reinstall_efi_bootloader()/,/^}/p' "$HELPER")" \
    || fail 'reinstall_efi_bootloader must route through resolve_efi_loader_layout'
grep -q 'parse_cached_uki_inventory' <<<"$(sed -n '/^rebuild_tuxedo_uki()/,/^}/p' "$HELPER")" \
    || fail 'rebuild_tuxedo_uki must consume the cached uki baseline'
reinstall_body="$(sed -n '/^reinstall_efi_bootloader()/,/^}/p' "$HELPER")"
for guard in validate_selected_esp validate_efi_bootloader_target \
    conventional_efi_backup_state conventional_efi_restore_backup \
    efi_reconcile_firmware_inventory; do
    grep -q "$guard" <<<"$reinstall_body" \
        || fail "reinstall_efi_bootloader must keep the fresh guard $guard"
done
grep -q 'validate_tuxedo_uki_target' <<<"$(sed -n '/^rebuild_tuxedo_uki()/,/^}/p' "$HELPER")" \
    || fail 'rebuild_tuxedo_uki must keep validate_tuxedo_uki_target fresh'
for fn in adaptive_grub_stage adaptive_grub_repair adaptive_fedora_grub_repair; do
    grep -qE 'grub_resolved_(layout|config_path|generator|boot_code_broken)' \
        <<<"$(sed -n "/^${fn}()/,/^}/p" "$HELPER")" \
        || fail "${fn} must consume the resolved grub baseline"
done
grep -q 'guard_grub_candidate_preserves_entries' <<<"$(sed -n '/^adaptive_grub_repair()/,/^}/p' "$HELPER")" \
    || fail 'adaptive_grub_repair must keep the live entry-preservation guard'
grep -q 'fedora_grub_reinstall_boot_code' <<<"$(sed -n '/^adaptive_fedora_grub_repair()/,/^}/p' "$HELPER")" \
    || fail 'adaptive_fedora_grub_repair must keep the fresh boot-code reinstall'
grep -q 'preflight_grub' <<<"$(sed -n '/^adaptive_grub_repair()/,/^}/p' "$HELPER")" \
    || fail 'adaptive_grub_repair must keep the trial preflight_grub'

# --- extlinux/boot-stack baseline wiring -------------------------------------
grep -q 'BOOT_EVIDENCE_BASELINE_B64=""' "$HELPER" \
    || fail 'parse_repair_arguments must reset BOOT_EVIDENCE_BASELINE_B64'
grep -qE 'boot-evidence\)[[:space:]]+BOOT_EVIDENCE_BASELINE_B64' "$HELPER" \
    || fail 'store_evidence_baseline must map boot-evidence to BOOT_EVIDENCE_BASELINE_B64'
grep -qE 'boot-evidence\)[[:space:]]+parse_cached_extlinux_inventory' "$HELPER" \
    || fail 'parse_cached_evidence_section must route boot-evidence'
if grep -qE '[[:space:]]boot\)[[:space:]]+(BOOT_EVIDENCE_BASELINE_B64|parse_cached_)' "$HELPER"; then
    fail 'store_evidence_baseline/parse_cached_evidence_section must not map the raw boot section'
fi
grep -q 'echo "Extlinux decision:' "$HELPER" \
    || fail 'diagnostic_boot_evidence must emit the Extlinux decision: line'
grep -q 'echo "Extlinux entry keys (normalized):' "$HELPER" \
    || fail 'diagnostic_boot_evidence must emit the normalized extlinux entry-key set'
body="$(grep -A40 '^adaptive_extlinux_repair()' "$HELPER")"
grep -q 'extlinux_resolved_config_path' <<<"$body" \
    || fail 'adaptive_extlinux_repair must route through extlinux_resolved_config_path'
grep -q 'preflight_extlinux' <<<"$body" \
    || fail 'adaptive_extlinux_repair must keep the fresh preflight_extlinux'
grep -q 'guard_extlinux_candidate_preserves_entries' <<<"$body" \
    || fail 'adaptive_extlinux_repair must keep the fresh entry-preservation guard'

# ---------------------------------------------------------------------------
# Behavioural harness (shared)
# ---------------------------------------------------------------------------
sandbox="$(mktemp -d)"
cleanup_sandbox() { rm -rf -- "$sandbox"; }
trap cleanup_sandbox EXIT

fake_root="$sandbox/root"
mkdir -p "$fake_root/etc/modprobe.d" "$fake_root/boot" "$fake_root/usr/bin" \
    "$fake_root/usr/sbin" "$fake_root/var/lib/dpkg" "$fake_root/etc/apt" \
    "$fake_root/usr/lib/systemd/system" "$fake_root/lib/modules" "$fake_root/etc"

source <(sed '/^main "$@"/d' "$HELPER")
trap - EXIT INT TERM HUP
trap cleanup_sandbox EXIT

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
    TARGET_OS_ID=""
    TARGET_REPAIR_BACKEND='Debian/APT'
}

TARGET_ROOT="$fake_root"
RUNNING_HOST_MODE=0
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"

b64() { printf '%s' "$1" | base64 | tr -d '\n'; }

# Default GPU collect stub (the GPU section redefines it locally).
gpu_collect_evidence() { GPU_EVIDENCE_COLLECTED=1; profile_target_backends; }

# ---------------------------------------------------------------------------
# (a) generic --evidence parse/dispatch/reset over the full key matrix
# ---------------------------------------------------------------------------
KERNEL_BASELINE_B64=""
GPU_BASELINE_B64=""
DISPLAY_BASELINE_B64=""
REPORT_BASELINE_B64=""
UKI_BASELINE_B64=""
GRUB_BASELINE_B64=""
BOOT_EVIDENCE_BASELINE_B64=""
parse_repair_arguments \
    --evidence kernel "$(b64 'Kernel running: 6.8.0-fake')" \
    --evidence gpu "$(b64 'GPU hardware: 10de:24b0')" \
    --evidence display "$(b64 'Display manager decision: backend=systemd service=sddm.service package=sddm label=SDDM unit=/usr/lib/systemd/system/sddm.service')" \
    --evidence report "$(b64 'Diagnostic: report')" \
    --evidence uki "$(b64 'Uki decision: layout=vendor-uki esp_source=/dev/sda1 esp_fstype=vfat ownership=target bootorder_lead=0001')" \
    --evidence grub "$(b64 'Grub decision: config=/boot/grub/grub.cfg layout=grub generator=/usr/sbin/grub-mkconfig grubenv=valid bootcode=not-applicable')" \
    --evidence boot-evidence "$(b64 'Extlinux decision: config=/boot/extlinux.conf default=lts overwrite=0')" \
    dkms
[[ "$KERNEL_BASELINE_B64" == "$(b64 'Kernel running: 6.8.0-fake')" ]] \
    || fail '--evidence kernel must populate KERNEL_BASELINE_B64'
[[ "$GPU_BASELINE_B64" == "$(b64 'GPU hardware: 10de:24b0')" ]] \
    || fail '--evidence gpu must populate GPU_BASELINE_B64'
[[ "$DISPLAY_BASELINE_B64" == "$(b64 'Display manager decision: backend=systemd service=sddm.service package=sddm label=SDDM unit=/usr/lib/systemd/system/sddm.service')" ]] \
    || fail '--evidence display must populate DISPLAY_BASELINE_B64'
[[ "$REPORT_BASELINE_B64" == "$(b64 'Diagnostic: report')" ]] \
    || fail '--evidence report must populate REPORT_BASELINE_B64'
[[ "$UKI_BASELINE_B64" == "$(b64 'Uki decision: layout=vendor-uki esp_source=/dev/sda1 esp_fstype=vfat ownership=target bootorder_lead=0001')" ]] \
    || fail '--evidence uki must populate UKI_BASELINE_B64'
[[ "$GRUB_BASELINE_B64" == "$(b64 'Grub decision: config=/boot/grub/grub.cfg layout=grub generator=/usr/sbin/grub-mkconfig grubenv=valid bootcode=not-applicable')" ]] \
    || fail '--evidence grub must populate GRUB_BASELINE_B64'
[[ "$BOOT_EVIDENCE_BASELINE_B64" == "$(b64 'Extlinux decision: config=/boot/extlinux.conf default=lts overwrite=0')" ]] \
    || fail '--evidence boot-evidence must populate BOOT_EVIDENCE_BASELINE_B64'
[[ "${REPAIR_STAGES[*]}" == dkms ]] || fail 'repair stages must be parsed alongside --evidence'

# Backward-compat: --baseline/--gpu-baseline still populate their variables.
KERNEL_BASELINE_B64=""
GPU_BASELINE_B64=""
parse_repair_arguments --baseline "$(b64 'Kernel running: 7.1.9.1-tuxedo')" \
    --gpu-baseline "$(b64 'GPU hardware: 8086:3e9b')" kernels
[[ "$KERNEL_BASELINE_B64" == "$(b64 'Kernel running: 7.1.9.1-tuxedo')" ]] \
    || fail '--baseline must keep populating KERNEL_BASELINE_B64'
[[ "$GPU_BASELINE_B64" == "$(b64 'GPU hardware: 8086:3e9b')" ]] \
    || fail '--gpu-baseline must keep populating GPU_BASELINE_B64'

# Unknown keys fail closed.
set +e
refusal_out="$(parse_repair_arguments --evidence nope "$(b64 x)" 2>&1)"
refusal_rc=$?
set -e
[[ "$refusal_rc" -ne 0 ]] || fail 'an unknown --evidence key must fail closed'
grep -q 'Unknown --evidence diagnostic section key: nope' <<<"$refusal_out" \
    || fail 'the unknown-key refusal must name the key'

# The boot -> boot-evidence remap: `boot` has no baseline consumer and fails
# closed (the extlinux decision lives in boot-evidence).
BOOT_EVIDENCE_BASELINE_B64=""
set +e
remap_out="$(parse_repair_arguments --evidence boot "$(b64 'x')" extlinux 2>&1)"
remap_rc=$?
set -e
[[ "$remap_rc" -ne 0 ]] || fail '--evidence boot must fail closed (no baseline consumer)'
grep -q 'Unknown --evidence diagnostic section key: boot' <<<"$remap_out" \
    || fail 'the --evidence boot refusal must name the section'

# Dispatch: supported keys route; unconsumed sections fail closed.
KERNEL_BASELINE_B64="$(b64 "$(printf '%s\n' \
    'Kernel running: 6.8.0-fake' \
    'Kernel latest: 6.8.0-fake' \
    'Kernel installed: 6.8.0-fake size=1024 pkgs=linux-image-6.8.0-fake')")"
parse_cached_evidence_section kernel
[[ "${KERNEL_INVENTORY_VERSIONS[*]}" == 6.8.0-fake ]] \
    || fail 'parse_cached_evidence_section kernel must rebuild the kernel inventory'
KERNEL_BASELINE_B64=""

REPORT_BASELINE_B64="$(b64 'Diagnostic: report')"
parse_cached_evidence_section report
[[ "$REPORT_BASELINE_TEXT" == 'Diagnostic: report' ]] \
    || fail 'parse_cached_evidence_section report must decode into REPORT_BASELINE_TEXT'
REPORT_BASELINE_B64=""

UKI_BASELINE_B64="$(b64 'Uki decision: layout=vendor-uki esp_source=/dev/sda1 esp_fstype=vfat ownership=target bootorder_lead=0001')"
parse_cached_evidence_section uki
[[ "$EFI_LOADER_LAYOUT" == vendor-uki ]] || fail "uki dispatch must rebuild EFI_LOADER_LAYOUT: $EFI_LOADER_LAYOUT"
UKI_BASELINE_B64=""

GRUB_BASELINE_B64="$(b64 'Grub decision: config=/boot/grub/grub.cfg layout=grub generator=/usr/sbin/grub-mkconfig grubenv=valid bootcode=not-applicable')"
parse_cached_evidence_section grub
[[ "$GRUB_CONFIG_PATH" == /boot/grub/grub.cfg ]] || fail "grub dispatch must rebuild GRUB_CONFIG_PATH: $GRUB_CONFIG_PATH"
GRUB_BASELINE_B64=""

BOOT_EVIDENCE_BASELINE_B64="$(b64 'Extlinux decision: config=/boot/syslinux/syslinux.cfg default=virt overwrite=1')"
parse_cached_evidence_section boot-evidence
[[ "$EXTLINUX_CONFIG_PATH" == /boot/syslinux/syslinux.cfg ]] \
    || fail "boot-evidence dispatch must rebuild EXTLINUX_CONFIG_PATH: $EXTLINUX_CONFIG_PATH"
BOOT_EVIDENCE_BASELINE_B64=""

set +e
refusal_out="$(parse_cached_evidence_section environment 2>&1)"
refusal_rc=$?
set -e
[[ "$refusal_rc" -ne 0 ]] || fail 'an unconsumed section must fail closed'
grep -q "No parser for diagnostic section 'environment'" <<<"$refusal_out" \
    || fail 'the no-parser refusal must name the section'

# ---------------------------------------------------------------------------
# (b/c) GPU baseline round-trip + fresh re-check + menu re-collect
# ---------------------------------------------------------------------------
(
    # ---- parse_cached_gpu_inventory round-trip (single device + mismatch) ----
    GPU_HARDWARE_DEVICES=("10de:24b0 (10de:24b0)")
    GPU_HARDWARE_DRIVERS=("nvidia")
    GPU_HARDWARE="10de:24b0 (10de:24b0)"
    GPU_HARDWARE_DRIVER_IN_USE="nvidia"
    GPU_HARDWARE_NOTE="recovery-host"
    GPU_LOADED_MODULES="nvidia"
    GPU_BLACKLISTED_MODULES="none"
    GPU_INSTALLED_PKGS="nvidia-driver@560.28.03, xserver-xorg-video-nouveau@1.0.17"
    GPU_EXPECTED="nvidia"
    GPU_EXPECTED_SOURCE="hardware-pciid"
    GPU_OEM_FLAVOUR="tuxedo"
    GPU_MISMATCH="nvidia vs nouveau"
    GPU_MISMATCH_REASON="expected nvidia but the installed driver is nouveau"
    GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=absent"
    GPU_JOURNAL_ERRORS="7"
    baseline_text="$(diagnostic_gpu)"
    GPU_BASELINE_B64="$(b64 "$baseline_text")"
    GPU_HARDWARE=""; GPU_HARDWARE_NOTE=""; GPU_HARDWARE_DRIVER_IN_USE=""
    GPU_HARDWARE_DEVICES=(); GPU_HARDWARE_DRIVERS=()
    GPU_LOADED_MODULES=""; GPU_BLACKLISTED_MODULES=""; GPU_INSTALLED_PKGS=""
    GPU_EXPECTED=""; GPU_EXPECTED_SOURCE=""; GPU_OEM_FLAVOUR=""
    GPU_MISMATCH=""; GPU_MISMATCH_REASON=""; GPU_DKMS_BUILD_STATUS=""
    GPU_JOURNAL_ERRORS="0"
    parse_cached_gpu_inventory
    [[ "${#GPU_HARDWARE_DEVICES[@]}" -eq 1 ]] || fail "parsed single-device count: ${#GPU_HARDWARE_DEVICES[@]}"
    [[ "${GPU_HARDWARE_DEVICES[0]}" == '10de:24b0 (10de:24b0)' ]] \
        || fail "parsed device: ${GPU_HARDWARE_DEVICES[0]}"
    [[ "$GPU_HARDWARE" == '10de:24b0 (10de:24b0)' ]] || fail "parsed GPU_HARDWARE: $GPU_HARDWARE"
    [[ "$GPU_HARDWARE_DRIVER_IN_USE" == nvidia ]] || fail "parsed driver-in-use: $GPU_HARDWARE_DRIVER_IN_USE"
    [[ "$GPU_HARDWARE_NOTE" == recovery-host ]] || fail "parsed note: $GPU_HARDWARE_NOTE"
    [[ "$GPU_EXPECTED" == nvidia ]] || fail "parsed expected: $GPU_EXPECTED"
    [[ "$GPU_EXPECTED_SOURCE" == hardware-pciid ]] || fail "parsed expected source: $GPU_EXPECTED_SOURCE"
    [[ "$GPU_MISMATCH" == 'nvidia vs nouveau' ]] || fail "parsed mismatch: $GPU_MISMATCH"
    [[ "$GPU_MISMATCH_REASON" == 'expected nvidia but the installed driver is nouveau' ]] \
        || fail "parsed mismatch reason: $GPU_MISMATCH_REASON"
    [[ "$GPU_DKMS_BUILD_STATUS" == '6.8.0-x=built, 6.5.0-old=absent' ]] \
        || fail "parsed DKMS build status: $GPU_DKMS_BUILD_STATUS"
    [[ "$GPU_JOURNAL_ERRORS" == 7 ]] || fail "parsed journal errors: $GPU_JOURNAL_ERRORS"
    GPU_BASELINE_B64=""

    # ---- round-trip: multi-device (every `GPU hardware:` line) --------------
    GPU_HARDWARE_DEVICES=("10de:24b0 (10de:24b0)" "8086:3e9b (1028:087d)")
    GPU_HARDWARE_DRIVERS=("nvidia" "i915")
    GPU_HARDWARE_NOTE="recovery-host"
    GPU_LOADED_MODULES="nvidia,i915"
    GPU_BLACKLISTED_MODULES="none"
    GPU_INSTALLED_PKGS="none"
    GPU_EXPECTED="nvidia"
    GPU_EXPECTED_SOURCE="hardware-pciid"
    GPU_OEM_FLAVOUR="none"
    GPU_MISMATCH=""
    GPU_MISMATCH_REASON=""
    GPU_DKMS_BUILD_STATUS="none"
    GPU_JOURNAL_ERRORS="0"
    baseline_text="$(diagnostic_gpu)"
    [[ "$(grep -c '^GPU hardware: ' <<<"$baseline_text")" == 2 ]] \
        || fail 'the diagnostic must emit one GPU hardware: line per device'
    GPU_BASELINE_B64="$(b64 "$baseline_text")"
    GPU_HARDWARE=""; GPU_HARDWARE_NOTE=""; GPU_HARDWARE_DRIVER_IN_USE=""
    GPU_HARDWARE_DEVICES=(); GPU_HARDWARE_DRIVERS=()
    GPU_LOADED_MODULES=""; GPU_BLACKLISTED_MODULES=""; GPU_INSTALLED_PKGS=""
    GPU_EXPECTED=""; GPU_EXPECTED_SOURCE=""; GPU_OEM_FLAVOUR=""
    GPU_MISMATCH=""; GPU_MISMATCH_REASON=""; GPU_DKMS_BUILD_STATUS=""
    GPU_JOURNAL_ERRORS="0"
    parse_cached_gpu_inventory
    [[ "${#GPU_HARDWARE_DEVICES[@]}" -eq 2 ]] || fail "parsed multi-device count: ${#GPU_HARDWARE_DEVICES[@]}"
    [[ "${GPU_HARDWARE_DEVICES[0]}" == '10de:24b0 (10de:24b0)' ]] \
        || fail "parsed multi-device first: ${GPU_HARDWARE_DEVICES[0]}"
    [[ "${GPU_HARDWARE_DEVICES[1]}" == '8086:3e9b (1028:087d)' ]] \
        || fail "parsed multi-device second: ${GPU_HARDWARE_DEVICES[1]}"
    [[ "${GPU_HARDWARE_DRIVERS[1]}" == i915 ]] \
        || fail "parsed multi-device second driver: ${GPU_HARDWARE_DRIVERS[1]}"
    [[ "$GPU_HARDWARE" == '10de:24b0 (10de:24b0)' ]] \
        || fail "parsed multi-device GPU_HARDWARE scalar: $GPU_HARDWARE"
    [[ "$GPU_HARDWARE_DRIVER_IN_USE" == nvidia ]] \
        || fail "parsed multi-device driver-in-use scalar: $GPU_HARDWARE_DRIVER_IN_USE"
    GPU_BASELINE_B64=""

    # ---- round-trip: none/target-only sentinels -----------------------------
    GPU_HARDWARE_DEVICES=()
    GPU_HARDWARE_DRIVERS=()
    GPU_HARDWARE=""
    GPU_HARDWARE_NOTE="target-only"
    GPU_LOADED_MODULES="none"
    GPU_BLACKLISTED_MODULES="none"
    GPU_INSTALLED_PKGS="none"
    GPU_EXPECTED=""
    GPU_EXPECTED_SOURCE=""
    GPU_OEM_FLAVOUR="none"
    GPU_MISMATCH=""
    GPU_MISMATCH_REASON=""
    GPU_DKMS_BUILD_STATUS="none"
    GPU_JOURNAL_ERRORS="0"
    baseline_text="$(diagnostic_gpu)"
    GPU_BASELINE_B64="$(b64 "$baseline_text")"
    GPU_HARDWARE="x"; GPU_HARDWARE_NOTE="x"; GPU_HARDWARE_DRIVER_IN_USE="x"
    GPU_HARDWARE_DEVICES=("x"); GPU_HARDWARE_DRIVERS=("x")
    GPU_LOADED_MODULES="x"; GPU_BLACKLISTED_MODULES="x"; GPU_INSTALLED_PKGS="x"
    GPU_EXPECTED="x"; GPU_EXPECTED_SOURCE="x"; GPU_OEM_FLAVOUR="x"
    GPU_MISMATCH="x"; GPU_MISMATCH_REASON="x"; GPU_DKMS_BUILD_STATUS="x"
    GPU_JOURNAL_ERRORS="99"
    parse_cached_gpu_inventory
    [[ "${#GPU_HARDWARE_DEVICES[@]}" -eq 0 ]] || fail "target-only must parse to no devices"
    [[ -z "$GPU_HARDWARE" ]] || fail "target-only must parse to empty GPU_HARDWARE: $GPU_HARDWARE"
    [[ "$GPU_HARDWARE_NOTE" == target-only ]] || fail "target-only note: $GPU_HARDWARE_NOTE"
    [[ -z "$GPU_EXPECTED" ]] || fail "none expected driver must parse empty: $GPU_EXPECTED"
    [[ -z "$GPU_MISMATCH" ]] || fail "none mismatch must parse empty: $GPU_MISMATCH"
    [[ "$GPU_INSTALLED_PKGS" == none ]] || fail "none installed pkgs: $GPU_INSTALLED_PKGS"
    GPU_BASELINE_B64=""

    # ---- fail-closed: absent / undecodable ----------------------------------
    GPU_BASELINE_B64=""
    set +e
    refusal_out="$(parse_cached_gpu_inventory 2>&1)"
    refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'a missing cached GPU baseline must fail closed'
    grep -q 'no cached GPU baseline' <<<"$refusal_out" \
        || fail 'the missing-baseline refusal must name the cached baseline'

    GPU_BASELINE_B64='!!!not-base64!!!'
    set +e
    refusal_out="$(parse_cached_gpu_inventory 2>&1)"
    refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'an undecodable cached GPU baseline must fail closed'
    grep -q 'could not be decoded' <<<"$refusal_out" \
        || fail 'the undecodable-baseline refusal must name the decode failure'
    GPU_BASELINE_B64=""

    # ---- fresh safety re-check: stale expected driver fails closed ----------
    GPU_BASELINE_B64="$(b64 "$(printf '%s\n' \
        'GPU hardware: 10de:24b0 (10de:24b0) [driver-in-use=nvidia]' \
        'GPU hardware note: recovery-host' \
        'GPU kernel module loaded: nvidia' \
        'GPU kernel module blacklisted: none' \
        'GPU installed driver packages: nvidia-driver@560.28.03' \
        'GPU expected driver: nvidia (source=hardware-pciid)' \
        'GPU OEM flavour: tuxedo' \
        'GPU driver mismatch: nvidia vs nouveau (reason=expected nvidia but the installed driver is nouveau)' \
        'GPU DKMS build status: 6.8.0-x=built, 6.5.0-old=absent' \
        'GPU journal errors: 7 actionable lines')")"
    gpu_hardware_evidence() { :; }
    gpu_installed_driver_packages() { :; }
    gpu_expected_driver() { GPU_EXPECTED="${FAKE_FRESH_EXPECTED:-}"; GPU_EXPECTED_SOURCE="stub-fresh"; }
    gpu_driver_mismatch_evidence() { :; }
    FAKE_FRESH_EXPECTED="amdgpu"
    set +e
    refusal_out="$(gpu_repair_prepare_evidence 2>&1)"
    refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'a stale expected driver must fail closed before mutation'
    grep -q 'expected GPU driver changed' <<<"$refusal_out" \
        || fail 'the stale-expected-driver refusal must name the changed driver'
    grep -q 'from nvidia to amdgpu' <<<"$refusal_out" \
        || fail 'the stale-expected-driver refusal must name both drivers'

    FAKE_FRESH_EXPECTED="nvidia"
    gpu_repair_prepare_evidence
    GPU_BASELINE_B64=""
    unset -f gpu_hardware_evidence gpu_installed_driver_packages \
        gpu_expected_driver gpu_driver_mismatch_evidence

    # ---- fresh safety re-check: no baseline -> fresh collect ----------------
    GPU_COLLECT_COUNT_FILE="$sandbox/gpu-collect-count.txt"
    printf '0' > "$GPU_COLLECT_COUNT_FILE"
    gpu_collect_evidence() {
        local n=0
        n="$(cat "$GPU_COLLECT_COUNT_FILE" 2>/dev/null || true)"
        n=$((n + 1))
        printf '%s' "$n" > "$GPU_COLLECT_COUNT_FILE"
        GPU_EVIDENCE_COLLECTED=1
    }
    GPU_BASELINE_B64=""
    gpu_repair_prepare_evidence
    [[ "$(cat "$GPU_COLLECT_COUNT_FILE")" -eq 1 ]] \
        || fail 'a no-baseline GPU preflight must collect evidence fresh'
    GPU_BASELINE_B64=""
    gpu_collect_evidence() { GPU_EVIDENCE_COLLECTED=1; profile_target_backends; }

    # ---- interactive menu: never re-collects before a mutation --------------
    GPU_BASELINE_B64="$(b64 "$(printf '%s\n' \
        'GPU hardware: 10de:24b0 (10de:24b0) [driver-in-use=nvidia]' \
        'GPU hardware note: recovery-host' \
        'GPU kernel module loaded: nvidia' \
        'GPU kernel module blacklisted: none' \
        'GPU installed driver packages: nvidia-driver@560.28.03, xserver-xorg-video-nouveau@1.0.17' \
        'GPU expected driver: nvidia (source=hardware-pciid)' \
        'GPU OEM flavour: tuxedo' \
        'GPU driver mismatch: nvidia vs nouveau (reason=expected nvidia but the installed driver is nouveau)' \
        'GPU DKMS build status: 6.8.0-x=built, 6.5.0-old=absent' \
        'GPU journal errors: 7 actionable lines')")"
    printf '0' > "$GPU_COLLECT_COUNT_FILE"
    gpu_collect_evidence() {
        local n=0
        n="$(cat "$GPU_COLLECT_COUNT_FILE" 2>/dev/null || true)"
        n=$((n + 1))
        printf '%s' "$n" > "$GPU_COLLECT_COUNT_FILE"
        GPU_EVIDENCE_COLLECTED=1
    }
    REPAIR_CHANGE_STATUS_COLLECT=0
    REPAIR_CHANGE_STATUS_COLLECTED=()
    BOOT_REPAIR_SESSION_PROTOCOL=1
    BOOT_REPAIR_SESSION_REQUEST=7
    NOC_PROMPT_FILE="$sandbox/gpu-nocollect-prompt.txt"
    NOC_PROMPT_COUNT_FILE="$sandbox/gpu-nocollect-prompt-count.txt"
    : > "$NOC_PROMPT_FILE"
    printf '0' > "$NOC_PROMPT_COUNT_FILE"
    NOC_ANSWERS=("99" "0")
    helper_menu_prompt() {
        local n=0
        n="$(cat "$NOC_PROMPT_COUNT_FILE" 2>/dev/null || true)"
        n=$((n + 1))
        printf '%s' "$n" > "$NOC_PROMPT_COUNT_FILE"
        printf '%s\n' "$1" >> "$NOC_PROMPT_FILE"
        if (( n <= ${#NOC_ANSWERS[@]} )); then
            HELPER_MENU_ANSWER="${NOC_ANSWERS[$((n - 1))]}"
        else
            HELPER_MENU_ANSWER="0"
        fi
        return 0
    }
    gpu_interactive_menu >/dev/null 2>&1 || true
    [[ "$(cat "$GPU_COLLECT_COUNT_FILE")" -eq 0 ]] \
        || fail 'the GPU menu must not re-collect before a mutation (invalid answer + abort)'
    [[ "$(cat "$NOC_PROMPT_COUNT_FILE")" -eq 2 ]] \
        || fail 'the GPU menu must re-prompt once after an invalid answer, then abort'
    grep -q 'driver mismatch: nvidia vs nouveau' "$NOC_PROMPT_FILE" \
        || fail 'the GPU menu summary must name the mismatch from the baseline'
    BOOT_REPAIR_SESSION_PROTOCOL=0
    GPU_BASELINE_B64=""

    # ---- interactive menu: re-collects fresh after a dispatched sub-action --
    GPU_BASELINE_B64="$(b64 "$(printf '%s\n' \
        'GPU hardware: 10de:24b0 (10de:24b0) [driver-in-use=nvidia]' \
        'GPU hardware note: recovery-host' \
        'GPU kernel module loaded: nvidia' \
        'GPU kernel module blacklisted: none' \
        'GPU installed driver packages: nvidia-driver@560.28.03, xserver-xorg-video-nouveau@1.0.17' \
        'GPU expected driver: nvidia (source=hardware-pciid)' \
        'GPU OEM flavour: tuxedo' \
        'GPU driver mismatch: nvidia vs nouveau (reason=expected nvidia but the installed driver is nouveau)' \
        'GPU DKMS build status: 6.8.0-x=built, 6.5.0-old=absent' \
        'GPU journal errors: 7 actionable lines')")"
    printf '0' > "$GPU_COLLECT_COUNT_FILE"
    gpu_collect_evidence() {
        local n=0
        n="$(cat "$GPU_COLLECT_COUNT_FILE" 2>/dev/null || true)"
        n=$((n + 1))
        printf '%s' "$n" > "$GPU_COLLECT_COUNT_FILE"
        GPU_MISMATCH=""
        GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=built"
        GPU_EVIDENCE_COLLECTED=1
    }
    repair_gpu_driver_swap() { :; }
    REPAIR_CHANGE_STATUS_COLLECT=0
    REPAIR_CHANGE_STATUS_COLLECTED=()
    BOOT_REPAIR_SESSION_PROTOCOL=1
    BOOT_REPAIR_SESSION_REQUEST=7
    SWAP_PROMPT_FILE="$sandbox/gpu-swap-prompt.txt"
    SWAP_PROMPT_COUNT_FILE="$sandbox/gpu-swap-prompt-count.txt"
    : > "$SWAP_PROMPT_FILE"
    printf '0' > "$SWAP_PROMPT_COUNT_FILE"
    helper_menu_prompt() {
        local n=0
        n="$(cat "$SWAP_PROMPT_COUNT_FILE" 2>/dev/null || true)"
        n=$((n + 1))
        printf '%s' "$n" > "$SWAP_PROMPT_COUNT_FILE"
        printf '%s\n' "$1" >> "$SWAP_PROMPT_FILE"
        HELPER_MENU_ANSWER="1"
        return 0
    }
    status_line="$(gpu_interactive_menu 2>/dev/null || true)"
    [[ "$(cat "$SWAP_PROMPT_COUNT_FILE")" -eq 1 ]] \
        || fail 'the mismatched GPU menu must prompt once for the driver swap'
    grep -q 'driver mismatch: nvidia vs nouveau' "$SWAP_PROMPT_FILE" \
        || fail 'the first GPU menu display must come from the baseline'
    [[ "$(cat "$GPU_COLLECT_COUNT_FILE")" -ge 1 ]] \
        || fail 'the GPU menu must re-collect fresh after the dispatched sub-action'
    grep -q '^Repair change status gpu: unchanged|reason:gpu-verified$' <<<"$status_line" \
        || fail 'the GPU menu must report gpu-verified after the simulated driver swap'
    BOOT_REPAIR_SESSION_PROTOCOL=0
    GPU_BASELINE_B64=""

    # ---- kernel-dependent actions route through repair_kernel_prepare_inventory
    ROUTE_LOG="$sandbox/gpu-kernel-route.log"
    : > "$ROUTE_LOG"
    repair_kernel_prepare_inventory() {
        printf 'prepare\n' >> "$ROUTE_LOG"
        KERNEL_REMOVABLE=()
    }
    kernel_inventory_collect() { printf 'collect\n' >> "$ROUTE_LOG"; }
    repair_change_status() { printf 'status %s\n' "$*" >> "$ROUTE_LOG"; }
    repair_gpu_remove_older_kernel
    grep -q 'prepare' "$ROUTE_LOG" \
        || fail 'repair_gpu_remove_older_kernel must route through repair_kernel_prepare_inventory'
    if grep -q 'collect' "$ROUTE_LOG"; then
        fail 'repair_gpu_remove_older_kernel must not call kernel_inventory_collect directly'
    fi
)

# ---------------------------------------------------------------------------
# (b) display baseline round-trip + poison-stub consumption
# ---------------------------------------------------------------------------
(
    # ---- parse_cached_display_inventory round-trip (systemd + OpenRC) -------
    DISPLAY_BASELINE_B64="$(b64 "$(printf '%s\n' \
        'Systemd default target:' \
        'graphical.target' \
        'Display manager decision: backend=systemd service=sddm.service package=sddm label=SDDM unit=/usr/lib/systemd/system/sddm.service')")"
    parse_cached_display_inventory
    [[ "$DISPLAY_MANAGER_BACKEND" == systemd ]] || fail "parsed display backend: $DISPLAY_MANAGER_BACKEND"
    [[ "$DISPLAY_MANAGER_SERVICE" == sddm.service ]] || fail "parsed display service: $DISPLAY_MANAGER_SERVICE"
    [[ "$DISPLAY_MANAGER_PACKAGE" == sddm ]] || fail "parsed display package: $DISPLAY_MANAGER_PACKAGE"
    [[ "$DISPLAY_MANAGER_LABEL" == SDDM ]] || fail "parsed display label: $DISPLAY_MANAGER_LABEL"
    [[ "$DISPLAY_MANAGER_UNIT_REL" == /usr/lib/systemd/system/sddm.service ]] \
        || fail "parsed display unit: $DISPLAY_MANAGER_UNIT_REL"
    DISPLAY_BASELINE_B64=""

    DISPLAY_BASELINE_B64="$(b64 'Display manager decision: backend=OpenRC service=gdm package=gdm label=GDM unit=/etc/init.d/gdm')"
    parse_cached_display_inventory
    [[ "$DISPLAY_MANAGER_BACKEND" == OpenRC ]] || fail "parsed OpenRC backend: $DISPLAY_MANAGER_BACKEND"
    [[ "$DISPLAY_MANAGER_SERVICE" == gdm ]] || fail "parsed OpenRC service: $DISPLAY_MANAGER_SERVICE"
    [[ "$DISPLAY_MANAGER_UNIT_REL" == /etc/init.d/gdm ]] || fail "parsed OpenRC unit: $DISPLAY_MANAGER_UNIT_REL"
    DISPLAY_BASELINE_B64=""

    # ---- fail-closed: absent / undecodable / empty / no-manager -------------
    DISPLAY_BASELINE_B64=""
    set +e
    refusal_out="$(parse_cached_display_inventory 2>&1)"
    refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'a missing cached display baseline must fail closed'
    grep -q 'no cached display baseline' <<<"$refusal_out" \
        || fail 'the missing-display-baseline refusal must name the baseline'

    DISPLAY_BASELINE_B64='!!!not-base64!!!'
    set +e
    refusal_out="$(parse_cached_display_inventory 2>&1)"
    refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'an undecodable cached display baseline must fail closed'
    grep -q 'could not be decoded' <<<"$refusal_out" \
        || fail 'the undecodable-display-baseline refusal must name the decode failure'
    DISPLAY_BASELINE_B64=""

    DISPLAY_BASELINE_B64="$(b64 'Display manager decision: backend=systemd service=none package=none label=none unit=none')"
    set +e
    refusal_out="$(parse_cached_display_inventory 2>&1)"
    refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'a no-manager display baseline must fail closed'
    grep -q 'resolves no configured display manager' <<<"$refusal_out" \
        || fail 'the no-manager refusal must be named'
    DISPLAY_BASELINE_B64=""

    # ---- dkms/initramfs consume the kernel baseline (no fresh re-collection)
    KERNEL_BASELINE_B64="$(b64 "$(printf '%s\n' \
        'Kernel running: 6.8.0-fake' \
        'Kernel latest: 6.8.0-fake' \
        'Kernel installed: 6.8.0-fake size=1024 pkgs=linux-image-6.8.0-fake')")"
    poison="$sandbox/kernel-poison.txt"
    printf '0' > "$poison"
    installed_kernel_versions() { printf '1' > "$poison"; printf '9.9.9-poison\n'; }
    arch_kernel_versions()     { printf '1' > "$poison"; printf '9.9.9-poison\n'; }
    alpine_kernel_pairs()      { printf '1' > "$poison"; printf '9.9.9-poison\n'; }
    rpm_kernel_pairs()         { printf '1' > "$poison"; printf '9.9.9-poison\n'; }
    running_kernel_version()   { printf '6.8.0-fake\n'; }
    mapfile -t vers < <(repair_kernel_versions installed)
    [[ "$(cat "$poison")" == 0 ]] \
        || fail 'repair_kernel_versions must not re-collect fresh when a baseline is present'
    [[ "${#vers[@]}" -eq 1 && "${vers[0]}" == 6.8.0-fake ]] \
        || fail "repair_kernel_versions must return the cached baseline versions: ${vers[*]}"
    KERNEL_BASELINE_B64=""
    unset -f installed_kernel_versions arch_kernel_versions alpine_kernel_pairs rpm_kernel_pairs running_kernel_version

    # ---- display repair consumes the cached display baseline ----------------
    DISPLAY_BASELINE_B64="$(b64 'Display manager decision: backend=systemd service=sddm.service package=sddm label=SDDM unit=/usr/lib/systemd/system/sddm.service')"
    touch "$fake_root/usr/lib/systemd/system/graphical.target"
    touch "$fake_root/usr/lib/systemd/system/sddm.service"
    dm_poison="$sandbox/display-poison.txt"
    printf '0' > "$dm_poison"
    detect_display_manager()       { printf '1' > "$dm_poison"; fail 'detect_display_manager must not run'; }
    target_display_manager_backend() { printf '1' > "$dm_poison"; printf 'systemd\n'; }
    resolve_display_manager()      { printf '1' > "$dm_poison"; return 1; }
    need()                          { :; }
    target_package_installed()      { return 0; }
    target_apt_ready()              { return 1; }
    run_selected_chroot()           { return 0; }
    trial_display_manager_headless() { :; }
    display_manager_already_correct() { return 1; }
    restore_display_manager()       { :; }
    status_line=""
    repair_change_status()          { status_line="$(printf '%s' "$*")"; }
    adaptive_display_manager_repair
    [[ "$(cat "$dm_poison")" == 0 ]] \
        || fail 'the display repair must not re-derive the selection when a baseline is present'
    [[ "$DISPLAY_MANAGER_SERVICE" == sddm.service ]] \
        || fail "the display repair must use the cached selection: $DISPLAY_MANAGER_SERVICE"
    [[ "$status_line" == "display changed" ]] \
        || fail "the display repair must report changed: $status_line"
    DISPLAY_BASELINE_B64=""

    # ---- display repair: correct config but the last boot stopped at a TTY --
    boot_failure_classification() { BOOT_REACHED='tty'; BOOT_FAILURE_MARKERS='tty-only'; BOOT_FAILURE_CLASSIFIED=1; }
    DISPLAY_BASELINE_B64="$(b64 'Display manager decision: backend=systemd service=sddm.service package=sddm label=SDDM unit=/usr/lib/systemd/system/sddm.service')"
    display_manager_already_correct() { return 0; }
    restore_display_manager() { :; }
    status_line=""
    adaptive_display_manager_repair
    [[ "$status_line" == "display unchanged|reason:display-already-correct-boot-tty" ]] \
        || fail "the display repair must report display-already-correct-boot-tty for a tty-only boot; got: $status_line"

    boot_failure_classification() { BOOT_REACHED='desktop'; BOOT_FAILURE_MARKERS=''; BOOT_FAILURE_CLASSIFIED=1; }
    status_line=""
    adaptive_display_manager_repair
    [[ "$status_line" == "display unchanged|reason:display-already-correct" ]] \
        || fail "the display repair must keep display-already-correct for a desktop boot; got: $status_line"
    DISPLAY_BASELINE_B64=""

    # ---- display diagnostic surfaces the shared boot classification ---------
    boot_failure_classification() { BOOT_REACHED='tty'; BOOT_FAILURE_MARKERS='tty-only'; BOOT_FAILURE_CLASSIFIED=1; }
    display_out="$(diagnostic_display)"
    grep -q '^Boot reached: tty$' <<<"$display_out" \
        || fail "diagnostic_display must emit 'Boot reached: tty'; got: $(grep 'Boot reached' <<<"$display_out")"
    grep -q '^Boot failure markers: tty-only$' <<<"$display_out" \
        || fail "diagnostic_display must emit 'Boot failure markers: tty-only'; got: $(grep 'Boot failure' <<<"$display_out")"
)

# ---------------------------------------------------------------------------
# (b) uki/grub baseline round-trip + poison-stub consumption
# ---------------------------------------------------------------------------
(
    # ---- parse_cached_uki_inventory round-trip ------------------------------
    UKI_BASELINE_B64="$(b64 'Uki decision: layout=alpine-efi-stub esp_source=/dev/sdb2 esp_fstype=vfat ownership=foreign bootorder_lead=001A')"
    parse_cached_uki_inventory
    [[ "$EFI_LOADER_LAYOUT" == alpine-efi-stub ]] || fail "parsed uki layout: $EFI_LOADER_LAYOUT"
    [[ "$EFI_ESP_SOURCE" == /dev/sdb2 ]] || fail "parsed uki esp source: $EFI_ESP_SOURCE"
    [[ "$EFI_ESP_FSTYPE" == vfat ]] || fail "parsed uki esp fstype: $EFI_ESP_FSTYPE"
    [[ "$EFI_ENTRY_OWNERSHIP" == foreign ]] || fail "parsed uki ownership: $EFI_ENTRY_OWNERSHIP"
    [[ "$EFI_BOOTORDER_LEAD" == 001A ]] || fail "parsed uki lead: $EFI_BOOTORDER_LEAD"
    UKI_BASELINE_B64=""

    # unresolved ESP + none ownership/lead stay non-fatal.
    UKI_BASELINE_B64="$(b64 'Uki decision: layout=generic-grub-efi esp_source=unresolved esp_fstype=unresolved ownership=none bootorder_lead=none')"
    parse_cached_uki_inventory
    [[ "$EFI_ESP_SOURCE" == "" ]] || fail "unresolved esp source must clear EFI_ESP_SOURCE: $EFI_ESP_SOURCE"
    [[ "$EFI_ENTRY_OWNERSHIP" == none ]] || fail "parsed uki ownership none: $EFI_ENTRY_OWNERSHIP"
    [[ "$EFI_BOOTORDER_LEAD" == none ]] || fail "parsed uki lead none: $EFI_BOOTORDER_LEAD"
    UKI_BASELINE_B64=""

    # ---- parse_cached_grub_inventory round-trip -----------------------------
    GRUB_BASELINE_B64="$(b64 'Grub decision: config=/boot/grub2/grub.cfg layout=grub2 generator=/usr/sbin/grub2-mkconfig grubenv=valid bootcode=broken')"
    parse_cached_grub_inventory
    [[ "$GRUB_CONFIG_PATH" == /boot/grub2/grub.cfg ]] || fail "parsed grub config: $GRUB_CONFIG_PATH"
    [[ "$GRUB_LAYOUT" == grub2 ]] || fail "parsed grub layout: $GRUB_LAYOUT"
    [[ "$GRUB_GENERATOR" == /usr/sbin/grub2-mkconfig ]] || fail "parsed grub generator: $GRUB_GENERATOR"
    [[ "$GRUB_ENV_VALID" == valid ]] || fail "parsed grub env: $GRUB_ENV_VALID"
    [[ "$GRUB_BOOT_CODE" == broken ]] || fail "parsed grub bootcode: $GRUB_BOOT_CODE"
    GRUB_BASELINE_B64=""

    # ---- fail-closed: absent / undecodable / empty / invalid ----------------
    UKI_BASELINE_B64=""
    set +e
    refusal_out="$(parse_cached_uki_inventory 2>&1)"; refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'a missing uki baseline must fail closed'
    grep -q 'no cached uki baseline' <<<"$refusal_out" \
        || fail 'the missing-uki-baseline refusal must name the baseline'

    UKI_BASELINE_B64='!!!not-base64!!!'
    set +e
    refusal_out="$(parse_cached_uki_inventory 2>&1)"; refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'an undecodable uki baseline must fail closed'
    grep -q 'could not be decoded' <<<"$refusal_out" \
        || fail 'the undecodable-uki-baseline refusal must name the decode failure'
    UKI_BASELINE_B64=""

    UKI_BASELINE_B64="$(b64 'Uki decision: layout=bogus')"
    set +e
    refusal_out="$(parse_cached_uki_inventory 2>&1)"; refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'an unknown uki layout must fail closed'
    grep -q 'unrecognised loader layout' <<<"$refusal_out" \
        || fail 'the unknown-uki-layout refusal must be named'
    UKI_BASELINE_B64=""

    GRUB_BASELINE_B64=""
    set +e
    refusal_out="$(parse_cached_grub_inventory 2>&1)"; refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'a missing grub baseline must fail closed'
    grep -q 'no cached grub baseline' <<<"$refusal_out" \
        || fail 'the missing-grub-baseline refusal must name the baseline'

    GRUB_BASELINE_B64="$(b64 'Grub decision: config=relative layout=grub generator=none grubenv=valid bootcode=ok')"
    set +e
    refusal_out="$(parse_cached_grub_inventory 2>&1)"; refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'a non-absolute grub config path must fail closed'
    grep -q 'no GRUB configuration path' <<<"$refusal_out" \
        || fail 'the missing-grub-config-path refusal must be named'
    GRUB_BASELINE_B64=""

    # ---- consumption: deterministic inputs come from the cache, not fresh ---
    EFI_LOADER_LAYOUT=""
    GRUB_CONFIG_PATH=""
    GRUB_LAYOUT=""
    GRUB_GENERATOR=""
    GRUB_BOOT_CODE=""
    UKI_BASELINE_B64="$(b64 'Uki decision: layout=vendor-uki esp_source=/dev/sda1 esp_fstype=vfat ownership=target bootorder_lead=0001')"
    poison="$sandbox/efi-poison.txt"
    printf '0' > "$poison"
    efi_loader_layout_classification() { printf '1' > "$poison"; printf 'generic-grub-efi\n'; }
    layout="$(resolve_efi_loader_layout)"
    [[ "$(cat "$poison")" == 0 ]] \
        || fail 'resolve_efi_loader_layout must not re-classify fresh when a baseline is present'
    [[ "$layout" == vendor-uki ]] \
        || fail "resolve_efi_loader_layout must return the cached layout: $layout"
    UKI_BASELINE_B64=""
    unset -f efi_loader_layout_classification

    GRUB_BASELINE_B64="$(b64 'Grub decision: config=/boot/grub2/grub.cfg layout=grub2 generator=/usr/sbin/grub2-mkconfig grubenv=valid bootcode=broken')"
    grub_poison="$sandbox/grub-poison.txt"
    printf '0' > "$grub_poison"
    grub2_layout_detected() { printf '1' > "$grub_poison"; return 1; }
    grub_config_path()     { printf '1' > "$grub_poison"; printf '/boot/grub/grub.cfg\n'; }
    grub_generator_tool()  { printf '1' > "$grub_poison"; printf '/usr/sbin/grub-mkconfig\n'; }
    fedora_grub_boot_code_broken() { printf '1' > "$grub_poison"; return 1; }
    [[ "$(grub_resolved_layout)" == grub2 ]] || fail 'grub_resolved_layout must return the cached layout'
    [[ "$(grub_resolved_config_path)" == /boot/grub2/grub.cfg ]] || fail 'grub_resolved_config_path must return the cached path'
    [[ "$(grub_resolved_generator)" == /usr/sbin/grub2-mkconfig ]] || fail 'grub_resolved_generator must return the cached generator'
    if grub_resolved_boot_code_broken; then
        :
    else
        fail 'grub_resolved_boot_code_broken must report broken from the cache'
    fi
    [[ "$(cat "$grub_poison")" == 0 ]] \
        || fail 'grub resolvers must not re-derive fresh when a baseline is present'
    GRUB_BASELINE_B64=""
    unset -f grub2_layout_detected grub_config_path grub_generator_tool fedora_grub_boot_code_broken
)

# ---------------------------------------------------------------------------
# (b/c) extlinux/boot-stack baseline: serialization + remap + round-trip
# ---------------------------------------------------------------------------
(
    # ---- boot-stack multi-baseline serialization ----------------------------
    BOOT_EVIDENCE_BASELINE_B64=""
    KERNEL_BASELINE_B64=""
    UKI_BASELINE_B64=""
    GRUB_BASELINE_B64=""
    parse_repair_arguments \
        --evidence boot-evidence "$(b64 'Extlinux decision: config=/boot/extlinux.conf default=lts overwrite=0')" \
        --evidence kernel "$(b64 'Kernel running: 6.8.0-fake')" \
        --evidence uki "$(b64 'Uki decision: layout=vendor-uki')" \
        --evidence grub "$(b64 'Grub decision: config=/boot/grub/grub.cfg layout=grub')" \
        boot-stack
    [[ "$BOOT_EVIDENCE_BASELINE_B64" == "$(b64 'Extlinux decision: config=/boot/extlinux.conf default=lts overwrite=0')" ]] \
        || fail '--evidence boot-evidence must populate BOOT_EVIDENCE_BASELINE_B64'
    [[ "$KERNEL_BASELINE_B64" == "$(b64 'Kernel running: 6.8.0-fake')" ]] \
        || fail '--evidence kernel must populate KERNEL_BASELINE_B64'
    [[ "$UKI_BASELINE_B64" == "$(b64 'Uki decision: layout=vendor-uki')" ]] \
        || fail '--evidence uki must populate UKI_BASELINE_B64'
    [[ "$GRUB_BASELINE_B64" == "$(b64 'Grub decision: config=/boot/grub/grub.cfg layout=grub')" ]] \
        || fail '--evidence grub must populate GRUB_BASELINE_B64'
    [[ "${REPAIR_STAGES[*]}" == boot-stack ]] \
        || fail 'repair stages must be parsed alongside the boot-stack baselines'

    # ---- parse_cached_extlinux_inventory round-trip -------------------------
    tab=$'\t'
    baseline_text="$(printf '%s\n' \
        'target extlinux configuration (/boot/extlinux.conf):' \
        'LABEL lts [default]' \
        '  LINUX vmlinuz-lts' \
        '' \
        'Extlinux entry keys (normalized):' \
        "LABEL${tab}lts" \
        "LINUX${tab}lts${tab}vmlinuz-lts" \
        "INITRD${tab}lts${tab}initramfs-lts" \
        '' \
        'Extlinux decision: config=/boot/extlinux.conf default=lts overwrite=0' \
        '')"
    BOOT_EVIDENCE_BASELINE_B64="$(b64 "$baseline_text")"
    parse_cached_extlinux_inventory
    [[ "$EXTLINUX_CONFIG_PATH" == /boot/extlinux.conf ]] \
        || fail "parsed extlinux config: $EXTLINUX_CONFIG_PATH"
    [[ "$EXTLINUX_DEFAULT" == lts ]] || fail "parsed extlinux default: $EXTLINUX_DEFAULT"
    [[ "$EXTLINUX_OVERWRITE" == 0 ]] || fail "parsed extlinux overwrite: $EXTLINUX_OVERWRITE"
    grep -Fqx "LABEL${tab}lts" <<<"$EXTLINUX_ENTRY_KEYS" \
        || fail "parsed extlinux LABEL key: $EXTLINUX_ENTRY_KEYS"
    grep -Fqx "LINUX${tab}lts${tab}vmlinuz-lts" <<<"$EXTLINUX_ENTRY_KEYS" \
        || fail "parsed extlinux LINUX key: $EXTLINUX_ENTRY_KEYS"
    grep -Fqx "INITRD${tab}lts${tab}initramfs-lts" <<<"$EXTLINUX_ENTRY_KEYS" \
        || fail "parsed extlinux INITRD key: $EXTLINUX_ENTRY_KEYS"
    BOOT_EVIDENCE_BASELINE_B64=""

    # The /boot/syslinux.cfg path and absent default=/overwrite= round-trip too.
    BOOT_EVIDENCE_BASELINE_B64="$(b64 'Extlinux decision: config=/boot/syslinux.cfg default=none overwrite=none')"
    parse_cached_extlinux_inventory
    [[ "$EXTLINUX_CONFIG_PATH" == /boot/syslinux.cfg ]] \
        || fail "parsed syslinux.cfg config: $EXTLINUX_CONFIG_PATH"
    [[ -z "$EXTLINUX_DEFAULT" ]] || fail "none default must clear EXTLINUX_DEFAULT: $EXTLINUX_DEFAULT"
    [[ -z "$EXTLINUX_OVERWRITE" ]] || fail "none overwrite must clear EXTLINUX_OVERWRITE: $EXTLINUX_OVERWRITE"
    [[ -z "$EXTLINUX_ENTRY_KEYS" ]] || fail "a decision-only baseline must carry no entry keys: $EXTLINUX_ENTRY_KEYS"
    BOOT_EVIDENCE_BASELINE_B64=""

    # ---- fail-closed: absent / undecodable / invalid ------------------------
    BOOT_EVIDENCE_BASELINE_B64=""
    set +e
    refusal_out="$(parse_cached_extlinux_inventory 2>&1)"; refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'a missing boot-evidence baseline must fail closed'
    grep -q 'no cached boot-evidence baseline' <<<"$refusal_out" \
        || fail 'the missing-boot-evidence-baseline refusal must name the baseline'

    BOOT_EVIDENCE_BASELINE_B64='!!!not-base64!!!'
    set +e
    refusal_out="$(parse_cached_extlinux_inventory 2>&1)"; refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'an undecodable boot-evidence baseline must fail closed'
    grep -q 'could not be decoded' <<<"$refusal_out" \
        || fail 'the undecodable-boot-evidence-baseline refusal must name the decode failure'
    BOOT_EVIDENCE_BASELINE_B64=""

    BOOT_EVIDENCE_BASELINE_B64="$(b64 'Extlinux decision: config=/boot/grub/grub.cfg default=lts overwrite=0')"
    set +e
    refusal_out="$(parse_cached_extlinux_inventory 2>&1)"; refusal_rc=$?
    set -e
    [[ "$refusal_rc" -ne 0 ]] || fail 'a non-extlinux config path must fail closed'
    grep -q 'no valid extlinux configuration path' <<<"$refusal_out" \
        || fail 'the invalid-path refusal must be named'
    BOOT_EVIDENCE_BASELINE_B64=""

    # ---- consumption: the resolved path comes from the cache, not fresh -----
    (
        BOOT_EVIDENCE_BASELINE_B64="$(b64 'Extlinux decision: config=/boot/syslinux/syslinux.cfg default=lts overwrite=0')"
        EXTLINUX_CONFIG_PATH=""
        poison="$sandbox/extlinux-poison.txt"
        printf '0' > "$poison"
        extlinux_config_path() { printf '1' > "$poison"; printf '%s\n' "$TARGET_ROOT/boot/extlinux.conf"; }
        resolved="$(extlinux_resolved_config_path)"
        [[ "$(cat "$poison")" == 0 ]] \
            || fail 'extlinux_resolved_config_path must not re-derive fresh when a baseline is present'
        [[ "$resolved" == "$TARGET_ROOT/boot/syslinux/syslinux.cfg" ]] \
            || fail "extlinux_resolved_config_path must return the cached path: $resolved"
    )

    # ---- extlinux_inventory_collect emits the same values the parser consumes
    cat > "$fake_root/boot/extlinux.conf" <<'EXTLINUX'
DEFAULT menu.c32
LABEL lts
  LINUX vmlinuz-lts
  INITRD initramfs-lts
EXTLINUX
    printf 'overwrite=1\ndefault=lts\n' > "$fake_root/etc/update-extlinux.conf"
    extlinux_inventory_collect
    [[ "$EXTLINUX_CONFIG_PATH" == /boot/extlinux.conf ]] \
        || fail "collected extlinux config: $EXTLINUX_CONFIG_PATH"
    [[ "$EXTLINUX_DEFAULT" == lts ]] || fail "collected extlinux default: $EXTLINUX_DEFAULT"
    [[ "$EXTLINUX_OVERWRITE" == 1 ]] || fail "collected extlinux overwrite: $EXTLINUX_OVERWRITE"
    grep -Fqx "LABEL${tab}lts" <<<"$EXTLINUX_ENTRY_KEYS" \
        || fail "collected extlinux LABEL key: $EXTLINUX_ENTRY_KEYS"
)

printf 'baseline-contract: PASS\n'
