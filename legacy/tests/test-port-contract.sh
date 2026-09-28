#!/usr/bin/env bash
# Fast contract for the legacy (Debian Etch / bash 3.1) helper port.
#
# Covers: deterministic drift gate (legacy/port.sh --check), absence of bash-4
# syntax in the generated helper, the modern helper staying untransformed,
# compat shim behaviour, and evidence-based legacy feature gating.
set -euo pipefail
# Every assertion here is an explicit ||/&& check; pipefail adds no coverage
# but turns `producer | grep -q` into a load-dependent false failure whenever
# grep -q exits early and the producer takes SIGPIPE (141) - observed on the
# 2-vCPU Alpine rig after the security sweep widened this suite. Disable it
# (gating_checks re-disables it after sourcing the helper, which re-enables
# it through its own set -Eeuo pipefail).
set +o pipefail

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
    'sed -nE +64' \
    'sed -E +16' \
    'sort -V +12' \
    'date --iso-8601 +4' \
    '=~ regex literal hoists +11' \
    'os-release gates +1' \
    'dpkg db:Status sites +4' \
    'wrapped/replaced modern functions +42' \
    'mount-sweep process substitutions rewritten +6' \
    'post-rewrite residual: mapfile +0' \
    'post-rewrite residual: sed -i -E +0' \
    'post-rewrite residual: sed -nE +0' \
    'post-rewrite residual: sed -E +0' \
    'post-rewrite residual: sort -V +0' \
    'post-rewrite residual: date --iso-8601 +0' \
    'post-rewrite residual: case conversion +0' \
    'post-rewrite residual: corrupted assoc set +0' \
    'shim calls: legacy_readarray +60' \
    'shim calls: legacy_sed_ext +81' \
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
# shellcheck source=/dev/null  # temp-extracted checker, resolved at runtime
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
# shellcheck source=/dev/null  # temp-extracted checker, resolved at runtime
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
grep -q '"$CRYPTSETUP_BIN" --key-file "$keyfile" luksOpen "$ROOT_DEVICE" "$mapper_name"' "$HELPER" \
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
grep -q 'LEGACY_CLEANUP_RC="$rc"' "$HELPER" \
    || fail "generated helper lost the cleanup rc handover (A9-09 follow-up)"
grep -q 'local rc="${LEGACY_CLEANUP_RC:-$?}"' "$HELPER" \
    || fail "generated helper cleanup_modern no longer preserves the handed-over rc (A9-09 follow-up)"
grep -q 'Copied recovery-host resolver into the target chroot' "$HELPER" \
    || fail "generated helper lost the resolver copy path"
grep -q 'Remounting target data filesystem ' "$HELPER" \
    || fail "generated helper lost the data-mount promotion line"
grep -q '^remount_target_data_rw()' "$HELPER" \
    || fail "generated helper lost the unwrapped fail-closed data promotion"
# A12-05: the mount-sweep hot paths must never feed an array from a process
# substitution - bash 3.1.17 corrupts its /dev/fd fifo bookkeeping when
# process substitution repeats inside the block sweep (reproduced on the Etch
# rig: rc 134/SIGABRT after the target data mounts were promoted).
helper_code2="$(mktemp "${TMPDIR:-/tmp}/legacy-sweep.XXXXXX")"
grep -vE '^[[:space:]]*#' "$HELPER" > "$helper_code2"
for sweep_fn in populate_writable_dev_filtered top_disks_for _top_disks_for_sysfs; do
    body="$(awk -v fn="$sweep_fn" '$0 ~ "^" fn "\\(\\)$" {f=1} f {print} f && /^}$/ {exit}' "$helper_code2")"
    [[ -n "$body" ]] || fail "generated helper lost $sweep_fn"
    # Only the populate sweep's single outer find feed may stay (one process
    # substitution for the whole loop, never one per swept entry).
    case "$sweep_fn" in
        populate_writable_dev_filtered)
            ps_feed="$(printf '%s\n' "$body" | grep -c '< <(')"
            [[ "$ps_feed" -le 1 ]] \
                || fail "$sweep_fn still feeds arrays from process substitutions"
            if printf '%s\n' "$body" | grep -q '< <(' \
                && ! printf '%s\n' "$body" | grep -q '< <(find "\$source"'; then
                fail "$sweep_fn kept a non-find process substitution feed"
            fi
            ;;
        *)
            if printf '%s\n' "$body" | grep -q '< <('; then
                fail "$sweep_fn still feeds an array from a process substitution"
            fi
            ;;
    esac
