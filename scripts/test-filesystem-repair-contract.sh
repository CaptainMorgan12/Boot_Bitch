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
grep -q 'local -a keys=(validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub extlinux bootstack)' "$HELPER"
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
# A2-05: the offline repair release path never lazy-detaches; the read-only
# inspection path keeps its existing best-effort release.
release_for_device_block="$(sed -n '/^filesystem_release_mounts_for_device()/,/^}/p' "$HELPER")"
if grep -Fq 'umount -l' <<<"$release_for_device_block"; then
    echo 'FAIL: the offline repair path still falls back to a lazy umount' >&2
    exit 1
fi
grep -Fq 'mount_cleanup_leak_evidence "$mountpath"' <<<"$release_for_device_block" \
    || { echo 'FAIL: the offline repair path does not record leak evidence on an umount failure' >&2; exit 1; }
release_all_block="$(sed -n '/^filesystem_release_all_mounts()/,/^}/p' "$HELPER")"
grep -Fq 'umount "$mountpath" 2>/dev/null || umount -l "$mountpath" 2>/dev/null || true' <<<"$release_all_block" \
    || { echo 'FAIL: the read-only inspection release path changed' >&2; exit 1; }
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
fc_root=""
cleanup_sandbox() {
    rm -rf -- "$sandbox"
    [[ -n "$fc_root" ]] && rm -rf -- "$fc_root"
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
for cap_key in dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub extlinux bootstack; do
    grep -Eq "^Repair tool $cap_key: " <<<"$cap_unresolved" \
        || { echo "FAIL: existing capability key missing: $cap_key" >&2; exit 1; }
done
[[ "$(grep -c '^Repair tool ' <<<"$cap_unresolved")" -eq 13 ]] \
    || { echo "FAIL: expected 13 capability keys" >&2; exit 1; }
# Running-host maintenance adds host-scope capability evidence but must never
# become a 14th `Repair tool` key; target scope never emits the host lines.
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
[[ "$(grep -c '^Repair tool ' <<<"$host_caps")" -eq 13 ]] \
    || { echo "FAIL: host scope changed the 13 Repair tool keys" >&2; printf '%s\n' "$host_caps" >&2; exit 1; }
grep -Eq '^Host snapshot rollback: (available|unavailable\|)' <<<"$host_caps" \
    || { echo "FAIL: host snapshot rollback capability line is missing" >&2; exit 1; }
grep -Eq '^Host reboot: (available|unavailable\|)' <<<"$host_caps" \
    || { echo "FAIL: host reboot capability line is missing" >&2; exit 1; }
if grep -q '^Host ' <<<"$cap_unresolved"; then
    echo 'FAIL: target-scope diagnostics must not emit host capability lines' >&2
    exit 1
fi

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
# Part 6: zfs.  Pool resolution is required, scrub waits for completion and
# the pool status is parsed; dataset sources (rpool/ROOT/...) map to their
# pool; the online mode is gated on pool import, not a mountpoint.
# ---------------------------------------------------------------------------
printf 'test-zfs zfs part ZFS-UUID test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
: > "$sandbox/dev-test-zfs"
cat >> "$sandbox/target/etc/fstab" <<'FSTAB'
/dev/test-zfs  /home  zfs  defaults  0 2
FSTAB
# Remove the previous xfs /home line so the zfs entry is the only /home source.
sed -i '\|^/dev/test-home  /home|d' "$sandbox/target/etc/fstab"

if zfs_missing_pool="$(run_harness '
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT=""
filesystem_pool_for_device() { return 1; }
fs_repair /dev/test-disk /dev/test-root /dev/test-zfs scrub
' 2>&1)"; then
    echo 'FAIL: zfs scrub without an identified pool was accepted' >&2
    exit 1
fi
grep -Fq "The ZFS pool for /dev/test-zfs could not be identified" <<<"$zfs_missing_pool" \
    || { echo 'FAIL: missing zfs pool refusal reason' >&2; exit 1; }

: > "$FAKE_TOOL_LOG"
zfs_clean="$(FAKE_ZPOOL_POOLS=rpool FAKE_ZPOOL_STATUS_P='	/dev/test-zfs	ONLINE' run_harness "$(repair_code /dev/test-zfs scrub)" 2>&1 || true)"
expect_repair_line /dev/test-zfs scrub clean "$zfs_clean"
grep -Fq 'zpool scrub -w rpool' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: zpool scrub must wait for completion' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1; }
grep -Fq 'zpool status -v rpool' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: zpool status must be scoped to the pool after the scrub' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1; }
# A clean scrub clears the pool's error counters (zpool clear), never before a
# faulted/degraded pool is proven healthy.
grep -Fq 'zpool clear rpool' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: a clean zpool scrub did not clear the pool error counters' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1; }

: > "$FAKE_TOOL_LOG"
zfs_degraded="$(FAKE_ZPOOL_POOLS=rpool FAKE_ZPOOL_STATUS_P='	/dev/test-zfs	ONLINE' FAKE_ZPOOL_STATUS_V='state: DEGRADED' run_harness "$(repair_code /dev/test-zfs scrub)" 2>&1 || true)"
expect_repair_line /dev/test-zfs scrub issues "$zfs_degraded"
# A degraded pool must not have its error counters cleared.
if grep -Fq 'zpool clear rpool' "$FAKE_TOOL_LOG"; then
    echo 'FAIL: a degraded zpool scrub cleared the pool error counters' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1
fi

# A dataset source is mapped to its imported pool for scope and scrub.
: > "$FAKE_TOOL_LOG"
zfs_dataset="$(FAKE_ZPOOL_POOLS=rpool run_harness '
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT=""
printf "/dev/test-disk  /  ext4  defaults  0 1\n" > "'"$sandbox"'/target/etc/fstab"
printf "rpool/ROOT/arch  /home  zfs  defaults  0 2\n" >> "'"$sandbox"'/target/etc/fstab"
prepare_target
filesystem_scope_resolve
printf "SCOPE:%s\n" "${FS_SCOPE_DEVICES[*]}"
fs_repair /dev/test-disk /dev/test-root rpool scrub
' 2>&1 || true)"
grep -Fq 'SCOPE:/dev/test-root rpool' <<<"$zfs_dataset" \
    || { echo 'FAIL: zfs dataset was not mapped to its pool in the scope' >&2; printf '%s\n' "$zfs_dataset" >&2; exit 1; }
grep -Fq 'zpool scrub -w rpool' "$FAKE_TOOL_LOG" \
    || { echo 'FAIL: zfs dataset scrub did not target the pool' >&2; cat "$FAKE_TOOL_LOG" >&2; exit 1; }
# Restore the shared target fstab for the remaining parts.
cat > "$sandbox/target/etc/fstab" <<'FSTAB'
/dev/test-root  /          ext4  defaults  0 1
/dev/test-boot  /boot      ext4  defaults  0 2
/dev/test-efi   /boot/efi  vfat  defaults  0 2
/dev/test-home  /home      xfs   defaults  0 2
FSTAB

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
[[ "$(grep -c '^Repair tool ' <<<"$combined_report")" -eq 13 ]] \
    || { echo 'FAIL: combined report lost capability key lines' >&2; exit 1; }
[[ "$(grep -c '^Repair capability evidence [a-z]*: ' <<<"$combined_report")" -eq 13 ]] \
    || { echo 'FAIL: combined report lost capability evidence lines' >&2; exit 1; }
[[ "$(grep -c '^Diagnostic: ' <<<"$combined_report")" -eq 15 ]] \
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
prepare_target() {
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
prepare_target() {
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
# Part 11: ESP writability.  The effective mount is the topmost findmnt row
# (never the bottom/autofs row, never a reused mount ID); a leaked read-only
# layer may only be cleared when it is the selected ESP above a writable
# same-device mount; cleanup must leave rw/foreign mounts untouched and report
# leaks instead of swallowing them.
# ---------------------------------------------------------------------------
esp_dir="$sandbox/target/boot/efi"
mkdir -p "$esp_dir/EFI/BOOT" "$sandbox/target/home"

# 11a: topmost-row probe.  The bottom autofs row is rw, but the effective top
# is the last ro vfat row, so the rw gate must refuse while target_mount_top
# selects the topmost row.
cat > "$FAKE_FINDMNT_DB" <<MNT
$esp_dir systemd-1 autofs rw 164
$esp_dir /dev/test-efi vfat rw 612
$esp_dir /dev/test-efi vfat ro 1477
$esp_dir /dev/test-efi vfat ro 1416
MNT
printf '%s\n' "$esp_dir" > "$FAKE_MOUNTPOINT_DB"
topmost="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
EFI_ESP_SOURCE=/dev/test-efi
printf "TOP:%s\n" "$(target_mount_top "'"$esp_dir"'")"
if target_path_is_mounted_rw "'"$esp_dir"'"; then echo RW:PASS; else echo RW:REFUSED; fi
')"
grep -Fqx "TOP:$esp_dir /dev/test-efi ro 1416" <<<"$topmost" \
    || { echo 'FAIL: target_mount_top did not select the topmost row' >&2; printf '%s\n' "$topmost" >&2; exit 1; }
grep -Fqx 'RW:REFUSED' <<<"$topmost" \
    || { echo 'FAIL: the rw gate accepted a read-only-topped ESP stack' >&2; printf '%s\n' "$topmost" >&2; exit 1; }
cat > "$FAKE_FINDMNT_DB" <<MNT
$esp_dir systemd-1 autofs rw 164
$esp_dir /dev/test-efi vfat rw 612
MNT
top_rw="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
EFI_ESP_SOURCE=/dev/test-efi
if target_path_is_mounted_rw "'"$esp_dir"'"; then echo RW:PASS; else echo RW:REFUSED; fi
')"
grep -Fqx 'RW:PASS' <<<"$top_rw" \
    || { echo 'FAIL: a writable topmost ESP row was refused' >&2; printf '%s\n' "$top_rw" >&2; exit 1; }

# 11b: leaked read-only preflight refusal.  check mode must fail closed with
# the topmost evidence, the leaked-layer count and the exact cleanup command,
# and must not unmount anything.
cat > "$FAKE_FINDMNT_DB" <<MNT
$esp_dir systemd-1 autofs rw 164
$esp_dir /dev/test-efi vfat rw 612
$esp_dir /dev/test-efi vfat ro 1477
MNT
printf '%s\n' "$esp_dir" > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_UMOUNT_LOG"
if leaked_check="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=/dev/test-efi
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
esp_writable_preflight check
' 2>&1)"; then
    echo 'FAIL: the check-only ESP preflight accepted a leaked read-only layer' >&2
    exit 1
fi
grep -Fq "msg:esp-mount-preflight|param:$esp_dir|param:3|param:/dev/test-efi|param:ro|param:1477|param:ro-leaked|param:1" <<<"$leaked_check" \
    || { echo 'FAIL: leaked preflight evidence line missing or wrong' >&2; printf '%s\n' "$leaked_check" >&2; exit 1; }
grep -Fq 'leaked read-only layers: 1' <<<"$leaked_check" \
    || { echo 'FAIL: leaked preflight did not report the leaked layer count' >&2; printf '%s\n' "$leaked_check" >&2; exit 1; }
grep -Fq "findmnt -T $esp_dir -o TARGET,SOURCE,OPTIONS,ID" <<<"$leaked_check" \
    || { echo 'FAIL: leaked preflight did not print the inspection command' >&2; exit 1; }
grep -Fq "umount $esp_dir" <<<"$leaked_check" \
    || { echo 'FAIL: leaked preflight did not print the manual cleanup command' >&2; exit 1; }
[[ ! -s "$FAKE_UMOUNT_LOG" ]] \
    || { echo 'FAIL: the check-only preflight unmounted a layer' >&2; exit 1; }

# 11c: foreign refusal.  A mount that is not the selected ESP must never be
# unmounted; clear mode refuses with the cleanup command.
cat > "$FAKE_FINDMNT_DB" <<MNT
$esp_dir /dev/test-efi vfat rw 612
$esp_dir /dev/test-outside ext4 ro 900
MNT
printf '%s\n' "$esp_dir" > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_UMOUNT_LOG"
if foreign_clear="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=/dev/test-efi
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
esp_writable_preflight clear
' 2>&1)"; then
    echo 'FAIL: the ESP preflight cleared a foreign topmost mount' >&2
    exit 1
fi
grep -Fq "param:/dev/test-outside|param:ro|param:900|param:foreign" <<<"$foreign_clear" \
    || { echo 'FAIL: foreign preflight evidence line missing or wrong' >&2; printf '%s\n' "$foreign_clear" >&2; exit 1; }
grep -Fq 'not the selected EFI System Partition' <<<"$foreign_clear" \
    || { echo 'FAIL: foreign preflight did not refuse' >&2; printf '%s\n' "$foreign_clear" >&2; exit 1; }
grep -Fq "findmnt -T $esp_dir -o TARGET,SOURCE,OPTIONS,ID" <<<"$foreign_clear" \
    || { echo 'FAIL: foreign preflight did not print the inspection command' >&2; exit 1; }
[[ ! -s "$FAKE_UMOUNT_LOG" ]] \
    || { echo 'FAIL: the foreign preflight unmounted a mount' >&2; exit 1; }

# The real vendor-builder entry re-probes and refuses before any target write.
: > "$FAKE_UMOUNT_LOG"
if builder_refusal="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=/dev/test-efi
SESSION_DIR="'"$sandbox"'/session.contract"
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
run_tuxedo_uki_builder "Contract vendor run" /bin/true
' 2>&1)"; then
    echo 'FAIL: run_tuxedo_uki_builder accepted a foreign ESP stack' >&2
    exit 1
fi
grep -Fq 'param:foreign' <<<"$builder_refusal" \
    || { echo 'FAIL: the vendor-builder preflight did not report the foreign stack' >&2; printf '%s\n' "$builder_refusal" >&2; exit 1; }
[[ ! -e "$sandbox/target/usr/local/libexec" ]] \
    || { echo 'FAIL: the vendor builder created target files before the ESP check' >&2; exit 1; }
[[ ! -s "$FAKE_UMOUNT_LOG" ]] \
    || { echo 'FAIL: the vendor-builder check unmounted a mount' >&2; exit 1; }

# rebuild_tuxedo_uki refuses the same foreign stack before the vendor builder.
: > "$sandbox/vendor.log"
if rebuild_refusal="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=/dev/test-efi
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
validate_tuxedo_uki_target() { :; }
newest_tuxedo_kernel() { printf "6.1.0-tuxedo-amd64\n"; }
uefi_nvram_writable() { return 1; }
run_tuxedo_uki_builder() { printf "vendor\n" >> "'"$sandbox"'/vendor.log"; }
rebuild_tuxedo_uki
' 2>&1)"; then
    echo 'FAIL: rebuild_tuxedo_uki accepted a foreign ESP stack' >&2
    exit 1
fi
grep -Fq 'param:foreign' <<<"$rebuild_refusal" \
    || { echo 'FAIL: rebuild_tuxedo_uki did not report the foreign stack' >&2; printf '%s\n' "$rebuild_refusal" >&2; exit 1; }
[[ ! -s "$sandbox/vendor.log" ]] \
    || { echo 'FAIL: rebuild_tuxedo_uki invoked the vendor builder on a foreign stack' >&2; exit 1; }

# A foreign layer in between the leaked ESP layer and its writable base must
# refuse before the first unmount, never partially dismantling the stack.
cat > "$FAKE_FINDMNT_DB" <<MNT
$esp_dir /dev/test-efi vfat rw 612
$esp_dir /dev/test-outside ext4 ro 900
$esp_dir /dev/test-efi vfat ro 1477
MNT
printf '%s\n' "$esp_dir" > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_UMOUNT_LOG"
if sandwiched_clear="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=/dev/test-efi
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
esp_writable_preflight clear
' 2>&1)"; then
    echo 'FAIL: the ESP preflight cleared a stack with a foreign layer in between' >&2
    exit 1
fi
grep -Fq 'param:ro-leaked|param:1' <<<"$sandwiched_clear" \
    || { echo 'FAIL: sandwiched preflight did not report the leaked layer' >&2; printf '%s\n' "$sandwiched_clear" >&2; exit 1; }
grep -Fq 'not directly bounded by a writable mount of the selected ESP' <<<"$sandwiched_clear" \
    || { echo 'FAIL: sandwiched preflight did not refuse before unmounting' >&2; printf '%s\n' "$sandwiched_clear" >&2; exit 1; }
[[ ! -s "$FAKE_UMOUNT_LOG" ]] \
    || { echo 'FAIL: the sandwiched preflight unmounted a layer' >&2; exit 1; }

# 11d: safe cleanup.  Only the two ro layers of the selected ESP above the rw
# base are unmounted; the rw ESP mount and the foreign mount below it stay.
cat > "$FAKE_FINDMNT_DB" <<MNT
$esp_dir systemd-1 autofs rw 164
$esp_dir /dev/test-outside ext4 ro 900
$esp_dir /dev/test-efi vfat rw 612
$esp_dir /dev/test-efi vfat ro 1477
$esp_dir /dev/test-efi vfat ro 1416
MNT
printf '%s\n' "$esp_dir" > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_UMOUNT_LOG"
: > "$FAKE_MOUNT_LOG"
safe_clear="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=/dev/test-efi
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
esp_writable_preflight clear
')"
grep -Fq "msg:esp-mount-preflight|param:$esp_dir|param:5|param:/dev/test-efi|param:ro|param:1416|param:ro-leaked|param:2" <<<"$safe_clear" \
    || { echo 'FAIL: safe-cleanup preflight evidence line missing or wrong' >&2; printf '%s\n' "$safe_clear" >&2; exit 1; }
[[ "$(grep -c 'msg:esp-mount-cleanup|param:' <<<"$safe_clear")" -eq 2 ]] \
    || { echo 'FAIL: expected two leaked-ro cleanup evidence lines' >&2; printf '%s\n' "$safe_clear" >&2; exit 1; }
grep -Fq "msg:esp-mount-preflight|param:$esp_dir|param:3|param:/dev/test-efi|param:rw|param:612|param:rw|param:0" <<<"$safe_clear" \
    || { echo 'FAIL: safe cleanup did not report a writable ESP afterwards' >&2; printf '%s\n' "$safe_clear" >&2; exit 1; }
[[ "$(wc -l < "$FAKE_UMOUNT_LOG")" -eq 2 ]] \
    || { echo 'FAIL: safe cleanup unmounted the wrong number of layers' >&2; cat "$FAKE_UMOUNT_LOG" >&2; exit 1; }
[[ "$(sort -u "$FAKE_UMOUNT_LOG")" == "$esp_dir" ]] \
    || { echo 'FAIL: safe cleanup unmounted a path other than the ESP mountpoint' >&2; cat "$FAKE_UMOUNT_LOG" >&2; exit 1; }
grep -Fq "$esp_dir /dev/test-outside ext4 ro 900" "$FAKE_FINDMNT_DB" \
    || { echo 'FAIL: safe cleanup unmounted the foreign mount' >&2; exit 1; }
grep -Fq "$esp_dir /dev/test-efi vfat rw 612" "$FAKE_FINDMNT_DB" \
    || { echo 'FAIL: safe cleanup unmounted the writable ESP mount' >&2; exit 1; }
[[ ! -s "$FAKE_MOUNT_LOG" ]] \
    || { echo 'FAIL: the ESP preflight mounted something' >&2; cat "$FAKE_MOUNT_LOG" >&2; exit 1; }

# 11e: successful rebuild after cleanup.  The fixture is already rw once the
# leaked layers are detached; the vendor stub writes a changed TUX.EFI and the
# rebuild reports the verified change.
printf 'old-image\n' > "$esp_dir/EFI/BOOT/TUX.EFI"
cat > "$FAKE_FINDMNT_DB" <<MNT
$esp_dir systemd-1 autofs rw 164
$esp_dir /dev/test-outside ext4 ro 900
$esp_dir /dev/test-efi vfat rw 612
$esp_dir /dev/test-efi vfat ro 1477
$esp_dir /dev/test-efi vfat ro 1416
MNT
printf '%s\n' "$esp_dir" > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_UMOUNT_LOG"
rebuild_ok="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=/dev/test-efi
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
validate_tuxedo_uki_target() { :; }
newest_tuxedo_kernel() { printf "6.1.0-tuxedo-amd64\n"; }
uefi_nvram_writable() { return 1; }
run_tuxedo_uki_builder() { printf "new-image\n" > "$TARGET_ROOT/boot/efi/EFI/BOOT/TUX.EFI"; }
rebuild_tuxedo_uki
')"
grep -Fq 'msg:esp-mount-cleanup|param:' <<<"$rebuild_ok" \
    || { echo 'FAIL: rebuild did not clear the leaked ESP layers' >&2; printf '%s\n' "$rebuild_ok" >&2; exit 1; }
grep -Fq 'msg:uki-vendor-changed' <<<"$rebuild_ok" \
    || { echo 'FAIL: rebuild did not report the changed TUX.EFI' >&2; printf '%s\n' "$rebuild_ok" >&2; exit 1; }
[[ "$(cat "$esp_dir/EFI/BOOT/TUX.EFI")" == 'new-image' ]] \
    || { echo 'FAIL: the vendor stub did not write TUX.EFI' >&2; exit 1; }
[[ "$(wc -l < "$FAKE_UMOUNT_LOG")" -eq 2 ]] \
    || { echo 'FAIL: rebuild did not clear exactly the two leaked layers' >&2; cat "$FAKE_UMOUNT_LOG" >&2; exit 1; }

# 11l: the vendor builder exports the hoisted target root UUID into the
# vendor command environment.  Inside a real chroot /proc is the bound host
# /proc, so the vendor script's findmnt-based resolution is host-tainted and
# the filtered chroot /dev makes its blkid fallback fail closed;
# HOST_ROOT_UUID keeps the vendor build deterministic.  Both the guarded and
# the unguarded command environments must carry it.
mkdir -p "$sandbox/target/usr/sbin" "$sandbox/target/usr/bin"
cat > "$sandbox/target/usr/sbin/create_boot_uki_base.sh" <<VENDOR
#!/bin/sh
env | grep '^HOST_ROOT_UUID=' > "$sandbox/vendor-env.txt" || true
env > "$sandbox/vendor-env-full.txt"
exit 0
VENDOR
chmod +x "$sandbox/target/usr/sbin/create_boot_uki_base.sh"

cat > "$FAKE_FINDMNT_DB" <<MNT
$esp_dir /dev/test-efi vfat rw 902
MNT
printf '%s\n' "$esp_dir" > "$FAKE_MOUNTPOINT_DB"
rm -f "$sandbox/target/usr/bin/efibootmgr"
: > "$sandbox/vendor-env.txt"
: > "$sandbox/vendor-env-full.txt"
builder_ok="$(run_harness '
run_selected_chroot() {
    # Emulate the chroot for the fixture: keep /usr/bin/env (it applies the
    # NAME=value words exactly as the real chroot invocation does) and point
    # the absolute vendor path at the fixture target copy.
    local -a args=("$@")
    local i
    for ((i = 0; i < ${#args[@]}; ++i)); do
        if [[ "${args[$i]}" == /usr/sbin/* ]]; then
            args[$i]="$TARGET_ROOT${args[$i]}"
        fi
    done
    "${args[@]}"
}
TARGET_ROOT="'"$sandbox"'/target"
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=/dev/test-efi
ROOT_CANONICAL=/dev/test-root
SESSION_DIR="'"$sandbox"'/session.hostrootuuid"
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
run_tuxedo_uki_builder "TUXEDO UKI vendor run (HOST_ROOT_UUID contract)" /usr/sbin/create_boot_uki_base.sh 6.1.0-tuxedo-amd64
')"
grep -Fq 'msg:vendor-command-success|param:TUXEDO UKI vendor run (HOST_ROOT_UUID contract)' <<<"$builder_ok" \
    || { echo 'FAIL: the vendor builder run failed (unguarded env)' >&2; printf '%s\n' "$builder_ok" >&2; exit 1; }
[[ "$(cat "$sandbox/vendor-env.txt")" == "HOST_ROOT_UUID=11111111-2222-3333-4444-555555555555" ]] \
    || { echo 'FAIL: HOST_ROOT_UUID was not exported with the target root UUID (unguarded env)' >&2; cat "$sandbox/vendor-env.txt" >&2; exit 1; }

# The guarded branch (target efibootmgr present) must export the same value
# and still remove the request-scoped guard directory afterwards.
cat > "$sandbox/target/usr/bin/efibootmgr" <<'EFIBOOTMGR'
#!/bin/sh
echo "stub efibootmgr $*"
exit 0
EFIBOOTMGR
chmod +x "$sandbox/target/usr/bin/efibootmgr"
: > "$sandbox/vendor-env.txt"
: > "$sandbox/vendor-env-full.txt"
builder_guard_ok="$(run_harness '
run_selected_chroot() {
    # Emulate the chroot for the fixture: keep /usr/bin/env (it applies the
    # NAME=value words exactly as the real chroot invocation does) and point
    # the absolute vendor path at the fixture target copy.
    local -a args=("$@")
    local i
    for ((i = 0; i < ${#args[@]}; ++i)); do
        if [[ "${args[$i]}" == /usr/sbin/* ]]; then
            args[$i]="$TARGET_ROOT${args[$i]}"
        fi
    done
    "${args[@]}"
}
TARGET_ROOT="'"$sandbox"'/target"
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=/dev/test-efi
ROOT_CANONICAL=/dev/test-root
SESSION_DIR="'"$sandbox"'/session.hostrootuuid-guard"
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
run_tuxedo_uki_builder "TUXEDO UKI vendor run (guarded env)" /usr/sbin/create_boot_uki_base.sh 6.1.0-tuxedo-amd64
')"
grep -Fq 'msg:vendor-command-success|param:TUXEDO UKI vendor run (guarded env)' <<<"$builder_guard_ok" \
    || { echo 'FAIL: the vendor builder run failed (guarded env)' >&2; printf '%s\n' "$builder_guard_ok" >&2; exit 1; }
[[ "$(cat "$sandbox/vendor-env.txt")" == "HOST_ROOT_UUID=11111111-2222-3333-4444-555555555555" ]] \
    || { echo 'FAIL: HOST_ROOT_UUID was not exported with the target root UUID (guarded env)' >&2; cat "$sandbox/vendor-env.txt" >&2; exit 1; }
grep -Fq 'PATH=/usr/local/libexec/boot-repair-efi-guard.session.hostrootuuid-guard:' "$sandbox/vendor-env-full.txt" \
    || { echo 'FAIL: the guarded env did not prepend the request-scoped EFI guard directory' >&2; exit 1; }
[[ ! -e "$sandbox/target/usr/local/libexec/boot-repair-efi-guard.session.hostrootuuid-guard" ]] \
    || { echo 'FAIL: the vendor builder left the request-scoped guard directory behind' >&2; exit 1; }

# 11m: the post-build root binding verification reuses the same hoisted UUID
# that was exported to the vendor builder, so the two can never diverge.  A
# diverging blkid probe (9999...) must not influence the check when the
# hoisted value is present, and the refusal names the hoisted UUID.
printf 'uki-image\n' > "$esp_dir/EFI/BOOT/TUX.EFI"
verify_reuse="$(run_harness '
need() { :; }
objcopy_dump_section() {
    local dest="${1#*=}"
    printf "root=UUID=11111111-2222-3333-4444-555555555555\n" > "$dest"
    return 0
}
blkid() { printf "99999999-8888-7777-6666-555555555555\n"; }
crypt_backing_device() { return 1; }
TARGET_ROOT="'"$sandbox"'/target"
ROOT_CANONICAL=/dev/test-root
TUXEDO_UKI_ROOT_UUID=11111111-2222-3333-4444-555555555555
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
verify_tuxedo_uki_root_binding
printf "VERIFY:OK\n"
')"
grep -Fqx 'VERIFY:OK' <<<"$verify_reuse" \
    || { echo 'FAIL: the binding verification did not reuse the hoisted root UUID' >&2; printf '%s\n' "$verify_reuse" >&2; exit 1; }
grep -Fq 'msg:uki-binding-verified' <<<"$verify_reuse" \
    || { echo 'FAIL: the binding verification did not report success' >&2; printf '%s\n' "$verify_reuse" >&2; exit 1; }

if verify_mismatch="$(run_harness '
need() { :; }
objcopy_dump_section() {
    local dest="${1#*=}"
    printf "root=UUID=99999999-8888-7777-6666-555555555555\n" > "$dest"
    return 0
}
crypt_backing_device() { return 1; }
TARGET_ROOT="'"$sandbox"'/target"
ROOT_CANONICAL=/dev/test-root
TUXEDO_UKI_ROOT_UUID=11111111-2222-3333-4444-555555555555
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
verify_tuxedo_uki_root_binding
' 2>&1)"; then
    echo 'FAIL: the binding verification accepted a cmdline that references a different root UUID' >&2
    printf '%s\n' "$verify_mismatch" >&2
    exit 1
fi
grep -Fq 'Rebuilt TUXEDO UKI does not reference the promoted root filesystem UUID 11111111-2222-3333-4444-555555555555.' <<<"$verify_mismatch" \
    || { echo 'FAIL: the binding refusal did not name the hoisted root UUID' >&2; printf '%s\n' "$verify_mismatch" >&2; exit 1; }

# 11m2: the promoted Btrfs subvolume match accepts both valid Btrfs spellings.
# The vendor builder's FINAL_SUBVOL strips the leading slash, so the rebuilt
# image carries `subvol=@`; a hand-assembled cmdline may keep the slashed form
# `subvol=/@`.  Both must pass in the rootflags= and bare-subvol positions
# (including a comma-joined rootflags= value), and a cmdline that selects no
# subvolume at all must still be refused with the promoted-root message.
verify_subvol_case()
{
    local label="$1" cmdline="$2" expect="$3" out="" rc=0
    # The negative case makes the harness exit 1, so the exit code is captured
    # under an if guard (errexit does not fire inside if conditions).
    if out="$(run_harness '
need() { :; }
objcopy_dump_section() {
    local dest="${1#*=}"
    printf "%s\n" "'"$cmdline"'" > "$dest"
    return 0
}
blkid() { printf "99999999-8888-7777-6666-555555555555\n"; }
lsblk() { printf "btrfs\n"; }
crypt_backing_device() { return 1; }
TARGET_ROOT="'"$sandbox"'/target"
ROOT_CANONICAL=/dev/test-root
TARGET_SUBVOL=@
TUXEDO_UKI_ROOT_UUID=11111111-2222-3333-4444-555555555555
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
verify_tuxedo_uki_root_binding
printf "VERIFY:OK\n"
' 2>&1)"; then
        rc=0
    else
        rc=$?
    fi
    if [[ "$expect" == pass ]]; then
        [[ $rc -eq 0 ]] \
            || { echo "FAIL: the promoted-subvolume check refused the valid $label form (exit $rc)" >&2; printf '%s\n' "$out" >&2; exit 1; }
        grep -Fqx 'VERIFY:OK' <<<"$out" \
            || { echo "FAIL: the promoted-subvolume check did not complete for the $label form" >&2; printf '%s\n' "$out" >&2; exit 1; }
        grep -Fq 'msg:uki-binding-verified' <<<"$out" \
            || { echo "FAIL: the promoted-subvolume check did not report success for the $label form" >&2; printf '%s\n' "$out" >&2; exit 1; }
    else
        if [[ $rc -eq 0 ]] || grep -Fqx 'VERIFY:OK' <<<"$out"; then
            echo "FAIL: the promoted-subvolume check accepted the $label" >&2
            printf '%s\n' "$out" >&2
            exit 1
        fi
        grep -Fq 'Rebuilt TUXEDO UKI does not explicitly select the promoted Btrfs @ root.' <<<"$out" \
            || { echo "FAIL: the promoted-subvolume refusal did not name the promoted root for the $label" >&2; printf '%s\n' "$out" >&2; exit 1; }
    fi
}

verify_subvol_case 'vendor no-slash form' 'root=UUID=11111111-2222-3333-4444-555555555555 subvol=@ rootflags=subvol=@' pass
verify_subvol_case 'slashed form' 'root=UUID=11111111-2222-3333-4444-555555555555 subvol=/@ rootflags=subvol=/@' pass
verify_subvol_case 'joined rootflags no-slash form' 'root=UUID=11111111-2222-3333-4444-555555555555 rootflags=rw,subvol=@' pass
verify_subvol_case 'joined rootflags slashed form' 'root=UUID=11111111-2222-3333-4444-555555555555 rootflags=rw,subvol=/@' pass
verify_subvol_case 'cmdline without any subvol selection' 'root=UUID=11111111-2222-3333-4444-555555555555 rw quiet' refuse

# 11f: host-path join.  With TARGET_ROOT="/" the real mount entry must resolve
# /boot (not //boot), record the pre-existing systemd mount and never mount
# over it.  /boot is used instead of /boot/efi because every rig (including the
# BIOS Alpine guest that runs this contract during the package check phase) has
# it, while a BIOS rig has no /boot/efi directory for the mount probe to stat.
cat > "$FAKE_FINDMNT_DB" <<MNT
/boot /dev/test-efi vfat rw 612
MNT
printf '/boot\n' > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_MOUNT_LOG"
join_out="$(run_harness '
TARGET_ROOT="/"
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
EFI_ESP_SOURCE=/dev/test-efi
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
fstab_entry_for_mountpoint() { printf "/dev/test-efi\tvfat\tdefaults\t0 2\n"; }
resolve_fstab_source() { printf "/dev/test-efi\n"; }
same_single_top_disk() { return 0; }
realpath_existing() { printf "%s\n" "$1"; }
eval "$real_mount_target_boot_entry"
mount_target_boot_entry "/boot" ro
printf "JOIN:%s\n" "$(target_path "/boot")"
printf "PREEXISTING:%s\n" "${PREEXISTING_MOUNTS[*]}"
')"
grep -Fqx 'JOIN:/boot' <<<"$join_out" \
    || { echo 'FAIL: the host-path join produced a double slash' >&2; printf '%s\n' "$join_out" >&2; exit 1; }
grep -Fqx 'PREEXISTING:/boot|/dev/test-efi|rw|612' <<<"$join_out" \
    || { echo 'FAIL: the pre-existing ESP mount was not recorded' >&2; printf '%s\n' "$join_out" >&2; exit 1; }
[[ ! -s "$FAKE_MOUNT_LOG" ]] \
    || { echo 'FAIL: mount_target_boot_entry mounted over the pre-existing mount' >&2; cat "$FAKE_MOUNT_LOG" >&2; exit 1; }

# 11g: unmounted fstab ESP.  The ESP is configured in the target fstab but not
# mounted; the preflight must mount it, re-run the writability probe and pass
# only after the new mount is verified as the ESP on the selected target disk.
cat > "$sandbox/target/etc/fstab" <<'FSTAB'
/dev/test-root  /          ext4  defaults  0 1
/dev/test-boot  /boot      ext4  defaults  0 2
/dev/test-efi   /boot/efi  vfat  defaults  0 2
FSTAB
: > "$FAKE_FINDMNT_DB"
: > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_MOUNT_LOG"
: > "$FAKE_SYSTEMCTL_LOG"
unmounted_ok="$(FAKE_MOUNT_ADD_POINT="$esp_dir" FAKE_MOUNT_ADD_ROW="$esp_dir /dev/test-efi vfat rw 900" run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_DISK=/dev/test-disk
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
esp_writable_preflight clear
')"
grep -Fq "msg:esp-mounted|param:$esp_dir|param:/dev/test-efi|param:mount" <<<"$unmounted_ok" \
    || { echo 'FAIL: unmounted fstab ESP was not auto-mounted with evidence' >&2; printf '%s\n' "$unmounted_ok" >&2; exit 1; }
grep -Fq "msg:esp-mount-preflight|param:$esp_dir|param:1|param:/dev/test-efi|param:rw|param:900|param:rw|param:0" <<<"$unmounted_ok" \
    || { echo 'FAIL: auto-mount did not re-run the writability probe on the new mount' >&2; printf '%s\n' "$unmounted_ok" >&2; exit 1; }
grep -Fq -- "-- $esp_dir" "$FAKE_MOUNT_LOG" \
    || { echo 'FAIL: auto-mount did not use the fstab-backed mount path' >&2; cat "$FAKE_MOUNT_LOG" >&2; exit 1; }
[[ "$(wc -l < "$FAKE_MOUNT_LOG")" -eq 1 ]] \
    || { echo 'FAIL: auto-mount mounted more than the configured ESP' >&2; cat "$FAKE_MOUNT_LOG" >&2; exit 1; }
[[ ! -s "$FAKE_SYSTEMCTL_LOG" ]] \
    || { echo 'FAIL: the target-scope auto-mount triggered systemd' >&2; cat "$FAKE_SYSTEMCTL_LOG" >&2; exit 1; }

# 11h: systemd automount trigger on the running host.  The fstab entry carries
# x-systemd.automount; the preflight must trigger the automount unit instead
# of calling mount, then verify the mount the kernel creates on access.  /boot
# is used for the same every-rig existence reason as 11f.
: > "$FAKE_FINDMNT_DB"
: > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_MOUNT_LOG"
: > "$FAKE_SYSTEMCTL_LOG"
automount_ok="$(FAKE_SYSTEMCTL_ADD_POINT=/boot FAKE_SYSTEMCTL_ADD_ROW='/boot /dev/test-efi vfat rw 901' run_harness '
TARGET_ROOT="/"
TARGET_DISK=/dev/test-disk
TARGET_ESP_MOUNT=/boot
EFI_ESP_SOURCE=
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
fstab_entry_for_mountpoint() { printf "/dev/test-efi\tvfat\tdefaults,x-systemd.automount\n"; }
esp_writable_preflight clear
')"
grep -Fq 'msg:esp-mounted|param:/boot|param:/dev/test-efi|param:automount' <<<"$automount_ok" \
    || { echo 'FAIL: the systemd automount was not triggered with evidence' >&2; printf '%s\n' "$automount_ok" >&2; exit 1; }
grep -Fq 'msg:esp-mount-preflight|param:/boot|param:1|param:/dev/test-efi|param:rw|param:901|param:rw|param:0' <<<"$automount_ok" \
    || { echo 'FAIL: the automount trigger did not re-run the writability probe' >&2; printf '%s\n' "$automount_ok" >&2; exit 1; }
grep -Fq 'start boot.automount' "$FAKE_SYSTEMCTL_LOG" \
    || { echo 'FAIL: the automount unit was not started' >&2; cat "$FAKE_SYSTEMCTL_LOG" >&2; exit 1; }
[[ ! -s "$FAKE_MOUNT_LOG" ]] \
    || { echo 'FAIL: the automount path called mount directly' >&2; cat "$FAKE_MOUNT_LOG" >&2; exit 1; }

# 11i: auto-mount failure refusal.  A configured ESP whose mount fails must
# fail closed with the auto-mount evidence and the actionable hint, never
# continue to the repair.
: > "$FAKE_FINDMNT_DB"
: > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_MOUNT_LOG"
: > "$FAKE_SYSTEMCTL_LOG"
if unmounted_fail="$(FAKE_MOUNT_FAIL=1 run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_DISK=/dev/test-disk
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
esp_writable_preflight clear
' 2>&1)"; then
    echo 'FAIL: the preflight passed although the ESP auto-mount failed' >&2
    exit 1
fi
grep -Fq "msg:esp-automount-failed|param:$esp_dir|param:mount $esp_dir failed|param:mount $esp_dir" <<<"$unmounted_fail" \
    || { echo 'FAIL: the auto-mount failure evidence line is missing or wrong' >&2; printf '%s\n' "$unmounted_fail" >&2; exit 1; }
grep -Fq "auto-mount did not succeed. Run 'mount $esp_dir'" <<<"$unmounted_fail" \
    || { echo 'FAIL: the preflight refusal does not carry the actionable hint' >&2; printf '%s\n' "$unmounted_fail" >&2; exit 1; }
[[ ! -s "$FAKE_SYSTEMCTL_LOG" ]] \
    || { echo 'FAIL: the failed target-scope auto-mount triggered systemd' >&2; cat "$FAKE_SYSTEMCTL_LOG" >&2; exit 1; }

# 11j: already-mounted ESP no-op.  A writable topmost ESP mount must pass
# without any mount or systemctl call.
cat > "$FAKE_FINDMNT_DB" <<MNT
$esp_dir /dev/test-efi vfat rw 902
MNT
printf '%s\n' "$esp_dir" > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_MOUNT_LOG"
: > "$FAKE_SYSTEMCTL_LOG"
mounted_noop="$(run_harness '
TARGET_ROOT="'"$sandbox"'/target"
TARGET_DISK=/dev/test-disk
TARGET_ESP_MOUNT=/boot/efi
EFI_ESP_SOURCE=/dev/test-efi
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
esp_writable_preflight check
')"
grep -Fq "msg:esp-mount-preflight|param:$esp_dir|param:1|param:/dev/test-efi|param:rw|param:902|param:rw|param:0" <<<"$mounted_noop" \
    || { echo 'FAIL: an already-mounted writable ESP did not pass the probe' >&2; printf '%s\n' "$mounted_noop" >&2; exit 1; }
if grep -Fq 'ESP mount:' <<<"$mounted_noop"; then
    echo 'FAIL: an already-mounted ESP emitted auto-mount evidence' >&2
    printf '%s\n' "$mounted_noop" >&2
    exit 1
fi
[[ ! -s "$FAKE_MOUNT_LOG" && ! -s "$FAKE_SYSTEMCTL_LOG" ]] \
    || { echo 'FAIL: an already-mounted ESP triggered mount/systemctl' >&2; exit 1; }

# 11k: cleanup leak evidence.  An unmount that fails must emit MOUNT_LEAK, keep
# the record in the process-independent state file and never be swallowed.
printf '%s\n' "$sandbox/target/home" > "$FAKE_MOUNTPOINT_DB"
printf '%s /dev/test-home xfs rw 77\n' "$sandbox/target/home" > "$FAKE_FINDMNT_DB"
rm -f "$sandbox/state/mount-leaks.log"
leak_out="$(FAKE_UMOUNT_FAIL="$sandbox/target/home" run_harness '
TARGET_ROOT="'"$sandbox"'/target"
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
MOUNTS=("'"$sandbox"'/target/home")
cleanup
' 2>&1 || true)"
grep -Fq "MOUNT_LEAK path=$sandbox/target/home source=/dev/test-home options=rw id=77" <<<"$leak_out" \
    || { echo 'FAIL: cleanup did not emit MOUNT_LEAK evidence' >&2; printf '%s\n' "$leak_out" >&2; exit 1; }
grep -Fq "MOUNT_LEAK path=$sandbox/target/home source=/dev/test-home options=rw id=77" "$sandbox/state/mount-leaks.log" \
    || { echo 'FAIL: the leaked mount was not persisted under STATE_ROOT' >&2; exit 1; }


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
# Part 13: A2-01/A4-01/A4-08 fstab mount-destination containment.  A hostile
# Btrfs subvolume mountpoint (".." traversal or a symlinked destination) is
# refused with a WARNING and never reaches mkdir/mount, while the legit
# subvolume still mounts.  context=/seclabel never pass through and
# nosuid,nodev is appended last for both data-partition and subvolume mounts.
# ---------------------------------------------------------------------------
printf 'test-disk - disk - - - -\n' > "$FAKE_LSBLK_DB"
printf 'test-root btrfs part 11111111-2222-3333-4444-555555555555 test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
printf 'test-var ext4 part 66666666-7777-8888-9999-aaaaaaaaaaaa test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
printf 'test-home xfs part BBBBBBBB-CCCC-DDDD-EEEE-FFFFFFFFFFFF test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
touch "$sandbox/dev-test-root" "$sandbox/dev-test-var" "$sandbox/dev-test-home"
mkdir -p "$sandbox/target/srv" "$sandbox/host-outside"
# Earlier parts created /home as a real directory; the hostile fixture needs
# it gone so the symlink can take its place.
rm -rf "$sandbox/target/home" "$sandbox/target/usr"
ln -s "$sandbox/host-outside" "$sandbox/target/home"
cat > "$sandbox/target/etc/fstab" <<'FSTAB'
/dev/test-root  /               btrfs  subvol=/@    0 0
/dev/test-root  /../../outside  btrfs  subvol=/@evil 0 0
/dev/test-root  /home           btrfs  subvol=/@home 0 0
/dev/test-root  /srv            btrfs  subvol=/@srv,context=system_u:object_r:etc_t:s0,seclabel 0 0
FSTAB
: > "$FAKE_MOUNT_LOG"
: > "$FAKE_MOUNTPOINT_DB"
subvol_containment="$(run_harness '
ROOT_CANONICAL=/dev/test-root
TARGET_DISK=/dev/test-disk
TARGET_ROOT="'"$sandbox"'/target"
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
TARGET_DATA_MOUNTS=()
mount_target_btrfs_subvolumes ro
printf "MOUNTS:%s\n" "$(printf "%s," "${TARGET_DATA_MOUNTS[@]:-}")"')"
grep -Fq 'msg:refuse-unsafe-btrfs-path|param:'"$sandbox"'/target/../../outside' <<<"$subvol_containment" \
    || { echo 'FAIL: the ".." Btrfs subvolume mountpoint was not refused with a warning' >&2; printf '%s\n' "$subvol_containment" >&2; exit 1; }
grep -Fq 'msg:refuse-unsafe-btrfs-path|param:'"$sandbox"'/target/home' <<<"$subvol_containment" \
    || { echo 'FAIL: the symlinked Btrfs subvolume mountpoint was not refused with a warning' >&2; printf '%s\n' "$subvol_containment" >&2; exit 1; }
grep -Fq "$sandbox/target/srv," <<<"$subvol_containment" \
    || { echo 'FAIL: the legitimate Btrfs subvolume was not mounted' >&2; printf '%s\n' "$subvol_containment" >&2; exit 1; }
grep -Fq -- '-o ro,subvol=/@srv,nosuid,nodev -- /dev/test-root '"$sandbox"'/target/srv' "$FAKE_MOUNT_LOG" \
    || { echo 'FAIL: the subvolume mount did not drop context=/seclabel and append nosuid,nodev' >&2; cat "$FAKE_MOUNT_LOG" >&2; exit 1; }
if grep -Fq 'context=' "$FAKE_MOUNT_LOG" || grep -Fq 'seclabel' "$FAKE_MOUNT_LOG" \
   || grep -Fq '@evil' "$FAKE_MOUNT_LOG" || grep -Fq '/home' "$FAKE_MOUNT_LOG"; then
    echo 'FAIL: a hostile or SELinux-labeled subvolume mount reached the mount command' >&2
    cat "$FAKE_MOUNT_LOG" >&2
    exit 1
fi
[[ -z "$(ls -A "$sandbox/host-outside" 2>/dev/null)" ]] \
    || { echo 'FAIL: the symlinked subvolume mountpoint redirected a write outside the target' >&2; exit 1; }

# Data partitions: the symlinked /usr destination is refused, the legitimate
# /var entry keeps its passthrough options but loses context= and gains the
# appended nosuid,nodev after the fstab suid,dev (rightmost wins).
ln -s "$sandbox/host-outside" "$sandbox/target/usr"
printf 'test-usr ext4 part USR-UUID test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
touch "$sandbox/dev-test-usr"
cat > "$sandbox/target/etc/fstab" <<'FSTAB'
/dev/test-root  /     btrfs  subvol=/@  0 0
/dev/test-var   /var  ext4   defaults,context=system_u:object_r:var_t:s0,suid,dev  0 2
/dev/test-home  /home xfs    defaults  0 2
/dev/test-usr   /usr  ext4   defaults  0 2
FSTAB
: > "$FAKE_MOUNT_LOG"
: > "$FAKE_MOUNTPOINT_DB"
data_containment="$(run_harness '
ROOT_CANONICAL=/dev/test-root
TARGET_DISK=/dev/test-disk
TARGET_ROOT="'"$sandbox"'/target"
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
TARGET_DATA_MOUNTS=()
mount_target_data_partitions ro
printf "MOUNTS:%s\n" "$(printf "%s," "${TARGET_DATA_MOUNTS[@]:-}")"')"
grep -Fq 'msg:refuse-unsafe-mount-path|param:/usr|param:'"$sandbox"'/target/usr' <<<"$data_containment" \
    || { echo 'FAIL: the symlinked data-partition destination was not refused with a warning' >&2; printf '%s\n' "$data_containment" >&2; exit 1; }
grep -Fq "$sandbox/target/var," <<<"$data_containment" \
    || { echo 'FAIL: the legitimate data partition was not mounted' >&2; printf '%s\n' "$data_containment" >&2; exit 1; }
grep -Fq -- '-o ro,suid,dev,nosuid,nodev -- /dev/test-var '"$sandbox"'/target/var' "$FAKE_MOUNT_LOG" \
    || { echo 'FAIL: the data mount did not drop context= and append nosuid,nodev after the fstab suid,dev' >&2; cat "$FAKE_MOUNT_LOG" >&2; exit 1; }
if grep -Fq 'context=' "$FAKE_MOUNT_LOG" || grep -Fq 'seclabel' "$FAKE_MOUNT_LOG"; then
    echo 'FAIL: a SELinux label option leaked into the data mount passthrough' >&2
    cat "$FAKE_MOUNT_LOG" >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# Part 14: A2-05 offline repair never lazy-detaches.  A plain umount failure
# leaves MOUNT_LEAK evidence and returns 2; `umount -l` is never invoked.
# ---------------------------------------------------------------------------
: > "$FAKE_FINDMNT_DB"
printf '%s /dev/test-home xfs rw 77\n' "$sandbox/target/home" > "$FAKE_FINDMNT_DB"
printf '%s\n' "$sandbox/target/home" > "$FAKE_MOUNTPOINT_DB"
: > "$sandbox/umount-args.log"
rm -f "$sandbox/state/mount-leaks.log"
release_fail="$(run_harness '
umount() { printf "%s\n" "$*" >> "'"$sandbox"'/umount-args.log"; return 1; }
TARGET_ROOT="'"$sandbox"'/target"
MOUNT_BASE="'"$sandbox"'/target"
ROOT_DEVICE=/dev/test-root
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
MOUNTS=("'"$sandbox"'/target/home")
set +e
filesystem_release_mounts_for_device /dev/test-home
release_rc=$?
set -e
printf "RELEASE_RC:%s\n" "$release_rc"
' 2>&1 || true)"
grep -Fq 'RELEASE_RC:2' <<<"$release_fail" \
    || { echo 'FAIL: a failed offline umount did not fail closed with rc 2' >&2; printf '%s\n' "$release_fail" >&2; exit 1; }
grep -Fq "MOUNT_LEAK path=$sandbox/target/home source=/dev/test-home options=rw id=77" <<<"$release_fail" \
    || { echo 'FAIL: the offline umount failure did not leave MOUNT_LEAK evidence' >&2; printf '%s\n' "$release_fail" >&2; exit 1; }
[[ "$(cat "$sandbox/umount-args.log")" == "$sandbox/target/home" ]] \
    || { echo 'FAIL: the offline release path invoked umount -l or extra umounts' >&2; cat "$sandbox/umount-args.log" >&2; exit 1; }
grep -Fq "MOUNT_LEAK path=$sandbox/target/home source=/dev/test-home options=rw id=77" "$sandbox/state/mount-leaks.log" \
    || { echo 'FAIL: the offline umount failure was not persisted under STATE_ROOT' >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 15: A4-06 same-disk scope gate.  A fstab /home on another disk is
# skipped with a WARNING at resolution time and its tool is never invoked.
# ---------------------------------------------------------------------------
printf 'test-disk - disk - - - -\n' > "$FAKE_LSBLK_DB"
printf 'test-root ext4 part 11111111-2222-3333-4444-555555555555 test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
printf 'test-boot ext4 part 66666666-7777-8888-9999-aaaaaaaaaaaa test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
printf 'test-efi vfat part ABCD-1234 test-disk 1 test-disk\n' >> "$FAKE_LSBLK_DB"
printf 'test-outside ext4 part OUTSIDE-UUID other-disk 1 other-disk\n' >> "$FAKE_LSBLK_DB"
touch "$sandbox/dev-test-outside"
rm -f "$sandbox/target/usr"
cat > "$sandbox/target/etc/fstab" <<'FSTAB'
/dev/test-root     /         ext4  defaults  0 1
/dev/test-boot     /boot     ext4  defaults  0 2
/dev/test-efi      /boot/efi vfat  defaults  0 2
/dev/test-outside  /home     ext4  defaults  0 2
FSTAB
: > "$FAKE_TOOL_LOG"
scope_gate="$(run_harness '
top_disks_for() {
    case "$1" in
        */test-outside) printf "%s\n" "'"$sandbox"'/dev-other-disk" ;;
        *) printf "%s\n" "'"$sandbox"'/dev-test-disk" ;;
    esac
}
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT="'"$sandbox"'/target"
SESSION_LOG="'"$sandbox"'/session.log"
: > "$SESSION_LOG"
filesystem_scope_resolve
printf "SCOPE:%s\n" "${FS_SCOPE_DEVICES[*]}"')"
grep -Fq 'msg:fstab-not-on-disk|param:/home|param:/dev/test-outside|param:/dev/test-disk' <<<"$scope_gate" \
    || { echo 'FAIL: the out-of-disk fstab /home was not skipped with a warning' >&2; printf '%s\n' "$scope_gate" >&2; exit 1; }
grep -Fq 'SCOPE:/dev/test-root /dev/test-boot /dev/test-efi' <<<"$scope_gate" \
    || { echo 'FAIL: the same-disk scope devices were not resolved' >&2; printf '%s\n' "$scope_gate" >&2; exit 1; }
if grep -Fq '/dev/test-outside' <<<"$(grep -F 'SCOPE:' <<<"$scope_gate")"; then
    echo 'FAIL: the out-of-disk /home device entered the repair scope' >&2
    exit 1
fi
gate_inspect="$(run_harness '
top_disks_for() {
    case "$1" in
        */test-outside) printf "%s\n" "'"$sandbox"'/dev-other-disk" ;;
        *) printf "%s\n" "'"$sandbox"'/dev-test-disk" ;;
    esac
}
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
TARGET_OS_ID=arch
TARGET_OS_LIKE=""
TARGET_DISTRO_FAMILY=arch
TARGET_ROOT=""
fs_inspect /dev/test-disk /dev/test-root
' 2>&1 || true)"
if grep -Fq '/dev/test-outside' "$FAKE_TOOL_LOG"; then
    echo 'FAIL: the out-of-disk fstab device was handed to a filesystem tool' >&2
    cat "$FAKE_TOOL_LOG" >&2
    exit 1
fi
grep -Fq 'File system check summary: devices=3 clean=3 issues=0 unsupported=0 tool-missing=0 skipped=0' <<<"$gate_inspect" \
    || { echo 'FAIL: the gated inspection scope is not the three same-disk devices' >&2; printf '%s\n' "$gate_inspect" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 16: A4-04 LUKS header UUID validation.  A malformed header UUID is
# refused before any luks-<UUID> mapper name is composed; a canonical UUID
# composes the mapper name.
# ---------------------------------------------------------------------------
if luks_bad="$(run_harness '
need() { :; }
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
assert_target_not_host() { :; }
same_single_top_disk() { return 0; }
lsblk() { printf "crypto_LUKS\n"; }
cryptsetup() {
    if [[ "${1:-}" == luksUUID ]]; then printf "not-a-uuid/../../etc\n"; fi
    return 0
}
unlock_target
' 2>&1)"; then
    echo 'FAIL: a malformed LUKS header UUID was accepted' >&2
    exit 1
fi
grep -Fq 'The LUKS header reported a malformed UUID; refusing to unlock: not-a-uuid/../../etc' <<<"$luks_bad" \
    || { echo 'FAIL: the malformed LUKS UUID refusal reason is missing' >&2; printf '%s\n' "$luks_bad" >&2; exit 1; }
if grep -Fq 'luks-not-a-uuid' <<<"$luks_bad"; then
    echo 'FAIL: a malformed LUKS UUID reached the mapper-name composition' >&2
    exit 1
fi

if luks_ok="$(run_harness '
need() { :; }
TARGET_DISK=/dev/test-disk
ROOT_DEVICE=/dev/test-root
assert_target_not_host() { :; }
same_single_top_disk() { return 0; }
lsblk() { printf "crypto_LUKS\n"; }
cryptsetup() {
    case "${1:-}" in
        luksUUID) printf "11111111-2222-3333-4444-555555555555\n" ;;
        *) return 0 ;;
    esac
    return 0
}
find_crypt_mapper_for_device() { return 1; }
unlock_target
' 2>&1)"; then
    echo 'FAIL: a canonical LUKS UUID unlock was refused' >&2
    printf '%s\n' "$luks_ok" >&2
    exit 1
fi
grep -Fq 'msg:luks-mapper-name|param:luks-11111111-2222-3333-4444-555555555555' <<<"$luks_ok" \
    || { echo 'FAIL: the canonical LUKS UUID did not compose the luks-<UUID> mapper name' >&2; printf '%s\n' "$luks_ok" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 17: A4-02 repair-to-host File Copy strips setuid/setgid and drops file
# capabilities.  The transfer carries --chmod=u-s,g-s and the
# security.capability xattr filter when the probed rsync supports them, the
# post-copy scan fails closed on either bit class, and the verification masks
# modes exactly like the copy so a stripped destination reports no phantom
# diff.  Host-to-repair keeps -aHAX with an explicit log note.
# ---------------------------------------------------------------------------
grep -q '^rsync_supports_chmod()' "$HELPER" \
    || { echo 'FAIL: the rsync --chmod support probe is missing' >&2; exit 1; }
grep -q '^rsync_supports_xattr_filter()' "$HELPER" \
    || { echo 'FAIL: the rsync xattr-filter support probe is missing' >&2; exit 1; }
grep -q '^reassert_host_destination()' "$HELPER" \
    || { echo 'FAIL: the repair-to-host destination reassert helper is missing' >&2; exit 1; }
grep -q '^host_copy_security_scan()' "$HELPER" \
    || { echo 'FAIL: the repair-to-host post-copy security scan is missing' >&2; exit 1; }
file_copy_block="$(sed -n '/^run_file_copy()/,/^}/p' "$HELPER")"
rsync_block="$(sed -n '/^run_rsync_item()/,/^}/p' "$HELPER")"
verify_block="$(sed -n '/^verify_rsync_item()/,/^}/p' "$HELPER")"
scan_block="$(sed -n '/^host_copy_security_scan()/,/^}/p' "$HELPER")"
grep -Fq -- '--chmod=u-s,g-s' <<<"$rsync_block" \
    || { echo 'FAIL: repair-to-host rsync does not strip setuid/setgid with --chmod' >&2; exit 1; }
grep -Fq -- "-f '-x security.capability'" <<<"$rsync_block" \
    || { echo 'FAIL: repair-to-host rsync does not drop security.capability with an xattr filter' >&2; exit 1; }
grep -Fq -- '--chmod=u-s,g-s' <<<"$verify_block" \
    || { echo 'FAIL: the post-copy verification does not mask setuid/setgid modes' >&2; exit 1; }
grep -Fq -- "-f '-x security.capability'" <<<"$verify_block" \
    || { echo 'FAIL: the post-copy verification does not filter security.capability' >&2; exit 1; }
grep -Fq -- '-perm /6000' <<<"$scan_block" \
    || { echo 'FAIL: the post-copy scan does not use find -perm /6000' >&2; exit 1; }
grep -Fq -- 'getfattr -R -m security.capability' <<<"$scan_block" \
    || { echo 'FAIL: the post-copy scan does not check security.capability with getfattr' >&2; exit 1; }
grep -Fq -- 'getcap -r' <<<"$scan_block" \
    || { echo 'FAIL: the post-copy scan lacks the getcap fallback' >&2; exit 1; }
grep -Fq 'cannot verify that file capabilities were removed' <<<"$scan_block" \
    || { echo 'FAIL: the scan does not fail with a clear message when xattr tooling and the rsync filter are both unavailable' >&2; exit 1; }
grep -Fq 'run_rsync_item "$mode" "$source" "$destination" "$chown_value" "$strip_setid"' <<<"$file_copy_block" \
    || { echo 'FAIL: the strip flag is not plumbed into the File Copy rsync' >&2; exit 1; }
grep -Fq 'verify_rsync_item "$source" "$destination" "$chown_value" "$strip_setid"' <<<"$file_copy_block" \
    || { echo 'FAIL: the strip flag is not plumbed into the File Copy verification' >&2; exit 1; }
grep -Fq 'host_copy_security_scan "$destination/$(basename -- "$source")"' <<<"$file_copy_block" \
    || { echo 'FAIL: the post-copy security scan is not wired into File Copy' >&2; exit 1; }
grep -Fq 'msg_log security-strip-on-repair-host' <<<"$file_copy_block" \
    || { echo 'FAIL: the File Copy preview/summary text does not state that repair-to-host removes setuid/setgid/capabilities' >&2; exit 1; }
grep -Fq 'msg_log security-keep-on-host-repair' <<<"$file_copy_block" \
    || { echo 'FAIL: the host-to-repair trusted -aHAX note is missing' >&2; exit 1; }

# Behavioural, non-root: a 4755 source file must arrive as 0755, the masked
# verification must report no difference, and the post-copy scan must accept
# the stripped tree and refuse a setuid-bearing one.  Uses the real host
# rsync, so the probe functions must agree it supports both options.
fc_root="$(mktemp -d)"
mkdir -p "$fc_root/src" "$fc_root/dst"
printf 'binary\n' > "$fc_root/src/tool"
chmod 4755 "$fc_root/src/tool"
printf 'plain\n' > "$fc_root/src/doc"
chmod 0644 "$fc_root/src/doc"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_DIR="$fc_root/session"
    mkdir -p "$SESSION_DIR"
    SESSION_LOG="$fc_root/session.log"
    : > "$SESSION_LOG"
    rsync_supports_chmod || exit 91
    rsync_supports_xattr_filter || exit 92
    run_rsync_item copy "$fc_root/src/tool" "$fc_root/dst" "" yes
    run_rsync_item copy "$fc_root/src/doc" "$fc_root/dst" "" yes
    verify_rsync_item "$fc_root/src/tool" "$fc_root/dst" "" yes
    verify_rsync_item "$fc_root/src/doc" "$fc_root/dst" "" yes
    host_copy_security_scan "$fc_root/dst/tool"
    host_copy_security_scan "$fc_root/dst/doc"
) > "$fc_root/copy.log" 2>&1
fc_rc=$?
if (( fc_rc != 0 )); then
    echo "FAIL: repair-to-host strip/verify/scan path failed (rc $fc_rc)" >&2
    cat "$fc_root/copy.log" >&2
    exit 1
fi
[[ "$(stat -c '%a' -- "$fc_root/dst/tool")" == 755 ]] \
    || { echo "FAIL: a 4755 source arrived as $(stat -c '%a' -- "$fc_root/dst/tool"), setuid/setgid was not stripped" >&2; exit 1; }
[[ "$(stat -c '%a' -- "$fc_root/dst/doc")" == 644 ]] \
    || { echo 'FAIL: the plain-file mode was not preserved' >&2; exit 1; }
mkdir -p "$fc_root/bad"
printf 'x\n' > "$fc_root/bad/tool"
chmod 4755 "$fc_root/bad/tool"
set +e
scan_fail_out="$(
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        SESSION_DIR="$fc_root/session"
        SESSION_LOG="$fc_root/session.log"
        host_copy_security_scan "$fc_root/bad"
    ) 2>&1
)"
set -e
grep -Fq 'found setuid/setgid bits in the copied files; the copy is refused' <<<"$scan_fail_out" \
    || { echo 'FAIL: the post-copy scan accepted a setuid-bearing copy' >&2; printf '%s\n' "$scan_fail_out" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 18: A4-03 repair-to-host destination TOCTOU and permission hardening.
# validate_host_destination refuses sticky and world-writable directories,
# only warns on group-writable ones, and the destination is re-validated
# immediately before each rsync and before the verification, failing when the
# realpath changed.
# ---------------------------------------------------------------------------
grep -Fq 'must not be sticky' "$HELPER" \
    || { echo 'FAIL: the sticky destination refusal is missing' >&2; exit 1; }
grep -Fq 'must not be world-writable' "$HELPER" \
    || { echo 'FAIL: the world-writable destination refusal is missing' >&2; exit 1; }
grep -Fq 'msg_log host-dest-group-writable' "$HELPER" \
    || { echo 'FAIL: the group-writable destination is not a warning' >&2; exit 1; }
grep -Fq 'and must not be sticky or world-writable' "$HELPER" \
    || { echo 'FAIL: the GUI-visible destination allowlist wording does not name the permission requirement' >&2; exit 1; }
grep -Fq 'changed during the copy (TOCTOU)' "$HELPER" \
    || { echo 'FAIL: the destination-change refusal message is missing' >&2; exit 1; }
awk '
    BEGIN { n = 0 }
    index($0, "reassert_host_destination \"$destination_virtual\" \"$destination\"") { n++; reassert[n] = NR }
    index($0, "run_rsync_item \"$mode\"") { rsync_line = NR }
    index($0, "verify_rsync_item \"$source\"") { verify_line = NR }
    END {
        if (n < 2 || reassert[1] > rsync_line || rsync_line > reassert[2] || reassert[2] > verify_line) exit 1
    }
' <<<"$file_copy_block" \
    || { echo 'FAIL: the repair-to-host destination is not re-validated before rsync and before verification' >&2; exit 1; }

fc_dest="$fc_root/host-dest"
mkdir -p "$fc_dest"
chmod 0755 "$fc_dest"
dest_ok="$( (
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$fc_root/session.log"
    validate_host_destination "$fc_dest"
) 2>&1 || true)"
grep -Fxq "$(readlink -f -- "$fc_dest")" <<<"$dest_ok" \
    || { echo 'FAIL: a clean host destination was refused' >&2; printf '%s\n' "$dest_ok" >&2; exit 1; }

