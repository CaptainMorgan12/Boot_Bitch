#!/usr/bin/env bash
# UKI/GRUB baseline evidence contract: --evidence uki|grub parsing and dispatch,
# the `uki` diagnostic -> UKI_BASELINE_B64 -> parse_cached_uki_inventory
# round-trip, the `grub` diagnostic -> GRUB_BASELINE_B64 ->
# parse_cached_grub_inventory round-trip, and the repair consumers' baseline
# reuse (with poison stubs proving the deterministic inputs come from the cache
# while the fresh guards remain).
#
# Covers the helper side of the efi/grub single-source-of-truth migration.  No
# real block device, chroot or package manager is touched.
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
    parse_cached_uki_inventory parse_cached_grub_inventory \
    efi_loader_layout_classification resolve_efi_loader_layout \
    grub_resolved_layout grub_resolved_config_path grub_resolved_generator \
    grub_resolved_boot_code_broken; do
    grep -q "^${fn}()" "$HELPER" || fail "helper is missing ${fn}()"
done

# --evidence accepts uki and grub; the state variables are reset.
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

# The diagnostics emit the machine-parseable decision lines the parsers consume.
grep -q 'echo "Uki decision:' "$HELPER" \
    || fail 'diagnostic_uki must emit the Uki decision: line'
grep -q 'echo "Grub decision:' "$HELPER" \
    || fail 'diagnostic_grub must emit the Grub decision: line'
grep -q 'Grub menu entry keys (normalized):' "$HELPER" \
    || fail 'diagnostic_grub must emit the normalized menu-entry key set'

# The EFI repair routes on the resolved (baseline-aware) loader layout.
grep -q 'resolve_efi_loader_layout' <<<"$(sed -n '/^reinstall_efi_bootloader()/,/^}/p' "$HELPER")" \
    || fail 'reinstall_efi_bootloader must route through resolve_efi_loader_layout'
grep -q 'parse_cached_uki_inventory' <<<"$(sed -n '/^rebuild_tuxedo_uki()/,/^}/p' "$HELPER")" \
    || fail 'rebuild_tuxedo_uki must consume the cached uki baseline'
# The fresh guards remain inside the EFI repair (never replaced by the baseline).
reinstall_body="$(sed -n '/^reinstall_efi_bootloader()/,/^}/p' "$HELPER")"
for guard in validate_selected_esp validate_efi_bootloader_target \
    conventional_efi_backup_state conventional_efi_restore_backup \
    efi_reconcile_firmware_inventory; do
    grep -q "$guard" <<<"$reinstall_body" \
        || fail "reinstall_efi_bootloader must keep the fresh guard $guard"
done
grep -q 'validate_tuxedo_uki_target' <<<"$(sed -n '/^rebuild_tuxedo_uki()/,/^}/p' "$HELPER")" \
    || fail 'rebuild_tuxedo_uki must keep validate_tuxedo_uki_target fresh'

# The GRUB repairs consume the resolvers while the live guards stay.
for fn in adaptive_grub_stage adaptive_grub_repair adaptive_fedora_grub_repair; do
    grep -qE 'grub_resolved_(layout|config_path|generator|boot_code_broken)' \
        <<<"$(sed -n "/^${fn}()/,/^}/p" "$HELPER")" \
        || fail "${fn} must consume the resolved grub baseline"
done
grep -q 'guard_grub_candidate_preserves_entries' <<<"$(sed -n '/^adaptive_grub_repair()/,/^}/p' "$HELPER")" \
    || fail 'adaptive_grub_repair must keep the live entry-preservation guard'
grep -q 'fedora_grub_reinstall_boot_code' <<<"$(sed -n '/^adaptive_fedora_grub_repair()/,/^}/p' "$HELPER")" \
    || fail 'adaptive_fedora_grub_repair must keep the fresh boot-code reinstall'
# The fresh guards remain inside adaptive_grub_repair even with the baseline
# consumption (trial generation + live entry guard).
grep -q 'preflight_grub' <<<"$(sed -n '/^adaptive_grub_repair()/,/^}/p' "$HELPER")" \
    || fail 'adaptive_grub_repair must keep the trial preflight_grub'

