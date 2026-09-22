#!/usr/bin/env bash
# Contract test for the repair-log TPM2/LUKS verbose noise filter.
#
# tpm2-tools on TPM2/LUKS targets makes the initramfs tooling print a verbose
# enrollment dump (tpm2-* policy fields, multi-line hex blobs, cryptsetup
# keyslot and digest hex) after every image build.  The filter must collapse
# those blocks into one concise summary line in the repair transcript without
# suppressing errors, warnings or unrelated output, and the raw captured
# command output must stay available to callers that parse it.
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
# Static contract: the filter exists and every repair transcript pipeline
# runs through it (run_chroot, run_chroot_try and both UKI builder paths).
# ---------------------------------------------------------------------------
grep -q '^repair_log_filter()' "$HELPER" || fail_test 'missing repair_log_filter helper'
filtered_pipelines="$(grep -c 'repair_log_filter | tee -a "\$SESSION_LOG"' "$HELPER" || true)"
[[ "$filtered_pipelines" -eq 4 ]] \
    || fail_test "expected 4 filtered transcript pipelines, found $filtered_pipelines"

# ---------------------------------------------------------------------------
# Fixture: the exact verbose block shape observed in the TUXEDO repair log,
# plus surrounding concise lines, an error and a warning inside the block and
# a non-block hex line that must never be collapsed.
# ---------------------------------------------------------------------------
WORK_ROOT="$(mktemp -d)"
trap 'rm -rf -- "$WORK_ROOT"' EXIT

cat > "$WORK_ROOT/verbose-block.txt" <<'BLOCK'
update-initramfs: Generating /boot/initrd.img-6.1.0-test
	tpm2-hash-pcrs:   7
	tpm2-pcr-bank:    sha256
	tpm2-pubkey:
	            (null)
	tpm2-pubkey-pcrs: 
	tpm2-primary-alg: ecc
	tpm2-pin:         false
	tpm2-pcrlock:     false
	tpm2-salt:        false
	tpm2-srk:         true
	tpm2-pcrlock-nv:  false
	tpm2-policy-hash:
	            80 33 9f 04 b4 57 03 0f b2 f7 39 9b 6c 7f 7a 90
	tpm2-blob:  00 9e 00 20 62 7d 75 83 e1 16 1b a0 dc 1e 6f cc
	            95 1a 49 b7 d4 85 55 38 b4 2d a0 81 ed df 4a 0f
	tpm2-pcrlock: warning: pcrlock policy not sealed
	Keyslot:    3
Digests:
  0: pbkdf2
	Hash:       sha512
	Iterations: 562540
	Salt:       
c5 
65 
	Digest:     
c2 
78 
Command successful.
BLOCK

filtered="$(bash -c 'source <(sed "/^main \"\$@\"\$/d" "$1"); repair_log_filter' _ "$HELPER" < "$WORK_ROOT/verbose-block.txt")"

grep -Fq 'update-initramfs: Generating /boot/initrd.img-6.1.0-test' <<<"$filtered" \
    || fail_test 'filter dropped the concise initramfs header'
grep -Fq 'Command successful.' <<<"$filtered" \
    || fail_test 'filter dropped the concise command result line'
grep -Fq 'TPM2/LUKS verbose output filtered:' <<<"$filtered" \
    || fail_test 'filter did not emit the summary line'
grep -Fq 'tpm2-hash-pcrs=7' <<<"$filtered" \
    || fail_test 'summary does not carry tpm2-hash-pcrs'
grep -Fq 'tpm2-pcr-bank=sha256' <<<"$filtered" \
    || fail_test 'summary does not carry tpm2-pcr-bank'
grep -Fq 'tpm2-primary-alg=ecc' <<<"$filtered" \
    || fail_test 'summary does not carry tpm2-primary-alg'
grep -Fq 'keyslot=3' <<<"$filtered" \
    || fail_test 'summary does not carry the keyslot'
grep -Fq 'line(s) suppressed' <<<"$filtered" \
    || fail_test 'summary does not carry the suppressed-line count'
grep -Fq 'tpm2-pcrlock: warning: pcrlock policy not sealed' <<<"$filtered" \
    || fail_test 'filter hid a warning inside the verbose block'
for verbose in 'tpm2-blob' 'tpm2-policy-hash' 'Keyslot:' 'Digests:' '562540' '95 1a 49 b7' 'tpm2-pubkey:'; do
    if grep -Fq "$verbose" <<<"$filtered"; then
        fail_test "filter leaked verbose content: $verbose"
    fi
done

