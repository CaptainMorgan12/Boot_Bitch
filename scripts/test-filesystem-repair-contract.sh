#!/usr/bin/env bash
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
grep -q 'local -a keys=(validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub bootstack)' "$HELPER"
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
grep -q 'btrfs check --repair is a last-resort' "$HELPER"
grep -q 'ntfsfix only clears the NTFS dirty state' "$HELPER"
grep -q '^filesystem_mode_matrix()' "$HELPER"
grep -q '^filesystem_zfs_status_result()' "$HELPER"
grep -q '^filesystem_btrfs_scrub_result()' "$HELPER"
grep -q 'result=skipped' "$HELPER"
grep -q 'summary_skipped += 1' "$HELPER"
grep -q 'Offline file system repair refuses mounted filesystem' "$HELPER"
grep -q 'Online file system repair requires' "$HELPER"
grep -q 'is not part of the resolved scope' "$HELPER"

# ---------------------------------------------------------------------------
# Live harness.  Mock tools and a generated harness script source the helper
# (without main) and override only mount/topology plumbing.  No real block
# device, mount or filesystem is ever touched.
# ---------------------------------------------------------------------------
sandbox="$(mktemp -d)"
cleanup_sandbox() { rm -rf -- "$sandbox"; }
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
printf 'test-efi vfat part ABCD-1234 test-disk 1 test-disk\n' >> "$sandbox/devices.db"
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
exit 0
MOCK

cat > "$sandbox/mockbin/umount" <<'MOCK'
#!/usr/bin/env bash
set -euo pipefail
target="${!#}"
if [[ -n "${FAKE_MOUNTPOINT_DB:-}" && -f "$FAKE_MOUNTPOINT_DB" ]]; then
    grep -Fxv -- "$target" "$FAKE_MOUNTPOINT_DB" > "$FAKE_MOUNTPOINT_DB.tmp" 2>/dev/null || true
    mv "$FAKE_MOUNTPOINT_DB.tmp" "$FAKE_MOUNTPOINT_DB"
fi
if [[ -n "${FAKE_FINDMNT_DB:-}" && -f "$FAKE_FINDMNT_DB" ]]; then
    awk -v t="$target" '$1 != t' "$FAKE_FINDMNT_DB" > "$FAKE_FINDMNT_DB.tmp"
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
esac
exit 0
MOCK

chmod +x "$sandbox/mockbin"/*

export FAKE_LSBLK_DB="$sandbox/devices.db"
export FAKE_FINDMNT_DB="$sandbox/findmnt.db"
export FAKE_MOUNTPOINT_DB="$sandbox/mountpoints.txt"
export FAKE_MOUNT_LOG="$sandbox/mount.log"
export FAKE_TOOL_LOG="$sandbox/tools.log"
export FAKE_DEV_DIR="$sandbox"
: > "$FAKE_FINDMNT_DB"
: > "$FAKE_MOUNTPOINT_DB"
: > "$FAKE_MOUNT_LOG"
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
grep -Fqx "Repair tool filesystem: unavailable|The selected scope's root, /boot, ESP and /home filesystems could not be resolved" <<<"$cap_unresolved" \
    || { echo "FAIL: missing filesystem unavailable capability line" >&2; printf '%s\n' "$cap_unresolved" >&2; exit 1; }
grep -Fqx 'Repair capability evidence filesystem: scope filesystems unresolved' <<<"$cap_unresolved" \
    || { echo "FAIL: missing unresolved filesystem evidence" >&2; exit 1; }
grep -Fqx 'Repair tool validate: available' <<<"$cap_unresolved" \
    || { echo "FAIL: existing capability keys changed" >&2; exit 1; }
for cap_key in dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub bootstack; do
    grep -Eq "^Repair tool $cap_key: " <<<"$cap_unresolved" \
        || { echo "FAIL: existing capability key missing: $cap_key" >&2; exit 1; }
done
[[ "$(grep -c '^Repair tool ' <<<"$cap_unresolved")" -eq 12 ]] \
    || { echo "FAIL: expected 12 capability keys" >&2; exit 1; }

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
grep -Fq 'a reboot is recommended before using the filesystem' <<<"$e2fsck_reboot" \
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
grep -Fq 'run Windows chkdsk /f for a real NTFS repair' <<<"$ntfs_remain" \
    || { echo 'FAIL: ntfsfix failure did not require chkdsk' >&2; exit 1; }
ntfs_ok="$(run_harness "$(repair_code /dev/test-home repair)" 2>&1 || true)"
expect_repair_line /dev/test-home repair clean "$ntfs_ok"
grep -Fq 'Windows chkdsk /f is required for a real NTFS repair' <<<"$ntfs_ok" \
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
grep -Fq 'btrfs check --repair is a last-resort' <<<"$btrfs_warning" \
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
expect_single_change_status "$clean_check_status" 'unchanged|read-only check reported no errors'

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

zfs_degraded="$(FAKE_ZPOOL_POOLS=rpool FAKE_ZPOOL_STATUS_P='	/dev/test-zfs	ONLINE' FAKE_ZPOOL_STATUS_V='state: DEGRADED' run_harness "$(repair_code /dev/test-zfs scrub)" 2>&1 || true)"
expect_repair_line /dev/test-zfs scrub issues "$zfs_degraded"

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
grep -Fqx 'Repair tool filesystem: unavailable|No supported file system check tool is installed in the recovery environment' <<<"$cap_notools" \
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
grep -Fqx 'Repair tool filesystem: unavailable|The selected scope has no supported file system type for a read-only check' <<<"$cap_unsupported" \
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
[[ "$(grep -c '^Repair tool ' <<<"$combined_report")" -eq 12 ]] \
    || { echo 'FAIL: combined report lost capability key lines' >&2; exit 1; }
[[ "$(grep -c '^Repair capability evidence [a-z]*: ' <<<"$combined_report")" -eq 12 ]] \
    || { echo 'FAIL: combined report lost capability evidence lines' >&2; exit 1; }
[[ "$(grep -c '^Diagnostic: ' <<<"$combined_report")" -eq 14 ]] \
    || { echo 'FAIL: combined report did not run every diagnostic section' >&2; exit 1; }
grep -Fqx 'Repair tool filesystem: available' <<<"$combined_report" \
    || { echo 'FAIL: combined report lost the filesystem capability key line' >&2; exit 1; }

echo "PASS: file system repair helper contract is wired, read-only by default and scope-safe."
