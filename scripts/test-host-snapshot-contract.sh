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

# Host-only capability lines never become a Repair tool key; the storage-stack
# markers are scope-neutral and must be present for every scope.
grep -Fq 'Host snapshot rollback: available' "$HELPER"
grep -Fq 'Host snapshot rollback: unavailable|%s' "$HELPER"
grep -Fq 'Host reboot: available' "$HELPER"
grep -Fq 'Storage lvm inspection: available' "$HELPER"
grep -Fq 'Storage software raid: available' "$HELPER"
grep -Fq 'if (( RUNNING_HOST_MODE == 1 )); then' "$HELPER"
grep -q 'local -a keys=(validate filesystem dpkg fixbroken aptupdate upgrade dkms display gpu initramfs efi grub extlinux bootstack kernels)' "$HELPER"

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
snapper() {
    if [[ "\${1:-}" == --version ]]; then printf 'snapper 0.10.6\n'; return 0; fi
    if [[ "\${*}" == *--no-dbus*list* && "\${FAKE_SNAPPER_LIST_RC:-0}" != 0 ]]; then
        return "\$FAKE_SNAPPER_LIST_RC"
    fi
    return 0
}
pgrep() { return "\${FAKE_SNAPPER_PROC_RC:-1}"; }
host_package_manager_gate() { return "\${FAKE_PKG_GATE_RC:-0}"; }
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
expect_refusal 'other nested child' 'FAKE_NESTED_CHILD="@/data"; run_host_snapshots plan 1' \
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
expect_refusal 'snapper list query failure' 'FAKE_SNAPPER_LIST_RC=1; run_host_snapshots plan 1' \
    'could not be queried'
expect_refusal 'concurrent snapper process' 'FAKE_SNAPPER_PROC_RC=0; run_host_snapshots plan 1' \
    'Another snapper command is running'
expect_refusal 'active package-manager lock' 'FAKE_PKG_GATE_RC=1; run_host_snapshots plan 1' \
    'package manager or package-manager lock is active'
expect_refusal 'missing snapshot' 'run_host_snapshots plan 999' \
    'was not found'
expect_refusal 'missing Snapper info.xml' \
    "rm -f '$sandbox/top/@/.snapshots/1/info.xml'; run_host_snapshots plan 1" \
    'has no Snapper info.xml metadata'
expect_refusal 'invalid snapshot root' \
    "rm -f '$sandbox/top/@/.snapshots/1/snapshot/etc/os-release'; run_host_snapshots plan 1" \
    'not a valid Linux root snapshot'
expect_refusal 'snapshot is the default subvolume' \
    "export FAKE_DEFAULT_LINE='ID 101 gen 1 top level 5 path @/.snapshots/1/snapshot'; run_host_snapshots plan 1" \
    'current Btrfs default subvolume'

# A pre or post snapshot is refused as a rollback target.
for snapshot_type in pre post; do
    build_nested_fixture
    sed -i "s#<type>single</type>#<type>$snapshot_type</type>#" "$sandbox/top/@/.snapshots/1/info.xml"
    type_output="$(run_harness 'run_host_snapshots plan 1' 2>&1 || true)"
    grep -Fq "Snapper '$snapshot_type' snapshot" <<<"$type_output" \
        || { echo "FAIL: $snapshot_type snapshot refusal reason is missing" >&2; printf '%s\n' "$type_output" >&2; exit 1; }
done

# Capability reasons are probe-based too: a non-Btrfs root and an active
# package-manager lock each name their own missing prerequisite.
non_btrfs_reason="$(run_harness 'FAKE_HOST_FSTYPE=ext4; host_snapshot_rollback_unavailable_reason' 2>&1 || true)"
grep -Fq 'reason:not-btrfs-root' <<<"$non_btrfs_reason" \
    || { echo 'FAIL: non-Btrfs capability reason is missing' >&2; printf '%s\n' "$non_btrfs_reason" >&2; exit 1; }
lock_reason="$(run_harness 'FAKE_PKG_GATE_RC=1; host_snapshot_rollback_unavailable_reason' 2>&1 || true)"
grep -Fq 'reason:package-lock-active' <<<"$lock_reason" \
    || { echo 'FAIL: package-lock capability reason is missing' >&2; printf '%s\n' "$lock_reason" >&2; exit 1; }

# A foreign (home) configuration alone is not a root configuration: the
# capability probe reports the missing root configuration.
build_nested_fixture
home_only="$(run_harness 'FAKE_SNAPPER_CONFIG_RC=1; host_snapshot_rollback_unavailable_reason' 2>&1 || true)"
grep -Fq 'reason:no-snapper-root-config' <<<"$home_only" \
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

# ---------------------------------------------------------------------------
# 8. ZFS-on-root running-host detection and ZFS-aware boot-chain capability.
#    A ZFS-on-root host resolves `/` via findmnt to a dataset (rpool/ROOT/...)
#    rather than a block device.  The running-host root assertion must map the
#    dataset to its pool vdev so the read-only and package host stages proceed.
#    The boot-chain stages are now ZFS-aware instead of fail-closed: initramfs
#    requires update-initramfs + the zfs-initramfs hook, grub requires the GRUB
#    ZFS module + a resolvable root pool, efi keeps the standard ESP preflight
#    and boot-stack follows its constituents.  A missing prerequisite still
#    fails closed with a probe-based reason (never a distribution, never a
#    blanket "not ZFS-aware" refusal).
# ---------------------------------------------------------------------------
# Static wiring: the resolver and the per-stage ZFS probes must be present.
for fn in zfs_pool_backing_devices zfs_dataset_backing_block \
    running_host_root_is_zfs_dataset running_host_zfs_root_pool \
    running_host_zfs_root_resolvable target_zfs_initramfs_hook_present \
    target_grub_zfs_module_present; do
    grep -q "^${fn}()" "$HELPER" \
        || { echo "FAIL: missing helper function: $fn" >&2; exit 1; }
done
assert_body="$(sed -n '/^assert_target_is_running_host()/,/^}/p' "$HELPER")"
grep -Fq 'zfs_dataset_backing_block "$root_source" "$root_device"' <<<"$assert_body" \
    || { echo 'FAIL: the running-host root assertion does not map a ZFS dataset to its vdev' >&2; exit 1; }
initramfs_body="$(sed -n '/^initramfs_unavailable_reason()/,/^}/p' "$HELPER")"
grep -Fq 'target_root_is_zfs_dataset' <<<"$initramfs_body" \
    || { echo 'FAIL: initramfs availability is not ZFS-aware' >&2; exit 1; }
grep -Fq 'target_zfs_initramfs_hook_present' <<<"$initramfs_body" \
    || { echo 'FAIL: initramfs does not require the zfs-initramfs hook on ZFS root' >&2; exit 1; }
grub_body="$(sed -n '/^grub_unavailable_reason()/,/^}/p' "$HELPER")"
grep -Fq 'target_grub_zfs_module_present' <<<"$grub_body" \
    || { echo 'FAIL: grub does not require the GRUB ZFS module on ZFS root' >&2; exit 1; }
grep -Fq 'target_root_zfs_resolvable' <<<"$grub_body" \
    || { echo 'FAIL: grub does not require a resolvable ZFS root pool' >&2; exit 1; }
# The blanket "not ZFS-aware" refusal must be gone.
if grep -q 'is not ZFS-aware' "$HELPER"; then
    echo 'FAIL: the blanket "not ZFS-aware" ZFS-on-root refusal is still present' >&2
    exit 1
fi

mkdir -p "$sandbox/state"
cat > "$sandbox/zfs-harness.sh" <<'ZFSHARNESS'
#!/usr/bin/env bash
set -Eeuo pipefail
source <(sed '/^main "\$@"/d' "${HELPER:?}")
trap - EXIT INT TERM HUP

# ZFS-on-root topology fixture (no real device is touched).  /dev/vda is the
# selected host disk (rpool vdev on /dev/vda4, bpool on /dev/vda2, ESP on
# /dev/vda1); /dev/vdb is a foreign disk used to prove the same-disk rule.
is_block_device() {
    case "$1" in
        /dev/vda|/dev/vda1|/dev/vda2|/dev/vda3|/dev/vda4|/dev/vdb|/dev/vdb1|/dev/vdb2) return 0 ;;
        *) return 1 ;;
    esac
}
canonical_block() { is_block_device "$1" && printf '%s\n' "$1"; }
top_disks_for() {
    case "$1" in
        /dev/vda|/dev/vda1|/dev/vda2|/dev/vda3|/dev/vda4) printf '%s\n' /dev/vda ;;
        /dev/vdb|/dev/vdb1|/dev/vdb2) printf '%s\n' /dev/vdb ;;
        *) return 1 ;;
    esac
}
same_single_top_disk() {
    local -a a=() b=()
    mapfile -t a < <(top_disks_for "$1" 2>/dev/null | sort -u || true)
    mapfile -t b < <(top_disks_for "$2" 2>/dev/null | sort -u || true)
    [[ ${#a[@]} -eq 1 && ${#b[@]} -eq 1 && "${a[0]}" == "${b[0]}" ]]
}
need() { :; }
mountpoint() { return 1; }
mapper_aliases_for_device() { return 0; }
read_target_os() { TARGET_OS_ID=ubuntu; TARGET_OS_LIKE=debian; TARGET_PRETTY='Ubuntu Contract Test'; }
detect_mounted_esp() { EFI_ESP_SOURCE=/dev/vda1; EFI_ESP_FSTYPE=vfat; TARGET_ESP_MOUNT=/boot/efi; }
current_btrfs_subvol() { return 1; }

findmnt() {
    local target="" columns=""
    while (($#)); do
        case "$1" in
            --target) target="$2"; shift 2 ;;
            -o) columns="$2"; shift 2 ;;
            *) shift ;;
        esac
    done
    case "$target:$columns" in
        "/:SOURCE") printf '%s\n' "${FAKE_ROOT_SOURCE:-rpool/ROOT/ubuntu_qev764}" ;;
        "/:SOURCE,FSTYPE") printf '%s %s\n' "${FAKE_ROOT_SOURCE:-rpool/ROOT/ubuntu_qev764}" "${FAKE_ROOT_FSTYPE:-zfs}" ;;
        "/boot:SOURCE") printf 'bpool/BOOT/ubuntu_qev764\n' ;;
        "/boot:SOURCE,FSTYPE") printf 'bpool/BOOT/ubuntu_qev764 zfs\n' ;;
        "/boot/efi:SOURCE") printf '/dev/vda1\n' ;;
        "/boot/efi:SOURCE,FSTYPE") printf '/dev/vda1 vfat\n' ;;
    esac
    # Round 42: the ZFS boot-chain gates probe the SELECTED scope's root mount
    # ($TARGET_ROOT — the running host "/" or the capability fixture's pinned
    # root), never the recovery host alone.  Answer the fixture root with the
    # same dataset source as the running host so the 8d/8e capability drills
    # exercise the scope-scoped probe.
    if [[ -n "${ZFS_FIXTURE_ROOT:-}" && "$target" == "$ZFS_FIXTURE_ROOT" && "$columns" == SOURCE ]]; then
        printf '%s\n' "${FAKE_ROOT_SOURCE:-rpool/ROOT/ubuntu_qev764}"
    fi
    return 0
}

