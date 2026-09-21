#!/usr/bin/env bash
# This contract test sources the helper under test dynamically and sets the
# helper's globals directly so ShellCheck cannot track their use.
# shellcheck disable=SC1090,SC2034
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="$ROOT_DIR/scripts/boot-repair-helper.sh"

[[ -x "$HELPER" ]] || { echo "FAIL: helper is not executable" >&2; exit 1; }
bash -n "$HELPER"

# Keep the shell action wired through every privileged entry point. This is a
# static contract test so it never mounts a real target or executes as root.
grep -q '^  \$PROGRAM_NAME shell' "$HELPER"
grep -q 'shell            Execute one reviewed command' "$HELPER"
grep -q 'unlock|validate|diagnose|config-read|config-write|snapshots|repair|shell|' "$HELPER"
grep -q '^        shell)' "$HELPER"
grep -q 'run_chroot_shell "\$1"' "$HELPER"

# The chroot shell must run commands non-interactively: stdin from /dev/null
# (never the privileged-session protocol pipe), a non-interactive dnf5/apt
# environment, a bounded runtime and a clear hint when a command still asks a
# question.  The host-shell path is deliberately unchanged.
chroot_shell_body="$(sed -n '/^run_chroot_shell()/,/^}/p' "$HELPER")"
[[ -n "$chroot_shell_body" ]] || { echo 'FAIL: run_chroot_shell is missing' >&2; exit 1; }
grep -Fq '< /dev/null' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not bind stdin to /dev/null' >&2; exit 1; }
grep -Fq 'DNF5_FORCE_INTERACTIVE=0' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not force dnf5 non-interactive' >&2; exit 1; }
grep -Fq 'DEBIAN_FRONTEND=noninteractive' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not keep apt non-interactive' >&2; exit 1; }
grep -Fq 'CHROOT_SHELL_TIMEOUT_SECONDS' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell runtime is not bounded by a named limit' >&2; exit 1; }
grep -Fq -- '--kill-after' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell bounded runtime cannot kill a stuck command' >&2; exit 1; }
grep -Fq 'interactive question' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not explain an interactive-prompt abort' >&2; exit 1; }
# The hint must name the concrete non-interactive form for every package
# backend family, and the failed command must be reported with its exit code.
for hint in 'dnf update -y' 'apt-get -y upgrade' 'pacman --noconfirm -Syu'; do
    grep -Fq "$hint" <<<"$chroot_shell_body" \
        || { echo "FAIL: the chroot shell prompt hint does not name '$hint'" >&2; exit 1; }
done
grep -Fq 'Chroot shell command failed (exit code $rc).' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not report the failed command' >&2; exit 1; }

# Host Maintenance mode uses a separate guarded host-shell path.  It must
# validate the running host identity and activate the host command guard, and
# must never be routed through the target chroot runner.
grep -q '^  \$PROGRAM_NAME host-shell' "$HELPER"
grep -q 'host-validate|host-diagnose|host-repair|host-default|host-shell' "$HELPER"
grep -q '^        host-shell)' "$HELPER"
host_shell_body="$(sed -n '/^run_host_shell()/,/^}/p' "$HELPER")"
[[ -n "$host_shell_body" ]] || { echo 'FAIL: run_host_shell is missing' >&2; exit 1; }
grep -q 'prepare_running_host' <<<"$host_shell_body"
grep -q 'prepare_host_command_guard' <<<"$host_shell_body"
if grep -q 'chroot' <<<"$host_shell_body"; then
    echo 'FAIL: host-shell must not enter a chroot' >&2
    exit 1
fi
# The interactive-prompt handling belongs to the target chroot shell only; the
# running-host shell keeps its existing behavior.
if grep -Fq 'CHROOT_SHELL_PROMPT_REGEX' <<<"$host_shell_body"; then
    echo 'FAIL: host-shell must not use the chroot-shell prompt handling' >&2
    exit 1