chmod 0777 "$fc_dest"
if world_out="$( (
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$fc_root/session.log"
    validate_host_destination "$fc_dest"
) 2>&1)"; then
    echo 'FAIL: a world-writable host destination was accepted' >&2
    exit 1
fi
grep -Fq 'must not be world-writable' <<<"$world_out" \
    || { echo 'FAIL: the world-writable refusal reason is missing' >&2; exit 1; }

chmod 1777 "$fc_dest"
if sticky_out="$( (
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$fc_root/session.log"
    validate_host_destination "$fc_dest"
) 2>&1)"; then
    echo 'FAIL: a sticky host destination was accepted' >&2
    exit 1
fi
grep -Fq 'must not be sticky' <<<"$sticky_out" \
    || { echo 'FAIL: the sticky refusal reason is missing' >&2; exit 1; }

chmod 0775 "$fc_dest"
group_out="$( (
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$fc_root/session.log"
    validate_host_destination "$fc_dest"
) 2>&1 || true)"
grep -Fq 'group-writable' <<<"$group_out" \
    || { echo 'FAIL: a group-writable host destination did not warn' >&2; printf '%s\n' "$group_out" >&2; exit 1; }
grep -Fxq "$(readlink -f -- "$fc_dest")" <<<"$group_out" \
    || { echo 'FAIL: a group-writable host destination was refused instead of warned' >&2; printf '%s\n' "$group_out" >&2; exit 1; }
