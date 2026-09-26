#!/usr/bin/env bash
# Legacy helper smoke test: bash -n plus read-only validate/diagnose runs
# against a synthetic os-release-less Debian Etch root.
#
# The fixture root carries /etc/debian_version + a dpkg database, an
# update-grub-only GRUB legacy menu.lst, initramfs-tools tooling and an APT
# source, so root detection, the dpkg status fallback, the GRUB legacy branch
# and the 13-key capability report are exercised without touching a real disk.
#
# shellcheck disable=SC2034
# Several assignments (TARGET_ROOT, ROOT_CANONICAL, CHROOT_TRY_*) are consumed
# by the helper functions sourced below, so ShellCheck cannot see their readers.
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
HELPER="$ROOT_DIR/legacy/boot-repair-helper.sh"

fail()
{
    printf 'FAIL: %s\n' "$*" >&2
    exit 1
}

pass()
{
    printf 'ok - %s\n' "$*"
}

echo "== legacy helper smoke =="

[[ -f "$HELPER" ]] || fail "generated helper is missing: $HELPER"
bash -n "$HELPER" || fail "bash -n failed: $HELPER"
pass "bash -n"

FIXTURE="$(mktemp -d "${TMPDIR:-/tmp}/boot-repair-legacy-smoke.XXXXXX")"
smoke_cleanup()
{
    # BOOT_REPAIR_LEGACY_SMOKE_KEEP=1 preserves the fixture for evidence
    # capture (the guest-side Etch run collects the 13 capability lines).
    if [[ -n "${BOOT_REPAIR_LEGACY_SMOKE_KEEP:-}" ]]; then
        printf 'fixture kept: %s\n' "$FIXTURE"
        return 0
    fi
    rm -rf -- "$FIXTURE"
}
trap smoke_cleanup EXIT

mkdir -p "$FIXTURE/etc/apt" "$FIXTURE/etc/initramfs-tools" "$FIXTURE/var/lib/dpkg" \
    "$FIXTURE/boot/grub" "$FIXTURE/usr/sbin" "$FIXTURE/usr/bin" "$FIXTURE/usr/share/initramfs-tools" \
    "$FIXTURE/tmp" "$FIXTURE/session"

echo "4.0" > "$FIXTURE/etc/debian_version"
cat > "$FIXTURE/var/lib/dpkg/status" <<'EOF'
Package: broken
Status: install ok unpacked
Version: 1.0

Package: good
Status: install ok installed
Version: 2.0
EOF
printf 'deb http://archive.debian.org/debian etch main\n' > "$FIXTURE/etc/apt/sources.list"
: > "$FIXTURE/boot/vmlinuz-2.6.18-6-686"
: > "$FIXTURE/boot/initrd.img-2.6.18-6-686"
cat > "$FIXTURE/boot/grub/menu.lst" <<'EOF'
title		Debian GNU/Linux, kernel 2.6.18-6-686
kernel		/boot/vmlinuz-2.6.18-6-686 root=/dev/sda1 ro
initrd		/boot/initrd.img-2.6.18-6-686
EOF
cp -a "$FIXTURE/boot/grub/menu.lst" "$FIXTURE/menu.lst.orig"
for tool in usr/sbin/update-grub usr/sbin/update-initramfs usr/sbin/mkinitramfs \
    usr/sbin/dpkg usr/sbin/dpkg-query usr/bin/apt-get usr/sbin/e2fsck; do
    : > "$FIXTURE/$tool"
    chmod +x "$FIXTURE/$tool"
done

export BOOT_REPAIR_STATE_ROOT="$FIXTURE/state"
export TMPDIR="$FIXTURE/tmp"
export BOOT_REPAIR_LEGACY_SHIMS=force

# bash 3.1 cannot source a process substitution; stage the helper without the
# final `main "$@"` call in a real file instead.
HELPER_SRC="$FIXTURE/boot-repair-helper-sourced.sh"
sed '/^main "\$@"$/d' "$HELPER" > "$HELPER_SRC"
# shellcheck disable=SC1090
source "$HELPER_SRC"
trap - EXIT INT TERM HUP
trap smoke_cleanup EXIT
set +e

TARGET_ROOT="$FIXTURE"
ROOT_CANONICAL="/dev/null"
ROOT_DEVICE="/dev/null"
TARGET_DISK="/dev/null"
SESSION_LOG="$FIXTURE/session.log"
: > "$SESSION_LOG"
SESSION_DIR="$FIXTURE/session"

# --- legacy root confirmation -----------------------------------------------
read_target_os || fail "read_target_os refused the fixture root"
[[ "$TARGET_OS_LEGACY" == 1 ]] || fail "TARGET_OS_LEGACY was not set"
[[ "$TARGET_OS_ID" == debian ]] || fail "legacy OS id: $TARGET_OS_ID"
[[ "$TARGET_PRETTY" == *"legacy root"* ]] || fail "legacy OS label: $TARGET_PRETTY"
legacy_root_evidence_present "$FIXTURE" || fail "legacy_root_evidence_present"
legacy_root_evidence_label "$FIXTURE" | grep -q '/etc/debian_version' \
    || fail "legacy_root_evidence_label does not name the evidence file"

mkdir -p "$FIXTURE/empty"
TARGET_ROOT="$FIXTURE/empty"
( read_target_os ) 2>"$FIXTURE/refusal.txt" \
    && fail "read_target_os accepted a root without any evidence"
grep -q '/etc/debian_version' "$FIXTURE/refusal.txt" \
    || fail "refusal does not name the probed legacy evidence"
TARGET_ROOT="$FIXTURE"

# --- split-mount root evidence (etch2 split-LV layout) ----------------------
# /etc/debian_version paired with any ONE of the dpkg status pair, the APT
# sources list or /etc/inittab must confirm the root even when /var (and the
# dpkg database) live on a separate LV; /etc/debian_version alone must not.
mkdir -p "$FIXTURE/split-root/etc" "$FIXTURE/split-root/etc/apt"
printf '4.0\n' > "$FIXTURE/split-root/etc/debian_version"
printf 'id:2:initdefault:\n' > "$FIXTURE/split-root/etc/inittab"
legacy_root_evidence_present "$FIXTURE/split-root" \
    || fail "split-mount root with /etc/inittab evidence was refused"
legacy_root_evidence_label "$FIXTURE/split-root" | grep -q 'split-mount safe' \
    || fail "legacy_root_evidence_label lost the split-mount-safe wording"
rm -f "$FIXTURE/split-root/etc/inittab"
printf 'deb http://archive.debian.org/debian etch main\n' > "$FIXTURE/split-root/etc/apt/sources.list"
legacy_root_evidence_present "$FIXTURE/split-root" \
    || fail "split-mount root with the APT sources list was refused"
rm -rf "$FIXTURE/split-root/etc/apt"
legacy_root_evidence_present "$FIXTURE/split-root" \
    && fail "debian_version alone must not confirm a root"
rm -rf "$FIXTURE/split-root"
pass "split-mount root evidence (dpkg status / sources.list / inittab)"

# --- dpkg ${db:Status-*} fallback (dpkg 1.13 has no virtual fields) ----------
run_selected_chroot()
{
    local arg
    for arg in "$@"; do
        case "$arg" in
            *db:Status*) return 2 ;;
        esac
    done
    case "$*" in
        *'dpkg-query -W'*)
            printf 'install ok unpacked\ninstall ok installed\n'
            ;;
    esac
    return 0
}
legacy_dpkg_query_virtual_status_supported && fail "virtual db:Status must be reported unsupported"
dpkg_configuration_pending || fail "dpkg_configuration_pending missed an unpacked package"

run_selected_chroot()
{
    case "$*" in
        *'dpkg-query -W'*) printf 'install ok installed\n' ;;
    esac
    return 0
}
dpkg_configuration_pending && fail "dpkg_configuration_pending reported a clean database as pending"

