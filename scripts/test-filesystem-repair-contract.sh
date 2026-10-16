#!/usr/bin/env bash
# This contract test sources the helper under test dynamically (with the
# dispatch main stripped) in the behavioural parts, so ShellCheck cannot
# follow the source.
# shellcheck disable=SC1090
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="$ROOT_DIR/scripts/boot-repair-helper.sh"
UI_SOURCE="$ROOT_DIR/src/MainWindow.cpp"

[[ -x "$HELPER" ]] || { echo "FAIL: helper is not executable" >&2; exit 1; }
[[ -f "$UI_SOURCE" ]] || { echo "FAIL: UI source is missing" >&2; exit 1; }
bash -n "$HELPER"

# ---------------------------------------------------------------------------
# Static wiring contracts
# ---------------------------------------------------------------------------
grep -q '^fs_inspect()' "$HELPER"
grep -q '^fs_repair()' "$HELPER"
grep -q '^filesystem_scope_resolve()' "$HELPER"
grep -q '^filesystem_capability_evidence()' "$HELPER"
grep -q '^  \$PROGRAM_NAME fs-inspect' "$HELPER"
grep -q '^  \$PROGRAM_NAME fs-repair' "$HELPER"
grep -q '^  \$PROGRAM_NAME host-fs-inspect' "$HELPER"
grep -q '^  \$PROGRAM_NAME host-fs-repair' "$HELPER"
grep -q 'fs-inspect|fs-repair' "$HELPER"
grep -q 'host-fs-inspect|host-fs-repair' "$HELPER"
grep -q '^        fs-inspect)' "$HELPER"
grep -q '^        fs-repair)' "$HELPER"
grep -q '^        host-fs-inspect)' "$HELPER"
grep -q '^        host-fs-repair)' "$HELPER"
grep -q 'filesystem_scope_tools >/dev/null' "$HELPER"

# install.sh preflights every check tool the engine can run, so a missing
# package is reported before the user reaches the repair UI.
for check_tool in e2fsck fsck.fat btrfs xfs_repair fsck.exfat ntfsfix fsck.f2fs jfs_fsck reiserfsck zpool; do
    grep -q "\"$check_tool:" "$ROOT_DIR/scripts/install.sh" \
        || { echo "FAIL: install.sh preflight does not cover $check_tool" >&2; exit 1; }
done

# The capability key is present and keeps every existing key unchanged.
grep -q 'local -a keys=(validate filesystem dpkg fixbroken aptupdate upgrade dkms display gpu initramfs efi grub extlinux bootstack kernels)' "$HELPER"
grep -Fq 'File system check %s: %s uuid=%s mount=%s tool=%s result=%s' "$HELPER"
grep -Fq 'File system check detail %s: tool=%s output=%s' "$HELPER"
grep -Fq 'File system check summary: devices=0 clean=0 issues=0 unsupported=0 tool-missing=0 skipped=0' "$HELPER"
grep -Fq 'File system repair %s: %s uuid=%s mount=%s tool=%s mode=%s result=%s' "$HELPER"

# Every required filesystem-specific invocation is defined verbatim.
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" -f -n "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" -f -p "$device")' "$HELPER"
grep -Fq 'filesystem_run_repair_command "$tool_name" -f -y "$device"' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" -n "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" -e "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" check --readonly "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" check --repair "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" rescue super-recover -y "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" scrub start -B -d "$mountpoint")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" -a -v "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" -p "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" -f "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" --check "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" -y --fix-fixable "$device")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" status -v "$pool")' "$HELPER"
grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" scrub -w "$pool")' "$HELPER"
grep -q 'f2fs:repair)' "$HELPER"
if grep -q 'f2fs:check' "$HELPER"; then
    echo 'FAIL: f2fs must not be inspected (fsck.f2fs -n does not exist)' >&2
    exit 1
fi
# Forbidden/retired invocations: xfs_repair -L (log destruction), generic -y
# repair, and the old jfs_fsck -y / reiserfsck prompt-hanging form.
if grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" -L' "$HELPER"; then
    echo 'FAIL: xfs_repair -L must never be offered' >&2
    exit 1
fi
if grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" -y "$device")' "$HELPER"; then
    echo 'FAIL: a generic -y repair invocation is still present' >&2
    exit 1
fi
if grep -Fq 'FS_INSPECT_ARGUMENTS=("$tool" -f -y "$device")' "$HELPER"; then
    echo 'FAIL: the old preen-skipping e2fsck repair invocation is still present' >&2
    exit 1
fi
grep -q 'msg_log btrfs-repair-warning' "$HELPER"
grep -q 'msg_log ntfsfix-note' "$HELPER"
grep -q '^filesystem_mode_matrix()' "$HELPER"
grep -q '^filesystem_zfs_status_result()' "$HELPER"
grep -q '^filesystem_btrfs_scrub_result()' "$HELPER"
# ZFS diagnostics: the read-only pool/dataset inventory is wired into the
# "File systems" diagnostic and emits the four documented read-only commands;
# the guarded zpool clear follow-up runs only after a clean scrub.
grep -q '^diagnostic_zfs_status()' "$HELPER"
grep -Fq 'diagnostic_zfs_status' <<<"$(sed -n '/^diagnostic_filesystem()/,/^}/p' "$HELPER")" \
    || { echo 'FAIL: the File systems diagnostic does not emit the ZFS pool inventory' >&2; exit 1; }
for zfs_diag_cmd in 'zpool status' 'zpool list -v' 'zfs list' 'zpool get all'; do
    grep -Fq "$zfs_diag_cmd" <<<"$(sed -n '/^diagnostic_zfs_status()/,/^}/p' "$HELPER")" \
        || { echo "FAIL: the ZFS diagnostic does not emit: $zfs_diag_cmd" >&2; exit 1; }
done
grep -Fq 'filesystem_run_repair_command "$tool_name" clear "$pool"' "$HELPER" \
    || { echo 'FAIL: the clean-scrub zpool clear follow-up is missing' >&2; exit 1; }
grep -Fq '&& "$result" == "clean"' "$HELPER" \
    || { echo 'FAIL: zpool clear is not gated on a clean scrub result' >&2; exit 1; }
grep -q 'result=skipped' "$HELPER"
grep -q '^mount_target_btrfs_subvolumes()' "$HELPER"
grep -q '^mount_target_data_partitions()' "$HELPER"
grep -q '^remount_target_data_rw()' "$HELPER"
grep -q 'mount_target_data_partitions "$mode"' "$HELPER"
grep -q 'msg_log remount-data-rw ' "$HELPER"
grep -q 'same_single_top_disk "$TARGET_DISK" "$resolved"' "$HELPER"
grep -q 'Repair requires the target ' "$HELPER"
# A2-01/A4-01: the fstab-driven mount paths share one containment validator
# (lexical ".." refusal, realpath-parent containment and final-component
# symlink refusal) applied before any mkdir/mount.
grep -q '^validate_target_mount_dest()' "$HELPER"
boot_entry_block="$(sed -n '/^mount_target_boot_entry()/,/^}/p' "$HELPER")"
grep -Fq 'validate_target_mount_dest "$dest"' <<<"$boot_entry_block" \
    || { echo 'FAIL: mount_target_boot_entry bypasses the shared mount-destination containment' >&2; exit 1; }
data_partitions_block="$(sed -n '/^mount_target_data_partitions()/,/^}/p' "$HELPER")"
subvolumes_block="$(sed -n '/^mount_target_btrfs_subvolumes()/,/^}/p' "$HELPER")"
for mount_block in "$data_partitions_block" "$subvolumes_block"; do
    grep -Fq 'validate_target_mount_dest "$dest"' <<<"$mount_block" \
        || { echo 'FAIL: a fstab-driven mount path does not run the shared destination containment' >&2; exit 1; }
    awk '
        /validate_target_mount_dest "\$dest"/ { saw_validate = 1 }
        saw_validate && /mkdir -p -- "\$dest"/ { mkdir_after = 1 }
        saw_validate && /mount_recorded "\$resolved" "\$dest"/ { mount_after = 1 }
        END { exit(mkdir_after && mount_after ? 0 : 1) }
    ' <<<"$mount_block" \
        || { echo 'FAIL: the mount-destination containment does not precede mkdir/mount' >&2; exit 1; }
done
# A4-08: fstab context=/seclabel never pass through and nosuid,nodev is
# appended last (rightmost wins over fstab suid,dev) in both data paths.
for mount_block in "$data_partitions_block" "$subvolumes_block"; do
    grep -Fq 'context=*|seclabel) continue ;;' <<<"$mount_block" \
        || { echo 'FAIL: a data mount path still passes fstab context=/seclabel through' >&2; exit 1; }
    grep -Fq 'mount_options="$requested_mode${filtered_options:+,$filtered_options},nosuid,nodev"' <<<"$mount_block" \
        || { echo 'FAIL: a data mount path does not append the recovery nosuid,nodev hardening' >&2; exit 1; }
done
# A4-09: the repair preflight derives an unmounted ESP discovered by GPT type
# on the selected disk through the same guarded boot-entry path (same-disk
# gate, FAT proof, destination containment, recorded mount) and only when
# exactly one ESP partition exists.
grep -q '^target_esp_partition_by_type()' "$HELPER" \
    || { echo 'FAIL: target_esp_partition_by_type is missing' >&2; exit 1; }
grep -q '^target_esp_derivable()' "$HELPER" \
    || { echo 'FAIL: target_esp_derivable is missing' >&2; exit 1; }
grep -q '^mount_target_esp_by_type()' "$HELPER" \
    || { echo 'FAIL: mount_target_esp_by_type is missing' >&2; exit 1; }
esp_by_type_block="$(sed -n '/^mount_target_esp_by_type()/,/^}/p' "$HELPER")"
for guard_fragment in 'validate_target_mount_dest "$dest"' 'same_single_top_disk "$TARGET_DISK" "$source"' 'mount_recorded "$source" "$dest"'; do
    grep -Fq "$guard_fragment" <<<"$esp_by_type_block" \
        || { echo "FAIL: mount_target_esp_by_type drops the sweep guard: $guard_fragment" >&2; exit 1; }
done
esp_partition_probe_block="$(sed -n '/^target_esp_partition_by_type()/,/^}/p' "$HELPER")"
grep -Fq 'C12A7328-F81F-11D2-BA4B-00A0C93EC93B' <<<"$esp_partition_probe_block" \
    || { echo 'FAIL: the by-type ESP probe does not match the GPT ESP partition type' >&2; exit 1; }
grep -Fq 'toupper($2)' <<<"$esp_partition_probe_block" \
    || { echo 'FAIL: the by-type ESP probe does not normalize the PARTTYPE case (lsblk prints lowercase)' >&2; exit 1; }
grep -Eq 'count == 1' <<<"$esp_partition_probe_block" \
    || { echo 'FAIL: the by-type ESP probe does not require exactly one ESP partition' >&2; exit 1; }
run_repair_block="$(sed -n '/^run_repair()/,/^}/p' "$HELPER")"
grep -Fq 'mount_target_esp_by_type ro || true' <<<"$run_repair_block" \
    || { echo 'FAIL: the repair mandatory preflight does not derive an ESP discovered by GPT type' >&2; exit 1; }
# A failed mutating stage must release the helper-owned target mount (plain
# umount, never lazy) and leave no MOUNT_LEAK.  The EXIT trap cleanup() runs the
# non-lazy release, and it must never `rm -rf` the session directory while a
# helper-owned mount is still attached (that would recurse into the mounted
# target and delete its files, breaking the next diagnose with "does not contain
# /etc/os-release").
cleanup_body="$(sed -n '/^cleanup()/,/^}/p' "$HELPER")"
if grep -Fq 'umount -l' <<<"$cleanup_body"; then
    echo 'FAIL: the EXIT trap cleanup still falls back to a lazy umount' >&2
    exit 1
fi
grep -Fq 'still_mounted' <<<"$cleanup_body" \
    || { echo 'FAIL: cleanup() does not guard the session directory removal against a still-mounted helper mount' >&2; exit 1; }
cleanup_rm_line="$(grep -n 'rm -rf -- "\$SESSION_DIR"' <<<"$cleanup_body" | head -n1 | cut -d: -f1 || true)"
cleanup_guard_line="$(grep -n 'still_mounted' <<<"$cleanup_body" | head -n1 | cut -d: -f1 || true)"
[[ -n "$cleanup_rm_line" && -n "$cleanup_guard_line" && "$cleanup_guard_line" -lt "$cleanup_rm_line" ]] \
    || { echo 'FAIL: cleanup() must check for a still-mounted helper mount before removing the session directory' >&2; exit 1; }
# A2-05: both the offline repair and the read-only inspection release paths
# never lazy-detach; a busy mount must stay visible (leak evidence) so the
# check path can skip its offline-only tool instead of running it against a
# still-attached filesystem.  The post-release "still mounted" probe scans the
# live findmnt table without the TARGET_ROOT skip, so a btrfs device that still
# has a subvolume mounted is detected instead of reported unmounted.
release_for_device_block="$(sed -n '/^filesystem_release_mounts_for_device()/,/^}/p' "$HELPER")"
if grep -Fq 'umount -l' <<<"$release_for_device_block"; then
    echo 'FAIL: the offline repair path still falls back to a lazy umount' >&2
    exit 1
fi
grep -Fq 'mount_cleanup_leak_evidence "$mountpath"' <<<"$release_for_device_block" \
    || { echo 'FAIL: the offline repair path does not record leak evidence on an umount failure' >&2; exit 1; }
release_all_block="$(sed -n '/^filesystem_release_all_mounts()/,/^}/p' "$HELPER")"
if grep -Fq 'umount -l' <<<"$release_all_block"; then
    echo 'FAIL: the read-only inspection release path still falls back to a lazy umount' >&2
    exit 1
fi
grep -Fq 'mount_cleanup_leak_evidence "$mountpath"' <<<"$release_all_block" \
    || { echo 'FAIL: the read-only inspection release path does not record leak evidence on an umount failure' >&2; exit 1; }
grep -q '^filesystem_device_still_mounted()' "$HELPER" \
    || { echo 'FAIL: the post-release still-mounted probe is missing' >&2; exit 1; }
grep -Fq 'findmnt -rn -o SOURCE,TARGET' <<<"$(sed -n '/^filesystem_device_still_mounted()/,/^}/p' "$HELPER")" \
    || { echo 'FAIL: the still-mounted probe does not scan the live findmnt table' >&2; exit 1; }
