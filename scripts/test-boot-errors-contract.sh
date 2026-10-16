#!/usr/bin/env bash
# Boot errors (`errors` diagnostic) shared boot-failure classification
# contract.  The `errors` diagnostic is the canonical place for boot-failure
# evidence: `boot_failure_classification()` is the single shared classifier
# that reads the session journal cache and sets BOOT_REACHED +
# BOOT_FAILURE_MARKERS, `diagnostic_errors()` emits them, and the GPU capability
# gate consumes the same BOOT_REACHED instead of re-deriving it.
#
# Static wiring plus a behavioural harness that sources the helper without main
# and stubs only the read-only journal cache.  No real block device, package
# manager or journalctl is ever touched.
# shellcheck disable=SC1090,SC2034,SC2317
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="${HELPER:-$ROOT_DIR/scripts/boot-repair-helper.sh}"

[[ -x "$HELPER" ]] || { echo 'FAIL: helper is not executable' >&2; exit 1; }
bash -n "$HELPER"

fail() { printf 'FAIL: %s\n' "$*" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Static wiring contracts
# ---------------------------------------------------------------------------
grep -q '^boot_failure_classification()' "$HELPER" \
    || fail 'helper is missing boot_failure_classification()'
grep -q '^diagnostic_errors()' "$HELPER" \
    || fail 'helper is missing diagnostic_errors()'
grep -q 'BOOT_REACHED="grub"' "$HELPER" \
    || fail 'BOOT_REACHED must default to grub'
grep -q "printf 'Boot reached: %s" "$HELPER" \
    || fail 'diagnostic_errors must emit a Boot reached: line'
grep -q "printf 'Boot failure markers:" "$HELPER" \
    || fail 'diagnostic_errors must emit a Boot failure markers: line'
# The classifier must reuse the existing session journal cache, never spawn a
# fresh journalctl.  It reads the boot HEAD slice (the earliest entries), not
# the newest-entry window, because the boot-complete markers land in the first
# couple of minutes and fall outside the newest-1600 tail on a long boot.
grep -q 'journal_cached_boot_head_lines 4000 | journal_boot_relevant_filter' "$HELPER" \
    || fail 'the classifier must read the shared session journal head cache'
grep -q '^journal_cached_boot_head_lines()' "$HELPER" \
    || fail 'helper is missing journal_cached_boot_head_lines()'
grep -q 'boot-head-4000.json' "$HELPER" \
    || fail 'the journal cache must capture the earliest boot entries as boot-head-4000.json'
grep -q '^non_systemd_current_boot_syslog_lines()' "$HELPER" \
    || fail 'helper is missing non_systemd_current_boot_syslog_lines()'
grep -q 'non_systemd_current_boot_syslog_lines' "$HELPER" \
    || fail 'the classifier must consume the current-boot syslog slice on the non-journald path'

# ---------------------------------------------------------------------------
# Behavioural harness
# ---------------------------------------------------------------------------
sandbox="$(mktemp -d)"
cleanup_sandbox() { rm -rf -- "$sandbox"; }
trap cleanup_sandbox EXIT

fake_root="$sandbox/root"
mkdir -p "$fake_root/var/log/journal"

source <(sed '/^main "$@"/d' "$HELPER")
trap - EXIT INT TERM HUP

JOURNAL_FIXTURE="$sandbox/journal.txt"
journal_cached_boot_head_lines() { head -n "${1:-4000}" "$JOURNAL_FIXTURE" 2>/dev/null || true; }
journal_boot_relevant_filter() { cat; }
non_journald_log_fallback_ready() { return 1; }
non_systemd_raw_log_lines() { :; }
target_logging_backend() { printf 'journald\n'; }
target_journal_evidence_present() { return 0; }

TARGET_ROOT="$fake_root"
RUNNING_HOST_MODE=0
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"

# classify <snippet>: feed a hand-crafted journal snippet through the shared
# classifier and print "BOOT_REACHED|BOOT_FAILURE_MARKERS".
classify() {
    BOOT_FAILURE_CLASSIFIED=0
    printf '%s\n' "$1" > "$JOURNAL_FIXTURE"
    boot_failure_classification
    printf '%s|%s\n' "$BOOT_REACHED" "$BOOT_FAILURE_MARKERS"
}

expect() {
    local name="$1" snippet="$2" want_reached="$3" want_markers="$4" got
    got="$(classify "$snippet")"
    [[ "$got" == "${want_reached}|${want_markers}" ]] \
        || fail "$name: got '$got', want '${want_reached}|${want_markers}'"
}

# desktop: the graphical session marker is the highest reached level.
expect desktop \
    'Oct 04 10:00:00 host systemd: Reached target Graphical Interface.' \
    'desktop' ''

# login-manager: a display manager started but no desktop session yet.
expect login-manager \
    'Oct 04 10:00:00 host systemd: Started Simple Desktop Display Manager.' \
    'login-manager' ''

# tty: multi-user reached but not graphical -> tty-only marker.
expect tty \
    'Oct 04 10:00:00 host systemd: Reached target Multi-User System.' \
    'tty' 'tty-only'

# kernel: only the kernel/command-line marker is present.
expect kernel \
    'Oct 04 10:00:00 host kernel: Linux version 6.8.0-123' \
    'kernel' ''

# grub: no boot evidence -> the fail-closed default.
expect grub \
    '' \
    'grub' ''

# emergency-shell: dropped to the emergency shell.
expect emergency-shell \
    'Oct 04 10:00:00 host systemd: Welcome to emergency mode.' \
    'grub' 'emergency-shell'

# nomodeset: kernel cmdline evidence -> graphics-save-mode (and kernel reached).
expect nomodeset \
    'Oct 04 10:00:00 host kernel: Command line: BOOT_IMAGE=/vmlinuz root=/dev/sda1 nomodeset' \
    'kernel' 'graphics-save-mode'

# rescue: an obvious rescue-shell marker.
expect rescue \
    'Oct 04 10:00:00 host systemd: Started Rescue Shell.' \
    'grub' 'rescue'

# combined markers: tty-only plus nomodeset, comma-joined deterministically.
expect tty-nomodeset \
    $'Oct 04 10:00:00 host systemd: Reached target Multi-User System.\nOct 04 10:00:00 host kernel: Command line: BOOT_IMAGE=/vmlinuz root=/dev/sda1 nomodeset' \
    'tty' 'graphics-save-mode,tty-only'

# rsyslog unit-interpolated forms: systemd interpolates the unit name into the
# message ("graphical.target - Graphical Interface." / "gdm.service - GNOME
# Display Manager." / "systemd-logind.service - User Login Management."), not
# the short journald form.  Both forms must classify identically.
expect desktop-rsyslog \
    'Oct 04 10:00:00 host systemd: Reached target graphical.target - Graphical Interface.' \
    'desktop' ''

expect login-manager-rsyslog \
    'Oct 04 10:00:00 host systemd: Started gdm.service - GNOME Display Manager.' \
    'login-manager' ''

expect tty-rsyslog \
    'Oct 04 10:00:00 host systemd: Reached target multi-user.target - Multi-User System.' \
    'tty' 'tty-only'

expect tty-rsyslog-logind \
    'Oct 04 10:00:00 host systemd: Started systemd-logind.service - User Login Management.' \
    'tty' 'tty-only'

# ---- head slice: a boot longer than 1600 entries ----------------------------
# The boot-complete markers live in the earliest entries (first couple of
# minutes), outside the newest-1600 tail on a long boot.  The classifier must
# read the boot head slice so the marker is still visible and the boot is
# classified desktop, not the grub default.
HEAD_SLICE_FIXTURE="$sandbox/head-slice.txt"
{
    printf '%s\n' 'Oct 04 10:00:00 host systemd: Reached target Graphical Interface.'
    i=0
    while (( i < 2000 )); do
        printf '%s\n' "Oct 04 10:00:01 host systemd: filler-$i.service: line $i"
        (( i += 1 ))
    done
} > "$HEAD_SLICE_FIXTURE"
if grep -q 'Reached target Graphical Interface' <<<"$(tail -n 1600 "$HEAD_SLICE_FIXTURE" 2>/dev/null)"; then
    fail 'head-slice fixture: boot marker unexpectedly inside the newest-1600 tail'
fi
grep -q 'Reached target Graphical Interface' <<<"$(head -n 4000 "$HEAD_SLICE_FIXTURE" 2>/dev/null)" \
    || fail 'head-slice fixture: boot marker missing from the 4000-entry head slice'

journal_cached_boot_head_lines() { head -n "${1:-4000}" "$HEAD_SLICE_FIXTURE" 2>/dev/null || true; }
BOOT_FAILURE_CLASSIFIED=0
boot_failure_classification
[[ "$BOOT_REACHED" == desktop ]] \
    || fail "head slice: boot beyond the 1600-entry tail classified '$BOOT_REACHED', want 'desktop'"
[[ -z "$BOOT_FAILURE_MARKERS" ]] \
    || fail "head slice: unexpected boot failure markers '$BOOT_FAILURE_MARKERS'"

# ---- diagnostic_errors emits the shared classification ----------------------
ERRORS_FIXTURE="$sandbox/errors-journal.txt"
printf '%s\n' \
    'Oct 04 10:00:00 host systemd: Reached target Graphical Interface.' \
    'Oct 04 10:00:01 host systemd: unit.service: Failed with result timeout.' \
    > "$ERRORS_FIXTURE"

# Re-route the classifier's journal read to the errors fixture and stub the
# error-priority pipeline pieces diagnostic_errors uses.
journal_cached_boot_head_lines() { head -n "${1:-4000}" "$ERRORS_FIXTURE" 2>/dev/null || true; }
journal_cached_error_lines() { :; }
journal_current_boot_actionable() { cat; }
collapse_similar_journal_lines() { cat; }
diagnostic_package_manager_logs() { :; }
target_journal_evidence_present() { return 0; }
profile_target_backends() { TARGET_LOGGING_BACKEND=journald; }

out="$(diagnostic_errors)"
grep -q '^Boot reached: desktop$' <<<"$out" \
    || fail "diagnostic_errors must emit 'Boot reached: desktop'; got: $out"
grep -q '^Boot failure markers: none$' <<<"$out" \
    || fail "diagnostic_errors must emit 'Boot failure markers: none' when clean; got: $out"

# ---- non-journald path: scan the current boot slice, not a fixed tail --------
# On a long-lived messages file the boot markers sit far above a tail window;
# the classifier must scan from the last kernel "Command line:" marker so the
# markers are found without streaming the whole (audit-heavy) file.
CURBOOT_ROOT="$sandbox/curboot-root"
mkdir -p "$CURBOOT_ROOT/var/log"
CURBOOT_MESSAGES="$CURBOOT_ROOT/var/log/messages"
{
    i=0
    while (( i < 6000 )); do
        printf '%s\n' "Oct 03 09:00:00 host audit: AVC avc: denied old-boot-$i"
        (( i += 1 ))
    done
    printf '%s\n' 'Oct 04 10:00:00 host kernel: Command line: BOOT_IMAGE=/vmlinuz root=/dev/sda1'
    printf '%s\n' 'Oct 04 10:00:01 host systemd: Reached target graphical.target - Graphical Interface.'
    i=0
    while (( i < 2000 )); do
        printf '%s\n' "Oct 04 10:00:02 host audit: AVC avc: denied current-boot-$i"
        (( i += 1 ))
    done
} > "$CURBOOT_MESSAGES"

# The slice starts at the kernel command line, discarding the earlier boot's
# audit noise, and still includes the early boot marker.
slice="$(TARGET_ROOT="$CURBOOT_ROOT" non_systemd_current_boot_syslog_lines)"
grep -q '^Oct 04 10:00:00 host kernel: Command line:' <<<"$slice" \
    || fail 'current-boot slice must begin at the kernel Command line: marker'
grep -q 'Reached target graphical.target - Graphical Interface.' <<<"$slice" \
    || fail 'current-boot slice must include the early boot marker'
! grep -q 'old-boot-0' <<<"$slice" \
    || fail 'current-boot slice must discard earlier boots noise'

# The classifier routes the non-journald path through that slice and reaches
# desktop even though the marker sits deep in the file.
TARGET_ROOT="$CURBOOT_ROOT"
target_logging_backend() { printf 'messages\n'; }
non_journald_log_fallback_ready() { return 0; }
non_systemd_kernel_log_lines() { :; }
BOOT_FAILURE_CLASSIFIED=0
boot_failure_classification
[[ "$BOOT_REACHED" == desktop ]] \
    || fail "non-journald classifier classified '$BOOT_REACHED', want 'desktop'"
[[ -z "$BOOT_FAILURE_MARKERS" ]] \
    || fail "non-journald classifier: unexpected failure markers '$BOOT_FAILURE_MARKERS'"

echo 'boot-errors-contract: PASS'