chmod 0755 "$fc_dest"

# TOCTOU: a destination whose revalidation resolves elsewhere fails the
# reassert; a stable destination passes it.
toctou_out="$(
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        SESSION_LOG="$fc_root/session.log"
        validate_host_destination() { printf '%s\n' "$fc_root/host-dest-elsewhere"; }
        reassert_host_destination "$fc_dest" "$fc_dest"
    ) 2>&1 || true
)"
grep -Fq 'changed during the copy (TOCTOU)' <<<"$toctou_out" \
    || { echo 'FAIL: a swapped repair-to-host destination was not refused on revalidation' >&2; printf '%s\n' "$toctou_out" >&2; exit 1; }
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_LOG="$fc_root/session.log"
    validate_host_destination() { printf '%s\n' "$fc_dest"; }
    reassert_host_destination "$fc_dest" "$fc_dest"
) \
    || { echo 'FAIL: a stable repair-to-host destination failed the revalidation reassert' >&2; exit 1; }
reval_fail_out="$(
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        SESSION_LOG="$fc_root/session.log"
        validate_host_destination() { return 1; }
        reassert_host_destination "$fc_dest" "$fc_dest"
    ) 2>&1 || true
)"
grep -Fq 'failed revalidation' <<<"$reval_fail_out" \
    || { echo 'FAIL: a failing destination revalidation was not reported' >&2; printf '%s\n' "$reval_fail_out" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Part 19: SHA-256 verification robustness.  verify_sha256_item() prints one
# bare integer on stdout for every outcome (the verified count; 0 on failure)
# and returns nonzero when a listed file failed, so a source lost mid-copy can
# never crash the File Copy arithmetic.  run_file_copy counts failures, names
# them in the final summary and fails closed instead of reporting a silently
# "complete" copy with 0 verified files.
# ---------------------------------------------------------------------------
grep -q '^sha256_verify_failure()' "$HELPER" \
    || { echo 'FAIL: the SHA-256 verification failure marker helper is missing' >&2; exit 1; }
sha_v_block="$(sed -n '/^verify_sha256_item()/,/^}/p' "$HELPER")"
grep -Fq "printf '0\\n'" <<<"$sha_v_block" \
    || { echo 'FAIL: verify_sha256_item no longer prints a bare 0 on failure paths' >&2; exit 1; }
grep -Fq 'sha256_verify_failure' <<<"$sha_v_block" \
    || { echo 'FAIL: verify_sha256_item no longer routes failures through the error reporter' >&2; exit 1; }
grep -Fq 'if ! verified="$(verify_sha256_item "$source" "$destination")"; then' <<<"$file_copy_block" \
    || { echo 'FAIL: the File Copy loop does not capture the verification status robustly' >&2; exit 1; }
grep -Fq '[[ "$verified" =~ ^[0-9]+$ ]] || verified=0' <<<"$file_copy_block" \
    || { echo 'FAIL: the File Copy verification count is not regex-guarded' >&2; exit 1; }
grep -Fq 'msg_log copy-sha-failures' <<<"$file_copy_block" \
    || { echo 'FAIL: the File Copy summary does not name verification failures' >&2; exit 1; }

# (b) verify_sha256_item stdout is always a bare integer; nonzero status only
# on failure.  Success file/dir, vanished source, missing/wrong-type
# destination, content mismatch and a file that disappears mid-hash.
sha_v_root="$fc_root/sha-verify"
mkdir -p "$sha_v_root/session"
: > "$sha_v_root/session.log"
sha_v_call() {
    # Run verify_sha256_item in a clean helper context; stdout lands in
    # sha_v_out, the function's status becomes sha_v_call's status.  The
    # caller owns set -e restoration: this function must not re-enable it
    # before returning a nonzero status (functions run in the caller's shell).
    local _rc
    set +e
    sha_v_out="$(
        SHAV_VANISH_AT="${SHAV_VANISH_AT:-}" bash -c '
            source <(sed '\''/^main "\$@"/d'\'' "$1")
            trap - EXIT INT TERM HUP
            SESSION_DIR="$2/session"
            SESSION_LOG="$2/session.log"
            : > "$SESSION_LOG"
            if [[ -n "${SHAV_VANISH_AT:-}" ]]; then
                sha256sum()
                {
                    local arg
                    for arg in "$@"; do
                        if [[ "$arg" == "$SHAV_VANISH_AT" ]]; then
                            rm -f -- "$SHAV_VANISH_AT"
                            break
                        fi
                    done
                    command sha256sum "$@"
                }
            fi
            verify_sha256_item "${@:4}"
        ' _ "$HELPER" "$sha_v_root" "$@" 2>/dev/null
    )"
    _rc=$?
    return $_rc
}