grep -Fq 'filesystem_device_still_mounted "$device"' <<<"$(sed -n '/^fs_inspect_scope()/,/^}/p' "$HELPER")" \
    || { echo 'FAIL: fs_inspect_scope does not re-probe the live mount state through the still-mounted probe' >&2; exit 1; }
# A2-05 (extended): the exit-trap cleanup and the stale-session sweep must also
# never lazy-detach a btrfs target — `umount -l` leaves the filesystem
# superblock (fs_info) alive holding the device with FMODE_EXCL, so a later
# `btrfs check` fails "Device or resource busy".  A busy mount stays visible
# (leak evidence) so the next session's sweep can retry a plain umount.
cleanup_block="$(sed -n '/^cleanup()/,/^}/p' "$HELPER")"
if grep -Fq 'umount -l' <<<"$cleanup_block"; then
    echo 'FAIL: the exit-trap cleanup still falls back to a lazy umount' >&2
    exit 1
fi
sweep_stale_block="$(sed -n '/^sweep_stale_target_mounts()/,/^}/p' "$HELPER")"
if grep -Fq 'umount -l' <<<"$sweep_stale_block"; then
    echo 'FAIL: the stale-session sweep still falls back to a lazy umount' >&2
    exit 1
fi
# A4-04: the LUKS header UUID is validated against the canonical hyphenated
# form before it composes the luks-<UUID> mapper name.
grep -Fq '^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$' "$HELPER" \
    || { echo 'FAIL: the LUKS UUID canonical-form validation is missing' >&2; exit 1; }
grep -Fq 'The LUKS header reported a malformed UUID; refusing to unlock' "$HELPER" \
    || { echo 'FAIL: the malformed LUKS UUID refusal reason is missing' >&2; exit 1; }
# A3-10: every EFI/bootloader apply stage re-runs the target/host identity
# gate as its first statement (native host maintenance skips it inside the
# helper itself).
grep -q '^reassert_target_write_safety()' "$HELPER"
for apply_fn in reinstall_efi_bootloader fedora_grub_reinstall_boot_code run_tuxedo_uki_builder; do
    apply_body="$(sed -n "/^${apply_fn}()/,/^}/p" "$HELPER")"
    grep -Fq 'reassert_target_write_safety' <<<"$apply_body" \
        || { echo "FAIL: $apply_fn does not reassert target write safety" >&2; exit 1; }
    awk '
        /^[[:space:]]*[a-zA-Z_][a-zA-Z0-9_]*\(\)[[:space:]]*$/ { in_fn = 1; started = 0; found = 0; next }
        in_fn && /^}/ { exit(found ? 0 : 1) }
        in_fn && started == 0 {
            if ($0 ~ /^[[:space:]]*local[[:space:]]/ || $0 ~ /^[[:space:]]*#/ || $0 ~ /^[[:space:]]*$/ || $0 ~ /^\{[[:space:]]*$/) next
            started = 1
            if ($0 ~ /reassert_target_write_safety/) found = 1
            else exit 1
            next
        }
        END { exit(found ? 0 : 1) }
    ' <<<"$apply_body" \
        || { echo "FAIL: $apply_fn does not reassert target write safety as its first statement" >&2; exit 1; }
done
# The data partitions are recorded in the same list the Btrfs subvolumes use,
# so the single rw promotion covers both layouts.
grep -q 'TARGET_DATA_MOUNTS+=("$dest")' "$HELPER"
grep -q 'summary_skipped += 1' "$HELPER"
grep -q 'Offline file system repair refuses mounted filesystem' "$HELPER"
grep -q 'Online file system repair requires' "$HELPER"
grep -q 'is not part of the resolved scope' "$HELPER"

# ---------------------------------------------------------------------------
# ESP writability static wiring.  The effective mount is the topmost findmnt
# row, never the bottom/autofs row and never a reused mount ID; the leaked
# preflight and its manual cleanup command are wired into the TUXEDO UKI path.
# ---------------------------------------------------------------------------
grep -q '^target_mount_stack()' "$HELPER"
grep -q '^target_mount_top()' "$HELPER"
grep -q '^esp_mount_probe()' "$HELPER"
grep -q '^esp_writable_preflight()' "$HELPER"
grep -q '^esp_mount_cleanup_command()' "$HELPER"
grep -q '^esp_mount_unit_name()' "$HELPER"
grep -q '^esp_automount_unit_present()' "$HELPER"
grep -q '^esp_auto_mount_if_configured()' "$HELPER"
grep -q '^mount_cleanup_leak_evidence()' "$HELPER"
grep -q '^mount_records_prune()' "$HELPER"
grep -q '^PREEXISTING_MOUNTS=()' "$HELPER"

# Auto-mount wiring: the writability preflight brings up a configured-but-
# unmounted ESP and keeps the actionable hint in its refusal, while every
# read-only diagnostic entry point stays free of any mount attempt.
preflight_body="$(sed -n '/^esp_writable_preflight()/,/^}/p' "$HELPER")"
grep -Fq 'esp_auto_mount_if_configured' <<<"$preflight_body" \
    || { echo 'FAIL: esp_writable_preflight does not auto-mount a configured unmounted ESP' >&2; exit 1; }
grep -Fq 'ESP_MOUNT_HINT' <<<"$preflight_body" \
    || { echo 'FAIL: esp_writable_preflight refusal does not carry the auto-mount hint' >&2; exit 1; }
for diag_fn in run_host_diagnostic run_target_diagnostic diagnostic_uki; do
    diag_body="$(sed -n "/^${diag_fn}()/,/^}/p" "$HELPER")"
    if grep -Fq 'esp_auto_mount_if_configured' <<<"$diag_body"; then
        echo "FAIL: read-only ${diag_fn} calls the ESP auto-mount" >&2
        exit 1
    fi
done
rw_probe_body="$(sed -n '/^target_path_is_mounted_rw()/,/^}/p' "$HELPER")"
grep -q 'target_mount_top' <<<"$rw_probe_body" \
    || { echo 'FAIL: target_path_is_mounted_rw does not use the topmost-mount probe' >&2; exit 1; }
if grep -q 'head -n1' <<<"$rw_probe_body"; then
    echo 'FAIL: target_path_is_mounted_rw still reads the bottom mount row (head -n1)' >&2
    exit 1
fi
stack_body="$(sed -n '/^target_mount_stack()/,/^}/p' "$HELPER")"
if grep -Eq 'sort|max' <<<"$stack_body"; then
    echo 'FAIL: target_mount_stack must not order mounts by ID' >&2
    exit 1
fi
top_body="$(sed -n '/^target_mount_top()/,/^}/p' "$HELPER")"
grep -Fq 'tail -n1' <<<"$top_body" \
    || { echo 'FAIL: target_mount_top must select the last (topmost) row' >&2; exit 1; }
grep -Fq 'dest="$(target_path "$mp")"' "$HELPER" \
    || { echo 'FAIL: mount_target_boot_entry bypasses target_path for the host join' >&2; exit 1; }
preflight_uki_body="$(sed -n '/^preflight_tuxedo_uki()/,/^}/p' "$HELPER")"
grep -Fq 'esp_writable_preflight clear' <<<"$preflight_uki_body" \
    || { echo 'FAIL: preflight_tuxedo_uki does not run the ESP writability preflight' >&2; exit 1; }
rebuild_uki_body="$(sed -n '/^rebuild_tuxedo_uki()/,/^}/p' "$HELPER")"
grep -Fq 'esp_writable_preflight clear' <<<"$rebuild_uki_body" \
    || { echo 'FAIL: rebuild_tuxedo_uki does not run the ESP writability preflight' >&2; exit 1; }
builder_body="$(sed -n '/^run_tuxedo_uki_builder()/,/^}/p' "$HELPER")"
grep -Fq 'esp_writable_preflight check' <<<"$builder_body" \
    || { echo 'FAIL: run_tuxedo_uki_builder does not re-probe before the vendor script' >&2; exit 1; }
# The TUXEDO vendor builder resolves the root UUID from the chroot's bound
# host /proc (host-tainted) and its blkid fallback fails closed on the
# filtered chroot /dev; the helper therefore exports the known target root
# UUID as HOST_ROOT_UUID in both chroot command environments and reuses the
# same hoisted value for the post-build root binding verification.
[[ "$(grep -Fc 'HOST_ROOT_UUID="$root_uuid"' <<<"$builder_body")" -eq 2 ]] \
    || { echo 'FAIL: run_tuxedo_uki_builder does not export HOST_ROOT_UUID in both chroot envs' >&2; exit 1; }
grep -Fq 'root_uuid="${TUXEDO_UKI_ROOT_UUID:-}"' <<<"$builder_body" \
    || { echo 'FAIL: run_tuxedo_uki_builder does not reuse the caller-hoisted root UUID' >&2; exit 1; }
grep -Fq 'root_uuid="$(blkid -s UUID -o value "$ROOT_CANONICAL" 2>/dev/null || true)"' <<<"$builder_body" \
    || { echo 'FAIL: run_tuxedo_uki_builder does not hoist the target root UUID from $ROOT_CANONICAL' >&2; exit 1; }
grep -Fq 'root_uuid="$(blkid -s UUID -o value "$ROOT_CANONICAL" 2>/dev/null || true)"' <<<"$rebuild_uki_body" \
    || { echo 'FAIL: rebuild_tuxedo_uki does not hoist the target root UUID before the vendor builder' >&2; exit 1; }
grep -Fq 'TUXEDO_UKI_ROOT_UUID="$root_uuid"' <<<"$rebuild_uki_body" \
    || { echo 'FAIL: rebuild_tuxedo_uki does not publish the hoisted root UUID' >&2; exit 1; }
verify_uki_body="$(sed -n '/^verify_tuxedo_uki_root_binding()/,/^}/p' "$HELPER")"
grep -Fq 'root_uuid="${TUXEDO_UKI_ROOT_UUID:-}"' <<<"$verify_uki_body" \
    || { echo 'FAIL: verify_tuxedo_uki_root_binding does not reuse the hoisted root UUID' >&2; exit 1; }
grep -Fq 'Rebuilt TUXEDO UKI does not reference the promoted root filesystem UUID $root_uuid.' <<<"$verify_uki_body" \
    || { echo 'FAIL: the post-build root binding failure message changed' >&2; exit 1; }
host_prepare_body="$(sed -n '/^prepare_running_host()/,/^}/p' "$HELPER")"
grep -Fq 'esp_writable_preflight clear' <<<"$host_prepare_body" \
    || { echo 'FAIL: prepare_running_host does not run the ESP writability preflight' >&2; exit 1; }
grep -Fq 'esp_auto_mount_if_configured' <<<"$host_prepare_body" \
    || { echo 'FAIL: prepare_running_host does not auto-mount a configured unmounted ESP' >&2; exit 1; }
grep -Fq 'require_rw" == yes' <<<"$host_prepare_body" \
    || { echo 'FAIL: prepare_running_host auto-mount is not gated on the repair write intent' >&2; exit 1; }
if grep -Fq 'target_path_is_mounted_rw "$TARGET_ESP_MOUNT"' <<<"$host_prepare_body"; then
    echo 'FAIL: prepare_running_host still gates the ESP on the bottom-row probe' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# Live harness.  Mock tools and a generated harness script source the helper
# (without main) and override only mount/topology plumbing.  No real block
# device, mount or filesystem is ever touched.
# ---------------------------------------------------------------------------
sandbox="$(mktemp -d)"
cleanup_sandbox() {
    rm -rf -- "$sandbox"
}
trap cleanup_sandbox EXIT

mkdir -p "$sandbox/mockbin" "$sandbox/target/etc" "$sandbox/target/boot/efi" "$sandbox/state"
printf 'ID=arch\nID_LIKE=arch\nPRETTY_NAME="Contract Test"\n' > "$sandbox/target/etc/os-release"

for name in test-disk test-root test-boot test-efi test-home test-data test-outside; do
    : > "$sandbox/dev-$name"
done

# Device metadata database consumed by the mock lsblk/blkid helpers:
#   <lsblk name> <fstype|-> <type|-> <uuid|-> <parent|-> <partition|-> <pkname|->
printf 'test-disk - disk - - - -\n' > "$sandbox/devices.db"
printf 'test-root ext4 part 11111111-2222-3333-4444-555555555555 test-disk 1 test-disk\n' >> "$sandbox/devices.db"
printf 'test-boot ext4 part 66666666-7777-8888-9999-aaaaaaaaaaaa test-disk 1 test-disk\n' >> "$sandbox/devices.db"
printf 'test-efi vfat part ABCD-1234 test-disk 1 test-disk C12A7328-F81F-11D2-BA4B-00A0C93EC93B\n' >> "$sandbox/devices.db"
printf 'test-home xfs part BBBBBBBB-CCCC-DDDD-EEEE-FFFFFFFFFFFF test-disk 1 test-disk\n' >> "$sandbox/devices.db"
printf 'test-data reiserfs part 9999 test-disk 1 test-disk\n' >> "$sandbox/devices.db"
printf 'test-outside ext4 part OUTSIDE-UUID other-disk 1 other-disk\n' >> "$sandbox/devices.db"