lsblk() {
    local args="$*"
    case "$args" in
        *'-ndo FSTYPE'*)
            case "$args" in
                *vda4*) printf 'zfs_member\n' ;;
                *vda2*) printf 'zfs_member\n' ;;
                *vda1*) printf 'vfat\n' ;;
                *) printf '\n' ;;
            esac
            ;;
        *'-prno NAME,TYPE,FSTYPE'*)
            printf '/dev/vda1 part vfat\n'
            printf '/dev/vda2 part zfs_member\n'
            printf '/dev/vda3 part swap\n'
            printf '/dev/vda4 part zfs_member\n'
            printf '/dev/vda disk \n'
            ;;
    esac
    return 0
}

zpool() {
    case "$*" in
        "status -P rpool"*)
            printf '  pool: rpool\n state: ONLINE\nconfig:\n\n\tNAME        STATE     READ WRITE CKSUM\n\trpool       ONLINE       0     0     0\n\t  /dev/vda4  ONLINE       0     0     0\n'
            ;;
        "list -v -H -P -o name rpool"*)
            printf 'rpool\n  /dev/vda4\n'
            ;;
        "list -H -o name"*) printf 'rpool\nbpool\n' ;;
        "list -H -o guid"*) printf '1234567890\n' ;;
    esac
    return 0
}

# Host-scope tail capability lines are a separate feature and are pinned so the
# ZFS-on-root capability assertions stay machine-independent.
host_snapshot_rollback_evidence() { printf 'snapper not detected; running root unknown; snapshot store not mounted'; }
host_snapshot_rollback_unavailable_reason() { printf 'the running host root filesystem is zfs_member, not Btrfs'; return 1; }
host_reboot_unavailable_reason() { printf 'no supported reboot mechanism in the contract fixture'; return 1; }
host_default_unavailable_reason() { printf 'no default-selection probe in the contract fixture'; return 1; }
host_default_diagnostic_evidence() { return 0; }

eval "${HARNESS_CODE:?missing HARNESS_CODE}"
ZFSHARNESS
chmod +x "$sandbox/zfs-harness.sh"

run_zfs_harness() {
    HELPER="$HELPER" BOOT_REPAIR_STATE_ROOT="$sandbox/state" HARNESS_CODE="$1" \
        bash --noprofile --norc "$sandbox/zfs-harness.sh"
}

# 8a. The running-host identity check accepts the ZFS dataset root by mapping it
#     to /dev/vda4 (the pool vdev the caller supplied) and resolving it to the
#     single physical disk /dev/vda.
za_out="$(run_zfs_harness '
    prepare_running_host /dev/vda /dev/vda4 no
    printf "ROOT_CANONICAL=%s ROOT_DEVICE=%s\n" "$ROOT_CANONICAL" "$ROOT_DEVICE"
' 2>&1)"
grep -Fq 'msg:running-host-identity-pass' <<<"$za_out" \
    || { echo 'FAIL: ZFS-on-root running host was not accepted by the identity check' >&2; printf '%s\n' "$za_out" >&2; exit 1; }
grep -Fq 'ROOT_CANONICAL=/dev/vda4 ROOT_DEVICE=/dev/vda4' <<<"$za_out" \
    || { echo 'FAIL: the ZFS dataset root did not resolve to its pool vdev' >&2; printf '%s\n' "$za_out" >&2; exit 1; }

# 8a-whole. ZFS-on-root accepts the whole disk as the root component: the
#     unprivileged caller cannot tell the rpool vdev (/dev/vda4) from the bpool
#     partition (/dev/vda2) via lsblk (both zfs_member), so it may name the
#     whole disk /dev/vda.  prepare_running_host must re-resolve it to the pool
#     vdev before the fstype check, so the host stages run against zfs_member
#     (/dev/vda4) instead of failing on the FSTYPE-less disk.
za_whole="$(run_zfs_harness '
    prepare_running_host /dev/vda /dev/vda no
    printf "ROOT_CANONICAL=%s ROOT_DEVICE=%s\n" "$ROOT_CANONICAL" "$ROOT_DEVICE"
' 2>&1)"
grep -Fq 'msg:running-host-identity-pass' <<<"$za_whole" \
    || { echo 'FAIL: ZFS-on-root whole-disk host was not accepted' >&2; printf '%s\n' "$za_whole" >&2; exit 1; }
grep -Fq 'ROOT_CANONICAL=/dev/vda4 ROOT_DEVICE=/dev/vda4' <<<"$za_whole" \
    || { echo 'FAIL: the whole-disk root component was not re-resolved to its pool vdev' >&2; printf '%s\n' "$za_whole" >&2; exit 1; }
if grep -Fq 'not an unlocked filesystem' <<<"$za_whole"; then
    echo 'FAIL: the whole-disk root component failed the fstype check' >&2; printf '%s\n' "$za_whole" >&2; exit 1
fi

# 8b. A root component on a DIFFERENT disk still fails closed: /dev/vdb resolves
#     to a different top-level disk than the verified /dev/vda, even though the
#     ZFS-on-root relaxation no longer demands the exact pool vdev match.
zb_reject="$(run_zfs_harness '
    assert_target_is_running_host /dev/vda /dev/vdb
' 2>&1 || true)"
grep -Fq 'Supplied root component does not belong to the selected host disk.' <<<"$zb_reject" \
    || { echo 'FAIL: a root component on a different disk was accepted for a ZFS dataset root' >&2; printf '%s\n' "$zb_reject" >&2; exit 1; }

# 8b-ext4. A non-ZFS (ext4) root keeps the exact-match requirement: the relaxed
#     ZFS-on-root rule must never weaken the identity check for conventional
#     block-device roots.  The exact root component is accepted, the whole disk
#     is not.
z_ext4_accept="$(run_zfs_harness '
    FAKE_ROOT_SOURCE=/dev/vda2
    FAKE_ROOT_FSTYPE=ext4
    assert_target_is_running_host /dev/vda /dev/vda2
    printf "EXT4_EXACT_ACCEPTED\n"
' 2>&1)"
grep -Fq 'EXT4_EXACT_ACCEPTED' <<<"$z_ext4_accept" \
    || { echo 'FAIL: an ext4 host rejected its exact root component' >&2; printf '%s\n' "$z_ext4_accept" >&2; exit 1; }
z_ext4_reject="$(run_zfs_harness '
    FAKE_ROOT_SOURCE=/dev/vda2
    FAKE_ROOT_FSTYPE=ext4
    assert_target_is_running_host /dev/vda /dev/vda
' 2>&1 || true)"
grep -Fq 'Supplied root component does not match the currently running root filesystem.' <<<"$z_ext4_reject" \
    || { echo 'FAIL: an ext4 host accepted a non-exact root component' >&2; printf '%s\n' "$z_ext4_reject" >&2; exit 1; }

# 8b-ext4-prep. The ZFS re-resolution in prepare_running_host is a no-op for a
#     conventional ext4 root: running_host_root_is_zfs_dataset is false for a
#     block-device source, so ROOT_CANONICAL stays exactly as supplied and the
#     fstype check still passes on the caller's own component.
z_ext4_prep="$(run_zfs_harness '
    FAKE_ROOT_SOURCE=/dev/vda2
    FAKE_ROOT_FSTYPE=ext4
    lsblk() { [[ "$*" == *"-ndo FSTYPE"* ]] && printf "ext4\n"; }
    prepare_running_host /dev/vda /dev/vda2 no
    printf "ROOT_CANONICAL=%s ROOT_DEVICE=%s\n" "$ROOT_CANONICAL" "$ROOT_DEVICE"
' 2>&1)"
grep -Fq 'msg:running-host-identity-pass' <<<"$z_ext4_prep" \
    || { echo 'FAIL: an ext4 host was not accepted by prepare_running_host' >&2; printf '%s\n' "$z_ext4_prep" >&2; exit 1; }
grep -Fq 'ROOT_CANONICAL=/dev/vda2 ROOT_DEVICE=/dev/vda2' <<<"$z_ext4_prep" \
    || { echo 'FAIL: the ext4 root component was altered by the ZFS re-resolution' >&2; printf '%s\n' "$z_ext4_prep" >&2; exit 1; }