mkdir -p "$sha_v_root/one/src" "$sha_v_root/one/dst"
printf 'alpha\n' > "$sha_v_root/one/src/f"
cp -p "$sha_v_root/one/src/f" "$sha_v_root/one/dst/f"
set +e
sha_v_call verify_sha256_item "$sha_v_root/one/src/f" "$sha_v_root/one/dst"
sha_v_rc=$?
set -e
[[ "$sha_v_rc" -eq 0 && "$sha_v_out" == 1 ]] \
    || { echo "FAIL: successful single-file verification returned rc=$sha_v_rc stdout='$sha_v_out' (expected rc=0 stdout=1)" >&2; exit 1; }

mkdir -p "$sha_v_root/two/src/d" "$sha_v_root/two/dst/d"
printf 'beta\n' > "$sha_v_root/two/src/d/a"
printf 'gamma\n' > "$sha_v_root/two/src/d/b"
cp -p "$sha_v_root/two/src/d/a" "$sha_v_root/two/dst/d/a"
cp -p "$sha_v_root/two/src/d/b" "$sha_v_root/two/dst/d/b"
set +e
sha_v_call verify_sha256_item "$sha_v_root/two/src/d" "$sha_v_root/two/dst"
sha_v_rc=$?
set -e
[[ "$sha_v_rc" -eq 0 && "$sha_v_out" == 2 ]] \
    || { echo "FAIL: successful directory verification returned rc=$sha_v_rc stdout='$sha_v_out' (expected rc=0 stdout=2)" >&2; exit 1; }

