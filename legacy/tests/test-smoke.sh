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

# --- legacy browse-target record format (cycle 8) ----------------------------
# The legacy port emits BROWSE_ENTRY records with raw percent-encoded names
# (Qt 3.3.7 has no QByteArray::fromBase64); the Qt3 picker decodes them.
mkdir -p "$FIXTURE/browse-root/alpha" "$FIXTURE/browse-root/beta"
prepare_target() { :; }
maybe_mount_target_path() { :; }
realpath_existing() { printf '%s\n' "$1"; }
TARGET_ROOT="$FIXTURE"
browse_out="$(browse_target_directory /browse-root)"
printf '%s\n' "$browse_out" | grep -q 'BROWSE_ENTRY	alpha' \
    || fail "browse-target did not list alpha"
printf '%s\n' "$browse_out" | grep -q 'BROWSE_ENTRY	beta' \
    || fail "browse-target did not list beta"
printf '%s\n' "$browse_out" | grep -q 'BROWSE_ENTRY	' \
    || fail "browse-target emitted no BROWSE_ENTRY records"
pass "legacy browse-target records (raw percent-encoded names)"

echo "legacy helper smoke: PASS"