done
rm -f -- "$helper_code2"
grep -q 'legacy_readarray -t entry_tops <<<"$(top_disks_for "$entry" 2>/dev/null | sort -u || true)"' "$HELPER" \
    || fail "generated helper lost the A12-05 here-string entry_tops sweep feed"
grep -q 'legacy_readarray -t disks <<<"$(' "$HELPER" \
    || fail "generated helper lost the A12-05 here-string top_disks_for feed"
# A12-03: the legacy findmnt --target branch resolves the most specific
# covering mount instead of the first (host-root) match.
grep -q 'legacy_findmnt_path_match' "$HELPER" \
    || fail "generated helper lost the covering-mount match"
grep -q 'MOST SPECIFIC covering mount' "$HELPER" \
    || fail "generated helper lost the A12-03 most-specific --target comment"
# A12-02: the standard-location tool probe and the sbin PATH normalization.
grep -q '^legacy_standard_tool_path()' "$HELPER" \
    || fail "generated helper lost the standard-location tool probe"
grep -q 'BOOT_REPAIR_LEGACY_TOOL_DIRS' "$HELPER" \
    || fail "generated helper lost the probe directory override"
grep -q 'PATH="/sbin:$PATH"' "$HELPER" \
    || fail "generated helper lost the /sbin PATH normalization"
grep -q 'PATH="/usr/sbin:$PATH"' "$HELPER" \
    || fail "generated helper lost the /usr/sbin PATH normalization"
grep -q 'CRYPTSETUP_BIN="$(legacy_standard_tool_path cryptsetup)"' "$HELPER" \
    || fail "generated helper unlock gate does not use the standard-location probe"
grep -q 'Required host command not found: cryptsetup (checked /sbin/cryptsetup, /usr/sbin/cryptsetup and PATH)' "$HELPER" \
    || fail "generated helper lost the fail-closed cryptsetup probe wording"
# A12-04: the single-user variant capture/restore/verification and the
# derived-variant guard relaxation.
for grub_symbol in legacy_grub_single_variant_capture legacy_grub_restore_single_variants \
    legacy_grub_variants_verified legacy_grub_relax_derived_variants \
    legacy_grub_kernel_strip_managed legacy_grub_kernel_without_single \
    legacy_grub_entry_blocks legacy_grub_block_kernel; do
    grep -q "^${grub_symbol}()" "$HELPER" || fail "generated helper is missing $grub_symbol"
done
grep -q 'derived single-user variant' "$HELPER" \
    || fail "generated helper lost the derived single-user variant wording"
grep -q 'derived-single-variant-missing' "$HELPER" \
    || fail "generated helper lost the variant verification failure marker"
grep -q 'single-user variant(s) captured for post-regeneration restoration' "$HELPER" \
    || fail "generated helper lost the variant capture preflight note"
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
grep -q 'LEGACY_RESOLVER_MARKER="$STATE_ROOT/resolver-copy-marker"' "$HELPER" \
    || fail "generated helper lost the A12-07 persistent resolver copy marker"
grep -q 'delete $LEGACY_RESOLVER_MARKER when the two systems share the identical resolver file' "$HELPER" \
    || fail "generated helper lost the A12-07 identical-content marker remedy"