cat > "$sandbox/mockbin/lsblk" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
device=""
for arg in "$@"; do
    [[ "$arg" == /* ]] && device="$arg"
done
[[ -n "${FAKE_LSBLK_DB:-}" && -f "$FAKE_LSBLK_DB" ]] || exit 0
name="$(basename -- "$device")"; name="${name#dev-}"
field() { awk -v n="$name" -v c="$1" '$1 == n {print $c; exit}' "$FAKE_LSBLK_DB"; }
[[ "$(field 2)" != "-" ]] && fstype="$(field 2)" || fstype=""
[[ "$(field 3)" != "-" ]] && type="$(field 3)" || type=""
[[ "$(field 5)" != "-" ]] && parent="$(field 5)" || parent=""
[[ "$(field 6)" != "-" ]] && partition="$(field 6)" || partition=""
[[ "$(field 7)" != "-" ]] && pkname="$(field 7)" || pkname=""
# Partition listing (lsblk -rno NAME,PARTTYPE <disk>): every device whose
# parent is the selected disk, plus the disk itself, with the GPT type when
# the database records one (column 8).
if [[ " $* " == *" NAME,PARTTYPE "* || " $* " == *" PARTTYPE,NAME "* ]]; then
    awk -v d="$name" '
        ($1 == d || $5 == d) && $8 != "" && $8 != "-" { print $1, $8 }
    ' "$FAKE_LSBLK_DB"
    exit 0
fi
for arg in "$@"; do
    case "$arg" in
        *PKNAME*) [[ -n "$pkname" ]] && printf '%s\n' "$pkname" ;;
        *KNAME*) [[ -n "$name" ]] && printf '%s\n' "$name" ;;
        *FSTYPE*) [[ -n "$fstype" ]] && printf '%s\n' "$fstype" ;;
        *MOUNTPOINTS*) printf '\n' ;;
        *NAME*) [[ -n "$type" ]] && printf '%s\n' "$device" ;;
        *TYPE*) [[ -n "$type" ]] && printf '%s\n' "$type" ;;
    esac
done
exit 0
MOCK

cat > "$sandbox/mockbin/blkid" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
device=""
for arg in "$@"; do
    [[ "$arg" == /* ]] && device="$arg"
done
[[ -n "${FAKE_LSBLK_DB:-}" && -f "$FAKE_LSBLK_DB" ]] || exit 0
name="$(basename -- "$device")"; name="${name#dev-}"
field() { awk -v n="$name" -v c="$1" '$1 == n {print $c; exit}' "$FAKE_LSBLK_DB"; }
for ((i=1; i<=$#; ++i)); do
    if [[ "${!i}" == "-s" ]]; then
        j=$((i+1)); fieldname="${!j}"
        if [[ "$fieldname" == UUID ]]; then
            [[ "$(field 4)" != "-" ]] && field 4
        fi
        exit 0
    fi
    if [[ "${!i}" == "-U" ]]; then
        j=$((i+1)); uuid="${!j}"
        awk -v u="$uuid" '$4 == u {print "/dev/" $1; exit}' "$FAKE_LSBLK_DB"
        exit 0
    fi
done
for ((i=1; i<=$#; ++i)); do
    if [[ "${!i}" == "-o" ]]; then
        j=$((i+1))
        if [[ "${!j}" == value ]]; then
            [[ "$(field 4)" != "-" ]] && field 4
        fi
        exit 0
    fi
done
exit 0
MOCK

cat > "$sandbox/mockbin/findmnt" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
target=""
columns=""
for ((i=1; i<=$#; ++i)); do
    if [[ "${!i}" == "--target" ]]; then
        j=$((i+1)); target="${!j}"
    fi
    if [[ "${!i}" == "-o" ]]; then
        j=$((i+1)); columns="${!j}"
    fi
done
[[ -n "${FAKE_FINDMNT_DB:-}" && -f "$FAKE_FINDMNT_DB" ]] || exit 0
if [[ "$columns" == "SOURCE,TARGET" ]]; then
    awk '$1 != "" { print $2, $1 }' "$FAKE_FINDMNT_DB"
    exit 0
fi
# TARGET,SOURCE,OPTIONS,ID rows carry the stack order (bottom-most first) that
# findmnt --target publishes.  Fields 4/5 default to rw/0 so older fixtures
# keep working; mount IDs are deliberately not unique across fixtures.
if [[ "$columns" == "TARGET,SOURCE,OPTIONS,ID" ]]; then
    awk -v t="$target" '
        $1 == t {
            options = ($4 == "" ? "rw" : $4)
            id = ($5 == "" ? "0" : $5)
            print $1, $2, options, id
        }
    ' "$FAKE_FINDMNT_DB"
    exit 0
fi
# Every stacked mount for the target is printed, matching findmnt --target:
# a systemd automount unit can publish a synthetic autofs source before the
# real filesystem, and the resolver must skip it instead of treating the
# whole path as unmounted.
awk -v t="$target" -v c="$columns" '
    $1 == t {
        if (c == "SOURCE") print $2
        else if (c == "FSTYPE") print $3
        else if (c == "TARGET") print $1
        else print $2, $3
    }
' "$FAKE_FINDMNT_DB"
MOCK

cat > "$sandbox/mockbin/mountpoint" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
[[ -n "${FAKE_MOUNTPOINT_DB:-}" && -f "$FAKE_MOUNTPOINT_DB" ]] || exit 1
grep -Fqx -- "${!#}" "$FAKE_MOUNTPOINT_DB"
MOCK

cat > "$sandbox/mockbin/mount" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
[[ -n "${FAKE_MOUNT_LOG:-}" ]] && printf '%s\n' "$*" >> "$FAKE_MOUNT_LOG"
if [[ -n "${FAKE_MOUNT_FAIL:-}" ]]; then
    printf 'mount: %s: mount failed\n' "${!#}" >&2
    exit 1
fi
# A remount (`mount -o remount,rw`) promotes the helper-recorded entry instead
# of creating a second one.  It must not append another row: a duplicate row
# makes findmnt emit two FAT rows, and the ESP re-derivation's first-row
# `grep -q` match can then SIGPIPE findmnt under `set -o pipefail` on a loaded
# rig, intermittently dropping the /boot/efi derivation.
case " $* " in
    *remount*) exit 0 ;;
esac
# A successful mount becomes visible to mountpoint/findmnt so the helper's
# post-mount verification exercises the real follow-up probes.
if [[ -n "${FAKE_MOUNT_ADD_POINT:-}" && -n "${FAKE_MOUNTPOINT_DB:-}" ]]; then
    printf '%s\n' "$FAKE_MOUNT_ADD_POINT" >> "$FAKE_MOUNTPOINT_DB"
fi
if [[ -n "${FAKE_MOUNT_ADD_ROW:-}" && -n "${FAKE_FINDMNT_DB:-}" ]]; then
    printf '%s\n' "$FAKE_MOUNT_ADD_ROW" >> "$FAKE_FINDMNT_DB"
fi
exit 0
MOCK

# systemd stub for the automount trigger.  list-unit-files publishes the
# configured unit names; start records the call and can publish the mount the
# kernel automount would create on first access.
cat > "$sandbox/mockbin/systemctl" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
[[ -n "${FAKE_SYSTEMCTL_LOG:-}" ]] && printf '%s\n' "$*" >> "$FAKE_SYSTEMCTL_LOG"
case "${1:-}" in
    list-unit-files)
        [[ -n "${FAKE_SYSTEMCTL_UNITS:-}" ]] && printf '%s\n' "$FAKE_SYSTEMCTL_UNITS"
        exit 0
        ;;
    start)
        if [[ -n "${FAKE_SYSTEMCTL_FAIL:-}" ]]; then
            printf 'Failed to start %s\n' "${2:-}" >&2
            exit 1
        fi
        if [[ -n "${FAKE_SYSTEMCTL_ADD_POINT:-}" && -n "${FAKE_MOUNTPOINT_DB:-}" ]]; then
            printf '%s\n' "$FAKE_SYSTEMCTL_ADD_POINT" >> "$FAKE_MOUNTPOINT_DB"
        fi
        if [[ -n "${FAKE_SYSTEMCTL_ADD_ROW:-}" && -n "${FAKE_FINDMNT_DB:-}" ]]; then
            printf '%s\n' "$FAKE_SYSTEMCTL_ADD_ROW" >> "$FAKE_FINDMNT_DB"
        fi
        exit 0
        ;;
esac
exit 0
MOCK

cat > "$sandbox/mockbin/umount" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
target="${!#}"
if [[ -n "${FAKE_UMOUNT_LOG:-}" ]]; then
    printf '%s\n' "$target" >> "$FAKE_UMOUNT_LOG"
fi
if [[ -n "${FAKE_UMOUNT_FAIL:-}" && "$target" == *"$FAKE_UMOUNT_FAIL"* ]]; then
    exit 1
fi
if [[ -n "${FAKE_MOUNTPOINT_DB:-}" && -f "$FAKE_MOUNTPOINT_DB" ]]; then
    grep -Fxv -- "$target" "$FAKE_MOUNTPOINT_DB" > "$FAKE_MOUNTPOINT_DB.tmp" 2>/dev/null || true
    mv "$FAKE_MOUNTPOINT_DB.tmp" "$FAKE_MOUNTPOINT_DB"
fi
if [[ -n "${FAKE_FINDMNT_DB:-}" && -f "$FAKE_FINDMNT_DB" ]]; then
    # Stacked mounts share the mountpoint: umount detaches the topmost mount,
    # so remove only the last matching row (findmnt order is bottom-to-top).
    awk -v t="$target" '
        { line[NR] = $0; if ($1 == t) last = NR }
        END { for (i = 1; i <= NR; ++i) if (i != last) print line[i] }
    ' "$FAKE_FINDMNT_DB" > "$FAKE_FINDMNT_DB.tmp"
    mv "$FAKE_FINDMNT_DB.tmp" "$FAKE_FINDMNT_DB"
fi
exit 0
MOCK

cat > "$sandbox/mockbin/readlink" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
path="${!#}"
# The contract test keeps the real /dev/test-* spellings; the sandbox files
# only prove existence.
if [[ "$path" == /dev/test-* && -e "$FAKE_DEV_DIR/dev-${path#/dev/}" ]]; then
    printf '%s\n' "$path"
    exit 0
fi
if [[ -e "$path" || -L "$path" ]]; then
    printf '%s\n' "$path"
    exit 0
fi
exit 1
MOCK

# Generic fsck-family mock.  FAKE_TOOL_FAIL names the tool whose scripted
# result applies; FAKE_TOOL_MATCH optionally restricts that to invocations
# whose arguments contain a substring (used for the e2fsck preen fallback).
for fsck_tool in e2fsck xfs_repair btrfs fsck.fat fsck.exfat ntfsfix fsck.f2fs jfs_fsck reiserfsck; do
    cat > "$sandbox/mockbin/$fsck_tool" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
tool="$(basename -- "$0")"
[[ -n "${FAKE_TOOL_LOG:-}" ]] && printf '%s %s\n' "$tool" "$*" >> "$FAKE_TOOL_LOG"
fail=0
if [[ -n "${FAKE_TOOL_FAIL:-}" && "$tool" == "$FAKE_TOOL_FAIL" ]]; then
    fail=1
fi
if (( fail == 1 )) && [[ -n "${FAKE_TOOL_MATCH:-}" && "$*" != *"$FAKE_TOOL_MATCH"* ]]; then
    fail=0
fi
if (( fail == 1 )); then
    printf '%s\n' "${FAKE_TOOL_FAIL_OUTPUT:-Filesystem is not clean; errors found}"
    exit "${FAKE_TOOL_FAIL_RC:-4}"
fi
printf '%s\n' "${FAKE_TOOL_OK_OUTPUT:-clean}"
exit "${FAKE_TOOL_OK_RC:-0}"
MOCK
done

cat > "$sandbox/mockbin/zpool" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
[[ -n "${FAKE_TOOL_LOG:-}" ]] && printf 'zpool %s\n' "$*" >> "$FAKE_TOOL_LOG"
case "$*" in
    "list -H -o name"*)
        [[ -n "${FAKE_ZPOOL_POOLS:-}" ]] && printf '%s\n' "$FAKE_ZPOOL_POOLS"
        exit 0 ;;
    "list -H -o guid"*)
        printf '%s\n' "${FAKE_ZPOOL_GUID:-0}"
        exit 0 ;;
    "status -P"*)
        [[ -n "${FAKE_ZPOOL_STATUS_P:-}" ]] && printf '%s\n' "$FAKE_ZPOOL_STATUS_P"
        exit 0 ;;
    "status -v"*)
        printf '%s\n' "${FAKE_ZPOOL_STATUS_V:-state: ONLINE errors: No known data errors}"
        exit "${FAKE_ZPOOL_STATUS_RC:-0}" ;;
    scrub*)
        printf '%s\n' "${FAKE_ZPOOL_SCRUB_OUTPUT:-scrub completed}"
        exit "${FAKE_ZPOOL_SCRUB_RC:-0}" ;;
    clear*)
        printf '%s\n' "${FAKE_ZPOOL_CLEAR_OUTPUT:-}"
        exit 0 ;;
esac
exit 0
MOCK

chmod +x "$sandbox/mockbin"/*

export FAKE_LSBLK_DB="$sandbox/devices.db"
export FAKE_FINDMNT_DB="$sandbox/findmnt.db"
export FAKE_MOUNTPOINT_DB="$sandbox/mountpoints.txt"
export FAKE_MOUNT_LOG="$sandbox/mount.log"
export FAKE_UMOUNT_LOG="$sandbox/umount.log"
export FAKE_SYSTEMCTL_LOG="$sandbox/systemctl.log"
export FAKE_TOOL_LOG="$sandbox/tools.log"
export FAKE_DEV_DIR="$sandbox"
: > "$FAKE_FINDMNT_DB"
: > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_MOUNT_LOG"
: > "$FAKE_UMOUNT_LOG"
: > "$FAKE_SYSTEMCTL_LOG"
: > "$FAKE_TOOL_LOG"

# Generated harness: sources the helper without main and replaces only the
# mount/topology plumbing.  The helper hardens PATH at startup, so the mock
# tools are injected with a DEBUG trap that re-prepends the sandbox after
# every PATH assignment.  mount_target_boot_entry() records helper-owned
# mounts the way the real function does so filesystem_release_all_mounts()
# can be exercised.
cat > "$sandbox/harness.sh" <<HARNESS
#!/usr/bin/env bash
set -euo pipefail
source <(sed '/^main "\$@"/d' "$HELPER")
trap - EXIT INT TERM HUP
trap 'PATH="$sandbox/mockbin:\$PATH"; export PATH' DEBUG
# The helper defaults SESSION_LOG to ""; harness blocks that skip prepare_target
# would otherwise tee into the empty path (noise).  Default it once so every
# transcript write lands in the sandbox session log.
: "\${SESSION_LOG:=$sandbox/session.log}"
: > "\$SESSION_LOG"
is_block_device() { [[ "\$1" == "$sandbox"/dev-* || "\$1" == /dev/test-* ]]; }
top_disks_for() { printf '%s\n' "$sandbox/dev-test-disk"; }
mktemp() { printf '%s\n' "$sandbox/session.mount"; }
prepare_target() {
    TARGET_ROOT="$sandbox/target"
    SESSION_LOG="$sandbox/session.log"
    : > "\$SESSION_LOG"
}
# Keep the real entry function available for the ESP host-path join contract;
# the simulation below replaces it for the filesystem-scope tests.
real_mount_target_boot_entry="\$(declare -f mount_target_boot_entry)"
mount_target_boot_entry() {
    local mp="\${1:-}"
    [[ -n "\$mp" ]] || return 0
    [[ -e "$sandbox/target\$mp" ]] || return 0
    printf '%s\n' "$sandbox/target\$mp" >> "\$FAKE_MOUNTPOINT_DB"
    MOUNTS+=("$sandbox/target\$mp")
}
prepare_running_host() {
    SESSION_LOG="$sandbox/session.log"
    : > "\$SESSION_LOG"
}
# The target-scope snapshot-rollback capability probe resolves the top-level
# Btrfs filesystem through this stub; the fixture under $sandbox/snapshot-top
# supplies the @ root and the @.snapshots/.snapshots store.
mount_snapshot_top() {
    SNAPSHOT_TOP="$sandbox/snapshot-top"
    mkdir -p "\$SNAPSHOT_TOP"
    return 0
}
snapshot_free_space_ok() { return 0; }
eval "\${HARNESS_CODE:?missing HARNESS_CODE}"
HARNESS

run_harness() {
    PATH="$sandbox/mockbin:$PATH" HARNESS_CODE="$1" BOOT_REPAIR_STATE_ROOT="$sandbox/state" \
        bash --noprofile --norc "$sandbox/harness.sh"
}

# The canonical target-scope repair invocation.
repair_code() {
    printf '\nTARGET_DISK=/dev/test-disk\nROOT_DEVICE=/dev/test-root\nTARGET_OS_ID=arch\nTARGET_OS_LIKE=""\nTARGET_DISTRO_FAMILY=arch\nTARGET_ROOT=""\nfs_repair /dev/test-disk /dev/test-root %s %s\n' "$1" "$2"
}

expect_repair_line() {
    local device="$1" mode="$2" result="$3" output="$4"
    grep -Eq "^File system repair ${device}: [a-z0-9_]+ uuid=[^ ]+ mount=[^ ]+ tool=[^ ]+ mode=${mode} result=${result}$" <<<"$output" \
        || { echo "FAIL: missing ${device} ${mode} result=${result} evidence line" >&2; printf '%s\n' "$output" >&2; exit 1; }
}

set_home_fstype() {
    sed -i "s/^test-home .*/test-home $1 part BBBBBBBB-CCCC-DDDD-EEEE-FFFFFFFFFFFF test-disk 1 test-disk/" "$FAKE_LSBLK_DB"
}

# ---------------------------------------------------------------------------
# Part 0: the helper mode matrix and the GUI mode list cannot drift.  The
# helper matrix is the support truth; the GUI literals are asserted verbatim
# and the UI test independently checks the returned lists.
# ---------------------------------------------------------------------------
mode_matrix="$(run_harness 'filesystem_mode_matrix')"
expected_matrix="$(cat <<'MATRIX'
ext2 repair yes no
ext2 check yes no
ext3 repair yes no
ext3 check yes no
ext4 repair yes no
ext4 check yes no
xfs repair yes no
xfs check yes no
btrfs repair yes no
btrfs rescue yes no
btrfs scrub no yes
btrfs check yes no
fat repair yes no
fat check yes no
exfat repair yes no
exfat check yes no
ntfs repair yes no
ntfs check yes no
f2fs repair yes no
jfs repair yes no
jfs check yes no
reiserfs repair yes no
reiserfs check yes no
zfs scrub no yes
zfs check no yes
MATRIX
)"
if [[ "$mode_matrix" != "$expected_matrix" ]]; then
    echo 'FAIL: the helper filesystem mode matrix changed' >&2
    diff <(printf '%s\n' "$expected_matrix") <(printf '%s\n' "$mode_matrix") >&2 || true
    exit 1
