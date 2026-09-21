#!/usr/bin/env bash
# Running-host Btrfs snapshot rollback helper contract.
#
# Static wiring plus a behavioural harness that sources the helper without main
# and replaces only mount/topology plumbing, the Snapper config probe and the
# boot-stack reconciler.  No real block device, mount, subvolume or reboot is
# ever touched.
# shellcheck disable=SC1090,SC2034
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="$ROOT_DIR/scripts/boot-repair-helper.sh"

[[ -x "$HELPER" ]] || { echo "FAIL: helper is not executable" >&2; exit 1; }
bash -n "$HELPER"

# ---------------------------------------------------------------------------
# Static wiring contracts
# ---------------------------------------------------------------------------
grep -q '^  \$PROGRAM_NAME host-snapshots' "$HELPER" \
    || { echo 'FAIL: usage is missing host-snapshots' >&2; exit 1; }
grep -q '^  \$PROGRAM_NAME host-reboot' "$HELPER" \
    || { echo 'FAIL: usage is missing host-reboot' >&2; exit 1; }
grep -q 'host-fs-repair|host-snapshots|host-reboot' "$HELPER" \
    || { echo 'FAIL: the broker host allowlist is missing host-snapshots/host-reboot' >&2; exit 1; }
grep -q '^        host-snapshots)' "$HELPER" \
    || { echo 'FAIL: main() has no host-snapshots dispatch' >&2; exit 1; }
grep -q '^        host-reboot)' "$HELPER" \
    || { echo 'FAIL: main() has no host-reboot dispatch' >&2; exit 1; }
grep -q '^run_host_snapshots()' "$HELPER"
grep -q '^run_host_reboot()' "$HELPER"
grep -q '^host_snapshot_rollback_plan()' "$HELPER"
grep -q '^host_rollback_snapshot()' "$HELPER"
grep -q '^host_snapshot_restore_preserved_root()' "$HELPER"
grep -q '^host_snapshot_nested_snapshots_child()' "$HELPER"
grep -q '^host_snapshot_other_nested_children()' "$HELPER"
grep -q '^host_snapshot_resolve_target()' "$HELPER"
grep -q '^host_snapshot_rollback_unavailable_reason()' "$HELPER"
grep -q '^host_reboot_unavailable_reason()' "$HELPER"

# Capability lines are host-scope only and never become a 14th Repair key.
grep -Fq 'Host snapshot rollback: available' "$HELPER"
grep -Fq 'Host snapshot rollback: unavailable|%s' "$HELPER"
grep -Fq 'Host reboot: available' "$HELPER"
grep -Fq 'if (( RUNNING_HOST_MODE == 1 )); then' "$HELPER"
grep -q 'local -a keys=(validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub extlinux bootstack)' "$HELPER"

# The host mechanism is the name-preserving transaction, never snapper
# rollback or a bare set-default.
if grep -v '^[[:space:]]*#' "$HELPER" | grep -Eq 'snapper[^|]*rollback|snapper.*--ambit'; then
    echo 'FAIL: the helper must not delegate host rollback to snapper rollback' >&2
    exit 1
fi
grep -Fq 'HOST_ROLLBACK_RESULT=SUCCESS' "$HELPER"
grep -Fq 'HOST_ROLLBACK_REBOOT_REQUIRED=1' "$HELPER"
grep -Fq 'HOST_ROLLBACK_RECOVERY=ok' "$HELPER"
grep -Fq 'HOST_ROLLBACK_BACKUP_ROOT=%s' "$HELPER"
grep -Fq 'HOST_REBOOT_SCHEDULED=1' "$HELPER"
grep -Fq 'repair_change_status snapshots changed' "$HELPER"
# ROLLBACK_RESULT=SUCCESS stays target-only so host rollback never triggers
# the target invalidation path.
host_rollback_body="$(sed -n '/^host_rollback_snapshot()/,/^}/p' "$HELPER")"
if grep -Fq "printf 'ROLLBACK_RESULT=SUCCESS" <<<"$host_rollback_body"; then
    echo 'FAIL: host rollback must emit HOST_ROLLBACK_RESULT, not ROLLBACK_RESULT' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# Behavioural harness
