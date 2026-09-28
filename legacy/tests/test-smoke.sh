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
# The sourced helper set `set -o pipefail`; with it active, `producer |
# grep -q` flips the pipeline nonzero whenever grep -q exits early and the
# producer takes SIGPIPE (141) - a load-dependent false failure observed on
# the 2-vCPU Alpine rig (e.g. the A9-12 noglob and unlock-probe checks).
# Every assertion here is an explicit ||/&& check, so pipefail only adds
# that hazard: disable it for the rest of the run.
set +o pipefail

# B6/A9-05: the split-LV data-mount tests below replace mount_recorded with
# stubs; capture the real wrapper now so the mount-option retry tests can
# restore it later.
MOUNT_RECORDED_DEFINITION="$(declare -f mount_recorded)"
# B5: the browse-target section later stubs realpath_existing at the top
# level; capture the real definition now so the config-write content-file
# path proof can restore it.
REALPATH_EXISTING_DEFINITION="$(declare -f realpath_existing)"

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

# --- A12-04: single-user variants are derived boot entries ------------------
# The rig menu.lst keeps a manually added single-user entry whose kernel line
# carries the defoptions-managed serial-console arguments; stock Etch
# update-grub regenerates altoptions single entries WITHOUT defoptions (they
# apply "to the default boot option, but not with the alternatives"), so the
# variant's serial arguments would be stripped.  The capture/restore pipeline
# must keep the variant verbatim next to its parent, and the guard must treat
# a missing variant whose parent survives as a derived entry, never as a
# removed one.
cat > "$FIXTURE/menu.lst.single.before" <<'EOF'
# defoptions=console=ttyS0,115200 console=tty0 consoleblank=0
# kopt=root=/dev/mapper/debian1-root ro
# altoptions=(single-user mode) single

title		Debian GNU/Linux, kernel 2.6.18-6-amd64
root		(hd0,0)
kernel		/vmlinuz-2.6.18-6-amd64 root=/dev/mapper/debian1-root ro console=ttyS0,115200 console=tty0 consoleblank=0
initrd		/initrd.img-2.6.18-6-amd64
savedefault

title		Debian GNU/Linux, kernel 2.6.18-6-amd64 (single-user mode)
root		(hd0,0)
kernel		/vmlinuz-2.6.18-6-amd64 root=/dev/mapper/debian1-root ro single console=ttyS0,115200 console=tty0 consoleblank=0
initrd		/initrd.img-2.6.18-6-amd64
savedefault
EOF
LEGACY_GRUB_MANAGED="$(legacy_grub_managed_options "$FIXTURE/menu.lst.single.before")"
variant_dir="$FIXTURE/variant-cap"
mkdir -p "$variant_dir"
legacy_grub_single_variant_capture "$FIXTURE/menu.lst.single.before" "$variant_dir"
manifest="$variant_dir/grub-single-variants.list"
[[ -s "$manifest" ]] || fail "single-user variant was not captured"
[[ "$(grep -c . "$manifest")" -eq 1 ]] || fail "single-user variant count: $(cat "$manifest")"
parent_line="$(cut -f1 "$manifest")"
[[ "$parent_line" == "/vmlinuz-2.6.18-6-amd64 root=/dev/mapper/debian1-root ro console=ttyS0,115200 console=tty0 consoleblank=0" ]] \
    || fail "captured parent kernel line: $parent_line"
variant_file="$(cut -f2 "$manifest")"
[[ -s "$variant_file" ]] || fail "captured variant block file is empty"
grep -q 'ro single console=ttyS0,115200 console=tty0 consoleblank=0' "$variant_file" \
    || fail "captured variant block lost its serial-console arguments"

# Simulated update-grub output: the main entry regenerated with defoptions
# and the altoptions alternative WITHOUT them (exactly the Etch behaviour).
cat > "$FIXTURE/menu.lst.single.after" <<'EOF'
# defoptions=console=ttyS0,115200 console=tty0 consoleblank=0
# kopt=root=/dev/mapper/debian1-root ro
# altoptions=(single-user mode) single

title		Debian GNU/Linux, kernel 2.6.18-6-amd64
root		(hd0,0)
kernel		/vmlinuz-2.6.18-6-amd64 root=/dev/mapper/debian1-root ro console=ttyS0,115200 console=tty0 consoleblank=0
initrd		/initrd.img-2.6.18-6-amd64
savedefault

title		Debian GNU/Linux, kernel 2.6.18-6-amd64 (single-user mode)
root		(hd0,0)
kernel		/vmlinuz-2.6.18-6-amd64 root=/dev/mapper/debian1-root ro single
initrd		/initrd.img-2.6.18-6-amd64
savedefault
EOF
cp -a "$FIXTURE/menu.lst.single.after" "$FIXTURE/menu.lst.single.restored"
legacy_grub_restore_single_variants "$FIXTURE/menu.lst.single.restored" "$manifest"
grep -q 'root=/dev/mapper/debian1-root ro single console=ttyS0,115200 console=tty0 consoleblank=0' \
    "$FIXTURE/menu.lst.single.restored" \
    || fail "restore did not re-insert the variant with its serial-console arguments"
[[ "$(grep -cE '^[[:space:]]*kernel[[:space:]]' "$FIXTURE/menu.lst.single.restored")" -eq 2 ]] \
    || fail "restore left a duplicate or lost a kernel entry"
legacy_grub_entry_declarations "$FIXTURE/menu.lst.single.before" "$LEGACY_GRUB_MANAGED" \
    > "$FIXTURE/decl.single.before"
legacy_grub_entry_declarations "$FIXTURE/menu.lst.single.restored" "$LEGACY_GRUB_MANAGED" \
    > "$FIXTURE/decl.single.after"
LEGACY_GRUB_VARIANT_MANIFEST="$manifest"
legacy_grub_guard_entries_preserved "$FIXTURE/decl.single.before" \
    "$FIXTURE/menu.lst.single.restored" \
    || fail "guard rejected the restored single-user variant regeneration"
legacy_grub_variants_verified "$manifest" "$FIXTURE/decl.single.after" \
    || fail "variant verification failed after a successful restoration"

# Guard relaxation: a missing variant whose parent survives is derived, not a
# removed entry; the same removal without a captured manifest stays fatal.
cat > "$FIXTURE/menu.lst.single.novariant" <<'EOF'
title		Debian GNU/Linux, kernel 2.6.18-6-amd64
kernel		/vmlinuz-2.6.18-6-amd64 root=/dev/mapper/debian1-root ro console=ttyS0,115200 console=tty0 consoleblank=0
initrd		/initrd.img-2.6.18-6-amd64
EOF
legacy_grub_guard_entries_preserved "$FIXTURE/decl.single.before" \
    "$FIXTURE/menu.lst.single.novariant" \
    || fail "guard must relax a missing derived single-user variant whose parent survived"