# 8c. The lsblk fallback (no zpool present) still resolves the root through the
#     caller's zfs_member vdev partition.
zfallback_out="$(run_zfs_harness '
    zpool() { return 1; }
    prepare_running_host /dev/vda /dev/vda4 no
    printf "FALLBACK_OK ROOT_CANONICAL=%s\n" "$ROOT_CANONICAL"
' 2>&1)"
grep -Fq 'FALLBACK_OK ROOT_CANONICAL=/dev/vda4' <<<"$zfallback_out" \
    || { echo 'FAIL: the zfs_member lsblk fallback did not resolve the ZFS dataset root' >&2; printf '%s\n' "$zfallback_out" >&2; exit 1; }

# 8d. Capability lines: the ZFS root is accepted for read-only + package
#     stages, the ZFS-aware boot-chain stages (initramfs, grub, efi, bootstack)
#     are available when their ZFS prerequisites are present, and the
#     filesystem (zpool check/scrub) capability is available.
mkdir -p "$sandbox/zfs-root"/usr/bin "$sandbox/zfs-root"/usr/sbin \
    "$sandbox/zfs-root"/usr/lib/systemd/system "$sandbox/zfs-root"/var/lib/dpkg \
    "$sandbox/zfs-root"/etc/apt "$sandbox/zfs-root"/boot/grub \
    "$sandbox/zfs-root"/usr/share/initramfs-tools/hooks \
    "$sandbox/zfs-root"/usr/lib/grub/x86_64-efi
printf 'ID=ubuntu\nID_LIKE=debian\nPRETTY_NAME="Ubuntu Contract Test"\n' > "$sandbox/zfs-root/etc/os-release"
for tool in dpkg apt-get dkms; do : > "$sandbox/zfs-root/usr/bin/$tool"; chmod +x "$sandbox/zfs-root/usr/bin/$tool"; done
: > "$sandbox/zfs-root/usr/sbin/update-initramfs"; chmod +x "$sandbox/zfs-root/usr/sbin/update-initramfs"
: > "$sandbox/zfs-root/usr/sbin/mkinitramfs"; chmod +x "$sandbox/zfs-root/usr/sbin/mkinitramfs"
: > "$sandbox/zfs-root/usr/bin/grub-mkconfig"; chmod +x "$sandbox/zfs-root/usr/bin/grub-mkconfig"
: > "$sandbox/zfs-root/usr/sbin/grub-install"; chmod +x "$sandbox/zfs-root/usr/sbin/grub-install"
: > "$sandbox/zfs-root/usr/share/initramfs-tools/hooks/zfs"
: > "$sandbox/zfs-root/usr/lib/grub/x86_64-efi/zfs.mod"
printf 'Package: base-files\nStatus: install ok installed\nVersion: 1\n\n' > "$sandbox/zfs-root/var/lib/dpkg/status"
printf 'deb http://deb.example.invalid/ stable main\n' > "$sandbox/zfs-root/etc/apt/sources.list"
: > "$sandbox/zfs-root/usr/lib/systemd/system/graphical.target"
: > "$sandbox/zfs-root/boot/grub/grub.cfg"

run_zfs_capabilities() {
    ZFS_FIXTURE_ROOT="$sandbox/zfs-root" run_zfs_harness '
        RUNNING_HOST_MODE=1
        TARGET_ROOT="$ZFS_FIXTURE_ROOT"
        TARGET_DISK=/dev/vda
        ROOT_DEVICE=/dev/vda4
        ROOT_CANONICAL=/dev/vda4
        TARGET_OS_ID=ubuntu
        TARGET_OS_LIKE=debian
        SESSION_LOG=/dev/null
        diagnostic_repair_capabilities
    ' 2>&1
}

zc_out="$(run_zfs_capabilities)"
for line in \
    'Repair tool validate: available' \
    'Repair tool dpkg: available' \
    'Repair tool fixbroken: available' \
    'Repair tool aptupdate: available' \
    'Repair tool upgrade: available' \
    'Repair tool initramfs: available' \
    'Repair tool grub: available' \
    'Repair tool efi: available' \
    'Repair tool bootstack: available' \
    'Repair tool filesystem: available'; do
    grep -Fqx "$line" <<<"$zc_out" \
        || { echo "FAIL: ZFS-on-root capability line missing: $line" >&2; printf '%s\n' "$zc_out" >&2; exit 1; }
done
# extlinux is not the ZFS-on-root bootloader: it fails closed with its normal
# probe-based reason (no update-extlinux tooling), never a ZFS blanket refusal.
grep -Fqx 'Repair tool extlinux: unavailable|reason:missing-update-extlinux' <<<"$zc_out" \
    || { echo 'FAIL: ZFS-on-root extlinux reason is not the probe-based tooling reason' >&2; printf '%s\n' "$zc_out" >&2; exit 1; }
# The reasons are probe-based and must never name a distribution or the old
# blanket "not ZFS-aware" wording.
if grep -E '^Repair tool [a-z]+: unavailable\|' <<<"$zc_out" | grep -Eqi 'ubuntu|debian|alpine|arch|fedora|tuxedo|not ZFS-aware'; then
    echo 'FAIL: a ZFS-on-root unavailable reason names a distribution or the retired blanket refusal' >&2
    printf '%s\n' "$zc_out" >&2
    exit 1
fi
[[ "$(grep -c '^Repair tool ' <<<"$zc_out")" -eq 15 ]] \
    || { echo 'FAIL: the ZFS-on-root capability preamble lost a key line' >&2; printf '%s\n' "$zc_out" >&2; exit 1; }

# 8e. Missing ZFS prerequisites fail closed with the exact prerequisite named.
#     Removing the zfs-initramfs hook disables the initramfs stage.
rm -f "$sandbox/zfs-root/usr/share/initramfs-tools/hooks/zfs"
zc_no_hook="$(run_zfs_capabilities)"
grep -Fqx 'Repair tool initramfs: unavailable|reason:missing-zfs-initramfs-hook' <<<"$zc_no_hook" \
    || { echo 'FAIL: missing zfs-initramfs hook did not fail closed with the exact reason' >&2; grep '^Repair tool initramfs' <<<"$zc_no_hook" >&2; exit 1; }
: > "$sandbox/zfs-root/usr/share/initramfs-tools/hooks/zfs"

#     Removing the GRUB ZFS module disables the grub stage.
rm -f "$sandbox/zfs-root/usr/lib/grub/x86_64-efi/zfs.mod"
zc_no_module="$(run_zfs_capabilities)"
grep -Fqx 'Repair tool grub: unavailable|reason:missing-grub-zfs-module' <<<"$zc_no_module" \
    || { echo 'FAIL: missing GRUB ZFS module did not fail closed with the exact reason' >&2; grep '^Repair tool grub' <<<"$zc_no_module" >&2; exit 1; }
: > "$sandbox/zfs-root/usr/lib/grub/x86_64-efi/zfs.mod"

#     An unimported/unresolvable root pool disables the grub stage.
zc_no_pool="$(ZFS_FIXTURE_ROOT="$sandbox/zfs-root" run_zfs_harness '
    zpool() { return 1; }
    RUNNING_HOST_MODE=1
    TARGET_ROOT="$ZFS_FIXTURE_ROOT"
    TARGET_DISK=/dev/vda
    ROOT_DEVICE=/dev/vda4
    ROOT_CANONICAL=/dev/vda4
    TARGET_OS_ID=ubuntu
    TARGET_OS_LIKE=debian
    SESSION_LOG=/dev/null
    diagnostic_repair_capabilities
' 2>&1)"
grep -Fqx 'Repair tool grub: unavailable|reason:unresolvable-zfs-root' <<<"$zc_no_pool" \
    || { echo 'FAIL: an unresolvable ZFS root pool did not fail closed with the exact reason' >&2; grep '^Repair tool grub' <<<"$zc_no_pool" >&2; exit 1; }

# ---------------------------------------------------------------------------
# 9. Nested-child safe set: the empty systemd-managed @/var/lib/portables and
#    @/var/lib/machines placeholders are tolerated and no longer produce
#    nested-subvolumes; an unknown nested child — including one nested *beneath*
#    the safe placeholders, i.e. an actual machine/portable image — still
#    refuses.  This drives the REAL host_snapshot_nested_child_paths walk
#    through a mocked `btrfs subvolume list -o /` (full record lines, so the
#    helper's own `path ` sed extraction runs).
# ---------------------------------------------------------------------------
cat > "$sandbox/safe-set-harness.sh" <<'SAFESET'
#!/usr/bin/env bash
set -euo pipefail
source <(sed '/^main "$@"/d' "${HELPER:?}")
trap - EXIT INT TERM HUP

btrfs() {
    case "${1:-}" in
        subvolume)
            case "${2:-}" in
                list) [[ -n "${FAKE_SUBVOLUME_LIST:-}" ]] && printf '%s\n' "$FAKE_SUBVOLUME_LIST" ;;
            esac
            ;;
    esac
    return 0
}
lsblk() { printf '%s\n' "${FAKE_HOST_FSTYPE:-btrfs}"; }
snapper() { [[ "${1:-}" == --version ]] && printf 'snapper 0.10.6\n'; return 0; }
pgrep() { return 1; }
host_snapshot_snapper_config_ok() { return 0; }
host_snapshot_running_path() { printf '@\n'; }
host_snapshot_subvolid_pinned() { return 1; }
host_snapshot_has_separate_boot() { return 1; }
host_snapshot_timeshift_managed() { return 1; }
target_path_is_mounted_rw() { return 0; }
host_snapshot_free_space_ok() { return 0; }
host_package_manager_gate() { return 0; }

