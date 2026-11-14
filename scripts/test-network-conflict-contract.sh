#!/usr/bin/env bash
# Contract test for the network/input driver-conflict machinery (the
# driver-fallback ladder step 9): the conflict collectors' pair-derivation
# shapes (including the round-169 comma class and the round-96
# PCI-scoped/live-verified semantics) and the resolve actions' refusal and
# switch semantics.  Registered round 170; the wider ladder fixtures (the
# menu, the user-artifact path, the map rows) stay in the unregistered
# round-93 throwaway drill, this contract covers the conflict class itself.
# Lean, host-independent: the helper is sourced dynamically, every
# probe/mutation is stubbed, no real devices or repairs.
#
# The helper under test is sourced dynamically and its globals are set
# directly, so ShellCheck cannot track their use.
# shellcheck disable=SC1090,SC2034,SC2317
set -Eeuo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="${HELPER:-$ROOT_DIR/scripts/boot-bitch-helper.sh}"
[[ -x "$HELPER" ]] || { echo "FAIL: helper is not executable" >&2; exit 1; }
bash -n "$HELPER"

fail_test() { printf 'FAIL: %s\n' "$*" >&2; exit 1; }
pass() { printf '  ok: %s\n' "$*"; }

sandbox="$(mktemp -d "${TMPDIR:-/tmp}/boot-bitch-network-conflict.XXXXXX")"
trap 'rm -rf -- "$sandbox"' EXIT
source <(sed '/^main "$@"/d' "$HELPER")
trap 'rm -rf -- "$sandbox"' EXIT

# Set AFTER the source: the helper's top-level init resets SESSION_* and
# TARGET_ROOT to empty, so the harness values must be applied afterwards.
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
TARGET_ROOT="$sandbox/root"
mkdir -p "$SESSION_DIR" "$TARGET_ROOT/etc/modprobe.d"
: > "$SESSION_LOG"

# ---------------------------------------------------------------------------
# Shared stubs: every live probe and every mutation is faked (the harness
# discipline — fast, bounded, no real devices or writes).
# ---------------------------------------------------------------------------
FAKE_LIVE_MODULES=""
FAKE_REFCOUNT_MODULES=""
FAKE_MODPROBE_RC=0
FAKE_MODPROBE_R=""
FAKE_IP_OUTPUT=""
MODPROBE_LOG="$sandbox/modprobe.log"
: > "$MODPROBE_LOG"
module_live_loaded() { grep -qw "$1" <<<"$FAKE_LIVE_MODULES"; }
module_refcnt_positive() { grep -qw "$1" <<<"$FAKE_REFCOUNT_MODULES"; }
journal_cancel_requested() { return 1; }
modprobe() {
    if [[ "$*" == "-R "* ]]; then printf '%s\n' "$FAKE_MODPROBE_R"; return 0; fi
    printf '%s\n' "modprobe $*" >> "$MODPROBE_LOG"
    return "$FAKE_MODPROBE_RC"
}
write_module_blacklist() {
    local mod="$1"
    local conf="$TARGET_ROOT/etc/modprobe.d/blacklist-${mod}.conf"
    mkdir -p -- "$(dirname -- "$conf")"
    printf 'blacklist %s\n' "$mod" > "$conf"
    return 0
}
boot_initramfs_modules_write() { printf '1\n'; }
adaptive_initramfs_repair() { return 0; }
ip() { printf '%s\n' "$FAKE_IP_OUTPUT"; }
# The host-shape gates that read the live keyboard stack: pinned so the
# contract is deterministic on any runner (a real keyboard is env-dependent).
legacy_host_kernel_shape() { return 1; }
input_ps2_keyboard_stack_live() { return 1; }

RUNNING_HOST_MODE=1
NETWORK_PROBE_FAILURES="none"
NETWORK_CANDIDATE_MODULES="none"
NETWORK_BOUND_MODULES=""
NETWORK_DRIVER_CONFLICT="none"
NETWORK_DRIVER_CONFLICT_NOTE=""
NETWORK_KNOWN_CONFLICT_PAIRS="r8168|r8169"