run_selected_chroot()
{
    case "$*" in
        *'broken'*) printf 'install ok unpacked\n' ;;
        *'good'*) printf 'install ok installed\n' ;;
        *) return 2 ;;
    esac
    return 0
}
target_package_installed good || fail 'installed package not recognized through the ${Status} fallback'
target_package_installed broken && fail "unpacked package reported as installed"
pass "dpkg status fallback (pending detection and installed queries)"

# --- GRUB legacy branch ------------------------------------------------------
legacy_grub_legacy_target || fail "GRUB legacy target not detected"
[[ "$(grub_config_path)" == /boot/grub/menu.lst ]] || fail "grub_config_path: $(grub_config_path)"
grub_unavailable_reason >/dev/null || fail "grub capability refused the GRUB legacy target"
legacy_grub_entry_declarations "$FIXTURE/boot/grub/menu.lst" | grep -q '^kernel ' \
    || fail "GRUB legacy declarations lost the kernel line"

cat > "$FIXTURE/menu.lst.preserved" <<'EOF'
title		Debian GNU/Linux, kernel 2.6.18-6-686
kernel		/boot/vmlinuz-2.6.18-6-686 root=/dev/sda1 ro
initrd		/boot/initrd.img-2.6.18-6-686
title		Debian GNU/Linux, kernel 2.6.18-6-686 (single-user)
kernel		/boot/vmlinuz-2.6.18-6-686 root=/dev/sda1 ro single
initrd		/boot/initrd.img-2.6.18-6-686
EOF
legacy_grub_entry_declarations "$FIXTURE/menu.lst.orig" > "$FIXTURE/decl.before"
legacy_grub_guard_entries_preserved "$FIXTURE/decl.before" "$FIXTURE/menu.lst.preserved" \
    || fail "entry-preservation guard rejected an additive regeneration"

cat > "$FIXTURE/menu.lst.lossy" <<'EOF'
title		Debian GNU/Linux, kernel 2.6.18-6-686
kernel		/boot/vmlinuz-2.6.18-6-686 root=/dev/sda1 ro
EOF
legacy_grub_guard_entries_preserved "$FIXTURE/decl.before" "$FIXTURE/menu.lst.lossy" \
    && fail "entry-preservation guard accepted a lost initrd declaration"
grep -q 'preserved-declaration-required' "$SESSION_LOG" \
    || fail "entry-preservation guard left no evidence in the session log"

# A failed apply must restore the pre-repair menu.lst byte-for-byte.
(
    run_chroot_try()
    {
        CHROOT_TRY_RC=1
        CHROOT_TRY_OUTPUT="simulated update-grub failure"
    }
    legacy_grub_repair
) && fail "legacy_grub_repair reported success for a failed update-grub"
cmp -s "$FIXTURE/menu.lst.orig" "$FIXTURE/boot/grub/menu.lst" \
    || fail "failed GRUB legacy repair did not restore menu.lst"

# A lossy apply must be rolled back as well.
(
    run_chroot_try()
    {
        CHROOT_TRY_RC=0
        CHROOT_TRY_OUTPUT="simulated update-grub success"
        cp -a "$FIXTURE/menu.lst.lossy" "$TARGET_ROOT/boot/grub/menu.lst"
    }
    legacy_grub_repair
) && fail "legacy_grub_repair accepted a lossy menu.lst"
cmp -s "$FIXTURE/menu.lst.orig" "$FIXTURE/boot/grub/menu.lst" \
    || fail "lossy GRUB legacy repair did not restore menu.lst"
pass "GRUB legacy detection, entry preservation and rollback"

# --- cycle 12: GRUB defoptions/kopt expansion must not trip the guard -------
# The user's etch2 menu.lst carries `# defoptions=console=...` (the setup
# console args) while its kernel lines predate the edit; update-grub expands
# defoptions into the regenerated kernel lines, so the comparison must ignore
# the managed arguments (and still refuse genuine removals or drops).
cat > "$FIXTURE/menu.lst.defopts.before" <<'EOF'
# defoptions=console=ttyS0,115200 consoleblank=0
# kopt=root=/dev/mapper/debian-root ro

title		Debian GNU/Linux, kernel 2.6.18-6-686
kernel		/boot/vmlinuz-2.6.18-6-686 root=/dev/mapper/debian-root ro
initrd		/boot/initrd.img-2.6.18-6-686
EOF
managed="$(legacy_grub_managed_options "$FIXTURE/menu.lst.defopts.before")"
[[ "$managed" == *"console=ttyS0,115200"* ]] \
    || fail "managed defoptions/kopt arguments were not extracted"
[[ "$managed" == *"root=/dev/mapper/debian-root"* ]] \
    || fail "managed kopt arguments were not extracted"
LEGACY_GRUB_MANAGED="$managed"
legacy_grub_entry_declarations "$FIXTURE/menu.lst.defopts.before" "$LEGACY_GRUB_MANAGED" \
    > "$FIXTURE/decl.defopts.before"
cat > "$FIXTURE/menu.lst.defopts.after" <<'EOF'
# defoptions=console=ttyS0,115200 consoleblank=0
# kopt=root=/dev/mapper/debian-root ro

title		Debian GNU/Linux, kernel 2.6.18-6-686
kernel		/boot/vmlinuz-2.6.18-6-686 root=/dev/mapper/debian-root ro console=ttyS0,115200 consoleblank=0
initrd		/boot/initrd.img-2.6.18-6-686
EOF
legacy_grub_guard_entries_preserved "$FIXTURE/decl.defopts.before" \
    "$FIXTURE/menu.lst.defopts.after" \
    || fail "entry-preservation guard rejected the defoptions expansion"

cat > "$FIXTURE/menu.lst.defopts.dropped" <<'EOF'
title		Debian GNU/Linux, kernel 2.6.18-6-686
kernel		/boot/vmlinuz-2.6.18-6-686 root=/dev/mapper/debian-root ro
initrd		/boot/initrd.img-2.6.18-6-686
EOF
legacy_grub_guard_entries_preserved "$FIXTURE/decl.defopts.before" \
    "$FIXTURE/menu.lst.defopts.dropped" \
    && fail "entry-preservation guard accepted a regeneration that dropped the defoptions argument"
LEGACY_GRUB_MANAGED=""
pass "GRUB legacy defoptions/kopt expansion (guard accepts the expansion, refuses a drop)"

# --- A9-06: backslash and pipe are escaped for both BRE and ERE ---------------
escaped="$(legacy_grub_sed_escape 'a\b|c')"
[[ "$escaped" == 'a\\b\|c' ]] \
    || fail "sed escape did not escape backslash and pipe: $escaped"
escaped="$(legacy_grub_grep_escape 'a\b|c')"
[[ "$escaped" == 'a\\b\|c' ]] \
    || fail "grep escape did not escape backslash and pipe: $escaped"
# The escaped tokens must survive the real call-site delimiters/classes.
tok='a\b|c'
escaped="$(legacy_grub_sed_escape "$tok")"
printf '%s\n' 'before a\b|c after' | sed "s| $escaped | X |g" | grep -q 'before X after' \
    || fail "sed-escaped token broke the | delimiter"
escaped="$(legacy_grub_grep_escape "$tok")"
printf '%s\n' 'before a\b|c after' | grep -qE "(^|[[:space:]])$escaped([[:space:]]|$)" \
    || fail "grep-escaped token did not match the literal token"
pass "A9-06 escape classes (backslash and pipe escaped for BRE and ERE)"

# --- A9-12: noglob around the managed-token loops ----------------------------
# A managed token carrying wildcard characters must never be glob-expanded
# against the working directory.
mkdir -p "$FIXTURE/globdir"
: > "$FIXTURE/globdir/aXb"
cat > "$FIXTURE/globmenu.before" <<'EOF'
title		glob
kernel		/boot/vmlinuz-2.6.18-6-686 root=/dev/sda1 ro a*b
EOF
LEGACY_GRUB_MANAGED='a*b'
legacy_grub_entry_declarations "$FIXTURE/globmenu.before" "$LEGACY_GRUB_MANAGED" \
    > "$FIXTURE/glob.decl.before"
