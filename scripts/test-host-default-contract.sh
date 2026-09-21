#!/usr/bin/env bash
# Contract test for the permanent TUXEDO UKI host-default fix (Make Default).
#
# The helper under test is sourced dynamically and its globals are set
# directly, so ShellCheck cannot track their use.
# shellcheck disable=SC1090,SC2034
set -Eeuo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="${HELPER:-$ROOT_DIR/scripts/boot-repair-helper.sh}"

[[ -x "$HELPER" ]] || { echo "FAIL: helper is not executable" >&2; exit 1; }
bash -n "$HELPER"

fail_test() { echo "FAIL: $*" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Static contract: the functions and safety properties the transaction relies
# on must stay present, and the historical bugs must stay fixed.
# ---------------------------------------------------------------------------
for fn in host_secure_boot_state host_uki_signature_verified host_uki_file_path \
    host_uki_layout_state host_uki_default_candidate ensure_host_default_entry \
    ensure_host_default_entry_for_loader host_canonical_loader_info \
    host_default_unavailable_reason host_default_diagnostic_evidence \
    host_default_loader_backup_state host_default_loader_restore_backup \
    run_host_default_generic_body host_default_generic_apply \
    fedora_bls_entry_ids fedora_bls_newest_id fedora_bls_entry_id_for_kernel \
    fedora_bls_entry_exists fedora_default_entry_target_id \
    fedora_default_entry_unavailable_reason fedora_default_entry_ensure \
    grubenv_keys_except_saved_entry grubenv_saved_entry_evidence \
    host_default_write_state host_default_created_id host_default_rollback \
    host_default_verify_preserved_ids run_host_default_uki_body \
    host_default_uki_apply efi_label_updater_path \
    extlinux_configured_default_label extlinux_config_marks_default \
    extlinux_config_default_label extlinux_entry_labels_for_kernel \
    extlinux_default_entry_unavailable_reason extlinux_default_verdict \
    extlinux_default_restore_backup extlinux_conf_with_default \
    extlinux_replace_conf extlinux_default_entry_ensure \
    diagnostic_uki_registration_state objcopy_dump_section; do
    grep -q "^${fn}()" "$HELPER" || fail_test "missing helper function: $fn"
done

# The running-host capability gate is its own probe line and must never fall
# back to the coarse efi key; the generic path must never promote the fallback
# while a canonical loader file exists.
grep -Fq 'host_default_unavailable_reason' <<<"$(sed -n '/^diagnostic_repair_capabilities()/,/^}/p' "$HELPER")" \
    || fail_test 'diagnostic_repair_capabilities does not emit the host default capability'
grep -Fq 'never a fallback promotion' <<<"$(sed -n '/^run_host_default_generic_body()/,/^}/p' "$HELPER")" \
    || fail_test 'run_host_default_generic_body does not refuse fallback promotion'
if grep -Fq 'for role in uki vendor-loader fallback wfai' <<<"$(sed -n '/^run_host_default()/,/^}/p' "$HELPER")"; then
    fail_test 'run_host_default still promotes the first unique role (fallback)'
fi
grep -Fq 'fedora_default_entry_ensure' <<<"$(sed -n '/^run_host_default()/,/^}/p' "$HELPER")" \
    || fail_test 'run_host_default does not route Fedora BIOS to the BLS default ensure'
grep -Fq 'extlinux_default_entry_ensure' <<<"$(sed -n '/^run_host_default()/,/^}/p' "$HELPER")" \
    || fail_test 'run_host_default does not route BIOS/extlinux to the default-label ensure'
grep -Fq 'extlinux_default_entry_unavailable_reason' <<<"$(sed -n '/^host_default_unavailable_reason()/,/^}/p' "$HELPER")" \
    || fail_test 'host_default_unavailable_reason does not probe the extlinux default action'

# The extlinux default action is config-only: it uses the overwrite=0 trial,
# the entry-preservation guard and a backup/restore, and never writes a boot
# sector, ldlinux.sys, the MBR or runs the extlinux installer.
extlinux_default_body="$(sed -n '/^extlinux_default_entry_ensure()/,/^}/p' "$HELPER")"
grep -Fq 'overwrite=0' <<<"$extlinux_default_body" \
    || fail_test 'extlinux default ensure lost the overwrite=0 trial pattern'
grep -Fq 'guard_extlinux_candidate_preserves_entries' <<<"$extlinux_default_body" \
    || fail_test 'extlinux default ensure lost the entry-preservation guard'
grep -Fq 'extlinux_default_restore_backup' <<<"$extlinux_default_body" \
    || fail_test 'extlinux default ensure lost the rollback path'
if grep -Eq 'extlinux --(install|update)|dd if=|mbr\.bin|gptmbr' <<<"$extlinux_default_body"; then
    fail_test 'extlinux default ensure must not write the boot sector or reinstall the loader'
fi

# F1: the silent EFI/TUXEDO directory gate must not return from the restore
# path; the UKI file is the gate.
if grep -Fq '[[ -d "$host_mount/EFI/TUXEDO" ]] || return 0' "$HELPER"; then
    fail_test 'the silent EFI/TUXEDO directory gate is still present'
fi
grep -Fq 'host_uki="$host_mount/EFI/BOOT/TUX.EFI"' "$HELPER" \
    || fail_test 'the host UKI restore gate is not based on TUX.EFI'

# Loader-based identity: UKI entries are found by PARTUUID + loader path, never
# by the firmware label alone.
uki_ids_body="$(sed -n '/^efi_uki_entry_ids_for_partuuid()/,/^}/p' "$HELPER")"
grep -Fq 'efi_entry_ids_for_partuuid_loader' <<<"$uki_ids_body" \
    || fail_test 'efi_uki_entry_ids_for_partuuid does not use loader-based identity'
if grep -Fq 'index($0, "TUXEDO UKI")' <<<"$uki_ids_body"; then
    fail_test 'efi_uki_entry_ids_for_partuuid still matches on the firmware label'
fi

# Read-only section inspection: GNU objcopy rewrites its input in place when
# the output operand is omitted, changing the PE TimeDateStamp and invalidating
# a signed UKI.  Every dump must go through objcopy_dump_section with an
# explicit throwaway output file.
objcopy_dumps="$(grep -n 'objcopy --dump-section' "$HELPER" || true)"
[[ -n "$objcopy_dumps" ]] || fail_test 'objcopy_dump_section lost its objcopy invocation'
while IFS= read -r dump_line; do
    [[ -n "$dump_line" ]] || continue
    [[ "$dump_line" == *'objcopy --dump-section "$section" "$image" "$out"'* ]] && continue
    fail_test "objcopy --dump-section outside objcopy_dump_section rewrites its input: $dump_line"
done <<<"$objcopy_dumps"
grep -Fq 'objcopy --dump-section "$section" "$image" "$out"' "$HELPER" \
    || fail_test 'objcopy_dump_section does not pass an explicit output operand'

# F4 regression: the BootOrder fallback append must match both active
# (`Boot0001* `) and inactive (`Boot0001 `) firmware definitions.
promote_body="$(sed -n '/^efi_promote_entry_first()/,/^}/p' "$HELPER")"
grep -Fq 's/^Boot([0-9A-Fa-f]{4})\*?[[:space:]].*/\1/p' <<<"$promote_body" \
    || fail_test 'efi_promote_entry_first lost the fixed optional-star sed regex'
if grep -Fq 'Boot([0-9A-Fa-f]{4})\\*?' <<<"$promote_body"; then
    fail_test 'efi_promote_entry_first still carries the double-backslash sed regex'
fi

# The transaction must reconcile the inventory, promote, preserve foreign IDs
# and compare the pre-capture for the change status.
default_body="$(sed -n '/^run_host_default_uki_body()/,/^}/p' "$HELPER")"
grep -Fq 'efi_reconcile_firmware_inventory' <<<"$default_body" \
    || fail_test 'run_host_default_uki_body does not reconcile the firmware inventory'
grep -Fq 'efi_promote_entry_first' <<<"$default_body" \
    || fail_test 'run_host_default_uki_body does not promote the UKI entry'
grep -Fq 'host_default_verify_preserved_ids' <<<"$default_body" \
    || fail_test 'run_host_default_uki_body does not verify preserved firmware IDs'
run_default_body="$(sed -n '/^run_host_default()/,/^}/p' "$HELPER")"
grep -Fq 'cmp -s "$pre" <(efibootmgr -v 2>/dev/null || true)' <<<"$run_default_body" \
    || fail_test 'run_host_default lost the pre/post NVRAM comparison'
grep -Fq 'host_default_uki_apply' <<<"$run_default_body" \
    || fail_test 'run_host_default does not run the rollback transaction wrapper'

# ---------------------------------------------------------------------------
# Fixture harness: stub efibootmgr/objcopy and the host/ESP probes.
# ---------------------------------------------------------------------------
WORK_ROOT="$(mktemp -d)"
source <(sed '/^main "\$@"$/d' "$HELPER")
trap - EXIT INT TERM HUP
trap 'rm -rf -- "$WORK_ROOT"' EXIT

SESSION_DIR="$WORK_ROOT/session"
SESSION_LOG="$SESSION_DIR/session.log"
EFI_TEST_ROOT="$WORK_ROOT/root"
EFI_TEST_ESP_MOUNT="$EFI_TEST_ROOT/esp"
EFI_TEST_STATE="$WORK_ROOT/nvram.txt"
EFI_TEST_CALLS="$WORK_ROOT/calls.log"
EFI_TEST_LABEL_CALLS="$WORK_ROOT/label-calls.log"
EFI_TEST_LABEL_UPDATER="$WORK_ROOT/efi-label-update"
HOST_PARTUUID="11111111-2222-3333-4444-555555555555"
FOREIGN_PARTUUID="66666666-7777-8888-9999-aaaaaaaaaaaa"
ROOT_UUID="aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee"
LUKS_UUID="12345678-1234-1234-1234-123456789abc"
DEFAULT_UKI_UNAME="6.8.2-tuxedo-amd64"
DEFAULT_UKI_CMDLINE="root=UUID=$ROOT_UUID rd.luks.uuid=$LUKS_UUID subvol=/@"
# The label updater runs as a child process and needs the fixture paths.
export EFI_TEST_STATE EFI_TEST_LABEL_CALLS

cat > "$EFI_TEST_LABEL_UPDATER" <<'UPDATER'
#!/usr/bin/env bash
set -euo pipefail
state="${EFI_TEST_STATE:?}"
id=""; label=""; partuuid=""; loader=""; backup=""
while (($#)); do
    case "$1" in
        --bootnum) id="$2"; shift 2 ;;
        --label) label="$2"; shift 2 ;;
        --partuuid) partuuid="$2"; shift 2 ;;
        --loader) loader="$2"; shift 2 ;;
        --backup) backup="$2"; shift 2 ;;
        *) shift ;;
    esac
done
[[ "$id" =~ ^[0-9A-Fa-f]{4}$ && -n "$label" ]] || exit 1
printf '%s\t%s\t%s\t%s\n' "$id" "$label" "$partuuid" "$loader" >> "${EFI_TEST_LABEL_CALLS:-/dev/null}"
if [[ -n "$backup" ]]; then
    mkdir -p -- "$(dirname -- "$backup")"
    cp -- "$state" "$backup"
