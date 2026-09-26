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
# Every transformation and every post-rewrite assertion must be listed with
# its exact audited count (A11-01/A11-04).  The `+N` are per-transform counts
# of the modern source; the shim/residual lines pin the ported body.
for rule in \
    'assoc declarations \(declare/local -A\) +10' \
    'assoc accesses: current_by_partition_label +6' \
    'assoc accesses: final_destination_count +3' \
    'assoc accesses: seen_destination_names +2' \
    'assoc accesses: FS_SCOPE_DEVICE_INDEX +4' \
    'assoc accesses: destination_priority +4' \
    'assoc accesses: destination_id +4' \
    'assoc accesses: current_by_key +6' \
    'assoc accesses: removed_seen +4' \
    'assoc accesses: current_by_id +3' \
    'assoc accesses: order_index +5' \
    'assoc accesses: reasons +31' \
    'assoc accesses: seen +4' \
    'assoc accesses: essential_by_dev +1' \
    'assoc accesses: allowed_nodes +1' \
    'case conversion ,, +64' \
    'case conversion \^\^ +15' \
    'case conversion \^ +23' \
    'array \[@\] expansions \(all forms\) +299' \
    'mapfile call sites +60' \
    'sed -i -E +1' \
    'sed -nE +61' \
    'sed -E +16' \
    'sort -V +12' \
    'date --iso-8601 +4' \
    '=~ regex literal hoists +11' \
    'os-release gates +1' \
    'dpkg db:Status sites +4' \
    'wrapped/replaced modern functions +42' \
    'post-rewrite residual: mapfile +0' \
    'post-rewrite residual: sed -i -E +0' \
    'post-rewrite residual: sed -nE +0' \
    'post-rewrite residual: sed -E +0' \
    'post-rewrite residual: sort -V +0' \
    'post-rewrite residual: date --iso-8601 +0' \
    'post-rewrite residual: case conversion +0' \
    'post-rewrite residual: corrupted assoc set +0' \
    'shim calls: legacy_readarray +60' \
    'shim calls: legacy_sed_ext +78' \
    'shim calls: legacy_sort_versions +12' \
    'shim calls: legacy_date_iso +4' \
    'shim calls: legacy_lc +64' \
    'shim calls: legacy_uc +15' \
    'shim calls: legacy_ucfirst +23' \
    'shim calls: legacy_assoc_get +26' \
    'shim calls: legacy_assoc_get_num +2' \
    'shim calls: legacy_assoc_keys +1' \
    'shim calls: legacy_assoc_has +1' \
    'shim calls: legacy_assoc_set +56' \
    'shim calls: legacy_assoc_unset +0' \
    'prelude \(legacy/compat.sh\) ' \
    'overlay \(legacy/overlay.sh\) ' \
    'port-introduced duplicate functions +0'; do
    printf '%s\n' "$check_out" | grep -qE "$rule" \
        || fail "port summary check failed: $rule"
done
pass "port --check in sync and lists every transformation with its audited count"

# --- generated helper has no bash-4 syntax ----------------------------------
# Scan the code with full-line comments excluded (comments may legitimately
# document the ported constructs, e.g. compat.sh naming `mapfile -t`).
helper_code="$(mktemp "${TMPDIR:-/tmp}/legacy-helper-code.XXXXXX")"
grep -vE '^[[:space:]]*#' "$HELPER" > "$helper_code"
grep -q 'mapfile -t' "$helper_code" && fail "generated helper still calls mapfile"
grep -qE '(declare|local) -A' "$helper_code" && fail "generated helper still declares associative arrays"
grep -q '&>>' "$helper_code" && fail "generated helper still uses &>>"
grep -qE 'wait +-n' "$helper_code" && fail "generated helper still uses wait -n"
grep -qE '(^|[^_[:alnum:]])readarray([^_[:alnum:]]|$)' "$helper_code" \
    && fail "generated helper still uses the readarray builtin"