LEGACY_GRUB_VARIANT_MANIFEST=""
legacy_grub_guard_entries_preserved "$FIXTURE/decl.single.before" \
    "$FIXTURE/menu.lst.single.novariant" \
    && fail "guard without a variant manifest accepted the removal"
# The variant's parent disappearing as well must refuse even with a manifest.
LEGACY_GRUB_VARIANT_MANIFEST="$manifest"
cat > "$FIXTURE/menu.lst.single.parentgone" <<'EOF'
title		Some other kernel
kernel		/boot/vmlinuz-2.6.18-6-486 root=/dev/sda1 ro
initrd		/boot/initrd.img-2.6.18-6-486
EOF
legacy_grub_guard_entries_preserved "$FIXTURE/decl.single.before" \
    "$FIXTURE/menu.lst.single.parentgone" \
    && fail "guard must refuse when the variant's parent entry is gone too"
# A genuinely distinct entry stays protected even with a manifest present.
cat > "$FIXTURE/menu.lst.single.custom-before" <<'EOF'
title		custom rescue entry
kernel		/boot/vmlinuz-rescue root=/dev/mapper/debian1-root ro custom-arg
initrd		/boot/initrd.img-rescue
EOF
legacy_grub_entry_declarations "$FIXTURE/menu.lst.single.custom-before" "$LEGACY_GRUB_MANAGED" \
    > "$FIXTURE/decl.custom.before"
cat > "$FIXTURE/menu.lst.single.custom-after" <<'EOF'
title		Debian GNU/Linux, kernel 2.6.18-6-amd64
kernel		/vmlinuz-2.6.18-6-amd64 root=/dev/mapper/debian1-root ro console=ttyS0,115200 console=tty0 consoleblank=0
initrd		/initrd.img-2.6.18-6-amd64
EOF
legacy_grub_guard_entries_preserved "$FIXTURE/decl.custom.before" \
    "$FIXTURE/menu.lst.single.custom-after" \
    && fail "guard accepted the removal of a genuinely distinct custom entry"
LEGACY_GRUB_VARIANT_MANIFEST=""
LEGACY_GRUB_MANAGED=""
pass "single-user variants (capture, verbatim restore, derived-variant guard relaxation, distinct-entry protection)"

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

# --- A12-03/A12-06: etch2 split-LV mount fixture ----------------------------
# The reference split-LV fstab names /, /usr, /var, /tmp and /home on
# separate LVM mapper LVs, /boot on the bare host-relative /dev/hda1 (written
# from the installed system's own perspective) and the vfat share at /host.
# Repairing that disk from another host must mount the four data LVs
# read-only with nosuid,nodev, remap the bare /boot source onto the selected
# target disk and skip the swap/proc/vfat pseudo entries.
cat > "$FIXTURE/etc/fstab" <<'EOF'
proc			/proc		proc	defaults	0	0
/dev/mapper/debian-root /		ext3	defaults,errors=remount-ro 0	1
/dev/hda1		/boot		ext3	defaults	0	2
/dev/mapper/debian-home	/home		ext3	defaults	0	2
/dev/mapper/debian-tmp	/tmp		ext3	defaults	0	2
/dev/mapper/debian-usr	/usr		ext3	defaults	0	2
/dev/mapper/debian-var	/var		ext3	defaults	0	2
/dev/mapper/debian-swap_1 none		swap	sw		0	0
/dev/hdc		/host		vfat	defaults,uid=1000,gid=1000	0	0
EOF
TARGET_ROOT="$FIXTURE"
TARGET_DISK=/dev/hdb
ROOT_DEVICE=/dev/mapper/debian-root
ROOT_CANONICAL=/dev/mapper/debian-root
MOUNTS=()
TARGET_DATA_MOUNTS=()
: > "$FIXTURE/mounts.log"
resolve_fstab_source() { legacy_remap_target_device_path "$1"; }
canonical_block() { printf '%s\n' "$1"; }
is_block_device() {
    [[ "$1" == /dev/mapper/debian-root || "$1" == /dev/mapper/debian-usr \
        || "$1" == /dev/mapper/debian-var || "$1" == /dev/mapper/debian-tmp \
        || "$1" == /dev/mapper/debian-home || "$1" == /dev/hdb1 ]]
}
same_single_top_disk() {
    [[ "$1" == /dev/hdb && "$2" == /dev/hdb1 ]] \
        || [[ "$1" == /dev/hdb && "$2" == /dev/mapper/debian-* ]]
}
mountpoint() { return 1; }
mount_recorded() {
    printf 'MOUNT %s\n' "$*" >> "$FIXTURE/mounts.log"
    MOUNTS+=("$2")
    return 0
}
mount_target_data_partitions ro
[[ "$(grep -c '^MOUNT ' "$FIXTURE/mounts.log")" -eq 4 ]] \
    || fail "etch2 split-LV ro mount did not mount exactly the four data LVs"
grep -q "MOUNT /dev/mapper/debian-var $FIXTURE/var -o ro,nosuid,nodev" "$FIXTURE/mounts.log" \
    || fail "etch2 /var was not mounted ro,nosuid,nodev from its mapper LV"
grep -q "MOUNT /dev/mapper/debian-usr $FIXTURE/usr -o ro,nosuid,nodev" "$FIXTURE/mounts.log" \
    || fail "etch2 /usr was not mounted ro,nosuid,nodev from its mapper LV"
grep -q "MOUNT /dev/mapper/debian-tmp $FIXTURE/tmp -o ro,nosuid,nodev" "$FIXTURE/mounts.log" \
    || fail "etch2 /tmp was not mounted ro,nosuid,nodev from its mapper LV"
grep -q "MOUNT /dev/mapper/debian-home $FIXTURE/home -o ro,nosuid,nodev" "$FIXTURE/mounts.log" \
    || fail "etch2 /home was not mounted ro,nosuid,nodev from its mapper LV"
grep -q 'swap' "$FIXTURE/mounts.log" && fail "etch2 swap was mounted as a data entry"
grep -q 'hdc\|/host' "$FIXTURE/mounts.log" && fail "etch2 /host vfat share was mounted as a data entry"
[[ "${#TARGET_DATA_MOUNTS[@]}" -eq 4 ]] \
    || fail "etch2 data mount records do not match the four data mounts"
# /boot: the bare /dev/hda1 source must remap onto the selected target disk
# and mount with the ext3 noload form the A9-05 retry later relaxes.
: > "$FIXTURE/mounts.log"
MOUNTS=()
if [[ -b /dev/hdb1 ]]; then
    mount_target_boot_entry /boot ro
    grep -q "MOUNT /dev/hdb1 $FIXTURE/boot -o ro,noload" "$FIXTURE/mounts.log" \
        || fail "etch2 /boot was not remapped onto the target disk and mounted ro,noload"