fi
tmp="$state.label.$$"
awk -v wanted="${id^^}" -v new_label="$label" '
  $0 ~ ("^Boot" wanted "\\*?[[:space:]]") {
    header=$0
    match(header, /^Boot[0-9A-Fa-f]{4}\*?[[:space:]]+/)
    header=substr(header, 1, RLENGTH)
    rest=$0
    sub(/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]+/, "", rest)
    path_start=match(rest, /[[:space:]](HD\(|File\(|PciRoot\(|VenHw\(|MemoryMapped\()/)
    if (path_start) { print header new_label " " substr(rest, path_start + 1); next }
  }
  { print }
' "$state" > "$tmp" && mv -- "$tmp" "$state"
echo "EFI label changed for Boot${id^^}: '$label'"
UPDATER
chmod +x "$EFI_TEST_LABEL_UPDATER"

reset_fixture()
{
    : > "$EFI_TEST_CALLS"
    : > "$EFI_TEST_LABEL_CALLS"
    : > "$EFI_TEST_STATE"
    rm -rf -- "$EFI_TEST_ROOT" "$SESSION_DIR"
    mkdir -p "$SESSION_DIR" "$EFI_TEST_ROOT/boot" "$EFI_TEST_ESP_MOUNT/EFI/BOOT"
    : > "$SESSION_LOG"
    printf 'fixture-uki\n' > "$EFI_TEST_ESP_MOUNT/EFI/BOOT/TUX.EFI"
    printf 'fixture-fallback\n' > "$EFI_TEST_ESP_MOUNT/EFI/BOOT/BOOTX64.EFI"
    printf 'fixture-kernel\n' > "$EFI_TEST_ROOT/boot/vmlinuz-$DEFAULT_UKI_UNAME"
    EFI_TEST_NVRAM_WRITABLE=1
    EFI_TEST_SECURE_BOOT=disabled
    EFI_TEST_UKI_SIGNED=0
    EFI_TEST_CREATE_ID=0009
    EFI_TEST_BIOS_MODE=0
    EFI_TEST_MODEL=TestModel
    EFI_TEST_EDITENV_CALLS="$WORK_ROOT/fedora-editenv.calls"
    : > "$EFI_TEST_EDITENV_CALLS"
    unset EFI_TEST_OBJCOPY_FAIL EFI_TEST_CREATE_LOADER_OVERRIDE EFI_TEST_UKI_UNAME EFI_TEST_UKI_CMDLINE
    ROOT_CANONICAL="/dev/fakeroot"
    TARGET_ROOT="$EFI_TEST_ROOT"
    TARGET_ESP_MOUNT="/esp"
    TARGET_SUBVOL="@"
    TARGET_OS_ID=tuxedo
    RUNNING_HOST_MODE=1
    EFI_LABEL_UPDATER="$EFI_TEST_LABEL_UPDATER"
}

# Generic EFI fixture (Arch/Alpine GRUB): no vendor UKI, a canonical vendor
# loader under EFI/arch and the firmware fallback copy.
reset_generic_fixture()
{
    reset_fixture
    rm -f -- "$EFI_TEST_ESP_MOUNT/EFI/BOOT/TUX.EFI"
    rm -rf -- "$EFI_TEST_ESP_MOUNT/EFI/tuxedo"
    mkdir -p "$EFI_TEST_ESP_MOUNT/EFI/arch"
    printf 'fixture-grub\n' > "$EFI_TEST_ESP_MOUNT/EFI/arch/grubx64.efi"
    TARGET_OS_ID=arch
    EFI_BOOTLOADER_ID=""
    EFI_TEST_GRUB_INSTALL_AVAILABLE=1
    EFI_TEST_GRUB_INSTALL_FAIL=0
    EFI_TEST_GRUB_INSTALL_CALLS="$WORK_ROOT/grub-install.calls"
    : > "$EFI_TEST_GRUB_INSTALL_CALLS"
}

# Fedora/RHEL GRUB2 BIOS fixture: valid grubenv + BLS entries, no ESP.
reset_fedora_fixture()
{
    : > "$EFI_TEST_CALLS"
    : > "$EFI_TEST_STATE"
    rm -rf -- "$EFI_TEST_ROOT" "$SESSION_DIR"
    mkdir -p "$SESSION_DIR" "$EFI_TEST_ROOT/boot/grub2" "$EFI_TEST_ROOT/boot/loader/entries" \
        "$EFI_TEST_ROOT/etc/default" "$EFI_TEST_ROOT/usr/bin"
    : > "$SESSION_LOG"
    printf 'insmod blscfg\nblscfg\n' > "$EFI_TEST_ROOT/boot/grub2/grub.cfg"
    {
        printf '# GRUB Environment Block\n'
        printf 'saved_entry=stale-entry\n'
        printf 'blsdir=/boot/loader/entries\n'
        printf 'menu_auto_hide=1\n'
        printf 'boot_success=1\n'
    } > "$EFI_TEST_ROOT/boot/grub2/grubenv"
    truncate -s 1024 "$EFI_TEST_ROOT/boot/grub2/grubenv"
    printf 'GRUB_DEFAULT=saved\nGRUB_ENABLE_BLSCFG=true\nGRUB_TIMEOUT=5\n' \
        > "$EFI_TEST_ROOT/etc/default/grub"
    printf 'title Fedora\nversion 6.8.0-300.fc44.x86_64\nlinux /vmlinuz-6.8.0-300.fc44.x86_64\n' \
        > "$EFI_TEST_ROOT/boot/loader/entries/machine-6.8.0-300.fc44.x86_64.conf"
    printf 'title Fedora\nversion 6.7.0-100.fc44.x86_64\nlinux /vmlinuz-6.7.0-100.fc44.x86_64\n' \
        > "$EFI_TEST_ROOT/boot/loader/entries/machine-6.7.0-100.fc44.x86_64.conf"
    printf 'title Rescue\nversion 0-rescue\nlinux /vmlinuz-0-rescue\n' \
        > "$EFI_TEST_ROOT/boot/loader/entries/machine-0-rescue.conf"
    : > "$EFI_TEST_ROOT/usr/bin/grub2-editenv"
    chmod +x "$EFI_TEST_ROOT/usr/bin/grub2-editenv"
    TARGET_ROOT="$EFI_TEST_ROOT"
    TARGET_OS_ID=fedora
    TARGET_DISTRO_FAMILY=fedora
    TARGET_ESP_MOUNT="unresolved"
    EFI_ESP_SOURCE=""
    RUNNING_HOST_MODE=1
    EFI_TEST_BIOS_MODE=1
    EFI_TEST_RUNNING_KERNEL=6.8.0-300.fc44.x86_64
    EFI_TEST_EDITENV_FAIL=0
    EFI_TEST_EDITENV_CALLS="$WORK_ROOT/fedora-editenv.calls"
    : > "$EFI_TEST_EDITENV_CALLS"
}

# Alpine BIOS/extlinux fixture: a generated /boot/extlinux.conf, the
# update-extlinux configuration with `default=`, two kernel flavors and the
# kernel.release mapping update-extlinux uses to derive LABEL names.
reset_extlinux_fixture()
{
    reset_fixture
    rm -f -- "$EFI_TEST_ESP_MOUNT/EFI/BOOT/TUX.EFI"
    mkdir -p "$EFI_TEST_ROOT/etc" "$EFI_TEST_ROOT/sbin" \
        "$EFI_TEST_ROOT/usr/share/kernel/lts" "$EFI_TEST_ROOT/usr/share/kernel/virt"
    : > "$EFI_TEST_ROOT/sbin/update-extlinux"
    chmod +x "$EFI_TEST_ROOT/sbin/update-extlinux"
    printf 'overwrite=1\ndefault=virt\n' > "$EFI_TEST_ROOT/etc/update-extlinux.conf"
    printf 'kernel\n' > "$EFI_TEST_ROOT/boot/vmlinuz-lts"
    printf 'initramfs\n' > "$EFI_TEST_ROOT/boot/initramfs-lts"
    printf 'kernel\n' > "$EFI_TEST_ROOT/boot/vmlinuz-virt"
    printf 'initramfs\n' > "$EFI_TEST_ROOT/boot/initramfs-virt"
    printf '6.18.52-0-lts\n' > "$EFI_TEST_ROOT/usr/share/kernel/lts/kernel.release"
    printf '6.19.10-0-virt\n' > "$EFI_TEST_ROOT/usr/share/kernel/virt/kernel.release"
    cat > "$EFI_TEST_ROOT/boot/extlinux.conf" <<'EXTLINUX'
DEFAULT menu.c32
LABEL lts
  LINUX vmlinuz-lts
  INITRD initramfs-lts
LABEL virt
  MENU DEFAULT
  LINUX vmlinuz-virt
  INITRD initramfs-virt
EXTLINUX
    TARGET_OS_ID=alpine
    TARGET_DISTRO_FAMILY=alpine
    TARGET_BOOTLOADER_BACKEND="syslinux/extlinux"
    RUNNING_HOST_MODE=1
    EFI_TEST_RUNNING_KERNEL="6.18.52-0-lts"
    EFI_TEST_EXT_CALLS="$WORK_ROOT/extlinux.calls"
    EFI_TEST_EXT_FAIL=0
    EFI_TEST_EXT_FLAVORS=""
    EFI_TEST_EXT_MARK=1
    : > "$EFI_TEST_EXT_CALLS"
}

# Simulated update-extlinux: generates the configuration from the fixture
# kernels using `default=` exactly like the Alpine script (MENU DEFAULT on the
# matching LABEL), removes the candidate when byte-identical, and records the
# invocation so tests can assert the boot sector was never touched.
extlinux_stub_run_chroot_try()
{
    local label="$1"; shift
    printf '%s\n' "$*" >> "$EFI_TEST_EXT_CALLS"
    CHROOT_TRY_OUTPUT=''
    CHROOT_TRY_RC=0
    [[ "${EFI_TEST_EXT_FAIL:-0}" == 1 ]] && { CHROOT_TRY_RC=1; return 0; }
    local default flavor
    local -a flavors=(lts virt)
    [[ -n "${EFI_TEST_EXT_FLAVORS:-}" ]] && read -r -a flavors <<<"$EFI_TEST_EXT_FLAVORS"
    default="$(sed -nE 's/^default=(.*)$/\1/p' "$TARGET_ROOT/etc/update-extlinux.conf" | head -n1)"
    {
        printf 'DEFAULT menu.c32\n'
        for flavor in "${flavors[@]}"; do
            printf 'LABEL %s\n' "$flavor"
            [[ "${EFI_TEST_EXT_MARK:-1}" == 1 && "$flavor" == "$default" ]] && printf '  MENU DEFAULT\n'
            printf '  LINUX vmlinuz-%s\n  INITRD initramfs-%s\n' "$flavor" "$flavor"
        done
    } > "$EFI_TEST_ROOT/extlinux.generated"
    if cmp -s "$EFI_TEST_ROOT/extlinux.generated" "$TARGET_ROOT/boot/extlinux.conf"; then
        rm -f -- "$TARGET_ROOT/boot/extlinux.conf.new"
    else
        cp -- "$EFI_TEST_ROOT/extlinux.generated" "$TARGET_ROOT/boot/extlinux.conf.new"
    fi
    return 0
}

# --- stubbed host environment ---------------------------------------------
prepare_running_host() { :; }
profile_target_backends() { :; }
is_alpine_family() { return 1; }
uefi_nvram_writable() { [[ "${EFI_TEST_NVRAM_WRITABLE:-1}" == 1 ]]; }
host_secure_boot_state() { printf '%s\n' "${EFI_TEST_SECURE_BOOT:-disabled}"; }
host_uki_signature_verified() { [[ "${EFI_TEST_UKI_SIGNED:-0}" == 1 ]]; }
crypt_backing_device() { return 1; }
efi_selected_system_model() { printf '%s\n' "${EFI_TEST_MODEL-TestModel}"; }
efi_disk_and_partnum_for_esp() { printf '/dev/fakenvme\t1\n'; }
canonical_block()
{
    case "$1" in
        /dev/fakehostesp|/dev/fakenvme|/dev/fakenvme1|/dev/fakeroot) printf '%s\n' "$1" ;;
        *) return 1 ;;
    esac
}
is_block_device() { canonical_block "$1" >/dev/null 2>&1; }
efi_set_inventory_esp_ids()
{
    EFI_HOST_ESP_SOURCE="/dev/fakehostesp"
    EFI_HOST_ESP_PARTUUID="$HOST_PARTUUID"
    EFI_HOST_ESP_MOUNT="$EFI_TEST_ESP_MOUNT"
    EFI_TARGET_ESP_PARTUUID="$HOST_PARTUUID"
    EFI_ESP_SOURCE="/dev/fakehostesp"
}
blkid()
{
    case "$*" in
        *'-s UUID'*) printf '%s\n' "$ROOT_UUID" ;;
        *'-s PARTUUID'*) printf '%s\n' "$HOST_PARTUUID" ;;
        *'-t PARTUUID='*) printf '/dev/fakehostesp\n' ;;
        *) return 1 ;;
    esac
}
lsblk()
{
    case "$*" in
        *FSTYPE*) printf 'btrfs\n' ;;
        *) printf '\n' ;;
    esac
}
objcopy()
{
    local section="" out="" arg
    for arg in "$@"; do
        case "$arg" in
            .uname=*) section="uname"; out="${arg#.uname=}" ;;
            .cmdline=*) section="cmdline"; out="${arg#.cmdline=}" ;;
        esac
    done
    [[ -n "${EFI_TEST_OBJCOPY_FAIL:-}" ]] && return 1
    case "$section" in
        uname) printf '%s\0' "${EFI_TEST_UKI_UNAME:-$DEFAULT_UKI_UNAME}" > "$out" ;;
        cmdline) printf '%s\0' "${EFI_TEST_UKI_CMDLINE:-$DEFAULT_UKI_CMDLINE}" > "$out" ;;
        *) return 1 ;;
    esac
}
efibootmgr()
{
    printf '%s\n' "$*" >> "$EFI_TEST_CALLS"
    case "${1:-}" in
        -v)
            cat "$EFI_TEST_STATE"
            ;;
        --create|-c)
            local label="" loader="" id="${EFI_TEST_CREATE_ID:-0009}"
            local partuuid="${EFI_TEST_CREATE_PARTUUID:-$HOST_PARTUUID}"
            while (($#)); do
                case "$1" in
                    --label|-L) label="$2"; shift 2 ;;
                    --loader|-l) loader="$2"; shift 2 ;;
                    *) shift ;;
                esac
            done
            [[ -n "${EFI_TEST_CREATE_LOADER_OVERRIDE:-}" ]] && loader="$EFI_TEST_CREATE_LOADER_OVERRIDE"
            printf 'Boot%s* %s HD(1,GPT,%s,0x1000,0x200000)/%s\n' "$id" "$label" "$partuuid" "$loader" >> "$EFI_TEST_STATE"
            ;;
        -o)
            sed -i -E "s/^BootOrder:.*/BootOrder: $2/" "$EFI_TEST_STATE"
            ;;
        -b)
            local id="${2^^}" delete=false
            shift 2
            while (($#)); do
                case "$1" in
                    -B|--delete-bootnum) delete=true; shift ;;
                    *) shift ;;
                esac
            done
            if [[ "$delete" == true ]]; then
                awk -v wanted="$id" '$0 !~ ("^Boot" wanted "\\*?[[:space:]]")' "$EFI_TEST_STATE" > "$EFI_TEST_STATE.tmp"
                mv -- "$EFI_TEST_STATE.tmp" "$EFI_TEST_STATE"
            fi
            ;;
        -n)
            if grep -q '^BootNext:' "$EFI_TEST_STATE"; then
                sed -i -E "s/^BootNext:.*/BootNext: $2/" "$EFI_TEST_STATE"
            else
                sed -i "1iBootNext: $2" "$EFI_TEST_STATE"
            fi
            ;;
        -N)
            sed -i '/^BootNext:/d' "$EFI_TEST_STATE"
            ;;
    esac
    return 0
}