grep -qE '\$\{[A-Za-z_][A-Za-z0-9_]*,,|\$\{[A-Za-z_][A-Za-z0-9_]*\^\^|\$\{[A-Za-z_][A-Za-z0-9_]*\^\}' "$helper_code" \
    && fail "generated helper still uses bash-4 case conversion"
grep -q 'legacy_readarray' "$helper_code" || fail "generated helper does not use legacy_readarray"
grep -q 'legacy_lc' "$helper_code" || fail "generated helper does not use legacy_lc"
grep -q 'legacy_assoc_get' "$helper_code" || fail "generated helper does not use legacy_assoc_get"
# Extended bash-4 blacklist (A11-02): the full list is unit-tested below, this
# is the smoke gate on the real artifact.
grep -qE '&>|\|&|\[\[[^]]*[[:space:]]-v[[:space:]]|declare[[:space:]]+-g|local[[:space:]]+-g|globstar|;&|;;&|\$\{[A-Za-z_][A-Za-z0-9_]*\[-[0-9]+\]\}|\$\{[A-Za-z_][A-Za-z0-9_]*@[QAEPaK]\}|\$\{![A-Za-z_][A-Za-z0-9_]*[@*]\}' "$helper_code" \
    && fail "generated helper contains an extended-blacklist bash-4 construct"
rm -f -- "$helper_code"
pass "generated helper is bash-3.1 syntax clean"

# --- port pipeline invariants (A9-07/A11-08) --------------------------------
# The assoc rewrite must never corrupt a name that shares a tail with another
# array (seen inside removed_seen); and every legacy_assoc_* first argument
# must be one of the twelve known assoc array names.
if grep -qE 'removed_legacy_assoc_set|removed_seen_legacy' "$HELPER"; then
    fail "generated helper contains a corrupted assoc-set rewrite"
fi
assoc_known='seen seen_destination_names reasons destination_id destination_priority removed_seen final_destination_count order_index current_by_id current_by_key current_by_partition_label FS_SCOPE_DEVICE_INDEX essential_by_dev allowed_nodes'
assoc_unknown="$(grep -oE 'legacy_assoc_(get|get_num|keys|has|set|unset)[[:space:]]+[A-Za-z_][A-Za-z0-9_]*' "$HELPER" \
    | awk '{print $2}' | sort -u \
    | while IFS= read -r n; do
        case " $assoc_known " in
            *" $n "*) ;;
            *) printf '%s\n' "$n" ;;
        esac
    done)"
[[ -z "$assoc_unknown" ]] \
    || fail "generated helper uses unknown assoc array names: $(printf '%s' "$assoc_unknown" | tr '\n' ' ')"
# Every function must be defined exactly once (the modern source's own
# intentional aliases are tracked by the port; a duplicate the port introduces
# fails generation — the overlay's dead run_chroot_shell/run_host_shell pair
# is the regression this pins).
[[ "$(grep -cE '^run_chroot_shell\(\)$' "$HELPER")" -eq 1 ]] \
    || fail "run_chroot_shell is not defined exactly once in the generated helper"
[[ "$(grep -cE '^run_host_shell\(\)$' "$HELPER")" -eq 1 ]] \
    || fail "run_host_shell is not defined exactly once in the generated helper"
pass "assoc rewrite integrity and single function definitions"