fi

ui_modes="$(awk '/^QStringList MainWindow::filesystemRepairModes/,/^}/' "$UI_SOURCE" | tr -d '[:space:]')"
[[ -n "$ui_modes" ]] || { echo 'FAIL: MainWindow::filesystemRepairModes was not found' >&2; exit 1; }
for fragment in \
    'fs==QStringLiteral("f2fs")' \
    'return{QStringLiteral("repair"),QStringLiteral("rescue"),QStringLiteral("scrub"),QStringLiteral("check")};' \
    'return{QStringLiteral("scrub"),QStringLiteral("check")};' \
    'return{QStringLiteral("check")};' \
    'return{QStringLiteral("repair"),QStringLiteral("check")};' \
    'QStringList{}:QStringList{QStringLiteral("repair")}'; do
    grep -Fq "$fragment" <<<"$ui_modes" \
        || { echo "FAIL: the GUI mode list is missing: $fragment" >&2; exit 1; }
done

# ---------------------------------------------------------------------------
# Part 1: capability gating.  An unresolved scope is unavailable with a clear
# reason; the existing keys and their evidence are unchanged.
# ---------------------------------------------------------------------------
cap_unresolved="$(run_harness '
TARGET_ROOT=""
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_INITRAMFS_BACKEND=mkinitcpio
TARGET_BOOTLOADER_BACKEND=grub
diagnostic_repair_capabilities
')"
grep -Fqx "Repair tool filesystem: unavailable|reason:no-resolvable-scope-filesystems" <<<"$cap_unresolved" \
    || { echo "FAIL: missing filesystem unavailable capability line" >&2; printf '%s\n' "$cap_unresolved" >&2; exit 1; }
grep -Fqx 'Repair capability evidence filesystem: scope filesystems unresolved' <<<"$cap_unresolved" \
    || { echo "FAIL: missing unresolved filesystem evidence" >&2; exit 1; }
grep -Fqx 'Repair tool validate: available' <<<"$cap_unresolved" \
    || { echo "FAIL: existing capability keys changed" >&2; exit 1; }
for cap_key in dpkg fixbroken aptupdate upgrade dkms display gpu initramfs efi grub extlinux bootstack kernels; do
    grep -Eq "^Repair tool $cap_key: " <<<"$cap_unresolved" \
        || { echo "FAIL: existing capability key missing: $cap_key" >&2; exit 1; }
done
[[ "$(grep -c '^Repair tool ' <<<"$cap_unresolved")" -eq 15 ]] \
    || { echo "FAIL: expected 15 capability keys" >&2; exit 1; }
# Running-host maintenance adds host-scope capability evidence but must never
# become a `Repair tool` key; target scope never emits the host-only lines
# (the storage-stack markers are scope-neutral and appear in both scopes).
host_caps="$(run_harness '
RUNNING_HOST_MODE=1
TARGET_ROOT=""
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_INITRAMFS_BACKEND=mkinitcpio
TARGET_BOOTLOADER_BACKEND=grub
diagnostic_repair_capabilities
')"
[[ "$(grep -c '^Repair tool ' <<<"$host_caps")" -eq 15 ]] \
    || { echo "FAIL: host scope changed the 15 Repair tool keys" >&2; printf '%s\n' "$host_caps" >&2; exit 1; }
grep -Eq '^Host snapshot rollback: (available|unavailable\|)' <<<"$host_caps" \
    || { echo "FAIL: host snapshot rollback capability line is missing" >&2; exit 1; }
grep -Eq '^Host reboot: (available|unavailable\|)' <<<"$host_caps" \
    || { echo "FAIL: host reboot capability line is missing" >&2; exit 1; }
grep -Eq '^Storage lvm inspection: (available|unavailable\|)' <<<"$host_caps" \
    || { echo "FAIL: storage lvm inspection capability line is missing" >&2; exit 1; }
grep -Eq '^Storage software raid: (available|unavailable\|)' <<<"$host_caps" \
    || { echo "FAIL: storage software raid capability line is missing" >&2; exit 1; }
# The storage-stack markers are scope-neutral: the repair-target scope emits
# them too (they are not host-only `Host ...` lines).
grep -Eq '^Storage lvm inspection: (available|unavailable\|)' <<<"$cap_unresolved" \
    || { echo "FAIL: target-scope storage lvm inspection capability line is missing" >&2; exit 1; }
grep -Eq '^Storage software raid: (available|unavailable\|)' <<<"$cap_unresolved" \
    || { echo "FAIL: target-scope storage software raid capability line is missing" >&2; exit 1; }
if grep -q '^Host ' <<<"$cap_unresolved"; then
    echo 'FAIL: target-scope diagnostics must not emit host capability lines' >&2
    exit 1
fi
# Snapshot rollback is scope-neutral: the repair-target scope surfaces the
# `Snapshot rollback:` line (not the host-only `Host snapshot rollback:` line),
# and the host scope keeps its name-preserving host line.  The unresolved
# target has no Btrfs root, so the line reports the precise reason.
grep -Eq '^Snapshot rollback: (available|unavailable\|)' <<<"$cap_unresolved" \
    || { echo "FAIL: target-scope snapshot rollback capability line is missing" >&2; grep '^Snapshot rollback' <<<"$cap_unresolved" >&2; exit 1; }
grep -Fqx 'Snapshot rollback: unavailable|reason:not-btrfs-root|param:unknown' <<<"$cap_unresolved" \
    || { echo "FAIL: unresolved target snapshot rollback reason is not not-btrfs-root" >&2; grep '^Snapshot rollback' <<<"$cap_unresolved" >&2; exit 1; }
if grep -q '^Snapshot rollback' <<<"$host_caps"; then
    echo 'FAIL: host-scope diagnostics must not emit the target snapshot rollback line' >&2
    exit 1
fi

# A non-Btrfs target root names the same precise scope-neutral reason.
snap_nonbtrfs="$(run_harness '
ROOT_CANONICAL=/dev/test-root
ROOT_DEVICE=/dev/test-root
TARGET_ROOT=""
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_INITRAMFS_BACKEND=mkinitcpio
TARGET_BOOTLOADER_BACKEND=grub
diagnostic_repair_capabilities
')"
grep -Fqx 'Snapshot rollback: unavailable|reason:not-btrfs-root|param:ext4' <<<"$snap_nonbtrfs" \
    || { echo "FAIL: non-Btrfs target snapshot rollback reason is not not-btrfs-root(ext4)" >&2; grep '^Snapshot rollback' <<<"$snap_nonbtrfs" >&2; exit 1; }

# A Btrfs target with a resolvable snapshot store and a normal top-level @ root
# reports snapshot rollback available in the repair-target scope.
printf 'test-btrfs btrfs part 22222222-3333-4444-5555-666666666666 test-disk 2 test-disk\n' >> "$FAKE_LSBLK_DB"
mkdir -p "$sandbox/snapshot-top/@/etc" "$sandbox/snapshot-top/@.snapshots"
printf 'ID=test\nPRETTY_NAME="Contract Test"\n' > "$sandbox/snapshot-top/@/etc/os-release"
snap_avail="$(run_harness '
ROOT_CANONICAL=/dev/test-btrfs
ROOT_DEVICE=/dev/test-btrfs
TARGET_ROOT=""
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_INITRAMFS_BACKEND=mkinitcpio
TARGET_BOOTLOADER_BACKEND=grub
diagnostic_repair_capabilities
')"
grep -Fqx 'Snapshot rollback: available' <<<"$snap_avail" \
    || { echo "FAIL: Btrfs target with snapshot store did not report snapshot rollback available" >&2; grep '^Snapshot rollback' <<<"$snap_avail" >&2; exit 1; }