# --- generic EFI + Fedora BIOS stubs --------------------------------------
# The firmware-mode probe is pinned per fixture so the same sourced helper can
# exercise UEFI, BIOS/extlinux and Fedora BIOS layouts.
bios_firmware_mode() { [[ "${EFI_TEST_BIOS_MODE:-0}" == 1 ]]; }
grub_install_tool()
{
    [[ "${EFI_TEST_GRUB_INSTALL_AVAILABLE:-0}" == 1 ]] || return 1
    printf '/usr/sbin/grub-install\n'
}
validate_efi_bootloader_target()
{
    EFI_BOOTLOADER_ID="${EFI_TEST_BOOTLOADER_ID:-arch}"
    EFI_GRUB_INSTALL_PATH=/usr/sbin/grub-install
}
reinstall_efi_bootloader()
{
    printf 'reinstall\n' >> "$EFI_TEST_GRUB_INSTALL_CALLS"
    [[ "${EFI_TEST_GRUB_INSTALL_FAIL:-0}" == 1 ]] && return 1
    mkdir -p -- "$EFI_TEST_ESP_MOUNT/EFI/${EFI_TEST_BOOTLOADER_ID:-arch}"
    printf 'fixture-grub-reinstalled\n' > "$EFI_TEST_ESP_MOUNT/EFI/${EFI_TEST_BOOTLOADER_ID:-arch}/grubx64.efi"
    return 0
}
running_kernel_version() { printf '%s\n' "${EFI_TEST_RUNNING_KERNEL:-6.8.0-300.fc44.x86_64}"; }
run_chroot_try()
{
    local label="$1"; shift
    printf '%s\n' "$*" >> "$EFI_TEST_EDITENV_CALLS"
    CHROOT_TRY_OUTPUT=''
    CHROOT_TRY_RC=0
    case "$*" in
        *' set saved_entry='*)
            if [[ "${EFI_TEST_EDITENV_FAIL:-0}" == 1 ]]; then
                CHROOT_TRY_RC=1
                CHROOT_TRY_OUTPUT='simulated grub2-editenv failure'
                return 0
            fi
            local grubenv="${2:-}" target="${4#saved_entry=}"
            awk -v entry="$target" '
                /^saved_entry=/ { print "saved_entry=" entry; next }
                { print }
            ' "$TARGET_ROOT$grubenv" > "$TARGET_ROOT$grubenv.new"
            mv -- "$TARGET_ROOT$grubenv.new" "$TARGET_ROOT$grubenv"
            truncate -s 1024 "$TARGET_ROOT$grubenv"
            ;;
    esac
    return 0
}

assert_no_mutation_calls()
{
    if grep -Eq '(^| )(-c|--create|-o|-b|-B|-n|-N)( |$)' "$EFI_TEST_CALLS"; then
        cat "$EFI_TEST_CALLS" >&2
        fail_test "unexpected firmware mutation call: $*"
    fi
}

# ---------------------------------------------------------------------------
# T1: missing entry + verified UKI + no EFI/TUXEDO directory -> create the
# \EFI\BOOT\TUX.EFI entry, promote it first, preserve foreign/fallback IDs and
# report the evidence lines and a changed status.
# ---------------------------------------------------------------------------
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0001,0002,0003
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0003* UEFI: Leef Bridge-C PMAP, Partition 1 PciRoot(0x0)/HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)0000424f
EOF
EFI_TEST_CREATE_ID=0009
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/t1.out" 2>&1; then
    cat "$WORK_ROOT/t1.out" >&2
    fail_test 'T1 run_host_default failed'
fi
[[ "$(grep -c '^Repair change status ' "$WORK_ROOT/t1.out" || true)" -eq 1 ]] \
    || fail_test 'T1 did not emit exactly one change status line'
grep -Fq 'Repair change status host-default: changed' "$WORK_ROOT/t1.out" \
    || fail_test 'T1 change status is not changed'
grep -Fq 'Host default probe: uki=present' "$WORK_ROOT/t1.out" \
    || fail_test 'T1 probe did not report a present UKI'
grep -Fq 'Host default probe: secure-boot=disabled candidate=uki' "$WORK_ROOT/t1.out" \
    || fail_test 'T1 probe did not select the UKI candidate'
grep -Fq "Host default: entry=Boot0009 label='TUXEDO UKI TestModel' loader=\\EFI\\BOOT\\TUX.EFI action=created" "$WORK_ROOT/t1.out" \
    || fail_test 'T1 did not report the created entry'
grep -Fq 'Host default: fallback=Boot0002' "$WORK_ROOT/t1.out" \
    || fail_test 'T1 did not report the retained fallback'
grep -Fq 'Host default: BootOrder before=0001,0002,0003 after=' "$WORK_ROOT/t1.out" \
    || fail_test 'T1 did not report before/after BootOrder'
grep -Fq 'PASS: running host default EFI entry is Boot0009 on /dev/fakehostesp.' "$WORK_ROOT/t1.out" \
    || fail_test 'T1 PASS line is missing'
grep -Eq "^Boot0009\* TUXEDO UKI TestModel .*HD\(1,GPT,$HOST_PARTUUID" "$EFI_TEST_STATE" \
    || fail_test 'T1 did not create the host UKI entry with the host PARTUUID'
grep -Fq '/\EFI\BOOT\TUX.EFI' "$EFI_TEST_STATE" \
    || fail_test 'T1 created entry does not reference TUX.EFI'
grep -q '^BootOrder: 0009,0002,0001,0003' "$EFI_TEST_STATE" \
    || fail_test 'T1 did not promote the UKI first while preserving every other ID'
for id in 0001 0002 0003; do
    grep -Eq "^Boot${id}\*?[[:space:]]" "$EFI_TEST_STATE" \
        || fail_test "T1 lost pre-existing firmware entry Boot$id"
done

# ---------------------------------------------------------------------------
# T2: an existing foreign-labelled entry at TUX.EFI is relabeled in place; no
# new ID and no duplicate entry are created.
# ---------------------------------------------------------------------------
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0005
BootOrder: 0005,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0005* Vendor Loader HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
EOF
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/t2.out" 2>&1; then
    cat "$WORK_ROOT/t2.out" >&2
    fail_test 'T2 run_host_default failed'
fi
grep -Eq '^Boot0005\* TUXEDO UKI TestModel HD\(1,GPT,' "$EFI_TEST_STATE" \
    || fail_test 'T2 did not relabel the existing entry in place'
grep -Fq 'action=relabeled' "$WORK_ROOT/t2.out" \
    || fail_test 'T2 did not report the relabel action'
[[ "$(grep -c '/\\EFI\\BOOT\\TUX.EFI' "$EFI_TEST_STATE" || true)" -eq 1 ]] \
    || fail_test 'T2 created a duplicate UKI entry'
if grep -Fq -- '--create' "$EFI_TEST_CALLS"; then
    fail_test 'T2 issued a create instead of relabeling'
fi
grep -Fq "$(printf '0005\tTUXEDO UKI TestModel\t%s\t' "$HOST_PARTUUID")" "$EFI_TEST_LABEL_CALLS" \
    || fail_test 'T2 relabel did not carry the host PARTUUID and target label'

# ---------------------------------------------------------------------------
# T3: TUX.EFI present but unverifiable, with a TUXEDO-UKI-labelled entry that
# actually points at BOOTX64.EFI -> fail closed with the verification reason and
# zero BootOrder/create writes.
# ---------------------------------------------------------------------------
reset_fixture
EFI_TEST_UKI_UNAME="9.9.9-not-installed"
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0001
BootOrder: 0001
Boot0001* TUXEDO UKI HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
if ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/t3.out" 2>&1; then
    cat "$WORK_ROOT/t3.out" >&2
    fail_test 'T3 accepted an unverified TUX.EFI'
fi
grep -Fq 'TUX.EFI is present but could not be verified' "$WORK_ROOT/t3.out" \
    || fail_test 'T3 did not name the verification failure'
grep -Fq 'embedded kernel 9.9.9-not-installed is not installed under /boot' "$WORK_ROOT/t3.out" \
    || fail_test 'T3 did not name the embedded-kernel mismatch'
assert_no_mutation_calls 'T3'
grep -q '^BootOrder: 0001' "$EFI_TEST_STATE" \
    || fail_test 'T3 changed BootOrder'