# --- verify_no_bash4 blacklist (extracted, unit-tested against fixtures) ----
# The checker is self-contained in port.sh; extract it and run it against a
# fixture containing every blacklisted construct (must fail, naming each) and
# a clean fixture (must pass).
checker_src="$(mktemp "${TMPDIR:-/tmp}/legacy-bash4-checker.XXXXXX")"
awk '/^verify_no_bash4\(\)$/{f=1} f{print} f&&/^}$/{exit}' "$PORT" > "$checker_src"
[[ -s "$checker_src" ]] || fail "could not extract verify_no_bash4 from port.sh"
bash4_fixture="$(mktemp "${TMPDIR:-/tmp}/legacy-bash4-fixture.XXXXXX")"
cat > "$bash4_fixture" <<'BASH4FIXTURE'
mapfile -t m < <(true)
declare -A aa=()
local -A la=()
coproc co { :; }
printf -v pv '%s' x
echo out &>> appended
echo out &> both
cat |& tr a-z A-Z
[[ -v SOME_VAR ]]
declare -g gv=1
local -g lgv
shopt -s globstar
case "$x" in a) : ;& b) : ;; c) : ;;& d) : ;; esac
v=${arr[-1]}
q=${name@Q}
ind=${!prefix@}
readarray -t r < file
wait -n
local -n ref=x
low=${v,,}
up=${v^^}
first=${v^}
BASH4FIXTURE
set +e
checker_out="$( ( source "$checker_src"; verify_no_bash4 "$bash4_fixture" ) 2>&1 )"
checker_rc=$?
set -e
[[ "$checker_rc" -ne 0 ]] || fail "verify_no_bash4 accepted a fixture full of bash-4 constructs"
for label in 'mapfile builtin' 'declare -A' 'local -A' 'coproc' 'printf -v' \
    'case fallthrough ;&' 'case fallthrough ;;&' 'pipe both streams |&' \
    '&> / &>> append both streams' '[[ -v var ]] test' 'declare -g' 'local -g' \
    'globstar' 'readarray builtin' 'wait -n' 'local -n' \
    'negative array subscript' '${var@Q} transform' 'indirect ${!prefix@}' \
    'case conversion ,,' 'case conversion ^^' 'case conversion ^'; do
    printf '%s\n' "$checker_out" | grep -qF "bash-4 construct remains in $bash4_fixture: $label" \
        || fail "verify_no_bash4 did not flag: $label"
done
# A clean fixture (bash 3.1-safe constructs, including 2>&1 and >&2 which must
# never match the &> / &>> entry) must pass.
cat > "$bash4_fixture" <<'BASH4CLEAN'
legacy_readarray -t m < <(true)
[[ "$x" == y ]]
foo 2>&1 | bar >&2
local -a names=()
arr=("${arr[@]:-}")
low="$(legacy_lc "$x")"
BASH4CLEAN
( source "$checker_src"; verify_no_bash4 "$bash4_fixture" ) \
    || fail "verify_no_bash4 rejected a clean fixture"
rm -f -- "$checker_src" "$bash4_fixture"
pass "verify_no_bash4 blacklist flags every construct and passes clean input"

# --- drift failure: a mutated block must abort generation (A11-01) ----------
# Run in a /tmp copy of the port tree; the canonical tree is never mutated.
drift_fixture="$(mktemp -d "${TMPDIR:-/tmp}/legacy-port-drift.XXXXXX")"
mkdir -p "$drift_fixture/legacy" "$drift_fixture/scripts"
cp "$MODERN" "$drift_fixture/scripts/boot-repair-helper.sh"
cp "$COMPAT" "$drift_fixture/legacy/compat.sh"
cp "$OVERLAY" "$drift_fixture/legacy/overlay.sh"
cp "$PORT" "$drift_fixture/legacy/port.sh"
# One regex-hoist block drifts: the spacing inside the =~ operand changes, so
# the block no longer matches and replace_block must fail loudly.
sed -i 's/=~ (\[A-Z\]+)=\\"/=~ ([A-Z]+) =\\"/' "$drift_fixture/scripts/boot-repair-helper.sh"
if ( cd "$drift_fixture" && bash legacy/port.sh ) > "$drift_fixture/generate.log" 2>&1; then
    cat "$drift_fixture/generate.log" >&2
    fail "port.sh generate must exit nonzero when a block drifts"
fi
grep -q 'replace_block matched 0 occurrences' "$drift_fixture/generate.log" \
    || fail "drift abort message is unclear: $(cat "$drift_fixture/generate.log")"
[[ ! -e "$drift_fixture/legacy/boot-repair-helper.sh" ]] \
    || fail "a failed generation must not write the output helper"