eval "${HARNESS_CODE:?missing HARNESS_CODE}"
SAFESET
chmod +x "$sandbox/safe-set-harness.sh"

run_safe_set_harness() {
    HELPER="$HELPER" BOOT_REPAIR_STATE_ROOT="$sandbox/state" HARNESS_CODE="$1" \
        bash --noprofile --norc "$sandbox/safe-set-harness.sh"
}

# 9a. Exactly the documented safe nested children: available, no reason.
if ! safe_out="$(FAKE_SUBVOLUME_LIST=$'ID 258 gen 1 top level 5 path @/.snapshots\nID 259 gen 1 top level 5 path @/var/lib/portables\nID 260 gen 1 top level 5 path @/var/lib/machines' \
        run_safe_set_harness 'host_snapshot_rollback_unavailable_reason' 2>&1)"; then
    echo 'FAIL: the systemd-managed nested children were not tolerated' >&2
    printf '%s\n' "$safe_out" >&2
    exit 1
fi
[[ -z "$safe_out" ]] \
    || { echo 'FAIL: the systemd-managed nested children still produced a refusal reason' >&2; printf '%s\n' "$safe_out" >&2; exit 1; }

# 9b. An unknown nested child (@/data) still refuses with nested-subvolumes.
unknown_out="$(FAKE_SUBVOLUME_LIST=$'ID 258 gen 1 top level 5 path @/.snapshots\nID 259 gen 1 top level 5 path @/data' \
    run_safe_set_harness 'host_snapshot_rollback_unavailable_reason' 2>&1 || true)"
grep -Fq 'reason:nested-subvolumes' <<<"$unknown_out" \
    || { echo 'FAIL: an unknown nested child did not produce nested-subvolumes' >&2; printf '%s\n' "$unknown_out" >&2; exit 1; }
grep -Fq '@/data' <<<"$unknown_out" \
    || { echo 'FAIL: the unknown nested child path is missing from the refusal' >&2; printf '%s\n' "$unknown_out" >&2; exit 1; }

# 9c. A nested child *beneath* a safe placeholder (a real machine image) is not
#     tolerated: only the exact empty placeholder path is exempt.
machine_image_out="$(FAKE_SUBVOLUME_LIST=$'ID 258 gen 1 top level 5 path @/.snapshots\nID 259 gen 1 top level 5 path @/var/lib/machines\nID 260 gen 1 top level 5 path @/var/lib/machines/debian-test' \
    run_safe_set_harness 'host_snapshot_rollback_unavailable_reason' 2>&1 || true)"
grep -Fq 'reason:nested-subvolumes' <<<"$machine_image_out" \
    || { echo 'FAIL: a nested child beneath a safe placeholder was tolerated' >&2; printf '%s\n' "$machine_image_out" >&2; exit 1; }
grep -Fq '@/var/lib/machines/debian-test' <<<"$machine_image_out" \
    || { echo 'FAIL: the machine-image path is missing from the refusal' >&2; printf '%s\n' "$machine_image_out" >&2; exit 1; }

# ---------------------------------------------------------------------------
# 10. Damaged-target snapshot rollback: a Btrfs repair target whose root @ is
#     missing /etc/os-release (the damaged-target symptom a prior mount-release
#     bug can leave) must still list/plan/rollback through its intact
#     @.snapshots store.  run_snapshots uses the lightweight
#     prepare_target_snapshots_only (no OS detection), so the "does not contain
#     /etc/os-release" hard failure never fires, while the running-host
#     protection and same-disk assertion still apply.
# ---------------------------------------------------------------------------
grep -q '^prepare_target_snapshots_only()' "$HELPER" \
    || { echo 'FAIL: the lightweight snapshot-only prepare function is missing' >&2; exit 1; }
snapshot_body="$(sed -n '/^run_snapshots()/,/^}/p' "$HELPER")"
grep -q 'prepare_target_snapshots_only' <<<"$snapshot_body" \
    || { echo 'FAIL: run_snapshots does not use the lightweight snapshot-only prepare' >&2; exit 1; }
if grep -q 'prepare_target ro' <<<"$snapshot_body"; then
    echo 'FAIL: run_snapshots still runs the full OS-detection prepare_target' >&2
    exit 1
fi
# The damaged-target recovery probe must call the snapshot-only prepare in its
# quiet mode so the safety preamble (protected-host-check-pass / target-disk /
# root-component) is not emitted twice on the healthy Btrfs path (the probe,
# then prepare_target ro).
recovery_probe_body="$(sed -n '/^target_root_needs_snapshot_recovery()/,/^}/p' "$HELPER")"
grep -Fq 'prepare_target_snapshots_only quiet' <<<"$recovery_probe_body" \
    || { echo 'FAIL: target_root_needs_snapshot_recovery does not call prepare_target_snapshots_only quiet' >&2; exit 1; }
# The probe's non-recovery teardown must follow the cleanup() mount-release
# invariant: plain non-lazy umount of every recorded mount first, then the
# session directory is removed ONLY behind a still-mounted check — never a
# bare rm -rf that could recurse through a still-attached mount.
grep -Fq 'still_mounted' <<<"$recovery_probe_body" \
    || { echo 'FAIL: target_root_needs_snapshot_recovery lost the still-mounted guard' >&2; exit 1; }
grep -Fq 'umount "$mnt"' <<<"$recovery_probe_body" \
    || { echo 'FAIL: target_root_needs_snapshot_recovery does not umount the recorded mounts first' >&2; exit 1; }
grep -Fq 'mount_records_prune' <<<"$recovery_probe_body" \
    || { echo 'FAIL: target_root_needs_snapshot_recovery does not prune released mount records' >&2; exit 1; }
grep -Fq 'rm -rf -- "$SESSION_DIR"' <<<"$recovery_probe_body" \
    || { echo 'FAIL: target_root_needs_snapshot_recovery no longer removes the released session directory' >&2; exit 1; }
# The rm -rf must sit behind the still-mounted check, never before it.
if ! awk '/still_mounted/{guard=NR} /rm -rf -- "\$SESSION_DIR"/{if (!guard) exit 1; exit 0}' \
        <<<"$recovery_probe_body" >/dev/null; then
    echo 'FAIL: target_root_needs_snapshot_recovery deletes the session directory without the still-mounted guard' >&2
    exit 1
fi

cat > "$sandbox/target-snapshot-harness.sh" <<'TARGETSNAP'
#!/usr/bin/env bash
set -Eeuo pipefail
source <(sed '/^main "\$@"/d' "${HELPER:?}")
trap - EXIT INT TERM HUP
PATH="$SANDBOX/mockbin:$PATH"; export PATH

# --- prepare_target_snapshots_only low-level plumbing -----------------------
is_block_device() { [[ "$1" == /dev/test-* ]]; }
canonical_block() { is_block_device "$1" && printf '%s\n' "$1"; }
top_disks_for() { printf '%s\n' /dev/test-disk; }
mapper_aliases_for_device() { return 0; }
need() { :; }
mountpoint() { return 1; }
mount() { printf 'mount %s\n' "$*" >> "$FAKE_COMMAND_LOG"; return 0; }
umount() { :; }
findmnt() { return 1; }
sweep_stale_target_mounts() { :; }
mktemp() { printf '%s\n' "$SANDBOX/session-dir"; }
sync() { :; }

lsblk() {
    case "$*" in
        *'-ndo FSTYPE'*) printf '%s\n' "${FAKE_FSTYPE:-btrfs}" ;;
        *) return 0 ;;
    esac
}

# The fixture lives under $SANDBOX/top; mount_snapshot_top is exercised
# behaviourally elsewhere, so this stub binds SNAPSHOT_TOP to the fixture.
mount_snapshot_top() {
    SNAPSHOT_TOP="$SANDBOX/top"
    mkdir -p "$SNAPSHOT_TOP"
    return 0
}

# --- snapshot rollback helpers (validated elsewhere) ------------------------
validate_snapshot_fstab_for_rollback() { return "${FAKE_FSTAB_RC:-0}"; }
validate_snapshot_crypttab_for_rollback() { return 0; }
snapshot_free_space_ok() { return "${FAKE_SPACE_RC:-0}"; }
snapshot_kernel_pair_audit() { return 0; }
snapshot_separate_subvolumes() { :; }
snapshot_mount_promoted_root_rw() { printf 'mount-promoted\n' >> "$FAKE_COMMAND_LOG"; return 0; }
snapshot_post_switch_reconcile() { return "${FAKE_RECONCILE_RC:-0}"; }
snapshot_restore_preserved_root() { return 1; }
prepare_mapper_compatibility_aliases() { :; }
read_target_os() { :; }

STATE_ROOT="$SANDBOX/state"
export FAKE_COMMAND_LOG="$SANDBOX/commands.log"
eval "${HARNESS_CODE:?missing HARNESS_CODE}"
TARGETSNAP
chmod +x "$sandbox/target-snapshot-harness.sh"

run_target_snapshot_harness() {
    SANDBOX="$sandbox" HELPER="$HELPER" HARNESS_CODE="$1" \
        bash --noprofile --norc "$sandbox/target-snapshot-harness.sh"
}

# Build a damaged target: the top-level @ root has no /etc/os-release, but the
# @.snapshots store holds an intact Linux root snapshot (os-release + fstab).
build_damaged_target_fixture() {
    local top="$sandbox/top"
    rm -rf -- "$top"
    mkdir -p "$top/@/etc" "$top/@.snapshots/1/snapshot/etc"
    # The damaged @ root: an /etc exists but os-release was deleted.
    : > "$top/@/etc/fstab"
    printf 'ID=test\nPRETTY_NAME="Contract Test"\n' > "$top/@.snapshots/1/snapshot/etc/os-release"
    printf '/dev/test-root / btrfs subvol=@ 0 1\n' > "$top/@.snapshots/1/snapshot/etc/fstab"
    cat > "$top/@.snapshots/1/info.xml" <<'XML'
<?xml version="1.0"?>
<snapshot>
  <type>single</type>
  <description>contract damaged-target snapshot</description>
</snapshot>
XML
    mkdir -p "$sandbox/session-dir/mount"
}