# A non-block hex line outside any verbose block must pass through untouched.
printf 'sha256 digest: 00 9e 00 20 62 7d 75 83\n' > "$WORK_ROOT/unrelated.txt"
unrelated="$(bash -c 'source <(sed "/^main \"\$@\"\$/d" "$1"); repair_log_filter' _ "$HELPER" < "$WORK_ROOT/unrelated.txt")"
grep -Fq 'sha256 digest: 00 9e 00 20 62 7d 75 83' <<<"$unrelated" \
    || fail_test 'filter collapsed unrelated hex-looking output'

# Real failures pass through: a block at EOF still yields the summary, and an
# update-initramfs failure line is never suppressed.
cat > "$WORK_ROOT/failure.txt" <<'FAILURE'
update-initramfs: Generating /boot/initrd.img-6.1.0-test
	tpm2-blob:  00 9e 00 20 62 7d 75 83
update-initramfs: failed for /boot/initrd.img-6.1.0-test with 1.
FAILURE
failure="$(bash -c 'source <(sed "/^main \"\$@\"\$/d" "$1"); repair_log_filter' _ "$HELPER" < "$WORK_ROOT/failure.txt")"
grep -Fq 'TPM2/LUKS verbose output filtered' <<<"$failure" \
    || fail_test 'filter did not summarize a block terminated by a failure line'
grep -Fq 'update-initramfs: failed for /boot/initrd.img-6.1.0-test with 1.' <<<"$failure" \
    || fail_test 'filter hid an update-initramfs failure'

# Idempotence: filtering the filtered transcript adds nothing and removes
# nothing.
twice="$(bash -c 'source <(sed "/^main \"\$@\"\$/d" "$1"); repair_log_filter' _ "$HELPER" <<<"$filtered")"
[[ "$twice" == "$filtered" ]] || fail_test 'filter is not idempotent'

# Empty input stays empty.
empty="$(printf '' | bash -c 'source <(sed "/^main \"\$@\"\$/d" "$1"); repair_log_filter' _ "$HELPER")"
[[ -z "$empty" ]] || fail_test 'filter invented output for empty input'

# ---------------------------------------------------------------------------
# Integration: run_chroot_try and run_chroot must write the filtered
# transcript to stdout and the session log while CHROOT_TRY_OUTPUT keeps the
# raw command output for callers that parse it.
# ---------------------------------------------------------------------------
source <(sed '/^main "\$@"$/d' "$HELPER")
trap - EXIT INT TERM HUP
trap 'rm -rf -- "$WORK_ROOT"' EXIT

SESSION_DIR="$WORK_ROOT/session"
SESSION_LOG="$SESSION_DIR/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"

run_selected_chroot()
{
    cat "$WORK_ROOT/verbose-block.txt"
}

run_chroot_try "Update initramfs for 6.1.0-test" update-initramfs -u -k 6.1.0-test \
    > "$WORK_ROOT/try.out" 2>&1
grep -Fq 'TPM2/LUKS verbose output filtered' "$WORK_ROOT/try.out" \
    || fail_test 'run_chroot_try stdout was not filtered'
grep -Fq 'TPM2/LUKS verbose output filtered' "$SESSION_LOG" \
    || fail_test 'run_chroot_try session log was not filtered'
if grep -Fq 'tpm2-blob' "$WORK_ROOT/try.out" "$SESSION_LOG"; then
    fail_test 'run_chroot_try leaked the verbose blob'
fi
grep -Fq 'tpm2-blob' <<<"$CHROOT_TRY_OUTPUT" \
    || fail_test 'CHROOT_TRY_OUTPUT must keep the raw command output'
grep -Fq 'update-initramfs: failed' <<<"$CHROOT_TRY_OUTPUT" && fail_test 'fixture unexpectedly failed'

: > "$SESSION_LOG"
run_selected_chroot()
{
    cat "$WORK_ROOT/failure.txt"
    return 1
}
if ( run_chroot "Update initramfs for 6.1.0-test" update-initramfs -u -k 6.1.0-test ) \
    > "$WORK_ROOT/chroot.out" 2>&1; then
    fail_test 'run_chroot accepted a failing command'
fi
grep -Fq 'TPM2/LUKS verbose output filtered' "$WORK_ROOT/chroot.out" \
    || fail_test 'run_chroot stdout was not filtered'
grep -Fq 'update-initramfs: failed for /boot/initrd.img-6.1.0-test with 1.' "$WORK_ROOT/chroot.out" \
    || fail_test 'run_chroot hid the command failure'
grep -Fq 'TPM2/LUKS verbose output filtered' "$SESSION_LOG" \
    || fail_test 'run_chroot session log was not filtered'

echo "PASS: repair-log TPM2/LUKS verbose noise is filtered with errors preserved."
