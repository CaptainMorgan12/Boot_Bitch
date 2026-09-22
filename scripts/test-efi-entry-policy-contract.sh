#!/usr/bin/env bash
# Contract test for the EFI entry policy applied by the boot-stack reconcile
# path (annotate -> prune -> group).
#
# Policy under test:
#   * exactly one managed UKI, fallback and WebFAI entry per detected drive/ESP
#     on the selected system ESP, and at most one shim entry per drive;
#   * an existing entry is relabeled/reordered, never recreated (no duplicate
#     `efibootmgr --create` calls);
#   * firmware-generated device-path records are preserved exactly and never
#     displace the managed BOOTX64.EFI fallback;
#   * foreign-drive entries are preserved untouched;
#   * BootOrder groups every drive's entries contiguously, with removable
#     (no-PARTUUID) entries last.
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
# Fixture harness: stub efibootmgr, the label updater and the ESP probes.
# ---------------------------------------------------------------------------
WORK_ROOT="$(mktemp -d)"
source <(sed '/^main "\$@"$/d' "$HELPER")
trap - EXIT INT TERM HUP
trap 'rm -rf -- "$WORK_ROOT"' EXIT

SESSION_DIR="$WORK_ROOT/session"
SESSION_LOG="$SESSION_DIR/session.log"
EFI_TEST_STATE="$WORK_ROOT/nvram.txt"
EFI_TEST_CALLS="$WORK_ROOT/calls.log"
EFI_TEST_LABEL_CALLS="$WORK_ROOT/label-calls.log"
EFI_TEST_LABEL_UPDATER="$WORK_ROOT/efi-label-update"
HOST_PARTUUID="11111111-2222-3333-4444-555555555555"
FOREIGN_PARTUUID="66666666-7777-8888-9999-aaaaaaaaaaaa"
SECOND_FOREIGN_PARTUUID="bbbbbbbb-cccc-dddd-eeee-ffffffffffff"
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

reset_fixture()
{
    : > "$EFI_TEST_CALLS"
    : > "$EFI_TEST_LABEL_CALLS"
    : > "$EFI_TEST_STATE"
    rm -rf -- "$SESSION_DIR"
    mkdir -p "$SESSION_DIR"
    : > "$SESSION_LOG"
    RUNNING_HOST_MODE=1
    TARGET_OS_ID=tuxedo
    TARGET_ESP_MOUNT="/esp"
    EFI_ESP_SOURCE="/dev/fakehostesp"
    EFI_TEST_SECURE_BOOT=disabled
    EFI_TEST_MODEL=TestModel
    EFI_TEST_CREATE_ID=0009
    EFI_LABEL_UPDATER="$EFI_TEST_LABEL_UPDATER"
}

uefi_nvram_writable() { return 0; }
host_secure_boot_state() { printf '%s\n' "${EFI_TEST_SECURE_BOOT:-disabled}"; }
efi_selected_system_model() { printf '%s\n' "${EFI_TEST_MODEL-TestModel}"; }
efi_disk_and_partnum_for_esp() { printf '/dev/fakenvme\t1\n'; }
canonical_block()
{
    case "$1" in
        /dev/fakehostesp|/dev/fakenvme) printf '%s\n' "$1" ;;
        *) return 1 ;;
    esac
}
is_block_device() { canonical_block "$1" >/dev/null 2>&1; }
efi_set_inventory_esp_ids()
{
    EFI_HOST_ESP_SOURCE="/dev/fakehostesp"
    EFI_HOST_ESP_PARTUUID="$HOST_PARTUUID"
    EFI_HOST_ESP_MOUNT="/esp"
    EFI_TARGET_ESP_PARTUUID="$HOST_PARTUUID"
    EFI_ESP_SOURCE="/dev/fakehostesp"
}
efi_selected_wfai_loader() { printf '\\EFI\\FAI\\iPXE.efi\n'; }
efi_selected_generic_loader() { return 1; }
blkid()
{
    case "$*" in
        *'-s PARTUUID'*) printf '%s\n' "$HOST_PARTUUID" ;;
        *) return 1 ;;
    esac
}