target_code='TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root'

# 10a. list succeeds and reports the intact snapshot without the hard failure.
build_damaged_target_fixture
damaged_list="$(run_target_snapshot_harness "$target_code
run_snapshots list")"
grep -Fq $'SNAPSHOT\t1\t' <<<"$damaged_list" \
    || { echo 'FAIL: damaged-target snapshot list did not report snapshot 1' >&2; printf '%s\n' "$damaged_list" >&2; exit 1; }
if grep -Fq 'does not contain /etc/os-release' <<<"$damaged_list"; then
    echo 'FAIL: damaged-target snapshot list hit the /etc/os-release hard failure' >&2
    printf '%s\n' "$damaged_list" >&2
    exit 1
fi

# 10b. plan succeeds and emits PLAN_OK=1.
build_damaged_target_fixture
damaged_plan="$(run_target_snapshot_harness "$target_code
run_snapshots plan 1")"
grep -Fq 'PLAN_OK=1' <<<"$damaged_plan" \
    || { echo 'FAIL: damaged-target snapshot plan did not emit PLAN_OK=1' >&2; printf '%s\n' "$damaged_plan" >&2; exit 1; }
if grep -Fq 'does not contain /etc/os-release' <<<"$damaged_plan"; then
    echo 'FAIL: damaged-target snapshot plan hit the /etc/os-release hard failure' >&2
    exit 1
fi

# 10c. rollback succeeds and promotes the intact snapshot to @.
build_damaged_target_fixture
: > "$sandbox/commands.log"
damaged_rollback="$(run_target_snapshot_harness "$target_code
run_snapshots rollback 1")"
grep -Fq 'ROLLBACK_RESULT=SUCCESS' <<<"$damaged_rollback" \
    || { echo 'FAIL: damaged-target snapshot rollback did not succeed' >&2; printf '%s\n' "$damaged_rollback" >&2; exit 1; }
if grep -Fq 'does not contain /etc/os-release' <<<"$damaged_rollback"; then
    echo 'FAIL: damaged-target snapshot rollback hit the /etc/os-release hard failure' >&2
    exit 1
fi
[[ -f "$sandbox/top/@/etc/os-release" ]] \
    || { echo 'FAIL: the promoted @ was not populated from the intact snapshot' >&2; exit 1; }
backup_count="$(find "$sandbox/top" -mindepth 1 -maxdepth 1 -type d -name '@rollback-before-*' | wc -l)"
[[ "$backup_count" == 1 ]] \
    || { echo 'FAIL: the damaged @ was not preserved as one @rollback-before-* root' >&2; exit 1; }

# 10d. prepare_target_snapshots_only honours an optional quiet mode: quiet
#     suppresses ONLY the three safety-preamble msg_log lines, while the default
#     (run_snapshots) call keeps them.  The recovery probe runs it quiet so a
#     healthy Btrfs target's diagnostic emits the preamble exactly once (from
#     prepare_target ro) instead of twice.
quiet_probe="$(run_target_snapshot_harness "$target_code
prepare_target_snapshots_only quiet
printf 'QUIET_DONE\n'")"
grep -Fq 'QUIET_DONE' <<<"$quiet_probe" \
    || { echo 'FAIL: quiet prepare_target_snapshots_only did not complete' >&2; printf '%s\n' "$quiet_probe" >&2; exit 1; }
if grep -Fq 'msg:protected-host-check-pass' <<<"$quiet_probe"; then
    echo 'FAIL: prepare_target_snapshots_only quiet still emitted the safety preamble' >&2
    printf '%s\n' "$quiet_probe" >&2
    exit 1
fi
default_probe="$(run_target_snapshot_harness "$target_code
prepare_target_snapshots_only
printf 'DEFAULT_DONE\n'")"
grep -Fq 'DEFAULT_DONE' <<<"$default_probe" \
    || { echo 'FAIL: default prepare_target_snapshots_only did not complete' >&2; printf '%s\n' "$default_probe" >&2; exit 1; }
grep -Fq 'msg:protected-host-check-pass' <<<"$default_probe" \
    || { echo 'FAIL: prepare_target_snapshots_only default lost the safety preamble' >&2; printf '%s\n' "$default_probe" >&2; exit 1; }

# ---------------------------------------------------------------------------
# 11. Damaged-target reduced recovery report.  run_target_diagnostic must
#     detect the damaged-but-recoverable Btrfs target (no /etc/os-release
#     anywhere in the top-level tree, but an intact @.snapshots store) and emit
#     a reduced report the GUI caches as `Snapshot rollback: available` instead
#     of the /etc/os-release hard refusal.  A HEALTHY Btrfs target must NOT take
#     the recovery path: it still runs the normal prepare_target ro flow.
# ---------------------------------------------------------------------------

# Healthy fixture: the top-level @ root still carries /etc/os-release, so
# find_btrfs_root resolves the installed root and prepare_target ro handles it.
build_healthy_target_fixture() {
    local top="$sandbox/top"
    rm -rf -- "$top"
    mkdir -p "$top/@/etc" "$top/@.snapshots/1/snapshot/etc"
    printf 'ID=test\nPRETTY_NAME="Healthy Contract Test"\n' > "$top/@/etc/os-release"
    : > "$top/@/etc/fstab"
    printf '/dev/test-root / btrfs subvol=@ 0 1\n' > "$top/@.snapshots/1/snapshot/etc/fstab"
    mkdir -p "$sandbox/session-dir/mount"
}

# 11a. Damaged target: the REAL run_target_diagnostic report emits the reduced
#      recovery report (Snapshot rollback: available + the damaged message),
#      never the /etc/os-release hard refusal, and exits 0.
build_damaged_target_fixture
damaged_diag="$(run_target_snapshot_harness "$target_code
run_target_diagnostic report")"
grep -Fqx 'Snapshot rollback: available' <<<"$damaged_diag" \
    || { echo 'FAIL: damaged-target diagnostic did not emit Snapshot rollback: available' >&2; printf '%s\n' "$damaged_diag" >&2; exit 1; }
grep -Fq 'Snapshot rollback evidence: ' <<<"$damaged_diag" \
    || { echo 'FAIL: damaged-target recovery report is missing Snapshot rollback evidence' >&2; printf '%s\n' "$damaged_diag" >&2; exit 1; }
grep -Fqx 'Repair capability probes (read-only, selected target):' <<<"$damaged_diag" \
    || { echo 'FAIL: damaged-target recovery report is missing the capability-preamble header' >&2; printf '%s\n' "$damaged_diag" >&2; exit 1; }
grep -Fqx 'Diagnostic: report' <<<"$damaged_diag" \
    || { echo 'FAIL: damaged-target recovery report is missing the Diagnostic: report frame' >&2; printf '%s\n' "$damaged_diag" >&2; exit 1; }
grep -Fq 'msg:root-damaged-snapshot-rollback' <<<"$damaged_diag" \
    || { echo 'FAIL: damaged-target diagnostic did not emit the damaged-target message' >&2; printf '%s\n' "$damaged_diag" >&2; exit 1; }
if grep -Fq 'does not contain /etc/os-release' <<<"$damaged_diag"; then
    echo 'FAIL: damaged-target diagnostic hit the /etc/os-release hard refusal' >&2
    printf '%s\n' "$damaged_diag" >&2
    exit 1
fi

# 11b. Healthy target: run_target_diagnostic must NOT take the recovery path.
#      The normal prepare_target ro flow is reached (proven by the stub markers)
#      and the reduced recovery report is never emitted.
build_healthy_target_fixture
healthy_diag="$(run_target_snapshot_harness "$target_code
prepare_target() { printf 'PREPARE_TARGET_RO_REACHED\n'; return 0; }
mount_target_boot_entry() { return 0; }
diagnostic_repair_capabilities() {
    echo 'Repair capability probes (read-only, selected target):'
    echo 'Repair tool validate: available'
}
run_one_target_diagnostic() { echo 'Diagnostic: stub-section'; }
run_target_diagnostic report")"
grep -Fqx 'PREPARE_TARGET_RO_REACHED' <<<"$healthy_diag" \
    || { echo 'FAIL: healthy-target diagnostic did not reach prepare_target ro' >&2; printf '%s\n' "$healthy_diag" >&2; exit 1; }
grep -Fqx 'Repair tool validate: available' <<<"$healthy_diag" \
    || { echo 'FAIL: healthy-target diagnostic did not emit the normal capability preamble' >&2; printf '%s\n' "$healthy_diag" >&2; exit 1; }
grep -Fqx 'Diagnostic: stub-section' <<<"$healthy_diag" \
    || { echo 'FAIL: healthy-target diagnostic did not run the diagnostic sections' >&2; printf '%s\n' "$healthy_diag" >&2; exit 1; }
if grep -Fq 'msg:root-damaged-snapshot-rollback' <<<"$healthy_diag"; then
    echo 'FAIL: healthy-target diagnostic took the damaged recovery path' >&2
    printf '%s\n' "$healthy_diag" >&2
    exit 1
fi
if grep -Fq 'Damaged target snapshot recovery' <<<"$healthy_diag"; then
    echo 'FAIL: healthy-target diagnostic emitted the reduced recovery report' >&2
    printf '%s\n' "$healthy_diag" >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# 12. Regression: target snapshot rollback re-profiles the promoted root's