# ---------------------------------------------------------------------------
# A. network_conflict_collect: the pair-derivation shapes.
# ---------------------------------------------------------------------------
# A1: the lspci bound-vs-first pair (pair-alone evidence, nothing loaded).
NETWORK_PCI_DEVICES=("10ec:8168 [class=ethernet, driver-in-use=r8169, modules=r8168 r8169]")
network_conflict_collect
[[ "$NETWORK_DRIVER_CONFLICT" == "r8169 vs r8168" ]] \
    || fail_test "A1: the lspci bound-vs-first pair must derive the conflict: [$NETWORK_DRIVER_CONFLICT]"
# A2: a passthrough stub (vfio-pci) is never a conflict pair.
NETWORK_PCI_DEVICES=("10ec:8168 [class=ethernet, driver-in-use=vfio-pci, modules=r8168 r8169]")
network_conflict_collect
[[ "$NETWORK_DRIVER_CONFLICT" == "none" ]] \
    || fail_test "A2: a non-candidate bound driver must not fire: [$NETWORK_DRIVER_CONFLICT]"
# A3: the sysfs bind/refcnt definitive shape (both held, refcnt > 0).
NETWORK_PCI_DEVICES=("10ec:8168 [class=ethernet, driver-in-use=r8169, modules=r8168 r8169]")
FAKE_LIVE_MODULES="r8168 r8169"
FAKE_REFCOUNT_MODULES="r8168 r8169"
network_conflict_collect
[[ "$NETWORK_DRIVER_CONFLICT" == "r8169 vs r8168" ]] \
    || fail_test "A3: the sysfs refcnt shape must derive the conflict: [$NETWORK_DRIVER_CONFLICT]"
# A4 (round 169, the comma class — the round-168 rig finding): the RAW lspci
# "Kernel modules:" list is "mod1, mod2" — the pre-fix first-token split kept
# the trailing comma and emitted "r8169 vs r8168,".
NETWORK_PROBE_FAILURES="none"
NETWORK_CANDIDATE_MODULES="none"
NETWORK_BOUND_MODULES=""
FAKE_LIVE_MODULES=""
FAKE_REFCOUNT_MODULES=""
NETWORK_PCI_DEVICES=("10ec:8168 [class=ethernet, driver-in-use=r8169, modules=r8168, r8169]")
network_conflict_collect
[[ "$NETWORK_DRIVER_CONFLICT" == "r8169 vs r8168" ]] \
    || fail_test "A4: the raw-lspci comma list must derive the CLEAN pair: [$NETWORK_DRIVER_CONFLICT]"
# A5: the bound module IS the first candidate (the healthy shape — the
# round-168 Fedora diag0 note).  The comma must NOT fabricate a self-conflict.
NETWORK_PCI_DEVICES=("10ec:8139 [class=ethernet, driver-in-use=8139cp, modules=8139cp, 8139too]")
network_conflict_collect
[[ "$NETWORK_DRIVER_CONFLICT" == "none" ]] \
    || fail_test "A5: a bound-first candidate list must not derive a conflict: [$NETWORK_DRIVER_CONFLICT]"
# A6: the bounded modprobe -R resolution-order disagreement.
NETWORK_PCI_DEVICES=("8086:125c [class=ethernet, driver-in-use=igc, modules=igb igc]")
FAKE_MODPROBE_R=$'igb\nigc'
network_conflict_collect
[[ "$NETWORK_DRIVER_CONFLICT" == "igc vs igb" ]] \
    || fail_test "A6: the modprobe -R disagreement must derive the conflict: [$NETWORK_DRIVER_CONFLICT]"