( cd "$FIXTURE/globdir" \
    && legacy_grub_guard_entries_preserved "$FIXTURE/glob.decl.before" "$FIXTURE/globmenu.before" ) \
    || fail "managed-token glob expansion broke the entry-preservation guard"
( cd "$FIXTURE/globdir" \
    && legacy_grub_entry_declarations "$FIXTURE/globmenu.before" "$LEGACY_GRUB_MANAGED" \
        | grep -q 'root=/dev/sda1 ro$' ) \
    || fail "managed-token glob expansion broke the declarations normalization"
LEGACY_GRUB_MANAGED=""
pass "A9-12 noglob around the managed-token loops"

# --- cycle 12: split-LV data mounts ------------------------------------------
mkdir -p "$FIXTURE/etc" "$FIXTURE/usr" "$FIXTURE/var/lib/dpkg" "$FIXTURE/tmp" "$FIXTURE/home" "$FIXTURE/opt"
cat > "$FIXTURE/etc/fstab" <<'EOF'
/dev/hda3	/	ext3	defaults,errors=remount-ro	0	1
/dev/hdb3	/usr	ext3	defaults	0	2
/dev/hdb4	/var	ext3	defaults	0	2
/dev/hdb5	/tmp	ext3	defaults	0	2
/dev/hdb6	/home	ext3	defaults	0	2
/dev/hdb7	none	swap	sw	0	0
proc		/proc	proc	defaults	0	0
none		/dev/shm	tmpfs	defaults	0	0
EOF
TARGET_ROOT="$FIXTURE"
TARGET_DISK=/dev/hdb
ROOT_DEVICE=/dev/hda3
ROOT_CANONICAL=/dev/hda3
MOUNTS=()
TARGET_DATA_MOUNTS=()
: > "$FIXTURE/mounts.log"
resolve_fstab_source() { printf '%s\n' "$1"; }
canonical_block() { printf '%s\n' "$1"; }
is_block_device() { [[ "$1" == /dev/hda3 || "$1" == /dev/hdb[3456] ]]; }
same_single_top_disk() { [[ "$1" == /dev/hdb && "$2" == /dev/hdb* ]]; }
mountpoint() { return 1; }
mount_recorded() {
    printf 'MOUNT %s\n' "$*" >> "$FIXTURE/mounts.log"
    MOUNTS+=("$2")
    return 0
}
mount_target_data_partitions ro
[[ "$(grep -c '^MOUNT ' "$FIXTURE/mounts.log")" -eq 4 ]] \
    || fail "split-LV ro mount did not mount exactly the four data entries"
grep -q "MOUNT /dev/hdb4 $FIXTURE/var -o ro" "$FIXTURE/mounts.log" \
    || fail "split-LV /var was not mounted read-only"
grep -q "MOUNT /dev/hdb3 $FIXTURE/usr -o ro" "$FIXTURE/mounts.log" \
    || fail "split-LV /usr was not mounted read-only"
grep -q '/home' "$FIXTURE/mounts.log" || fail "split-LV /home was not mounted"
grep -q '/tmp' "$FIXTURE/mounts.log" || fail "split-LV /tmp was not mounted"
grep -q 'swap' "$FIXTURE/mounts.log" && fail "swap was mounted as a data entry"
grep -q 'proc' "$FIXTURE/mounts.log" && fail "proc was mounted as a data entry"
grep -q 'tmpfs' "$FIXTURE/mounts.log" && fail "tmpfs was mounted as a data entry"
[[ "${#TARGET_DATA_MOUNTS[@]}" -eq 4 ]] || fail "the shared data mount records do not match the data mounts"

# A repair (rw) must fail closed when a required data mount fails.
mount_recorded() { return 1; }
( mount_target_data_partitions rw ) > "$FIXTURE/mounts-rw.log" 2>&1 \
    && fail "rw split-LV mount did not fail closed on a mount failure"
grep -q 'could not be mounted' "$FIXTURE/mounts-rw.log" \
    || fail "rw split-LV mount failure left no fail-closed evidence"
pass "split-LV data mounts (ro tolerant, rw fail-closed, pseudo entries skipped)"

# --- read-only validate / diagnose against the fixture root ------------------
prepare_target() { :; }
mount_target_boot_entry() { :; }
read_target_os
validate_target > "$FIXTURE/validate.txt" 2>&1
grep -q 'Validation summary' "$FIXTURE/validate.txt" || fail "validate produced no summary"
grep -q 'OS: Debian (legacy root' "$FIXTURE/validate.txt" || fail "validate did not report the legacy OS"
grep -q 'Repair change status validate: unchanged|validation is read-only' "$FIXTURE/validate.txt" \
    || fail "validate did not emit its change status"

run_target_diagnostic all > "$FIXTURE/diagnose.txt" 2>&1
[[ "$(grep -c '^Repair tool ' "$FIXTURE/diagnose.txt")" -eq 13 ]] \
    || fail "diagnose all did not emit exactly 13 Repair tool lines"
for key in validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub extlinux bootstack; do
    grep -q "^Repair tool ${key}: " "$FIXTURE/diagnose.txt" || fail "diagnose is missing Repair tool $key"
done
grep -q '^Repair tool efi: unavailable|legacy BIOS target; no EFI boot path is available$' \
    "$FIXTURE/diagnose.txt" || fail "diagnose did not gate EFI off with the BIOS reason"
grep -q '^Repair tool bootstack: available$' "$FIXTURE/diagnose.txt" \
    || fail "diagnose did not enable the legacy boot-stack pass for the GRUB-legacy fixture"
grep -q '^Repair tool grub: available$' "$FIXTURE/diagnose.txt" \
    || fail "diagnose did not enable the GRUB legacy branch"
grep -q '^Legacy feature file-copy: ' "$FIXTURE/diagnose.txt" \
    || fail "diagnose did not report legacy feature gating"
grep -q '^Diagnostic: luks$' "$FIXTURE/diagnose.txt" || fail "diagnose all skipped a section"
pass "read-only validate and diagnose against the fixture root"

# --- legacy SysV display-manager probe + guarded host repair -----------------
mkdir -p "$FIXTURE/etc/X11" "$FIXTURE/etc/init.d" "$FIXTURE/etc/rc2.d" "$FIXTURE/usr/bin"
printf '/usr/bin/kdm\n' > "$FIXTURE/etc/X11/default-display-manager"
: > "$FIXTURE/usr/bin/kdm"
chmod +x "$FIXTURE/usr/bin/kdm"
: > "$FIXTURE/etc/init.d/kdm"
printf 'id:2:initdefault:\n' > "$FIXTURE/etc/inittab"

RUNNING_HOST_MODE=1
[[ "$(legacy_sysv_display_manager_entry)" == /usr/bin/kdm ]] \
    || fail "legacy display entry probe: $(legacy_sysv_display_manager_entry)"
legacy_display_manager_probe | grep -q 'sysvinit display manager kdm' \
    || fail "legacy display probe evidence line"
# A9-10: uncanonical entries (. / .. / // segments) are refused before use.
printf '/usr/bin/../sbin/kdm\n' > "$FIXTURE/etc/X11/default-display-manager"
( legacy_sysv_display_manager_entry ) >/dev/null 2>&1 \
    && fail "display entry with a .. segment was accepted"
printf '/usr/bin//kdm\n' > "$FIXTURE/etc/X11/default-display-manager"
( legacy_sysv_display_manager_entry ) >/dev/null 2>&1 \
    && fail "display entry with a // segment was accepted"
printf '/usr/bin/kdm\n' > "$FIXTURE/etc/X11/default-display-manager"
[[ "$(legacy_sysv_display_manager_entry)" == /usr/bin/kdm ]] \
    || fail "canonical display entry was refused: $(legacy_sysv_display_manager_entry)"
display_unavailable_reason || fail "display capability refused the legacy SysV host"
repair_capability_evidence display | grep -q 'legacy SysV: sysvinit display manager kdm' \
    || fail "display capability evidence line"
RUNNING_HOST_MODE=0
reason="$(display_unavailable_reason)" \
    && fail "display capability must stay a host-scope stage for the offline scope"