# ---------------------------------------------------------------------------
# Behavioural harness
# ---------------------------------------------------------------------------
sandbox="$(mktemp -d)"
cleanup_sandbox() { rm -rf -- "$sandbox"; }
trap cleanup_sandbox EXIT

fake_root="$sandbox/root"
mkdir -p "$fake_root/boot" "$fake_root/usr/bin" "$fake_root/usr/sbin"

source <(sed '/^main "$@"/d' "$HELPER")
trap - EXIT INT TERM HUP

profile_target_backends() {
    TARGET_DISTRO_FAMILY=debian
    TARGET_BOOTLOADER_BACKEND=grub
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
UKI_BASELINE_B64=""
GRUB_BASELINE_B64=""
parse_repair_arguments \
    --evidence uki "$(b64 'Uki decision: layout=vendor-uki esp_source=/dev/sda1 esp_fstype=vfat ownership=target bootorder_lead=0001')" \
    --evidence grub "$(b64 'Grub decision: config=/boot/grub/grub.cfg layout=grub generator=/usr/sbin/grub-mkconfig grubenv=valid bootcode=not-applicable')" \
    efi
[[ "$UKI_BASELINE_B64" == "$(b64 'Uki decision: layout=vendor-uki esp_source=/dev/sda1 esp_fstype=vfat ownership=target bootorder_lead=0001')" ]] \
    || fail '--evidence uki must populate UKI_BASELINE_B64'
[[ "$GRUB_BASELINE_B64" == "$(b64 'Grub decision: config=/boot/grub/grub.cfg layout=grub generator=/usr/sbin/grub-mkconfig grubenv=valid bootcode=not-applicable')" ]] \
    || fail '--evidence grub must populate GRUB_BASELINE_B64'
[[ "${REPAIR_STAGES[*]}" == efi ]] || fail 'repair stages must be parsed alongside --evidence'

# Dispatch: supported keys route (the --evidence values from the parse above).
parse_cached_evidence_section uki
[[ "$EFI_LOADER_LAYOUT" == vendor-uki ]] || fail "uki dispatch must rebuild EFI_LOADER_LAYOUT: $EFI_LOADER_LAYOUT"
parse_cached_evidence_section grub
[[ "$GRUB_CONFIG_PATH" == /boot/grub/grub.cfg ]] || fail "grub dispatch must rebuild GRUB_CONFIG_PATH: $GRUB_CONFIG_PATH"

# ---- parse_cached_uki_inventory round-trip -----------------------------------
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

# ---- parse_cached_grub_inventory round-trip ----------------------------------
GRUB_BASELINE_B64="$(b64 'Grub decision: config=/boot/grub2/grub.cfg layout=grub2 generator=/usr/sbin/grub2-mkconfig grubenv=valid bootcode=broken')"
parse_cached_grub_inventory
[[ "$GRUB_CONFIG_PATH" == /boot/grub2/grub.cfg ]] || fail "parsed grub config: $GRUB_CONFIG_PATH"
[[ "$GRUB_LAYOUT" == grub2 ]] || fail "parsed grub layout: $GRUB_LAYOUT"
[[ "$GRUB_GENERATOR" == /usr/sbin/grub2-mkconfig ]] || fail "parsed grub generator: $GRUB_GENERATOR"
[[ "$GRUB_ENV_VALID" == valid ]] || fail "parsed grub env: $GRUB_ENV_VALID"
[[ "$GRUB_BOOT_CODE" == broken ]] || fail "parsed grub bootcode: $GRUB_BOOT_CODE"
GRUB_BASELINE_B64=""

# ---- fail-closed: absent / undecodable / empty / invalid ---------------------
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

# ---- consumption: deterministic inputs come from the cache, not fresh --------
# Simulate the parse_repair_arguments reset so the resolver re-parses the cache.
EFI_LOADER_LAYOUT=""
GRUB_CONFIG_PATH=""
GRUB_LAYOUT=""
GRUB_GENERATOR=""
GRUB_BOOT_CODE=""
# uki: resolve_efi_loader_layout must return the cached layout without touching
# the fresh classifier (poisoned).
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

# grub: the resolvers must return the cached values without touching the fresh
# probes (poisoned).
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

printf 'All uki/grub baseline contract checks passed.\n'