mkdir -p "$sha_v_root/three/src" "$sha_v_root/three/dst"
set +e
sha_v_call verify_sha256_item "$sha_v_root/three/src/gone" "$sha_v_root/three/dst"
sha_v_rc=$?
set -e
[[ "$sha_v_rc" -eq 1 && "$sha_v_out" == 0 ]] \
    || { echo "FAIL: a vanished source returned rc=$sha_v_rc stdout='$sha_v_out' (expected rc=1 stdout=0)" >&2; exit 1; }
grep -Fq 'the source no longer exists' "$sha_v_root/session.log" \
    || { echo 'FAIL: the vanished source failure was not recorded in the session log' >&2; exit 1; }

mkdir -p "$sha_v_root/four/src" "$sha_v_root/four/dst"
printf 'delta\n' > "$sha_v_root/four/src/f"
set +e
sha_v_call verify_sha256_item "$sha_v_root/four/src/f" "$sha_v_root/four/dst"
sha_v_rc=$?
set -e
[[ "$sha_v_rc" -eq 1 && "$sha_v_out" == 0 ]] \
    || { echo "FAIL: a missing destination returned rc=$sha_v_rc stdout='$sha_v_out' (expected rc=1 stdout=0)" >&2; exit 1; }
grep -Fq 'destination file missing or not a regular file' "$sha_v_root/session.log" \
    || { echo 'FAIL: the missing-destination failure was not recorded in the session log' >&2; exit 1; }