grep -Eq '^Snapshot rollback evidence: btrfs root /dev/test-btrfs; top-level @\.snapshots' <<<"$snap_avail" \
    || { echo "FAIL: Btrfs target snapshot rollback evidence does not name the store" >&2; grep '^Snapshot rollback evidence' <<<"$snap_avail" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 2: fs-inspect resolves the target scope read-only and reports evidence.
# Helper-owned /boot and /boot/efi mounts are released before the checks so
# offline-only tools can run; the mount simulation proves that release.
# ---------------------------------------------------------------------------
cat > "$sandbox/target/etc/fstab" <<'FSTAB'
/dev/test-root  /          ext4  defaults  0 1
/dev/test-boot  /boot      ext4  defaults  0 2
/dev/test-efi   /boot/efi  vfat  defaults  0 2
/dev/test-home  /home      xfs   defaults  0 2
FSTAB

inspect_output="$(run_harness '
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT=""
fs_inspect /dev/test-disk /dev/test-root
')"
grep -Fqx "File system check /dev/test-root: ext4 uuid=11111111-2222-3333-4444-555555555555 mount=unmounted tool=e2fsck result=clean" <<<"$inspect_output" \
    || { echo "FAIL: missing root fs-inspect evidence line" >&2; printf '%s\n' "$inspect_output" >&2; exit 1; }
grep -Fqx "File system check /dev/test-boot: ext4 uuid=66666666-7777-8888-9999-aaaaaaaaaaaa mount=unmounted tool=e2fsck result=clean" <<<"$inspect_output" \
    || { echo "FAIL: missing /boot fs-inspect evidence line" >&2; printf '%s\n' "$inspect_output" >&2; exit 1; }
grep -Fqx "File system check /dev/test-efi: vfat uuid=ABCD-1234 mount=unmounted tool=fsck.fat result=clean" <<<"$inspect_output" \
    || { echo "FAIL: missing ESP fs-inspect evidence line" >&2; printf '%s\n' "$inspect_output" >&2; exit 1; }
grep -Fqx "File system check /dev/test-home: xfs uuid=BBBBBBBB-CCCC-DDDD-EEEE-FFFFFFFFFFFF mount=unmounted tool=xfs_repair result=clean" <<<"$inspect_output" \
    || { echo "FAIL: missing /home fs-inspect evidence line" >&2; printf '%s\n' "$inspect_output" >&2; exit 1; }
grep -Fq 'File system check summary: devices=4 clean=4 issues=0 unsupported=0 tool-missing=0 skipped=0' <<<"$inspect_output" \
    || { echo "FAIL: missing fs-inspect summary line" >&2; printf '%s\n' "$inspect_output" >&2; exit 1; }
grep -Fq "File system check detail /dev/test-root: tool=e2fsck output=clean" <<<"$inspect_output" \
    || { echo "FAIL: missing fs-inspect detail line" >&2; exit 1; }
grep -Fq -- "-f -n /dev/test-root" "$FAKE_TOOL_LOG" \
    || { echo "FAIL: e2fsck read-only preflight was not invoked" >&2; exit 1; }
if grep -Eq -- ' -[yp]( |$)' "$FAKE_TOOL_LOG"; then
    echo 'FAIL: fs-inspect invoked a repairing mode' >&2
    exit 1
fi

# An issue result is reported with its captured output.
: > "$FAKE_TOOL_LOG"
inspect_issues="$(FAKE_TOOL_FAIL=e2fsck run_harness '
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT=""
fs_inspect /dev/test-disk /dev/test-root
')"
grep -Fqx "File system check /dev/test-root: ext4 uuid=11111111-2222-3333-4444-555555555555 mount=unmounted tool=e2fsck result=issues" <<<"$inspect_issues" \
    || { echo "FAIL: e2fsck issue result was not reported" >&2; printf '%s\n' "$inspect_issues" >&2; exit 1; }
grep -Fq "File system check detail /dev/test-root: tool=e2fsck output=Filesystem is not clean; errors found" <<<"$inspect_issues" \
    || { echo "FAIL: e2fsck issue detail was not captured" >&2; exit 1; }
grep -Fq 'File system check summary: devices=4 clean=2 issues=2 unsupported=0 tool-missing=0 skipped=0' <<<"$inspect_issues" \
    || { echo "FAIL: issue summary mismatch" >&2; printf '%s\n' "$inspect_issues" >&2; exit 1; }

# An unsupported filesystem is reported, never guessed about.
printf '/dev/test-data  /efi  reiserfs  defaults  0 2\n' >> "$sandbox/target/etc/fstab"
sed -i 's/^test-data reiserfs.*/test-data zzzfs part 9999 test-disk 1 test-disk/' "$FAKE_LSBLK_DB"
inspect_unsupported="$(run_harness '
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT=""
fs_inspect /dev/test-disk /dev/test-root
')"
grep -Fqx "File system check /dev/test-data: zzzfs uuid=9999 mount=unmounted tool=none result=unsupported" <<<"$inspect_unsupported" \
    || { echo "FAIL: unsupported filesystem was not reported" >&2; printf '%s\n' "$inspect_unsupported" >&2; exit 1; }
grep -Fq 'File system check summary: devices=5 clean=4 issues=0 unsupported=1' <<<"$inspect_unsupported" \
    || { echo "FAIL: unsupported filesystem summary mismatch" >&2; exit 1; }
sed -i '\|/dev/test-data|d' "$sandbox/target/etc/fstab"
sed -i 's/^test-data zzzfs.*/test-data reiserfs part 9999 test-disk 1 test-disk/' "$FAKE_LSBLK_DB"

# A resolved capability probe names the scope and its tools.
cap_resolved="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_INITRAMFS_BACKEND=mkinitcpio
TARGET_BOOTLOADER_BACKEND=grub
ROOT_DEVICE=/dev/test-root
diagnostic_repair_capabilities
')"
grep -Fqx 'Repair tool filesystem: available' <<<"$cap_resolved" \
    || { echo "FAIL: resolved scope did not advertise filesystem repair" >&2; printf '%s\n' "$cap_resolved" >&2; exit 1; }
grep -Eq '^Repair capability evidence filesystem: scope filesystems resolved; tools: (e2fsck\(ext4\)|fsck\.fat\(vfat\)|xfs_repair\(xfs\))' <<<"$cap_resolved" \
    || { echo "FAIL: filesystem capability evidence does not name tools" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 3: fs-repair preflights.  A device outside the scope is refused, an
# offline tool on a mounted device is refused, and a valid unmounted repair
# runs the filesystem-specific command.
# ---------------------------------------------------------------------------
if out_of_scope="$(run_harness "$(repair_code /dev/test-outside repair)" 2>&1)"; then
    echo 'FAIL: fs-repair accepted a device outside the scope' >&2
    exit 1
fi
grep -Fq 'is not part of the resolved scope' <<<"$out_of_scope" \
    || { echo 'FAIL: out-of-scope refusal reason missing' >&2; printf '%s\n' "$out_of_scope" >&2; exit 1; }

: > "$FAKE_MOUNT_LOG"
: > "$FAKE_TOOL_LOG"
if mounted_refusal="$(run_harness "$(repair_code /dev/test-boot repair)" 2>&1)"; then
    echo 'FAIL: fs-repair accepted an offline repair on a mounted filesystem' >&2
    exit 1
fi
grep -Fq "Offline file system repair refuses mounted filesystem /dev/test-boot (mounted at /boot)" <<<"$mounted_refusal" \
    || { echo 'FAIL: mounted offline refusal reason missing' >&2; printf '%s\n' "$mounted_refusal" >&2; exit 1; }
if grep -Eq -- ' -[yp]( |$)' "$FAKE_TOOL_LOG"; then
    echo 'FAIL: a mounted filesystem repair command was executed' >&2
    exit 1
fi

: > "$FAKE_MOUNT_LOG"
: > "$FAKE_TOOL_LOG"
if unmounted_repair="$(run_harness "$(repair_code /dev/test-home repair)" 2>&1)"; then
    :
else
    echo "FAIL: valid unmounted xfs repair was refused: $unmounted_repair" >&2
    exit 1
fi
grep -Fq "xfs_repair -e /dev/test-home" "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: xfs_repair -e repair command was not invoked' >&2; exit 1; }
expect_repair_line /dev/test-home repair clean "$unmounted_repair"

# An unsupported mode for the filesystem is refused.
if mode_refusal="$(run_harness "$(repair_code /dev/test-home scrub)" 2>&1)"; then
    echo 'FAIL: fs-repair accepted an unsupported mode' >&2
    exit 1
fi
grep -Fq "Repair mode 'scrub' is not supported for filesystem 'xfs'" <<<"$mode_refusal" \
    || { echo 'FAIL: unsupported mode refusal reason missing' >&2; exit 1; }

# A tool that cannot be found is refused with a clear reason.
if missing_refusal="$(run_harness '
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT=""
filesystem_repair_tool() { return 1; }
fs_repair /dev/test-disk /dev/test-root /dev/test-home repair
' 2>&1)"; then
    echo 'FAIL: fs-repair accepted a missing repair tool' >&2
    exit 1
fi
grep -Fq 'is not installed in the recovery environment' <<<"$missing_refusal" \
    || { echo 'FAIL: missing tool refusal reason missing' >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 4: per-filesystem repair classification.  Every exit map is asserted
# against its documented meaning; a nonzero exit must fail the helper and a
# false success (for example fsck.fat 2 or fsck.f2fs 0 with [Fail]) must never
# be reported as clean.
# ---------------------------------------------------------------------------
: > "$FAKE_TOOL_LOG"
e2fsck_fixed="$(FAKE_TOOL_FAIL=e2fsck FAKE_TOOL_FAIL_RC=1 run_harness "$(repair_code /dev/test-root repair)" 2>&1 || true)"
expect_repair_line /dev/test-root repair repaired "$e2fsck_fixed"

e2fsck_reboot="$(FAKE_TOOL_FAIL=e2fsck FAKE_TOOL_FAIL_RC=2 run_harness "$(repair_code /dev/test-root repair)" 2>&1 || true)"
expect_repair_line /dev/test-root repair repaired "$e2fsck_reboot"
grep -Fq 'msg:fs-repair-pass2|param:' <<<"$e2fsck_reboot" \
    || { echo 'FAIL: e2fsck rc 2 did not recommend a reboot' >&2; exit 1; }

# Safe preen first; the forced pass runs only after preen left rc 4.
: > "$FAKE_TOOL_LOG"
preen_fallback="$(FAKE_TOOL_FAIL=e2fsck FAKE_TOOL_MATCH=-p FAKE_TOOL_FAIL_RC=4 FAKE_TOOL_OK_RC=1 run_harness "$(repair_code /dev/test-root repair)" 2>&1 || true)"
expect_repair_line /dev/test-root repair repaired "$preen_fallback"
grep -Fq 'e2fsck -f -p /dev/test-root' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: e2fsck safe preen was not invoked first' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1; }
grep -Fq 'e2fsck -f -y /dev/test-root' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: e2fsck forced fallback was not invoked after preen rc 4' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1; }

if preen_failed="$(FAKE_TOOL_FAIL=e2fsck FAKE_TOOL_FAIL_RC=4 run_harness "$(repair_code /dev/test-root repair)" 2>&1)"; then
    echo 'FAIL: e2fsck rc 4 after the forced pass was treated as success' >&2
    exit 1
fi
expect_repair_line /dev/test-root repair issues "$preen_failed"

# xfs_repair: 2 is a dirty log with nothing repaired and must fail.
if xfs_dirty="$(FAKE_TOOL_FAIL=xfs_repair FAKE_TOOL_FAIL_RC=2 run_harness "$(repair_code /dev/test-home repair)" 2>&1)"; then
    echo 'FAIL: xfs_repair rc 2 (dirty log) was treated as success' >&2
    exit 1
fi
expect_repair_line /dev/test-home repair issues "$xfs_dirty"

if xfs_uncorrected="$(FAKE_TOOL_FAIL=xfs_repair FAKE_TOOL_FAIL_RC=1 run_harness "$(repair_code /dev/test-home repair)" 2>&1)"; then
    echo 'FAIL: xfs_repair rc 1 (uncorrected) was treated as success' >&2
    exit 1
fi
expect_repair_line /dev/test-home repair issues "$xfs_uncorrected"

xfs_repaired="$(FAKE_TOOL_FAIL=xfs_repair FAKE_TOOL_FAIL_RC=4 run_harness "$(repair_code /dev/test-home repair)" 2>&1 || true)"
expect_repair_line /dev/test-home repair repaired "$xfs_repaired"

# fsck.fat: 2 is a usage error and the filesystem was not touched.
set_home_fstype fat
if fat_usage="$(FAKE_TOOL_FAIL=fsck.fat FAKE_TOOL_FAIL_RC=2 run_harness "$(repair_code /dev/test-home repair)" 2>&1)"; then
    echo 'FAIL: fsck.fat rc 2 (usage error) was treated as success' >&2
    exit 1
fi
expect_repair_line /dev/test-home repair issues "$fat_usage"
fat_fixed="$(FAKE_TOOL_FAIL=fsck.fat FAKE_TOOL_FAIL_RC=1 run_harness "$(repair_code /dev/test-home repair)" 2>&1 || true)"
expect_repair_line /dev/test-home repair repaired "$fat_fixed"
set_home_fstype xfs

# ntfsfix: 255 means errors remain and chkdsk is required; 0 is only a dirty
# state clear, which must still surface the chkdsk requirement.
set_home_fstype ntfs
if ntfs_remain="$(FAKE_TOOL_FAIL=ntfsfix FAKE_TOOL_FAIL_RC=255 run_harness "$(repair_code /dev/test-home repair)" 2>&1)"; then
    echo 'FAIL: ntfsfix rc 255 (errors remain) was treated as success' >&2
    exit 1
fi
expect_repair_line /dev/test-home repair issues "$ntfs_remain"
grep -Fq 'msg:ntfsfix-fail|param:' <<<"$ntfs_remain" \
    || { echo 'FAIL: ntfsfix failure did not require chkdsk' >&2; exit 1; }
ntfs_ok="$(run_harness "$(repair_code /dev/test-home repair)" 2>&1 || true)"
expect_repair_line /dev/test-home repair clean "$ntfs_ok"
grep -Fq 'msg:ntfsfix-note' <<<"$ntfs_ok" \
    || { echo 'FAIL: ntfsfix success did not surface the chkdsk requirement' >&2; exit 1; }
set_home_fstype xfs

# fsck.f2fs: the exit status is not trusted; [Fail] output fails even at rc 0.
set_home_fstype f2fs
: > "$FAKE_TOOL_LOG"
f2fs_corrupt="$(FAKE_TOOL_FAIL=fsck.f2fs FAKE_TOOL_FAIL_RC=0 FAKE_TOOL_FAIL_OUTPUT='[FSCK] SIT valid block bitmap checking [Fail]' run_harness "$(repair_code /dev/test-home repair)" 2>&1 || true)"
expect_repair_line /dev/test-home repair issues "$f2fs_corrupt"
f2fs_ok="$(FAKE_TOOL_OK_RC=0 run_harness "$(repair_code /dev/test-home repair)" 2>&1 || true)"
expect_repair_line /dev/test-home repair repaired "$f2fs_ok"
grep -Fq 'fsck.f2fs -f /dev/test-home' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: fsck.f2fs repair invocation missing' >&2; exit 1; }

# f2fs has no read-only check mode: inspect must report it unsupported and
# must never invoke fsck.f2fs -n.
: > "$FAKE_TOOL_LOG"
f2fs_inspect="$(run_harness '
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT=""
fs_inspect /dev/test-disk /dev/test-root
')"
grep -Fqx "File system check /dev/test-home: f2fs uuid=BBBBBBBB-CCCC-DDDD-EEEE-FFFFFFFFFFFF mount=unmounted tool=none result=unsupported" <<<"$f2fs_inspect" \
    || { echo 'FAIL: f2fs was not reported as unsupported for inspection' >&2; printf '%s\n' "$f2fs_inspect" >&2; exit 1; }
if grep -Fq 'fsck.f2fs' "$FAKE_TOOL_LOG"; then
    echo 'FAIL: fsck.f2fs was invoked during inspection' >&2
    exit 1
fi
set_home_fstype xfs

# jfs_fsck and reiserfsck corrected invocations (no -y for jfs; -y for the
# reiserfsck fix-fixable path so a non-tty stdin cannot hang the prompt).
set_home_fstype jfs
: > "$FAKE_TOOL_LOG"
run_harness "$(repair_code /dev/test-home repair)" >/dev/null 2>&1 || true
grep -Fq 'jfs_fsck -f /dev/test-home' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: jfs_fsck must be invoked as -f only' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1; }
set_home_fstype reiserfs
: > "$FAKE_TOOL_LOG"
run_harness "$(repair_code /dev/test-home repair)" >/dev/null 2>&1 || true
grep -Fq 'reiserfsck -y --fix-fixable /dev/test-home' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: reiserfsck must be invoked with -y --fix-fixable' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1; }
set_home_fstype exfat
: > "$FAKE_TOOL_LOG"
run_harness "$(repair_code /dev/test-home repair)" >/dev/null 2>&1 || true
grep -Fq 'fsck.exfat -p /dev/test-home' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: fsck.exfat must prefer -p over -y' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1; }
set_home_fstype xfs

# btrfs repair is a confirmed last resort and must log the warning; scrub uses
# the foreground device-statistics form and maps exit 3 / uncorrectable
# statistics to issues.
set_home_fstype btrfs
: > "$FAKE_TOOL_LOG"
btrfs_warning="$(run_harness "$(repair_code /dev/test-home repair)" 2>&1 || true)"
grep -Fq 'msg:btrfs-repair-warning' <<<"$btrfs_warning" \
    || { echo 'FAIL: btrfs check --repair warning missing' >&2; exit 1; }
grep -Fq 'btrfs check --repair /dev/test-home' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: btrfs repair invocation missing' >&2; exit 1; }
set_home_fstype xfs

sed -i 's/^test-boot ext4.*/test-boot btrfs part 66666666-7777-8888-9999-aaaaaaaaaaaa test-disk 1 test-disk/' "$FAKE_LSBLK_DB"
: > "$FAKE_TOOL_LOG"
if btrfs_scrub_fail="$(FAKE_TOOL_FAIL=btrfs FAKE_TOOL_FAIL_RC=3 run_harness "$(repair_code /dev/test-boot scrub)" 2>&1)"; then
    echo 'FAIL: btrfs scrub rc 3 (uncorrectable) was treated as success' >&2
    exit 1
fi
expect_repair_line /dev/test-boot scrub issues "$btrfs_scrub_fail"
grep -Fq 'btrfs scrub start -B -d /boot' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: btrfs scrub must run in foreground with device stats' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1; }
btrfs_scrub_stats="$(FAKE_TOOL_OK_OUTPUT='Error summary:    read=0  csum=2  verify=0 data=0 tree=0' run_harness "$(repair_code /dev/test-boot scrub)" 2>&1 || true)"
expect_repair_line /dev/test-boot scrub repaired "$btrfs_scrub_stats"
sed -i 's/^test-boot btrfs.*/test-boot ext4 part 66666666-7777-8888-9999-aaaaaaaaaaaa test-disk 1 test-disk/' "$FAKE_LSBLK_DB"

# ---------------------------------------------------------------------------
# Part 4b: evidence-driven change status.  fs-repair emits exactly one stable
# "Repair change status filesystem:" line: a clean result (no errors, no
# changes) reports unchanged while a repaired result reports changed. A failed
# repair emits no line so the GUI fails safe.
# ---------------------------------------------------------------------------
grep -q '^repair_change_status()' "$HELPER"
grep -Fq "printf 'Repair change status %s: %s\\n' \"\$key\" \"\$state\"" "$HELPER"
expect_single_change_status() {
    local output="$1" expected="$2"
    [[ "$(grep -c '^Repair change status filesystem: ' <<<"$output")" -eq 1 ]] \
        || { echo "FAIL: expected exactly one filesystem change status line" >&2; printf '%s\n' "$output" >&2; exit 1; }
    grep -Fqx "Repair change status filesystem: $expected" <<<"$output" \
        || { echo "FAIL: unexpected filesystem change status (expected ${expected})" >&2; printf '%s\n' "$output" >&2; exit 1; }
}

# A repair-mode command may still write filesystem metadata (superblock
# timestamps, scrub statistics) even when it reports no errors, so it stays
# changed; only a read-only check mode proves unchanged.
clean_repair_status="$(run_harness "$(repair_code /dev/test-home repair)" 2>&1 || true)"
expect_repair_line /dev/test-home repair clean "$clean_repair_status"
expect_single_change_status "$clean_repair_status" 'changed'

clean_check_status="$(run_harness "$(repair_code /dev/test-home check)" 2>&1 || true)"
expect_repair_line /dev/test-home check clean "$clean_check_status"
expect_single_change_status "$clean_check_status" 'unchanged|reason:filesystem-check-clean'

repaired_status="$(FAKE_TOOL_FAIL=e2fsck FAKE_TOOL_FAIL_RC=1 run_harness "$(repair_code /dev/test-root repair)" 2>&1 || true)"
expect_repair_line /dev/test-root repair repaired "$repaired_status"
expect_single_change_status "$repaired_status" 'changed'

failed_status="$(FAKE_TOOL_FAIL=e2fsck FAKE_TOOL_FAIL_RC=4 run_harness "$(repair_code /dev/test-root repair)" 2>&1 || true)"
if grep -q '^Repair change status filesystem: ' <<<"$failed_status"; then
    echo 'FAIL: a failed fs-repair must emit no change status (GUI fails safe)' >&2
    printf '%s\n' "$failed_status" >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# Part 5: running-host scope.  The mounted host root refuses an offline
# repair; the read-only inspection skips offline-only tools on live mounts and
# reports the skipped counter instead of inventing a clean result.
# ---------------------------------------------------------------------------
: > "$FAKE_FINDMNT_DB"
printf '/ /dev/test-host-root ext4\n' >> "$FAKE_FINDMNT_DB"
printf '/boot /dev/test-host-boot ext4\n' >> "$FAKE_FINDMNT_DB"
# A systemd automount unit publishes its synthetic autofs source before the
# real ESP, exactly like the running host's findmnt output. The scope resolver
# must skip it and still record the ESP device.
printf '/boot/efi systemd-1 autofs\n' >> "$FAKE_FINDMNT_DB"
printf '/boot/efi /dev/test-host-efi vfat\n' >> "$FAKE_FINDMNT_DB"
printf 'test-host-root ext4 part HOST-ROOT-UUID test-host-disk 1 test-host-disk\n' >> "$FAKE_LSBLK_DB"
printf 'test-host-boot ext4 part HOST-BOOT-UUID test-host-disk 1 test-host-disk\n' >> "$FAKE_LSBLK_DB"
printf 'test-host-efi vfat part HOST-EFI-UUID test-host-disk 1 test-host-disk\n' >> "$FAKE_LSBLK_DB"
: > "$sandbox/dev-test-host-root"
: > "$sandbox/dev-test-host-boot"
: > "$sandbox/dev-test-host-efi"

host_inspect="$(run_harness '
RUNNING_HOST_MODE=1
TARGET_DISK=/dev/test-host-disk
ROOT_DEVICE=/dev/test-host-root
TARGET_ROOT=/
filesystem_scope_resolve
printf "SCOPE:%s\n" "${FS_SCOPE_DEVICES[*]}"
printf "MOUNTS:%s\n" "${FS_SCOPE_MOUNTS[*]}"
')"
grep -Fq "SCOPE:/dev/test-host-root /dev/test-host-boot /dev/test-host-efi" <<<"$host_inspect" \
    || { echo 'FAIL: running-host scope devices were not resolved' >&2; printf '%s\n' "$host_inspect" >&2; exit 1; }
grep -Fq 'MOUNTS:/ /boot /boot/efi' <<<"$host_inspect" \
    || { echo 'FAIL: running-host mountpoints were not resolved' >&2; printf '%s\n' "$host_inspect" >&2; exit 1; }
if grep -Fq 'systemd-1' <<<"$host_inspect"; then
    echo 'FAIL: the synthetic automount source leaked into the running-host scope' >&2
    exit 1
fi

if host_offline="$(run_harness '
RUNNING_HOST_MODE=1
TARGET_DISK=/dev/test-host-disk
ROOT_DEVICE=/dev/test-host-root
TARGET_ROOT=/
fs_repair /dev/test-host-disk /dev/test-host-root /dev/test-host-root repair
' 2>&1)"; then
    echo 'FAIL: running-host root accepted an offline repair' >&2
    exit 1
fi
grep -Fq "Offline file system repair refuses mounted filesystem /dev/test-host-root" <<<"$host_offline" \
    || { echo 'FAIL: running-host offline refusal reason missing' >&2; exit 1; }

host_skip_inspect="$(run_harness '
RUNNING_HOST_MODE=1
TARGET_DISK=/dev/test-host-disk
ROOT_DEVICE=/dev/test-host-root
TARGET_ROOT=/
fs_inspect /dev/test-host-disk /dev/test-host-root
')"
grep -Fqx "File system check /dev/test-host-root: ext4 uuid=HOST-ROOT-UUID mount=/ tool=e2fsck result=skipped" <<<"$host_skip_inspect" \
    || { echo 'FAIL: mounted host root was not skipped by the offline-only check' >&2; printf '%s\n' "$host_skip_inspect" >&2; exit 1; }
grep -Fq 'File system check summary: devices=3 clean=0 issues=0 unsupported=0 tool-missing=0 skipped=3' <<<"$host_skip_inspect" \
    || { echo 'FAIL: mounted host skip summary mismatch' >&2; printf '%s\n' "$host_skip_inspect" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 7: fail-closed capability gating and offline-repair mount release.
# ---------------------------------------------------------------------------
printf 'test-disk - disk - - - -\n' > "$FAKE_LSBLK_DB"
printf 'test-root ext4 part 11111111-2222-3333-4444-555555555555 test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
printf 'test-boot ext4 part 66666666-7777-8888-9999-aaaaaaaaaaaa test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
printf 'test-efi vfat part ABCD-1234 test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
printf 'test-home xfs part BBBBBBBB-CCCC-DDDD-EEEE-FFFFFFFFFFFF test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
printf 'test-data2 ext4 part DATA2-UUID test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
: > "$sandbox/dev-test-data2"

# An unresolved scope must not abort the read-only inspection.
unresolved_inspect="$(run_harness '
prepare_target() { TARGET_ROOT=""; SESSION_LOG="'"$sandbox"'/session.log"; : > "$SESSION_LOG"; }
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=""
fs_inspect /dev/test-disk /dev/test-root
' 2>&1 || true)"
grep -Fq 'File system check summary: devices=0 clean=0 issues=0 unsupported=0 tool-missing=0 skipped=0' <<<"$unresolved_inspect" \
    || { echo 'FAIL: unresolved scope did not report an empty summary' >&2; printf '%s\n' "$unresolved_inspect" >&2; exit 1; }

# A supported filesystem with no installed check tool fails closed.
cap_notools="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_INITRAMFS_BACKEND=mkinitcpio
TARGET_BOOTLOADER_BACKEND=grub
ROOT_DEVICE=/dev/test-root
filesystem_repair_tool() { return 1; }
diagnostic_repair_capabilities
')"
grep -Fqx 'Repair tool filesystem: unavailable|reason:no-filesystem-check-tool' <<<"$cap_notools" \
    || { echo 'FAIL: no-tools filesystem capability did not fail closed' >&2; grep '^Repair tool filesystem' <<<"$cap_notools" >&2; exit 1; }

# A scope with no supported filesystem type fails closed with its own reason.
cap_unsupported="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_INITRAMFS_BACKEND=mkinitcpio
TARGET_BOOTLOADER_BACKEND=grub
ROOT_DEVICE=/dev/test-root
filesystem_mode_field() { return 1; }
diagnostic_repair_capabilities
')"
grep -Fqx 'Repair tool filesystem: unavailable|reason:no-supported-filesystem-type' <<<"$cap_unsupported" \
    || { echo 'FAIL: unsupported filesystem capability did not fail closed' >&2; grep '^Repair tool filesystem' <<<"$cap_unsupported" >&2; exit 1; }

# A device on the selected disk but outside the resolved scope is refused even
# when the topology helper reports the disk as the target disk.
same_disk_out="$(run_harness "
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=''
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT=''
top_disks_for() { printf '%s\n' /dev/test-disk; }
fs_repair /dev/test-disk /dev/test-root /dev/test-data2 repair
" 2>&1 || true)"
grep -Fq 'is not part of the resolved scope' <<<"$same_disk_out" \
    || { echo 'FAIL: same-disk out-of-scope device was not refused' >&2; printf '%s\n' "$same_disk_out" >&2; exit 1; }

# A helper-owned read-only mount is released before an offline repair instead
# of being mistaken for a live mount.
: > "$FAKE_FINDMNT_DB"
: > "$FAKE_MOUNTPOINT_DB"
printf '%s /dev/test-home xfs\n' "$sandbox/target/home" > "$FAKE_FINDMNT_DB"
printf '%s\n' "$sandbox/target/home" > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_TOOL_LOG"
release_repair="$(run_harness '
prepare_target() {
    TARGET_ROOT="'"$sandbox"'/target"
    SESSION_LOG="'"$sandbox"'/session.log"
    : > "$SESSION_LOG"
    MOUNT_BASE="'"$sandbox"'/target"
    MOUNTS=("'"$sandbox"'/target/home")
}
mount_target_boot_entry() { :; }
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT=""
fs_repair /dev/test-disk /dev/test-root /dev/test-home repair
' 2>&1 || true)"
expect_repair_line /dev/test-home repair clean "$release_repair"
grep -Fq 'xfs_repair -e /dev/test-home' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: offline repair tool was not invoked after mount release' >&2; exit 1; }

# Every inspection command runs under the helper's timeout pattern.
: > "$FAKE_TOOL_LOG"
timeout_inspect="$(run_harness '
timeout() { printf "TIMEOUT[%s %s] " "$1" "$2" >> "'"$FAKE_TOOL_LOG"'"; shift 2; "$@"; }
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT=""
BOOT_REPAIR_FS_TOOL_TIMEOUT=123
fs_inspect /dev/test-disk /dev/test-root
' 2>&1 || true)"
grep -Fq 'TIMEOUT[--foreground 123]' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: inspection commands were not wrapped in the timeout' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1; }
grep -Fq "File system check summary:" <<<"$timeout_inspect" \
    || { echo 'FAIL: timeout-wrapped inspection did not complete' >&2; printf '%s\n' "$timeout_inspect" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 8: a combined Run All report emits the shared capability preamble once,
# before the first diagnostic section, while every section still runs and no
# capability key line is lost (the filesystem key must survive the dedupe).
# ---------------------------------------------------------------------------
combined_report="$(run_harness '
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_INITRAMFS_BACKEND=mkinitcpio
TARGET_BOOTLOADER_BACKEND=grub
run_target_diagnostic all
' 2>/dev/null || true)"
[[ "$(grep -c '^Repair capability probes (read-only, selected target):$' <<<"$combined_report")" -eq 1 ]] \
    || { echo 'FAIL: combined report emitted duplicate capability preambles' >&2; exit 1; }
[[ "$(grep -c '^Repair capability evidence (read-only):$' <<<"$combined_report")" -eq 1 ]] \
    || { echo 'FAIL: combined report emitted duplicate capability evidence headers' >&2; exit 1; }
[[ "$(grep -c '^Repair tool ' <<<"$combined_report")" -eq 15 ]] \
    || { echo 'FAIL: combined report lost capability key lines' >&2; exit 1; }
[[ "$(grep -c '^Repair capability evidence [a-z]*: ' <<<"$combined_report")" -eq 15 ]] \
    || { echo 'FAIL: combined report lost capability evidence lines' >&2; exit 1; }
[[ "$(grep -c '^Diagnostic: ' <<<"$combined_report")" -eq 17 ]] \
    || { echo 'FAIL: combined report did not run every diagnostic section' >&2; exit 1; }
grep -Fqx 'Repair tool filesystem: available' <<<"$combined_report" \
    || { echo 'FAIL: combined report lost the filesystem capability key line' >&2; exit 1; }
# The combined report embeds the same read-only File systems body the
# individual fs-inspect command produces, so the section is cached on scope
# entry/target selection like every other section.
grep -Fqx 'Diagnostic: filesystem' <<<"$combined_report" \
    || { echo 'FAIL: combined report lost the File systems diagnostic section' >&2; exit 1; }
grep -Fq 'File system check summary:' <<<"$combined_report" \
    || { echo 'FAIL: combined report File systems section did not run the read-only check' >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 9: target root-component confirmation fallback. When the committed
# component does not expose /etc/os-release, the helper probes the other
# Linux-capable partitions of the same disk, largest first, and adopts the
# first with os-release evidence; without a qualifying candidate it fails
# with every candidate named.
# ---------------------------------------------------------------------------
grep -q '^is_linux_root_fstype()' "$HELPER"
grep -q '^target_root_candidates()' "$HELPER"
grep -q '^component_has_os_release()' "$HELPER"
grep -q '^resolve_target_root_component()' "$HELPER"
grep -Fq 'lsblk -P -b -p -o NAME,FSTYPE,SIZE,TYPE,LABEL,PARTLABEL' "$HELPER"
grep -q 'resolve_target_root_component "\$TARGET_DISK" "\$ROOT_CANONICAL"' "$HELPER"
grep -Fq 'msg_log root-component-fallback' "$HELPER"
grep -Fq 'does not contain /etc/os-release and no other Linux-capable partition' "$HELPER"

# The Linux-capable type set mirrors the GUI's preferredRepairNode() rule and
# never admits swap, ESP vfat, LUKS, ntfs/exfat or an empty type.
fstype_probe="$(run_harness '
for fstype in ext2 ext3 ext4 xfs btrfs f2fs; do
    is_linux_root_fstype "$fstype" || { echo "REJECTED:$fstype"; exit 1; }
done
for fstype in swap vfat fat fat32 crypto_LUKS ntfs exfat ""; do
    if is_linux_root_fstype "$fstype"; then echo "ACCEPTED:${fstype:-empty}"; exit 1; fi
done
echo FSTYPE_SET_OK
')"
grep -Fqx 'FSTYPE_SET_OK' <<<"$fstype_probe" \
    || { echo 'FAIL: the Linux-capable filesystem type set changed' >&2; printf '%s\n' "$fstype_probe" >&2; exit 1; }

# Candidate enumeration: the disk itself, swap, ESP vfat, LUKS and mounted
# partitions are excluded; equal-size candidates keep a deterministic
# root-label tie-break. The fake lsblk answers the -P record query and the
# per-device MOUNTPOINTS query.
candidates="$(run_harness '
lsblk() {
    if [[ "$*" == *MOUNTPOINTS* ]]; then
        case "${!#}" in
            /dev/test-disk6) printf "/mnt/data\n" ;;
            *) printf "\n" ;;
        esac
        return 0
    fi
    printf "%s\n" \
        "NAME=\"/dev/test-disk\" FSTYPE=\"\" SIZE=\"21474836480\" TYPE=\"disk\" LABEL=\"\" PARTLABEL=\"\"" \
        "NAME=\"/dev/test-disk1\" FSTYPE=\"ext4\" SIZE=\"314572800\" TYPE=\"part\" LABEL=\"\" PARTLABEL=\"\"" \
        "NAME=\"/dev/test-disk2\" FSTYPE=\"swap\" SIZE=\"4134535168\" TYPE=\"part\" LABEL=\"\" PARTLABEL=\"\"" \
        "NAME=\"/dev/test-disk3\" FSTYPE=\"ext4\" SIZE=\"17024679936\" TYPE=\"part\" LABEL=\"root\" PARTLABEL=\"\"" \
        "NAME=\"/dev/test-disk4\" FSTYPE=\"vfat\" SIZE=\"536870912\" TYPE=\"part\" LABEL=\"EFI\" PARTLABEL=\"\"" \
        "NAME=\"/dev/test-disk5\" FSTYPE=\"crypto_LUKS\" SIZE=\"8589934592\" TYPE=\"part\" LABEL=\"\" PARTLABEL=\"\"" \
        "NAME=\"/dev/test-disk6\" FSTYPE=\"ext4\" SIZE=\"1073741824\" TYPE=\"part\" LABEL=\"data\" PARTLABEL=\"\"" \
        "NAME=\"/dev/test-disk7\" FSTYPE=\"xfs\" SIZE=\"314572800\" TYPE=\"part\" LABEL=\"boot\" PARTLABEL=\"\""
}
target_root_candidates /dev/test-disk /dev/test-disk1
')"
expected_candidates="$(printf '17024679936\t2\t/dev/test-disk3\text4\n314572800\t-1\t/dev/test-disk7\txfs')"
if [[ "$candidates" != "$expected_candidates" ]]; then
    echo 'FAIL: target root candidate enumeration changed' >&2
    diff <(printf '%s\n' "$expected_candidates") <(printf '%s\n' "$candidates") >&2 || true
    exit 1
fi

# Largest first, first with os-release evidence wins; the selected component
# is excluded and the probe order is recorded.
probe_log="$sandbox/fallback-probes.txt"
: > "$probe_log"
resolved="$(run_harness '
lsblk() {
    if [[ "$*" == *MOUNTPOINTS* ]]; then
        printf "\n"
        return 0
    fi
    printf "%s\n" \
        "NAME=\"/dev/test-disk1\" FSTYPE=\"ext4\" SIZE=\"314572800\" TYPE=\"part\" LABEL=\"\" PARTLABEL=\"\"" \
        "NAME=\"/dev/test-disk3\" FSTYPE=\"ext4\" SIZE=\"17024679936\" TYPE=\"part\" LABEL=\"root\" PARTLABEL=\"\"" \
        "NAME=\"/dev/test-disk7\" FSTYPE=\"xfs\" SIZE=\"314572800\" TYPE=\"part\" LABEL=\"boot\" PARTLABEL=\"\""
}
component_has_os_release() {
    printf "%s\n" "$1" >> "'"$probe_log"'"
    [[ "$1" == /dev/test-disk7 ]]
}
resolve_target_root_component /dev/test-disk /dev/test-disk1
')"
[[ "$resolved" == /dev/test-disk7 ]] \
    || { echo "FAIL: fallback did not select the first candidate with os-release evidence: $resolved" >&2; exit 1; }
[[ "$(cat "$probe_log")" == $'/dev/test-disk3\n/dev/test-disk7' ]] \
    || { echo 'FAIL: fallback did not probe largest first' >&2; cat "$probe_log" >&2; exit 1; }

# No candidate qualifies: the resolver fails and names every candidate so the
# refusal lists what was probed.
if no_candidate="$(run_harness '
lsblk() {
    if [[ "$*" == *MOUNTPOINTS* ]]; then
        printf "\n"
        return 0
    fi
    printf "%s\n" \
        "NAME=\"/dev/test-disk3\" FSTYPE=\"ext4\" SIZE=\"17024679936\" TYPE=\"part\" LABEL=\"root\" PARTLABEL=\"\"" \
        "NAME=\"/dev/test-disk7\" FSTYPE=\"xfs\" SIZE=\"314572800\" TYPE=\"part\" LABEL=\"boot\" PARTLABEL=\"\""
}
component_has_os_release() { return 1; }
resolve_target_root_component /dev/test-disk /dev/test-disk1
' 2>&1)"; then
    echo 'FAIL: the fallback resolver accepted a disk with no qualifying candidate' >&2
    exit 1
fi
grep -Fq '/dev/test-disk3 (ext4), /dev/test-disk7 (xfs)' <<<"$no_candidate" \
    || { echo 'FAIL: the no-candidate failure does not list the probed candidates' >&2; printf '%s\n' "$no_candidate" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 10: a non-Btrfs target reports the snapshot inventory as informational
# (exit code 0 with a stable marker), never as a failed snapshot operation.
# Inspect/plan/rollback stay explicit errors because they name a concrete
# Btrfs operation.
# ---------------------------------------------------------------------------
grep -Fq 'SNAPSHOT_INVENTORY_NOT_APPLICABLE=1' "$HELPER"
grep -Fq 'Snapshot inventory is not applicable: the selected target filesystem is %s, not Btrfs.' "$HELPER"

snapshot_info="$(run_harness '
prepare_target_snapshots_only() {
    ROOT_CANONICAL=/dev/test-root
    SESSION_LOG="'"$sandbox"'/snapshot-session.log"
    : > "$SESSION_LOG"
}
lsblk() { printf "ext4\n"; }
need() {
    if [[ "$1" == btrfs ]]; then return 1; fi
    command -v "$1" >/dev/null 2>&1
}
run_snapshots list
')"
grep -Fq 'SNAPSHOT_INVENTORY_NOT_APPLICABLE=1' <<<"$snapshot_info" \
    || { echo 'FAIL: non-Btrfs snapshot inventory did not emit the informational marker' >&2; printf '%s\n' "$snapshot_info" >&2; exit 1; }
grep -Fq 'Snapshot inventory is not applicable: the selected target filesystem is ext4, not Btrfs.' <<<"$snapshot_info" \
    || { echo 'FAIL: non-Btrfs snapshot inventory did not name the detected filesystem' >&2; exit 1; }
if grep -q 'Required host command not found: btrfs' <<<"$snapshot_info"; then
    echo 'FAIL: the non-Btrfs list path must not require btrfs-progs' >&2
    exit 1
fi

if snapshot_inspect="$(run_harness '
prepare_target_snapshots_only() {
    ROOT_CANONICAL=/dev/test-root
    SESSION_LOG="'"$sandbox"'/snapshot-session.log"
    : > "$SESSION_LOG"
}
lsblk() { printf "ext4\n"; }
run_snapshots inspect 1
' 2>&1)"; then
    echo 'FAIL: non-Btrfs snapshot inspect must fail explicitly' >&2
    exit 1
fi
grep -Fq 'Snapshot inspection requires a Btrfs repair root; detected ext4.' <<<"$snapshot_inspect" \
    || { echo 'FAIL: non-Btrfs snapshot inspect refusal reason changed' >&2; printf '%s\n' "$snapshot_inspect" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 12: prepare_target mounts the target fstab's separate non-Btrfs data
# partitions (/usr /var /tmp /home /opt /srv) that live on the selected disk.
# Read-only diagnostics get ro mounts; remount_target_data_rw promotes them
# for the modifying stages; entries outside the selected disk are refused;
# pseudo and Btrfs entries keep their existing handling; and a modifying
# stage fails closed when a required data filesystem cannot be mounted.
# ---------------------------------------------------------------------------
cat > "$sandbox/target/etc/fstab" <<'FSTAB'
/dev/test-root  /     ext4  defaults  0 1
/dev/test-boot  /boot ext4  defaults  0 2
/dev/test-root  /usr  ext4  defaults  0 2
/dev/test-var   /var  ext4  defaults,noatime  0 2
/dev/test-home  /home xfs   defaults  0 2
/dev/test-other /none ext4  defaults  0 2
tmpfs           /tmp  tmpfs defaults  0 0
/dev/test-opt   /opt  btrfs subvol=/@opt  0 0
/dev/test-srv   /srv  ext4  nofail,relatime  0 2
FSTAB

# The mocked readlink proves /dev/test-* existence through sandbox files.
touch "$sandbox/dev-test-var" "$sandbox/dev-test-srv" "$sandbox/dev-test-opt" "$sandbox/dev-test-other"

data_mount_harness='top_disks_for() {
    case "$1" in
        */test-home) printf "%s\n" "test-other-disk" ;;
        *) printf "%s\n" "test-disk" ;;
    esac
}
ROOT_CANONICAL=/dev/test-root
TARGET_DISK=/dev/test-disk
TARGET_ROOT="'"$sandbox"'/target"
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
: > "$FAKE_MOUNT_LOG"
TARGET_DATA_MOUNTS=()
mount_target_data_partitions ro
printf "MOUNTS:%s\n" "$(printf "%s," "${TARGET_DATA_MOUNTS[@]:-}")"'

data_ro="$(run_harness "$data_mount_harness")"
grep -Fq 'MOUNTS:' <<<"$data_ro" \
    || { echo 'FAIL: data mount harness produced no mount list' >&2; printf '%s\n' "$data_ro" >&2; exit 1; }
grep -Fq "$sandbox/target/var," <<<"$data_ro" \
    || { echo 'FAIL: separate /var was not recorded as a data mount' >&2; printf '%s\n' "$data_ro" >&2; exit 1; }
grep -Fq "$sandbox/target/srv," <<<"$data_ro" \
    || { echo 'FAIL: separate /srv was not recorded as a data mount' >&2; printf '%s\n' "$data_ro" >&2; exit 1; }
grep -Fq "$sandbox/target/home," <<<"$data_ro" \
    && { echo 'FAIL: an off-disk /home was mounted instead of refused' >&2; printf '%s\n' "$data_ro" >&2; exit 1; }
grep -Fq "$sandbox/target/opt," <<<"$data_ro" \
    && { echo 'FAIL: a Btrfs /opt was mounted as a plain data partition' >&2; printf '%s\n' "$data_ro" >&2; exit 1; }
grep -Fq "$sandbox/target/usr," <<<"$data_ro" \
    && { echo 'FAIL: a same-device /usr entry was mounted twice' >&2; printf '%s\n' "$data_ro" >&2; exit 1; }
grep -Fq -- '-o ro,noatime,nosuid,nodev -- /dev/test-var' "$FAKE_MOUNT_LOG" \
    || { echo 'FAIL: /var was not mounted read-only with its fstab options plus the recovery nosuid,nodev hardening' >&2; cat "$FAKE_MOUNT_LOG" >&2; exit 1; }
grep -Fq -- '-o ro,relatime,nosuid,nodev -- /dev/test-srv' "$FAKE_MOUNT_LOG" \
    || { echo 'FAIL: /srv was not mounted read-only with its fstab options plus the recovery nosuid,nodev hardening' >&2; cat "$FAKE_MOUNT_LOG" >&2; exit 1; }
grep -Fq -- 'tmpfs' "$FAKE_MOUNT_LOG" \
    && { echo 'FAIL: a tmpfs pseudo entry was mounted' >&2; cat "$FAKE_MOUNT_LOG" >&2; exit 1; }

# The rw promotion remounts every recorded data mount read-write with the
# machine-readable line; the modifying stage fails closed when the mount is
# unavailable.  The harness snippet is single-quoted (the shellcheck-safe
# pattern used by data_mount_harness) and reuses TARGET_ROOT for the sandbox
# paths.
promotion="$(run_harness "$data_mount_harness"'
: > "$FAKE_MOUNT_LOG"
printf "%s\n" "$TARGET_ROOT/var" "$TARGET_ROOT/srv" >> "$FAKE_MOUNTPOINT_DB"
remount_target_data_rw')"
grep -Fq 'msg:remount-data-rw|param:/var' <<<"$promotion" \
    || { echo 'FAIL: /var rw promotion line missing' >&2; printf '%s\n' "$promotion" >&2; exit 1; }
grep -Fq 'msg:remount-data-rw|param:/srv' <<<"$promotion" \
    || { echo 'FAIL: /srv rw promotion line missing' >&2; printf '%s\n' "$promotion" >&2; exit 1; }
grep -Fq -- '-o remount,rw ' "$FAKE_MOUNT_LOG" \
    || { echo 'FAIL: the rw promotion did not remount the data mounts' >&2; cat "$FAKE_MOUNT_LOG" >&2; exit 1; }

fail_out="$(FAKE_MOUNT_FAIL=1 run_harness "$data_mount_harness"'
: > "$FAKE_MOUNTPOINT_DB"
TARGET_DATA_MOUNTS=()
mount_target_data_partitions rw' 2>&1 || true)"
grep -Fq 'Repair requires the target /var filesystem (/dev/test-var) and it could not be mounted.' <<<"$fail_out" \
    || { echo 'FAIL: a modifying stage tolerated an unmountable required data filesystem' >&2; printf '%s\n' "$fail_out" >&2; exit 1; }

# Already-mounted destinations are never mounted twice.
printf '%s\n' "$sandbox/target/var" >> "$FAKE_MOUNTPOINT_DB"
data_skip="$(run_harness "$data_mount_harness")"
grep -Fq "$sandbox/target/srv," <<<"$data_skip" \
    || { echo 'FAIL: the already-mounted /var suppressed the remaining data mounts' >&2; printf '%s\n' "$data_skip" >&2; exit 1; }
grep -Fq "$sandbox/target/var," <<<"$data_skip" \
    && { echo 'FAIL: the already-mounted /var was recorded again' >&2; printf '%s\n' "$data_skip" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part: ESP derivation fixture.  A UEFI target whose root and ESP are separate
# partitions on the selected disk, referenced by UUID in fstab (the Debian
# trixie generic-cloud layout), must derive the ESP through the guarded
# fstab-driven mount and pass the conventional EFI preflight.  When no ESP is
# derivable at all, the efi capability reports unavailable with the precise
# reason instead of staying available behind a preflight that can never pass.
# ---------------------------------------------------------------------------
cat > "$sandbox/target/etc/fstab" <<'FSTAB'
UUID=11111111-2222-3333-4444-555555555555  /          ext4  defaults  0 1
UUID=ABCD-1234                              /boot/efi  vfat  defaults  0 2
FSTAB
mkdir -p "$sandbox/target/usr/sbin"
for esp_tool in grub-install grub-mkconfig; do
    : > "$sandbox/target/usr/sbin/$esp_tool"; chmod +x "$sandbox/target/usr/sbin/$esp_tool"
done

esp_preflight="$(FAKE_MOUNT_ADD_POINT="$sandbox/target/boot/efi" \
    FAKE_MOUNT_ADD_ROW="$sandbox/target/boot/efi /dev/test-efi vfat rw 0" \
    run_harness '
SESSION_LOG="'"$sandbox"'/esp-preflight.log"; : > "$SESSION_LOG"
: > "$FAKE_FINDMNT_DB"; : > "$FAKE_MOUNTPOINT_DB"; : > "$FAKE_MOUNT_LOG"
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_ROOT="'"$sandbox"'/target"
TARGET_OS_ID=debian
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=debian
eval "$real_mount_target_boot_entry"
mount_target_boot_entry /boot/efi ro
profile_target_backends
[[ "$TARGET_ESP_MOUNT" == /boot/efi ]]
validate_selected_esp
printf "DERIVED\t%s\t%s\t%s\n" "$TARGET_ESP_MOUNT" "$EFI_ESP_SOURCE" "$EFI_ESP_FSTYPE"
')"
grep -Fqx $'DERIVED\t/boot/efi\t/dev/test-efi\tvfat' <<<"$esp_preflight" \
    || { echo "FAIL: the fstab-by-UUID ESP was not derived through the guarded boot-entry mount" >&2; printf '%s\n' "$esp_preflight" >&2; exit 1; }
grep -Fq 'msg:mounting-target|param:/boot/efi|param:/dev/test-efi' <<<"$esp_preflight" \
    || { echo 'FAIL: the fstab-driven ESP mount did not run through the guarded mount path' >&2; exit 1; }

# The same layout keeps the efi capability available ...
# (Bare /boot/efi: only the fstab entry can make the ESP derivable.)
rm -rf "$sandbox/target/boot/efi/EFI"
mkdir -p "$sandbox/target/boot/efi"
esp_cap_available="$(run_harness '
SESSION_LOG="'"$sandbox"'/esp-cap.log"; : > "$SESSION_LOG"
: > "$FAKE_FINDMNT_DB"; : > "$FAKE_MOUNTPOINT_DB"
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_ROOT="'"$sandbox"'/target"
TARGET_OS_ID=debian
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=debian
if reason="$(efi_unavailable_reason)"; then
    printf "AVAILABLE\n"
else
    printf "UNEXPECTED\t%s\n" "$reason"
fi
')"
grep -Fqx 'AVAILABLE' <<<"$esp_cap_available" \
    || { echo "FAIL: a derivable fstab-by-UUID ESP made the efi capability unavailable" >&2; printf '%s\n' "$esp_cap_available" >&2; exit 1; }

# ... and with no ESP entry and no ESP partition it reports unavailable with
# the precise reason, so a Full Repair plan never schedules the EFI stage.
printf 'UUID=11111111-2222-3333-4444-555555555555  /  ext4  defaults  0 1\n' > "$sandbox/target/etc/fstab"
# Earlier fixtures left loader directories under the mountpoint; the
# unavailable case must run with a bare /boot/efi directory so no dir
# heuristic can invent an ESP.
rm -rf "$sandbox/target/boot/efi/EFI"
mkdir -p "$sandbox/target/boot/efi"
esp_cap_unavailable="$(run_harness '
SESSION_LOG="'"$sandbox"'/esp-cap.log"; : > "$SESSION_LOG"
: > "$FAKE_FINDMNT_DB"; : > "$FAKE_MOUNTPOINT_DB"
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_ROOT="'"$sandbox"'/target"
TARGET_OS_ID=debian
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=debian
if reason="$(efi_unavailable_reason)"; then
    printf "UNEXPECTED-AVAILABLE\n"
else
    printf "UNAVAILABLE\t%s\n" "$reason"
fi
')"
grep -Fqx 'UNAVAILABLE	reason:no-esp-derivable' <<<"$esp_cap_unavailable" \
    || { echo "FAIL: a target with no derivable ESP kept the efi capability available" >&2; printf '%s\n' "$esp_cap_unavailable" >&2; exit 1; }

# By-type discovery: with no fstab ESP entry the preflight derives the ESP
# partition from its GPT type, and the read-write promotion remounts the
# helper-recorded preflight mount instead of leaving it read-only.  (Part 7
# rewrote the device database without the GPT type; restore it for this
# fixture.)
sed -i 's#^test-efi .*#test-efi vfat part ABCD-1234 test-disk 1 test-disk C12A7328-F81F-11D2-BA4B-00A0C93EC93B#' "$FAKE_LSBLK_DB"
bytype_preflight="$(FAKE_MOUNT_ADD_POINT="$sandbox/target/boot/efi" \
    FAKE_MOUNT_ADD_ROW="$sandbox/target/boot/efi /dev/test-efi vfat rw 0" \
    run_harness '
SESSION_LOG="'"$sandbox"'/bytype.log"; : > "$SESSION_LOG"
: > "$FAKE_FINDMNT_DB"; : > "$FAKE_MOUNTPOINT_DB"; : > "$FAKE_MOUNT_LOG"
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_ROOT="'"$sandbox"'/target"
TARGET_OS_ID=debian
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=debian
mount_target_esp_by_type ro
eval "$real_mount_target_boot_entry"
mount_target_boot_entry /boot/efi rw
profile_target_backends
[[ "$TARGET_ESP_MOUNT" == /boot/efi ]]
validate_selected_esp
printf "BYTYPE\t%s\t%s\n" "$EFI_ESP_SOURCE" "$EFI_ESP_FSTYPE"
')"
grep -Fqx $'BYTYPE\t/dev/test-efi\tvfat' <<<"$bytype_preflight" \
    || { echo "FAIL: the GPT-type ESP was not derived and validated through the guarded by-type mount" >&2; printf '%s\n' "$bytype_preflight" >&2; exit 1; }
grep -Fq 'msg:mounted-esp-gpt|param:/dev/test-efi|param:ro' <<<"$bytype_preflight" \
    || { echo 'FAIL: the by-type ESP mount did not log its guarded mount evidence' >&2; exit 1; }
grep -Fq 'msg:remount-rw|param:/boot/efi' <<<"$bytype_preflight" \
    || { echo 'FAIL: the read-write promotion did not remount the helper-recorded by-type ESP' >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part: guarded target configuration on a backend without a GRUB configuration
# path (Alpine extlinux).  The read degrades to an informative skip instead of
# ERROR-level noise, a present configuration still reads, and the write still
# refuses to create a missing path.
# ---------------------------------------------------------------------------
rm -rf "$sandbox/target/usr/sbin"
: > "$sandbox/target/boot/extlinux.conf"
mkdir -p "$sandbox/target/etc/default"
printf 'GRUB_TIMEOUT=5\n' > "$sandbox/target/etc/default/grub"
config_read_ok="$(run_harness '
SESSION_LOG="'"$sandbox"'/config-read.log"; : > "$SESSION_LOG"
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=alpine
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=alpine
run_target_config read grub-defaults
')"
grep -Fq 'GRUB_TIMEOUT=5' <<<"$config_read_ok" \
    || { echo 'FAIL: an existing target configuration no longer reads' >&2; printf '%s\n' "$config_read_ok" >&2; exit 1; }

rm -rf "$sandbox/target/etc/default"
config_read_skip="$(run_harness '
SESSION_LOG="'"$sandbox"'/config-read.log"; : > "$SESSION_LOG"
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=alpine
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=alpine
run_target_config read grub-defaults
')"
grep -Fqx 'Target configuration: /etc/default/grub' <<<"$config_read_skip" \
    || { echo 'FAIL: the skipped configuration read did not name the configuration' >&2; printf '%s\n' "$config_read_skip" >&2; exit 1; }
grep -Fq 'msg:config-read-skipped|param:/etc/default/grub|param:' <<<"$config_read_skip" \
    || { echo 'FAIL: the missing GRUB configuration parent did not degrade to an informative skip' >&2; printf '%s\n' "$config_read_skip" >&2; exit 1; }
grep -Fq 'Not present in this target (bootloader backend: syslinux/extlinux); no configuration path exists.' <<<"$config_read_skip" \
    || { echo 'FAIL: the skipped configuration read did not name the extlinux backend' >&2; printf '%s\n' "$config_read_skip" >&2; exit 1; }
if grep -Fq 'ERROR' <<<"$config_read_skip"; then
    echo 'FAIL: a missing GRUB configuration path still emits ERROR-level noise' >&2
    printf '%s\n' "$config_read_skip" >&2
    exit 1
fi

if config_write_refused="$(run_harness '
SESSION_LOG="'"$sandbox"'/config-write.log"; : > "$SESSION_LOG"
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=alpine
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=alpine
run_target_config write grub-defaults "GRUB_TIMEOUT=1"
' 2>&1)"; then
    echo 'FAIL: config-write created a GRUB configuration path the target does not have' >&2
    exit 1
fi
grep -Fq 'Target configuration does not exist; refusing to create: /etc/default/grub' <<<"$config_write_refused" \
    || { echo 'FAIL: the missing-configuration write refusal lost its explicit reason' >&2; printf '%s\n' "$config_write_refused" >&2; exit 1; }
[[ ! -e "$sandbox/target/etc/default/grub" ]] \
    || { echo 'FAIL: config-write created a missing GRUB configuration path' >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part: a failed mutating stage releases the helper-owned target mount on exit.
# The EXIT trap cleanup() unmounts MOUNTS with a plain umount (never a lazy
# detach), so a `fail` in a kernels/dkms/initramfs stage leaves no MOUNT_LEAK and
# no stale session mount.  This uses a dedicated harness that KEEPS the helper's
# EXIT trap (the shared harness above disarms it), so the real release path runs.
# ---------------------------------------------------------------------------
: > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_UMOUNT_LOG"
: > "$FAKE_FINDMNT_DB"

cat > "$sandbox/exit-trap-harness.sh" <<HARNESS_EXIT
#!/usr/bin/env bash
set -euo pipefail
source <(sed '/^main "\$@"/d' "$HELPER")
PATH="$sandbox/mockbin:\$PATH"
STATE_ROOT="$sandbox/state"
SESSION_DIR="$sandbox/exit-session"
SESSION_LOG="$sandbox/exit-session.log"
mkdir -p "\$SESSION_DIR"
: > "\$SESSION_LOG"
TARGET_WRITE_INTENT=0
MOUNT_BASE="$sandbox/exit-session/mount"
mkdir -p "\$MOUNT_BASE"
# Simulate prepare_target having mounted the target root and recorded it.
printf '%s\n' "\$MOUNT_BASE" >> "$FAKE_MOUNTPOINT_DB"
MOUNTS=("\$MOUNT_BASE")
# A mutating stage fails mid-way: fail() exits and the EXIT trap cleanup() runs.
fail "simulated mutating-stage failure"
HARNESS_EXIT

set +e
exit_trap_out="$(bash --noprofile --norc "$sandbox/exit-trap-harness.sh" 2>&1)"
exit_trap_rc=$?
set -e
[[ "$exit_trap_rc" -ne 0 ]] \
    || { echo 'FAIL: a failed stage must exit nonzero' >&2; exit 1; }
grep -Fq 'simulated mutating-stage failure' <<<"$exit_trap_out" \
    || { echo 'FAIL: the failed stage message was not surfaced' >&2; printf '%s\n' "$exit_trap_out" >&2; exit 1; }
if grep -Fq 'MOUNT_LEAK' <<<"$exit_trap_out"; then
    echo 'FAIL: a failed stage leaked the target mount (MOUNT_LEAK emitted)' >&2
    printf '%s\n' "$exit_trap_out" >&2
    exit 1
fi
grep -Fqx -- "$sandbox/exit-session/mount" "$FAKE_UMOUNT_LOG" \
    || { echo 'FAIL: the EXIT trap did not unmount the target root' >&2; cat "$FAKE_UMOUNT_LOG" >&2; exit 1; }
[[ ! -s "$FAKE_MOUNTPOINT_DB" ]] \
    || { echo 'FAIL: the target mount is still recorded as mounted after the failure' >&2; cat "$FAKE_MOUNTPOINT_DB" >&2; exit 1; }


echo "PASS: file system repair helper contract is wired, read-only by default and scope-safe."