else
    # No hdb1 on this host: the remap must pass the source through (fail
    # closed) instead of inventing a device; the rig drill proves the remap.
    [[ "$(legacy_remap_target_device_path /dev/hda1)" == /dev/hda1 ]] \
        || fail "etch2 bare /boot source was not passed through when the remapped node is absent"
    [[ "$(legacy_remap_target_device_path /dev/mapper/debian-root)" == /dev/mapper/debian-root ]] \
        || fail "etch2 mapper sources must pass through the remap unchanged"
fi
# The vfat /host entry is never a data or boot mount candidate.
pass "etch2 split-LV fstab fixture (four data LVs ro,nosuid,nodev; /boot remap; pseudo entries skipped)"

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
# A12-02: unlock_target resolves cryptsetup through the standard-location
# probe (legacy_standard_tool_path), so the tests stub the probe seam with a
# fixture binary instead of a PATH function.
mkdir -p "$FIXTURE/unlock-run1" "$FIXTURE/unlock-run2" "$FIXTURE/unlock-run3"
cat > "$FIXTURE/cryptsetup-stub" <<'EOF'
#!/bin/sh
case "$1" in
    isLuks) exit 0 ;;
    luksUUID) printf '11111111-2222-3333-4444-555555555555\n' ;;
    luksOpen) exit 0 ;;
    luksClose) exit 0 ;;
    *) exit 1 ;;
esac
EOF
chmod +x "$FIXTURE/cryptsetup-stub"
set +e
# Accepted (SUDO_UID matches): runs through to the stubbed open and then
# fails at the mapper-appeared check - after the session directory exists.
(
    trap - EXIT
    canonical_block() { printf '%s\n' "$1"; }
    assert_target_not_host() { return 0; }
    same_single_top_disk() { return 0; }
    lsblk() { return 0; }
    legacy_standard_tool_path() { printf '%s\n' "$FIXTURE/cryptsetup-stub"; }
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
    legacy_standard_tool_path() { printf '%s\n' "$FIXTURE/cryptsetup-stub"; }
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
    legacy_standard_tool_path() { printf '%s\n' "$FIXTURE/cryptsetup-stub"; }
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

# --- cycle 12 + B6/A9-08: existing-mapper unlock probes the root candidate --
# A previous session may have left the mapper open: the reuse path must emit
# UNLOCKED_ROOT for the already-open chain (a fresh GUI process cannot see
# the LVs in its read-only inventory otherwise).
#
# B6/A9-08: the probe scopes its candidates to the unlocked PV's own volume
# group, so a foreign running-host LV (even one named *root*) is never
# offered; without LVM tooling (or without a provable VG) it falls back to
# the unscoped probe with a WARNING - the candidate is never silently
# dropped.
find() {
    if [[ "$1" == /dev/mapper ]]; then
        printf '%s\n' /dev/mapper/etch-root /dev/mapper/etch-var \
            /dev/mapper/etch-home /dev/mapper/hostvg-root \
            /dev/mapper/luks-etchroot
        return 0
    fi
    command find "$@"
}
legacy_blkid_value_path() {
    case "$1" in
        *etch-*|*hostvg-*) printf 'ext3\n' ;;
        *) return 0 ;;
    esac
}
mkdir -p "$FIXTURE/lvm-bin" "$FIXTURE/lvm-notools"
cat > "$FIXTURE/lvm-bin/pvs" <<'EOF'
#!/bin/sh
# Fake pvs: `pvs --noheadings -o vg_name -- <pv>`.
if [ "${PVS_NONE:-}" = "1" ]; then
    exit 0
fi
printf '  %s\n' "${PVS_VG:-etch}"
EOF
chmod +x "$FIXTURE/lvm-bin/pvs"
cat > "$FIXTURE/lvm-bin/lvs" <<'EOF'
#!/bin/sh
# Fake lvs: `lvs --noheadings -o lv_path -- <vg>`.
if [ "${LVS_NONE:-}" = "1" ]; then
    exit 0
fi
printf '  %s\n' "${LVS_LV1:-/dev/mapper/etch-root}"
[ -n "${LVS_LV2:-}" ] && printf '  %s\n' "$LVS_LV2"
EOF
chmod +x "$FIXTURE/lvm-bin/lvs"
ln -s "$(command -v sort)" "$FIXTURE/lvm-notools/sort"

# Strict mode: the VG gate must exclude the foreign hostvg-root LV even
# though it matches the root-name heuristic, and select the target VG's LV.
legacy_real_tool_path() { type -P "$1"; }
(
    PATH="$FIXTURE/lvm-bin:$PATH"
    LVS_LV1=/dev/mapper/etch-root
    LVS_LV2=/dev/mapper/etch-var
    export PATH LVS_LV1 LVS_LV2
    probe_out="$(legacy_unlock_root_probe /dev/mapper/luks-etchroot)"
    printf '%s\n' "$probe_out" > "$FIXTURE/unlock-probe-strict.txt"
)
strict_out="$(cat "$FIXTURE/unlock-probe-strict.txt")"
printf '%s\n' "$strict_out" | grep -q 'UNLOCKED_ROOT=/dev/mapper/etch-root' \
    || fail "VG-scoped probe did not select the target VG root LV: $strict_out"
printf '%s\n' "$strict_out" | grep -q 'UNLOCKED_ROOT_FSTYPE=ext3' \
    || fail "VG-scoped probe did not emit the unlocked root fstype"
printf '%s\n' "$strict_out" | grep -q 'hostvg' \
    && fail "VG-scoped probe leaked a foreign running-host LV: $strict_out"
printf '%s\n' "$strict_out" | grep -q "scoped to volume group 'etch'" \
    || fail "VG-scoped probe left no scope evidence: $strict_out"

# Strict mode, no root-named LV in the VG: the only VG member must win over
# the foreign root-named LV (the VG gate beats the name heuristic).
(
    PATH="$FIXTURE/lvm-bin:$PATH"
    LVS_LV1=/dev/mapper/etch-home
    LVS_LV2=
    export PATH LVS_LV1 LVS_LV2
    probe_out="$(legacy_unlock_root_probe /dev/mapper/luks-etchroot)"
    printf '%s\n' "$probe_out" > "$FIXTURE/unlock-probe-strict2.txt"
)
strict_out="$(cat "$FIXTURE/unlock-probe-strict2.txt")"
printf '%s\n' "$strict_out" | grep -q 'UNLOCKED_ROOT=/dev/mapper/etch-home' \
    || fail "VG-scoped probe did not fall back to the in-VG LV: $strict_out"
printf '%s\n' "$strict_out" | grep -q 'hostvg' \
    && fail "VG-scoped probe leaked a foreign running-host LV: $strict_out"

