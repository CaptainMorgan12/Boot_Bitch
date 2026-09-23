#!/usr/bin/env bash
# Headless Qt3 test for HelperRunner::authenticateElevation's password pipe.
#
# A fake `sudo` is placed first in PATH:
#   - `sudo -n true` fails, forcing the interactive plain-sudo path;
#   - `sudo -S -p '' -v` exits 0 immediately without reading stdin, exactly
#     like a sudo whose timestamp is already valid.
# The harness must survive the write (EPIPE, never SIGPIPE) and report success.
#
# Runs on the Etch guest (qmake-qt3 + g++ 4.1); the modern host skips it with a
# clear note because Qt3 is not available there. Never builds a package,
# installs anything or touches a disk.
set -euo pipefail

TESTS_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PRO="$TESTS_DIR/auth-pipe-test.pro"
SOURCE="$TESTS_DIR/auth-pipe-test.cpp"

if ! command -v qmake-qt3 >/dev/null 2>&1 || ! command -v make >/dev/null 2>&1; then
    printf 'SKIP: qmake-qt3/make not found; run this on the Etch guest (legacy package validation).\n'
    exit 0
fi
if [[ "$(id -u)" -eq 0 ]]; then
    printf 'SKIP: run as a non-root user; root/direct execution never prompts for sudo.\n'
    exit 0
fi

[[ -f "$PRO" && -f "$SOURCE" ]] || { printf 'FAIL: auth-pipe harness files missing\n' >&2; exit 1; }

TMP="$(mktemp -d "${TMPDIR:-/tmp}/boot-repair-legacy-auth-pipe.XXXXXX")"
trap 'rm -rf "$TMP"' EXIT

FAKE_SUDO_DIR="$TMP/bin"
mkdir -p "$FAKE_SUDO_DIR"
cat > "$FAKE_SUDO_DIR/sudo" <<'EOF'
#!/bin/sh
# Fake sudo: fails the -n probe, then exits 0 for `-S -p '' -v` without
# reading its stdin (a valid-timestamp sudo).
case "$1" in
    -n) exit 1 ;;
    -S) exit 0 ;;
esac
exit 1
EOF
chmod 0755 "$FAKE_SUDO_DIR/sudo"

BUILD="$TMP/build"
mkdir -p "$BUILD"
(
    cd -- "$BUILD" || exit 1
    qmake-qt3 "$PRO"
    make
) || { printf 'FAIL: the Qt3 auth-pipe harness failed to build\n' >&2; exit 1; }

[[ -x "$BUILD/legacy-auth-pipe-test" ]] \
    || { printf 'FAIL: the Qt3 auth-pipe harness binary is missing\n' >&2; exit 1; }

set +e
OUTPUT="$("$BUILD/legacy-auth-pipe-test" "$FAKE_SUDO_DIR" 2>&1)"
RC=$?
set -e
printf '%s\n' "$OUTPUT"
if [[ "$RC" -ne 0 ]]; then
    if [[ "$RC" -eq 141 ]]; then
        printf 'FAIL: the harness died with SIGPIPE (141) writing the password pipe\n' >&2
    else
        printf 'FAIL: the auth-pipe harness exited %s\n' "$RC" >&2
    fi
    exit 1
fi
grep -q 'AUTH-PIPE OK' <<<"$OUTPUT" \
    || { printf 'FAIL: the harness did not report success\n' >&2; exit 1; }
printf 'legacy auth-pipe test: PASS\n'