# A7: the EBUSY (-16) shape with a multi-module PCI candidate set.
NETWORK_PCI_DEVICES=("10ec:8168 [class=ethernet, driver-in-use=r8168, modules=r8168 r8169]")
NETWORK_PROBE_FAILURES="r8168=16"
FAKE_MODPROBE_R=""
network_conflict_collect
[[ "$NETWORK_DRIVER_CONFLICT" == "r8168 vs r8169" ]] \
    || fail_test "A7: the EBUSY multi-alias shape must derive the conflict: [$NETWORK_DRIVER_CONFLICT]"
# A8: a non-16 probe failure must NOT fire.
NETWORK_PROBE_FAILURES="r8168=6"
network_conflict_collect
[[ "$NETWORK_DRIVER_CONFLICT" == "none" ]] \
    || fail_test "A8: a non-EBUSY probe failure is not a conflict: [$NETWORK_DRIVER_CONFLICT]"
# A9: the both-loaded cross-check (scoped to ONE device's own list).
NETWORK_PROBE_FAILURES="none"
NETWORK_BOUND_MODULES="r8169"
NETWORK_PCI_DEVICES=("10ec:8168 [class=ethernet, driver-in-use=r8169, modules=r8168 r8169]")
FAKE_LIVE_MODULES="r8168 r8169"
FAKE_REFCOUNT_MODULES="r8168 r8169"
network_conflict_collect
[[ "$NETWORK_DRIVER_CONFLICT" == "r8169 vs r8168" ]] \
    || fail_test "A9: the both-loaded cross-check must derive the conflict: [$NETWORK_DRIVER_CONFLICT]"
pass "A: network_conflict_collect (lspci pair, refcnt, comma class, modprobe -R, EBUSY, both-loaded)"

# ---------------------------------------------------------------------------
# B. input_conflict_collect: the EBUSY shape, the refcnt cross-check and the
#    round-96 live-verified override.
# ---------------------------------------------------------------------------
INPUT_PROBE_FAILURES="none"
INPUT_CANDIDATE_MODULES="none"
INPUT_DEVICES=()
INPUT_DEVICE_BUS=()
INPUT_DEVICE_CLASS=()
INPUT_DEVICE_DRIVER=()
INPUT_DRIVER_CONFLICT="none"
INPUT_DRIVER_CONFLICT_NOTE=""
# B1: EBUSY with the family-derived candidate (no live devices -> the verdict
# is NOT downgraded by the live-verified override).
INPUT_PROBE_FAILURES="hid_samsung=16"
INPUT_CANDIDATE_MODULES="hid_samsung,hid_generic"
input_conflict_collect
[[ "$INPUT_DRIVER_CONFLICT" == "hid_samsung vs hid_generic" ]] \
    || fail_test "B1: the input EBUSY shape must derive the conflict: [$INPUT_DRIVER_CONFLICT]"
# B2: the host refcnt cross-check — round 96: a working keyboard with a bound
# driver is LIVE-VERIFIED, so the derived pair is downgraded to the note.
INPUT_PROBE_FAILURES="none"
INPUT_DEVICES=("Vendor HID keyboard")
INPUT_DEVICE_BUS=("usb")
INPUT_DEVICE_CLASS=("keyboard")
INPUT_DEVICE_DRIVER=("hid_samsung")
FAKE_REFCOUNT_MODULES="hid_samsung hid_generic"
input_conflict_collect
[[ "$INPUT_DRIVER_CONFLICT" == "none" ]] \
    || fail_test "B2: the live-verified override must hold: [$INPUT_DRIVER_CONFLICT]"
[[ "$INPUT_DRIVER_CONFLICT_NOTE" == "hid_samsung vs hid_generic" ]] \
    || fail_test "B2: the refcnt cross-check must derive the pair into the note: [$INPUT_DRIVER_CONFLICT_NOTE]"
pass "B: input_conflict_collect (EBUSY shape + the refcnt cross-check + the live-verified override)"