mkdir -p "$sha_v_root/five/src" "$sha_v_root/five/dst"
printf 'epsilon\n' > "$sha_v_root/five/src/f"
printf 'different\n' > "$sha_v_root/five/dst/f"
set +e
sha_v_call verify_sha256_item "$sha_v_root/five/src/f" "$sha_v_root/five/dst"
sha_v_rc=$?
set -e
[[ "$sha_v_rc" -eq 1 && "$sha_v_out" == 0 ]] \
    || { echo "FAIL: a content mismatch returned rc=$sha_v_rc stdout='$sha_v_out' (expected rc=1 stdout=0)" >&2; exit 1; }
grep -Fq 'SHA-256 verification failed:' "$sha_v_root/session.log" \
    || { echo 'FAIL: the hash-mismatch failure was not recorded in the session log' >&2; exit 1; }

mkdir -p "$sha_v_root/six/src/d" "$sha_v_root/six/dst/d"
printf 'zeta\n' > "$sha_v_root/six/src/d/keep"
printf 'vanishes\n' > "$sha_v_root/six/src/d/vanishes"
cp -p "$sha_v_root/six/src/d/keep" "$sha_v_root/six/dst/d/keep"
cp -p "$sha_v_root/six/src/d/vanishes" "$sha_v_root/six/dst/d/vanishes"
set +e
SHAV_VANISH_AT="$sha_v_root/six/src/d/vanishes" \
    sha_v_call verify_sha256_item "$sha_v_root/six/src/d" "$sha_v_root/six/dst"