# Missing LVM tooling: WARNING + unscoped fallback (current behaviour, which
# prefers the last root-name match - the foreign LV here - and is never
# silently empty).
legacy_real_tool_path() {
    case "$1" in
        pvs|lvs) return 1 ;;
        *) type -P "$1" ;;
    esac
}
(
    probe_out="$(legacy_unlock_root_probe /dev/mapper/luks-etchroot)"
    printf '%s\n' "$probe_out" > "$FIXTURE/unlock-probe-notools.txt"
)
fallback_out="$(cat "$FIXTURE/unlock-probe-notools.txt")"
printf '%s\n' "$fallback_out" | grep -q 'no volume group could be proven' \
    || fail "missing-tooling probe lost the fallback WARNING: $fallback_out"
printf '%s\n' "$fallback_out" | grep -q 'UNLOCKED_ROOT=' \
    || fail "missing-tooling fallback silently dropped the candidate: $fallback_out"

# VG proven but lvs missing: WARNING + fallback.
legacy_real_tool_path() {
    case "$1" in
        lvs) return 1 ;;
        *) type -P "$1" ;;
    esac
}
(
    PATH="$FIXTURE/lvm-bin:$PATH"
    probe_out="$(legacy_unlock_root_probe /dev/mapper/luks-etchroot)"
    printf '%s\n' "$probe_out" > "$FIXTURE/unlock-probe-nolvs.txt"
)
fallback_out="$(cat "$FIXTURE/unlock-probe-nolvs.txt")"
printf '%s\n' "$fallback_out" | grep -q 'lvs is not installed' \
    || fail "lvs-missing probe lost the fallback WARNING: $fallback_out"
printf '%s\n' "$fallback_out" | grep -q 'UNLOCKED_ROOT=' \
    || fail "lvs-missing fallback silently dropped the candidate: $fallback_out"

# pvs reports no VG (the device is not a provable PV): WARNING + fallback.
legacy_real_tool_path() { type -P "$1"; }
(
    PATH="$FIXTURE/lvm-bin:$PATH"
    PVS_NONE=1
    export PATH PVS_NONE
    probe_out="$(legacy_unlock_root_probe /dev/mapper/luks-etchroot)"
    printf '%s\n' "$probe_out" > "$FIXTURE/unlock-probe-novg.txt"
)
fallback_out="$(cat "$FIXTURE/unlock-probe-novg.txt")"
printf '%s\n' "$fallback_out" | grep -q 'no volume group could be proven' \
    || fail "no-VG probe lost the fallback WARNING: $fallback_out"
printf '%s\n' "$fallback_out" | grep -q 'UNLOCKED_ROOT=' \
    || fail "no-VG fallback silently dropped the candidate: $fallback_out"

# VG proven but lvs lists zero logical volumes: WARNING + fallback.
(
    PATH="$FIXTURE/lvm-bin:$PATH"
    LVS_NONE=1
    export PATH LVS_NONE
    probe_out="$(legacy_unlock_root_probe /dev/mapper/luks-etchroot)"
    printf '%s\n' "$probe_out" > "$FIXTURE/unlock-probe-zerolvs.txt"
)
fallback_out="$(cat "$FIXTURE/unlock-probe-zerolvs.txt")"
printf '%s\n' "$fallback_out" | grep -q 'no logical volumes could be listed' \
    || fail "zero-LV probe lost the fallback WARNING: $fallback_out"
printf '%s\n' "$fallback_out" | grep -q 'UNLOCKED_ROOT=' \
    || fail "zero-LV fallback silently dropped the candidate: $fallback_out"
pass "legacy unlock root probe (VG-scoped; foreign LVs excluded; unscoped fallback with WARNING)"

# --- B6/A9-05: noload kept on the first ro mount, stripped only on retry ----
# Etch's util-linux 2.12r can reject `ro,noload` on ext3; the filter must keep
# noload for the first attempt (a plain ro mount of a dirty journal would
# silently replay it), retry with the stripped options only after that first
# mount fails, and consult tune2fs first: clean -> plain retry, dirty/unknown
# -> WARNING naming the replay risk, still proceeding (fail-soft).
[[ "$(legacy_filter_mount_options 'ro,noload')" == 'ro,noload' ]] \
    || fail "mount-option filter no longer keeps noload"
[[ "$(legacy_filter_mount_options 'ro,noload,nosuid')" == 'ro,noload,nosuid' ]] \
    || fail "mount-option filter mangled the option list"
[[ "$(legacy_mount_options_without_noload 'ro,noload,nosuid')" == 'ro,nosuid' ]] \
    || fail "retry form did not strip exactly noload"
[[ "$(legacy_mount_options_without_noload 'ro,nosuid')" == 'ro,nosuid' ]] \
    || fail "retry form mangled a noload-free list"
pass "mount-option filter keeps noload; retry form strips exactly noload"

# The helper functions re-armed errexit after the earlier sections; the
# mount-retry scenarios below rely on explicit rc handling, so re-arm set +e
# like the resolver block does.
set +e
mkdir -p "$FIXTURE/mount-retry"
# Restore the real mount_recorded (the split-LV data-mount test left a
# fail-closed stub in its place).
eval "$MOUNT_RECORDED_DEFINITION"
# Successful first attempt: no retry, no tune2fs, noload retained.
(
    trap - EXIT
    : > "$FIXTURE/mount-retry/calls.log"
    mount_calls=0
    tune2fs_calls=0
    mount_recorded_modern() {
        mount_calls=$((mount_calls + 1))
        printf '%s\n' "$*" >> "$FIXTURE/mount-retry/calls.log"
        return 0
    }
    tune2fs() { tune2fs_calls=$((tune2fs_calls + 1)); return 1; }
    mount_recorded /dev/null "$FIXTURE/mount-retry/dst" -o ro,noload \
        || fail "a successful first noload mount must not fail"
    [[ $mount_calls -eq 1 ]] || fail "a successful noload mount must not retry"
    [[ $tune2fs_calls -eq 0 ]] || fail "a successful first mount must not consult tune2fs"
    grep -q 'ro,noload' "$FIXTURE/mount-retry/calls.log" \
        || fail "the first attempt lost noload"
    exit 0
)
rc=$?
[[ $rc -eq 0 ]] || fail "mount-option first-attempt checks failed"
pass "mount-option retry: a successful noload mount keeps noload and never retries"

