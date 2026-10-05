#!/usr/bin/env bash
# Multi-baseline evidence contract: --evidence <section> <base64> parsing and
# dispatch, the display baseline (`display` diagnostic -> DISPLAY_BASELINE_B64 ->
# parse_cached_display_inventory), and the dkms/initramfs/display consumers'
# baseline reuse (with a poison stub proving no fresh re-collection).
#
# Covers the helper side of the wave-1 multi-baseline foundation.  The GUI-side
# required-sections mapping and the popup-alert-on-missing-cache path are
# covered by the MainWindow UI suite (requiredDiagnosticSectionsMapping and
# repairToolRefusesRunWhenRequiredDiagnosticMissing).
#
# Static wiring plus a behavioural harness that sources the helper without main
# and stubs only the read-only target probes and the chroot/package-manager
# transcript plumbing.  No real block device, package manager or chroot is ever
# touched.
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
for fn in store_evidence_baseline parse_cached_evidence_section \
    parse_cached_display_inventory repair_kernel_versions \
    resolve_display_manager resolve_alpine_display_manager; do
    grep -q "^${fn}()" "$HELPER" || fail "helper is missing ${fn}()"
done

# The --evidence flag is accepted by the repair arg parser; the display and
# report baseline state variables exist and are reset by parse_repair_arguments.
grep -q -- '--evidence)' "$HELPER" \
    || fail 'the --evidence baseline flag is not parsed'
grep -q 'DISPLAY_BASELINE_B64=""' "$HELPER" \
    || fail 'parse_repair_arguments must reset DISPLAY_BASELINE_B64'
grep -q 'REPORT_BASELINE_B64=""' "$HELPER" \
    || fail 'parse_repair_arguments must reset REPORT_BASELINE_B64'

# The display diagnostic emits the machine-parseable decision line in both the
# systemd and OpenRC paths (expanded diagnostic, never re-derived in the repair).
grep -q 'echo "Display manager decision:' "$HELPER" \
    || fail 'the display diagnostic must emit a Display manager decision: line'
[[ "$(grep -c 'Display manager decision:' "$HELPER")" -ge 2 ]] \
    || fail 'both diagnostic_display and diagnostic_display_openrc must emit the decision line'

# dkms and initramfs enumerate through the baseline-aware helper, never the
# fresh backend probes directly.
for fn in preflight_dkms preflight_initramfs adaptive_initramfs_repair; do
    grep -A40 "^${fn}()" "$HELPER" | grep -q 'repair_kernel_versions' \
        || fail "${fn} must enumerate through repair_kernel_versions"
done

# The display repair consumes the cached baseline backend, not a fresh probe.
grep -A8 '^adaptive_display_manager_repair()' "$HELPER" \
    | grep -q 'parse_cached_display_inventory' \
    || fail 'adaptive_display_manager_repair must parse the cached display baseline'

# ---------------------------------------------------------------------------
# Behavioural harness
# ---------------------------------------------------------------------------
sandbox="$(mktemp -d)"
cleanup_sandbox() { rm -rf -- "$sandbox"; }
trap cleanup_sandbox EXIT

fake_root="$sandbox/root"
mkdir -p "$fake_root/etc/modprobe.d" "$fake_root/boot" "$fake_root/usr/bin" \
    "$fake_root/usr/sbin" "$fake_root/var/lib/dpkg" "$fake_root/etc/apt" \
    "$fake_root/usr/lib/systemd/system" "$fake_root/lib/modules"

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

# ---- --evidence parsing + dispatch -------------------------------------------
KERNEL_BASELINE_B64=""
GPU_BASELINE_B64=""
DISPLAY_BASELINE_B64=""
REPORT_BASELINE_B64=""
parse_repair_arguments \
    --evidence kernel "$(b64 'Kernel running: 6.8.0-fake')" \
    --evidence gpu "$(b64 'GPU hardware: 10de:24b0')" \
    --evidence display "$(b64 'Display manager decision: backend=systemd service=sddm.service package=sddm label=SDDM unit=/usr/lib/systemd/system/sddm.service')" \
    --evidence report "$(b64 'Diagnostic: report')" \
    dkms
[[ "$KERNEL_BASELINE_B64" == "$(b64 'Kernel running: 6.8.0-fake')" ]] \
    || fail '--evidence kernel must populate KERNEL_BASELINE_B64'
[[ "$GPU_BASELINE_B64" == "$(b64 'GPU hardware: 10de:24b0')" ]] \
    || fail '--evidence gpu must populate GPU_BASELINE_B64'
[[ "$DISPLAY_BASELINE_B64" == "$(b64 'Display manager decision: backend=systemd service=sddm.service package=sddm label=SDDM unit=/usr/lib/systemd/system/sddm.service')" ]] \
    || fail '--evidence display must populate DISPLAY_BASELINE_B64'
[[ "$REPORT_BASELINE_B64" == "$(b64 'Diagnostic: report')" ]] \
    || fail '--evidence report must populate REPORT_BASELINE_B64'
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

set +e
refusal_out="$(parse_cached_evidence_section environment 2>&1)"
refusal_rc=$?
set -e
[[ "$refusal_rc" -ne 0 ]] || fail 'an unconsumed section must fail closed'
grep -q "No parser for diagnostic section 'environment'" <<<"$refusal_out" \
    || fail 'the no-parser refusal must name the section'

# ---- parse_cached_display_inventory round-trip (systemd + OpenRC) ------------
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

# Fail closed: absent / undecodable / empty / no-manager.
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

# ---- dkms/initramfs consume the kernel baseline (no fresh re-collection) -----
# Poison every fresh probe; with a baseline repair_kernel_versions must come from
# the cached inventory and never touch the poison marker.
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

# ---- display repair consumes the cached display baseline ---------------------
# With a display baseline the repair resolves the backend/service from the cache
# and never calls the fresh probes (poisoned).  The preflight re-verifies the
# resolved selection against the (fake) target filesystem.
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

printf 'All display-baseline contract checks passed.\n'