assert_no_create()
{
    if grep -q -- '--create' "$EFI_TEST_CALLS"; then
        cat "$EFI_TEST_CALLS" >&2
        fail_test "$1 issued a create call instead of reusing/relabeling entries"
    fi
}

host_role_count()
{
    local wanted="$1"
    grep -E "^Boot[0-9A-Fa-f]{4}\*?[[:space:]]" "$EFI_TEST_STATE" \
        | while IFS= read -r line; do
            [[ "$(efi_entry_partuuid_line "$line")" == "$HOST_PARTUUID" ]] || continue
            [[ "$(efi_entry_policy_role "$line")" == "$wanted" ]] && printf 'x\n'
        done | grep -c . || true
}

# ---------------------------------------------------------------------------
# P1: the TUXEDO log state.  A managed fallback and a firmware device-path
# fallback coexist on the host ESP: annotate relabels the managed entry, prune
# must keep it (the observed regression removed it) and preserve the
# device-path record, and grouping keeps each drive's entries contiguous.
# ---------------------------------------------------------------------------
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0001,0002,0000,0004,0005,0006,0007,0003
Boot0000* TUXEDO UKI HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0001* UEFI: Foreign Disk, Partition 1 PciRoot(0x0)/HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)0000424f
Boot0002* UEFI: Host Disk, Partition 1 PciRoot(0x0)/HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)0000424f
Boot0003* UEFI: USB Stick, Partition 1 PciRoot(0x0)/HD(1,MBR,0xad2510c3,0x800,0x39c2800)0000424f
Boot0004* WFAI HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\FAI\\iPXE.efi
Boot0005* TUXEDO UKI Foreign Disk HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0006* WFAI Foreign Disk HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\FAI\\iPXE.efi
Boot0007* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
grep -E '^Boot000[156]' "$EFI_TEST_STATE" > "$WORK_ROOT/p1-foreign-before.txt"
if ! ( efi_annotate_selected_entries && efi_prune_selected_duplicate_destinations && efi_group_firmware_boot_order ) \
    > "$WORK_ROOT/p1.out" 2>&1; then
    cat "$WORK_ROOT/p1.out" >&2
    fail_test 'P1 annotate/prune/group failed'
fi
grep -Eq "^Boot0007\* UEFI OS TestModel HD\(1,GPT,$HOST_PARTUUID" "$EFI_TEST_STATE" \
    || fail_test 'P1 removed or failed to relabel the managed fallback entry'
grep -Eq "^Boot0002\* UEFI: Host Disk, Partition 1 PciRoot" "$EFI_TEST_STATE" \
    || fail_test 'P1 removed the firmware device-path entry'
grep -Eq "^Boot0000\* TUXEDO UKI TestModel HD\(1,GPT,$HOST_PARTUUID" "$EFI_TEST_STATE" \
    || fail_test 'P1 did not annotate the UKI label'
grep -Eq "^Boot0004\* WFAI TestModel HD\(1,GPT,$HOST_PARTUUID" "$EFI_TEST_STATE" \
    || fail_test 'P1 did not annotate the WebFAI label'
[[ "$(host_role_count uki)" -eq 1 ]] || fail_test 'P1 host drive has more than one UKI entry'
[[ "$(host_role_count fallback)" -eq 1 ]] || fail_test 'P1 host drive has more than one managed fallback entry'
[[ "$(host_role_count wfai)" -eq 1 ]] || fail_test 'P1 host drive has more than one WebFAI entry'
assert_no_create P1
cmp -s <(grep -E '^Boot000[156]' "$EFI_TEST_STATE") "$WORK_ROOT/p1-foreign-before.txt" \
    || fail_test 'P1 touched a foreign-drive entry'
grep -q '^BootOrder: 0000,0007,0004,0002,0005,0006,0001,0003$' "$EFI_TEST_STATE" \
    || fail_test "P1 BootOrder is not grouped per drive: $(grep '^BootOrder:' "$EFI_TEST_STATE")"