# ---------------------------------------------------------------------------
# C. the evidence emission + the cached round-trip (the machine-parsed lines).
# ---------------------------------------------------------------------------
# Network: the conflict line emits and round-trips.
NETWORK_DRIVER_CONFLICT="r8169 vs r8168"
NETWORK_CONFIG_BACKEND="ifupdown"; NETWORK_CONFIG_NOTE=""
NETWORK_ADAPTERS=("enp1s0")
NETWORK_ADAPTER_STATES=("state=up, carrier=1, type=1, kind=physical, driver=r8169, module=r8169, bus=pci, id=10ec:8168")
NETWORK_BOOTLOG_ADAPTERS=(); NETWORK_PCI_DEVICES=(); NETWORK_PCI_MODULES_NOTE=""
NETWORK_LOADED_MODULES="none"; NETWORK_BUILTIN_MODULES="none"
NETWORK_BLACKLISTED_MODULES="none"; NETWORK_INITRAMFS_INCLUSION="none-detected"
NETWORK_FIRMWARE="none"; NETWORK_FIRMWARE_MISSING="none"
NETWORK_DEFAULT_ROUTE="none"; NETWORK_ADDRESSES="none"; NETWORK_DNS_CHAIN="plain file"
NETWORK_DNS_SERVERS="1.1.1.1"; NETWORK_LEASE_GATEWAY="none"; NETWORK_REACHABILITY="unknown"
NETWORK_JOURNAL_COUNT="0"; NETWORK_DRIVER_BUILD_SYSTEM="none"
NETWORK_MODULE_VERSIONS="none"; NETWORK_PACKAGE_VERSIONS="none"
NETWORK_CANDIDATE_MODULES="r8168,r8169"; NETWORK_CANDIDATE_MODULES_NOTE=""
NETWORK_AVAILABLE_MODULES="r8168,r8169"
NETWORK_EVIDENCE_COLLECTED=1
NETWORK_FAILURE_CLASS="verified"
network_failure_classification
[[ "$NETWORK_FAILURE_CLASS" == "driver-conflict" ]] \
    || fail_test "C: the classifier must map the conflict to driver-conflict: [$NETWORK_FAILURE_CLASS]"
nsection="$(diagnostic_network)"
grep -q '^Network driver conflict: r8169 vs r8168$' <<<"$nsection" \
    || fail_test "C: the network diagnostic must emit the conflict line: [$nsection]"
NETWORK_BASELINE_B64="$(printf '%s' "$nsection" | base64 | tr -d '\n')"
NETWORK_DRIVER_CONFLICT="none"
parse_cached_network_inventory
[[ "$NETWORK_DRIVER_CONFLICT" == "r8169 vs r8168" ]] \
    || fail_test "C: the network conflict line must round-trip: [$NETWORK_DRIVER_CONFLICT]"
[[ "$NETWORK_FAILURE_CLASS" == "driver-conflict" ]] \
    || fail_test "C: the round-tripped network class must stay driver-conflict: [$NETWORK_FAILURE_CLASS]"
NETWORK_BASELINE_B64=""
# Input: the conflict line emits and round-trips.
INPUT_DRIVER_CONFLICT="hid_samsung vs hid_generic"
INPUT_VERDICT="driver-conflict"
INPUT_LOADED_MODULES="none"; INPUT_BUILTIN_MODULES="none"
INPUT_AVAILABLE_MODULES="usbhid,hid_generic,hid_samsung"; INPUT_BLACKLISTED_MODULES="none"
INPUT_BUS_CONTROLLER="none"; INPUT_INITRAMFS_INCLUSION="none-detected"
INPUT_INITRAMFS_PROOF="running-host: live keyboard evidence applies"; INPUT_MODULE_OWNERSHIP=""
INPUT_JOURNAL_COUNT="1"; INPUT_JOURNAL_CLASSES="usbhid"; INPUT_FIRMWARE_FAILURES="none"
INPUT_QUIRK_STATE="none"; INPUT_LUKS_EXPECTED="no"
INPUT_DRIVER_BUILD_SYSTEM="none"; INPUT_MODULE_VERSIONS="none"; INPUT_PACKAGE_VERSIONS="none"
INPUT_CANDIDATE_MODULES="hid_samsung,hid_generic"
INPUT_DEVICES=(); INPUT_DEVICE_BUS=(); INPUT_DEVICE_CLASS=(); INPUT_DEVICE_DRIVER=()
INPUT_BOOTLOG_DEVICES=(); INPUT_LAYOUT="us"; INPUT_EVIDENCE_COLLECTED=1
isection="$(diagnostic_input)"
grep -q '^Input driver conflict: hid_samsung vs hid_generic$' <<<"$isection" \
    || fail_test "C: the input diagnostic must emit the conflict line: [$isection]"