#     backends.  snapshot_mount_promoted_root_rw must re-detect the initramfs
#     backend against the rolled-back @ (the running-host path already does)
#     BEFORE snapshot_post_switch_reconcile dispatches adaptive_initramfs_repair.
#     A stale/empty TARGET_INITRAMFS_BACKEND otherwise reaches the guarded
#     initramfs repair and fails closed with "unknown has no guarded repair
#     implementation", which in turn drives the recovery path into a damaged
#     preserved @.  This drives the REAL snapshot_mount_promoted_root_rw (mount
#     plumbing is stubbed; the backend probes, read_target_os and the fstab
#     subvolume walk run for real) against a valid Debian-family @ fixture and
#     asserts the backend globals are re-detected, never left at their stale
#     pre-rollback values.
# ---------------------------------------------------------------------------
# Static wiring: the call must exist inside the real mount function.
promoted_body="$(sed -n '/^snapshot_mount_promoted_root_rw()/,/^}/p' "$HELPER")"
grep -Eq '^[[:space:]]*profile_target_backends([[:space:]]|$)' <<<"$promoted_body" \
    || { echo 'FAIL: snapshot_mount_promoted_root_rw does not call profile_target_backends' >&2; exit 1; }

# Build a valid Debian-family @ snapshot root: os-release identifies Debian,
# the dpkg database records initramfs-tools, and the update-initramfs/
# mkinitramfs executables plus /etc/apt and /boot/grub evidence are present so
# every backend probe can resolve the native profile.
build_debian_snapshot_fixture() {
    local root="$sandbox/debian-snapshot-root"
    rm -rf -- "$root"
    mkdir -p "$root/etc" "$root/etc/apt" "$root/usr/sbin" "$root/usr/bin" \
        "$root/var/lib/dpkg" "$root/boot/grub" "$root/usr/share/initramfs-tools"
    printf 'ID=debian\nID_LIKE=debian\nPRETTY_NAME="Debian Contract Test"\n' > "$root/etc/os-release"
    printf '/dev/test-root / btrfs subvol=@ 0 1\n' > "$root/etc/fstab"
    printf 'deb http://deb.example.invalid/ stable main\n' > "$root/etc/apt/sources.list"
    for tool in dpkg apt-get; do : > "$root/usr/bin/$tool"; chmod +x "$root/usr/bin/$tool"; done
    for tool in update-initramfs mkinitramfs grub-mkconfig; do : > "$root/usr/sbin/$tool"; chmod +x "$root/usr/sbin/$tool"; done
    : > "$root/boot/grub/grub.cfg"
    cat > "$root/var/lib/dpkg/status" <<'STATUS'
Package: base-files
Status: install ok installed
Version: 1

Package: initramfs-tools
Status: install ok installed
Version: 1

STATUS
}

cat > "$sandbox/promoted-root-harness.sh" <<'PROMOTED'
#!/usr/bin/env bash
set -Eeuo pipefail
source <(sed '/^main "$@"/d' "${HELPER:?}")
trap - EXIT INT TERM HUP

# Mount/topology plumbing is stubbed (no real device, mount, subvolume or
# chroot is touched).  read_target_os, profile_target_backends and
# mount_target_btrfs_subvolumes run for real against the @ fixture.
mount() { printf 'mount %s\n' "$*" >> "${FAKE_COMMAND_LOG:?}"; return 0; }
umount() { :; }
mountpoint() { return 1; }
findmnt() { return 1; }
lsblk() { [[ "$*" == *'-ndo FSTYPE'* ]] && printf 'btrfs\n'; return 0; }
mapper_aliases_for_device() { return 0; }
mount_special() { :; }
mount_target_boot_entry() { :; }

eval "${HARNESS_CODE:?missing HARNESS_CODE}"
PROMOTED
chmod +x "$sandbox/promoted-root-harness.sh"

run_promoted_root_harness() {
    HELPER="$HELPER" SANDBOX="$sandbox" FAKE_COMMAND_LOG="$sandbox/promoted-commands.log" \
        HARNESS_CODE="$1" bash --noprofile --norc "$sandbox/promoted-root-harness.sh"
}

build_debian_snapshot_fixture
: > "$sandbox/promoted-commands.log"
promoted_out="$(run_promoted_root_harness '
    ROOT_DEVICE=/dev/test-root
    ROOT_CANONICAL=/dev/test-root
    TARGET_DISK=/dev/test-disk
    MOUNT_BASE="$SANDBOX/debian-snapshot-root"
    SESSION_DIR="$SANDBOX/session"
    SESSION_LOG="$SANDBOX/session/session.log"
    mkdir -p "$SESSION_DIR"
    : > "$SESSION_LOG"
    # Stale pre-rollback state: prepare_target_snapshots_only never profiles
    # backends, so these globals are empty/unknown when the rollback promotes
    # @.  The re-profile must overwrite them.  TARGET_DISTRO_FAMILY is left
    # unset (the real pre-rollback state) so profile_target_backends recomputes
    # it from the freshly read os-release.
    TARGET_INITRAMFS_BACKEND=unknown
    TARGET_BOOTLOADER_BACKEND=unknown
    TARGET_PACKAGE_MANAGER=unknown
    snapshot_mount_promoted_root_rw
    printf "TARGET_INITRAMFS_BACKEND=%s\n" "${TARGET_INITRAMFS_BACKEND:-EMPTY}"
    printf "TARGET_BOOTLOADER_BACKEND=%s\n" "${TARGET_BOOTLOADER_BACKEND:-EMPTY}"
    printf "TARGET_PACKAGE_MANAGER=%s\n" "${TARGET_PACKAGE_MANAGER:-EMPTY}"
    printf "TARGET_DISTRO_FAMILY=%s\n" "${TARGET_DISTRO_FAMILY:-EMPTY}"
    printf "PROMOTED_ROOT_PROBE_DONE\n"
' 2>&1)"

grep -Fqx 'TARGET_INITRAMFS_BACKEND=initramfs-tools' <<<"$promoted_out" \
    || { echo 'FAIL: promoted-root probe did not re-detect initramfs-tools' >&2; printf '%s\n' "$promoted_out" >&2; exit 1; }
grep -Fqx 'TARGET_BOOTLOADER_BACKEND=grub' <<<"$promoted_out" \
    || { echo 'FAIL: promoted-root probe did not re-detect grub' >&2; printf '%s\n' "$promoted_out" >&2; exit 1; }
grep -Fqx 'TARGET_PACKAGE_MANAGER=apt/dpkg' <<<"$promoted_out" \
    || { echo 'FAIL: promoted-root probe did not re-detect apt/dpkg' >&2; printf '%s\n' "$promoted_out" >&2; exit 1; }
grep -Fqx 'TARGET_DISTRO_FAMILY=debian' <<<"$promoted_out" \
    || { echo 'FAIL: promoted-root probe did not re-detect the Debian family' >&2; printf '%s\n' "$promoted_out" >&2; exit 1; }
grep -Fqx 'PROMOTED_ROOT_PROBE_DONE' <<<"$promoted_out" \
    || { echo 'FAIL: promoted-root probe did not complete' >&2; printf '%s\n' "$promoted_out" >&2; exit 1; }

# ---------------------------------------------------------------------------
# 13. Regression: find_btrfs_root must never adopt a rollback-staging
#     subvolume (@rollback-new-*, @rollback-before-*, @rollback-failed-*) as
#     the installed root.  When the real @ is damaged (no /etc/os-release) but
#     a leftover top-level staging subvolume still carries one, the resolver
#     must return nothing so the damaged-target recovery report fires instead
#     of silently adopting the staging copy.  A healthy, non-rollback @ root
#     keeps the existing adoption behaviour.
# ---------------------------------------------------------------------------
build_staging_fixture() {
    local top="$sandbox/top"
    rm -rf -- "$top"
    mkdir -p "$top/@/etc" \
        "$top/@rollback-failed-20261008-120000/etc" \
        "$top/@rollback-new-20261008-120001/etc" \
        "$top/@rollback-before-20261008-120002/etc"
    : > "$top/@/etc/fstab"
    printf 'ID=test\nPRETTY_NAME="Staging"\n' > "$top/@rollback-failed-20261008-120000/etc/os-release"
    printf 'ID=test\nPRETTY_NAME="Staging"\n' > "$top/@rollback-new-20261008-120001/etc/os-release"
    printf 'ID=test\nPRETTY_NAME="Staging"\n' > "$top/@rollback-before-20261008-120002/etc/os-release"
}

# 13a. A damaged @ plus leftover staging subvolumes resolves to nothing.
build_staging_fixture
staging_root="$(run_target_snapshot_harness "$target_code
find_btrfs_root '$sandbox/top' || true
printf 'FBR_STAGING_DONE\n'")"
[[ "$staging_root" == "FBR_STAGING_DONE" ]] \
    || { echo 'FAIL: find_btrfs_root adopted a @rollback-* staging subvolume as the root' >&2; printf '%s\n' "$staging_root" >&2; exit 1; }

# 13b. A healthy non-rollback @ root is still adopted (existing behaviour).
build_healthy_target_fixture
healthy_root="$(run_target_snapshot_harness "$target_code
find_btrfs_root '$sandbox/top' || true
printf 'FBR_HEALTHY_DONE\n'")"
grep -Fq "$sandbox/top/@" <<<"$healthy_root" \
    || { echo 'FAIL: find_btrfs_root no longer adopts a healthy top-level @ root' >&2; printf '%s\n' "$healthy_root" >&2; exit 1; }