[[ "$reason" == *'host-scope stage'* ]] || fail "offline display reason: $reason"
RUNNING_HOST_MODE=1

# Repair: the missing S-symlink is restored (the entry stays), the display
# manager is never started, and a second run reports unchanged.
rm -f "$FIXTURE/etc/rc2.d/S99kdm"
legacy_display_manager_repair > "$FIXTURE/display-repair.txt"
[[ "$(head -n1 "$FIXTURE/etc/X11/default-display-manager")" == /usr/bin/kdm ]] \
    || fail "display repair changed the configured entry"
[[ -L "$FIXTURE/etc/rc2.d/S99kdm" ]] || fail "display repair did not create the S-symlink"
[[ "$(readlink "$FIXTURE/etc/rc2.d/S99kdm")" == ../init.d/kdm ]] \
    || fail "display repair S-symlink target: $(readlink "$FIXTURE/etc/rc2.d/S99kdm")"
grep -q 'Repair change status display: changed' "$FIXTURE/display-repair.txt" \
    || fail "display repair did not report its change status"
display_out="$(legacy_display_manager_repair)"
printf '%s\n' "$display_out" | grep -q 'Repair change status display: unchanged' \
    || fail "idempotent display repair did not report unchanged"

# Refuse (fail closed): a configured entry whose binary is missing is never
# repaired and nothing is written.
printf '/usr/bin/xdm\n' > "$FIXTURE/etc/X11/default-display-manager"
( legacy_display_manager_repair ) >/dev/null 2>&1 \
    && fail "display repair accepted a configured entry with a missing binary"
printf '/usr/bin/kdm\n' > "$FIXTURE/etc/X11/default-display-manager"

# Rollback: a repair whose runlevel write fails restores the entry backup
# (the entry content differs from the configured token, so it is rewritten
# first) and leaves no S-symlink behind.
printf '/usr/bin/kdm /usr/bin/xdm\n' > "$FIXTURE/etc/X11/default-display-manager"
rm -f "$FIXTURE/etc/rc2.d/S99kdm"
chmod 555 "$FIXTURE/etc/rc2.d"
( legacy_display_manager_repair ) >/dev/null 2>&1 \
    && fail "display repair accepted an uncreatable runlevel directory"
chmod 755 "$FIXTURE/etc/rc2.d"
[[ "$(head -n1 "$FIXTURE/etc/X11/default-display-manager")" == '/usr/bin/kdm /usr/bin/xdm' ]] \
    || fail "display repair rollback did not restore the entry backup"
[[ ! -L "$FIXTURE/etc/rc2.d/S99kdm" ]] || fail "display repair rollback left the S-symlink behind"
printf '/usr/bin/kdm\n' > "$FIXTURE/etc/X11/default-display-manager"
pass "legacy SysV display-manager probe, host repair and rollback"

# --- guarded plain-chroot fallback (B7-5 shell/host-shell) -------------------
# On a BIOS-only recovery environment (BOOT_REPAIR_LEGACY_HOST_EFI=no, the
# read-only test seam for the firmware probe) the fallback covers
# host-maintenance and host-shell; the offline shell runs through the guarded
# plain chroot with chroot alone.
mkdir -p "$FIXTURE/chroot-only"
ln -s "$(command -v chroot)" "$FIXTURE/chroot-only/chroot"
BOOT_REPAIR_LEGACY_HOST_EFI=no PATH="$FIXTURE/chroot-only" legacy_feature_reason host-maintenance \
    || fail "host-maintenance must be available through the plain-chroot fallback"
BOOT_REPAIR_LEGACY_HOST_EFI=no PATH="$FIXTURE/chroot-only" legacy_feature_reason host-shell \
    || fail "host-shell must be available on a BIOS-only host"
PATH="$FIXTURE/chroot-only" legacy_feature_reason shell \
    || fail "shell must be available through the guarded plain chroot"
PATH="$FIXTURE/chroot-only" legacy_feature_reason file-copy >/dev/null 2>&1 \
    && fail "file-copy must stay unavailable without cp/cmp/find on PATH"
pass "guarded plain-chroot fallback gates (shell, host-shell, host-maintenance)"

# --- legacy boot-stack availability (B7-7) -----------------------------------
# The fixture is a GRUB-legacy target with initramfs-tools: the legacy
# boot-stack pass is available with the legacy evidence line.
bootstack_unavailable_reason || fail "bootstack capability refused the GRUB-legacy fixture"
repair_capability_evidence bootstack | grep -q 'legacy boot-stack' \
    || fail "bootstack capability evidence: $(repair_capability_evidence bootstack)"
pass "legacy boot-stack capability (GRUB-legacy pass)"

# --- legacy file-copy backend (B7-6) -----------------------------------------
# cp -a + chown --reference + cmp: a small legacy copy round-trip inside the
# fixture (no VM, no block device).
mkdir -p "$FIXTURE/copy-src" "$FIXTURE/copy-dst"
printf 'hello legacy copy\n' > "$FIXTURE/copy-src/a.txt"
printf 'second file\n' > "$FIXTURE/copy-src/b.txt"
(
    legacy_run_copy_item copy "$FIXTURE/copy-src" "$FIXTURE/copy-dst" ""
) || fail "legacy cp -a copy failed"
[[ "$(legacy_verify_copy_item "$FIXTURE/copy-src" "$FIXTURE/copy-dst" | tail -1)" == 2 ]] \
    || fail "legacy cmp verification did not report 2 verified files"
cmp -s "$FIXTURE/copy-src/a.txt" "$FIXTURE/copy-dst/copy-src/a.txt" \
    || fail "legacy copy content mismatch"
(
    legacy_run_copy_item preview "$FIXTURE/copy-src" "$FIXTURE/copy-dst" ""
) || fail "legacy copy preview failed"
[[ -e "$FIXTURE/copy-dst/copy-src" ]] || fail "legacy copy preview wrote the destination"
pass "legacy file-copy backend (cp -a, cmp verification, preview is read-only)"

# --- legacy unlock keyfile handling (cycle 9 loop fix) -----------------------
# The passphrase read from stdin tolerates exactly one trailing newline/CR
# (cryptsetup 1.0 would otherwise treat it as key material), lands in a
# mode-600 keyfile under the session state, and the caller deletes it after
# the open attempt.
mkdir -p "$FIXTURE/state"
SESSION_DIR="$FIXTURE/state"
printf 'smoke-test-key\n' | legacy_unlock_keyfile_from_stdin "$FIXTURE/state/key" \
    > "$FIXTURE/state/key.path" \
    || fail "newline-tolerant unlock key write failed"
key_path="$(cat "$FIXTURE/state/key.path")"
[[ -f "$key_path" ]] || fail "unlock keyfile was not created"
[[ "$(cat "$key_path")" == "smoke-test-key" ]] \
    || fail "unlock keyfile kept the trailing newline"
[[ "$(stat -c '%a' "$key_path")" == "600" ]] \
    || fail "unlock keyfile is not mode 600"
printf 'smoke-test-key\r\n' | legacy_unlock_keyfile_from_stdin "$FIXTURE/state/key2" \
    >/dev/null || fail "CRLF unlock key write failed"
[[ "$(cat "$FIXTURE/state/key2")" == "smoke-test-key" ]] \
    || fail "unlock keyfile kept the CRLF terminator"
printf '\n' | legacy_unlock_keyfile_from_stdin "$FIXTURE/state/key3" \
    >/dev/null 2>&1 && fail "an empty unlock passphrase was accepted"
rm -f "$FIXTURE/state/key" "$FIXTURE/state/key2" "$FIXTURE/state/key3" "$FIXTURE/state/key.path"
pass "legacy unlock keyfile (one trailing newline/CR stripped, mode 600, empty refused)"

# The GUI --key-file channel: the argument file must be a regular file owned
# by the caller, its content (one trailing newline tolerated) lands in the
# session keyfile, and the argument file itself is NEVER deleted by the
# helper (the Qt3 GUI unlinks its own file when the command finishes).
printf 'smoke-test-key\n' > "$FIXTURE/state/gui-key"
out_path="$(legacy_unlock_keyfile_from_file "$FIXTURE/state/gui-key" "$FIXTURE/state/session-key")" \
    || fail "GUI keyfile channel failed"