grep -q 'record the resolver copy marker' "$HELPER" \
    || fail "generated helper lost the A12-07 marker write path"
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
    # A12-01: legacy_readarray skips empty records (a here-string feed adds
    # one trailing newline, which an empty producer turns into a spurious ""
    # element that would break the top_disks_for sysfs-fallback semantics).
    legacy_readarray -t arr <<< $'one two\nthree\\four\n\nlast'
    [[ "${#arr[@]}" -eq 3 ]] || fail "legacy_readarray element count: ${#arr[@]}"
    [[ "${arr[0]}" == 'one two' ]] || fail "legacy_readarray space handling"
    [[ "${arr[1]}" == 'three\four' ]] || fail "legacy_readarray backslash handling"
    [[ "${arr[2]}" == last ]] || fail "legacy_readarray last line handling"
    legacy_readarray -t arr <<< ''
    [[ "${#arr[@]}" -eq 0 ]] || fail "legacy_readarray empty feed must yield zero elements"

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

    # The mountpoint/findmnt fallbacks resolve the mount table through
    # legacy_mountinfo_table(); pin that resolver to a fixture (the
    # documented in-test seam the shim honours in every environment) so
    # these checks never depend on the running host's mount table or its
    # device naming.  Rows use the legacy_mountinfo_table output format:
    # ID<TAB>TARGET<TAB>SOURCE<TAB>FSTYPE<TAB>OPTIONS, in mount order.  The
    # table mirrors the Etch rig: "/" first, then the private session
    # mount, then /boot; the A12-03 block below reuses the same table.
    local mountinfo_fixture
    mountinfo_fixture="$(mktemp "${TMPDIR:-/tmp}/legacy-mountinfo.XXXXXX")"
    cat > "$mountinfo_fixture" <<'MOUNTINFO'