fi
grep -q '^mount_target_resolver()' "$HELPER"
grep -q 'mount_target_resolver' "$HELPER"
# Some distributions (for example TUXEDO OS) disable the plain apt/apt-get
# upgrade subcommand and demand full-upgrade instead.  Both shell paths must
# recognize exactly that command shape, retry it once with the equivalent
# transaction and log the mapping; every other command stays untouched and
# dnf/pacman/apk commands are never rewritten.
grep -q '^apt_shell_full_upgrade_command()' "$HELPER"
grep -q '^apt_shell_upgrade_policy_refused()' "$HELPER"
grep -q 'apt_shell_full_upgrade_command' <<<"$chroot_shell_body"
grep -q 'apt_shell_upgrade_policy_refused' <<<"$chroot_shell_body"
grep -q 'apt_shell_full_upgrade_command' <<<"$host_shell_body"
grep -q 'apt_shell_upgrade_policy_refused' <<<"$host_shell_body"
grep -Fq "apt upgrade is disabled by this distribution; running 'apt full-upgrade' instead" "$HELPER" \
    || { echo 'FAIL: the apt-upgrade policy mapping is not logged' >&2; exit 1; }
# Repairs begin with a read-only preflight and only then promote the target to
# read-write.  Network-dependent stages must install the temporary host
# resolver during that promotion, just as the chroot shell path does.
promote_block="$(sed -n '/^promote_target_rw()/,/^}/p' "$HELPER")"
grep -q 'mount_target_resolver' <<<"$promote_block"
awk '
    /mount_special tmpfs none/ { saw_tmpfs = 1 }
    saw_tmpfs && /^[[:space:]]*mount_target_resolver[[:space:]]*$/ { found = 1 }
    END { exit(found ? 0 : 1) }
' <<<"$promote_block"
grep -q '^run_apt_update()' "$HELPER"
grep -q 'Refresh package metadata did not complete' "$HELPER"

# Target chroots receive a private writable /dev tmpfs instead of the
# read-only recovery-host /dev bind.  rpm/dnf5 package payloads that own /dev
# (for example Fedora's filesystem package) must be unpackable, but no write
# may reach the running host's /dev.
grep -q '^populate_writable_dev()' "$HELPER"
grep -q '^        dev-rw)$' "$HELPER"
dev_rw_block="$(sed -n '/^        dev-rw)$/,/^            ;;/p' "$HELPER")"
[[ -n "$dev_rw_block" ]] || { echo 'FAIL: mount_special has no dev-rw case' >&2; exit 1; }
grep -Fq 'mount -t tmpfs -o mode=755,nosuid,size=64M tmpfs "$destination"' <<<"$dev_rw_block" \
    || { echo 'FAIL: the private target /dev is not a writable tmpfs' >&2; exit 1; }
grep -Fq 'populate_writable_dev "$destination"' <<<"$dev_rw_block" \
    || { echo 'FAIL: the private target /dev is not populated' >&2; exit 1; }
if grep -q 'nodev' <<<"$dev_rw_block"; then
    echo 'FAIL: the private target /dev must allow device nodes' >&2
    exit 1
fi
# Four chroot installs: prepare_target rw, promote_target_rw,
# snapshot_mount_promoted_root_rw and the running-host rollback scratch chroot.
[[ "$(grep -c 'mount_special dev-rw none "\$TARGET_ROOT/dev"' "$HELPER")" == 4 ]] \
    || { echo 'FAIL: not every offline chroot installs the private writable /dev' >&2; exit 1; }
if grep -Eq 'mount_special rbind-ro /dev|remount,bind,rw,rec .*dev' "$HELPER"; then
    echo 'FAIL: the target /dev is still the recovery host bind' >&2
    exit 1
fi

# populate_writable_dev copies the host device tree without recursing into the
# nested /dev mounts and always provides the essential device nodes.
dev_populate_root=""
shell_stub_root=""
cleanup_dev_contract()
{
    [[ -n "$dev_populate_root" ]] && rm -rf -- "$dev_populate_root"
    [[ -n "$shell_stub_root" ]] && rm -rf -- "$shell_stub_root"
}
trap cleanup_dev_contract EXIT
dev_populate_root="$(mktemp -d)"
mkdir -p "$dev_populate_root/src/disk/by-uuid" "$dev_populate_root/src/input" \
    "$dev_populate_root/src/pts" "$dev_populate_root/src/shm" "$dev_populate_root/src/mqueue"