[[ "$(cat "$out_path")" == "smoke-test-key" ]] \
    || fail "GUI keyfile channel kept the trailing newline"
[[ "$(stat -c '%a' "$out_path")" == "600" ]] \
    || fail "GUI keyfile channel session keyfile is not mode 600"
printf 'smoke-test-key' > "$FIXTURE/state/gui-key2"
chmod 600 "$FIXTURE/state/gui-key2"
legacy_unlock_keyfile_from_file "$FIXTURE/state/gui-key2" "$FIXTURE/state/session-key2" \
    >/dev/null || fail "GUI keyfile channel without a newline failed"
[[ "$(cat "$FIXTURE/state/session-key2")" == "smoke-test-key" ]] \
    || fail "GUI keyfile channel mangled the passphrase"
rm -f "$FIXTURE/state/gui-key" "$FIXTURE/state/gui-key2" \
    "$FIXTURE/state/session-key" "$FIXTURE/state/session-key2"
pass "legacy GUI keyfile channel (regular file, mode 600, newline tolerated, caller file survives)"

# --- batch B2: unlock keyfile hardening (A9-01/A9-02/A9-14) ------------------
set +e
mkdir -p "$FIXTURE/unlock-state" "$FIXTURE/unlock-keydir"
printf 'batch-b2-key\n' > "$FIXTURE/unlock-keydir/plain-key"
key_canon="$(readlink -f -- "$FIXTURE/unlock-keydir/plain-key")"
ln -s "$FIXTURE/unlock-keydir" "$FIXTURE/unlock-keydir-link"

# A9-02: the unlock session directory is created (mode 0700) under STATE_ROOT
# when none exists, is reused afterwards, and hosts the session keyfile so it
# never lands at /unlock-keyfile.
(
    trap - EXIT
    STATE_ROOT="$FIXTURE/ensure-state"
    mkdir -p "$STATE_ROOT"
    SESSION_DIR=""
    SESSION_LOG=""
    legacy_unlock_ensure_session
    first="$SESSION_DIR"
    [[ "$first" == "$FIXTURE/ensure-state/session."* ]] \
        || fail "ensure session did not create the directory under STATE_ROOT"
    [[ "$(stat -c '%a' "$first")" == 700 ]] \
        || fail "ensure session directory is not mode 700"
    [[ "$SESSION_LOG" == "$first/session.log" ]] \
        || fail "ensure session did not set SESSION_LOG"
    [[ -f "$SESSION_LOG" ]] || fail "ensure session did not create the session log"
    legacy_unlock_ensure_session
    [[ "$SESSION_DIR" == "$first" ]] \
        || fail "ensure session did not reuse the existing session directory"
    key_path="$(printf 'k\n' | legacy_unlock_keyfile_from_stdin "$SESSION_DIR/unlock-keyfile")"
    [[ "$(dirname -- "$key_path")" == "$SESSION_DIR" ]] \
        || fail "session keyfile escaped the session directory: $key_path"
    rm -f -- "$key_path"
    exit 0
)
rc=$?
[[ $rc -eq 0 ]] || fail "unlock session ensure checks failed"
pass "unlock session ensure (created 0700 under STATE_ROOT, reused, keyfile never at /unlock-keyfile)"

# A9-01: provable ownership only - the effective-uid acceptance is gone.
uid="$(id -u)"
SUDO_UID="$uid" legacy_unlock_keyfile_owner_proven "$key_canon" "" \
    || fail "SUDO_UID match was refused"
SUDO_UID="$uid" legacy_unlock_keyfile_owner_proven "$key_canon" "$uid" \
    || fail "SUDO_UID plus matching key-owner was refused"
SUDO_UID="$uid" legacy_unlock_keyfile_owner_proven "$key_canon" "$((uid + 1))" \
    && fail "a key-owner different from SUDO_UID was accepted"
unset SUDO_UID
# A root- or user-owned file with no SUDO_UID and no key-owner can never be
# proven (this is the old "accept the effective uid" path, now removed).
legacy_unlock_keyfile_owner_proven "$key_canon" "" \
    && fail "a keyfile with no SUDO_UID and no key-owner was accepted"
legacy_unlock_keyfile_owner_proven "$key_canon" "$uid" \
    || fail "the gksu-style key-owner match was refused"
legacy_unlock_keyfile_owner_proven "$key_canon" "0" \
    && fail "a key-owner of 0 without SUDO_UID was accepted"
pass "unlock keyfile owner proof (SUDO_UID / gksu-style key-owner, provable only)"

# A9-01: symlinked paths are refused; a plain canonical path is accepted.
legacy_unlock_keyfile_path_safe "$key_canon" \
    || fail "a plain canonical keyfile path was refused"
legacy_unlock_keyfile_path_safe "$FIXTURE/unlock-keydir-link/plain-key" \
    && fail "a keyfile reached through a symlinked intermediate directory was accepted"
ln -s "$key_canon" "$FIXTURE/unlock-keydir/final-link"
legacy_unlock_keyfile_path_safe "$FIXTURE/unlock-keydir/final-link" \
    && fail "a symlinked keyfile final component was accepted"
pass "unlock keyfile path proof (plain path accepted, symlinked paths refused)"

# A9-14: the first-line read is capped at 1024 characters and the keyfile is
# sanity-capped at 65536 bytes (both the file and the stdin channels).
head -c 2048 /dev/zero | tr '\0' 'x' > "$FIXTURE/unlock-keydir/long-line-key"
printf '\n' >> "$FIXTURE/unlock-keydir/long-line-key"
( legacy_unlock_keyfile_from_file "$FIXTURE/unlock-keydir/long-line-key" \
    "$FIXTURE/state/cap-key" ) >/dev/null 2>&1 \
    && fail "a 2 KiB first line was accepted"
head -c 1024 /dev/zero | tr '\0' 'x' > "$FIXTURE/unlock-keydir/exact-line-key"
printf '\n' >> "$FIXTURE/unlock-keydir/exact-line-key"
( legacy_unlock_keyfile_from_file "$FIXTURE/unlock-keydir/exact-line-key" \
    "$FIXTURE/state/cap-key" ) >/dev/null 2>&1 \
    && fail "an exactly-1024-character first line was accepted"
head -c 65537 /dev/zero > "$FIXTURE/unlock-keydir/oversize-key"
( legacy_unlock_keyfile_from_file "$FIXTURE/unlock-keydir/oversize-key" \
    "$FIXTURE/state/cap-key" ) >/dev/null 2>&1 \
    && fail "a keyfile over the 65536-byte stat cap was accepted"
head -c 1024 /dev/zero | tr '\0' 'y' | legacy_unlock_keyfile_from_stdin "$FIXTURE/state/cap-key" \
    >/dev/null 2>&1 \
    && fail "an exactly-1024-character stdin passphrase was accepted"
head -c 2048 /dev/zero | tr '\0' 'y' | legacy_unlock_keyfile_from_stdin "$FIXTURE/state/cap-key" \
    >/dev/null 2>&1 \
    && fail "an over-cap stdin passphrase was accepted"
pass "unlock keyfile length caps (1024-char first line, 65536-byte stat cap, file and stdin)"