# ---------------------------------------------------------------------------
sandbox="$(mktemp -d)"
cleanup_sandbox() { rm -rf -- "$sandbox"; }
trap cleanup_sandbox EXIT

mkdir -p "$sandbox/mockbin" "$sandbox/session" "$sandbox/top"
: > "$sandbox/commands.log"

cat > "$sandbox/mockbin/btrfs" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
printf 'btrfs %s\n' "$*" >> "${FAKE_COMMAND_LOG:-/dev/null}"
case "${1:-}" in
    inspect-internal)
        # rootid <path>
        printf '%s\n' "${FAKE_ROOTID:-375}"
        ;;
    subvolume)
        case "${2:-}" in
            show)
                [[ -d "${3:-}" ]] || exit 1
                printf '        Creation time: 2026-09-20 12:00:00 +0000\n'
                ;;
            list)
                if [[ -n "${FAKE_SUBVOLUME_LIST:-}" ]]; then
                    printf '%s\n' "$FAKE_SUBVOLUME_LIST"
                fi
                ;;
            get-default)
                printf '%s\n' "${FAKE_DEFAULT_LINE:-ID 5 gen 1 top level 5 path <FS_TREE>}"
                ;;
            snapshot)
                mkdir -p "${4:-}"
                cp -a "${3:-}"/. "${4:-}/" 2>/dev/null || true
                ;;
            set-default)
                printf 'set-default %s\n' "${3:-}" >> "${FAKE_COMMAND_LOG:-/dev/null}"
                ;;
            delete)
                rm -rf -- "${3:-}"
                ;;
        esac
        ;;
    property)
        printf 'ro=false\n'
        ;;
esac
exit 0
MOCK
chmod +x "$sandbox/mockbin/btrfs"

cat > "$sandbox/harness.sh" <<HARNESS
#!/usr/bin/env bash
set -euo pipefail
source <(sed '/^main "\$@"/d' "$HELPER")
trap - EXIT INT TERM HUP
PATH="$sandbox/mockbin:\$PATH"; export PATH
export FAKE_COMMAND_LOG="$sandbox/commands.log"