# ---------------------------------------------------------------------------
# T4a: two host UKI entries with an active one -> the active entry is kept and
# the duplicate is pruned.
# ---------------------------------------------------------------------------
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0005
BootOrder: 0005,0006,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0005* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0006* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
EOF
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/t4a.out" 2>&1; then
    cat "$WORK_ROOT/t4a.out" >&2
    fail_test 'T4a run_host_default failed'
fi
grep -Fq 'keeping active entry Boot0005' "$WORK_ROOT/t4a.out" \
    || fail_test 'T4a did not keep the active UKI entry'
[[ "$(grep -c '/\\EFI\\BOOT\\TUX.EFI' "$EFI_TEST_STATE" || true)" -eq 1 ]] \
    || fail_test 'T4a did not prune the duplicate UKI entry'
grep -Eq '^BootOrder: 0005,' "$EFI_TEST_STATE" \
    || fail_test 'T4a did not keep the active UKI entry first'
grep -q '^Boot0001' "$EFI_TEST_STATE" \
    || fail_test 'T4a lost the foreign entry'

# T4b: two host UKI entries without an active one -> fail closed ambiguous.
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootOrder: 0005,0006
Boot0005* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0006* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
EOF
if ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/t4b.out" 2>&1; then
    fail_test 'T4b accepted ambiguous host UKI entries'
fi
grep -Fq 'ambiguous host UKI entries: 0005 0006' "$WORK_ROOT/t4b.out" \
    || fail_test 'T4b did not name the ambiguous entries'
assert_no_mutation_calls 'T4b'

# ---------------------------------------------------------------------------
# T5: read-only NVRAM -> fail closed before any mutation.
# ---------------------------------------------------------------------------
reset_fixture
EFI_TEST_NVRAM_WRITABLE=0
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0002
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
if ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/t5.out" 2>&1; then
    fail_test 'T5 changed a read-only NVRAM'
fi
grep -Fq 'UEFI variables are not writable' "$WORK_ROOT/t5.out" \
    || fail_test 'T5 did not name the read-only NVRAM'
assert_no_mutation_calls 'T5'
[[ "$(grep -c '^Repair change status ' "$WORK_ROOT/t5.out" || true)" -eq 0 ]] \
    || fail_test 'T5 emitted a change status after failing closed'

# ---------------------------------------------------------------------------
# T6: create succeeds but the post-verify loader is wrong -> rollback restores
# BootOrder, deletes the created ID and leaves a state byte-identical to the
# pre-capture.
# ---------------------------------------------------------------------------
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0001,0002
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
cp -- "$EFI_TEST_STATE" "$WORK_ROOT/t6-pre.txt"
EFI_TEST_CREATE_ID=0009
EFI_TEST_CREATE_LOADER_OVERRIDE='\EFI\BOOT\BOOTX64.EFI'
if ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/t6.out" 2>&1; then
    cat "$WORK_ROOT/t6.out" >&2
    fail_test 'T6 accepted an unverified created entry'
fi
grep -Fq 'ROLLBACK: restored BootOrder=0001,0002' "$WORK_ROOT/t6.out" \
    || fail_test 'T6 did not report the rollback'
grep -Fq 'could not be verified by PARTUUID' "$WORK_ROOT/t6.out" \
    || fail_test 'T6 did not name the failed verification'
cmp -s "$WORK_ROOT/t6-pre.txt" "$EFI_TEST_STATE" \
    || fail_test 'T6 rollback did not restore the pre-capture byte-identically'
if grep -q '^Boot0009' "$EFI_TEST_STATE"; then
    fail_test 'T6 rollback left the created entry behind'
fi
grep -Fq -- '-o 0001,0002' "$EFI_TEST_CALLS" \
    || fail_test 'T6 rollback did not restore BootOrder'
grep -Fq -- '-b 0009 -B' "$EFI_TEST_CALLS" \
    || fail_test 'T6 rollback did not delete the created entry'

# ---------------------------------------------------------------------------
# T7: embedded kernel/cmdline mismatches -> none|<reason> with no write; and
# the file-based detection is not distribution-gated.
# ---------------------------------------------------------------------------
reset_fixture
EFI_TEST_UKI_UNAME="9.9.9-not-installed"
layout_state="$(host_uki_layout_state)"
case "$layout_state" in
    absent\|*) ;;
    *) fail_test "T7 uname mismatch was not rejected: $layout_state" ;;
esac
grep -Fq 'embedded kernel 9.9.9-not-installed is not installed under /boot' <<<"$layout_state" \
    || fail_test 'T7 uname mismatch reason is missing'
candidate="$(host_uki_default_candidate)"
grep -Fq 'none|' <<<"$candidate" || fail_test 'T7 uname mismatch did not yield none'
assert_no_mutation_calls 'T7-uname'

reset_fixture
EFI_TEST_UKI_CMDLINE="root=UUID=deadbeef-dead-dead-dead-deaddeadbeef rd.luks.uuid=$LUKS_UUID subvol=/@"
layout_state="$(host_uki_layout_state)"
grep -Fq 'does not reference the live root UUID' <<<"$layout_state" \
    || fail_test 'T7 root mismatch reason is missing'
assert_no_mutation_calls 'T7-root'

reset_fixture
EFI_TEST_UKI_CMDLINE="root=UUID=$ROOT_UUID rd.luks.uuid=$LUKS_UUID subvol=/@.snapshots/1/snapshot"
layout_state="$(host_uki_layout_state)"
grep -Fq 'does not select the live Btrfs subvolume /@' <<<"$layout_state" \
    || fail_test 'T7 subvolume mismatch reason is missing'

# A verified UKI is detected from files even when the OS ID is not tuxedo.
reset_fixture
TARGET_OS_ID=debian
layout_state="$(host_uki_layout_state)"
[[ "$layout_state" == present\|* ]] \
    || fail_test 'T7 file-based UKI detection is distribution-gated'
# The target-repair layout gate keeps its existing builder/OS semantics.
TARGET_OS_ID=tuxedo
mkdir -p "$TARGET_ROOT/usr/sbin"
printf '#!/bin/sh\n' > "$TARGET_ROOT/usr/sbin/create_boot_uki_base.sh"
chmod +x "$TARGET_ROOT/usr/sbin/create_boot_uki_base.sh"
is_tuxedo_uki_layout || fail_test 'T7 is_tuxedo_uki_layout broke for a TUXEDO target'
TARGET_OS_ID=debian
if is_tuxedo_uki_layout; then
    fail_test 'T7 is_tuxedo_uki_layout no longer requires the TUXEDO OS ID'
fi

# ---------------------------------------------------------------------------
# T8: Secure Boot enabled + unsigned UKI -> the shim chain is selected when
# present and fails closed when absent.
# ---------------------------------------------------------------------------
reset_fixture
EFI_TEST_SECURE_BOOT=enabled
mkdir -p "$EFI_TEST_ESP_MOUNT/EFI/tuxedo"
printf 'shim\n' > "$EFI_TEST_ESP_MOUNT/EFI/tuxedo/shimx64.efi"
candidate="$(host_uki_default_candidate)"
[[ "$candidate" == 'shim \EFI\tuxedo\shimx64.efi' ]] \
    || fail_test "T8 did not choose the shim chain: $candidate"
efibootmgr -v > "$SESSION_DIR/pre-t8.txt"
if ! t8_id="$( ensure_host_default_entry "$SESSION_DIR/pre-t8.txt" )"; then
    fail_test 'T8 ensure_host_default_entry failed for the shim chain'
fi
[[ "$t8_id" == 0009 ]] || fail_test "T8 shim entry ID is wrong: $t8_id"
grep -Eq '^Boot0009\* TUXEDO UKI TestModel HD\(1,GPT,' "$EFI_TEST_STATE" \
    || fail_test 'T8 shim entry label is wrong'
grep -Fq '/\EFI\tuxedo\shimx64.efi' "$EFI_TEST_STATE" \
    || fail_test 'T8 shim entry does not reference the shim loader'
if grep -Fq '/\EFI\BOOT\TUX.EFI' "$EFI_TEST_STATE"; then
    fail_test 'T8 promoted the unsigned UKI under Secure Boot'
fi

rm -f -- "$EFI_TEST_ESP_MOUNT/EFI/tuxedo/shimx64.efi"
candidate="$(host_uki_default_candidate)"
grep -Fq 'none|secure-boot requires a signed loader' <<<"$candidate" \
    || fail_test "T8 did not fail closed without a shim: $candidate"
reset_fixture
EFI_TEST_SECURE_BOOT=enabled
: > "$EFI_TEST_CALLS"
if ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/t8.out" 2>&1; then
    fail_test 'T8 promoted an unsigned UKI without a shim'
fi
grep -Fq 'secure-boot requires a signed loader' "$WORK_ROOT/t8.out" \
    || fail_test 'T8 fail-closed reason is missing'
assert_no_mutation_calls 'T8'

# ---------------------------------------------------------------------------
# T9: F4 regression: a firmware entry defined but absent from BootOrder must
# still be retained by the promotion.
# ---------------------------------------------------------------------------
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0004
BootOrder: 0004,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0004* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0009* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
EOF
efi_promote_entry_first 0009
grep -Eq '^BootOrder: 0009,0004,0001$' "$EFI_TEST_STATE" \
    || fail_test 'T9 promotion dropped the out-of-order Boot0009 or another entry'

# ---------------------------------------------------------------------------
# T10: F5 regression: a verified running-host UKI is a complete EFI boot path,
# so `efi` must stay available without GRUB tooling; a host without a verified
# UKI keeps the probe-based GRUB prerequisite reason.
# ---------------------------------------------------------------------------
reset_fixture
if ! ( efi_unavailable_reason ) > "$WORK_ROOT/t10a.out" 2>&1; then
    cat "$WORK_ROOT/t10a.out" >&2
    fail_test 'T10 verified host UKI did not make efi available'
fi
grep -Fq 'verified running-host TUX.EFI UKI' <<<"$(repair_capability_evidence efi)" \
    || fail_test 'T10 efi evidence does not name the verified host UKI'
reset_generic_fixture
EFI_TEST_GRUB_INSTALL_AVAILABLE=0
if ( efi_unavailable_reason ) > "$WORK_ROOT/t10b.out" 2>&1; then
    fail_test 'T10 host without a verified UKI lost the GRUB prerequisite gate'
fi
grep -Fq 'requires GRUB configuration tooling' "$WORK_ROOT/t10b.out" \
    || fail_test 'T10 did not name the missing GRUB prerequisite'

# ---------------------------------------------------------------------------
# Diagnostics: TUX.EFI present but unregistered must be flagged, and a stale
# grubenv saved_entry must be reported as read-only evidence.
# ---------------------------------------------------------------------------
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0001,0002
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
reg_state="$(diagnostic_uki_registration_state)"
[[ "$reg_state" == "unregistered $HOST_PARTUUID" ]] \
    || fail_test "diagnostic registration state is wrong: $reg_state"
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0009
BootOrder: 0009,0002
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0009* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
EOF
reg_state="$(diagnostic_uki_registration_state)"
read -r reg_kind reg_id reg_pos reg_lead reg_current <<<"$reg_state"
[[ "$reg_kind" == registered && "$reg_id" == 0009 && "$reg_pos" == first ]] \
    || fail_test "registered diagnostic state is wrong: $reg_state"
[[ "$reg_lead" == lead=0009 && "$reg_current" == current=0009 ]] \
    || fail_test "registered diagnostic lead/current is wrong: $reg_state"

reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0001,0002
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
boot_chain_out="$(diagnostic_boot_chain)"
grep -Fq 'WARNING: TUX.EFI present but unregistered' <<<"$boot_chain_out" \
    || fail_test 'diagnostic_boot_chain did not flag the unregistered TUX.EFI'
if grep -Fq 'Primary: firmware EFI entry -> TUXEDO UKI' <<<"$boot_chain_out"; then
    fail_test 'diagnostic_boot_chain still claims the UKI is primary'
fi

