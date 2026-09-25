#!/usr/bin/env bash
# Fast contract for the legacy (Debian Etch / bash 3.1) helper port.
#
# Covers: deterministic drift gate (legacy/port.sh --check), absence of bash-4
# syntax in the generated helper, the modern helper staying untransformed,
# compat shim behaviour, and evidence-based legacy feature gating.
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
LEGACY_DIR="$ROOT_DIR/legacy"
HELPER="$LEGACY_DIR/boot-repair-helper.sh"
MODERN="$ROOT_DIR/scripts/boot-repair-helper.sh"
PORT="$LEGACY_DIR/port.sh"
COMPAT="$LEGACY_DIR/compat.sh"
OVERLAY="$LEGACY_DIR/overlay.sh"

fail()
{
    printf 'FAIL: %s\n' "$*" >&2
    exit 1
}

pass()
{
    printf 'ok - %s\n' "$*"
}

echo "== legacy port contract =="

# --- syntax -----------------------------------------------------------------
for file in "$MODERN" "$COMPAT" "$OVERLAY" "$PORT" "$HELPER"; do
    bash -n "$file" || fail "bash -n failed: ${file#"$ROOT_DIR"/}"
done
pass "bash -n (modern, compat, overlay, port, generated)"

# Optional real bash 3.1 parse check. Point BOOT_REPAIR_LEGACY_BASH31 at an
# extracted Etch /bin/bash (for example the cached bash_3.1dfsg-8 deb) to
# prove the generated helper parses on the target interpreter.
if [[ -n "${BOOT_REPAIR_LEGACY_BASH31:-}" ]]; then
    [[ -x "$BOOT_REPAIR_LEGACY_BASH31" ]] \
        || fail "BOOT_REPAIR_LEGACY_BASH31 is not executable: $BOOT_REPAIR_LEGACY_BASH31"
    "$BOOT_REPAIR_LEGACY_BASH31" -n "$HELPER" \
        || fail "bash 3.1 could not parse $HELPER"
    pass "bash 3.1 parse (BOOT_REPAIR_LEGACY_BASH31)"
fi

# The generated file carries the file-wide ShellCheck waiver for the
# transform-induced dynamic-array/case-conversion warnings; the port sources
# stay clean.  Set BOOT_REPAIR_LEGACY_SHELLCHECK=1 to run the full check (about
# 12 s, kept out of the default fast path).
grep -q '^# shellcheck disable=SC2034,SC2120,SC2154,SC2155$' "$HELPER" \
    || fail "generated helper is missing the file-wide shellcheck directive"
if [[ -n "${BOOT_REPAIR_LEGACY_SHELLCHECK:-}" ]]; then
    command -v shellcheck >/dev/null 2>&1 \
        || fail "BOOT_REPAIR_LEGACY_SHELLCHECK is set but shellcheck is not installed"
    shellcheck -S warning -s bash "$COMPAT" "$OVERLAY" "$PORT" "$HELPER" \
        || fail "shellcheck -S warning reported findings"
    pass "shellcheck -S warning (compat, overlay, port, generated)"
fi

# --- the modern helper must stay untransformed ------------------------------
grep -q 'mapfile -t' "$MODERN" || fail "modern helper lost its mapfile call sites"
grep -q 'declare -A FS_SCOPE_DEVICE_INDEX' "$MODERN" || fail "modern helper lost its associative array"
grep -q 'grub-mkconfig' "$MODERN" || fail "modern helper lost grub-mkconfig"
if grep -q 'legacy_readarray\|Legacy compatibility prelude' "$MODERN"; then
    fail "modern helper contains legacy port artefacts"
fi
pass "modern helper untouched"