prepare_running_host() {
    TARGET_DISK=/dev/test-disk
    ROOT_CANONICAL=/dev/test-root
    ROOT_DEVICE=/dev/test-root
    MOUNT_BASE=/
    TARGET_ROOT=/
    TARGET_SUBVOL="@"
    RUNNING_HOST_MODE=1
    EFI_ESP_SOURCE=""
    EFI_ESP_FSTYPE=""
    SESSION_DIR="$sandbox/session"
    SESSION_LOG="$sandbox/session/session.log"
    mkdir -p "\$SESSION_DIR"
    : > "\$SESSION_LOG"
}
mount_snapshot_top() {
    SNAPSHOT_TOP="$sandbox/top"
    mkdir -p "\$SNAPSHOT_TOP"
}
mount() { printf 'mount %s\n' "\$*" >> "$sandbox/commands.log"; }
umount() { :; }
mountpoint() { return 1; }
need() { :; }
lsblk() { printf '%s\n' "\${FAKE_HOST_FSTYPE:-btrfs}"; }
snapper() { [[ "\${1:-}" == --version ]] && printf 'snapper 0.10.6\n'; return 0; }
host_package_manager_gate() { :; }
validate_snapshot_fstab_for_rollback() { return "\${FAKE_FSTAB_RC:-0}"; }
validate_snapshot_crypttab_for_rollback() { return 0; }
snapshot_has_separate_boot() { return "\${FAKE_SNAP_BOOT_RC:-1}"; }
snapshot_kernel_pair_audit() { return 0; }
snapshot_separate_subvolumes() { :; }
host_snapshot_snapper_config_ok() { return "\${FAKE_SNAPPER_CONFIG_RC:-0}"; }
host_snapshot_running_path() { printf '%s\n' "\${FAKE_RUNNING_PATH:-@}"; }
host_snapshot_subvolid_pinned() { return "\${FAKE_SUBVOLID_RC:-1}"; }
host_snapshot_has_separate_boot() { return "\${FAKE_BOOT_RC:-1}"; }
host_snapshot_timeshift_managed() { return "\${FAKE_TIMESHIFT_RC:-1}"; }
host_snapshot_other_nested_children() { [[ -n "\${FAKE_NESTED_CHILD:-}" ]] && printf '%s\n' "\$FAKE_NESTED_CHILD"; return 0; }
host_snapshot_nested_snapshots_child() {
    if [[ "\${FAKE_NESTED_SNAPSHOTS_RC:-auto}" == "auto" ]]; then
        [[ -n "\${SNAPSHOT_TOP:-}" && -d "\$SNAPSHOT_TOP/@/.snapshots" ]]
    else
        return "\$FAKE_NESTED_SNAPSHOTS_RC"
    fi
}
host_snapshot_free_space_ok() { return "\${FAKE_SPACE_RC:-0}"; }
target_path_is_mounted_rw() { return "\${FAKE_RW_RC:-0}"; }
btrfs_subvol_id() {
    case "\${1:-}" in
        /) printf '%s\n' "\${FAKE_RUNNING_ID:-375}" ;;
        "\$SNAPSHOT_TOP/@") printf '%s\n' "\${FAKE_AT_ID:-256}" ;;
        */snapshot) printf '%s\n' "\${FAKE_SNAP_ID:-101}" ;;
        *) printf '200\n' ;;
    esac
}
prepare_host_command_guard() { printf 'guard\n' >> "$sandbox/commands.log"; }
host_snapshot_mount_promoted_root_rw() { printf 'mount-promoted\n' >> "$sandbox/commands.log"; }
snapshot_post_switch_reconcile() {
    local count=0
    [[ -f "$sandbox/reconcile-count" ]] && count="\$(cat "$sandbox/reconcile-count")"
    count=\$((count + 1))
    printf '%s\n' "\$count" > "$sandbox/reconcile-count"
    if (( count == 1 )); then
        return "\${FAKE_RECONCILE_RC:-0}"
    fi
    return 0
}
rm -f "$sandbox/reconcile-count"
eval "\${HARNESS_CODE:?missing HARNESS_CODE}"
HARNESS

run_harness() {
    PATH="$sandbox/mockbin:$PATH" HARNESS_CODE="$1" BOOT_REPAIR_STATE_ROOT="$sandbox/state" \
        bash --noprofile --norc "$sandbox/harness.sh"
}

# Build one complete nested Snapper @ fixture: running @ with a nested
# @/.snapshots child holding snapshot 1.
build_nested_fixture() {
    local top="$sandbox/top"
    rm -rf -- "$top"
    mkdir -p "$top/@/etc" "$top/@/.snapshots/1/snapshot/etc"
    printf 'ID=test\nPRETTY_NAME="Contract Test"\n' > "$top/@/etc/os-release"
    printf '/dev/test-root / btrfs subvol=@ 0 1\n' > "$top/@/etc/fstab"
    printf 'ID=test\nPRETTY_NAME="Contract Test"\n' > "$top/@/.snapshots/1/snapshot/etc/os-release"
    printf '/dev/test-root / btrfs subvol=@ 0 1\n' > "$top/@/.snapshots/1/snapshot/etc/fstab"
    cat > "$top/@/.snapshots/1/info.xml" <<'XML'
<?xml version="1.0"?>
<snapshot>
  <type>single</type>
  <description>contract test snapshot</description>
</snapshot>
XML
}

# ---------------------------------------------------------------------------
# 1. Non-Btrfs host list is informational and exits 0.
# ---------------------------------------------------------------------------
non_btrfs="$(FAKE_HOST_FSTYPE=ext4 run_harness 'run_host_snapshots list')"
grep -Fq 'SNAPSHOT_INVENTORY_NOT_APPLICABLE=1' <<<"$non_btrfs" \
    || { echo 'FAIL: non-Btrfs host list did not emit the informational marker' >&2; printf '%s\n' "$non_btrfs" >&2; exit 1; }