# Per-drive read-only evidence names the policy counts.
inventory="$(efi_print_firmware_inventory "$EFI_TEST_STATE")"
grep -Fq "EFI drive summary: partuuid=$HOST_PARTUUID class=host entries=4 uki=1 fallback=1 wfai=1 shim=0 device-path=1 loader=0 other=0" <<<"$inventory" \
    || { printf '%s\n' "$inventory" >&2; fail_test 'P1 host drive summary is wrong'; }
grep -Fq "EFI drive summary: partuuid=$FOREIGN_PARTUUID class=foreign entries=3 uki=1 fallback=0 wfai=1 shim=0 device-path=1 loader=0 other=0" <<<"$inventory" \
    || { printf '%s\n' "$inventory" >&2; fail_test 'P1 foreign drive summary is wrong'; }
grep -Fq 'EFI drive summary: partuuid=unknown class=unknown entries=1 uki=0 fallback=0 wfai=0 shim=0 device-path=1 loader=0 other=0' <<<"$inventory" \
    || { printf '%s\n' "$inventory" >&2; fail_test 'P1 removable drive summary is wrong'; }

# ---------------------------------------------------------------------------
# P2: duplicate managed destinations on the host ESP are pruned to exactly
# one, the active entry wins, and an existing loader-labelled entry is
# relabeled in place rather than recreated.
# ---------------------------------------------------------------------------
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0005
BootOrder: 0005,0006,0007,0008,0009,000A,0001
Boot0001* tuxedo HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
Boot0005* Vendor Loader HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0006* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0007* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0008* UEFI OS HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0009* WFAI HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\FAI\\iPXE.efi
Boot000A* WFAI HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\FAI\\iPXE.efi
EOF
if ! ( efi_annotate_selected_entries && efi_prune_selected_duplicate_destinations ) \
    > "$WORK_ROOT/p2.out" 2>&1; then
    cat "$WORK_ROOT/p2.out" >&2
    fail_test 'P2 annotate/prune failed'
fi
grep -Eq "^Boot0005\* Vendor Loader TestModel HD\(1,GPT,$HOST_PARTUUID" "$EFI_TEST_STATE" \
    || fail_test 'P2 did not relabel the existing host UKI entry in place'
[[ "$(host_role_count uki)" -eq 1 ]] || fail_test 'P2 kept duplicate UKI entries'
[[ "$(host_role_count fallback)" -eq 1 ]] || fail_test 'P2 kept duplicate fallback entries'
[[ "$(host_role_count wfai)" -eq 1 ]] || fail_test 'P2 kept duplicate WebFAI entries'
grep -Eq "^Boot0005\* " "$EFI_TEST_STATE" || fail_test 'P2 removed the active UKI entry'
if grep -Eq '^Boot0006\*|^Boot0008\*|^Boot000A\*' "$EFI_TEST_STATE"; then
    fail_test 'P2 kept a duplicate non-active destination'
fi
assert_no_create P2
grep -q '^Boot0001\* tuxedo ' "$EFI_TEST_STATE" || fail_test 'P2 lost the foreign entry'
grep -q '^BootOrder: 0005,0007,0009,0001$' "$EFI_TEST_STATE" \
    || fail_test "P2 BootOrder filter is wrong: $(grep '^BootOrder:' "$EFI_TEST_STATE")"

# ---------------------------------------------------------------------------
# P3: shim decision.  The legacy TUXEDO shim is pruned only when the running
# host proves Secure Boot disabled and a direct route exists; an active,
# Secure-Boot, unknown-state or target-mode shim is retained, at most one per
# drive, and a non-TUXEDO shim is never treated as legacy.
# ---------------------------------------------------------------------------
write_shim_state()
{
    cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: ${1:-0005}
BootOrder: 0005,0007,0008,0002
Boot0002* UEFI: Host Disk, Partition 1 PciRoot(0x0)/HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)0000424f
Boot0005* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0007* UEFI OS TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0008* TUXEDO HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
EOF
}

