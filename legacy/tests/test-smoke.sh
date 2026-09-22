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
pass "legacy root confirmation and os-release-less refusal"

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
grep -q '^Repair tool bootstack: unavailable|' "$FIXTURE/diagnose.txt" \
    || fail "diagnose did not gate boot-stack off"
grep -q '^Repair tool grub: available$' "$FIXTURE/diagnose.txt" \
    || fail "diagnose did not enable the GRUB legacy branch"
grep -q '^Legacy feature file-copy: ' "$FIXTURE/diagnose.txt" \
    || fail "diagnose did not report legacy feature gating"
grep -q '^Diagnostic: luks$' "$FIXTURE/diagnose.txt" || fail "diagnose all skipped a section"
pass "read-only validate and diagnose against the fixture root"

echo "legacy helper smoke: PASS"