# A9-01/A9-02 through unlock_target itself: the accepted path creates the
# 0700 session under STATE_ROOT and never deletes the caller file; refused
# paths fail before any session state is created.
mkdir -p "$FIXTURE/unlock-run1" "$FIXTURE/unlock-run2" "$FIXTURE/unlock-run3"
set +e
# Accepted (SUDO_UID matches): runs through to the stubbed open and then
# fails at the mapper-appeared check - after the session directory exists.
(
    trap - EXIT
    canonical_block() { printf '%s\n' "$1"; }
    assert_target_not_host() { return 0; }
    same_single_top_disk() { return 0; }
    lsblk() { return 0; }
    cryptsetup() {
        case "$1" in
            isLuks) return 0 ;;
            luksUUID) printf '11111111-2222-3333-4444-555555555555\n' ;;
            luksOpen) return 0 ;;
            luksClose) return 0 ;;
            *) return 1 ;;
        esac
    }
    find_crypt_mapper_for_device() { return 1; }
    SESSION_DIR=""
    STATE_ROOT="$FIXTURE/unlock-run1"
    SUDO_UID="$uid"
    TARGET_DISK=/dev/null
    ROOT_DEVICE=/dev/null
    unlock_target --key-file "$key_canon" >/dev/null 2>&1
    rc=$?
    exit "$rc"
)
unlock_rc=$?
[[ $unlock_rc -ne 0 ]] || fail "stubbed unlock unexpectedly succeeded"
session_dirs=("$FIXTURE/unlock-run1"/session.*)
[[ ${#session_dirs[@]} -eq 1 && -d "${session_dirs[0]}" ]] \
    || fail "accepted unlock did not create its session directory"
[[ "$(stat -c '%a' "${session_dirs[0]}")" == 700 ]] \
    || fail "accepted unlock session directory is not mode 700"
[[ -f "$key_canon" ]] || fail "accepted unlock deleted the caller-supplied keyfile"
# Refused (no SUDO_UID, no key-owner): fails before any session state exists
# and the caller file survives.
(
    trap - EXIT
    canonical_block() { printf '%s\n' "$1"; }
    assert_target_not_host() { return 0; }
    same_single_top_disk() { return 0; }
    lsblk() { return 0; }
    cryptsetup() {
        case "$1" in
            isLuks) return 0 ;;
            luksUUID) printf '11111111-2222-3333-4444-555555555555\n' ;;
            luksOpen) return 0 ;;
            luksClose) return 0 ;;
            *) return 1 ;;
        esac
    }
    find_crypt_mapper_for_device() { return 1; }
    SESSION_DIR=""
    STATE_ROOT="$FIXTURE/unlock-run2"
    unset SUDO_UID
    TARGET_DISK=/dev/null
    ROOT_DEVICE=/dev/null
    unlock_target --key-file "$key_canon" >/dev/null 2>"$FIXTURE/unlock-refusal.txt"
    rc=$?
    exit "$rc"
)
unlock_rc=$?
[[ $unlock_rc -ne 0 ]] || fail "unprovable-owner unlock unexpectedly succeeded"
grep -q 'cannot be proven' "$FIXTURE/unlock-refusal.txt" \
    || fail "unprovable-owner refusal lacks the proof wording"
[[ -z "$(find "$FIXTURE/unlock-run2" -mindepth 1 2>/dev/null | head -n1)" ]] \
    || fail "refused unlock created session state"
[[ -f "$key_canon" ]] || fail "refused unlock deleted the caller-supplied keyfile"
# Refused (symlinked intermediate directory): same guarantees.
(
    trap - EXIT
    canonical_block() { printf '%s\n' "$1"; }
    assert_target_not_host() { return 0; }
    same_single_top_disk() { return 0; }
    lsblk() { return 0; }
    cryptsetup() {
        case "$1" in
            isLuks) return 0 ;;
            luksUUID) printf '11111111-2222-3333-4444-555555555555\n' ;;
            luksOpen) return 0 ;;
            luksClose) return 0 ;;
            *) return 1 ;;
        esac
    }
    find_crypt_mapper_for_device() { return 1; }
    SESSION_DIR=""
    STATE_ROOT="$FIXTURE/unlock-run3"
    SUDO_UID="$uid"
    TARGET_DISK=/dev/null
    ROOT_DEVICE=/dev/null
    unlock_target --key-file "$FIXTURE/unlock-keydir-link/plain-key" >/dev/null 2>&1
    rc=$?
    exit "$rc"
)
unlock_rc=$?
[[ $unlock_rc -ne 0 ]] || fail "symlinked-keyfile unlock unexpectedly succeeded"
[[ -z "$(find "$FIXTURE/unlock-run3" -mindepth 1 2>/dev/null | head -n1)" ]] \
    || fail "symlink-refused unlock created session state"
pass "unlock_target keyfile acceptance (0700 session on accept, refusals leave no state, caller file survives)"

# The path-based blkid TYPE probe parses both the modern `-o value -s TYPE`
# and the Etch-era bare `TYPE="..."` output, and probes the given path
# directly (no basename/readlink translation).
mkdir -p "$FIXTURE/blkid-stub"
cat > "$FIXTURE/blkid-stub/blkid" <<'EOF'
#!/bin/sh
if [ "$1" = "-o" ]; then
    printf 'ext3\n'
else
    printf '/dev/mapper/smoke: UUID="x" TYPE="reiserfs"\n'
fi
EOF
chmod +x "$FIXTURE/blkid-stub/blkid"
legacy_real_tool_path() { printf '%s\n' "$FIXTURE/blkid-stub/blkid"; }
[[ "$(legacy_blkid_value_path /dev/mapper/smoke)" == "ext3" ]] \
    || fail "path-based blkid probe did not parse the modern output"
legacy_real_tool_path() { printf '%s\n' "$FIXTURE/blkid-stub/blkid-old"; }
cat > "$FIXTURE/blkid-stub/blkid-old" <<'EOF'
#!/bin/sh
if [ "$1" = "-o" ]; then
    exit 1
fi
printf '/dev/mapper/smoke: UUID="x" TYPE="reiserfs"\n'
EOF
chmod +x "$FIXTURE/blkid-stub/blkid-old"
[[ "$(legacy_blkid_value_path /dev/mapper/smoke)" == "reiserfs" ]] \
    || fail "path-based blkid probe did not parse the Etch-era TYPE= output"
pass "legacy path-based blkid TYPE probe (modern and Etch-era output)"

# --- cycle 12: existing-mapper unlock still probes the root candidate --------
# A previous session may have left the mapper open: the reuse path must emit
# UNLOCKED_ROOT for the already-open chain (a fresh GUI process cannot see
# the LVs in its read-only inventory otherwise).
find() {
    if [[ "$1" == /dev/mapper ]]; then
        printf '%s\n' /dev/mapper/debian-root /dev/mapper/debian-usr \
            /dev/mapper/luks-etchroot
        return 0
    fi
    command find "$@"
}
legacy_blkid_value_path() {
    case "$1" in
        *debian-root*|*debian-usr*) printf 'ext3\n' ;;
        *) return 0 ;;
    esac
}
probe_out="$(legacy_unlock_root_probe /dev/mapper/luks-etchroot)"
printf '%s\n' "$probe_out" | grep -q 'UNLOCKED_ROOT=/dev/mapper/debian-root' \
    || fail "existing-mapper probe did not emit the unlocked root"
printf '%s\n' "$probe_out" | grep -q 'UNLOCKED_ROOT_FSTYPE=ext3' \
    || fail "existing-mapper probe did not emit the unlocked root fstype"
pass "legacy unlock root probe on the already-open mapper chain"

# --- cycle 12: the file system check skips devices mounted under the target --
# The legacy check keeps the target's helper-owned mounts and must emit the
# modern offline-only skip lines instead of running an offline fsck over a
# live filesystem on Etch's IDE/PIO disk.
cat > "$FIXTURE/fs-check-mounts.txt" <<'EOF'
/dev/mapper/debian-root /tmp/boot-repair-session/session.xxxx/mount
EOF
(
    findmnt() { cat "$FIXTURE/fs-check-mounts.txt"; }
    canonical_block() { printf '%s\n' "$1"; }
    umount() { printf 'UMOUNT CALLED\n' >> "$FIXTURE/fs-check.txt"; }
    RUNNING_HOST_MODE=0
    TARGET_ROOT="/tmp/boot-repair-session/session.xxxx/mount"
    filesystem_scope_resolve() {
        FS_SCOPE_DEVICES=(/dev/mapper/debian-root)
        FS_SCOPE_MOUNTS=("")
        FS_SCOPE_FSTYPES=(ext3)
        FS_SCOPE_UUIDS=("")
        FS_SCOPE_TOOLS=(e2fsck)
    }
    filesystem_scope_tools() { :; }
    filesystem_detail_line() { printf 'detail: %s\n' "$3"; }
    filesystem_repair_tool() { printf '/sbin/e2fsck'; }
    filesystem_run_inspect_command() {
        printf 'INSPECT RAN %s\n' "$2"
        FS_INSPECT_OUTPUT=""
        FS_INSPECT_RC=0
    }
    fs_inspect_scope > "$FIXTURE/fs-check.txt" 2>&1 || true
)
grep -q 'result=skipped' "$FIXTURE/fs-check.txt" \
    || fail "mounted target device was not skipped by the file system check"