rm -rf -- "$drift_fixture"
pass "port.sh generate aborts loudly when a transform matches nothing"

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
grep -q 'The unlock keyfile owner cannot be proven; refusing.' "$HELPER" \
    || fail "generated helper lost the provable keyfile ownership check"
grep -q 'legacy_unlock_keyfile_owner_proven' "$HELPER" \
    || fail "generated helper lost the provable-ownership proof function"
grep -q 'legacy_unlock_keyfile_path_safe' "$HELPER" \
    || fail "generated helper lost the symlink-free keyfile path proof"
grep -q 'realpath -- "$keyfile_arg"' "$HELPER" \
    || fail "generated helper keyfile path proof does not resolve through realpath"
grep -q 'SUDO_UID' "$HELPER" \
    || fail "generated helper keyfile ownership check does not consult SUDO_UID"
if grep -q 'rm -f -- "$keyfile_arg"' "$HELPER"; then
    fail "generated helper still deletes the caller-supplied unlock keyfile"
fi
grep -q 'legacy_unlock_ensure_session' "$HELPER" \
    || fail "generated helper lost the unlock session-ensure function"
grep -q 'mktemp -d "$STATE_ROOT/session.XXXXXX"' "$HELPER" \
    || fail "generated helper lost the unlock session mktemp"
grep -q 'chmod 0700 -- "$SESSION_DIR"' "$HELPER" \
    || fail "generated helper does not secure the unlock session directory"
grep -q '( umask 077; : > "$keyfile" )' "$HELPER" \
    || fail "generated helper lost the umask-insensitive session keyfile creation"
grep -q 'IFS= read -r -n 1024 passphrase' "$HELPER" \
    || fail "generated helper lost the capped keyfile first-line read"
grep -q '65536-byte sanity cap' "$HELPER" \
    || fail "generated helper lost the keyfile stat sanity cap"
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
# Batch B2 hardening surface.
grep -Fq "sed 's/[]\\.*^\$|[]/\\\\&/g'" "$HELPER" \
    || fail "generated helper lost the A9-06 BRE escape class (backslash and pipe)"
grep -Fq "sed 's/[]\\.[*^\$+?|(){}]/\\\\&/g'" "$HELPER" \
    || fail "generated helper lost the A9-06 ERE escape class (backslash and pipe)"
grep -q 'cmp -s -- "$destination" /etc/resolv.conf' "$HELPER" \
    || fail "generated helper lost the A9-09 polluted-resolver refusal"
grep -q 'interrupted session' "$HELPER" \
    || fail "generated helper lost the interrupted-session resolver wording"
grep -q 'cmp -s -- "$LEGACY_RESOLVER_DESTINATION" /etc/resolv.conf' "$HELPER" \
    || fail "generated helper lost the A9-09 cleanup resolver comparison"
grep -q 'NOT overwritten' "$HELPER" \
    || fail "generated helper lost the A9-09 skip-restore evidence wording"
grep -q 'legacy_realpath -m "$entry"' "$HELPER" \
    || fail "generated helper lost the A9-10 display-entry canonicalization"
grep -q 'set -f' "$HELPER" \
    || fail "generated helper lost the A9-12 noglob guards"
grep -q 'set +f' "$HELPER" \
    || fail "generated helper lost the A9-12 noglob restoration"
grep -q 's/\\t/%09/g' "$HELPER" \
    || fail "generated helper lost the A9-13 TAB percent-encoding"
grep -q '[[ -n "$SESSION_DIR" ]] || SESSION_DIR="$(mktemp -d "$STATE_ROOT/session.XXXXXX")"' "$HELPER" \
    || fail "generated helper lost the A9-02 prepare_target session reuse guard"