1	/	/dev/mapper/debian1-root	ext3	rw,data=ordered
2	/var/run/boot-repair/session.OU5116/mount	/dev/mapper/debian-root	ext3	ro,data=ordered
3	/var/run/boot-repair/session.OU5116/mount/boot	/dev/hdb1	ext3	ro,data=ordered
MOUNTINFO
    legacy_mountinfo_table() { cat "$mountinfo_fixture"; }

    legacy_mountpoint -q / || fail "legacy_mountpoint /"
    legacy_mountpoint -q /var/run/boot-repair/session.OU5116/mount/boot \
        || fail "legacy_mountpoint nested mount"
    legacy_mountpoint -q /no/such/mountpoint && fail "legacy_mountpoint accepted a non-mountpoint"

    out="$(legacy_findmnt -rn -o SOURCE --target /)"
    [[ "$out" == "/dev/mapper/debian1-root" ]] \
        || fail "legacy_findmnt --target / resolved the wrong source: $out"
    out="$(legacy_findmnt -rn -o SOURCE,TARGET)"
    [[ "$(printf '%s\n' "$out" | wc -l | tr -d '[:space:]')" -eq 3 ]] \
        || fail "legacy_findmnt full listing lost rows: $out"
    printf '%s\n' "$out" | grep -q ' /$' || fail "legacy_findmnt full listing lost the / row"
    legacy_findmnt --definitely-unknown-option >/dev/null 2>&1 \
        && fail "legacy_findmnt accepted an unknown option"

    legacy_mount_rslave_supported && fail "legacy_mount_rslave_supported must fail under force"
    legacy_mount_special bogus a b && fail "legacy_mount_special accepted an unknown kind"

    # The lsblk fallback resolves device identity through the sysfs block
    # tree; pin that tree to a fixture (the same in-test seam style as the
    # mountinfo table above) so these checks never depend on the running
    # host's devices, its /proc/mounts spelling or its device naming.  The
    # queried root is spelled as the CI-runner-style alias node ("root",
    # like /dev/root) with no sysfs directory of its own: the CI failure
    # this pins came from the oracle comparing that queried spelling's
    # basename against the KNAME the shim correctly resolved through the
    # major:minor scan (nvme0n1p1 on the runner).  A fixture file stats
    # 0:0, so the fake kernel partition carries dev "0:0" and the shim
    # must name the alias nvme0n1p1 through the pinned tree.
    local sysfs_fixture dev_fixture kname one many
    sysfs_fixture="$(mktemp -d "${TMPDIR:-/tmp}/legacy-sysfs.XXXXXX")"
    dev_fixture="$sysfs_fixture/dev"
    mkdir -p "$dev_fixture" "$sysfs_fixture/nvme0n1/nvme0n1p1"
    # Kernel 2.6.18 has no /sys/class/block; its partitions are directories
    # below their disk.  The fixture keeps that Etch layout plus the flat
    # alias symlink the class/block resolver branch would return.
    ln -s "$sysfs_fixture/nvme0n1/nvme0n1p1" "$sysfs_fixture/nvme0n1p1"
    printf '%s\n' '0:0' > "$sysfs_fixture/nvme0n1/nvme0n1p1/dev"
    printf '1\n' > "$sysfs_fixture/nvme0n1/nvme0n1p1/partition"
    printf '2048\n' > "$sysfs_fixture/nvme0n1/nvme0n1p1/size"
    : > "$dev_fixture/root"
    # legacy_block_kname's first two identity probes (sysfs basename,
    # readlink -f) miss for the alias by design - "root" exists in no real
    # sysfs tree - and the stat probe reads the regular file's 0:0, the
    # identity the fake partition carries.
    legacy_sys_block_dir() { printf '%s\n' "$sysfs_fixture/$1"; }
    legacy_sys_block_all_dirs() {
        printf '%s\n' "$sysfs_fixture/nvme0n1" "$sysfs_fixture/nvme0n1/nvme0n1p1"
    }

    kname="$(legacy_lsblk -ndo KNAME "$dev_fixture/root")"
    [[ "$kname" == nvme0n1p1 ]] || fail "legacy_lsblk KNAME: $kname != nvme0n1p1"
    out="$(legacy_lsblk -ndo TYPE "$dev_fixture/root")"
    [[ "$out" == part ]] || fail "legacy_lsblk TYPE: $out"
    out="$(legacy_lsblk -srnpo NAME,TYPE "$dev_fixture/root")"
    [[ "$out" == $'/dev/nvme0n1p1 part\n/dev/nvme0n1 disk' ]] \
        || fail "legacy_lsblk inverse walk: $out"
    # FSTYPE/MOUNTPOINTS are deliberately absent from the pinned columns:
    # those shim branches consult the real blkid and /proc/self/mountinfo,
    # so asserting them here would reintroduce host dependence.
    out="$(legacy_lsblk -P -b -p -o NAME,SIZE,TYPE "$dev_fixture/root")"
    [[ "$out" == 'NAME="/dev/nvme0n1p1" SIZE="1048576" TYPE="part"' ]] \
        || fail "legacy_lsblk -P output: $out"
    # The mount-table fallback matches the major:minor identity: both the
    # queried alias file and the fixture row's source stat 0:0, so the
    # pinned row must resolve to "/" exactly.
    out="$(printf '1\t/\t%s\text3\trw\n' "$dev_fixture/root" | legacy_mountpoints_from_table "$dev_fixture/root")"
    [[ "$out" == "/" ]] || fail "legacy_mountpoints_from_table: $out"

    # Unknown options and unsupported flag letters must fail closed.
    legacy_lsblk -ndo NAME --definitely-unknown >/dev/null 2>&1 \
        && fail "legacy_lsblk accepted an unknown long option"
    legacy_lsblk -ndqo NAME "$dev_fixture/root" >/dev/null 2>&1 \
        && fail "legacy_lsblk accepted an unknown short flag"

    # -d (no children) must suppress child rows when the kernel has them:
    # give the disk the same 0:0 identity (the scan resolves the first
    # match in the pinned order, the disk) and query it through a second
    # alias shape.
    printf '%s\n' '0:0' > "$sysfs_fixture/nvme0n1/dev"
    : > "$dev_fixture/rootdisk"
    one="$(legacy_lsblk -dno NAME "$dev_fixture/rootdisk")"
    many="$(legacy_lsblk -no NAME "$dev_fixture/rootdisk")"
    [[ "$one" == nvme0n1 ]] || fail "legacy_lsblk -d did not suppress child rows: $one"
    [[ "$many" == $'nvme0n1\nnvme0n1p1' ]] \
        || fail "legacy_lsblk without -d lost child rows: $many"
    rm -rf -- "$sysfs_fixture"
    unset -f legacy_sys_block_dir legacy_sys_block_all_dirs

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

    # --- A12-03: findmnt --target resolves the MOST SPECIFIC covering mount.
    # The old first-match loop returned the host root "/" for any path below a
    # session mount, corrupting the target mount evidence (the selected
    # repair root reported the running host's source and options).  The
    # The pinned mount table above doubles as the fixture: "/" first, then
    # the private session mount, then /boot.
    out="$(legacy_findmnt -rn -o SOURCE --target /var/run/boot-repair/session.OU5116/mount)"
    [[ "$(printf '%s\n' "$out" | tail -n1)" == "/dev/mapper/debian-root" ]] \
        || fail "legacy_findmnt --target resolved the host root instead of the session mount: $out"
    out="$(legacy_findmnt -rn -o SOURCE --target /var/run/boot-repair/session.OU5116/mount/boot)"
    [[ "$(printf '%s\n' "$out" | tail -n1)" == "/dev/hdb1" ]] \
        || fail "legacy_findmnt --target did not resolve the nested /boot mount: $out"
    out="$(legacy_findmnt -rn -o SOURCE --target /var/run/boot-repair/session.OU5116/mount/usr)"
    [[ "$(printf '%s\n' "$out" | tail -n1)" == "/dev/mapper/debian-root" ]] \
        || fail "legacy_findmnt --target did not fall back to the covering session mount: $out"
    # An unmounted path under / resolves to the / entry itself.
    out="$(legacy_findmnt -rn -o SOURCE --target /no/such/mount)"
    [[ "$(printf '%s\n' "$out" | tail -n1)" == "/dev/mapper/debian1-root" ]] \
        || fail "legacy_findmnt --target did not fall back to the / entry: $out"
    # Stacked entries on the exact mountpoint are all emitted (mount order).
    out="$(legacy_findmnt -rn -o SOURCE --target /)"
    [[ "$(printf '%s\n' "$out" | wc -l | tr -d '[:space:]')" -eq 1 ]] \
        || fail "legacy_findmnt --target / lost its exact-match row: $out"
    rm -f -- "$mountinfo_fixture"
    unset -f legacy_mountinfo_table

    # --- A12-02: the standard-location tool probe (sbin dirs, then PATH) and
    # its fail-closed behaviour.
    local tool_fixture
    tool_fixture="$(mktemp -d "${TMPDIR:-/tmp}/legacy-tools.XXXXXX")"
    mkdir -p "$tool_fixture/sbin" "$tool_fixture/usr-sbin"
    : > "$tool_fixture/sbin/cryptsetup"
    chmod +x "$tool_fixture/sbin/cryptsetup"
    # Exported like the other test seams: the reader lives inside the sourced
    # compat shims, which ShellCheck cannot follow from this file.
    export BOOT_REPAIR_LEGACY_TOOL_DIRS="$tool_fixture/sbin $tool_fixture/usr-sbin"
    [[ "$(legacy_standard_tool_path cryptsetup)" == "$tool_fixture/sbin/cryptsetup" ]] \
        || fail "legacy_standard_tool_path did not probe /sbin first"
    rm -f -- "$tool_fixture/sbin/cryptsetup"
    : > "$tool_fixture/usr-sbin/cryptsetup"
    chmod +x "$tool_fixture/usr-sbin/cryptsetup"
    [[ "$(legacy_standard_tool_path cryptsetup)" == "$tool_fixture/usr-sbin/cryptsetup" ]] \
        || fail "legacy_standard_tool_path did not probe /usr/sbin second"
    rm -f -- "$tool_fixture/usr-sbin/cryptsetup"
    # Neither standard location nor PATH: fail closed with an empty path.
    if [[ -z "$(legacy_real_tool_path definitely-no-such-tool-xyz)" ]]; then
        [[ -z "$(legacy_standard_tool_path definitely-no-such-tool-xyz)" ]] \
            || fail "legacy_standard_tool_path returned a path for a missing tool"
    fi
    # A slash in the name is refused outright (path injection, fail closed).
    [[ -z "$(legacy_standard_tool_path /sbin/cryptsetup)" ]] \
        || fail "legacy_standard_tool_path accepted a path instead of a bare name"
    BOOT_REPAIR_LEGACY_TOOL_DIRS=""
    rm -rf -- "$tool_fixture"
    # PATH-only fallback still works (the host PATH resolves sh itself).
    [[ -n "$(legacy_standard_tool_path sh)" ]] \
        || fail "legacy_standard_tool_path lost its PATH fallback"
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
    # The sourced helper re-enabled pipefail; see the header note.
    set +o pipefail

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
    # The host-shell gate is firmware-dependent: on a BIOS-only machine the
    # legacy host shell is legitimately available (no firmware namespace to
    # isolate), so pin the EFI seam here - the probe's own read-only test
    # seam - to exercise the fail-closed unavailable|host-shell: refusal on
    # every machine, not just the EFI hosts the suite was written on.
    out="$(BOOT_REPAIR_LEGACY_HOST_EFI=yes PATH=/nonexistent run_host_shell /dev/null /dev/null true 2>&1)"
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
    export TARGET_ROOT
    RUNNING_HOST_MODE=0
    export RUNNING_HOST_MODE
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