# First attempt fails + clean journal: one stripped retry (noload removed)
# succeeds; the log records the clean-journal note without a WARNING.
(
    trap - EXIT
    : > "$FIXTURE/mount-retry/calls.log"
    : > "$FIXTURE/mount-retry/log"
    mount_calls=0
    mount_recorded_modern() {
        mount_calls=$((mount_calls + 1))
        printf '%s\n' "$*" >> "$FIXTURE/mount-retry/calls.log"
        if [[ $mount_calls -eq 1 ]]; then return 1; fi
        return 0
    }
    tune2fs() { printf 'Filesystem state:           clean\n'; }
    log() { printf '%s\n' "$*" >> "$FIXTURE/mount-retry/log"; }
    mount_recorded /dev/null "$FIXTURE/mount-retry/dst" -o ro,noload \
        || fail "the clean-journal stripped retry mount failed"
    [[ $mount_calls -eq 2 ]] || fail "a failed noload mount did not retry"
    last_call="$(tail -n1 "$FIXTURE/mount-retry/calls.log")"
    [[ "$last_call" == *'-o ro'* && "$last_call" != *noload* ]] \
        || fail "the retry did not strip noload: $last_call"
    grep -q 'journal is clean' "$FIXTURE/mount-retry/log" \
        || fail "the clean-journal retry left no evidence"
    grep -q 'WARNING' "$FIXTURE/mount-retry/log" \
        && fail "the clean-journal retry logged a WARNING"
    exit 0
)
rc=$?
[[ $rc -eq 0 ]] || fail "clean-journal retry checks failed"
pass "mount-option retry: clean journal -> stripped retry without a WARNING"

# Dirty journal: WARNING naming the replay risk, retry still proceeds.
(
    trap - EXIT
    : > "$FIXTURE/mount-retry/log"
    mount_calls=0
    mount_recorded_modern() {
        mount_calls=$((mount_calls + 1))
        if [[ $mount_calls -eq 1 ]]; then return 1; fi
        return 0
    }
    tune2fs() { printf 'Filesystem state:           not clean\n'; }
    log() { printf '%s\n' "$*" >> "$FIXTURE/mount-retry/log"; }
    mount_recorded /dev/null "$FIXTURE/mount-retry/dst" -o ro,noload \
        || fail "the dirty-journal retry mount failed"
    [[ $mount_calls -eq 2 ]] || fail "the dirty-journal retry did not proceed"
    grep -q 'WARNING' "$FIXTURE/mount-retry/log" \
        || fail "the dirty-journal retry lost its WARNING"
    grep -q 'replay the target' "$FIXTURE/mount-retry/log" \
        || fail "the dirty-journal WARNING does not name the replay risk"
    exit 0
)
rc=$?
[[ $rc -eq 0 ]] || fail "dirty-journal retry checks failed"
pass "mount-option retry: dirty journal -> WARNING naming the replay risk, still proceeds"

# Unreadable journal state (tune2fs fails): WARNING with the unknown state,
# retry still proceeds (fail-soft, never a hard refusal).
(
    trap - EXIT
    : > "$FIXTURE/mount-retry/log"
    mount_calls=0
    mount_recorded_modern() {
        mount_calls=$((mount_calls + 1))
        if [[ $mount_calls -eq 1 ]]; then return 1; fi
        return 0
    }
    tune2fs() { return 1; }
    log() { printf '%s\n' "$*" >> "$FIXTURE/mount-retry/log"; }
    mount_recorded /dev/null "$FIXTURE/mount-retry/dst" -o ro,noload \
        || fail "the unknown-state retry mount failed"
    [[ $mount_calls -eq 2 ]] || fail "the unknown-state retry did not proceed"
    grep -q 'WARNING' "$FIXTURE/mount-retry/log" \
        || fail "the unknown-state retry lost its WARNING"
    grep -q 'unknown' "$FIXTURE/mount-retry/log" \
        || fail "the unknown-state WARNING does not name the unknown state"
    exit 0
)
rc=$?
[[ $rc -eq 0 ]] || fail "unknown-state retry checks failed"
pass "mount-option retry: unknown journal state -> WARNING, still proceeds"

# A mount without noload (non-ext/xfs path) never retries and propagates the
# failure unchanged.
(
    trap - EXIT
    : > "$FIXTURE/mount-retry/calls.log"
    mount_calls=0
    tune2fs_calls=0
    mount_recorded_modern() {
        mount_calls=$((mount_calls + 1))
        printf '%s\n' "$*" >> "$FIXTURE/mount-retry/calls.log"
        return 1
    }
    tune2fs() { tune2fs_calls=$((tune2fs_calls + 1)); printf 'Filesystem state:           clean\n'; }
    mount_recorded /dev/null "$FIXTURE/mount-retry/dst" -o ro,norecovery
    mrc=$?
    [[ $mrc -ne 0 ]] || fail "a failed non-noload mount must propagate its failure"
    [[ $mount_calls -eq 1 ]] || fail "a non-noload mount must not retry"
    [[ $tune2fs_calls -eq 0 ]] || fail "a non-noload mount must not consult tune2fs"
    exit 0
)
rc=$?
[[ $rc -eq 0 ]] || fail "non-noload retry checks failed"
pass "mount-option retry: non-noload mounts never retry and propagate the failure"

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

# --- A9-09/A12-07: resolver copy marker, interrupted-session refusal and
#     skip-restore-when-changed ----------------------------------------------
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
    LEGACY_RESOLVER_MARKER="$FIXTURE/resolver2/resolver-copy-marker"
    realpath_existing() { printf '%s\n' "$1"; }
    path_within() { return 0; }
    log() { printf '%s\n' "$*" >> "$LOG_FILE"; }
    cleanup_modern() { :; }
    # A clean destination is copied over (backup first, marker recorded).
    printf 'nameserver original-target\n' > "$FIXTURE/resolver2/etc/resolv.conf"
    mount_target_resolver >/dev/null 2>&1 \
        || fail "resolver copy failed on a clean destination"
    [[ -f "$SESSION_DIR/resolv.conf.target.before" ]] \
        || fail "resolver backup was not created on the clean destination"
    [[ -s "$LEGACY_RESOLVER_MARKER" ]] \
        || fail "resolver copy marker was not recorded"
    [[ "$(head -n1 "$LEGACY_RESOLVER_MARKER")" == "$FIXTURE/resolver2/etc/resolv.conf" ]] \
        || fail "resolver copy marker does not carry the destination"
    # A12-07: content equality ALONE is not evidence of an interrupted copy —
    # a target whose own resolver file is identical to the host's must keep
    # repairing (recovery rigs behind the same user-mode-network DNS).
    LEGACY_RESOLVER_DESTINATION=""
    LEGACY_RESOLVER_BACKUP=""
    rm -f "$LEGACY_RESOLVER_MARKER" "$SESSION_DIR/resolv.conf.target.before"
    cp -- /etc/resolv.conf "$FIXTURE/resolver2/etc/resolv.conf"
    mount_target_resolver >/dev/null 2>&1 \
        || fail "resolver copy refused an identical-content original without a marker"
    [[ -f "$SESSION_DIR/resolv.conf.target.before" ]] \
        || fail "resolver did not back up the identical-content original"
    # Interrupted-session refusal: the same equal content WITH the persistent
    # marker proves the leftover copy; the polluted file must never be
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
    # overwritten and the skip is logged; the marker goes with the copy.
    rm -f "$LOG_FILE" "$SESSION_DIR/resolv.conf.target.before" "$LEGACY_RESOLVER_MARKER"
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
    [[ ! -e "$LEGACY_RESOLVER_MARKER" ]] \
        || fail "resolver cleanup kept a stale copy marker"
    exit 0
)
rc=$?
[[ $rc -eq 0 ]] || fail "resolver A9-09/A12-07 hardening checks failed"
pass "legacy resolver A9-09/A12-07 (identical-content copy proceeds, marker-proven refusal, skip-restore-when-changed)"