sha_v_rc=$?
set -e
SHAV_VANISH_AT=""
[[ "$sha_v_rc" -eq 1 && "$sha_v_out" == 1 ]] \
    || { echo "FAIL: a mid-hash source loss returned rc=$sha_v_rc stdout='$sha_v_out' (expected rc=1 stdout=1)" >&2; exit 1; }
grep -Fq 'disappeared or is unreadable while hashing' "$sha_v_root/session.log" \
    || { echo 'FAIL: the mid-hash source loss was not recorded in the session log' >&2; exit 1; }

mkdir -p "$sha_v_root/seven/src" "$sha_v_root/seven/dst"
printf 'target\n' > "$sha_v_root/seven/src/real"
ln -s real "$sha_v_root/seven/src/link"
set +e
sha_v_call verify_sha256_item "$sha_v_root/seven/src/link" "$sha_v_root/seven/dst"
sha_v_rc=$?
set -e
[[ "$sha_v_rc" -eq 0 && "$sha_v_out" == 0 ]] \
    || { echo "FAIL: a symlink source returned rc=$sha_v_rc stdout='$sha_v_out' (expected rc=0 stdout=0)" >&2; exit 1; }

# (a) a source file removed between copy and verification: the loop completes,
# no arithmetic error is printed, the failure is counted and named in the
# final summary, and the request fails closed.
sha_a_root="$fc_root/sha-copy"
mkdir -p "$sha_a_root/src" "$sha_a_root/dst" "$sha_a_root/session"
printf 'keep-me\n' > "$sha_a_root/src/keep"
printf 'vanishing\n' > "$sha_a_root/src/vanishes"
sha_a_run() {
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        SESSION_DIR="$sha_a_root/session"
        SESSION_LOG="$sha_a_root/session.log"
        : > "$SESSION_LOG"
        prepare_target() { :; }
        maybe_mount_target_path() { :; }
        promote_target_data_rw() { :; }
        validate_virtual_path() { :; }
        validate_host_source() { :; }
        target_destination_sensitive() { return 1; }
        target_destination_path() { printf '%s\n' "$sha_a_root/dst"; }
        if [[ -n "${SHAV_VANISH_AT:-}" ]]; then
            sha256sum()
            {
                local arg
                for arg in "$@"; do
                    if [[ "$arg" == "$SHAV_VANISH_AT" ]]; then
                        rm -f -- "$SHAV_VANISH_AT"
                        break
                    fi
                done
                command sha256sum "$@"
            }
        fi
        run_file_copy "$@"
    )
}