reset_fixture
mkdir -p "$TARGET_ROOT/etc/default" "$TARGET_ROOT/boot/grub"
printf 'GRUB_DEFAULT=saved\n' > "$TARGET_ROOT/etc/default/grub"
{
    printf '# GRUB Environment Block\n'
    printf 'saved_entry=gnulinux-7.1.8.2-tuxedo-amd64-advanced-1234\n'
} > "$TARGET_ROOT/boot/grub/grubenv"
saved_evidence="$(grubenv_saved_entry_evidence)"
grep -Fq 'WARNING: stale grubenv saved_entry' <<<"$saved_evidence" \
    || fail_test 'stale grubenv saved_entry was not reported'
grep -Fq 'grubenv is not modified' <<<"$saved_evidence" \
    || fail_test 'grubenv evidence does not state that no write happens'
grep -Fq 'saved_entry=gnulinux-7.1.8.2' "$TARGET_ROOT/boot/grub/grubenv" \
    || fail_test 'grubenv evidence mutated the environment block'
{
    printf '# GRUB Environment Block\n'
    printf 'saved_entry=gnulinux-%s-advanced-1234\n' "$DEFAULT_UKI_UNAME"
} > "$TARGET_ROOT/boot/grub/grubenv"
saved_evidence="$(grubenv_saved_entry_evidence)"
grep -Fq "PASS: grubenv saved_entry 'gnulinux-$DEFAULT_UKI_UNAME-advanced-1234' resolves to installed kernel $DEFAULT_UKI_UNAME." <<<"$saved_evidence" \
    || fail_test 'fresh grubenv saved_entry was not resolved to the installed kernel'

# ---------------------------------------------------------------------------
# G1: canonical Arch vendor loader present + fallback-only NVRAM -> create and
# promote the canonical EFI/arch/grubx64.efi entry; never promote `UEFI OS`,
# never run grub-install; foreign entries and the fallback stay bootable.
# ---------------------------------------------------------------------------
reset_generic_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0002,0001,0000
Boot0000* EFI Firmware Setup FvVol(00000000-0000-0000-0000-000000000000)/FvFile(abc)
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/g1.out" 2>&1; then
    cat "$WORK_ROOT/g1.out" >&2
    fail_test 'G1 run_host_default failed'
fi
grep -Fq "Host default: entry=Boot0009 label='arch TestModel' loader=\\EFI\\arch\\grubx64.efi action=created" "$WORK_ROOT/g1.out" \
    || fail_test 'G1 did not create the canonical arch entry'
grep -Eq '^BootOrder: 0009,0002,0001,0000$' "$EFI_TEST_STATE" \
    || fail_test 'G1 did not promote the canonical entry while retaining the fallback/foreign entries'
grep -q '^Boot0002\* UEFI OS ' "$EFI_TEST_STATE" \
    || fail_test 'G1 dropped the fallback entry'
grep -q '/\\EFI\\arch\\grubx64.efi' "$EFI_TEST_STATE" \
    || fail_test 'G1 did not register the canonical loader path'
[[ ! -s "$EFI_TEST_GRUB_INSTALL_CALLS" ]] \
    || fail_test 'G1 ran grub-install although the canonical loader file exists'
grep -Fq 'Repair change status host-default: changed' "$WORK_ROOT/g1.out" \
    || fail_test 'G1 change status is not changed'

# Idempotence: a second run converges to the same verified entry/order and
# reports unchanged with zero mutations.
cp -- "$EFI_TEST_STATE" "$WORK_ROOT/g1-before-second.txt"
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/g1b.out" 2>&1; then
    cat "$WORK_ROOT/g1b.out" >&2
    fail_test 'G1 second run_host_default failed'
fi
grep -Fq 'Repair change status host-default: unchanged' "$WORK_ROOT/g1b.out" \
    || fail_test 'G1 second run was not idempotent/unchanged'
cmp -s "$WORK_ROOT/g1-before-second.txt" "$EFI_TEST_STATE" \
    || fail_test 'G1 second run changed the firmware state'

# ---------------------------------------------------------------------------
# G2: duplicate canonical entries -> active one kept and promoted, duplicate
# pruned; no active one -> fail closed ambiguous with zero writes.
# ---------------------------------------------------------------------------
reset_generic_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0006
BootOrder: 0005,0006,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0005* arch TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\arch\\grubx64.efi
Boot0006* arch TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\arch\\grubx64.efi
EOF
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/g2a.out" 2>&1; then
    cat "$WORK_ROOT/g2a.out" >&2
    fail_test 'G2a run_host_default failed'
fi
grep -Fq 'keeping active entry Boot0006' "$WORK_ROOT/g2a.out" \
    || fail_test 'G2a did not keep the active canonical entry'
[[ "$(grep -c '/\\EFI\\arch\\grubx64.efi' "$EFI_TEST_STATE" || true)" -eq 1 ]] \
    || fail_test 'G2a did not prune the duplicate canonical entry'
grep -Eq '^BootOrder: 0006,' "$EFI_TEST_STATE" \
    || fail_test 'G2a did not keep the active canonical entry first'
grep -q '^Boot0001' "$EFI_TEST_STATE" \
    || fail_test 'G2a lost the foreign entry'

reset_generic_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootOrder: 0005,0006
Boot0005* arch TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\arch\\grubx64.efi
Boot0006* arch TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\arch\\grubx64.efi
EOF
if ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/g2b.out" 2>&1; then
    fail_test 'G2b accepted ambiguous canonical entries'
fi
grep -Fq 'ambiguous host canonical entries: 0005 0006' "$WORK_ROOT/g2b.out" \
    || fail_test 'G2b did not name the ambiguous canonical entries'
assert_no_mutation_calls 'G2b'

# ---------------------------------------------------------------------------
# G3: no canonical loader file and no grub-install -> fail closed naming the
# missing loader with zero writes (never a fallback promotion).
# ---------------------------------------------------------------------------
reset_generic_fixture
rm -rf -- "$EFI_TEST_ESP_MOUNT/EFI/arch"
EFI_TEST_GRUB_INSTALL_AVAILABLE=0
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0002,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
if ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/g3.out" 2>&1; then
    fail_test 'G3 promoted the fallback without a canonical loader'
fi
grep -Fq 'no canonical EFI vendor loader was found on the running host ESP and grub-install is not available' "$WORK_ROOT/g3.out" \
    || fail_test 'G3 did not name the missing canonical loader'
assert_no_mutation_calls 'G3'

# ---------------------------------------------------------------------------
# G4: missing canonical loader with grub-install available -> one guarded
# reinstall, ESP backup first, then the canonical entry is created/promoted.
# ---------------------------------------------------------------------------
reset_generic_fixture
rm -rf -- "$EFI_TEST_ESP_MOUNT/EFI/arch"
EFI_TEST_GRUB_INSTALL_AVAILABLE=1
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0002,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/g4.out" 2>&1; then
    cat "$WORK_ROOT/g4.out" >&2
    fail_test 'G4 guarded reinstall run failed'
fi
[[ "$(grep -c '^reinstall$' "$EFI_TEST_GRUB_INSTALL_CALLS" || true)" -eq 1 ]] \
    || fail_test 'G4 did not run exactly one guarded grub-install'
grep -Fq 'Backed up running-host EFI loader files' "$WORK_ROOT/g4.out" \
    || fail_test 'G4 did not take the ESP loader backup'
grep -Fq "Host default: entry=Boot0009 label='arch TestModel' loader=\\EFI\\arch\\grubx64.efi action=created" "$WORK_ROOT/g4.out" \
    || fail_test 'G4 did not create the canonical entry after the reinstall'
grep -Eq '^BootOrder: 0009,0002,0001$' "$EFI_TEST_STATE" \
    || fail_test 'G4 did not promote the reinstalled canonical entry'

# ---------------------------------------------------------------------------
# G5: guarded reinstall fails -> ESP loader backup restored byte-identical and
# the firmware state rolled back to the pre-capture.
# ---------------------------------------------------------------------------
reset_generic_fixture
rm -rf -- "$EFI_TEST_ESP_MOUNT/EFI/arch"
EFI_TEST_GRUB_INSTALL_AVAILABLE=1
EFI_TEST_GRUB_INSTALL_FAIL=1
printf 'fallback-original\n' > "$EFI_TEST_ESP_MOUNT/EFI/BOOT/BOOTX64.EFI"
cp -- "$EFI_TEST_ESP_MOUNT/EFI/BOOT/BOOTX64.EFI" "$WORK_ROOT/g5-fallback.txt"
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0002,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
cp -- "$EFI_TEST_STATE" "$WORK_ROOT/g5-pre.txt"
if ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/g5.out" 2>&1; then
    fail_test 'G5 accepted a failed guarded reinstall'
fi
grep -Fq 'guarded canonical EFI loader reinstall failed' "$WORK_ROOT/g5.out" \
    || fail_test 'G5 did not name the failed guarded reinstall'
grep -Fq 'Restored the running-host EFI loader files' "$WORK_ROOT/g5.out" \
    || fail_test 'G5 did not restore the ESP loader backup'
cmp -s "$WORK_ROOT/g5-fallback.txt" "$EFI_TEST_ESP_MOUNT/EFI/BOOT/BOOTX64.EFI" \
    || fail_test 'G5 did not restore the fallback loader byte-identically'
[[ ! -d "$EFI_TEST_ESP_MOUNT/EFI/arch" ]] \
    || fail_test 'G5 left a partial vendor directory behind'
cmp -s "$WORK_ROOT/g5-pre.txt" "$EFI_TEST_STATE" \
    || fail_test 'G5 did not roll the firmware state back to the pre-capture'

# ---------------------------------------------------------------------------
# G6: the diagnostics capability line names the host default action and the
# canonical candidate evidence stays informational.
# ---------------------------------------------------------------------------
reset_generic_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0002
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
caps="$(
    filesystem_scope_resolve() { :; }
    filesystem_scope_tools() { :; }
    diagnostic_repair_capabilities
)"
grep -Fqx 'Host default: available' <<<"$caps" \
    || fail_test 'G6 diagnostics did not report Host default: available'
grep -Fq "Host default candidate: Boot---- role=none loader=\\EFI\\arch\\grubx64.efi" <<<"$caps" \
    || { printf '%s\n' "$caps" >&2; fail_test 'G6 diagnostics did not name the canonical loader file'; }