# --- A9-09 follow-up: a failed helper run must keep its exit status through
# the EXIT-trap cleanup chain.  Regression: the resolver wrapper reset $?
# before the renamed modern teardown (cleanup_modern) re-captured it, so
# fail() exited 0.  Run the real chain in a fresh bash: source the staged
# helper (no main), trigger fail() and expect the trap to preserve rc 1. ---
(
    export BOOT_REPAIR_STATE_ROOT="$FIXTURE/rc-preserve-state"
    mkdir -p "$BOOT_REPAIR_STATE_ROOT"
    bash -c '
        # shellcheck disable=SC1090
        source "$1"
        fail "rc-preservation contract probe"
    ' _ "$HELPER_SRC" >/dev/null 2>&1
)
rc=$?
[[ $rc -eq 1 ]] \
    || fail "helper fail path exited $rc, expected 1 (cleanup rc preservation)"
pass "legacy fail path exits nonzero through the cleanup chain (rc preservation)"

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
# A12-02: the parser resolves the binary through the standard-location probe,
# so the test stubs the probe seam with a fixture binary (capture/restore the
# real definition around the stub).
LEGACY_STD_TOOL_DEF="$(declare -f legacy_standard_tool_path)"
cat > "$FIXTURE/cryptsetup-status-stub" <<'EOF'
#!/bin/sh
printf 'device:  /dev/.static/dev/hdb5\n'
EOF
chmod +x "$FIXTURE/cryptsetup-status-stub"
legacy_standard_tool_path() { printf '%s\n' "$FIXTURE/cryptsetup-status-stub"; }
status_device="$(legacy_crypt_status_device smoke-mapper)" \
    || fail "cryptsetup 1.0 status was not parsed"
[[ "$status_device" == "/dev/hdb5" ]] \
    || fail "cryptsetup 1.0 device path was not normalized: $status_device"
cat > "$FIXTURE/cryptsetup-status-stub" <<'EOF'
#!/bin/sh
printf 'cipher:  aes\n'
EOF
chmod +x "$FIXTURE/cryptsetup-status-stub"
legacy_crypt_status_device smoke-mapper \
    && fail "a status without a device field must not yield a device"
eval "$LEGACY_STD_TOOL_DEF"
pass "legacy cryptsetup 1.0 status parsing (device: field, /dev/.static strip)"

# --- A12-02: cryptsetup probe matrix and sbin PATH normalization ------------
# The helper reports cryptsetup as missing when it launches with a desktop
# PATH (no /sbin) even though Etch keeps the binary at /sbin/cryptsetup.
# Both fixes are pinned: the standard-location probe (/sbin, then /usr/sbin,
# then PATH) and the startup PATH normalization prepending the sbin dirs.
mkdir -p "$FIXTURE/probe-sbin" "$FIXTURE/probe-usr-sbin"
BOOT_REPAIR_LEGACY_TOOL_DIRS="$FIXTURE/probe-sbin $FIXTURE/probe-usr-sbin"
# A tool present nowhere (not even PATH) fails closed with an empty path.
[[ -z "$(legacy_standard_tool_path definitely-no-such-tool-xyz)" ]] \
    || fail "the probe returned a path for a tool that exists nowhere"
: > "$FIXTURE/probe-sbin/cryptsetup"
chmod +x "$FIXTURE/probe-sbin/cryptsetup"
[[ "$(legacy_standard_tool_path cryptsetup)" == "$FIXTURE/probe-sbin/cryptsetup" ]] \
    || fail "the probe did not resolve /sbin/cryptsetup first"
rm -f -- "$FIXTURE/probe-sbin/cryptsetup"
: > "$FIXTURE/probe-usr-sbin/cryptsetup"
chmod +x "$FIXTURE/probe-usr-sbin/cryptsetup"
[[ "$(legacy_standard_tool_path cryptsetup)" == "$FIXTURE/probe-usr-sbin/cryptsetup" ]] \
    || fail "the probe did not resolve /usr/sbin/cryptsetup second"
BOOT_REPAIR_LEGACY_TOOL_DIRS=""
rm -rf -- "$FIXTURE/probe-sbin" "$FIXTURE/probe-usr-sbin"
# The startup normalization guarantees /sbin and /usr/sbin are in PATH (the
# overlay top-level code ran while the helper was sourced).
case ":$PATH:" in
    *:/sbin:*) ;;
    *) fail "helper startup did not prepend /sbin to PATH" ;;
esac
case ":$PATH:" in
    *:/usr/sbin:*) ;;
    *) fail "helper startup did not prepend /usr/sbin to PATH" ;;
esac
pass "cryptsetup standard-location probe matrix and sbin PATH normalization"

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

# --- batch B5: config-write content-file transport (A9-01 parity) ------------
# The generated helper inherits the modern guarded transport: a regular,
# symlink-free caller-owned content file (raw path == resolved path) is
# copied into the atomic temp under the 1 MiB stat cap, the content never
# reaches argv, helper output or the session log, and the caller's file is
# never deleted.  The deprecated argv form keeps its 256 KiB Etch bound.
set +e
# Restore the real realpath_existing (the browse-target section left an
# identity stub at the top level); the content-file path proof depends on it.
eval "$REALPATH_EXISTING_DEFINITION"
mkdir -p "$FIXTURE/cfg/etc" "$FIXTURE/cfg/content" "$FIXTURE/cfg/session"
printf 'cfg-old-content\n' > "$FIXTURE/cfg/etc/crypttab"
chmod 640 "$FIXTURE/cfg/etc/crypttab"
: > "$FIXTURE/cfg/session/session.log"
uid="$(id -u)"

# Round-trip through the file form: byte-identical content, target mode
# preserved, success reported, caller file untouched, nothing in the log.
printf 'cfg-b5-marker\nsecond line\n' > "$FIXTURE/cfg/content/content-file"
chmod 600 "$FIXTURE/cfg/content/content-file"
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/cfg"
    SESSION_LOG="$FIXTURE/cfg/session/session.log"
    prepare_target() { return 0; }
    maybe_mount_target_path() { return 0; }
    target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
    SUDO_UID="$uid"
    run_target_config write crypttab \
        --content-file "$FIXTURE/cfg/content/content-file" --content-owner "$uid"
) > "$FIXTURE/cfg/roundtrip.out" 2>&1
cfg_rc=$?
[[ $cfg_rc -eq 0 ]] || fail "content-file round-trip failed: $(cat "$FIXTURE/cfg/roundtrip.out")"
cmp -s "$FIXTURE/cfg/content/content-file" "$FIXTURE/cfg/etc/crypttab" \
    || fail "content-file transport did not write byte-identical content"
