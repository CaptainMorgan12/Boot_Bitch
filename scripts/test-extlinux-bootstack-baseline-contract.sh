#!/usr/bin/env bash
# Extlinux/boot-stack baseline evidence contract:
#   * the extlinux section remap: --evidence boot-evidence ->
#     BOOT_EVIDENCE_BASELINE_B64 -> parse_cached_extlinux_inventory round-trip
#     (resolved config path, default=/overwrite= and the guard-compatible
#     LABEL/LINUX/INITRD/KERNEL entry-key set), while --evidence boot stays
#     fail-closed (the extlinux decision lives in boot-evidence, not boot);
#   * the extlinux repair consumes the resolved config path from the baseline
#     while the fresh preflight and entry-preservation guard remain;
#   * the boot-stack multi-baseline serialization: one repair request carrying
#     --evidence boot-evidence ... --evidence kernel ... --evidence uki ...
#     --evidence grub ... populates every component baseline.
#
# Covers the helper side only; the GUI-side required-sections remap and the
# run-time serialization are asserted by the MainWindow UI suite
# (requiredDiagnosticSectionsMapping, bootstackSerializesComponentBaselines).
# No real block device, chroot or package manager is touched.
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
    parse_cached_extlinux_inventory extlinux_resolved_config_path \
    extlinux_inventory_collect extlinux_configured_overwrite \
    extlinux_config_path extlinux_entry_keys preflight_extlinux \
    adaptive_extlinux_repair guard_extlinux_candidate_preserves_entries; do
    grep -q "^${fn}()" "$HELPER" || fail "helper is missing ${fn}()"
done

# --evidence boot-evidence maps to BOOT_EVIDENCE_BASELINE_B64 and dispatches.
grep -q 'BOOT_EVIDENCE_BASELINE_B64=""' "$HELPER" \
    || fail 'parse_repair_arguments must reset BOOT_EVIDENCE_BASELINE_B64'
grep -qE 'boot-evidence\)[[:space:]]+BOOT_EVIDENCE_BASELINE_B64' "$HELPER" \
    || fail 'store_evidence_baseline must map boot-evidence to BOOT_EVIDENCE_BASELINE_B64'
grep -qE 'boot-evidence\)[[:space:]]+parse_cached_extlinux_inventory' "$HELPER" \
    || fail 'parse_cached_evidence_section must route boot-evidence'
# The remap: `boot` is a valid diagnostic section but must have no baseline
# consumer, so --evidence boot stays fail-closed.
if grep -qE '[[:space:]]boot\)[[:space:]]+(BOOT_EVIDENCE_BASELINE_B64|parse_cached_)' "$HELPER"; then
    fail 'store_evidence_baseline/parse_cached_evidence_section must not map the raw boot section'
fi

# The boot-evidence diagnostic emits the decision line and the entry-key block.
grep -q 'echo "Extlinux decision:' "$HELPER" \
    || fail 'diagnostic_boot_evidence must emit the Extlinux decision: line'
grep -q 'echo "Extlinux entry keys (normalized):' "$HELPER" \
    || fail 'diagnostic_boot_evidence must emit the normalized extlinux entry-key set'

# The extlinux repair routes on the resolved (baseline-aware) config path and
# keeps the fresh preflight and entry-preservation guard.
grep -A40 '^adaptive_extlinux_repair()' "$HELPER" | grep -q 'extlinux_resolved_config_path' \
    || fail 'adaptive_extlinux_repair must route through extlinux_resolved_config_path'
grep -A40 '^adaptive_extlinux_repair()' "$HELPER" | grep -q 'preflight_extlinux' \
    || fail 'adaptive_extlinux_repair must keep the fresh preflight_extlinux'
grep -A40 '^adaptive_extlinux_repair()' "$HELPER" | grep -q 'guard_extlinux_candidate_preserves_entries' \
    || fail 'adaptive_extlinux_repair must keep the fresh entry-preservation guard'

# ---------------------------------------------------------------------------
# Behavioural harness
# ---------------------------------------------------------------------------
sandbox="$(mktemp -d)"
cleanup_sandbox() { rm -rf -- "$sandbox"; }
trap cleanup_sandbox EXIT

fake_root="$sandbox/root"
mkdir -p "$fake_root/boot" "$fake_root/etc"

source <(sed '/^main "$@"/d' "$HELPER")
trap - EXIT INT TERM HUP

profile_target_backends() {
    TARGET_DISTRO_FAMILY=alpine
    TARGET_BOOTLOADER_BACKEND=syslinux/extlinux
    TARGET_ESP_MOUNT=unresolved
    TARGET_OS_ID=""
}

TARGET_ROOT="$fake_root"
RUNNING_HOST_MODE=0
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"

b64() { printf '%s' "$1" | base64 | tr -d '\n'; }

# ---- --evidence parsing + dispatch -------------------------------------------
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

# The remap: --evidence boot (the raw boot section) has no consumer and fails
# closed, exactly as before the extlinux decision moved to boot-evidence.
BOOT_EVIDENCE_BASELINE_B64=""
set +e
remap_out="$(parse_repair_arguments --evidence boot "$(b64 'x')" extlinux 2>&1)"
remap_rc=$?
set -e
[[ "$remap_rc" -ne 0 ]] || fail '--evidence boot must fail closed (no baseline consumer)'
grep -q 'Unknown --evidence diagnostic section key: boot' <<<"$remap_out" \
    || fail 'the --evidence boot refusal must name the section'

# Dispatch: boot-evidence routes to the extlinux parser.
BOOT_EVIDENCE_BASELINE_B64="$(b64 'Extlinux decision: config=/boot/syslinux/syslinux.cfg default=virt overwrite=1')"
parse_cached_evidence_section boot-evidence
[[ "$EXTLINUX_CONFIG_PATH" == /boot/syslinux/syslinux.cfg ]] \
    || fail "boot-evidence dispatch must rebuild EXTLINUX_CONFIG_PATH: $EXTLINUX_CONFIG_PATH"
BOOT_EVIDENCE_BASELINE_B64=""

# ---- parse_cached_extlinux_inventory round-trip ------------------------------
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

# ---- fail-closed: absent / undecodable / empty / invalid ---------------------
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

BOOT_EVIDENCE_BASELINE_B64="$(b64 'Extlinux decision: config=/boot/extlinux.conf default=lts overwrite=0')"
set +e
# A valid absolute but non-extlinux path must fail closed.
BOOT_EVIDENCE_BASELINE_B64="$(b64 'Extlinux decision: config=/boot/grub/grub.cfg default=lts overwrite=0')"
refusal_out="$(parse_cached_extlinux_inventory 2>&1)"; refusal_rc=$?
set -e
[[ "$refusal_rc" -ne 0 ]] || fail 'a non-extlinux config path must fail closed'
grep -q 'no valid extlinux configuration path' <<<"$refusal_out" \
    || fail 'the invalid-path refusal must be named'
BOOT_EVIDENCE_BASELINE_B64=""

# ---- consumption: the resolved path comes from the cache, not fresh ----------
# Scoped in a subshell so the poison redefinition of extlinux_config_path never
# leaks into the following extlinux_inventory_collect check.
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

# ---- extlinux_inventory_collect emits the same values the parser consumes ----
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

printf 'All extlinux/boot-stack baseline contract checks passed.\n'
