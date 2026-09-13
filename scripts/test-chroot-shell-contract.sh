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