# --- deterministic drift gate ----------------------------------------------
check_out="$(bash "$PORT" --check)" || {
    printf '%s\n' "$check_out" >&2
    fail "legacy/port.sh --check reports drift"
}
printf '%s\n' "$check_out" | grep -q 'in sync' || fail "port --check did not report in sync"
for rule in 'assoc declarations' 'assoc accesses: reasons' 'case conversion ,,' \
    'case conversion ^^' 'case conversion ^' 'mapfile call sites' 'sed -nE' \
    'sed -E' 'sort -V' 'date --iso-8601' 'os-release gates' 'dpkg db:Status sites' \
    '=~ regex literal hoists' 'wrapped/replaced modern functions' 'prelude' 'overlay'; do
    printf '%s\n' "$check_out" | grep -qF "$rule" \
        || fail "port summary is missing transformation: $rule"
done
printf '%s\n' "$check_out" | grep -qE 'mapfile call sites +58' \
    || fail "port summary mapfile count is not 58"
printf '%s\n' "$check_out" | grep -qE 'sed -nE +61' \
    || fail "port summary sed -nE count is not 61"
printf '%s\n' "$check_out" | grep -qE '=~ regex literal hoists +11' \
    || fail "port summary regex-hoist count is not 11"
printf '%s\n' "$check_out" | grep -qE 'wrapped/replaced modern functions +39' \
    || fail "port summary wrapped-function count is not 39"
pass "port --check in sync and lists every transformation"

# --- generated helper has no bash-4 syntax ----------------------------------
grep -q 'mapfile -t' "$HELPER" && fail "generated helper still calls mapfile"
grep -qE '(declare|local) -A' "$HELPER" && fail "generated helper still declares associative arrays"
grep -q '&>>' "$HELPER" && fail "generated helper still uses &>>"
grep -qE 'wait +-n' "$HELPER" && fail "generated helper still uses wait -n"
grep -qE '(^|[^_[:alnum:]])readarray([^_[:alnum:]]|$)' "$HELPER" \
    && fail "generated helper still uses the readarray builtin"
grep -qE '\$\{[A-Za-z_][A-Za-z0-9_]*,,|\$\{[A-Za-z_][A-Za-z0-9_]*\^\^|\$\{[A-Za-z_][A-Za-z0-9_]*\^\}' "$HELPER" \
    && fail "generated helper still uses bash-4 case conversion"
grep -q 'legacy_readarray' "$HELPER" || fail "generated helper does not use legacy_readarray"
grep -q 'legacy_lc' "$HELPER" || fail "generated helper does not use legacy_lc"
grep -q 'legacy_assoc_get' "$HELPER" || fail "generated helper does not use legacy_assoc_get"
pass "generated helper is bash-3.1 syntax clean"

# --- overlay wiring and gating surface --------------------------------------
for symbol in read_target_os_modern grub_unavailable_reason_modern \
    adaptive_grub_repair_modern diagnostic_repair_capabilities_modern \
    config_path_for_key_modern legacy_config_path_for_key \
    legacy_config_file_report mount_special_modern \
    legacy_dpkg_status_field legacy_dpkg_status_version \
    legacy_grub_repair legacy_grub_legacy_target legacy_feature_gating_report \
    legacy_feature_reason legacy_require_feature legacy_root_evidence_present \
    legacy_mount_special legacy_sort_versions legacy_b64e legacy_b64d \
    legacy_sed_ext legacy_findmnt legacy_lsblk legacy_realpath legacy_timeout_watchdog \
    legacy_mountinfo_table_from legacy_mounts_table_from legacy_mountpoints_from_table \
    legacy_chroot run_selected_chroot_modern display_unavailable_reason_modern \
    adaptive_display_manager_repair_modern repair_capability_evidence_modern \
    legacy_sysv_display_manager_entry legacy_display_manager_probe \
    legacy_display_manager_repair legacy_display_manager_rollback \
    unlock_target_modern mount_recorded_modern legacy_filter_mount_options \
    resolve_fstab_source_modern legacy_remap_target_device_path \
    legacy_boot_stack_repair repair_boot_stack_modern \
    legacy_chroot_shell legacy_host_shell \
    legacy_run_file_copy legacy_run_copy_item legacy_verify_copy_item \
    legacy_chown_reference_for_item \
    legacy_host_default_repair legacy_menu_lst_canonical_entry \
    host_default_unavailable_reason_modern; do
    grep -q "$symbol" "$HELPER" || fail "generated helper is missing $symbol"
