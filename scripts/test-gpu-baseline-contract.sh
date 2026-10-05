#!/usr/bin/env bash
# GPU baseline (`gpu` diagnostic -> --gpu-baseline -> parse_cached_gpu_inventory)
# helper contract.  The interactive `gpu` repair menu consumes the cached `gpu`
# diagnostic section as its baseline (passed from the GUI via --gpu-baseline) so
# the inventory is computed once in the diagnostic, and only re-collects fresh
# after a sub-action mutates state; every driver-decision mutation re-derives the
# safety-critical expected driver fresh and fails closed on divergence, and the
# kernel-dependent GPU actions route through repair_kernel_prepare_inventory
# (which reuses the cached `kernel` section passed via --baseline).
#
# Static wiring plus a behavioural harness that sources the helper without main
# and stubs only the read-only target probes and the package-manager/bootloader
# transcript plumbing.  No real block device, package manager, GPU or DKMS is
# ever touched.
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
for fn in parse_cached_gpu_inventory gpu_repair_prepare_evidence \
    repair_gpu_purge_wrong_driver repair_gpu_install_correct_driver \
    repair_gpu_blacklist_module repair_gpu_rebuild_dkms repair_gpu_rebuild_boot \
    repair_gpu_remove_older_kernel repair_gpu_replace_kernel \
    repair_kernel_prepare_inventory; do
    grep -q "^${fn}()" "$HELPER" || fail "helper is missing ${fn}()"
done

# The --gpu-baseline flag is accepted by the repair arg parser and its state
# variable exists alongside the kernel --baseline state.
grep -q -- '--gpu-baseline)' "$HELPER" \
    || fail 'the --gpu-baseline GPU baseline mode hint is not parsed'
grep -q 'GPU_BASELINE_B64=' "$HELPER" \
    || fail 'the gpu baseline state variable is missing'
grep -q 'GPU_BASELINE_B64=""' "$HELPER" \
    || fail 'parse_repair_arguments must reset GPU_BASELINE_B64'
grep -q -- '--baseline)' "$HELPER" \
    || fail 'the --baseline kernel baseline mode hint is not parsed'

# The diagnostic emits every class-03 device, one `GPU hardware:` line each.
grep -q 'for i in "${!GPU_HARDWARE_DEVICES\[@\]}"' "$HELPER" \
    || fail 'diagnostic_gpu must iterate GPU_HARDWARE_DEVICES to emit every device'

# Every driver-decision mutation action opens with the fresh safety re-check.
for fn in repair_gpu_purge_wrong_driver repair_gpu_install_correct_driver \
    repair_gpu_blacklist_module repair_gpu_rebuild_dkms repair_gpu_rebuild_boot; do
    grep -A2 "^${fn}()" "$HELPER" | grep -q 'gpu_repair_prepare_evidence' \
        || fail "${fn} must re-run the fresh safety re-check via gpu_repair_prepare_evidence"
done

# The kernel-dependent GPU actions route through repair_kernel_prepare_inventory
# (never a bare kernel_inventory_collect).
for fn in repair_gpu_remove_older_kernel repair_gpu_replace_kernel; do
    grep -A3 "^${fn}()" "$HELPER" | grep -q 'repair_kernel_prepare_inventory' \
        || fail "${fn} must route through repair_kernel_prepare_inventory"
done

# The parse must fail closed on absent/undecodable/empty baselines.
grep -q 'no cached GPU baseline' "$HELPER" \
    || fail 'the missing-baseline refusal message is missing'
grep -q 'could not be decoded' "$HELPER" \
    || fail 'the undecodable-baseline refusal message is missing'
grep -q 'is empty' "$HELPER" \
    || fail 'the empty-baseline refusal message is missing'
# The fresh expected-driver divergence refusal must name the changed driver.
grep -q 'expected GPU driver changed' "$HELPER" \
    || fail 'the stale-expected-driver refusal message is missing'

# The GPU stack must no longer re-derive "how far boot reached": the shared
# boot_failure_classification (emitted by the errors diagnostic) is the single
# source of truth, and the GPU gate consumes BOOT_REACHED instead.
grep -q '^boot_failure_classification()' "$HELPER" \
    || fail 'the shared boot_failure_classification() classifier is missing'
if grep -q '^gpu_boot_reached()' "$HELPER"; then
    fail 'gpu_boot_reached() must be removed; the GPU gate consumes boot_failure_classification'
fi
grep -q "GPU_BOOT_REACHED" "$HELPER" \
    && fail 'GPU_BOOT_REACHED global must be removed' || true
grep -q 'GPU boot reached:' "$HELPER" \
    && fail 'the GPU diagnostic must no longer emit a GPU boot reached: line' || true

# ---------------------------------------------------------------------------
# Behavioural harness
# ---------------------------------------------------------------------------
sandbox="$(mktemp -d)"
cleanup_sandbox() { rm -rf -- "$sandbox"; }
trap cleanup_sandbox EXIT

fake_root="$sandbox/root"
mkdir -p "$fake_root/etc/modprobe.d" "$fake_root/boot" "$fake_root/usr/bin" \
    "$fake_root/var/lib/dpkg" "$fake_root/etc/apt"

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
# The diagnostic/gate consume the fixture globals, never real probes.
gpu_collect_evidence() { GPU_EVIDENCE_COLLECTED=1; profile_target_backends; }

TARGET_ROOT="$fake_root"
RUNNING_HOST_MODE=0
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"

# Build a base64 baseline from literal diagnostic section text.
b64() { printf '%s' "$1" | base64 | tr -d '\n'; }

# ---- parse_cached_gpu_inventory round-trip (single device + mismatch) --------
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
# Wipe every GPU global so the parse is provably rebuilding from the baseline.
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

# ---- round-trip: multi-device (every `GPU hardware:` line) -------------------
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

# ---- round-trip: none/target-only sentinels + no hardware/mismatch lines -----
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

# ---- fail-closed: absent / undecodable / empty ------------------------------
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

# ---- fresh safety re-check: stale expected driver fails closed ---------------
# The baseline expects nvidia; the then-current target state derives amdgpu, so
# the mutation must refuse before any wrong-driver purge.
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
# Stub the fresh re-check probes so only the expected-driver decision differs.
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

# A fresh expected driver that still matches the baseline proceeds.
FAKE_FRESH_EXPECTED="nvidia"
gpu_repair_prepare_evidence
GPU_BASELINE_B64=""
unset -f gpu_hardware_evidence gpu_installed_driver_packages \
    gpu_expected_driver gpu_driver_mismatch_evidence

# ---- fresh safety re-check: no baseline -> fresh collect --------------------
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
# Restore the default no-op collect stub for the menu tests.
gpu_collect_evidence() { GPU_EVIDENCE_COLLECTED=1; profile_target_backends; }

# ---- interactive menu: never re-collects before a mutation ------------------
# The first display and any re-display after an invalid answer re-parse the
# cached baseline (no fresh collect); only a dispatched sub-action clears the
# baseline so the FOLLOWING display re-collects fresh.
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

# ---- interactive menu: re-collects fresh after a dispatched sub-action -------
# A driver swap (simulated) leaves a consistent stack; the next iteration
# re-collects fresh (not the stale baseline) and reports gpu-verified.
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

# ---- kernel-dependent actions route through repair_kernel_prepare_inventory --
# With no baseline, repair_kernel_prepare_inventory falls back to a fresh
# kernel_inventory_collect; the GPU kernel actions must never call it directly.
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

echo 'gpu-baseline-contract: PASS'
