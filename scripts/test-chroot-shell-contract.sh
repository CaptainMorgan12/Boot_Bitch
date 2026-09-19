#!/usr/bin/env bash
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
grep -q '^mount_target_resolver()' "$HELPER"
grep -q 'mount_target_resolver' "$HELPER"
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

# Bash cannot store NUL bytes in variables; the old pattern rejected every
# command. NUL validation belongs to the GUI protocol boundary instead.
if grep -q 'command.*\\$.*0.*unsupported NUL' "$HELPER"; then
    echo "FAIL: helper contains the universal-rejecting Bash NUL check" >&2
    exit 1
fi

echo "PASS: chroot shell helper contract is wired and ordinary commands are accepted."