# G7: Alpine BIOS/extlinux default-label selection is a guarded, config-only
# action: the LABEL that boots the running kernel is written to
# /etc/update-extlinux.conf and rendered through the overwrite=0 trial, the
# entry-preservation guard and the candidate verification; the boot sector,
# ldlinux.sys and the MBR are never touched.
if ! (
    set -Eeuo pipefail
    ext_fail() { echo "FAIL: $*" >&2; exit 1; }
    run_chroot_try() { extlinux_stub_run_chroot_try "$@"; }

    # E1: wrong default label -> the running kernel's LABEL is selected, both
    # files change, every other setting survives and no boot-sector write runs.
    reset_extlinux_fixture
    ensure_out="$(extlinux_default_entry_ensure 2>&1)" || ext_fail 'E1 extlinux default ensure failed'
    grep -Fq 'Extlinux default entry: label=lts kernel=6.18.52-0-lts action=set' <<<"$ensure_out" \
        || ext_fail 'E1 did not report the selected label'
    grep -Fq "Host default: entry=lts label='lts' loader=vmlinuz-lts action=set" <<<"$ensure_out" \
        || ext_fail 'E1 did not emit the verified default evidence'
    grep -Fq 'Repair change status host-default: changed|extlinux default label set to lts' <<<"$ensure_out" \
        || ext_fail 'E1 change status is wrong'
    grep -q '^default=lts$' "$TARGET_ROOT/etc/update-extlinux.conf" \
        || ext_fail 'E1 did not persist default=lts'
    grep -q '^overwrite=1$' "$TARGET_ROOT/etc/update-extlinux.conf" \
        || ext_fail 'E1 did not preserve the original overwrite setting'
    awk '/^LABEL lts$/{in_lts=1;next} /^LABEL /{in_lts=0} in_lts && /MENU DEFAULT/{found=1} END{exit(found?0:1)}' \
        "$TARGET_ROOT/boot/extlinux.conf" \
        || ext_fail 'E1 did not mark LABEL lts as MENU DEFAULT'
    if awk '/^LABEL virt$/{in_virt=1;next} /^LABEL /{in_virt=0} in_virt && /MENU DEFAULT/{found=1} END{exit(found?0:1)}' \
        "$TARGET_ROOT/boot/extlinux.conf"; then
        ext_fail 'E1 left the stale LABEL virt as MENU DEFAULT'
    fi
    # The successful selection must keep every existing entry in the installed
    # configuration: a generated candidate that drops the untouched LABEL is
    # the exact regression E6 fails closed on, and it must never reach the
    # installed file on the success path either.
    for label in lts virt; do
        grep -Eq "^[[:space:]]*LABEL[[:space:]]+$label\$" "$TARGET_ROOT/boot/extlinux.conf" \
            || ext_fail "E1 dropped the existing LABEL $label"
    done
    grep -Eq '^[[:space:]]*LINUX[[:space:]]+vmlinuz-virt$' "$TARGET_ROOT/boot/extlinux.conf" \
        || ext_fail 'E1 dropped the untouched virt LINUX entry'
    grep -Eq '^[[:space:]]*INITRD[[:space:]]+initramfs-virt$' "$TARGET_ROOT/boot/extlinux.conf" \
        || ext_fail 'E1 dropped the untouched virt INITRD entry'
    [[ "$(grep -c . "$EFI_TEST_EXT_CALLS" || true)" -eq 1 ]] \
        || ext_fail 'E1 did not run exactly one update-extlinux trial'
    if grep -Fq 'overwrite=0' "$TARGET_ROOT/etc/update-extlinux.conf"; then
        ext_fail 'E1 left overwrite=0 behind'
    fi
    [[ ! -e "$TARGET_ROOT/boot/extlinux.conf.new" ]] \
        || ext_fail 'E1 left the trial candidate behind'
    grep -Fq 'update-extlinux' "$EFI_TEST_EXT_CALLS" \
        || ext_fail 'E1 did not invoke update-extlinux'
    if grep -Eq 'extlinux --(install|update)|dd if=|mbr\.bin|gptmbr' "$EFI_TEST_EXT_CALLS"; then
        ext_fail 'E1 touched the boot sector or the extlinux installer'
    fi

    # E2: idempotent second run: unchanged, zero writes, zero trials.
    cp -a -- "$TARGET_ROOT/etc/update-extlinux.conf" "$WORK_ROOT/e2-conf-before"
    cp -a -- "$TARGET_ROOT/boot/extlinux.conf" "$WORK_ROOT/e2-cfg-before"
    : > "$EFI_TEST_EXT_CALLS"
    ensure_out="$(extlinux_default_entry_ensure 2>&1)" || ext_fail 'E2 second extlinux default ensure failed'
    grep -Fq 'action=unchanged' <<<"$ensure_out" \
        || ext_fail 'E2 did not report unchanged'
    grep -Fq 'Repair change status host-default: unchanged|extlinux default label lts is already selected' <<<"$ensure_out" \
        || ext_fail 'E2 change status is wrong'
    [[ ! -s "$EFI_TEST_EXT_CALLS" ]] || ext_fail 'E2 re-ran update-extlinux for an already-correct default'
    cmp -s "$WORK_ROOT/e2-conf-before" "$TARGET_ROOT/etc/update-extlinux.conf" \
        || ext_fail 'E2 rewrote /etc/update-extlinux.conf'
    cmp -s "$WORK_ROOT/e2-cfg-before" "$TARGET_ROOT/boot/extlinux.conf" \
        || ext_fail 'E2 rewrote /boot/extlinux.conf'

    # E3: the running kernel has no extlinux entry -> fail closed with the
    # exact reason and zero writes.
    reset_extlinux_fixture
    rm -f -- "$TARGET_ROOT/boot/vmlinuz-lts" "$TARGET_ROOT/usr/share/kernel/lts/kernel.release"
    cp -a -- "$TARGET_ROOT/etc/update-extlinux.conf" "$WORK_ROOT/e3-conf-before"
    cp -a -- "$TARGET_ROOT/boot/extlinux.conf" "$WORK_ROOT/e3-cfg-before"
    reason_out="$(extlinux_default_entry_unavailable_reason || true)"
    grep -Fq 'the running kernel 6.18.52-0-lts has no entry in /boot/extlinux.conf' <<<"$reason_out" \
        || ext_fail "E3 did not name the missing running-kernel entry: $reason_out"
    if ( host_default_unavailable_reason ) > "$WORK_ROOT/e3.out" 2>&1; then
        ext_fail 'E3 host default availability accepted a missing running-kernel entry'
    fi
    if ( extlinux_default_entry_ensure ) > "$WORK_ROOT/e3b.out" 2>&1; then
        ext_fail 'E3 extlinux default ensure accepted a missing running-kernel entry'
    fi
    cmp -s "$WORK_ROOT/e3-conf-before" "$TARGET_ROOT/etc/update-extlinux.conf" \
        || ext_fail 'E3 changed /etc/update-extlinux.conf'
    cmp -s "$WORK_ROOT/e3-cfg-before" "$TARGET_ROOT/boot/extlinux.conf" \
        || ext_fail 'E3 changed /boot/extlinux.conf'

    # E4: two labels mapping to the running kernel -> fail closed ambiguous.
    reset_extlinux_fixture
    printf '6.18.52-0-lts\n' > "$TARGET_ROOT/usr/share/kernel/virt/kernel.release"
    reason_out="$(extlinux_default_entry_unavailable_reason || true)"
    grep -Fq 'multiple extlinux entries reference the running kernel 6.18.52-0-lts (lts,virt)' <<<"$reason_out" \
        || ext_fail "E4 did not name the ambiguous labels: $reason_out"

    # E5: the update-extlinux trial fails -> both files restored byte-identical.
    reset_extlinux_fixture
    cp -a -- "$TARGET_ROOT/etc/update-extlinux.conf" "$WORK_ROOT/e5-conf-before"
    cp -a -- "$TARGET_ROOT/boot/extlinux.conf" "$WORK_ROOT/e5-cfg-before"
    EFI_TEST_EXT_FAIL=1
    if ( extlinux_default_entry_ensure ) > "$WORK_ROOT/e5.out" 2>&1; then
        ext_fail 'E5 accepted a failed update-extlinux trial'
    fi
    grep -Fq 'update-extlinux trial failed while selecting default label lts' "$WORK_ROOT/e5.out" \
        || ext_fail 'E5 did not name the failed trial'
    cmp -s "$WORK_ROOT/e5-conf-before" "$TARGET_ROOT/etc/update-extlinux.conf" \
        || ext_fail 'E5 rollback did not restore /etc/update-extlinux.conf'
    cmp -s "$WORK_ROOT/e5-cfg-before" "$TARGET_ROOT/boot/extlinux.conf" \
        || ext_fail 'E5 rollback did not restore /boot/extlinux.conf'
    [[ ! -e "$TARGET_ROOT/boot/extlinux.conf.new" ]] || ext_fail 'E5 left the trial candidate behind'

    # E6: the generated candidate drops an existing entry -> entry guard rolls
    # the selection back.
    reset_extlinux_fixture
    cp -a -- "$TARGET_ROOT/etc/update-extlinux.conf" "$WORK_ROOT/e6-conf-before"
    cp -a -- "$TARGET_ROOT/boot/extlinux.conf" "$WORK_ROOT/e6-cfg-before"
    EFI_TEST_EXT_FLAVORS="lts"
    if ( extlinux_default_entry_ensure ) > "$WORK_ROOT/e6.out" 2>&1; then
        ext_fail 'E6 accepted a candidate that removed an existing entry'
    fi
    grep -Fq 'removed an existing boot entry' "$WORK_ROOT/e6.out" \
        || ext_fail 'E6 did not name the entry loss'
    cmp -s "$WORK_ROOT/e6-conf-before" "$TARGET_ROOT/etc/update-extlinux.conf" \
        || ext_fail 'E6 rollback did not restore /etc/update-extlinux.conf'
    cmp -s "$WORK_ROOT/e6-cfg-before" "$TARGET_ROOT/boot/extlinux.conf" \
        || ext_fail 'E6 rollback did not restore /boot/extlinux.conf'

    # E7: the generated candidate does not mark the target label -> fail and
    # restore instead of installing a wrong default.
    reset_extlinux_fixture
    cp -a -- "$TARGET_ROOT/etc/update-extlinux.conf" "$WORK_ROOT/e7-conf-before"
    cp -a -- "$TARGET_ROOT/boot/extlinux.conf" "$WORK_ROOT/e7-cfg-before"
    EFI_TEST_EXT_MARK=0
    if ( extlinux_default_entry_ensure ) > "$WORK_ROOT/e7.out" 2>&1; then
        ext_fail 'E7 accepted a candidate without the target default marker'
    fi
    grep -Fq 'does not mark LABEL lts as the default' "$WORK_ROOT/e7.out" \
        || ext_fail 'E7 did not name the missing default marker'
    cmp -s "$WORK_ROOT/e7-conf-before" "$TARGET_ROOT/etc/update-extlinux.conf" \
        || ext_fail 'E7 rollback did not restore /etc/update-extlinux.conf'
    cmp -s "$WORK_ROOT/e7-cfg-before" "$TARGET_ROOT/boot/extlinux.conf" \
        || ext_fail 'E7 rollback did not restore /boot/extlinux.conf'

    # E8: diagnostics gate: available evidence names the extlinux candidate;
    # the missing-kernel state is unavailable with the exact reason.
    reset_extlinux_fixture
    if ! ( host_default_unavailable_reason ) > "$WORK_ROOT/e8a.out" 2>&1; then
        cat "$WORK_ROOT/e8a.out" >&2
        ext_fail 'E8 extlinux host default is not available'
    fi
    evidence="$(host_default_diagnostic_evidence)"
    grep -Fq 'Host default probe: candidate=extlinux label=lts kernel=6.18.52-0-lts' <<<"$evidence" \
        || ext_fail "E8 did not name the extlinux candidate: $evidence"
    caps="$(
        filesystem_scope_resolve() { :; }
        filesystem_scope_tools() { :; }
        diagnostic_repair_capabilities
    )"
    grep -Fqx 'Host default: available' <<<"$caps" \
        || ext_fail 'E8 diagnostics did not report Host default: available for extlinux'
    grep -Fq 'Host default probe: candidate=extlinux label=lts' <<<"$caps" \
        || ext_fail 'E8 diagnostics did not name the extlinux label'
    rm -f -- "$TARGET_ROOT/boot/vmlinuz-lts" "$TARGET_ROOT/usr/share/kernel/lts/kernel.release"
    caps="$(
        filesystem_scope_resolve() { :; }
        filesystem_scope_tools() { :; }
        diagnostic_repair_capabilities
    )"
    grep -Fq 'Host default: unavailable|the running kernel 6.18.52-0-lts has no entry in /boot/extlinux.conf' <<<"$caps" \
        || ext_fail 'E8 diagnostics did not fail closed for the missing running-kernel entry'

    # E9: read-only verdict for a configured default that names no entry or
    # disagrees with the current marker.
    reset_extlinux_fixture
    printf 'overwrite=1\ndefault=missing\n' > "$TARGET_ROOT/etc/update-extlinux.conf"
    verdict="$(extlinux_default_verdict)"
    grep -Fq "WARNING: extlinux default 'missing' does not name an entry; the first entry (LABEL virt) will boot." <<<"$verdict" \
        || ext_fail "E9 did not flag the stale default label: $verdict"
    printf 'overwrite=1\ndefault=lts\n' > "$TARGET_ROOT/etc/update-extlinux.conf"
    verdict="$(extlinux_default_verdict)"
    grep -Fq "WARNING: extlinux default 'lts' does not match the current default entry (LABEL virt)" <<<"$verdict" \
        || ext_fail "E9 did not flag the mismatched default label: $verdict"
    printf 'overwrite=1\ndefault=virt\n' > "$TARGET_ROOT/etc/update-extlinux.conf"
    [[ -z "$(extlinux_default_verdict)" ]] \
        || ext_fail 'E9 warned for a consistent default label'

    # E10: a detected syslinux.cfg instead of /boot/extlinux.conf fails closed.
    reset_extlinux_fixture
    mkdir -p "$TARGET_ROOT/boot/syslinux"
    mv -- "$TARGET_ROOT/boot/extlinux.conf" "$TARGET_ROOT/boot/syslinux/syslinux.cfg"
    reason_out="$(extlinux_default_entry_unavailable_reason || true)"
    grep -Fq 'is not /boot/extlinux.conf; refusing to select a default label' <<<"$reason_out" \
        || ext_fail "E10 accepted a non-/boot/extlinux.conf configuration: $reason_out"

    # E11: the update-extlinux configuration mode is preserved (a
    # password-protected conf must not be relaxed by the write).
    reset_extlinux_fixture
    chmod 0600 "$TARGET_ROOT/etc/update-extlinux.conf"
    if ! ( extlinux_default_entry_ensure ) > "$WORK_ROOT/e11.out" 2>&1; then
        cat "$WORK_ROOT/e11.out" >&2
        ext_fail 'E11 extlinux default ensure failed with a restricted conf'
    fi
    [[ "$(stat -c '%a' "$TARGET_ROOT/etc/update-extlinux.conf")" == 600 ]] \
        || ext_fail 'E11 relaxed the /etc/update-extlinux.conf mode'
) ; then
    fail_test 'extlinux default-label contract tests failed'