set +e
SHAV_VANISH_AT="$sha_a_root/src/vanishes" \
    sha_a_run copy host-to-repair preserve normal "$sha_a_root/dst" "$sha_a_root/src" \
    > "$sha_a_root/copy-fail.out" 2>&1
sha_a_rc=$?
set -e
SHAV_VANISH_AT=""
(( sha_a_rc != 0 )) \
    || { echo 'FAIL: a copy with a failed SHA-256 verification succeeded instead of failing closed' >&2; cat "$sha_a_root/copy-fail.out" >&2; exit 1; }
grep -Fq 'operand expected' "$sha_a_root/copy-fail.out" \
    && { echo 'FAIL: the verification failure still triggers an arithmetic error' >&2; cat "$sha_a_root/copy-fail.out" >&2; exit 1; }
grep -Fq 'syntax error' "$sha_a_root/copy-fail.out" \
    && { echo 'FAIL: the verification failure still triggers an arithmetic syntax error' >&2; cat "$sha_a_root/copy-fail.out" >&2; exit 1; }
grep -Fq 'msg:copy-complete-sha-failed' "$sha_a_root/copy-fail.out" \
    || { echo 'FAIL: the failed copy did not report the verification failure in its summary' >&2; cat "$sha_a_root/copy-fail.out" >&2; exit 1; }
grep -Eq 'msg:copy-sha-verified|param:1' "$sha_a_root/copy-fail.out" \
    || { echo 'FAIL: the failed copy did not count the one healthy verified file' >&2; cat "$sha_a_root/copy-fail.out" >&2; exit 1; }
grep -Eq 'msg:copy-sha-failures|param:1' "$sha_a_root/copy-fail.out" \
    || { echo 'FAIL: the failed copy did not count the verification failure' >&2; cat "$sha_a_root/copy-fail.out" >&2; exit 1; }
grep -Fq 'msg:copy-failed-verification|param:' "$sha_a_root/copy-fail.out" \
    || { echo 'FAIL: the failed copy summary did not name the failed verification' >&2; cat "$sha_a_root/copy-fail.out" >&2; exit 1; }
grep -Fq 'vanishes' "$sha_a_root/copy-fail.out" \
    || { echo 'FAIL: the failed copy summary did not name the vanished source file' >&2; cat "$sha_a_root/copy-fail.out" >&2; exit 1; }
grep -Fq 'the File Copy is not verified complete' "$sha_a_root/copy-fail.out" \
    || { echo 'FAIL: the failed copy did not fail closed with the verification failure reason' >&2; cat "$sha_a_root/copy-fail.out" >&2; exit 1; }
[[ -f "$sha_a_root/dst/src/keep" ]] \
    || { echo 'FAIL: the copy loop did not complete: the healthy file never reached the destination' >&2; exit 1; }

# The same tree without the mid-copy loss verifies cleanly and stays on the
# success path, and the preview path is untouched (no verification, rc 0).
printf 'vanishing\n' > "$sha_a_root/src/vanishes"
rm -f "$sha_a_root/dst/src/vanishes"
set +e
sha_a_run copy host-to-repair preserve normal "$sha_a_root/dst" "$sha_a_root/src" \
    > "$sha_a_root/copy-ok.out" 2>&1
sha_a_rc=$?
set -e
(( sha_a_rc == 0 )) \
    || { echo "FAIL: a clean copy failed (rc $sha_a_rc)" >&2; cat "$sha_a_root/copy-ok.out" >&2; exit 1; }
grep -Eq 'msg:copy-complete' "$sha_a_root/copy-ok.out" \
    || { echo 'FAIL: the clean copy did not report msg:copy-complete' >&2; cat "$sha_a_root/copy-ok.out" >&2; exit 1; }
grep -Eq 'msg:copy-sha-verified|param:2' "$sha_a_root/copy-ok.out" \
    || { echo 'FAIL: the clean copy did not count both verified files' >&2; cat "$sha_a_root/copy-ok.out" >&2; exit 1; }
if grep -Fq 'SHA-256 verification FAILURES' "$sha_a_root/copy-ok.out"; then
    echo 'FAIL: the clean copy summary names verification failures' >&2
    cat "$sha_a_root/copy-ok.out" >&2
    exit 1
fi
set +e
sha_a_run copy-preview host-to-repair preserve normal "$sha_a_root/dst" "$sha_a_root/src" \
    > "$sha_a_root/preview.out" 2>&1
sha_a_rc=$?
set -e
(( sha_a_rc == 0 )) \
    || { echo "FAIL: a copy preview failed (rc $sha_a_rc)" >&2; cat "$sha_a_root/preview.out" >&2; exit 1; }
grep -Fq 'msg:preview-complete' "$sha_a_root/preview.out" \
    || { echo 'FAIL: the preview path changed its completion wording' >&2; cat "$sha_a_root/preview.out" >&2; exit 1; }
if grep -Fq 'msg:copy-complete' "$sha_a_root/preview.out"; then
    echo 'FAIL: the preview path reports a copy completion' >&2
    cat "$sha_a_root/preview.out" >&2
    exit 1
fi


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


echo "PASS: file system repair helper contract is wired, read-only by default and scope-safe."