INPUT_BASELINE_B64="$(printf '%s' "$isection" | base64 | tr -d '\n')"
INPUT_DRIVER_CONFLICT="none"
parse_cached_input_inventory
[[ "$INPUT_DRIVER_CONFLICT" == "hid_samsung vs hid_generic" ]] \
    || fail_test "C: the input conflict line must round-trip: [$INPUT_DRIVER_CONFLICT]"
INPUT_BASELINE_B64=""
pass "C: the evidence emission + the cached round-trip (both sections)"

# ---------------------------------------------------------------------------
# D. network_repair_resolve_conflict: the uplink refusal, the host swap, the
#    config-only target path and the refusals.
# ---------------------------------------------------------------------------
RUNNING_HOST_MODE=1
NETWORK_DRIVER_CONFLICT="r8169 vs r8168"
NETWORK_AVAILABLE_MODULES="r8168,r8169"
NETWORK_BUILTIN_MODULES="none"
NETWORK_BOUND_MODULES="r8169"
NETWORK_REACHABILITY="blocked"
FAKE_LIVE_MODULES="r8169"
FAKE_REFCOUNT_MODULES=""
# D1: the conflicting module serves the live uplink -> refused.
FAKE_IP_OUTPUT='default via 192.0.2.1 dev enp1s0 proto dhcp'
readlink() { [[ "$1" == /sys/class/net/enp1s0/device/driver/module ]] && printf '/sys/module/r8169'; return 1; }
status="$(network_repair_resolve_conflict 2>/dev/null | grep -E '^Repair change status' || true)"
printf '%s\n' "$status" | grep -q '^Repair change status network:resolve-conflict: unchanged|reason:network-conflict-would-kill-uplink|param:r8169$' \
    || fail_test "D1: the uplink must refuse the swap: [$status]"
# D2: not the uplink -> the runtime swap + blacklist + inclusion apply.
FAKE_IP_OUTPUT='default via 192.0.2.1 dev enp2s0 proto dhcp'
readlink() { [[ "$1" == /sys/class/net/enp2s0/device/driver/module ]] && printf '/sys/module/igc'; return 1; }
: > "$MODPROBE_LOG"
rm -f "$TARGET_ROOT/etc/modprobe.d/blacklist-r8169.conf"
status="$(network_repair_resolve_conflict 2>/dev/null | grep -E '^Repair change status' || true)"
printf '%s\n' "$status" | grep -q '^Repair change status network:resolve-conflict: changed$' \
    || fail_test "D2: the host swap + config must report changed: [$status]"
grep -q '^modprobe -r r8169$' "$MODPROBE_LOG" \
    || fail_test "D2: the runtime swap must unload the conflicting module: [$(cat "$MODPROBE_LOG")]"
grep -q '^modprobe r8168$' "$MODPROBE_LOG" \
    || fail_test "D2: the runtime swap must load the candidate: [$(cat "$MODPROBE_LOG")]"
grep -q '^blacklist r8169$' "$TARGET_ROOT/etc/modprobe.d/blacklist-r8169.conf" \
    || fail_test "D2: the blacklist-one write must land"