fi

# G8: Alpine GRUB EFI uses the same canonical EFI/alpine/grubx64.efi path and
# reports a byte-identical BOOTX64.EFI fallback without promoting it.
reset_generic_fixture
TARGET_OS_ID=alpine
rm -rf -- "$EFI_TEST_ESP_MOUNT/EFI/arch"
mkdir -p "$EFI_TEST_ESP_MOUNT/EFI/alpine"
printf 'fixture-grub\n' > "$EFI_TEST_ESP_MOUNT/EFI/alpine/grubx64.efi"
printf 'fixture-grub\n' > "$EFI_TEST_ESP_MOUNT/EFI/BOOT/BOOTX64.EFI"
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0002,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
if ! (
    is_alpine_family() { return 0; }
    alpine_efi_backend() { printf 'grub\n'; }
    run_host_default /dev/fakenvme /dev/fakeroot
) > "$WORK_ROOT/g8.out" 2>&1; then
    cat "$WORK_ROOT/g8.out" >&2
    fail_test 'G8 Alpine GRUB EFI run_host_default failed'
fi
grep -Fq "Host default: entry=Boot0009 label='alpine TestModel' loader=\\EFI\\alpine\\grubx64.efi action=created" "$WORK_ROOT/g8.out" \
    || fail_test 'G8 did not create the canonical Alpine entry'
grep -Fq 'byte-identical=yes' "$WORK_ROOT/g8.out" \
    || fail_test 'G8 did not report the byte-identical Alpine fallback'
grep -Eq '^BootOrder: 0009,0002,0001$' "$EFI_TEST_STATE" \
    || fail_test 'G8 did not promote the canonical Alpine entry'

# ---------------------------------------------------------------------------
# F1..F6: Fedora/RHEL GRUB2 BIOS default via grubenv saved_entry + BLS.
# ---------------------------------------------------------------------------
reset_fedora_fixture
if ! ( host_default_unavailable_reason ) > "$WORK_ROOT/f0.out" 2>&1; then
    cat "$WORK_ROOT/f0.out" >&2
    fail_test 'F0 Fedora BIOS host default is not available'
fi
if ! ( fedora_default_entry_ensure ) > "$WORK_ROOT/f1.out" 2>&1; then
    cat "$WORK_ROOT/f1.out" >&2
    fail_test 'F1 fedora_default_entry_ensure failed'
fi
grep -Fq "Fedora default entry: saved_entry=stale-entry resolves=no target=machine-6.8.0-300.fc44.x86_64 action=set" "$WORK_ROOT/f1.out" \
    || fail_test 'F1 did not report the stale-to-target transition'
grep -Fq 'Fedora default entry: saved_entry=machine-6.8.0-300.fc44.x86_64 resolves=yes target=machine-6.8.0-300.fc44.x86_64 action=set' "$WORK_ROOT/f1.out" \
    || fail_test 'F1 did not verify the written saved_entry'
grep -Fq 'Repair change status host-default: changed|grubenv saved_entry set to machine-6.8.0-300.fc44.x86_64' "$WORK_ROOT/f1.out" \
    || fail_test 'F1 change status is missing'
grep -q '^saved_entry=machine-6.8.0-300.fc44.x86_64$' "$TARGET_ROOT/boot/grub2/grubenv" \
    || fail_test 'F1 grubenv saved_entry was not written'
for key in 'blsdir=/boot/loader/entries' 'menu_auto_hide=1' 'boot_success=1'; do
    grep -q "^${key}$" "$TARGET_ROOT/boot/grub2/grubenv" \
        || fail_test "F1 grubenv key was not preserved: $key"
done
grep -Fq ' set saved_entry=machine-6.8.0-300.fc44.x86_64' "$EFI_TEST_EDITENV_CALLS" \
    || fail_test 'F1 did not invoke grub2-editenv set'

# F2: already correct -> unchanged, zero grub2-editenv calls.
: > "$EFI_TEST_EDITENV_CALLS"
if ! ( fedora_default_entry_ensure ) > "$WORK_ROOT/f2.out" 2>&1; then
    cat "$WORK_ROOT/f2.out" >&2
    fail_test 'F2 second fedora_default_entry_ensure failed'
fi
grep -Fq 'action=unchanged' "$WORK_ROOT/f2.out" \
    || fail_test 'F2 did not report unchanged'
grep -Fq 'Repair change status host-default: unchanged' "$WORK_ROOT/f2.out" \
    || fail_test 'F2 change status is not unchanged'
[[ ! -s "$EFI_TEST_EDITENV_CALLS" ]] \
    || fail_test 'F2 rewrote an already-correct saved_entry'

# F3: only the rescue entry -> fail closed naming the missing running-kernel BLS.
rm -f -- "$TARGET_ROOT/boot/loader/entries/machine-6.8.0-300.fc44.x86_64.conf" \
    "$TARGET_ROOT/boot/loader/entries/machine-6.7.0-100.fc44.x86_64.conf"
if ( fedora_default_entry_ensure ) > "$WORK_ROOT/f3.out" 2>&1; then
    fail_test 'F3 accepted a rescue-only BLS layout'
fi
grep -Fq 'the running kernel 6.8.0-300.fc44.x86_64 has no installed BLS entry' "$WORK_ROOT/f3.out" \
    || fail_test 'F3 did not name the missing running-kernel BLS entry'

# F4: invalid grubenv -> fail closed before any write.
reset_fedora_fixture
truncate -s 100 "$TARGET_ROOT/boot/grub2/grubenv"
if ( fedora_default_entry_ensure ) > "$WORK_ROOT/f4.out" 2>&1; then
    fail_test 'F4 accepted an invalid grubenv'
fi
grep -Fq 'grubenv is missing or not a valid GRUB environment block' "$WORK_ROOT/f4.out" \
    || fail_test 'F4 did not name the invalid grubenv'
[[ ! -s "$EFI_TEST_EDITENV_CALLS" ]] \
    || fail_test 'F4 wrote through an invalid grubenv'

# F5: grub2-editenv failure -> byte-identical rollback of the whole grubenv.
reset_fedora_fixture
cp -a -- "$TARGET_ROOT/boot/grub2/grubenv" "$WORK_ROOT/f5-before.txt"
EFI_TEST_EDITENV_FAIL=1
if ( fedora_default_entry_ensure ) > "$WORK_ROOT/f5.out" 2>&1; then
    fail_test 'F5 accepted a grub2-editenv failure'
fi
grep -Fq 'the previous grubenv was restored byte-identical' "$WORK_ROOT/f5.out" \
    || fail_test 'F5 did not report the grubenv rollback'
cmp -s "$WORK_ROOT/f5-before.txt" "$TARGET_ROOT/boot/grub2/grubenv" \
    || fail_test 'F5 rollback was not byte-identical'

# F6: target mode selects the newest non-rescue BLS entry and diagnostics
# report the BLS verdict; a stale saved_entry names the fallback entry.
reset_fedora_fixture
RUNNING_HOST_MODE=0
[[ "$(fedora_default_entry_target_id)" == machine-6.8.0-300.fc44.x86_64 ]] \
    || fail_test 'F6 target mode did not select the newest non-rescue BLS entry'
RUNNING_HOST_MODE=1
saved_evidence="$(grubenv_saved_entry_evidence)"
grep -Fq 'does not name an installed BLS entry' <<<"$saved_evidence" \
    || fail_test 'F6 stale BLS saved_entry was not reported'
grep -Fq 'GRUB falls back to the first entry (machine-6.8.0-300.fc44.x86_64)' <<<"$saved_evidence" \
    || fail_test 'F6 BLS fallback entry was not named'

# F7: a Fedora BIOS diagnostics run reports the host default capability.
fedora_caps="$(
    filesystem_scope_resolve() { :; }
    filesystem_scope_tools() { :; }
    diagnostic_repair_capabilities
)"
grep -Fqx 'Host default: available' <<<"$fedora_caps" \
    || fail_test 'F7 Fedora BIOS diagnostics did not report Host default: available'

# ---------------------------------------------------------------------------
# Order matrix: every starting state (already-correct, re-enumerated foreign
# first, fallback-only) converges to the same verified canonical entry and
# order while preserving foreign entries.  The repair stage order itself is
# intentionally enforced by run_repair and an out-of-order request is refused
# before any target work: the supported order is initramfs -> efi -> boot-stack
# -> grub; make-default is a separate host operation that is safe before or
# after any of them because it only ensures/promotes the verified host entry.
# ---------------------------------------------------------------------------
# O1: a prior EFI repair already created the canonical entry first -> verified
# no-op with a byte-identical firmware state.
reset_generic_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0009
BootOrder: 0009,0002,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0009* arch TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\arch\\grubx64.efi
EOF
cp -- "$EFI_TEST_STATE" "$WORK_ROOT/o1-before.txt"
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/o1.out" 2>&1; then
    cat "$WORK_ROOT/o1.out" >&2
    fail_test 'O1 run_host_default failed'
fi
grep -Fq 'Repair change status host-default: unchanged' "$WORK_ROOT/o1.out" \
    || fail_test 'O1 already-correct state was not unchanged'
cmp -s "$WORK_ROOT/o1-before.txt" "$EFI_TEST_STATE" \
    || fail_test 'O1 changed an already-correct firmware state'

# O2: firmware re-enumeration put a foreign entry first -> the canonical entry
# is re-promoted and the foreign entry is preserved.
reset_generic_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0001
BootOrder: 0001,0009,0002
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0009* arch TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\arch\\grubx64.efi
EOF
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/o2.out" 2>&1; then
    cat "$WORK_ROOT/o2.out" >&2
    fail_test 'O2 run_host_default failed'
fi
grep -Eq '^BootOrder: 0009,0002,0001$' "$EFI_TEST_STATE" \
    || fail_test 'O2 did not re-promote the canonical entry'
grep -q '^Boot0001' "$EFI_TEST_STATE" \
    || fail_test 'O2 lost the foreign entry'

# O3: fallback-only state -> the canonical entry is created and promoted; the
# final order is the same as O1/O2.
reset_generic_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0002,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/o3.out" 2>&1; then
    cat "$WORK_ROOT/o3.out" >&2
    fail_test 'O3 run_host_default failed'