# Batch B6 surface: host-shell timeout wrap, noload mount retry, VG-scoped
# unlocked-root probe.
host_shell_body="$(awk '/^legacy_host_shell\(\)$/{f=1} f{print} f&&/^}$/{exit}' "$HELPER")"
[[ -n "$host_shell_body" ]] || fail "could not extract legacy_host_shell from the generated helper"
printf '%s\n' "$host_shell_body" | grep -q 'timeout --foreground 300 --kill-after=10 /usr/bin/env' \
    || fail "generated helper lost the B6/A9-03 host-shell timeout wrap"
printf '%s\n' "$host_shell_body" | grep -q 'rc=${PIPESTATUS\[0\]}' \
    || fail "generated helper lost the B6/A9-03 host-shell PIPESTATUS rc"
grep -q 'grandchildren of a killed shell may outlive' "$HELPER" \
    || fail "generated helper lost the B6/A9-03 watchdog deviation comment"
grep -q 'legacy_mount_options_without_noload' "$HELPER" \
    || fail "generated helper lost the B6/A9-05 noload-stripping retry form"
grep -q 'tune2fs -l -- "\$source"' "$HELPER" \
    || fail "generated helper lost the B6/A9-05 tune2fs journal-state check"
grep -q 's/\^Filesystem state:\[\[:space:\]\]\*//p' "$HELPER" \
    || fail "generated helper lost the B6/A9-05 journal-state gate"
grep -q 'may replay the target'"'"'s journal during a nominally read-only mount' "$HELPER" \
    || fail "generated helper lost the B6/A9-05 replay-risk WARNING"
grep -q -- '--noheadings -o vg_name' "$HELPER" \
    || fail "generated helper lost the B6/A9-08 pvs VG probe"
grep -q -- '--noheadings -o lv_path' "$HELPER" \
    || fail "generated helper lost the B6/A9-08 lvs LV enumeration"
grep -q 'falling back to the unscoped unlocked-root probe' "$HELPER" \
    || fail "generated helper lost the B6/A9-08 unscoped fallback warning"
grep -q 'Unlocked-root probe scoped to volume group' "$HELPER" \
    || fail "generated helper lost the B6/A9-08 VG scope log line"
pass "overlay wiring and gating surface present"

# Batch B4: cancel-token surface in the generated helper.
grep -q '^legacy_cancel_requested()' "$HELPER" \
    || fail "generated helper lost the cancel-token predicate"
grep -q '^legacy_cancel_stage_check()' "$HELPER" \
    || fail "generated helper lost the stage-boundary cancel check"
grep -q '^legacy_cancel_watcher()' "$HELPER" \
    || fail "generated helper lost the shell-command cancel watcher"
grep -q 'kill -0 "$PPID"' "$HELPER" \
    || fail "generated helper watcher does not poll kill -0 PPID"
grep -q 'terminate_helper_tree "$child"' "$HELPER" \
    || fail "generated helper watcher lost the bounded TERM->KILL escalation"
grep -q -- '--cancel-file requires a path.' "$HELPER" \
    || fail "generated helper lost the --cancel-file option parsing"
grep -q -- '--cancel-token requires a value.' "$HELPER" \
    || fail "generated helper lost the --cancel-token option parsing"
grep -q 'CANCEL_FILE="$2"' "$HELPER" \
    || fail "generated helper does not store the --cancel-file path"
grep -q 'CANCEL_TOKEN="$2"' "$HELPER" \
    || fail "generated helper does not store the --cancel-token value"
grep -q 'CANCEL_FILE="${CANCEL_FILE:-}"' "$HELPER" \
    || fail "generated helper lost the environment cancel-file surface"
grep -q 'CANCEL_TOKEN="${CANCEL_TOKEN:-}"' "$HELPER" \
    || fail "generated helper lost the CANCEL_TOKEN environment surface"
grep -q '$SESSION_DIR/cancel' "$HELPER" \
    || fail "generated helper lost the session-dir cancel path"
grep -q '^run_package_stage_modern()' "$HELPER" \
    || fail "generated helper lost the renamed package-stage body"
grep -q '^adaptive_initramfs_repair_modern()' "$HELPER" \
    || fail "generated helper lost the renamed initramfs-stage body"