if FAKE_HOST_FSTYPE=ext4 run_harness 'run_host_snapshots plan 1' >/dev/null 2>&1; then
    echo 'FAIL: host plan must fail on a non-Btrfs root' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# 2. Refusal matrix.  Every check fails closed with a stable reason and no
#    PLAN_OK marker.
# ---------------------------------------------------------------------------
expect_refusal() {
    local label="$1" code="$2" needle="$3" output
    build_nested_fixture
    if output="$(run_harness "$code" 2>&1)"; then
        echo "FAIL: $label was accepted" >&2
        printf '%s\n' "$output" >&2
        exit 1
    fi
    grep -Fq "$needle" <<<"$output" \
        || { echo "FAIL: $label refusal reason is missing '$needle'" >&2; printf '%s\n' "$output" >&2; exit 1; }
    if grep -Fq 'PLAN_OK=1' <<<"$output"; then
        echo "FAIL: $label emitted PLAN_OK=1 on refusal" >&2
        exit 1
    fi
}

plan_code='run_host_snapshots plan 1'
expect_refusal 'missing snapper config' 'FAKE_SNAPPER_CONFIG_RC=1; run_host_snapshots plan 1' \
    'No Snapper root configuration'
expect_refusal 'running from a snapshot' "FAKE_RUNNING_PATH='@/.snapshots/1/snapshot'; run_host_snapshots plan 1" \
    'not the top-level @'
expect_refusal 'subvolid pin' 'FAKE_SUBVOLID_RC=0; run_host_snapshots plan 1' \
    'pins subvolid='
expect_refusal 'separate /boot' 'FAKE_BOOT_RC=0; run_host_snapshots plan 1' \
    'separate /boot'
expect_refusal 'Timeshift inventory' 'FAKE_TIMESHIFT_RC=0; run_host_snapshots plan 1' \
    'Timeshift'
expect_refusal 'other nested child' 'FAKE_NESTED_CHILD="@/var/lib/machines"; run_host_snapshots plan 1' \
    'nested subvolume'
expect_refusal 'read-only root' 'FAKE_RW_RC=1; run_host_snapshots plan 1' \
    'read-only'
expect_refusal 'insufficient free space' 'FAKE_SPACE_RC=1; run_host_snapshots plan 1' \
    'Less than 1 GiB'
expect_refusal 'snapshot fstab incompatibility' 'FAKE_FSTAB_RC=1; run_host_snapshots plan 1' \
    'fstab is not compatible'
expect_refusal 'snapshot separate /boot' 'FAKE_SNAP_BOOT_RC=0; run_host_snapshots plan 1' \
    'separate /boot entry'
expect_refusal 'active snapshot' 'FAKE_RUNNING_ID=101; FAKE_SNAP_ID=101; run_host_snapshots plan 1' \
    'currently running root'

# A pre snapshot is refused as a rollback target.
build_nested_fixture
sed -i 's#<type>single</type>#<type>pre</type>#' "$sandbox/top/@/.snapshots/1/info.xml"
pre_output="$(run_harness 'run_host_snapshots plan 1' 2>&1 || true)"
grep -Fq "Snapper 'pre' snapshot" <<<"$pre_output" \
    || { echo 'FAIL: pre snapshot refusal reason is missing' >&2; printf '%s\n' "$pre_output" >&2; exit 1; }

# A foreign (home) configuration alone is not a root configuration: the
# capability probe reports the missing root configuration.
build_nested_fixture
home_only="$(run_harness 'FAKE_SNAPPER_CONFIG_RC=1; host_snapshot_rollback_unavailable_reason' 2>&1 || true)"
grep -Fq 'no Snapper root configuration' <<<"$home_only" \
    || { echo 'FAIL: missing root configuration reason is missing' >&2; printf '%s\n' "$home_only" >&2; exit 1; }