done
grep -q 'STATE_ROOT=/var/run/boot-repair' "$HELPER" || fail "generated helper has no /var/run state fallback"
grep -q 'for feature in file-copy shell host-shell host-maintenance snapshots host-default' "$HELPER" \
    || fail "generated helper has no legacy feature list"
grep -q 'Legacy feature %s:' "$HELPER" || fail "generated helper has no legacy feature report format"
grep -q 'the legacy SysV display-manager repair is a host-scope stage' "$HELPER" \
    || fail "generated helper has no host-scope display reason"
grep -q 'cryptsetup --key-file "$keyfile" luksOpen "$ROOT_DEVICE" "$mapper_name"' "$HELPER" \
    || fail "generated helper lost the keyfile-based cryptsetup 1.0 luksOpen invocation"
grep -q 'legacy_unlock_keyfile_from_stdin' "$HELPER" \
    || fail "generated helper lost the newline-tolerant unlock keyfile handling"
grep -q 'unlock --key-file requires exactly one path argument (plus an optional --key-owner uid).' "$HELPER" \
    || fail "generated helper lost the optional unlock --key-file argument"
grep -q 'unlock_target --key-file "$2"' "$HELPER" \
    || fail "generated helper does not pass --key-file through to unlock_target"
grep -q 'unlock_target --key-file "$2" --key-owner "$4"' "$HELPER" \
    || fail "generated helper does not pass --key-owner through to unlock_target"
grep -q 'unlock --key-owner requires a uid.' "$HELPER" \
    || fail "generated helper lost the --key-owner argument parsing"
grep -q 'UNLOCKED_ROOT=' "$HELPER" \
    || fail "generated helper lost the unlocked-root probe line"
grep -q 'legacy_blkid_value_path "$entry"' "$HELPER" \
    || fail "generated helper lost the direct-path unlocked-root fstype probe"
grep -q 'UNLOCKED_ROOT_UUID=' "$HELPER" \
    || fail "generated helper lost the unlocked-root UUID probe line"
grep -q 'legacy_blkid_uuid_path' "$HELPER" \
    || fail "generated helper lost the path-based blkid UUID probe"
grep -q 'legacy_blkid_value_path' "$HELPER" \
    || fail "generated helper lost the path-based blkid TYPE probe"
grep -q '^mount_target_data_partitions()' "$HELPER" \
    || fail "generated helper lost the shared split-LV data mount pass"
grep -q '^prepare_target()' "$HELPER" \
    || fail "generated helper lost the unwrapped shared prepare_target"
if grep -q 'legacy_mount_target_fstab_entries\|LEGACY_DATA_MOUNTS\|LEGACY_DATA_PROMOTED' "$HELPER"; then
    fail "generated helper still carries the retired legacy data-mount pass"
fi
grep -q 'defoptions/kopt-managed arguments captured' "$HELPER" \
    || fail "generated helper lost the GRUB defoptions preflight note"
grep -q 'legacy_grub_managed_options' "$HELPER" \
    || fail "generated helper lost the defoptions/kopt extraction"
grep -q 'legacy_unlock_root_probe "$existing_mapper"' "$HELPER" \
    || fail "generated helper lost the existing-mapper root probe"
grep -q 'filesystem_release_all_mounts_modern' "$HELPER" \
    || fail "generated helper lost the filesystem release wrap"
grep -q 'filesystem_mountpoint_for_device_modern' "$HELPER" \
    || fail "generated helper lost the filesystem mountpoint wrap"
grep -q 'mount_target_resolver_modern' "$HELPER" \
    || fail "generated helper lost the idempotent resolver-bind wrap"