printf 'uuid\n' > "$dev_populate_root/src/disk/by-uuid/abc"
ln -s ../../null "$dev_populate_root/src/disk/by-uuid/null-link"
printf 'input\n' > "$dev_populate_root/src/input/event0"
printf 'pts\n' > "$dev_populate_root/src/pts/inside"
printf 'shm\n' > "$dev_populate_root/src/shm/inside"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    MOUNTS=()
    populate_writable_dev "$dev_populate_root/dst" "$dev_populate_root/src" || exit 1
    [[ "$(cat "$dev_populate_root/dst/disk/by-uuid/abc" 2>/dev/null)" == uuid ]] || exit 1
    [[ "$(readlink "$dev_populate_root/dst/disk/by-uuid/null-link" 2>/dev/null)" == ../../null ]] || exit 1
    [[ "$(cat "$dev_populate_root/dst/input/event0" 2>/dev/null)" == input ]] || exit 1
    [[ -d "$dev_populate_root/dst/pts" && ! -e "$dev_populate_root/dst/pts/inside" ]] || exit 1
    [[ -d "$dev_populate_root/dst/shm" && ! -e "$dev_populate_root/dst/shm/inside" ]] || exit 1
    [[ "$(stat -c '%a' "$dev_populate_root/dst/shm" 2>/dev/null)" == 1777 ]] || exit 1
    [[ -L "$dev_populate_root/dst/fd" && "$(readlink "$dev_populate_root/dst/fd" 2>/dev/null)" == /proc/self/fd ]] || exit 1
) || { echo 'FAIL: populate_writable_dev did not populate a private /dev' >&2; exit 1; }