# ---------------------------------------------------------------------------
# 3. Plan succeeds only after every check and creates nothing.
# ---------------------------------------------------------------------------
build_nested_fixture
top_before="$(find "$sandbox/top" -printf '%P %y\n' | LC_ALL=C sort)"
plan_output="$(run_harness "$plan_code")"
grep -Fq 'PLAN_OK=1' <<<"$plan_output" \
    || { echo 'FAIL: plan did not emit PLAN_OK=1' >&2; printf '%s\n' "$plan_output" >&2; exit 1; }
grep -Fq 'RUNNING-HOST BTRFS ROLLBACK PLAN' <<<"$plan_output" \
    || { echo 'FAIL: plan header is missing' >&2; exit 1; }
grep -Fq 'A reboot is required and is never performed automatically.' <<<"$plan_output" \
    || { echo 'FAIL: plan must state the reboot requirement' >&2; exit 1; }
grep -Fq 'Nested @/.snapshots child subvolume: will be migrated' <<<"$plan_output" \
    || { echo 'FAIL: nested migration evidence is missing' >&2; exit 1; }
top_after="$(find "$sandbox/top" -printf '%P %y\n' | LC_ALL=C sort)"
[[ "$top_before" == "$top_after" ]] \
    || { echo 'FAIL: the host plan modified the snapshot tree' >&2; diff <(printf '%s\n' "$top_before") <(printf '%s\n' "$top_after") >&2; exit 1; }

# ---------------------------------------------------------------------------
# 4. Rollback transaction: candidate -> preserve -> promote -> migrate ->
#    set-default -> reconcile -> success evidence.
# ---------------------------------------------------------------------------
build_nested_fixture
: > "$sandbox/commands.log"
rollback_output="$(run_harness 'FAKE_NESTED_SNAPSHOTS_RC=0; run_host_snapshots rollback 1')"
grep -Fq 'HOST_ROLLBACK_RESULT=SUCCESS' <<<"$rollback_output" \
    || { echo 'FAIL: rollback success evidence is missing' >&2; printf '%s\n' "$rollback_output" >&2; exit 1; }
grep -Fq 'HOST_ROLLBACK_REBOOT_REQUIRED=1' <<<"$rollback_output" \
    || { echo 'FAIL: rollback must require a reboot' >&2; exit 1; }
grep -Fq 'HOST_ROLLBACK_NEW_ROOT=@' <<<"$rollback_output" \
    || { echo 'FAIL: promoted root evidence is missing' >&2; exit 1; }
grep -Fq 'Repair change status snapshots: changed' <<<"$rollback_output" \
    || { echo 'FAIL: rollback change status is missing' >&2; exit 1; }
grep -Fq 'set-default 256' "$sandbox/commands.log" \
    || { echo 'FAIL: promoted @ was not set as the Btrfs default' >&2; cat "$sandbox/commands.log" >&2; exit 1; }
[[ -f "$sandbox/top/@/.snapshots/1/info.xml" ]] \
    || { echo 'FAIL: the nested @/.snapshots child was not migrated into the promoted @' >&2; exit 1; }
[[ -f "$sandbox/top/@/etc/os-release" ]] \
    || { echo 'FAIL: the promoted @ is missing its root metadata' >&2; exit 1; }
backup_count="$(find "$sandbox/top" -mindepth 1 -maxdepth 1 -type d -name '@rollback-before-*' | wc -l)"
[[ "$backup_count" == 1 ]] \
    || { echo 'FAIL: the running @ was not preserved as one @rollback-before-* root' >&2; exit 1; }
if find "$sandbox/top" -mindepth 1 -maxdepth 1 -type d -name '@rollback-new-*' | grep -q .; then
    echo 'FAIL: a rollback candidate was left behind after success' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# 5. Rollback failure restores the preserved root and reports recovery.
# ---------------------------------------------------------------------------
build_nested_fixture
: > "$sandbox/commands.log"
if failed_output="$(run_harness 'FAKE_RECONCILE_RC=1; run_host_snapshots rollback 1' 2>&1)"; then
    echo 'FAIL: a failing reconcile must fail the host rollback' >&2
    exit 1