grep -q 'this check tool is offline-only' "$FIXTURE/fs-check.txt" \
    || fail "skip line lost the offline-only detail"
grep -q 'File system check summary: devices=1 clean=0 issues=0 unsupported=0 tool-missing=0 skipped=1' \
    "$FIXTURE/fs-check.txt" || fail "file system check summary counts are wrong"
grep -q 'INSPECT RAN' "$FIXTURE/fs-check.txt" \
    && fail "the mounted device was checked instead of skipped"
grep -q 'UMOUNT CALLED' "$FIXTURE/fs-check.txt" \
    && fail "the legacy file system check released the target mounts"
pass "file system check skips devices mounted under the target (offline-only)"

# --- cycle 12 loop 3 + A9-09: resolver copy with teardown restore ------------
# The legacy resolver is a file copy (the 2.6.18 kernel refuses a
# remount,bind,ro of a single-file bind with EBUSY): the target's original is
# backed up, the host resolver is copied over it, a second call reuses the
# copy, and the exit cleanup restores the original and removes the backup.
# A9-09: a destination that already equals the recovery-host resolver (an
# interrupted session's leftover) is refused instead of re-backed up, and the
# cleanup restores only while the destination still equals the copied
# resolver - a user change is never overwritten.
# Helper functions leave `set -e` enabled at their end, so the smoke re-arms
# `set +e` here and the case exits 0 explicitly.
set +e
mkdir -p "$FIXTURE/etc"
[[ -r /etc/resolv.conf ]] || fail "smoke fixture requires a readable /etc/resolv.conf"
printf 'nameserver smoke-ns-target\n' > "$FIXTURE/etc/resolv.conf"
(
    # The subshell must not inherit the smoke's EXIT trap (it would delete the
    # fixture when the case's last assertion returns non-zero).
    trap - EXIT
    TARGET_ROOT="$FIXTURE"
    SESSION_DIR="$FIXTURE/session-resolver"
    mkdir -p "$SESSION_DIR"
    LEGACY_RESOLVER_DESTINATION=""
    LEGACY_RESOLVER_BACKUP=""
    cp_calls=0
    cp() {
        cp_calls=$((cp_calls + 1))
        command cp "$@"
    }
    realpath_existing() { printf '%s\n' "$1"; }
    path_within() { return 0; }
    log() { :; }
    cleanup_modern() { :; }
    mount_target_resolver
    cmp -s "$FIXTURE/etc/resolv.conf" /etc/resolv.conf \
        || fail "resolver copy did not land in the target"
    [[ -f "$SESSION_DIR/resolv.conf.target.before" ]] \
        || fail "resolver backup was not created"
    mount_target_resolver
    [[ $cp_calls -le 2 ]] || fail "resolver copy ran more than once"
    cleanup >/dev/null 2>&1 || true
    [[ "$(cat "$FIXTURE/etc/resolv.conf")" == "nameserver smoke-ns-target" ]] \
        || fail "resolver restore did not put the target's original back"
    [[ -e "$SESSION_DIR/resolv.conf.target.before" ]] \
        && fail "resolver backup was not removed by the teardown"
    exit 0
)
rc=$?
[[ $rc -eq 0 ]] || fail "resolver copy round-trip failed"
pass "legacy resolver copy with teardown restore (idempotent, never remounted)"

# --- A9-09: polluted-destination refusal and skip-restore-when-changed -------
set +e
mkdir -p "$FIXTURE/resolver2/etc" "$FIXTURE/resolver2/session"
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/resolver2"
    SESSION_DIR="$FIXTURE/resolver2/session"
    SESSION_LOG="$FIXTURE/resolver2/session/session.log"
    : > "$SESSION_LOG"
    LOG_FILE="$FIXTURE/resolver2/log.txt"
    : > "$LOG_FILE"
    LEGACY_RESOLVER_DESTINATION=""
    LEGACY_RESOLVER_BACKUP=""
    realpath_existing() { printf '%s\n' "$1"; }
    path_within() { return 0; }
    log() { printf '%s\n' "$*" >> "$LOG_FILE"; }
    cleanup_modern() { :; }
    # A clean destination is copied over (backup first).
    printf 'nameserver original-target\n' > "$FIXTURE/resolver2/etc/resolv.conf"
    mount_target_resolver >/dev/null 2>&1 \
        || fail "resolver copy failed on a clean destination"
    [[ -f "$SESSION_DIR/resolv.conf.target.before" ]] \
        || fail "resolver backup was not created on the clean destination"
    # Interrupted-session refusal: simulate a fresh session whose destination
    # still holds the recovery-host copy; the polluted file must never be
    # re-backed-up and the refusal names the scenario.
    LEGACY_RESOLVER_DESTINATION=""
    LEGACY_RESOLVER_BACKUP=""
    rm -f "$SESSION_DIR/resolv.conf.target.before"
    ( mount_target_resolver ) >/dev/null 2>&1
    polluted_rc=$?
    [[ $polluted_rc -ne 0 ]] \
        || fail "resolver accepted a polluted destination (interrupted-session copy)"
    grep -q 'interrupted session' "$LOG_FILE" \
        || fail "resolver pollution refusal does not name the interrupted-session scenario"
    [[ -e "$SESSION_DIR/resolv.conf.target.before" ]] \
        && fail "resolver re-backed-up the polluted destination"
    # Skip-restore-when-changed: a user edit after the copy is never
    # overwritten and the skip is logged.
    rm -f "$LOG_FILE" "$SESSION_DIR/resolv.conf.target.before"
    LEGACY_RESOLVER_DESTINATION=""
    LEGACY_RESOLVER_BACKUP=""
    printf 'nameserver original-target\n' > "$FIXTURE/resolver2/etc/resolv.conf"
    mount_target_resolver >/dev/null 2>&1 \
        || fail "resolver copy failed for the skip-restore case"
    printf 'nameserver user-changed\n' > "$FIXTURE/resolver2/etc/resolv.conf"
    cleanup >/dev/null 2>&1 || true
    [[ "$(cat "$FIXTURE/resolver2/etc/resolv.conf")" == "nameserver user-changed" ]] \
        || fail "resolver cleanup overwrote the user's change"
    grep -q 'NOT overwritten' "$LOG_FILE" \
        || fail "resolver cleanup left no skip evidence"
    [[ -f "$SESSION_DIR/resolv.conf.target.before" ]] \
        || fail "resolver cleanup removed the backup evidence"
    exit 0
)
rc=$?
[[ $rc -eq 0 ]] || fail "resolver A9-09 hardening checks failed"
pass "legacy resolver A9-09 (polluted-destination refusal, skip-restore-when-changed)"

# --- cycle 12 loop 4: shared data mount promotion ---------------------------
# The split-LV data mounts (usr/var/tmp/home) must be promoted read-write
# before the modifying stages; the shared promotion is fail-closed when a
# data filesystem cannot be promoted or is no longer mounted.
set +e
(
    trap - EXIT
    SESSION_LOG="$FIXTURE/session.log"
    TARGET_ROOT="$FIXTURE"
    TARGET_DATA_MOUNTS=("$FIXTURE/var" "$FIXTURE/home")
    remount_calls=0
    : > "$FIXTURE/remounts.log"
    mount() {
        remount_calls=$((remount_calls + 1))
        printf 'REM %s\n' "$*" >> "$FIXTURE/remounts.log"
        return 0
    }
    mountpoint() { return 0; }
    log() { :; }
    remount_target_data_rw
    [[ $remount_calls -eq 2 ]] || fail "data promotion did not remount both data mounts"
    grep -q "REM -o remount,rw $FIXTURE/var" "$FIXTURE/remounts.log" \
        || fail "data promotion did not remount /var read-write"
    grep -q "REM -o remount,rw $FIXTURE/home" "$FIXTURE/remounts.log" \
        || fail "data promotion did not remount /home read-write"
    mount() { return 1; }
    ( remount_target_data_rw ) >/dev/null 2>&1 \
        && fail "data promotion did not fail closed on a remount failure"
    mountpoint() { return 1; }
    ( remount_target_data_rw ) >/dev/null 2>&1 \
        && fail "data promotion did not fail closed on a vanished data mount"
    exit 0
)
pass "shared data mount promotion (rw remount, fail-closed)"