# (a) Secure Boot disabled + direct route -> legacy shim pruned.
reset_fixture
write_shim_state 0005
efi_prune_selected_duplicate_destinations > "$WORK_ROOT/p3a.out" 2>&1 || {
    cat "$WORK_ROOT/p3a.out" >&2
    fail_test 'P3a prune failed'
}
if grep -q 'shimx64.efi' "$EFI_TEST_STATE"; then
    fail_test 'P3a kept the redundant legacy TUXEDO shim under Secure Boot disabled'
fi
grep -q '^BootOrder: 0005,0007,0002$' "$EFI_TEST_STATE" \
    || fail_test "P3a BootOrder is wrong: $(grep '^BootOrder:' "$EFI_TEST_STATE")"

# (b) Secure Boot enabled -> the shim chain is needed and retained.
reset_fixture
write_shim_state 0005
EFI_TEST_SECURE_BOOT=enabled
efi_prune_selected_duplicate_destinations > "$WORK_ROOT/p3b.out" 2>&1 || {
    cat "$WORK_ROOT/p3b.out" >&2
    fail_test 'P3b prune failed'
}
[[ "$(grep -c 'shimx64.efi' "$EFI_TEST_STATE" || true)" -eq 1 ]] \
    || fail_test 'P3b did not retain exactly one shim entry under Secure Boot'

# (c) Secure Boot state unknown -> retained (fail safe).
reset_fixture
write_shim_state 0005
EFI_TEST_SECURE_BOOT=unknown
efi_prune_selected_duplicate_destinations > "$WORK_ROOT/p3c.out" 2>&1 || {
    cat "$WORK_ROOT/p3c.out" >&2
    fail_test 'P3c prune failed'
}
[[ "$(grep -c 'shimx64.efi' "$EFI_TEST_STATE" || true)" -eq 1 ]] \
    || fail_test 'P3c pruned a shim whose Secure Boot state is unknown'

# (d) Target mode: the target's Secure Boot state cannot be probed -> retained.
reset_fixture
write_shim_state 0005
RUNNING_HOST_MODE=0
efi_prune_selected_duplicate_destinations > "$WORK_ROOT/p3d.out" 2>&1 || {
    cat "$WORK_ROOT/p3d.out" >&2
    fail_test 'P3d prune failed'
}
[[ "$(grep -c 'shimx64.efi' "$EFI_TEST_STATE" || true)" -eq 1 ]] \
    || fail_test 'P3d pruned a shim in target mode'

# (e) The active shim is never removed.
reset_fixture
write_shim_state 0008
efi_prune_selected_duplicate_destinations > "$WORK_ROOT/p3e.out" 2>&1 || {
    cat "$WORK_ROOT/p3e.out" >&2
    fail_test 'P3e prune failed'
}
[[ "$(grep -c 'shimx64.efi' "$EFI_TEST_STATE" || true)" -eq 1 ]] \
    || fail_test 'P3e removed the active shim entry'

# (f) Duplicate shims collapse to one when the chain is needed.
reset_fixture
write_shim_state 0005
EFI_TEST_SECURE_BOOT=enabled
cat >> "$EFI_TEST_STATE" <<EOF
Boot0009* TUXEDO HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\tuxedo\\shimx64.efi
EOF
efi_prune_selected_duplicate_destinations > "$WORK_ROOT/p3f.out" 2>&1 || {
    cat "$WORK_ROOT/p3f.out" >&2
    fail_test 'P3f prune failed'
}
[[ "$(grep -c 'shimx64.efi' "$EFI_TEST_STATE" || true)" -eq 1 ]] \
    || fail_test 'P3f did not collapse duplicate shim entries to one'