grep -q '^adaptive_grub_stage_modern()' "$HELPER" \
    || fail "generated helper lost the renamed grub-stage body"
grep -q 'exit "\$rc" ) &' "$HELPER" \
    || fail "generated helper shell pipelines do not run in the watcher-owned background subshell"
grep -q 'rc=${PIPESTATUS\[0\]}' "$HELPER" \
    || fail "generated helper shell pipelines do not preserve the command status through the watcher"
grep -q 'Cancellation:' "$HELPER" \
    || fail "generated helper usage lost the cancellation section"
pass "B4 cancel-token surface (options, token forms, watcher, stage checks, usage)"

# Batch B5: config-write content-file transport.  The legacy helper inherits
# the modern guarded transport (the dispatcher and run_target_config live in
# the ported modern body; the overlay only wraps config_path_for_key), so the
# generated helper must carry the exact modern proof surface plus the kept
# argv fallback.
grep -q '^config_content_file_path_safe()' "$HELPER" \
    || fail "generated helper lost the content-file path proof"
grep -q '^config_content_file_owner_proven()' "$HELPER" \
    || fail "generated helper lost the content-file owner proof"
grep -q 'config-write accepts either' "$HELPER" \
    || fail "generated helper dispatcher does not reject a malformed option shape"
grep -qF -- '--content-file <path> --content-owner <uid>' "$HELPER" \
    || fail "generated helper usage does not document the content-file transport"
grep -qF 'config-write <target-disk> <root-device> <config-key> <content>   (deprecated argv transport)' "$HELPER" \
    || fail "generated helper usage lost the deprecated argv transport form"
grep -q 'Target configuration is limited to 1 MiB; refusing.' "$HELPER" \
    || fail "generated helper content-file cap is not 1 MiB"
grep -q 'Target configuration is limited to 256 KiB.' "$HELPER" \
    || fail "generated helper lost the 256 KiB argv fallback bound"
grep -qF 'cp -- "$content_file" "$tmp"' "$HELPER" \
    || fail "generated helper does not copy the content file into the atomic temp"
grep -q 'content copy failed verification' "$HELPER" \
    || fail "generated helper lost the post-copy verification"
grep -q 'symlink-free regular file' "$HELPER" \
    || fail "generated helper content-file path safety refusal missing"
grep -q 'owner cannot be proven' "$HELPER" \
    || fail "generated helper content-file owner refusal missing"
grep -q 'PKEXEC_UID' "$HELPER" \
    || fail "generated helper content-file proof does not consult PKEXEC_UID"
grep -q 'SUDO_UID' "$HELPER" \
    || fail "generated helper content-file proof does not consult SUDO_UID"
if grep -q 'rm -f -- "$content_file"' "$HELPER"; then
    fail "generated helper still deletes the caller-supplied content file"
fi
pass "B5 config-write transport (path/owner proofs, 1 MiB cap, atomic copy, argv fallback, caller file survives)"

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

    # A11-03: the shim only honours -t/--.  -n/-d/-O/-s and every other unknown
    # option must be refused with a stderr reason, not silently consumed.
    local ropt rrc rerr
    for ropt in -n -d -O -s -X; do
        set +e
        rerr="$(legacy_readarray "$ropt" 5 arr </dev/null 2>&1)"
        rrc=$?
        set -e
        [[ "$rrc" -ne 0 ]] || fail "legacy_readarray must refuse $ropt"
        printf '%s\n' "$rerr" | grep -qF "unsupported option $ropt" \
            || fail "legacy_readarray refusal reason for $ropt: $rerr"
    done
    # -t and -- stay accepted.
    legacy_readarray -t arr </dev/null || fail "legacy_readarray -t must stay accepted"
    legacy_readarray -- arr </dev/null || fail "legacy_readarray -- must stay accepted"
    [[ "${#arr[@]}" -eq 0 ]] || fail "legacy_readarray -- arr must yield an empty array"

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