grep -q 'legacy_crypt_status_device' "$HELPER" \
    || fail "generated helper lost the cryptsetup 1.0 status parser"
grep -q 'find_crypt_mapper_for_device_modern' "$HELPER" \
    || fail "generated helper lost the existing-mapper lookup wrap"
grep -q 'cleanup_modern' "$HELPER" \
    || fail "generated helper lost the cleanup wrap"
grep -q 'LEGACY_RESOLVER_DESTINATION' "$HELPER" \
    || fail "generated helper lost the resolver-copy teardown state"
grep -q 'Copied recovery-host resolver into the target chroot' "$HELPER" \
    || fail "generated helper lost the resolver copy path"
grep -q 'Remounting target data filesystem ' "$HELPER" \
    || fail "generated helper lost the data-mount promotion line"
grep -q '^remount_target_data_rw()' "$HELPER" \
    || fail "generated helper lost the unwrapped fail-closed data promotion"
# The shared evidence-based apt-intent translation covers the guarded shell
# paths; the legacy plain-chroot paths keep the unconditional Etch translation.
grep -q '^apt_intent_translate()' "$HELPER" \
    || fail "generated helper lost the shared evidence-based apt-intent translation"
grep -q 'legacy_apt_intent_translate' "$HELPER" \
    || fail "generated helper lost the plain-chroot apt-intent translation"
if grep -E 'awk .*\[\[:space:\]\]' "$OVERLAY"; then
    fail "overlay awk programs still use the mawk-incompatible [[:space:]] class"
fi
grep -q 'legacy_unlock_keyfile_from_file' "$HELPER" \
    || fail "generated helper lost the GUI keyfile channel"
grep -q 'The unlock keyfile is not owned by the calling user.' "$HELPER" \
    || fail "generated helper lost the keyfile ownership check"
grep -q 'SUDO_UID' "$HELPER" \
    || fail "generated helper keyfile ownership check does not consult SUDO_UID"
grep -q 'cryptsetup open --type luks --key-file -' "$HELPER" \
    || fail "generated helper lost the modern cryptsetup open invocation (rename)"
grep -q "grep -v '^noload$'" "$HELPER" \
    || fail "generated helper lost the noload mount-option filter"
grep -q 'vgscan --mknodes' "$HELPER" \
    || fail "generated helper lost the post-unlock LVM scan"
grep -q 'LVM scan/activation after unlock:' "$HELPER" \
    || fail "generated helper lost the post-unlock LVM log line"
grep -q 'legacy_remap_target_device_path' "$HELPER" \
    || fail "generated helper lost the host-relative device path remap"
grep -q 'resolve_fstab_source_modern' "$HELPER" \
    || fail "generated helper lost the renamed fstab source resolution"
grep -q 'split-mount safe' "$HELPER" \
    || fail "generated helper lost the split-mount-safe legacy evidence chain"
grep -q '/etc/apt/sources.list' "$HELPER" \
    || fail "generated helper lost the sources-list root evidence"
grep -q 'guarded plain-chroot fallback' "$HELPER" \
    || fail "generated helper has no plain-chroot fallback wording"
pass "overlay wiring and gating surface present"