# D3: the config-only target path (no modprobe, still the blacklist + inclusion).
RUNNING_HOST_MODE=0
unset -f readlink
: > "$MODPROBE_LOG"
rm -f "$TARGET_ROOT/etc/modprobe.d/blacklist-r8169.conf"
status="$(network_repair_resolve_conflict 2>/dev/null | grep -E '^Repair change status' || true)"
printf '%s\n' "$status" | grep -q '^Repair change status network:resolve-conflict: changed$' \
    || fail_test "D3: the config-only target path must report changed: [$status]"
[[ ! -s "$MODPROBE_LOG" ]] || fail_test "D3: a target must never modprobe: [$(cat "$MODPROBE_LOG")]"
# D4: the candidate not available/builtin -> refused.
NETWORK_AVAILABLE_MODULES="r8169"
NETWORK_BUILTIN_MODULES="none"
status="$(network_repair_resolve_conflict 2>/dev/null | grep -E '^Repair change status' || true)"
printf '%s\n' "$status" | grep -q '^Repair change status network:resolve-conflict: unchanged|reason:replacement-module-unavailable|param:r8168$' \
    || fail_test "D4: an unavailable candidate must be refused: [$status]"
# D5: no conflict evidence -> the honest reason.
NETWORK_DRIVER_CONFLICT="none"
status="$(network_repair_resolve_conflict 2>/dev/null | grep -E '^Repair change status' || true)"
printf '%s\n' "$status" | grep -q '^Repair change status network:resolve-conflict: unchanged|reason:no-driver-conflict$' \
    || fail_test "D5: no conflict -> no-driver-conflict: [$status]"
pass "D: network_repair_resolve_conflict (uplink refusal, host swap, config-only, refusals)"

# ---------------------------------------------------------------------------
# E. input_repair_resolve_conflict: the only-live-input refusal + the swap
#    (round 96/169: the live-verified override gates every keyboard-class
#    shape, so the refusal and the swap shapes are MOUSE-class).
# ---------------------------------------------------------------------------
RUNNING_HOST_MODE=1
INPUT_DRIVER_CONFLICT="psmouse vs hid_generic"
INPUT_AVAILABLE_MODULES="usbhid,hid_generic,hid_samsung,psmouse"
INPUT_BUILTIN_MODULES="none"
FAKE_LIVE_MODULES="psmouse"
# E1: psmouse is the ONLY live input module -> refused.
INPUT_DEVICES=("PS/2 mouse")
INPUT_DEVICE_BUS=("ps2"); INPUT_DEVICE_CLASS=("mouse"); INPUT_DEVICE_DRIVER=("psmouse")
status="$(input_repair_resolve_conflict 2>/dev/null | grep -E '^Repair change status' || true)"
printf '%s\n' "$status" | grep -q '^Repair change status input-devices:resolve-conflict: unchanged|reason:input-blacklist-would-kill-input|param:psmouse$' \
    || fail_test "E1: the only-live-input module must be refused: [$status]"
# E2: another live input module serves the input -> the swap applies.
INPUT_DEVICES=("PS/2 mouse" "USB mouse")
INPUT_DEVICE_BUS=("ps2" "usb"); INPUT_DEVICE_CLASS=("mouse" "mouse")
INPUT_DEVICE_DRIVER=("psmouse" "hid_generic")
FAKE_LIVE_MODULES="psmouse hid_generic"
: > "$MODPROBE_LOG"
rm -f "$TARGET_ROOT/etc/modprobe.d/blacklist-psmouse.conf"
status="$(input_repair_resolve_conflict 2>/dev/null | grep -E '^Repair change status' || true)"
printf '%s\n' "$status" | grep -q '^Repair change status input-devices:resolve-conflict: changed$' \
    || fail_test "E2: the input swap must apply: [$status]"
grep -q '^modprobe -r psmouse$' "$MODPROBE_LOG" \
    || fail_test "E2: the input swap must unload the conflicting module: [$(cat "$MODPROBE_LOG")]"
pass "E: input_repair_resolve_conflict (only-live-input refusal + swap)"

echo "PASS: network-conflict contract (collectors, comma class, live-verified override, refusals, swaps)"