fi
grep -Eq '^BootOrder: 0009,0002,0001$' "$EFI_TEST_STATE" \
    || fail_test 'O3 did not converge to the canonical-first order'

# O4: an out-of-order repair stage list is intentionally refused before any
# target work (grub before efi is the classic permutation).
if (
    parse_repair_arguments() { REPAIR_STAGES=(grub efi); REPAIR_POST_EFI=0; }
    run_repair
) > "$WORK_ROOT/o4.out" 2>&1; then
    fail_test 'O4 accepted out-of-order repair stages'
fi
grep -Fq 'out of safe dependency order' "$WORK_ROOT/o4.out" \
    || fail_test 'O4 did not name the refused stage order'

# T11: TUXEDO UKI idempotence: a second Make Default converges to the same
# verified entry and a byte-identical firmware state.
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0009
BootOrder: 0009,0002,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0009* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
EOF
cp -- "$EFI_TEST_STATE" "$WORK_ROOT/t11-before.txt"
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/t11.out" 2>&1; then
    cat "$WORK_ROOT/t11.out" >&2
    fail_test 'T11 run_host_default failed'
fi
grep -Fq 'Repair change status host-default: unchanged' "$WORK_ROOT/t11.out" \
    || fail_test 'T11 already-correct TUXEDO state was not unchanged'
cmp -s "$WORK_ROOT/t11-before.txt" "$EFI_TEST_STATE" \
    || fail_test 'T11 changed an already-correct firmware state'

# T12: label annotation carries the selected ESP's own drive model, so two NVMe
# installs of the same distribution stay distinguishable by label; identity
# remains PARTUUID + loader path and a foreign-labelled entry on another ESP is
# never touched.
reset_fixture
EFI_TEST_MODEL="SecondModel"
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0001
BootOrder: 0001,0005
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0005* Vendor Loader HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
EOF
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/t12.out" 2>&1; then
    cat "$WORK_ROOT/t12.out" >&2
    fail_test 'T12 run_host_default failed'
fi
grep -Fq "Host default: entry=Boot0005 label='TUXEDO UKI SecondModel' loader=\\EFI\\BOOT\\TUX.EFI action=relabeled" "$WORK_ROOT/t12.out" \
    || fail_test 'T12 did not use the selected ESP drive model in the label'
grep -Eq '^Boot0005\* TUXEDO UKI SecondModel HD\(1,GPT,' "$EFI_TEST_STATE" \
    || fail_test 'T12 did not relabel the host UKI entry'
grep -Eq '^Boot0001\* tuxedo HD\(1,GPT,' "$EFI_TEST_STATE" \
    || fail_test 'T12 touched a foreign-ESP entry'

# A1: model-suffix annotation unit contract: a model already present in the
# label is never appended twice, underscore/space spellings compare equal, a
# capacity-stripped model stem counts as present, and a generic label is
# annotated with the model exactly once.
efi_label_has_selected_model "arch TestModel" "TestModel" \
    || fail_test 'A1 exact model was not detected'
efi_label_has_selected_model "arch Test_Model" "Test Model" \
    || fail_test 'A1 underscore/space model spelling was not normalized'
efi_label_has_selected_model "arch Acme NVMe 990 PRO" "Acme NVMe 990 PRO 2TB" \
    || fail_test 'A1 capacity-stripped model stem was not detected'
if efi_label_has_selected_model "arch" "TestModel"; then
    fail_test 'A1 generic label was treated as already annotated'
fi
[[ "$(efi_label_with_selected_model "arch" "TestModel")" == "arch TestModel" ]] \
    || fail_test 'A1 model was not appended once'
[[ "$(efi_label_with_selected_model "arch TestModel" "TestModel")" == "arch TestModel" ]] \
    || fail_test 'A1 model was appended twice'
[[ "$(efi_label_with_selected_model "arch" "")" == "arch" ]] \
    || fail_test 'A1 empty model changed the label'

# A2: generic Arch loader identity is PARTUUID + loader path, never the label:
# a foreign-labelled entry that already points at the canonical
# \EFI\arch\grubx64.efi is relabeled in place with the drive model instead of a
# second entry being created.
reset_generic_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0005
BootOrder: 0005,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0005* Vendor Loader HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\arch\\grubx64.efi
EOF
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/a2.out" 2>&1; then
    cat "$WORK_ROOT/a2.out" >&2
    fail_test 'A2 generic relabel run_host_default failed'
fi
grep -Fq "Host default: entry=Boot0005 label='arch TestModel' loader=\\EFI\\arch\\grubx64.efi action=relabeled" "$WORK_ROOT/a2.out" \
    || fail_test 'A2 did not relabel the loader-identified entry with the drive model'
grep -Fq "$(printf '0005\tarch TestModel\t%s\t\\EFI\\arch\\grubx64.efi' "$HOST_PARTUUID")" "$EFI_TEST_LABEL_CALLS" \
    || fail_test 'A2 relabel did not carry the host PARTUUID and annotated label'
if grep -Fq -- '--create' "$EFI_TEST_CALLS"; then
    fail_test 'A2 created a duplicate instead of relabeling the loader-identified entry'
fi
[[ "$(grep -c '/\\EFI\\arch\\grubx64.efi' "$EFI_TEST_STATE" || true)" -eq 1 ]] \
    || fail_test 'A2 left a duplicate canonical entry'
grep -Eq '^BootOrder: 0005,' "$EFI_TEST_STATE" \
    || fail_test 'A2 did not keep the relabeled entry first'

# A3: an empty drive model (QEMU/virtio rigs) keeps the loader-id stem label
# without a trailing model suffix.
reset_generic_fixture
EFI_TEST_MODEL=""
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0002,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0002* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
if ! ( run_host_default /dev/fakenvme /dev/fakeroot ) > "$WORK_ROOT/a3.out" 2>&1; then
    cat "$WORK_ROOT/a3.out" >&2
    fail_test 'A3 empty-model run_host_default failed'
fi
grep -Fq "Host default: entry=Boot0009 label='arch' loader=\\EFI\\arch\\grubx64.efi action=created" "$WORK_ROOT/a3.out" \
    || fail_test 'A3 empty model did not keep the loader-id stem label'
grep -Eq '^Boot0009\* arch HD\(1,GPT,' "$EFI_TEST_STATE" \
    || fail_test 'A3 created entry label carries a model suffix'
if grep -Eq "^Boot0009\* arch .+ " "$EFI_TEST_STATE"; then
    fail_test 'A3 created entry label has a trailing model suffix'
fi

# G9: Alpine GRUB EFI: a foreign target-ESP entry with the same
# \EFI\alpine\grubx64.efi loader path is preserved after the host entry and is
# never pruned or promoted (identity is PARTUUID + loader).
reset_generic_fixture
TARGET_OS_ID=alpine
rm -rf -- "$EFI_TEST_ESP_MOUNT/EFI/arch"
mkdir -p "$EFI_TEST_ESP_MOUNT/EFI/alpine"
printf 'fixture-grub\n' > "$EFI_TEST_ESP_MOUNT/EFI/alpine/grubx64.efi"
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0003
BootOrder: 0003,0002,0001
Boot0001* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0002* alpine TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\alpine\\grubx64.efi
Boot0003* alpine TestModel HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\alpine\\grubx64.efi
EOF
if ! (
    is_alpine_family() { return 0; }
    alpine_efi_backend() { printf 'grub\n'; }
    run_host_default /dev/fakenvme /dev/fakeroot
) > "$WORK_ROOT/g9.out" 2>&1; then
    cat "$WORK_ROOT/g9.out" >&2
    fail_test 'G9 Alpine foreign same-path run_host_default failed'
fi
grep -Fq "Host default: entry=Boot0002 label='alpine TestModel' loader=\\EFI\\alpine\\grubx64.efi action=reused" "$WORK_ROOT/g9.out" \
    || fail_test 'G9 did not reuse the host-ESP canonical Alpine entry'
grep -Eq '^BootOrder: 0002,0001,0003$' "$EFI_TEST_STATE" \
    || fail_test 'G9 did not group the host entry ahead of the foreign same-path entry'
grep -Eq "^Boot0003\* alpine TestModel HD\(1,GPT,$FOREIGN_PARTUUID" "$EFI_TEST_STATE" \
    || fail_test 'G9 pruned or relabeled the foreign same-path entry'
[[ "$(grep -c '/\\EFI\\alpine\\grubx64.efi' "$EFI_TEST_STATE" || true)" -eq 2 ]] \
    || fail_test 'G9 lost one of the two same-loader entries'

# F8: next_entry and prev_saved_entry are deliberate state owned by GRUB and
# must survive the guarded saved_entry write untouched.
reset_fedora_fixture
{
    printf '# GRUB Environment Block\n'
    printf 'saved_entry=stale-entry\n'
    printf 'next_entry=machine-6.7.0-100.fc44.x86_64\n'
    printf 'prev_saved_entry=stale-entry\n'
    printf 'blsdir=/boot/loader/entries\n'
    printf 'menu_auto_hide=1\n'
    printf 'boot_success=1\n'
} > "$TARGET_ROOT/boot/grub2/grubenv"
truncate -s 1024 "$TARGET_ROOT/boot/grub2/grubenv"
if ! ( fedora_default_entry_ensure ) > "$WORK_ROOT/f8.out" 2>&1; then
    cat "$WORK_ROOT/f8.out" >&2
    fail_test 'F8 fedora_default_entry_ensure failed'
fi
grep -q '^saved_entry=machine-6.8.0-300.fc44.x86_64$' "$TARGET_ROOT/boot/grub2/grubenv" \
    || fail_test 'F8 did not set the running-kernel saved_entry'
for key in 'next_entry=machine-6.7.0-100.fc44.x86_64' 'prev_saved_entry=stale-entry' \
    'blsdir=/boot/loader/entries' 'menu_auto_hide=1' 'boot_success=1'; do
    grep -q "^${key}$" "$TARGET_ROOT/boot/grub2/grubenv" \
        || fail_test "F8 grubenv key was not preserved: $key"
done
grep -Fq ' set saved_entry=machine-6.8.0-300.fc44.x86_64' "$EFI_TEST_EDITENV_CALLS" \
    || fail_test 'F8 did not write only saved_entry through grub2-editenv'
if grep -Eq 'unset|next_entry=' "$EFI_TEST_EDITENV_CALLS"; then
    fail_test 'F8 touched next_entry or unset another grubenv key'
fi

# F9: a saved_entry naming the rescue entry is stale for the permanent default
# (the rescue entry is never a Make Default target) and is replaced by the
# running kernel's non-rescue BLS id.
reset_fedora_fixture
{
    printf '# GRUB Environment Block\n'
    printf 'saved_entry=machine-0-rescue\n'
    printf 'menu_auto_hide=1\n'
    printf 'boot_success=1\n'
} > "$TARGET_ROOT/boot/grub2/grubenv"
truncate -s 1024 "$TARGET_ROOT/boot/grub2/grubenv"
saved_evidence="$(grubenv_saved_entry_evidence)"
grep -Fq "WARNING: grubenv saved_entry 'machine-0-rescue' does not name an installed BLS entry" <<<"$saved_evidence" \
    || fail_test 'F9 rescue saved_entry was not reported as stale'
if ! ( fedora_default_entry_ensure ) > "$WORK_ROOT/f9.out" 2>&1; then
    cat "$WORK_ROOT/f9.out" >&2
    fail_test 'F9 fedora_default_entry_ensure failed'
fi
grep -Fq 'saved_entry=machine-0-rescue resolves=no target=machine-6.8.0-300.fc44.x86_64 action=set' "$WORK_ROOT/f9.out" \
    || fail_test 'F9 did not treat the rescue entry as a stale default'
grep -q '^saved_entry=machine-6.8.0-300.fc44.x86_64$' "$TARGET_ROOT/boot/grub2/grubenv" \
    || fail_test 'F9 did not set the running-kernel saved_entry'

echo "PASS: running-host TUXEDO UKI default contract is wired and fail-closed."