# --- compat shim behaviour ---------------------------------------------------
shim_checks()
{
    export BOOT_REPAIR_LEGACY_SHIMS=force
    # shellcheck disable=SC1090
    source "$COMPAT"

    [[ "$(legacy_lc AbC)" == abc ]] || fail "legacy_lc"
    [[ "$(legacy_uc aBc)" == ABC ]] || fail "legacy_uc"
    [[ "$(legacy_ucfirst hello)" == Hello ]] || fail "legacy_ucfirst"

    local -a arr=()
    legacy_readarray -t arr <<< $'one two\nthree\\four\n\nlast'
    [[ "${#arr[@]}" -eq 4 ]] || fail "legacy_readarray element count: ${#arr[@]}"
    [[ "${arr[0]}" == 'one two' ]] || fail "legacy_readarray space handling"
    [[ "${arr[1]}" == 'three\four' ]] || fail "legacy_readarray backslash handling"
    [[ -z "${arr[2]}" ]] || fail "legacy_readarray empty line handling"
    [[ "${arr[3]}" == last ]] || fail "legacy_readarray last line handling"

    # shellcheck disable=SC2034  # consumed by the legacy_assoc_* dynamic scope
    assoc_checks()
    {
        local -a m=() m_keys=()
        legacy_assoc_set m alpha 1
        legacy_assoc_set m beta 2
        legacy_assoc_set m alpha 3
        [[ "$(legacy_assoc_get m alpha)" == 3 ]] || fail "legacy_assoc_get overwrite"
        [[ "$(legacy_assoc_get m beta)" == 2 ]] || fail "legacy_assoc_get second key"
        [[ -z "$(legacy_assoc_get m missing)" ]] || fail "legacy_assoc_get missing key"
        legacy_assoc_has m alpha || fail "legacy_assoc_has present"
        legacy_assoc_has m missing && fail "legacy_assoc_has missing"
        [[ "$(legacy_assoc_get_num m missing)" == 0 ]] || fail "legacy_assoc_get_num default"
        [[ "$(legacy_assoc_keys m | LC_ALL=C sort | tr '\n' ' ')" == 'alpha beta ' ]] \
            || fail "legacy_assoc_keys"
        legacy_assoc_unset m alpha || fail "legacy_assoc_unset"
        legacy_assoc_has m alpha && fail "legacy_assoc_unset did not remove"
        [[ "$(legacy_assoc_get m beta)" == 2 ]] || fail "legacy_assoc_unset shifted the wrong entry"
        return 0
    }
    assoc_checks

    local out
    out="$(printf '2.6.18\n2.6.9\n2.6.10\n2.10\n' | legacy_sort_versions | tr '\n' ' ')"
    [[ "$out" == '2.6.9 2.6.10 2.6.18 2.10 ' ]] || fail "legacy_sort_versions order: $out"
    out="$(printf 'b\na\nb\n' | legacy_sort_versions -u | tr '\n' ' ')"
    [[ "$out" == 'a b ' ]] || fail "legacy_sort_versions -u: $out"
    out="$(printf '2-3\n1-9\n1-10\n' | legacy_sort_versions -k1,1 | tr '\n' ' ')"
    [[ "$out" == '1-9 1-10 2-3 ' ]] || fail "legacy_sort_versions -k1,1: $out"

    [[ "$(printf 'secret data' | legacy_b64e | legacy_b64d)" == 'secret data' ]] \
        || fail "legacy_b64 roundtrip"

    [[ "$(legacy_realpath -m /no/such/path)" == /no/such/path ]] || fail "legacy_realpath -m"
    [[ "$(legacy_realpath -e /etc/hosts)" == /etc/hosts ]] || fail "legacy_realpath -e"

    legacy_mountpoint -q / || fail "legacy_mountpoint /"
    legacy_mountpoint -q /no/such/mountpoint && fail "legacy_mountpoint accepted a non-mountpoint"

    out="$(legacy_findmnt -rn -o SOURCE --target / | head -n1)"
    [[ -n "$out" ]] || fail "legacy_findmnt --target / returned no source"
    out="$(legacy_findmnt -rn -o SOURCE,TARGET)"
    printf '%s\n' "$out" | grep -q ' /$' || fail "legacy_findmnt full listing"
    legacy_findmnt --definitely-unknown-option >/dev/null 2>&1 && fail "legacy_findmnt accepted an unknown option"

    legacy_mount_rslave_supported && fail "legacy_mount_rslave_supported must fail under force"
    legacy_mount_special bogus a b && fail "legacy_mount_special accepted an unknown kind"

    local dev=""
    dev="$(awk '$2 == "/" && $1 ~ /^\/dev\// {print $1; exit}' /proc/mounts)"
    if [[ -n "$dev" && -b "$dev" ]]; then
        local kname expected
        kname="$(legacy_lsblk -ndo KNAME "$dev")"
        expected="$(basename -- "$(readlink -f -- "$dev")")"
        [[ "$kname" == "$expected" ]] || fail "legacy_lsblk KNAME: $kname != $expected"
        [[ -n "$(legacy_lsblk -ndo TYPE "$dev")" ]] || fail "legacy_lsblk TYPE empty"
        legacy_lsblk -srnpo NAME,TYPE "$dev" | awk '$2 == "disk" { found = 1 } END { exit(found ? 0 : 1) }' \
            || fail "legacy_lsblk inverse walk found no disk"
        legacy_lsblk -P -b -p -o NAME,FSTYPE,SIZE,TYPE "$dev" | grep -q 'NAME="' \
            || fail "legacy_lsblk -P output missing NAME"
        # The /proc/mounts fallback matches the canonicalized source path.
        out="$(printf '1\t/\t%s\text3\trw\n' "$dev" | legacy_mountpoints_from_table "$dev")"
        [[ "$out" == "/" ]] || fail "legacy_mountpoints_from_table: $out"
    else
        printf 'skip - no / block device available for the lsblk fallback check\n'
    fi

    # Unknown options and unsupported flag letters must fail closed.
    legacy_lsblk -ndo NAME --definitely-unknown >/dev/null 2>&1 \
        && fail "legacy_lsblk accepted an unknown long option"
    legacy_lsblk -ndqo NAME "$dev" >/dev/null 2>&1 \
        && fail "legacy_lsblk accepted an unknown short flag"

    # -d (no children) must suppress child rows when the kernel has them.
    local disk disk_with_children=""
    for disk in /sys/class/block/*; do
        [[ -d "$disk" && ! -e "$disk/partition" ]] || continue
        if ls "$disk"/*/partition >/dev/null 2>&1; then
            disk_with_children="/dev/${disk##*/}"
            break
        fi
    done
    if [[ -n "$disk_with_children" ]]; then
        local one many
        one="$(legacy_lsblk -dno NAME "$disk_with_children")"
        many="$(legacy_lsblk -no NAME "$disk_with_children")"
        [[ "$(printf '%s\n' "$one" | wc -l | tr -d '[:space:]')" -eq 1 ]] \
            || fail "legacy_lsblk -d did not suppress child rows: $one"
        [[ "$(printf '%s\n' "$many" | wc -l | tr -d '[:space:]')" -gt 1 ]] \
            || fail "legacy_lsblk without -d lost child rows: $many"
    else
        printf 'skip - no partitioned disk for the lsblk -d check\n'
    fi

    # /proc/mounts fallback (Etch 2.6.18 has no /proc/self/mountinfo): the
    # effective mount is the last entry for a target and the first-seen row
    # position is kept.
    local mounts_fixture
    mounts_fixture="$(mktemp "${TMPDIR:-/tmp}/legacy-mounts.XXXXXX")"
    cat > "$mounts_fixture" <<'MOUNTS'
rootfs / rootfs rw 0 0
none /sys sysfs rw 0 0
/dev/mapper/vg-root / ext3 rw,data=ordered 0 0
tmpfs /tmp tmpfs rw,nosuid 0 0
/dev/sda1 /tmp ext4 rw 0 0
MOUNTS
    out="$(legacy_mounts_table_from "$mounts_fixture")"
    [[ "$(printf '%s\n' "$out" | wc -l | tr -d '[:space:]')" -eq 3 ]] \
        || fail "legacy_mounts_table_from row count: $out"
    printf '%s\n' "$out" | awk -F'\t' '$2 == "/" && $3 == "/dev/mapper/vg-root" { found = 1 } END { exit(found ? 0 : 1) }' \
        || fail "legacy_mounts_table_from did not keep the effective / row: $out"
    printf '%s\n' "$out" | awk -F'\t' '$2 == "/tmp" && $3 == "/dev/sda1" { found = 1 } END { exit(found ? 0 : 1) }' \
        || fail "legacy_mounts_table_from did not keep the effective /tmp row: $out"
    rm -f -- "$mounts_fixture"

    local rc=0
    set +e
    timeout 1 sleep 5
    rc=$?
    set -e
    [[ "$rc" -eq 124 ]] || fail "legacy_timeout watchdog did not return 124 (got $rc)"
    [[ "$(timeout 5 echo ok)" == ok ]] || fail "legacy_timeout watchdog command result"
    return 0
}
( shim_checks ) || fail "compat shim behaviour"
pass "compat shims (readarray, case, assoc, sort, base64, realpath, mountpoint, findmnt, lsblk, timeout)"