[[ "$(stat -c '%a' "$FIXTURE/cfg/etc/crypttab")" == 640 ]] \
    || fail "content-file write did not preserve the target mode"
grep -q 'Target configuration updated: /etc/crypttab' "$FIXTURE/cfg/roundtrip.out" \
    || fail "content-file write did not report success"
grep -q 'cfg-b5-marker' "$FIXTURE/cfg/roundtrip.out" \
    && fail "the transported content appeared in the helper output"
grep -q 'cfg-b5-marker' "$FIXTURE/cfg/session/session.log" \
    && fail "the transported content reached the session log"
[[ -f "$FIXTURE/cfg/content/content-file" ]] \
    || fail "the helper deleted the caller's content file"

# Owner refusal: a SUDO_UID that does not own the file must fail closed.
printf 'cfg-b5-owner\n' > "$FIXTURE/cfg/content/owner-file"
chmod 600 "$FIXTURE/cfg/content/owner-file"
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/cfg"
    SESSION_LOG="$FIXTURE/cfg/session/session.log"
    prepare_target() { return 0; }
    maybe_mount_target_path() { return 0; }
    target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
    SUDO_UID="$((uid + 1))"
    run_target_config write crypttab \
        --content-file "$FIXTURE/cfg/content/owner-file" --content-owner "$uid"
) > "$FIXTURE/cfg/owner-refuse.out" 2>&1
cfg_rc=$?
[[ $cfg_rc -ne 0 ]] || fail "a SUDO_UID-mismatched content file was accepted"
grep -q 'owner cannot be proven' "$FIXTURE/cfg/owner-refuse.out" \
    || fail "content-file owner refusal lacks the proof wording"
[[ -f "$FIXTURE/cfg/content/owner-file" ]] \
    || fail "the owner refusal deleted the caller's content file"

# A zero --content-owner without SUDO_UID can never be proven; the gksu-style
# non-zero matching owner stays accepted.
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/cfg"
    SESSION_LOG="$FIXTURE/cfg/session/session.log"
    prepare_target() { return 0; }
    maybe_mount_target_path() { return 0; }
    target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
    unset SUDO_UID
    run_target_config write crypttab \
        --content-file "$FIXTURE/cfg/content/owner-file" --content-owner 0
) > "$FIXTURE/cfg/zero-owner-refuse.out" 2>&1
cfg_rc=$?
[[ $cfg_rc -ne 0 ]] || fail "a zero --content-owner without SUDO_UID was accepted"
grep -q 'owner cannot be proven' "$FIXTURE/cfg/zero-owner-refuse.out" \
    || fail "zero-owner refusal lacks the proof wording"
# The gksu-style non-zero matching owner stays accepted (unprovable as root,
# where the file uid is 0 and the branch requires a non-zero owner).
if [[ "$uid" != "0" ]]; then
    (
        trap - EXIT
        TARGET_ROOT="$FIXTURE/cfg"
        SESSION_LOG="$FIXTURE/cfg/session/session.log"
        prepare_target() { return 0; }
        maybe_mount_target_path() { return 0; }
        target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
        unset SUDO_UID
        run_target_config write crypttab \
            --content-file "$FIXTURE/cfg/content/owner-file" --content-owner "$uid"
    ) > "$FIXTURE/cfg/gksu-accept.out" 2>&1
    cfg_rc=$?
    [[ $cfg_rc -eq 0 ]] || fail "the gksu-style --content-owner match was refused"
    cmp -s "$FIXTURE/cfg/content/owner-file" "$FIXTURE/cfg/etc/crypttab" \
        || fail "the gksu-style content write did not land the content"
    [[ -f "$FIXTURE/cfg/content/owner-file" ]] \
        || fail "the gksu-style write deleted the caller's content file"
else
    printf 'skip - root run cannot exercise the non-zero --content-owner branch\n'
fi

# Symlink refusal: the final component and an intermediate directory must
# both be refused before the file is read, and the caller file survives.
ln -s "$FIXTURE/cfg/content/content-file" "$FIXTURE/cfg/content/content-link"
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/cfg"
    SESSION_LOG="$FIXTURE/cfg/session/session.log"
    prepare_target() { return 0; }
    maybe_mount_target_path() { return 0; }
    target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
    SUDO_UID="$uid"
    run_target_config write crypttab \
        --content-file "$FIXTURE/cfg/content/content-link" --content-owner "$uid"
) > "$FIXTURE/cfg/symlink-refuse.out" 2>&1
cfg_rc=$?
[[ $cfg_rc -ne 0 ]] || fail "a symlinked content-file final component was accepted"
grep -q 'symlink-free regular file' "$FIXTURE/cfg/symlink-refuse.out" \
    || fail "symlink refusal lacks the path wording"
mkdir -p "$FIXTURE/cfg/content-dir"
printf 'cfg-b5-intermediate\n' > "$FIXTURE/cfg/content-dir/plain"
chmod 600 "$FIXTURE/cfg/content-dir/plain"
ln -s "$FIXTURE/cfg/content-dir" "$FIXTURE/cfg/content-dir-link"
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/cfg"
    SESSION_LOG="$FIXTURE/cfg/session/session.log"
    prepare_target() { return 0; }
    maybe_mount_target_path() { return 0; }
    target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
    SUDO_UID="$uid"
    run_target_config write crypttab \
        --content-file "$FIXTURE/cfg/content-dir-link/plain" --content-owner "$uid"
) > "$FIXTURE/cfg/intermediate-refuse.out" 2>&1
cfg_rc=$?
[[ $cfg_rc -ne 0 ]] || fail "a content file reached through a symlinked directory was accepted"
grep -q 'symlink-free regular file' "$FIXTURE/cfg/intermediate-refuse.out" \
    || fail "intermediate-symlink refusal lacks the path wording"
[[ -f "$FIXTURE/cfg/content/content-file" && -f "$FIXTURE/cfg/content-dir/plain" ]] \
    || fail "the symlink refusals deleted the caller's content files"

# Oversize refusal at the 1 MiB stat cap; an exactly-1 MiB file is accepted.
head -c 1048577 /dev/zero > "$FIXTURE/cfg/content/oversize-file"
chmod 600 "$FIXTURE/cfg/content/oversize-file"
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/cfg"
    SESSION_LOG="$FIXTURE/cfg/session/session.log"
    prepare_target() { return 0; }
    maybe_mount_target_path() { return 0; }
    target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
    SUDO_UID="$uid"
    run_target_config write crypttab \
        --content-file "$FIXTURE/cfg/content/oversize-file" --content-owner "$uid"
) > "$FIXTURE/cfg/oversize-refuse.out" 2>&1
cfg_rc=$?
[[ $cfg_rc -ne 0 ]] || fail "an over-1 MiB content file was accepted"
grep -q 'limited to 1 MiB' "$FIXTURE/cfg/oversize-refuse.out" \
    || fail "oversize refusal does not name the 1 MiB cap"