# Behavioural check with stubbed chroot/timeout runners: the command must
# receive stdin from /dev/null and the non-interactive dnf5/apt environment,
# and a prompt abort must be explained with the non-interactive flag in the
# command output and the session log.
shell_stub_root="$(mktemp -d)"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_DIR="$shell_stub_root"
    SESSION_LOG="$shell_stub_root/session.log"
    TARGET_ROOT="$shell_stub_root/target"
    mkdir -p "$TARGET_ROOT"
    prepare_target() { :; }
    # Mirror the production log() closely enough that the failure message is
    # observable in both the request output and the session log.
    log() { printf '%s\n' "$*" | tee -a "$SESSION_LOG" >&2; }
    # Drop timeout's options and duration, then run the wrapped command in this
    # shell process so the redirections under test stay observable.
    timeout()
    {
        while (( $# )) && [[ "$1" == -* || "$1" =~ ^[0-9]+$ ]]; do
            if [[ "$1" == "--kill-after" ]]; then shift 2; else shift; fi
        done
        "$@"
    }
    chroot()
    {
        printf '%s\n' "$@" > "$shell_stub_root/args"
        # Inspect the running command's own fd 0: the helper redirects the
        # whole chroot invocation from /dev/null. /proc/self (not the test
        # script's $$) reflects the redirection and stays meaningful no matter
        # what stdin ctest or CI hands to this script.
        readlink "/proc/self/fd/0" > "$shell_stub_root/stdin" 2>/dev/null || true
        printf 'Is this ok [y/N]: ' >&2
        return 1
    }
    # fail() exits the shell, so keep the request in a nested subshell and
    # absorb its status here.
    ( run_chroot_shell 'dnf update' ) > "$shell_stub_root/output" 2>&1 || true
)
grep -Fxq '/dev/null' "$shell_stub_root/stdin" \
    || { echo 'FAIL: the chroot shell command stdin is not /dev/null' >&2; exit 1; }
grep -Fq 'DNF5_FORCE_INTERACTIVE=0' "$shell_stub_root/args" \
    || { echo 'FAIL: the chroot shell command does not carry DNF5_FORCE_INTERACTIVE=0' >&2; exit 1; }
grep -Fq 'DEBIAN_FRONTEND=noninteractive' "$shell_stub_root/args" \
    || { echo 'FAIL: the chroot shell command does not carry DEBIAN_FRONTEND=noninteractive' >&2; exit 1; }
grep -Fq 'non-interactive flag' "$shell_stub_root/output" \
    || { echo 'FAIL: the interactive-prompt abort is not explained to the caller' >&2; exit 1; }
grep -Fq 'non-interactive flag' "$shell_stub_root/session.log" \
    || { echo 'FAIL: the interactive-prompt hint is missing from the session log' >&2; exit 1; }
grep -Fq 'Chroot shell command failed (exit code 1)' "$shell_stub_root/output" \
    || { echo 'FAIL: the failed chroot shell command is not reported in the output' >&2; exit 1; }
grep -Fq 'Chroot shell command failed (exit code 1)' "$shell_stub_root/session.log" \
    || { echo 'FAIL: the failed chroot shell command is not reported in the session log' >&2; exit 1; }

# Behavioural apt-upgrade retry for the chroot shell: a plain apt/apt-get
# upgrade that fails with the distribution policy message is retried once with
# full-upgrade (the caller's options are preserved) and the mapping line is
# emitted; the original policy failure stays in the transcript.
mkdir -p "$shell_stub_root/apt-retry"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_DIR="$shell_stub_root/apt-retry"
    SESSION_LOG="$shell_stub_root/apt-retry/session.log"
    TARGET_ROOT="$shell_stub_root/apt-retry/target"
    mkdir -p "$TARGET_ROOT"
    prepare_target() { :; }
    log() { printf '%s\n' "$*" | tee -a "$SESSION_LOG" >&2; }
    timeout()
    {
        while (( $# )) && [[ "$1" == -* || "$1" =~ ^[0-9]+$ ]]; do
            if [[ "$1" == "--kill-after" ]]; then shift 2; else shift; fi
        done
        "$@"
    }
    chroot()
    {
        local call
        call="$(cat "$shell_stub_root/apt-retry/calls" 2>/dev/null || echo 0)"
        call=$((call + 1))
        printf '%s\n' "$call" > "$shell_stub_root/apt-retry/calls"
        printf '%s\n' "$*" > "$shell_stub_root/apt-retry/args-$call"
        if (( call == 1 )); then
            printf "E: 'apt upgrade' is disabled on TUXEDO OS! Please use 'apt full-upgrade' instead.\n" >&2
            return 1
        fi
        printf 'mock full-upgrade completed\n'
        return 0
    }
    ( run_chroot_shell 'apt-get -y upgrade' ) > "$shell_stub_root/apt-retry/output" 2>&1 || true
)
apt_retry_root="$shell_stub_root/apt-retry"
[[ "$(cat "$apt_retry_root/calls")" == 2 ]] \
    || { echo 'FAIL: the chroot shell did not retry a policy-disabled apt upgrade exactly once' >&2; exit 1; }
if grep -Fq 'full-upgrade' "$apt_retry_root/args-1"; then
    echo 'FAIL: the first chroot shell attempt was already rewritten' >&2
    exit 1
fi
grep -Fq '/bin/sh -c apt-get -y full-upgrade' "$apt_retry_root/args-2" \
    || { echo 'FAIL: the chroot shell retry did not map upgrade to full-upgrade with the original options' >&2; exit 1; }
grep -Fq "apt upgrade is disabled by this distribution; running 'apt full-upgrade' instead" "$apt_retry_root/output" \
    || { echo 'FAIL: the chroot shell retry mapping is not reported to the caller' >&2; exit 1; }
grep -Fq 'disabled on TUXEDO OS' "$apt_retry_root/output" \
    || { echo 'FAIL: the chroot shell transcript lost the original policy failure' >&2; exit 1; }

# A dnf command and an apt command with an extra subcommand argument are not
# plain upgrades: they run exactly once and are never rewritten even when
# their output carries the same policy text.
for untouched_command in 'dnf update' 'apt-get upgrade extra'; do
    untouched_root="$shell_stub_root/untouched-$(printf '%s' "$untouched_command" | tr ' ' '-')"
    mkdir -p "$untouched_root"
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        SESSION_DIR="$untouched_root"
        SESSION_LOG="$untouched_root/session.log"
        TARGET_ROOT="$untouched_root/target"
        mkdir -p "$TARGET_ROOT"
        prepare_target() { :; }
        log() { printf '%s\n' "$*" | tee -a "$SESSION_LOG" >&2; }
        timeout()
        {
            while (( $# )) && [[ "$1" == -* || "$1" =~ ^[0-9]+$ ]]; do
                if [[ "$1" == "--kill-after" ]]; then shift 2; else shift; fi
            done
            "$@"
        }
        chroot()
        {
            printf '%s\n' "$*" >> "$untouched_root/calls"
            printf "E: 'apt upgrade' is disabled on TUXEDO OS! Please use 'apt full-upgrade' instead.\n" >&2
            return 1
        }
        ( run_chroot_shell "$untouched_command" ) > "$untouched_root/output" 2>&1 || true
    )
    [[ "$(wc -l < "$untouched_root/calls")" == 1 ]] \
        || { echo "FAIL: '$untouched_command' was retried but is not a plain apt upgrade" >&2; exit 1; }
    if grep -Fq 'apt upgrade is disabled by this distribution' "$untouched_root/output"; then
        echo "FAIL: '$untouched_command' received the apt-upgrade mapping" >&2
        exit 1
    fi
done

# Behavioural apt-upgrade retry for the running-host shell: the same policy
# refusal is retried once with full-upgrade on the live host, and a successful
# retry is reported as a pass.
mkdir -p "$shell_stub_root/host-retry"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_DIR="$shell_stub_root/host-retry"
    SESSION_LOG="$shell_stub_root/host-retry/session.log"
    CURRENT_STAGE=""
    prepare_running_host() { :; }
    prepare_host_command_guard() { :; }
    log() { printf '%s\n' "$*" | tee -a "$SESSION_LOG" >&2; }
    run_host_command_isolated()
    {
        local call
        call="$(cat "$shell_stub_root/host-retry/calls" 2>/dev/null || echo 0)"
        call=$((call + 1))
        printf '%s\n' "$call" > "$shell_stub_root/host-retry/calls"
        printf '%s\n' "$*" > "$shell_stub_root/host-retry/args-$call"
        if (( call == 1 )); then
            printf "E: 'apt upgrade' is disabled on TUXEDO OS! Please use 'apt full-upgrade' instead.\n"
            return 1
        fi
        printf 'mock host full-upgrade completed\n'
        return 0
    }
    ( run_host_shell /dev/test-disk /dev/test-root 'apt upgrade' ) \
        > "$shell_stub_root/host-retry/output" 2>&1 || true
)
host_retry_root="$shell_stub_root/host-retry"
[[ "$(cat "$host_retry_root/calls")" == 2 ]] \
    || { echo 'FAIL: the host shell did not retry a policy-disabled apt upgrade exactly once' >&2; exit 1; }
if grep -Fq 'full-upgrade' "$host_retry_root/args-1"; then
    echo 'FAIL: the first host shell attempt was already rewritten' >&2
    exit 1
fi
grep -Fq '/bin/bash -lc apt full-upgrade' "$host_retry_root/args-2" \
    || { echo 'FAIL: the host shell retry did not map upgrade to full-upgrade' >&2; exit 1; }
grep -Fq "apt upgrade is disabled by this distribution; running 'apt full-upgrade' instead" "$host_retry_root/output" \
    || { echo 'FAIL: the host shell retry mapping is not reported to the caller' >&2; exit 1; }
grep -Fq 'mock host full-upgrade completed' "$host_retry_root/output" \
    || { echo 'FAIL: the host shell retry result is missing from the output' >&2; exit 1; }
grep -Fq 'PASS: Running-host shell command' "$host_retry_root/output" \
    || { echo 'FAIL: a successful host shell retry was not reported as a pass' >&2; exit 1; }

# Bash cannot store NUL bytes in variables; the old pattern rejected every
# command. NUL validation belongs to the GUI protocol boundary instead.
if grep -q 'command.*\\$.*0.*unsupported NUL' "$HELPER"; then
    echo "FAIL: helper contains the universal-rejecting Bash NUL check" >&2
    exit 1
fi

echo "PASS: chroot shell helper contract is wired and ordinary commands are accepted."