# --- evidence-based feature gating ------------------------------------------
gating_checks()
{
    # bash 3.1 cannot source a process substitution; use a staged file.
    local helper_src
    helper_src="$(mktemp "${TMPDIR:-/tmp}/legacy-helper-sourced.XXXXXX")"
    sed '/^main "\$@"$/d' "$HELPER" > "$helper_src"
    # shellcheck disable=SC1090
    source "$helper_src"
    rm -f -- "$helper_src"
    trap - EXIT INT TERM HUP
    set +e

    local report line feature reason out
    report="$(legacy_feature_gating_report)"
    printf '%s\n' "$report" | grep -q '^Legacy feature gating (read-only):$' \
        || fail "legacy feature report header missing"
    for feature in file-copy shell host-shell host-maintenance snapshots host-default; do
        line="$(printf '%s\n' "$report" | grep "^Legacy feature ${feature}:")"
        [[ -n "$line" ]] || fail "legacy feature report line missing: $feature"
        # bash 3.1 cannot parse an unquoted `(`/`|` regex operand, so the
        # report shape is checked with a case glob (portable and sufficient).
        case "$line" in
            'Legacy feature '*': available') ;;
            'Legacy feature '*': unavailable|'*) ;;
            *) fail "legacy feature report shape: $line" ;;
        esac
    done

    # Forced-unavailable probes (no tools on PATH).
    reason="$(PATH=/nonexistent legacy_feature_reason file-copy 2>&1)"
    [[ $? -ne 0 ]] || fail "file-copy must be unavailable without cp"
    [[ "$reason" == *'cp is not installed'* ]] || fail "file-copy reason: $reason"
    reason="$(PATH=/nonexistent legacy_feature_reason host-maintenance 2>&1)"
    [[ $? -ne 0 ]] || fail "host-maintenance must be unavailable with no tools"
    case "$reason" in
        *'unshare is not installed'*'EFI host needs firmware-variable isolation'*|*'chroot is not installed'*)
            ;;
        *) fail "host-maintenance reason: $reason" ;;
    esac
    local tmpbin
    tmpbin="$(mktemp -d "${TMPDIR:-/tmp}/legacy-feature-probe.XXXXXX")"
    ln -s "$(command -v chroot)" "$tmpbin/chroot"
    # shell is now the guarded plain chroot: chroot alone makes it available.
    PATH="$tmpbin" legacy_feature_reason shell \
        || fail "shell must be available through the guarded plain chroot"
    # BIOS hosts: host-maintenance and host-shell need only chroot; EFI hosts
    # keep the unshare/timeout gate.  BOOT_REPAIR_LEGACY_HOST_EFI is the
    # read-only test seam for the firmware probe.
    BOOT_REPAIR_LEGACY_HOST_EFI=no PATH="$tmpbin" legacy_feature_reason host-maintenance \
        || fail "host-maintenance must be available through the plain-chroot fallback on a BIOS-only host"
    BOOT_REPAIR_LEGACY_HOST_EFI=no PATH="$tmpbin" legacy_feature_reason host-shell \
        || fail "host-shell must be available on a BIOS-only host"
    reason="$(BOOT_REPAIR_LEGACY_HOST_EFI=yes PATH="$tmpbin" legacy_feature_reason host-maintenance 2>&1)"
    [[ $? -ne 0 ]] || fail "host-maintenance must keep the unshare gate on an EFI host"
    [[ "$reason" == *'unshare is not installed'* ]] || fail "EFI host-maintenance reason: $reason"
    reason="$(BOOT_REPAIR_LEGACY_HOST_EFI=yes PATH="$tmpbin" legacy_feature_reason host-shell 2>&1)"
    [[ $? -ne 0 ]] || fail "host-shell must keep the strict gate on an EFI host"
    [[ "$reason" == *'unshare is not installed'* ]] || fail "EFI host-shell reason: $reason"
    rm -rf -- "$tmpbin"

    # Runtime entry points must fail closed with the unavailable|<reason> line.
    out="$(PATH=/nonexistent run_file_copy copy host-to-repair smart normal /tmp /tmp 2>&1)"
    [[ $? -ne 0 ]] || fail "run_file_copy must refuse on a legacy host"
    [[ "$out" == *'unavailable|file-copy:'* ]] || fail "run_file_copy refusal: $out"
    out="$(PATH=/nonexistent run_chroot_shell true 2>&1)"
    [[ $? -ne 0 ]] || fail "run_chroot_shell must refuse on a legacy host"
    [[ "$out" == *'unavailable|shell:'* ]] || fail "run_chroot_shell refusal: $out"
    out="$(PATH=/nonexistent run_host_shell /dev/null /dev/null true 2>&1)"
    [[ $? -ne 0 ]] || fail "run_host_shell must refuse on a legacy host"
    [[ "$out" == *'unavailable|host-shell:'* ]] || fail "run_host_shell refusal: $out"

    # Etch target configuration keys and the read-only availability report.
    [[ "$(config_path_for_key fstab)" == /etc/fstab ]] \
        || fail "config_path_for_key fstab"
    [[ "$(config_path_for_key menu-lst)" == /boot/grub/menu.lst ]] \
        || fail "config_path_for_key menu-lst"
    [[ "$(config_path_for_key grub-config)" == /boot/grub/grub.cfg ]] \
        || fail "config_path_for_key delegates the modern keys"
    legacy_config_path_for_key mystery >/dev/null 2>&1 \
        && fail "legacy_config_path_for_key accepted an unknown key"

    local target_fixture
    target_fixture="$(mktemp -d "${TMPDIR:-/tmp}/legacy-config-target.XXXXXX")"
    mkdir -p "$target_fixture/etc/apt"
    printf 'proc /proc proc defaults 0 0\n' > "$target_fixture/etc/fstab"
    printf 'id:3:initdefault:\n' > "$target_fixture/etc/inittab"
    printf 'loop\n' > "$target_fixture/etc/modules"
    TARGET_ROOT="$target_fixture"
    RUNNING_HOST_MODE=0
    out="$(legacy_config_file_report)"
    [[ "$(printf '%s\n' "$out" | grep -c '^Legacy config ')" -eq 8 ]] \
        || fail "config report must list 8 keys: $out"
    printf '%s\n' "$out" | grep -q '^Legacy config fstab: available$' \
        || fail "config report fstab availability"
    printf '%s\n' "$out" | grep -q '^Legacy config inittab: available$' \
        || fail "config report inittab availability"
    printf '%s\n' "$out" | grep -q '^Legacy config menu-lst: unavailable|/boot/grub/menu.lst ' \
        || fail "config report menu-lst reason"
    RUNNING_HOST_MODE=1
    out="$(legacy_config_file_report)"
    [[ -z "$out" ]] || fail "config report must stay target-scope only: $out"
    rm -rf -- "$target_fixture"
    return 0
}
( gating_checks ) || fail "legacy feature gating"
pass "feature gating (file-copy, shell, host modes with the plain-chroot fallback, snapshots, host-default)"

echo "legacy port contract: PASS"