# (g) A non-TUXEDO shim is a normal loader destination: it is never pruned as
# legacy while Secure Boot is disabled.
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0005
BootOrder: 0005,0007,0008
Boot0005* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0007* UEFI OS TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0008* debian HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\debian\\shimx64.efi
EOF
efi_prune_selected_duplicate_destinations > "$WORK_ROOT/p3g.out" 2>&1 || {
    cat "$WORK_ROOT/p3g.out" >&2
    fail_test 'P3g prune failed'
}
[[ "$(grep -c 'shimx64.efi' "$EFI_TEST_STATE" || true)" -eq 1 ]] \
    || fail_test 'P3g pruned a non-legacy vendor shim entry'

# ---------------------------------------------------------------------------
# P4: foreign-drive duplicates are preserved untouched; only the selected
# system ESP is maintained.
# ---------------------------------------------------------------------------
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0005
BootOrder: 0001,0002,0003,0004,0005,0006
Boot0001* TUXEDO UKI FOREIGN HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0002* TUXEDO UKI FOREIGN HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0003* UEFI OS FOREIGN HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
Boot0004* WFAI FOREIGN HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\FAI\\iPXE.efi
Boot0005* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0006* UEFI OS TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\BOOTX64.EFI
EOF
cp -- "$EFI_TEST_STATE" "$WORK_ROOT/p4-before.txt"
efi_prune_selected_duplicate_destinations > "$WORK_ROOT/p4.out" 2>&1 || {
    cat "$WORK_ROOT/p4.out" >&2
    fail_test 'P4 prune failed'
}
for id in 0001 0002 0003 0004; do
    grep -q "^Boot$id" "$EFI_TEST_STATE" || fail_test "P4 removed foreign entry Boot$id"
done
if grep -Eq -- '-b 000[1-4]' "$EFI_TEST_CALLS"; then
    fail_test 'P4 issued a delete for a foreign-drive entry'
fi
cmp -s <(grep -E '^Boot000[1-4]' "$EFI_TEST_STATE") <(grep -E '^Boot000[1-4]' "$WORK_ROOT/p4-before.txt") \
    || fail_test 'P4 modified a foreign-drive entry'

# ---------------------------------------------------------------------------
# P5: the observed interleaved BootOrder converges to per-drive groups with
# the removable entry last.
# ---------------------------------------------------------------------------
reset_fixture
cat > "$EFI_TEST_STATE" <<EOF
BootCurrent: 0002
BootOrder: 0000,0002,0004,0005,0001,0003,0006
Boot0000* TUXEDO UKI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0001* UEFI: Foreign Disk, Partition 1 PciRoot(0x0)/HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)0000424f
Boot0002* UEFI: Host Disk, Partition 1 PciRoot(0x0)/HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)0000424f
Boot0003* UEFI: USB Stick, Partition 1 PciRoot(0x0)/HD(1,MBR,0xad2510c3,0x800,0x39c2800)0000424f
Boot0004* WFAI TestModel HD(1,GPT,$HOST_PARTUUID,0x1000,0x200000)/\\EFI\\FAI\\iPXE.efi
Boot0005* TUXEDO UKI Foreign Disk HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\BOOT\\TUX.EFI
Boot0006* WFAI Foreign Disk HD(1,GPT,$FOREIGN_PARTUUID,0x1000,0x200000)/\\EFI\\FAI\\iPXE.efi
EOF
efi_group_firmware_boot_order > "$WORK_ROOT/p5.out" 2>&1 || {
    cat "$WORK_ROOT/p5.out" >&2
    fail_test 'P5 grouping failed'
}
grep -q '^BootOrder: 0000,0004,0002,0005,0006,0001,0003$' "$EFI_TEST_STATE" \
    || fail_test "P5 BootOrder is not grouped per drive: $(grep '^BootOrder:' "$EFI_TEST_STATE")"
# The foreign drive's entries stay contiguous and the USB entry is last.
order="$(sed -n 's/^BootOrder: //p' "$EFI_TEST_STATE" | head -n1)"
[[ "$order" == *'0005,0006,0001,0003' ]] \
    || fail_test 'P5 split the foreign drive group with the removable entry'

echo "PASS: EFI entry policy (one per drive, reuse/relabel, shim decision, foreign preservation, grouping) holds."