# --- cycle 12 loop 2: cryptsetup 1.0 status parsing --------------------------
# Etch prints `device:  /dev/.static/dev/hdb5`; the parser must strip the
# colon-bearing field and the /dev/.static prefix.
cryptsetup() { printf 'device:  /dev/.static/dev/hdb5\n'; }
status_device="$(legacy_crypt_status_device smoke-mapper)" \
    || fail "cryptsetup 1.0 status was not parsed"
[[ "$status_device" == "/dev/hdb5" ]] \
    || fail "cryptsetup 1.0 device path was not normalized: $status_device"
cryptsetup() { printf 'cipher:  aes\n'; }
legacy_crypt_status_device smoke-mapper \
    && fail "a status without a device field must not yield a device"
pass "legacy cryptsetup 1.0 status parsing (device: field, /dev/.static strip)"

# --- legacy Make Default (B7-7 host-default) ---------------------------------
mkdir -p "$FIXTURE/etc" "$FIXTURE/etc/init.d"
RUNNING_HOST_MODE=1
host_default_unavailable_reason || fail "host-default capability refused the GRUB-legacy host fixture"
cp -a "$FIXTURE/boot/grub/menu.lst" "$FIXTURE/menu.lst.predefault"
prepare_running_host() { RUNNING_HOST_MODE=1; TARGET_ROOT="$FIXTURE"; TARGET_DISK=/dev/null; ROOT_DEVICE=/dev/null; }
profile_target_backends() { :; }
repair_file_fingerprint() { sha256sum "$1" 2>/dev/null | awk '{print $1}'; }
prepare_host_command_guard() { HOST_COMMAND_GUARD=0; HOST_COMMAND_GUARD_DIR=""; }
(
    legacy_host_default_repair /dev/null /dev/null
) || fail "legacy_host_default_repair failed on the fixture"
grep -qE '^[[:space:]]*default[[:space:]]+0([[:space:]]|$)' "$FIXTURE/boot/grub/menu.lst" \
    || fail "Make Default did not set the canonical default directive"
default_out="$(legacy_host_default_repair /dev/null /dev/null)"
printf '%s\n' "$default_out" | grep -q 'Repair change status host-default: unchanged' \
    || fail "idempotent Make Default did not report unchanged"
# Fail closed: an unwritable menu without a default directive must refuse and
# leave the file byte-identical.
cp -a "$FIXTURE/boot/grub/menu.lst" "$FIXTURE/menu.lst.good"
grep -v '^default ' "$FIXTURE/boot/grub/menu.lst" > "$FIXTURE/menu.lst.nodefault" || true
cp "$FIXTURE/menu.lst.nodefault" "$FIXTURE/boot/grub/menu.lst"
chmod 444 "$FIXTURE/boot/grub/menu.lst"
( legacy_host_default_repair /dev/null /dev/null ) >/dev/null 2>&1 \
    && fail "Make Default accepted an unwritable menu.lst"
chmod 644 "$FIXTURE/boot/grub/menu.lst"
cmp -s "$FIXTURE/boot/grub/menu.lst" "$FIXTURE/menu.lst.nodefault" \
    || fail "failed Make Default modified the menu"
RUNNING_HOST_MODE=0
pass "legacy Make Default (canonical entry, idempotent, fail-closed)"

# --- legacy apt intent translation (cycle 8) ---------------------------------
# Etch's apt has no subcommands: `apt update` must become `apt-get update`,
# `apt full-upgrade` must become `apt-get dist-upgrade`, and anything else
# (bare apt, unknown subcommands, other frontends) must run unchanged.
[[ "$(legacy_apt_intent_translate 'apt update')" == 'apt-get update' ]] \
    || fail "apt update was not translated to apt-get update"
[[ "$(legacy_apt_intent_translate 'apt upgrade')" == 'apt-get upgrade' ]] \
    || fail "apt upgrade was not translated to apt-get upgrade"
[[ "$(legacy_apt_intent_translate 'apt full-upgrade')" == 'apt-get dist-upgrade' ]] \
    || fail "apt full-upgrade was not translated to dist-upgrade"
[[ "$(legacy_apt_intent_translate 'apt dist-upgrade')" == 'apt-get dist-upgrade' ]] \
    || fail "apt dist-upgrade was not translated"
[[ "$(legacy_apt_intent_translate 'apt install htop')" == 'apt-get install htop' ]] \
    || fail "apt install arguments were not preserved through the translation"
[[ "$(legacy_apt_intent_translate 'apt remove --purge htop')" == 'apt-get remove --purge htop' ]] \
    || fail "apt remove arguments were not preserved through the translation"
[[ "$(legacy_apt_intent_translate 'apt autoremove')" == 'apt-get autoremove' ]] \
    || fail "apt autoremove was not translated"
[[ "$(legacy_apt_intent_translate 'apt')" == 'apt' ]] \
    || fail "a bare apt was rewritten"
[[ "$(legacy_apt_intent_translate 'apt frobnicate')" == 'apt frobnicate' ]] \
    || fail "an unknown apt subcommand was rewritten"
[[ "$(legacy_apt_intent_translate 'apt-get update')" == 'apt-get update' ]] \
    || fail "an apt-get command was rewritten (must stay untouched)"
[[ "$(legacy_apt_intent_translate 'aptitude update')" == 'aptitude update' ]] \
    || fail "aptitude was rewritten"
[[ "$(legacy_apt_intent_translate 'apt-cache search htop')" == 'apt-cache search htop' ]] \
    || fail "apt-cache was rewritten"
pass "legacy apt intent translation (apt -> apt-get, full-upgrade -> dist-upgrade)"

# --- legacy browse-target record format (cycle 8 + A9-13) --------------------
# The legacy port emits BROWSE_ENTRY records with raw percent-encoded names
# (Qt 3.3.7 has no QByteArray::fromBase64); the Qt3 picker decodes them.
# A9-13: TAB inside a name is percent-encoded (%09) so the tab-delimited
# record format stays unambiguous.
mkdir -p "$FIXTURE/browse-root/alpha" "$FIXTURE/browse-root/beta"
mkdir -p "$FIXTURE/browse-root/$(printf 'tab\tname')"
prepare_target() { :; }
maybe_mount_target_path() { :; }
realpath_existing() { printf '%s\n' "$1"; }
TARGET_ROOT="$FIXTURE"
browse_out="$(browse_target_directory /browse-root)"
printf '%s\n' "$browse_out" | grep -q 'BROWSE_ENTRY	alpha' \
    || fail "browse-target did not list alpha"
printf '%s\n' "$browse_out" | grep -q 'BROWSE_ENTRY	beta' \
    || fail "browse-target did not list beta"
printf '%s\n' "$browse_out" | grep -q $'BROWSE_ENTRY\ttab%09name' \
    || fail "browse-target did not percent-encode the TAB in a directory name"
printf '%s\n' "$browse_out" | grep -q $'tab\tname' \
    && fail "browse-target emitted a raw TAB inside a record name"
printf '%s\n' "$browse_out" | grep -q 'BROWSE_ENTRY	' \
    || fail "browse-target emitted no BROWSE_ENTRY records"
pass "legacy browse-target records (raw percent-encoded names, TAB encoded as %09)"

echo "legacy helper smoke: PASS"