# ---------------------------------------------------------------------------
# 14. Regression: the TUXEDO UKI vendor builder's LUKS detection is host-tainted
#     inside the target chroot (the host-bound /proc names the recovery host's
#     mapper and the filtered private /dev lacks it), so a rollback whose
#     promoted root is LUKS-backed could rebuild TUX.EFI without rd.luks.uuid.
#     The helper must resolve the target LUKS UUID and hand it to a
#     request-scoped findmnt guard: run_tuxedo_uki_builder exports HOST_LUKS_UUID
#     and the generated guard redirects only the LUKS-detection probe
#     `findmnt -n -o SOURCE /` to the target's unlocked mapper, leaving every
#     other findmnt invocation untouched.
# ---------------------------------------------------------------------------
mkdir -p "$sandbox/uki-root/usr/bin" "$sandbox/uki-root/usr/local/libexec"
printf '#!/bin/sh\necho REAL-FINDMNT\n' > "$sandbox/uki-root/usr/bin/findmnt"
chmod +x "$sandbox/uki-root/usr/bin/findmnt"

cat > "$sandbox/uki-builder-harness.sh" <<'UKIHARNESS'
#!/usr/bin/env bash
set -euo pipefail
source <(sed '/^main "$@"/d' "${HELPER:?}")
trap - EXIT INT TERM HUP

# Mount/topology and transcript mocks: run_tuxedo_uki_builder is exercised
# down to the (mocked) chroot boundary.  No real mount, chroot or write runs.
reassert_target_write_safety() { :; }
esp_writable_preflight() { :; }
display_locale_env() { :; }
msg_log() { :; }
log() { :; }
fail() { echo "FAIL-CALLED: $*" >&2; exit 99; }
realpath_existing() { printf '%s\n' "${@: -1}"; }
realpath() { printf '%s\n' "${@: -1}"; }
path_within() { return 0; }
repair_log_filter() { cat; }
run_selected_chroot() {
    # Capture the builder env and copy the generated guard for later checks.
    { printf 'ARGS\n'; printf '<%s>\n' "$@"; } > "$CAPTURE_ARGS"
    local w
    for w in "$TARGET_ROOT"/usr/local/libexec/boot-repair-efi-guard.*/findmnt; do
        [[ -f "$w" ]] && cp -- "$w" "$CAPTURE_WRAPPER"
    done
    return 0
}

RUNNING_HOST_MODE=0
TARGET_ROOT="$SANDBOX/uki-root"
SESSION_DIR="$SANDBOX/session.uki"
mkdir -p "$SESSION_DIR"
SESSION_LOG="$SANDBOX/uki-session.log"
: > "$SESSION_LOG"
TUXEDO_UKI_ROOT_UUID="11111111-2222-3333-4444-555555555555"
TUXEDO_UKI_LUKS_UUID="66666666-7777-8888-9999-aaaaaaaaaaaa"

eval "${HARNESS_CODE:?missing HARNESS_CODE}"
UKIHARNESS
chmod +x "$sandbox/uki-builder-harness.sh"

run_uki_harness() {
    HELPER="$HELPER" SANDBOX="$sandbox" \
        CAPTURE_ARGS="$sandbox/uki-capture-args" CAPTURE_WRAPPER="$sandbox/uki-capture-wrapper" \
        HARNESS_CODE="$1" bash --noprofile --norc "$sandbox/uki-builder-harness.sh"
}

rm -f "$sandbox/uki-capture-args" "$sandbox/uki-capture-wrapper"
uki_out="$(run_uki_harness 'run_tuxedo_uki_builder "Rebuild TUXEDO UKI for 7.1.13.1-tuxedo-amd64" /usr/sbin/create_boot_uki_base.sh 7.1.13.1-tuxedo-amd64')"
[[ -z "$uki_out" ]] \
    || { echo 'FAIL: UKI builder harness produced unexpected output' >&2; printf '%s\n' "$uki_out" >&2; exit 1; }

# 14a. The chroot environment must carry both the root and LUKS UUIDs.
grep -Fq '<HOST_ROOT_UUID=11111111-2222-3333-4444-555555555555>' "$sandbox/uki-capture-args" \
    || { echo 'FAIL: UKI builder did not export HOST_ROOT_UUID' >&2; cat "$sandbox/uki-capture-args" >&2; exit 1; }
grep -Fq '<HOST_LUKS_UUID=66666666-7777-8888-9999-aaaaaaaaaaaa>' "$sandbox/uki-capture-args" \
    || { echo 'FAIL: UKI builder did not export HOST_LUKS_UUID' >&2; cat "$sandbox/uki-capture-args" >&2; exit 1; }

# 14b. The request-scoped findmnt guard must have been generated.
[[ -s "$sandbox/uki-capture-wrapper" ]] \
    || { echo 'FAIL: UKI builder did not generate the findmnt guard' >&2; exit 1; }
chmod +x "$sandbox/uki-capture-wrapper"

# 14c. The LUKS-detection probe is redirected to the target's unlocked mapper.
redirect="$(HOST_LUKS_UUID=66666666-7777-8888-9999-aaaaaaaaaaaa "$sandbox/uki-capture-wrapper" -n -o SOURCE /)"
[[ "$redirect" == "/dev/mapper/luks-66666666-7777-8888-9999-aaaaaaaaaaaa" ]] \
    || { echo 'FAIL: findmnt guard did not redirect the LUKS probe to the target mapper' >&2; printf '%s\n' "$redirect" >&2; exit 1; }

# 14d. Every other findmnt invocation passes through without the redirect.
passthrough="$(HOST_LUKS_UUID=66666666-7777-8888-9999-aaaaaaaaaaaa "$sandbox/uki-capture-wrapper" -n -o FSTYPE / 2>&1 || true)"
[[ "$passthrough" != *228aa9a3* ]] \
    || { echo 'FAIL: findmnt guard redirected a non-LUKS findmnt invocation' >&2; printf '%s\n' "$passthrough" >&2; exit 1; }

# 14e. No target LUKS identity means no redirect (the guard falls through).
nogate="$( "$sandbox/uki-capture-wrapper" -n -o SOURCE / 2>&1 || true)"
[[ "$nogate" != *228aa9a3* ]] \
    || { echo 'FAIL: findmnt guard redirected with no target LUKS UUID' >&2; printf '%s\n' "$nogate" >&2; exit 1; }

# ---------------------------------------------------------------------------
# 15. Regression: stale-UKI direct build fallback.  When the vendor builder
#     no-ops (leaves TUX.EFI embedding an older kernel), rebuild_tuxedo_uki must
#     rebuild the UKI directly with ukify under the request-scoped EFI guard
#     (reusing the stale image's already-correct cmdline) and re-verify the
#     embedded kernel instead of failing.  The vendor command is stubbed as a
#     no-op; the objcopy section reads and the ukify-equivalent chroot
#     invocation exercise the real direct-build path with only the tool
#     boundary stubbed.
# ---------------------------------------------------------------------------
mkdir -p "$sandbox/rebuild-uki-root/boot/efi/EFI/BOOT" \
         "$sandbox/rebuild-uki-root/usr/local/libexec" \
         "$sandbox/rebuild-uki-root/usr/bin"
printf 'FAKE-TUX-EFI\n' > "$sandbox/rebuild-uki-root/boot/efi/EFI/BOOT/TUX.EFI"
printf '#!/bin/sh\nexit 0\n' > "$sandbox/mockbin/objcopy"
chmod +x "$sandbox/mockbin/objcopy"

cat > "$sandbox/rebuild-uki-harness.sh" <<'REBUILDUKI'
#!/usr/bin/env bash
set -Eeuo pipefail
source <(sed '/^main "$@"/d' "${HELPER:?}")
trap - EXIT INT TERM HUP
PATH="$SANDBOX/mockbin:$PATH"; export PATH

# Tool boundary stubs: the vendor builder no-ops (leaves TUX.EFI stale); the
# ukify-equivalent run_selected_chroot captures its argv and simulates the
# freshly built image now embedding the newest kernel.
validate_tuxedo_uki_target() { :; }
esp_writable_preflight() { :; }
newest_tuxedo_kernel() { printf '%s\n' "$FAKE_NEWEST_KERNEL"; }
uefi_nvram_writable() { return 1; }
run_tuxedo_uki_builder() { :; }   # vendor no-op -> stale TUX.EFI remains
blkid() { printf '%s\n' "$FAKE_ROOT_UUID"; }
target_luks_uuid() { printf '%s\n' "$FAKE_LUKS_UUID"; }
reassert_target_write_safety() { :; }
display_locale_env() { :; }
repair_log_filter() { cat; }
realpath_existing() { printf '%s\n' "${@: -1}"; }
realpath() { printf '%s\n' "${@: -1}"; }
path_within() { return 0; }
log() { :; }
fail() { echo "FAIL-CALLED: $*" >&2; exit 99; }
msg_log() { printf 'MSGLOG:%s\n' "$*"; }
objcopy_dump_section() {
    local section="$1" name dest
    name="${section%%=*}"; dest="${section#*=}"
    case "$name" in
        .uname)   printf '%s\0' "$(cat "$SANDBOX/embedded-kernel")"  > "$dest" ;;
        .cmdline) printf '%s\0' "$(cat "$SANDBOX/embedded-cmdline")" > "$dest" ;;
    esac
    return 0
}
run_selected_chroot() {
    { printf 'ARGS\n'; printf '<%s>\n' "$@"; } > "$CAPTURE_UKIFY"
    printf '%s\n' "$FAKE_NEWEST_KERNEL" > "$SANDBOX/embedded-kernel"
    return 0
}

TARGET_ROOT="$SANDBOX/rebuild-uki-root"
ROOT_CANONICAL=/dev/test-root
RUNNING_HOST_MODE=0
EFI_ESP_SOURCE=""
SESSION_DIR="$SANDBOX/session.rebuild"
mkdir -p "$SESSION_DIR"
SESSION_LOG="$SANDBOX/rebuild-session.log"
: > "$SESSION_LOG"