[[ -f "$FIXTURE/cfg/content/oversize-file" ]] \
    || fail "the oversize refusal deleted the caller's content file"
head -c 1048576 /dev/zero | tr '\0' 'x' > "$FIXTURE/cfg/content/exact-file"
chmod 600 "$FIXTURE/cfg/content/exact-file"
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/cfg"
    SESSION_LOG="$FIXTURE/cfg/session/session.log"
    prepare_target() { return 0; }
    maybe_mount_target_path() { return 0; }
    target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
    SUDO_UID="$uid"
    run_target_config write crypttab \
        --content-file "$FIXTURE/cfg/content/exact-file" --content-owner "$uid"
) > "$FIXTURE/cfg/exact-accept.out" 2>&1
cfg_rc=$?
[[ $cfg_rc -eq 0 ]] || fail "an exactly-1 MiB content file was refused"
cmp -s "$FIXTURE/cfg/content/exact-file" "$FIXTURE/cfg/etc/crypttab" \
    || fail "the exactly-1 MiB write did not land byte-identical content"

# Truncated/malformed option shapes refuse before any write.
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/cfg"
    SESSION_LOG="$FIXTURE/cfg/session/session.log"
    prepare_target() { return 0; }
    maybe_mount_target_path() { return 0; }
    target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
    SUDO_UID="$uid"
    run_target_config write crypttab \
        --content-file "$FIXTURE/cfg/content/content-file"
) > "$FIXTURE/cfg/argc-refuse.out" 2>&1
cfg_rc=$?
[[ $cfg_rc -ne 0 ]] || fail "a truncated --content-file form was accepted"
grep -q 'invalid argument count' "$FIXTURE/cfg/argc-refuse.out" \
    || fail "truncated-form refusal lacks the argument-count wording"
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/cfg"
    SESSION_LOG="$FIXTURE/cfg/session/session.log"
    prepare_target() { return 0; }
    maybe_mount_target_path() { return 0; }
    target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
    SUDO_UID="$uid"
    run_target_config write crypttab \
        --content-file "$FIXTURE/cfg/content/content-file" --content-owner-extra "$uid"
) > "$FIXTURE/cfg/shape-refuse.out" 2>&1
cfg_rc=$?
[[ $cfg_rc -ne 0 ]] || fail "a malformed --content-file option shape was accepted"
grep -q 'requires --content-owner' "$FIXTURE/cfg/shape-refuse.out" \
    || fail "malformed-shape refusal lacks the --content-owner wording"

# The deprecated argv form still works (existing Etch bound, 256 KiB).
printf 'cfg-old-content\n' > "$FIXTURE/cfg/etc/crypttab"
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/cfg"
    SESSION_LOG="$FIXTURE/cfg/session/session.log"
    prepare_target() { return 0; }
    maybe_mount_target_path() { return 0; }
    target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
    run_target_config write crypttab 'cfg-b5-argv-content'
) > "$FIXTURE/cfg/argv.out" 2>&1
cfg_rc=$?
[[ $cfg_rc -eq 0 ]] || fail "the argv fallback form failed: $(cat "$FIXTURE/cfg/argv.out")"
[[ "$(cat "$FIXTURE/cfg/etc/crypttab")" == 'cfg-b5-argv-content' ]] \
    || fail "the argv fallback form did not write the content"
cfg_big="$(head -c 262145 /dev/zero | tr '\0' 'x')"
(
    trap - EXIT
    TARGET_ROOT="$FIXTURE/cfg"
    SESSION_LOG="$FIXTURE/cfg/session/session.log"
    prepare_target() { return 0; }
    maybe_mount_target_path() { return 0; }
    target_config_path() { printf '%s\n' "$TARGET_ROOT$1"; }
    run_target_config write crypttab "$cfg_big"
) > "$FIXTURE/cfg/argv-oversize.out" 2>&1
cfg_rc=$?
[[ $cfg_rc -ne 0 ]] || fail "an over-256 KiB argv payload was accepted"
grep -q 'limited to 256 KiB' "$FIXTURE/cfg/argv-oversize.out" \
    || fail "argv oversize refusal does not name the 256 KiB bound"
rm -rf "$FIXTURE/cfg"
set -e
pass "config-write content-file transport (round-trip, owner/symlink/oversize refusals, argv fallback, no log leak, caller file survives)"

# --- B4/A10-05: cancel-token surface -----------------------------------------
# The existence signal (CANCEL_FILE) and the token form (CANCEL_TOKEN +
# $SESSION_DIR/cancel with matching content) must be distinguished exactly:
# an absent file, a stray file with the wrong content, or no configured
# surface at all never cancels a run; the matching forms do.
CANCEL_FILE="$FIXTURE/cancel-request"
CANCEL_TOKEN=""
rm -f "$CANCEL_FILE"
legacy_cancel_requested && fail "an absent cancel file was treated as a cancel request"
: > "$CANCEL_FILE"
legacy_cancel_requested || fail "a present cancel file was not treated as a cancel request"
rm -f "$CANCEL_FILE"
CANCEL_FILE=""
legacy_cancel_requested && fail "a cancel request was reported without any cancel surface"

CANCEL_TOKEN="smoke-session-token"
SESSION_DIR="$FIXTURE/cancel-session"
mkdir -p "$SESSION_DIR"
legacy_cancel_requested && fail "the token form cancelled without a cancel file"
printf 'smoke-session-token\n' > "$SESSION_DIR/cancel"
legacy_cancel_requested || fail "a matching session cancel token was not honoured"
printf 'wrong-token\n' > "$SESSION_DIR/cancel"
legacy_cancel_requested && fail "a mismatched session cancel token was honoured"
rm -f "$SESSION_DIR/cancel"
CANCEL_TOKEN=""
SESSION_DIR=""

# The stage-boundary check stays silent without a request and aborts through
# fail() (exit 1, "Cancelled at the caller's request.") when one is present.
CANCEL_FILE="$FIXTURE/cancel-request"
rm -f "$CANCEL_FILE"
legacy_cancel_stage_check || fail "the stage check failed without a cancel request"
: > "$CANCEL_FILE"
CURRENT_STAGE=""
set +e
( legacy_cancel_stage_check ) > "$FIXTURE/cancel-abort.txt" 2>&1
cancel_rc=$?
set -e
[[ $cancel_rc -ne 0 ]] || fail "the stage check did not abort on the cancel token"
grep -q "Cancelled at the caller.s request" "$FIXTURE/cancel-abort.txt" \
    || fail "the stage abort left no evidence"
rm -f "$CANCEL_FILE"
CANCEL_FILE=""
pass "legacy cancel token (existence signal, token form, stage-boundary abort)"

echo "legacy helper smoke: PASS"