fi
grep -Fq 'HOST_ROLLBACK_RECOVERY=ok' <<<"$failed_output" \
    || { echo 'FAIL: automatic recovery was not reported as ok' >&2; printf '%s\n' "$failed_output" >&2; exit 1; }
[[ -f "$sandbox/top/@/etc/os-release" && -f "$sandbox/top/@/.snapshots/1/info.xml" ]] \
    || { echo 'FAIL: recovery did not restore a complete preserved @' >&2; exit 1; }
if grep -Fq 'HOST_ROLLBACK_RESULT=SUCCESS' <<<"$failed_output"; then
    echo 'FAIL: a failed rollback reported success' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# 6. Undo target: an existing @rollback-before-* subvolume is a valid target
#    and an arbitrary name is refused.
# ---------------------------------------------------------------------------
build_nested_fixture
mkdir -p "$sandbox/top/@rollback-before-20260920-120000/etc"
printf 'ID=test\nPRETTY_NAME="Contract Test"\n' > "$sandbox/top/@rollback-before-20260920-120000/etc/os-release"
printf '/dev/test-root / btrfs subvol=@ 0 1\n' > "$sandbox/top/@rollback-before-20260920-120000/etc/fstab"
undo_output="$(run_harness 'run_host_snapshots plan @rollback-before-20260920-120000')"
grep -Fq 'PLAN_OK=1' <<<"$undo_output" \
    || { echo 'FAIL: an existing undo point was not accepted as a rollback target' >&2; printf '%s\n' "$undo_output" >&2; exit 1; }
grep -Fq 'Snapper type: rollback-backup' <<<"$undo_output" \
    || { echo 'FAIL: the undo plan does not name the rollback backup type' >&2; exit 1; }
if run_harness 'run_host_snapshots plan @rollback-before-evil' >/dev/null 2>&1; then
    echo 'FAIL: an arbitrary @rollback-before-* name was accepted' >&2
    exit 1
fi

# The undo transaction preserves the currently running @ as a new backup.
: > "$sandbox/commands.log"
undo_rollback="$(run_harness 'run_host_snapshots rollback @rollback-before-20260920-120000')"
grep -Fq 'HOST_ROLLBACK_RESULT=SUCCESS' <<<"$undo_rollback" \
    || { echo 'FAIL: the undo rollback did not succeed' >&2; printf '%s\n' "$undo_rollback" >&2; exit 1; }
[[ -f "$sandbox/top/@/etc/os-release" ]] || { echo 'FAIL: undo did not promote @' >&2; exit 1; }
[[ -f "$sandbox/top/@rollback-before-20260920-120000/etc/os-release" ]] \
    || { echo 'FAIL: the selected undo point was consumed' >&2; exit 1; }

# ---------------------------------------------------------------------------
# 7. Host reboot: explicit mechanisms only, never an unprivileged fallback.
# ---------------------------------------------------------------------------
reboot_body="$(sed -n '/^run_host_reboot()/,/^}/p' "$HELPER")"
grep -q 'systemctl reboot --no-block' <<<"$reboot_body"
grep -q 'rc-shutdown -r now' <<<"$reboot_body"
grep -q 'HOST_REBOOT_SCHEDULED=1' <<<"$reboot_body"
if grep -Eq 'systemctl --user|sudo ' <<<"$reboot_body"; then
    echo 'FAIL: host-reboot must not fall back to an unprivileged mechanism' >&2
    exit 1
fi

# host-reboot is unreachable without a running-host identity: the helper
# re-proves the selected disk backs / before scheduling anything.
grep -q 'prepare_running_host "\$raw_disk" "\$raw_root" no' <<<"$reboot_body" \
    || { echo 'FAIL: host-reboot does not re-prove the running-host identity' >&2; exit 1; }

echo "PASS: host snapshot rollback helper contract is wired, fail-closed and transactional."