eval "${HARNESS_CODE:?missing HARNESS_CODE}"
REBUILDUKI
chmod +x "$sandbox/rebuild-uki-harness.sh"

# The stale image embeds an older kernel and already carries the correct cmdline.
printf '7.2.7.1-tuxedo-amd64\n' > "$sandbox/embedded-kernel"
printf '%s\n' 'root=UUID=11111111-2222-3333-4444-555555555555 rd.luks.uuid=66666666-7777-8888-9999-aaaaaaaaaaaa rd.luks.name=66666666-7777-8888-9999-aaaaaaaaaaaa=luks-66666666-7777-8888-9999-aaaaaaaaaaaa subvol=@ rootflags=subvol=@' \
    > "$sandbox/embedded-cmdline"

rm -f "$sandbox/rebuild-uki-capture-args"
rebuild_out="$(HELPER="$HELPER" SANDBOX="$sandbox" \
    CAPTURE_UKIFY="$sandbox/rebuild-uki-capture-args" \
    FAKE_NEWEST_KERNEL=7.1.13.1-tuxedo-amd64 \
    FAKE_ROOT_UUID=11111111-2222-3333-4444-555555555555 \
    FAKE_LUKS_UUID=66666666-7777-8888-9999-aaaaaaaaaaaa \
    HARNESS_CODE='rebuild_tuxedo_uki' \
    bash --noprofile --norc "$sandbox/rebuild-uki-harness.sh")"

# 15a. The fallback must fire and the post-build check must pass (no fail).
if grep -Fq 'FAIL-CALLED' <<<"$rebuild_out"; then
    echo 'FAIL: stale-UKI rebuild_tuxedo_uki failed instead of falling back' >&2
    printf '%s\n' "$rebuild_out" >&2
    exit 1
fi
grep -Fq 'MSGLOG:uki-vendor-stale' <<<"$rebuild_out" \
    || { echo 'FAIL: stale-UKI fallback did not emit uki-vendor-stale' >&2; printf '%s\n' "$rebuild_out" >&2; exit 1; }

# 15b. The direct ukify-equivalent invocation must use the newest kernel and the
#      extracted (already-correct) cmdline.
[[ -s "$sandbox/rebuild-uki-capture-args" ]] \
    || { echo 'FAIL: direct ukify-equivalent path did not run' >&2; exit 1; }
grep -Fq '<--linux=/boot/vmlinuz-7.1.13.1-tuxedo-amd64>' "$sandbox/rebuild-uki-capture-args" \
    || { echo 'FAIL: direct ukify path used the wrong kernel' >&2; cat "$sandbox/rebuild-uki-capture-args" >&2; exit 1; }
grep -Fq -- '--cmdline=root=UUID=11111111-2222-3333-4444-555555555555 rd.luks.uuid=66666666-7777-8888-9999-aaaaaaaaaaaa rd.luks.name=66666666-7777-8888-9999-aaaaaaaaaaaa=luks-66666666-7777-8888-9999-aaaaaaaaaaaa subvol=@ rootflags=subvol=@' "$sandbox/rebuild-uki-capture-args" \
    || { echo 'FAIL: direct ukify path did not reuse the stale image correct cmdline' >&2; cat "$sandbox/rebuild-uki-capture-args" >&2; exit 1; }

# 15c. The post-build embedded-kernel check reports the newest kernel.
grep -Fq 'MSGLOG:uki-embedded-kernel 7.1.13.1-tuxedo-amd64' <<<"$rebuild_out" \
    || { echo 'FAIL: post-build embedded-kernel check did not report the newest kernel' >&2; printf '%s\n' "$rebuild_out" >&2; exit 1; }

# ---------------------------------------------------------------------------
# 16. Regression: damaged preserved-root recovery must not hard-fail.  A
#     preserved @rollback-before-* root whose /etc/os-release is missing still
#     restores the @ name and default subvolume; the recovery reports the
#     damaged root, skips boot-stack reconciliation and returns 0 so the caller
#     reports the preserved root as restored.  A healthy preserved root still
#     reconciles.
# ---------------------------------------------------------------------------
cat > "$sandbox/preserved-root-harness.sh" <<'PRESERVED'
#!/usr/bin/env bash
set -Eeuo pipefail
source <(sed '/^main "$@"/d' "${HELPER:?}")
trap - EXIT INT TERM HUP

mount() { printf 'mount %s\n' "$*" >> "$FAKE_COMMAND_LOG"; return 0; }
umount() { :; }
mountpoint() { return 1; }
snapshot_unmount_target_keep_top() { printf 'unmount-keep-top\n' >> "$FAKE_COMMAND_LOG"; return 0; }
btrfs() { printf 'btrfs %s\n' "$*" >> "$FAKE_COMMAND_LOG"; return 0; }
btrfs_subvol_id() { printf '200\n'; }
snapshot_mount_promoted_root_rw() { printf 'mount-promoted\n' >> "$FAKE_COMMAND_LOG"; return 0; }
snapshot_post_switch_reconcile() { printf 'reconcile\n' >> "$FAKE_COMMAND_LOG"; return 0; }
msg_log() { printf 'MSGLOG:%s\n' "$*"; }
log() { :; }

SESSION_LOG="$SANDBOX/session-preserved.log"
: > "$SESSION_LOG"

eval "${HARNESS_CODE:?missing HARNESS_CODE}"
PRESERVED
chmod +x "$sandbox/preserved-root-harness.sh"

run_preserved_root_harness() {
    HELPER="$HELPER" SANDBOX="$sandbox" \
        FAKE_COMMAND_LOG="$sandbox/preserved-commands.log" \
        HARNESS_CODE="$1" bash --noprofile --norc "$sandbox/preserved-root-harness.sh"
}

build_preserved_root_damaged_fixture() {
    local top="$sandbox/preserved-top"
    rm -rf -- "$top"
    mkdir -p "$top/@rollback-before-test/etc" "$top/@/etc"
    : > "$top/@rollback-before-test/etc/fstab"
    printf 'ID=test\nPRETTY_NAME="Failed Candidate"\n' > "$top/@/etc/os-release"
}
build_preserved_root_healthy_fixture() {
    local top="$sandbox/preserved-top"
    rm -rf -- "$top"
    mkdir -p "$top/@rollback-before-test/etc" "$top/@/etc"
    printf 'ID=test\nPRETTY_NAME="Preserved Root"\n' > "$top/@rollback-before-test/etc/os-release"
    : > "$top/@rollback-before-test/etc/fstab"
    printf 'ID=test\nPRETTY_NAME="Failed Candidate"\n' > "$top/@/etc/os-release"
}

# 16a. Damaged preserved root: return 0, emit the skipped-reconcile message, and
#      never mount/reconcile the promoted root.
build_preserved_root_damaged_fixture
: > "$sandbox/preserved-commands.log"
damaged_preserved="$(run_preserved_root_harness 'snapshot_restore_preserved_root "$SANDBOX/preserved-top" "@rollback-before-test" "@rollback-failed-test" "375"
printf "RESTORE_RC=%s\n" "$?"')"
grep -Fqx 'RESTORE_RC=0' <<<"$damaged_preserved" \
    || { echo 'FAIL: damaged preserved-root recovery did not return 0' >&2; printf '%s\n' "$damaged_preserved" >&2; exit 1; }
grep -Fq 'MSGLOG:rollback-preserved-root-damaged' <<<"$damaged_preserved" \
    || { echo 'FAIL: damaged preserved-root recovery did not emit the skipped-reconcile message' >&2; printf '%s\n' "$damaged_preserved" >&2; exit 1; }
if grep -Fq 'mount-promoted' "$sandbox/preserved-commands.log" || grep -Fq 'reconcile' "$sandbox/preserved-commands.log"; then
    echo 'FAIL: damaged preserved-root recovery still mounted/reconciled the promoted root' >&2
    cat "$sandbox/preserved-commands.log" >&2
    exit 1
fi
[[ -d "$sandbox/preserved-top/@/etc" && ! -f "$sandbox/preserved-top/@/etc/os-release" ]] \
    || { echo 'FAIL: damaged preserved root was not restored to @ (name restore)' >&2; exit 1; }
[[ -f "$sandbox/preserved-top/@rollback-failed-test/etc/os-release" ]] \
    || { echo 'FAIL: failed candidate was not moved out of @' >&2; exit 1; }

# 16b. Healthy preserved root: still proceeds to mount + reconcile and returns 0.
build_preserved_root_healthy_fixture
: > "$sandbox/preserved-commands.log"
healthy_preserved="$(run_preserved_root_harness 'snapshot_restore_preserved_root "$SANDBOX/preserved-top" "@rollback-before-test" "@rollback-failed-test" "375"
printf "RESTORE_RC=%s\n" "$?"')"
grep -Fqx 'RESTORE_RC=0' <<<"$healthy_preserved" \
    || { echo 'FAIL: healthy preserved-root recovery did not return 0' >&2; printf '%s\n' "$healthy_preserved" >&2; exit 1; }
grep -Fq 'mount-promoted' "$sandbox/preserved-commands.log" \
    || { echo 'FAIL: healthy preserved-root recovery did not mount the promoted root' >&2; exit 1; }
grep -Fq 'reconcile' "$sandbox/preserved-commands.log" \
    || { echo 'FAIL: healthy preserved-root recovery did not reconcile the boot stack' >&2; exit 1; }
if grep -Fq 'MSGLOG:rollback-preserved-root-damaged' <<<"$healthy_preserved"; then
    echo 'FAIL: healthy preserved-root recovery took the damaged skip path' >&2
    printf '%s\n' "$healthy_preserved" >&2
    exit 1
fi

echo "PASS: host snapshot rollback helper contract is wired, fail-closed and transactional."
