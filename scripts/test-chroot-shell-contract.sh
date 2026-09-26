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
# A2-03: the resolver's realpath/path_within containment validation must run
# BEFORE any mkdir/touch of the destination, and a symlink final component is
# refused instead of being created through.
resolver_block="$(sed -n '/^mount_target_resolver()/,/^}/p' "$HELPER")"
resolver_within_line="$(grep -n 'path_within "\$dest_real"' <<<"$resolver_block" | head -n1 | cut -d: -f1 || true)"
resolver_symlink_line="$(grep -n 'target symlink: \$destination' <<<"$resolver_block" | head -n1 | cut -d: -f1 || true)"
resolver_mkdir_line="$(grep -n 'mkdir -p -- "\$(dirname -- "\$destination")"' <<<"$resolver_block" | head -n1 | cut -d: -f1 || true)"
resolver_touch_line="$(grep -n 'touch -- "\$destination"' <<<"$resolver_block" | head -n1 | cut -d: -f1 || true)"
[[ -n "$resolver_within_line" && -n "$resolver_mkdir_line" && -n "$resolver_touch_line" ]] \
    || { echo 'FAIL: the resolver containment/creation steps are missing' >&2; exit 1; }
[[ "$resolver_within_line" -lt "$resolver_mkdir_line" && "$resolver_within_line" -lt "$resolver_touch_line" ]] \
    || { echo 'FAIL: the resolver validates containment only after creating the destination' >&2; exit 1; }
[[ -n "$resolver_symlink_line" && "$resolver_symlink_line" -lt "$resolver_mkdir_line" ]] \
    || { echo 'FAIL: the resolver does not refuse a symlink final component before creating it' >&2; exit 1; }
# A1-01/A2-02: the cleanup session-log append into the target is contained:
# the resolved /var/log must stay inside the real target root and the log file
# must not be an existing symlink, with an evidence line on refusal.
cleanup_block="$(sed -n '/^cleanup()/,/^}/p' "$HELPER")"
grep -Fq 'session_logdir_real="$(realpath_existing "$TARGET_ROOT/var/log"' <<<"$cleanup_block" \
    || { echo 'FAIL: cleanup does not resolve the target /var/log before the append' >&2; exit 1; }
grep -Fq 'path_within "$session_logdir_real" "$session_root_real"' <<<"$cleanup_block" \
    || { echo 'FAIL: cleanup does not contain the resolved /var/log under the target root' >&2; exit 1; }
grep -Fq '[[ ! -L "$session_target_log" ]]' <<<"$cleanup_block" \
    || { echo 'FAIL: cleanup does not refuse an existing symlinked session log file' >&2; exit 1; }
grep -Fq 'Session log was NOT appended into the target: unsafe target log path.' <<<"$cleanup_block" \
    || { echo 'FAIL: cleanup refusal evidence line is missing' >&2; exit 1; }
# A2-07: prepare_target's rw branch is the single deliberate write-boundary
# crossing point (chroot shell + config-write), so TARGET_WRITE_INTENT must be
# set there before the chroot mounts are installed.
prepare_block="$(sed -n '/^prepare_target()/,/^}/p' "$HELPER")"
grep -Fq 'TARGET_WRITE_INTENT=1' <<<"$prepare_block" \
    || { echo 'FAIL: prepare_target rw does not set TARGET_WRITE_INTENT' >&2; exit 1; }
awk '
    index($0, "if [[ \"$mode\" == \"rw\" ]]; then") { in_rw = 1 }
    in_rw && /TARGET_WRITE_INTENT=1/ { saw_intent = 1 }
    in_rw && /mount_target_boot_entry/ && !saw_intent { exit 1 }
    END { exit(saw_intent ? 0 : 1) }
' <<<"$prepare_block" \
    || { echo 'FAIL: TARGET_WRITE_INTENT is not set before the rw chroot mounts' >&2; exit 1; }
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
grep -Fq "apt upgrade is disabled by this distribution; running '\$retry_command' instead" "$HELPER" \
    || { echo 'FAIL: the apt-upgrade policy mapping is not logged with the actual retry command' >&2; exit 1; }
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

# Evidence-based apt-intent translation: the reviewed shell command is probed
# through apt's own --help exit status (read-only evidence) and translated to
# apt-get only when this apt lacks the action; the mapping line is machine-
# readable and appears in both the request output and the session log.
grep -q '^apt_intent_translate()' "$HELPER"
grep -Fq 'apt "$sub" --help >/dev/null 2>&1' "$HELPER"
grep -Fq "apt intent translated: " "$HELPER"
grep -q 'apt_intent_translate' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not apply the apt-intent translation' >&2; exit 1; }
grep -q 'apt_intent_translate' <<<"$host_shell_body" \
    || { echo 'FAIL: the running-host shell does not apply the apt-intent translation' >&2; exit 1; }

# Generic interactive input channel: the trigger is quiescence-while-alive
# (never prompt-text pattern matching), the wire carries base64 only, the
# answer arrives as ANSWER on the protocol pipe, and every failure closes the
# input and fails closed.  The channel only exists through the privileged
# session broker (BOOT_REPAIR_SESSION_PROTOCOL=1).
grep -q 'SHELL_PROMPT_QUIET_TICKS=2' "$HELPER"
grep -q 'SHELL_PROMPT_TRAILING_BYTES=800' "$HELPER"
grep -q 'SHELL_ANSWER_WINDOW_SECONDS=600' "$HELPER"
grep -q '^shell_interactive_enabled()' "$HELPER"
grep -q '^shell_interactive_ready()' "$HELPER"
grep -q '^shell_command_string()' "$HELPER"
grep -q '^shell_run_interactive()' "$HELPER"
grep -q 'BOOT_REPAIR_SESSION_PROTOCOL=1 BOOT_REPAIR_SESSION_REQUEST=' "$HELPER" \
    || { echo 'FAIL: the broker does not arm the interactive channel env' >&2; exit 1; }
grep -q '^host_command_guard_body()' "$HELPER" \
    || { echo 'FAIL: the firmware guard body is not shared with the interactive host shell' >&2; exit 1; }
grep -Fq "printf 'PROMPT\\t%s\\t%s\\t%s\\n' \"\${BOOT_REPAIR_SESSION_REQUEST:-0}\" \"\$pump_token\" \"\$encoded\"" "$HELPER" \
    || { echo 'FAIL: the interactive runner lost the token-carrying PROMPT wire record' >&2; exit 1; }
grep -Fq "printf 'PUMP\\t%s\\t%s\\n'" "$HELPER" \
    || { echo 'FAIL: the interactive runner lost the PUMP registration record' >&2; exit 1; }
grep -Fq 'pump_token="$(od -An -N8 -tx8 /dev/urandom 2>/dev/null | tr -d '"'"' \n'"'"' || true)"' "$HELPER" \
    || { echo 'FAIL: the pump token is not generated from /dev/urandom into a plain variable' >&2; exit 1; }
grep -Fq "export pump_token" "$HELPER" \
    && { echo 'FAIL: the pump token must never be exported' >&2; exit 1; }
grep -Fq 'IFS=$'"'"'\t'"'"' read -r -t "$SHELL_ANSWER_WINDOW_SECONDS" tag id encoded' "$HELPER" \
    || { echo 'FAIL: the interactive runner lost the bounded ANSWER read' >&2; exit 1; }
grep -Fq '"PROMPT"$'"'"'\t'"'"'"$request_id"$'"'"'\t'"'"'"$pump_token"$'"'"'\t'"'"'*' "$HELPER" \
    || { echo 'FAIL: the broker lost the token-matching PROMPT passthrough for the active request' >&2; exit 1; }
grep -Fq 'printf '"'"'PROMPT\t%s\t%s\n'"'"' "$request_id" "$prompt_payload"' "$HELPER" \
    || { echo 'FAIL: the broker does not re-emit authenticated prompts in the 3-field GUI form' >&2; exit 1; }
grep -Fq 'interactive prompt was cancelled or no answer arrived in time' "$HELPER" \
    || { echo 'FAIL: the no-answer fail-closed message is missing' >&2; exit 1; }
grep -Fq 'interactive prompt was cancelled, so the command cannot continue' "$HELPER" \
    || { echo 'FAIL: the empty-answer cancel message is missing' >&2; exit 1; }
grep -Fq 'Chroot shell command was cancelled at an interactive prompt.' "$HELPER" \
    || { echo 'FAIL: the cancelled chroot shell is not reported' >&2; exit 1; }
grep -Fq 'Running-host shell command was cancelled at an interactive prompt.' "$HELPER" \
    || { echo 'FAIL: the cancelled running-host shell is not reported' >&2; exit 1; }
grep -Fq 'command -v script' "$HELPER" \
    || { echo 'FAIL: the PTY tool probe is missing' >&2; exit 1; }
grep -Fq 'setsid' "$HELPER" \
    || { echo 'FAIL: the process-group leader for the interactive runner is missing' >&2; exit 1; }
grep -q 'shell_interactive_ready && interactive_run=1' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not arm the interactive channel' >&2; exit 1; }
grep -q 'shell_interactive_ready && interactive_run=1' <<<"$host_shell_body" \
    || { echo 'FAIL: the running-host shell does not arm the interactive channel' >&2; exit 1; }
grep -q 'shell_run_interactive "$transcript" "$CHROOT_SHELL_TIMEOUT_SECONDS"' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not call the interactive runner' >&2; exit 1; }
grep -q 'shell_run_interactive "$transcript" 300' <<<"$host_shell_body" \
    || { echo 'FAIL: the running-host shell does not call the interactive runner' >&2; exit 1; }
# The interactive branch drops the non-interactive frontend switches so
# debconf/dpkg questions reach the popup; the plain branch keeps them and
# stays bound to /dev/null.
grep -q 'DEBIAN_FRONTEND=noninteractive' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the plain chroot shell lost its non-interactive frontend' >&2; exit 1; }

# A5-01/A5-03/A5-05/A5-09/A1-09 wiring: the deadline is checked at the top of
# every pump iteration (EPOCHSECONDS with a date fallback), the dead-runner
# drain reads are bounded, the answer echo is redacted through the same filter
# logic, EOF is distinguished from a read timeout, and SIGPIPE is ignored.
interactive_body="$(sed -n '/^shell_run_interactive()/,/^}/p' "$HELPER")"
grep -Fq 'shell_now_epoch()' "$HELPER" \
    || { echo 'FAIL: the epoch clock helper is missing' >&2; exit 1; }
grep -Fq 'if (( now - begin >= deadline )); then' <<<"$interactive_body" \
    || { echo 'FAIL: the interactive pump lost the top-of-loop deadline check' >&2; exit 1; }
grep -Fq 'read -r -n 1 -t 2 -u 8 ch' <<<"$(sed -n '/^shell_pump_drain_dead()/,/^}/p' "$HELPER")" \
    || { echo 'FAIL: the dead-runner drain is not a bounded timed read' >&2; exit 1; }
grep -Fq '+ 5' <<<"$(sed -n '/^shell_pump_drain_dead()/,/^}/p' "$HELPER")" \
    || { echo 'FAIL: the dead-runner drain lost its deadline cap' >&2; exit 1; }
grep -Fq 'shell_expected_echo' <<<"$interactive_body" \
    || { echo 'FAIL: the interactive pump does not compute the expected answer echo' >&2; exit 1; }
grep -Fq 'redact_active=1' <<<"$interactive_body" \
    || { echo 'FAIL: the interactive pump does not arm the answer-echo redaction' >&2; exit 1; }
grep -Fq 'read_rc > 128' <<<"$interactive_body" \
    || { echo 'FAIL: the interactive pump does not distinguish a read timeout from EOF' >&2; exit 1; }
grep -Fq 'never emit a PROMPT' <<<"$interactive_body" \
    || { echo 'FAIL: the EOF path does not document the never-PROMPT rule' >&2; exit 1; }
grep -Fq "trap '' PIPE" "$HELPER" \
    || { echo 'FAIL: the SIGPIPE guards are missing' >&2; exit 1; }
# A1-06/A1-07 wiring: bounded protocol reads, overrun skip and the secret gate.
grep -Fq 'session_read_bounded()' "$HELPER" \
    || { echo 'FAIL: the bounded protocol reader is missing' >&2; exit 1; }
grep -Fq 'session_skip_to_end' "$HELPER" \
    || { echo 'FAIL: the bounded skip-to-END recovery is missing' >&2; exit 1; }
grep -Fq 'session_read_bounded 262144' "$HELPER" \
    || { echo 'FAIL: ARG/SECRET records are not bounded to 262144 encoded bytes' >&2; exit 1; }
grep -Fq 'decoded_total > 4194304' "$HELPER" \
    || { echo 'FAIL: the per-request 4 MiB decoded cap is missing' >&2; exit 1; }
grep -Fq 'A privileged-session secret is only permitted for the unlock command.' "$HELPER" \
    || { echo 'FAIL: the secret verb gate is missing' >&2; exit 1; }
# A5-08: the apt intent probe targets the chroot's own apt for offline targets.
grep -Fq 'RUNNING_HOST_MODE != 1 )) && [[ -n "$TARGET_ROOT" && -x "$TARGET_ROOT/usr/bin/apt" ]]' "$HELPER" \
    || { echo 'FAIL: the apt intent probe does not chroot into the target apt' >&2; exit 1; }
# A5-11: the chroot shell exits 124/125 explicitly instead of a generic 1.
grep -Fq 'exit 125' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not exit 125 on a prompt cancel' >&2; exit 1; }
grep -Fq 'exit 124' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not exit 124 on the deadline' >&2; exit 1; }

# The chroot shell neuters snapper's apt hook (Debian/TUXEDO
# /etc/apt/apt.conf.d/80snapper): its DPkg::Pre/Post-Invoke `snapper
# create|cleanup` runs can only fail on the absent system bus and print
# "Failure (org.freedesktop.DBus.Error.FileNotFound)."  The hook's documented
# kill-switch travels in the command environment (the hook itself is `|| true`,
# so skipping it cannot change the transaction).  The running-host shell keeps
# real host snapshots and must never receive the switch.
grep -Fq 'DISABLE_APT_SNAPSHOT=yes' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not carry the snapper apt-hook kill-switch' >&2; exit 1; }
if grep -Fq 'DISABLE_APT_SNAPSHOT=yes' <<<"$host_shell_body"; then
    echo 'FAIL: the running-host shell must keep real snapper snapshots' >&2
    exit 1
fi

# The environment switch alone cannot win where the target's
# /etc/default/snapper forces DISABLE_APT_SNAPSHOT="no" (the hook sources it
# before testing the variable), so the chroot session setup adds an
# evidence-based guard: it applies only when the target proves BOTH an
# executable /usr/bin/snapper AND an apt config referencing snapper (scanned
# under /etc/apt/apt.conf and /etc/apt/apt.conf.d/ — no hardcoded hook file
# names), it bind-mounts a session-tmp stub (never written to the target disk)
# read-only over the target's /etc/default/snapper, and it records the bind in
# MOUNTS so the EXIT cleanup detaches it in the normal reverse order.
grep -q '^chroot_shell_guard_snapper()' "$HELPER" \
    || { echo 'FAIL: the snapper guard function is missing' >&2; exit 1; }
guard_block="$(sed -n '/^chroot_shell_guard_snapper()/,/^}/p' "$HELPER")"
[[ -n "$guard_block" ]] || { echo 'FAIL: the snapper guard body is missing' >&2; exit 1; }
grep -Fq '[[ -x "$TARGET_ROOT/usr/bin/snapper" ]]' <<<"$guard_block" \
    || { echo 'FAIL: the snapper guard does not probe the target snapper binary' >&2; exit 1; }
grep -Fq 'grep -rqs -- '"'"'snapper'"'"' "$TARGET_ROOT/etc/apt/apt.conf" "$TARGET_ROOT/etc/apt/apt.conf.d"' <<<"$guard_block" \
    || { echo 'FAIL: the snapper guard does not scan the target apt config for snapper references' >&2; exit 1; }
grep -Fq 'snapper-default-guard' <<<"$guard_block" \
    || { echo 'FAIL: the snapper guard stub is not generated in the session directory' >&2; exit 1; }
grep -Fq 'DISABLE_APT_SNAPSHOT="yes"' <<<"$guard_block" \
    || { echo 'FAIL: the snapper guard stub does not declare the kill-switch' >&2; exit 1; }
grep -Fq 'Temporary Boot Bitch guard' <<<"$guard_block" \
    || { echo 'FAIL: the snapper guard stub is not marked as temporary' >&2; exit 1; }
grep -Fq 'mount --bind "$stub" "$target_file"' <<<"$guard_block" \
    || { echo 'FAIL: the snapper guard does not bind the stub over the target default file' >&2; exit 1; }
grep -Fq 'mount -o remount,bind,ro "$target_file"' <<<"$guard_block" \
    || { echo 'FAIL: the snapper guard bind is not read-only' >&2; exit 1; }
grep -Fq 'MOUNTS+=("$target_file")' <<<"$guard_block" \
    || { echo 'FAIL: the snapper guard bind is not recorded for cleanup' >&2; exit 1; }
grep -Fq 'Target has snapper apt hooks; guarding the chroot shell against snapshots (temporary, read-only)' <<<"$guard_block" \
    || { echo 'FAIL: the snapper guard lost its evidence-named session log line' >&2; exit 1; }
# A2-08: a failed guard bind must warn (session log evidence) and continue;
# it must never abort the reviewed command.
grep -Fq 'WARNING: could not bind the read-only snapper guard over $target_file; the chroot shell continues without the snapshot kill-switch.' <<<"$guard_block" \
    || { echo 'FAIL: the snapper guard does not log a WARNING when its bind fails' >&2; exit 1; }
grep -Fq 'chroot_shell_guard_snapper' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not apply the snapper guard' >&2; exit 1; }
if grep -Fq 'chroot_shell_guard_snapper' <<<"$host_shell_body"; then
    echo 'FAIL: the running-host shell must never apply the snapper guard' >&2
    exit 1
fi

# Transcript hygiene: both shell paths run the command with TERM=dumb and
# stream the output through the byte-level transcript filter, which drops
# ANSI CSI/OSC sequences and BEL and normalizes CR/CRLF.
grep -q 'TERM=dumb' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not run with TERM=dumb' >&2; exit 1; }
grep -q 'TERM=dumb' <<<"$host_shell_body" \
    || { echo 'FAIL: the running-host shell does not run with TERM=dumb' >&2; exit 1; }
grep -q '^shell_stream_filter()' "$HELPER"
grep -q '^shell_filter_byte()' "$HELPER"
grep -q 'SHELL_FILTER_ESC' "$HELPER"
grep -q '\[@-~\]' "$HELPER" \
    || { echo 'FAIL: the transcript filter lost the CSI final-byte class' >&2; exit 1; }
grep -Fq 'shell_stream_filter | tee' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the plain chroot shell does not stream through the transcript filter' >&2; exit 1; }
grep -Fq 'shell_stream_filter | tee' <<<"$host_shell_body" \
    || { echo 'FAIL: the plain running-host shell does not stream through the transcript filter' >&2; exit 1; }
grep -q 'shell_pump_input_byte "$ch"' <<<"$(sed -n '/^shell_run_interactive()/,/^}/p' "$HELPER")" \
    || { echo 'FAIL: the interactive pump does not use the redacting transcript filter' >&2; exit 1; }

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
# The private /dev must carry a fresh devpts instance for target-side PTY
# allocation (posix_openpt): gid=5 for the tty group, ptmxmode=000 so only the
# chroot root can use the instance's ptmx, and the copied host /dev/ptmx node
# (5:2, which would address the running host's devpts instance) replaced by a
# link into the private instance.
grep -Fq 'mount -t devpts -o newinstance,gid=5,mode=620,ptmxmode=000 devpts "$destination/pts"' <<<"$dev_rw_block" \
    || { echo 'FAIL: the private target /dev does not mount a fresh devpts instance' >&2; exit 1; }
grep -Fq 'ln -s pts/ptmx "$destination/ptmx"' <<<"$dev_rw_block" \
    || { echo 'FAIL: the target /dev/ptmx is not linked into the private devpts instance' >&2; exit 1; }
# The devpts mount must be recorded in MOUNTS after its /dev tmpfs parent so
# the EXIT cleanup unmounts it in the same reverse-order path as /dev.
mount_special_block="$(sed -n '/^mount_special()/,/^}/p' "$HELPER")"
grep -Fq 'MOUNTS+=("$destination/pts")' <<<"$mount_special_block" \
    || { echo 'FAIL: the private devpts mount is not recorded for cleanup' >&2; exit 1; }
awk '
    /^mount_special\(\)/ { in_fn = 1 }
    in_fn && index($0, "MOUNTS+=(\"$destination\")") { saw_parent = 1 }
    in_fn && saw_parent && index($0, "MOUNTS+=(\"$destination/pts\")") { pts_after_parent = 1 }
    in_fn && /^}/ { exit }
    END { exit(pts_after_parent ? 0 : 1) }
' <<<"$mount_special_block" \
    || { echo 'FAIL: the devpts mount is not recorded after its /dev parent for reverse cleanup' >&2; exit 1; }
cleanup_block="$(sed -n '/^cleanup()/,/^}/p' "$HELPER")"
grep -Fq 'for (( idx=${#MOUNTS[@]}-1; idx>=0; --idx ))' <<<"$cleanup_block" \
    || { echo 'FAIL: the cleanup path does not unmount MOUNTS in reverse order' >&2; exit 1; }
grep -Fq 'umount "$mountpoint" 2>/dev/null || umount -l "$mountpoint" 2>/dev/null || true' <<<"$cleanup_block" \
    || { echo 'FAIL: the cleanup path lost the recorded-mount unmount' >&2; exit 1; }
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
grep -Fq '/bin/sh -c apt-get -y dist-upgrade' "$apt_retry_root/args-2" \
    || { echo 'FAIL: the chroot shell retry did not map apt-get upgrade to dist-upgrade with the original options' >&2; exit 1; }
grep -Fq "apt upgrade is disabled by this distribution; running 'apt-get -y dist-upgrade' instead" "$apt_retry_root/output" \
    || { echo 'FAIL: the chroot shell retry mapping is not reported to the caller with the actual retry command' >&2; exit 1; }
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

# Evidence-based apt-intent translation for the chroot shell: a reviewed
# `apt <action>` command is probed through the target apt's own --help exit
# status (read-only evidence).  When this apt rejects the action, the command
# runs as apt-get with the mapping line logged; when apt accepts the action
# (or the backend is not Debian-family), the command runs verbatim.
apt_intent_root="$shell_stub_root/apt-intent"
mkdir -p "$apt_intent_root"
run_apt_intent_case()
{
    local case_root="$1" command="$2"
    mkdir -p "$case_root/target"
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        SESSION_DIR="$case_root"
        SESSION_LOG="$case_root/session.log"
        TARGET_ROOT="$case_root/target"
        if [[ -n "${APT_INTENT_BACKEND:-}" ]]; then
            TARGET_PACKAGE_MANAGER="$APT_INTENT_BACKEND"
        fi
        prepare_target() { :; }
        log() { printf '%s\n' "$*" | tee -a "$SESSION_LOG" >&2; }
        apt()
        {
            # The helper only ever probes `apt <action> --help` (read-only
            # evidence); everything else never reaches this stub.
            local action="${1:-}"
            if [[ "${2:-}" == "--help" ]]; then
                if grep -Fxq "$action" "$case_root/reject" 2>/dev/null; then
                    return 1
                fi
                return 0
            fi
            return 0
        }
        timeout()
        {
            while (( $# )) && [[ "$1" == -* || "$1" =~ ^[0-9]+$ ]]; do
                if [[ "$1" == "--kill-after" ]]; then shift 2; else shift; fi
            done
            "$@"
        }
        chroot()
        {
            printf '%s\n' "$*" >> "$case_root/calls"
            return 0
        }
        ( run_chroot_shell "$command" ) > "$case_root/output" 2>&1 || true
    )
}

printf 'full-upgrade\n' > "$apt_intent_root/reject"
run_apt_intent_case "$apt_intent_root" 'apt full-upgrade'
grep -Fq '/bin/sh -c apt-get dist-upgrade' "$apt_intent_root/calls" \
    || { echo 'FAIL: a rejecting apt did not translate full-upgrade to apt-get dist-upgrade' >&2; exit 1; }
grep -Fq 'apt intent translated: apt-get dist-upgrade' "$apt_intent_root/output" \
    || { echo 'FAIL: the apt-intent mapping line is missing from the request output' >&2; exit 1; }
grep -Fq 'apt intent translated: apt-get dist-upgrade' "$apt_intent_root/session.log" \
    || { echo 'FAIL: the apt-intent mapping line is missing from the session log' >&2; exit 1; }

printf 'update\n' > "$apt_intent_root/reject"
run_apt_intent_case "$apt_intent_root" 'apt update'
grep -Fq '/bin/sh -c apt-get update' "$apt_intent_root/calls" \
    || { echo 'FAIL: a rejecting apt did not translate apt update to apt-get update' >&2; exit 1; }
grep -Fq 'apt intent translated: apt-get update' "$apt_intent_root/output" \
    || { echo 'FAIL: the apt update mapping line is missing' >&2; exit 1; }

# An apt that supports the action runs the reviewed command verbatim.
: > "$apt_intent_root/reject"
: > "$apt_intent_root/calls"
run_apt_intent_case "$apt_intent_root" 'apt full-upgrade'
grep -Fq '/bin/sh -c apt full-upgrade' "$apt_intent_root/calls" \
    || { echo 'FAIL: a supporting apt command was rewritten' >&2; exit 1; }
if grep -Fq 'apt intent translated' "$apt_intent_root/output"; then
    echo 'FAIL: a supporting apt command emitted a translation line' >&2
    exit 1
fi

# A known non-Debian backend never receives the apt rewrite even when the
# local apt would reject the action.
printf 'update\n' > "$apt_intent_root/reject"
: > "$apt_intent_root/calls"
APT_INTENT_BACKEND=dnf run_apt_intent_case "$apt_intent_root" 'apt update'
grep -Fq '/bin/sh -c apt update' "$apt_intent_root/calls" \
    || { echo 'FAIL: a non-Debian backend rewrote an apt command' >&2; exit 1; }

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
    # This apt accepts every action, so the intent probe never rewrites the
    # reviewed command before the policy retry below.
    apt() { return 0; }
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

# ---------------------------------------------------------------------------
# Generic interactive channel (behavioural).  shell_run_interactive is driven
# directly: a stubbed script(1) runs the command on a plain pipe, the harness
# writes ANSWER records on the runner's stdin (the protocol pipe), and the
# prompt text round-trips through base64 unchanged.
# ---------------------------------------------------------------------------
interactive_root="$shell_stub_root/interactive"
mkdir -p "$interactive_root"

start_interactive_case()
{
    local name="$1" command="$2" window="$3" deadline="${4:-30}"
    local root="$interactive_root/$name"
    mkdir -p "$root"
    : > "$root/output"
    : > "$root/session.log"
    : > "$root/transcript"
    : > "$root/rc"
    rm -f "$root/answers"
    mkfifo "$root/answers"
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        SESSION_DIR="$root"
        SESSION_LOG="$root/session.log"
        BOOT_REPAIR_SESSION_PROTOCOL=1
        BOOT_REPAIR_SESSION_REQUEST=7
        SHELL_ANSWER_WINDOW_SECONDS="$window"
        export BOOT_REPAIR_SESSION_PROTOCOL BOOT_REPAIR_SESSION_REQUEST SHELL_ANSWER_WINDOW_SECONDS
        # The harness replaces the PTY allocator: parse -c (split or inside a
        # combined flag cluster such as -qefc) and run the command on the
        # inherited pipes; setsid passes the stub through unchanged.
        script()
        {
            local cmd="" arg=""
            while (($# > 0)); do
                arg="$1"
                shift
                case "$arg" in
                    -c) cmd="$1"; shift; break ;;
                    -?*c*) cmd="$1"; shift; break ;;
                esac
            done
            /bin/sh -c "$cmd"
        }
        setsid() { "$@"; }
        set +e
        shell_run_interactive "$root/transcript" "$deadline" "$command"
        printf '%s\n' "$?" > "$root/rc"
        exit 0
    ) <"$root/answers" >"$root/output" 2>&1 &
    printf '%s\n' "$!" > "$root/pid"
    # Open the writer end; this unblocks the runner's stdin open.
    exec 14>"$root/answers"
}

wait_prompt_record()
{
    local output="$1" seen="" n=0
    while (( n < 200 )); do
        seen="$(grep '^PROMPT	7	' "$output" 2>/dev/null | head -n1 || true)"
        [[ -n "$seen" ]] && break
        sleep 0.05
        n=$((n + 1))
    done
    [[ -n "$seen" ]] || { echo 'FAIL: no PROMPT record arrived' >&2; return 1; }
    printf '%s\n' "$seen"
}

finish_interactive_case()
{
    local root="$1"
    local pid
    pid="$(cat "$root/pid")"
    wait "$pid" 2>/dev/null || true
    exec 14>&-
}

# Case 1: one answered prompt.  The prompt is a partial line with no newline;
# the PROMPT record carries the PUMP-registered token (A1-05) and the base64
# payload must round-trip byte-for-byte, the answer must reach the command
# verbatim, and the run must exit 0.
start_interactive_case one-prompt \
    "printf 'Continue? [Y/n] '; IFS= read -r a; printf 'got:%s\\n' \"\$a\"" 5
one_root="$interactive_root/one-prompt"
prompt_record="$(wait_prompt_record "$one_root/output")" \
    || { finish_interactive_case "$one_root"; exit 1; }
pump_line="$(grep '^PUMP	7	' "$one_root/output" 2>/dev/null | head -n1 || true)"
pump_token="${pump_line#*$'\t'}"
pump_token="${pump_token#*$'\t'}"
[[ -n "$pump_token" ]] \
    || { echo 'FAIL: the interactive runner never emitted the PUMP registration record' >&2; finish_interactive_case "$one_root"; exit 1; }
IFS=$'\t' read -r ptag pid ptok prompt_b64 <<<"$prompt_record"
[[ "$ptag" == "PROMPT" && "$pid" == "7" && "$ptok" == "$pump_token" ]] \
    || { echo "FAIL: the PROMPT record is not the 4-field token-carrying form: $prompt_record" >&2; finish_interactive_case "$one_root"; exit 1; }
expected_b64="$(printf 'Continue? [Y/n] ' | base64 | tr -d '\n')"
[[ "$prompt_b64" == "$expected_b64" ]] \
    || { echo "FAIL: prompt text did not round-trip through base64: $prompt_b64" >&2; finish_interactive_case "$one_root"; exit 1; }
decoded_prompt="$(printf '%s' "$prompt_b64" | base64 -d 2>/dev/null || true)"
[[ "$decoded_prompt" == 'Continue? [Y/n] ' ]] \
    || { echo 'FAIL: the decoded prompt text is not the original' >&2; finish_interactive_case "$one_root"; exit 1; }
answer_b64="$(printf 'Y' | base64 | tr -d '\n')"
printf 'ANSWER\t7\t%s\n' "$answer_b64" >&14
finish_interactive_case "$one_root"
[[ "$(cat "$one_root/rc")" == "0" ]] \
    || { echo 'FAIL: an answered interactive command did not exit 0' >&2; cat "$one_root/output" >&2; exit 1; }
grep -Fq 'Continue? [Y/n] ' "$one_root/transcript" \
    || { echo 'FAIL: the prompt text is missing from the transcript' >&2; exit 1; }
grep -Fq 'got:Y' "$one_root/transcript" \
    || { echo 'FAIL: the answer did not reach the command' >&2; cat "$one_root/transcript" >&2; exit 1; }

# Case 2: no answer within the window fails closed with rc 125 and the
# actionable message; the runner must not hang.
start_interactive_case no-answer \
    "printf 'Waiting? [y/N] '; IFS= read -r a; printf 'never\\n'" 1
no_answer_root="$interactive_root/no-answer"
wait_prompt_record "$no_answer_root/output" >/dev/null \
    || { finish_interactive_case "$no_answer_root"; exit 1; }
finish_interactive_case "$no_answer_root"
[[ "$(cat "$no_answer_root/rc")" == "125" ]] \
    || { echo 'FAIL: an unanswered prompt did not fail closed with rc 125' >&2; cat "$no_answer_root/output" >&2; exit 1; }
grep -Fq 'interactive prompt was cancelled or no answer arrived in time' "$no_answer_root/output" \
    || { echo 'FAIL: the no-answer fail-closed message is missing' >&2; cat "$no_answer_root/output" >&2; exit 1; }

# Case 3: two prompts in a row round-trip two answers in order.
start_interactive_case two-prompts \
    "printf 'First? [y/N] '; IFS= read -r a; printf 'first=%s\\n' \"\$a\"; printf 'Second (Y/n) '; IFS= read -r b; printf 'second=%s\\n' \"\$b\"" 5
two_root="$interactive_root/two-prompts"
wait_prompt_record "$two_root/output" >/dev/null \
    || { finish_interactive_case "$two_root"; exit 1; }
printf 'ANSWER\t7\t%s\n' "$(printf 'y' | base64 | tr -d '\n')" >&14
# The second prompt must arrive after the first answer.
n=0
while (( n < 200 )); do
    [[ "$(grep -c '^PROMPT	7	' "$two_root/output" 2>/dev/null || true)" -ge 2 ]] && break
    sleep 0.05
    n=$((n + 1))
done
[[ "$(grep -c '^PROMPT	7	' "$two_root/output" 2>/dev/null || true)" -ge 2 ]] \
    || { echo 'FAIL: the second prompt never arrived' >&2; finish_interactive_case "$two_root"; exit 1; }
printf 'ANSWER\t7\t%s\n' "$(printf 'n' | base64 | tr -d '\n')" >&14
finish_interactive_case "$two_root"
[[ "$(cat "$two_root/rc")" == "0" ]] \
    || { echo 'FAIL: a two-prompt command did not exit 0' >&2; cat "$two_root/output" >&2; exit 1; }
grep -Fq 'first=y' "$two_root/transcript" \
    || { echo 'FAIL: the first answer was not delivered in order' >&2; exit 1; }
grep -Fq 'second=n' "$two_root/transcript" \
    || { echo 'FAIL: the second answer was not delivered in order' >&2; exit 1; }

# Case 4: an empty ANSWER payload is a cancel and fails closed with rc 125.
start_interactive_case cancelled "printf 'Stop? [y/N] '; IFS= read -r a" 5
cancel_root="$interactive_root/cancelled"
wait_prompt_record "$cancel_root/output" >/dev/null \
    || { finish_interactive_case "$cancel_root"; exit 1; }
printf 'ANSWER\t7\t\n' >&14
finish_interactive_case "$cancel_root"
[[ "$(cat "$cancel_root/rc")" == "125" ]] \
    || { echo 'FAIL: an empty (cancel) answer did not fail closed with rc 125' >&2; cat "$cancel_root/output" >&2; exit 1; }
grep -Fq 'interactive prompt was cancelled, so the command cannot continue' "$cancel_root/output" \
    || { echo 'FAIL: the cancel message is missing' >&2; cat "$cancel_root/output" >&2; exit 1; }

# Case 5: transcript hygiene.  A command emitting ANSI CSI sequences and CR
# progress updates must leave a transcript with plain one-shot lines: no ESC
# bytes, no CSI parameter remnants ([33m/[0m), no blank lines and no
# duplicates.
start_interactive_case ansi-cr \
    "printf 'Downloading 10%%\rDownloading 50%%\r\033[33mDownloading 100%%\033[0m\r\nDone\n'" 5
ansi_root="$interactive_root/ansi-cr"
finish_interactive_case "$ansi_root"
[[ "$(cat "$ansi_root/rc")" == "0" ]] \
    || { echo 'FAIL: the ANSI/CR fixture command did not exit 0' >&2; cat "$ansi_root/output" >&2; exit 1; }
printf 'Downloading 10%%\nDownloading 50%%\nDownloading 100%%\nDone\n' > "$ansi_root/expected"
cmp -s "$ansi_root/expected" "$ansi_root/transcript" \
    || { echo 'FAIL: the interactive transcript is not the clean one-shot-lines transcript' >&2; cat -A "$ansi_root/transcript" >&2; exit 1; }
# The wire stream carries the one PUMP registration record on top of the clean
# bytes; strip it before the byte-exact comparison.
grep -v '^PUMP	7	' "$ansi_root/output" > "$ansi_root/output-clean" || true
cmp -s "$ansi_root/expected" "$ansi_root/output-clean" \
    || { echo 'FAIL: the interactive wire stream is not clean' >&2; cat -A "$ansi_root/output-clean" >&2; exit 1; }
if grep -Fq $'\x1b' "$ansi_root/transcript"; then
    echo 'FAIL: the transcript still carries an ESC byte' >&2
    exit 1
fi
if grep -Eq '\[33m|\[0m|\[31m' "$ansi_root/transcript"; then
    echo 'FAIL: the transcript still carries ANSI CSI parameter remnants' >&2
    exit 1
fi
if grep -q '^$' "$ansi_root/transcript"; then
    echo 'FAIL: the transcript contains a blank line' >&2
    exit 1
fi

# Case 6: the plain (non-interactive) shell path streams through the same
# transcript filter, so /dev/null-stdin runs produce equally clean evidence.
mkdir -p "$shell_stub_root/plain-filter"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_DIR="$shell_stub_root/plain-filter"
    SESSION_LOG="$shell_stub_root/plain-filter/session.log"
    TARGET_ROOT="$shell_stub_root/plain-filter/target"
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
        printf 'Progress 10%%\rProgress 50%%\r\033[33mProgress 100%%\033[0m\r\nFinished\n'
        return 0
    }
    ( run_chroot_shell 'any command' ) > "$shell_stub_root/plain-filter/output" 2>&1 || true
)
plain_transcript="$shell_stub_root/plain-filter/chroot-shell-output"
printf 'Progress 10%%\nProgress 50%%\nProgress 100%%\nFinished\n' > "$shell_stub_root/plain-filter/expected"
cmp -s "$shell_stub_root/plain-filter/expected" "$plain_transcript" \
    || { echo 'FAIL: the plain shell transcript is not clean' >&2; cat -A "$plain_transcript" >&2; exit 1; }
if grep -Fq $'\x1b' "$plain_transcript"; then
    echo 'FAIL: the plain shell transcript still carries an ESC byte' >&2
    exit 1
fi
if grep -q '^$' "$plain_transcript"; then
    echo 'FAIL: the plain shell transcript contains a blank line' >&2
    exit 1
fi

# Filter behavioural case: apt pads progress lines to the terminal width with
# spaces/tabs and clears them with space runs, all CR-terminated.  A CR-ended
# line must be right-trimmed of trailing spaces/tabs before its newline is
# emitted (a pure-space CR line emits nothing at all), while a line ended by a
# real LF keeps its trailing whitespace.
filter_case_root="$shell_stub_root/filter-cr-trim"
mkdir -p "$filter_case_root"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    # CR progress line padded with spaces -> trimmed; padded with spaces+tabs
    # -> trimmed; a pure-space/tab CR "clear" line -> gone entirely; an LF line
    # with trailing spaces -> kept verbatim; a CRLF line -> collapsed + trimmed.
    printf 'Reading state 10%%   \rReading state 50%% \t \r \t \rDone  \nLast line  \n' \
        | shell_stream_filter
) > "$filter_case_root/output"
printf 'Reading state 10%%\nReading state 50%%\nDone  \nLast line  \n' > "$filter_case_root/expected"
cmp -s "$filter_case_root/expected" "$filter_case_root/output" \
    || { echo 'FAIL: CR-ended lines are not trimmed / LF-ended lines are trimmed' >&2; \
         cat -A "$filter_case_root/output" >&2; exit 1; }
grep -Fxq 'Done  ' "$filter_case_root/output" \
    || { echo 'FAIL: an LF-terminated line lost its trailing spaces' >&2; exit 1; }
if grep -Eq '^[[:space:]]+$' "$filter_case_root/output"; then
    echo 'FAIL: a pure-whitespace CR gap line survived the trim' >&2
    exit 1
fi
if grep -q '^$' "$filter_case_root/output"; then
    echo 'FAIL: the filtered stream contains a blank line' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# A5-10 filter bounds: an unterminated CSI/OSC sequence longer than 2048 bytes
# is dropped and the cap-triggering byte is reprocessed as plain content, so
# one ESC sequence can never swallow the stream.  (Digits are CSI parameter
# bytes and never final bytes, so the sequence cannot terminate by accident.)
# ---------------------------------------------------------------------------
filter_cap_root="$shell_stub_root/filter-seq-cap"
mkdir -p "$filter_cap_root"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    {
        printf '\033['
        i=0
        while (( i < 3000 )); do
            printf '0'
            i=$((i + 1))
        done
        printf 'tail\n'
    } | shell_stream_filter
) > "$filter_cap_root/output"
[[ "$(wc -c < "$filter_cap_root/output")" == $((952 + 5)) ]] \
    || { echo "FAIL: the CSI cap did not resync after 2048 bytes (got $(wc -c < "$filter_cap_root/output") bytes)" >&2; exit 1; }
grep -Fq 'tail' "$filter_cap_root/output" \
    || { echo 'FAIL: the CSI resync lost the stream content after the sequence' >&2; exit 1; }
# OSC bound: BEL-terminated sequences still work and terminate below the cap.
filter_osc_root="$shell_stub_root/filter-osc-bel"
mkdir -p "$filter_osc_root"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    printf '\033]0;title\007ok\n' | shell_stream_filter
) > "$filter_osc_root/output"
grep -Fxq 'ok' "$filter_osc_root/output" \
    || { echo 'FAIL: a BEL-terminated OSC title sequence was not dropped cleanly' >&2; exit 1; }

# ---------------------------------------------------------------------------
# A1-05 (behavioural): the broker forward loop authenticates prompts with the
# per-request pump token.  The PUMP registration line is consumed (never
# forwarded), only token-matching PROMPT records are re-emitted in the 3-field
# GUI form, and every other line -- including forged PROMPT-lookalikes -- goes
# out as an OUT record with ESC/BEL stripped and one trailing CR dropped.
# ---------------------------------------------------------------------------
forward_root="$shell_stub_root/forward"
mkdir -p "$forward_root"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    {
        printf 'PUMP\t7\tTOKEN123\n'
        printf 'PROMPT\t7\tTOKEN123\taGVsbG8=\n'
        printf 'PROMPT\t7\tEVIL\tZm9yZ2Vk\n'
        printf 'PROMPT\t6\tTOKEN123\tZm9yZ2Vk\n'
        printf 'PROMPT\t7\tTOKEN123\textra\tfields\n'
        printf 'forged without registration\n'
        printf 'esc\x1b[31mred\x07\r\n'
    } | session_forward_request 7
) > "$forward_root/output"
grep -Fxq 'PROMPT	7	aGVsbG8=' "$forward_root/output" \
    || { echo 'FAIL: a token-matching prompt was not re-emitted in the 3-field GUI form' >&2; cat "$forward_root/output" >&2; exit 1; }
grep -Fxq 'OUT	7	PROMPT	7	EVIL	Zm9yZ2Vk' "$forward_root/output" \
    || { echo 'FAIL: a forged PROMPT-lookalike with a wrong token was not demoted to an OUT record' >&2; cat "$forward_root/output" >&2; exit 1; }
grep -Fxq 'OUT	7	PROMPT	6	TOKEN123	Zm9yZ2Vk' "$forward_root/output" \
    || { echo 'FAIL: a prompt for another request id was not demoted to an OUT record' >&2; exit 1; }
grep -Fxq 'OUT	7	PROMPT	7	TOKEN123	extra	fields' "$forward_root/output" \
    || { echo 'FAIL: a malformed token-carrying prompt line was not demoted to an OUT record' >&2; exit 1; }
grep -Fxq 'OUT	7	forged without registration' "$forward_root/output" \
    || { echo 'FAIL: an ordinary output line was not forwarded as an OUT record' >&2; exit 1; }
grep -Fxq 'OUT	7	esc[31mred' "$forward_root/output" \
    || { echo 'FAIL: the OUT payload was not stripped of ESC/BEL and the trailing CR' >&2; exit 1; }
if grep -q '^OUT	7	PUMP' "$forward_root/output"; then
    echo 'FAIL: the PUMP registration record leaked into the OUT stream' >&2
    exit 1
fi
# No registration at all (the non-interactive fallback): prompts are never
# forwarded.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    printf 'PROMPT\t7\tTOKEN123\tZm9yZ2Vk\n' | session_forward_request 7
) > "$forward_root/no-reg"
grep -Fxq 'OUT	7	PROMPT	7	TOKEN123	Zm9yZ2Vk' "$forward_root/no-reg" \
    || { echo 'FAIL: an unregistered PROMPT-lookalike was forwarded without a PUMP registration' >&2; exit 1; }

# ---------------------------------------------------------------------------
# A5-01 (behavioural): a continuously-printing command must hit the
# interactive deadline and exit 124 even though the pump never goes quiet.
# ---------------------------------------------------------------------------
start_interactive_case deadline-124 \
    "i=0; while :; do printf 'spin %s\\n' \"\$i\"; i=\$((i+1)); done" 5 3
deadline_root="$interactive_root/deadline-124"
deadline_start=$(date +%s)
finish_interactive_case "$deadline_root"
deadline_elapsed=$(( $(date +%s) - deadline_start ))
[[ "$(cat "$deadline_root/rc")" == "124" ]] \
    || { echo "FAIL: a continuously-printing command did not exit 124 (got $(cat "$deadline_root/rc"))" >&2; exit 1; }
(( deadline_elapsed < 30 )) \
    || { echo 'FAIL: the continuous-output deadline did not fire promptly' >&2; exit 1; }
if grep -q '^PROMPT	7	' "$deadline_root/output"; then
    echo 'FAIL: a continuous-output run emitted a PROMPT record' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# A5-03 (behavioural): after the runner dies the pump drains its FIFO with a
# bounded read.  A writer that stays open (so EOF never arrives) must not hang
# the pump: the drain finishes within its ~5 s deadline.
# ---------------------------------------------------------------------------
drain_root="$interactive_root/drain-bound"
mkdir -p "$drain_root"
: > "$drain_root/output"
: > "$drain_root/session.log"
: > "$drain_root/transcript"
: > "$drain_root/rc"
rm -f "$drain_root/answers"
mkfifo "$drain_root/answers"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_DIR="$drain_root"
    SESSION_LOG="$drain_root/session.log"
    BOOT_REPAIR_SESSION_PROTOCOL=1
    BOOT_REPAIR_SESSION_REQUEST=7
    SHELL_ANSWER_WINDOW_SECONDS=5
    export BOOT_REPAIR_SESSION_PROTOCOL BOOT_REPAIR_SESSION_REQUEST SHELL_ANSWER_WINDOW_SECONDS
    script()
    {
        local cmd="" arg=""
        while (($# > 0)); do
            arg="$1"
            shift
            case "$arg" in
                -c) cmd="$1"; shift; break ;;
                -?*c*) cmd="$1"; shift; break ;;
            esac
        done
        /bin/sh -c "$cmd"
    }
    setsid() { "$@"; }
    set +e
    shell_run_interactive "$drain_root/transcript" 30 "printf 'final bytes\n'; sleep 1"
    printf '%s\n' "$?" > "$drain_root/rc"
    exit 0
) <"$drain_root/answers" >"$drain_root/output" 2>&1 &
printf '%s\n' "$!" > "$drain_root/pid"
exec 14>"$drain_root/answers"
# Hold the runner's output FIFO open so its EOF never arrives; the pump's
# post-mortem drain must still finish (bounded ~5 s) instead of hanging.
drain_fifo_n=0
while (( drain_fifo_n < 100 )) && [[ ! -p "$drain_root/shell-out" ]]; do
    sleep 0.05
    drain_fifo_n=$((drain_fifo_n + 1))
done
[[ -p "$drain_root/shell-out" ]] \
    || { echo 'FAIL: the drain-bound output FIFO never appeared' >&2; exit 1; }
exec 15>"$drain_root/shell-out"
drain_start=$(date +%s)
drain_pid="$(cat "$drain_root/pid")"
wait "$drain_pid" 2>/dev/null || true
exec 14>&-
exec 15>&-
drain_elapsed=$(( $(date +%s) - drain_start ))
[[ "$(cat "$drain_root/rc")" == "0" ]] \
    || { echo "FAIL: the drain-bound run did not exit 0 (got $(cat "$drain_root/rc"))" >&2; exit 1; }
(( drain_elapsed < 12 )) \
    || { echo "FAIL: the post-mortem drain blocked past its ~5 s bound (${drain_elapsed}s)" >&2; exit 1; }
grep -Fq 'final bytes' "$drain_root/transcript" \
    || { echo 'FAIL: the bounded drain lost the runner final bytes' >&2; exit 1; }

# ---------------------------------------------------------------------------
# A5-05 (behavioural): the terminal echo of an answer is computed through the
# same filter logic and redacted from the pump, so the answer never reaches
# the wire, the session log, the transcript or the prompt buffer.  A
# position-0 mismatch (echo off, like a password prompt) stops redacting at
# once; a mid-match divergence suppresses only the rest of that line.
# ---------------------------------------------------------------------------
start_interactive_case redact-echo \
    "printf 'Enter secret: '; IFS= read -r a; printf '%s\\n' \"\$a\"; printf 'done\\n'" 5
redact_root="$interactive_root/redact-echo"
wait_prompt_record "$redact_root/output" >/dev/null \
    || { finish_interactive_case "$redact_root"; exit 1; }
printf 'ANSWER\t7\t%s\n' "$(printf 'hunter2' | base64 | tr -d '\n')" >&14
finish_interactive_case "$redact_root"
[[ "$(cat "$redact_root/rc")" == "0" ]] \
    || { echo "FAIL: the echo-redaction case did not exit 0 (got $(cat "$redact_root/rc"))" >&2; cat "$redact_root/output" >&2; exit 1; }
grep -Fq 'Enter secret: ' "$redact_root/transcript" \
    || { echo 'FAIL: the echo-redaction prompt text is missing from the transcript' >&2; exit 1; }
grep -Fq 'done' "$redact_root/transcript" \
    || { echo 'FAIL: the output after the redacted echo is missing' >&2; exit 1; }
for redact_file in "$redact_root/transcript" "$redact_root/session.log" "$redact_root/output"; do
    if grep -Fq 'hunter2' "$redact_file"; then
        echo "FAIL: the answer leaked into $(basename "$redact_file")" >&2
        cat "$redact_file" >&2
        exit 1
    fi
done
# Echo off (position-0 mismatch): redaction stops immediately, so later real
# output -- including the answer printed by the command itself -- passes.
start_interactive_case redact-echo-off \
    "printf 'Key? '; IFS= read -r a; printf 'PROCESSING\\n'; printf '%s\\n' \"\$a\"" 5
redact_off_root="$interactive_root/redact-echo-off"
wait_prompt_record "$redact_off_root/output" >/dev/null \
    || { finish_interactive_case "$redact_off_root"; exit 1; }
printf 'ANSWER\t7\t%s\n' "$(printf 'hunter2' | base64 | tr -d '\n')" >&14
finish_interactive_case "$redact_off_root"
[[ "$(cat "$redact_off_root/rc")" == "0" ]] \
    || { echo 'FAIL: the echo-off case did not exit 0' >&2; cat "$redact_off_root/output" >&2; exit 1; }
grep -Fq 'PROCESSING' "$redact_off_root/transcript" \
    || { echo 'FAIL: the echo-off case lost the command output before the answer echo position' >&2; exit 1; }
grep -Fq 'hunter2' "$redact_off_root/transcript" \
    || { echo 'FAIL: the echo-off case redacted real command output that only looks like an echo' >&2; exit 1; }
# Mid-match divergence: the mangled echo suppresses only the rest of its line.
start_interactive_case redact-mid \
    "printf 'Pick: '; IFS= read -r a; printf '%sZZ\\nAFTER\\n' \"\$a\"" 5
redact_mid_root="$interactive_root/redact-mid"
wait_prompt_record "$redact_mid_root/output" >/dev/null \
    || { finish_interactive_case "$redact_mid_root"; exit 1; }
printf 'ANSWER\t7\t%s\n' "$(printf 'Y' | base64 | tr -d '\n')" >&14
finish_interactive_case "$redact_mid_root"
[[ "$(cat "$redact_mid_root/rc")" == "0" ]] \
    || { echo 'FAIL: the mid-match redaction case did not exit 0' >&2; cat "$redact_mid_root/output" >&2; exit 1; }
grep -Fq 'Pick: ' "$redact_mid_root/transcript" \
    || { echo 'FAIL: the mid-match redaction prompt text is missing' >&2; exit 1; }
grep -Fq 'AFTER' "$redact_mid_root/transcript" \
    || { echo 'FAIL: the mid-match redaction swallowed output after the mangled echo line' >&2; exit 1; }
if grep -Fq 'ZZ' "$redact_mid_root/transcript"; then
    echo 'FAIL: the mid-match redaction did not suppress the rest of the diverged line' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# A5-09 (behavioural): EOF on the runner output stream means the runner is
# dead -- no PROMPT is emitted, the input is closed and the runner's real
# exit code is reported.  An answer arriving after the runner died takes the
# death path instead of the 125-cancel path.
# ---------------------------------------------------------------------------
start_interactive_case dead-runner-no-prompt "printf 'output then exit\\n'; exit 3" 5
dead_runner_root="$interactive_root/dead-runner-no-prompt"
finish_interactive_case "$dead_runner_root"
[[ "$(cat "$dead_runner_root/rc")" == "3" ]] \
    || { echo "FAIL: a dead runner's real exit code was not reported (got $(cat "$dead_runner_root/rc"))" >&2; cat "$dead_runner_root/output" >&2; exit 1; }
grep -Fq 'output then exit' "$dead_runner_root/transcript" \
    || { echo 'FAIL: the dead-runner final output is missing from the transcript' >&2; exit 1; }
if grep -q '^PROMPT	7	' "$dead_runner_root/output"; then
    echo 'FAIL: a PROMPT record was emitted after the runner had already died' >&2
    exit 1
fi
start_interactive_case dead-before-answer "printf 'Waiting? [y/N] '; sleep 3; exit 7" 5
dead_answer_root="$interactive_root/dead-before-answer"
wait_prompt_record "$dead_answer_root/output" >/dev/null \
    || { finish_interactive_case "$dead_answer_root"; exit 1; }
# The runner dies ~1 s after the prompt is emitted; the answer must then take
# the death path instead of the 125-cancel path.
sleep 2
printf 'ANSWER\t7\t%s\n' "$(printf 'y' | base64 | tr -d '\n')" >&14
finish_interactive_case "$dead_answer_root"
[[ "$(cat "$dead_answer_root/rc")" == "7" ]] \
    || { echo "FAIL: an answer to a dead runner did not take the death path with the real exit code (got $(cat "$dead_answer_root/rc"))" >&2; cat "$dead_answer_root/output" >&2; exit 1; }
if grep -Fq 'interactive prompt was cancelled' "$dead_answer_root/output"; then
    echo 'FAIL: an answer to a dead runner was misreported as a 125-cancel' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# A5-11 (behavioural): the chroot shell propagates the interactive deadline
# (124) and prompt-cancel (125) exit codes instead of collapsing them to 1.
# ---------------------------------------------------------------------------
prop_root="$shell_stub_root/exit-codes"
mkdir -p "$prop_root"
run_prop_case()
{
    local case_root="$1" command="$2"
    mkdir -p "$case_root/target"
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        SESSION_DIR="$case_root"
        SESSION_LOG="$case_root/session.log"
        TARGET_ROOT="$case_root/target"
        BOOT_REPAIR_SESSION_PROTOCOL=1
        export BOOT_REPAIR_SESSION_PROTOCOL
        CHROOT_SHELL_TIMEOUT_SECONDS="$PROP_TIMEOUT"
        SHELL_ANSWER_WINDOW_SECONDS="$PROP_WINDOW"
        export CHROOT_SHELL_TIMEOUT_SECONDS SHELL_ANSWER_WINDOW_SECONDS
        prepare_target() { :; }
        log() { printf '%s\n' "$*" | tee -a "$SESSION_LOG" >&2; }
        script()
        {
            local cmd="" arg=""
            while (($# > 0)); do
                arg="$1"
                shift
                case "$arg" in
                    -c) cmd="$1"; shift; break ;;
                    -?*c*) cmd="$1"; shift; break ;;
                esac
            done
            eval "$cmd"
        }
        setsid() { "$@"; }
        chroot()
        {
            local cmd="" arg=""
            while (($# > 0)); do
                arg="$1"
                shift
                case "$arg" in
                    -c) cmd="$1"; shift; break ;;
                    -?*c*) cmd="$1"; shift; break ;;
                esac
            done
            eval "$cmd"
        }
        timeout()
        {
            while (( $# )) && [[ "$1" == -* || "$1" =~ ^[0-9]+$ ]]; do
                if [[ "$1" == "--kill-after" ]]; then shift 2; else shift; fi
            done
            "$@"
        }
        set +e
        if ( run_chroot_shell "$command" ) > "$case_root/output" 2>&1; then
            printf '0\n' > "$case_root/rc"
        else
            printf '%s\n' "$?" > "$case_root/rc"
        fi
        exit 0
    )
}
PROP_TIMEOUT=3 PROP_WINDOW=5 run_prop_case "$prop_root/timeout" 'i=0; while :; do printf "spin %s\n" "$i"; i=$((i+1)); done'
[[ "$(cat "$prop_root/timeout/rc")" == "124" ]] \
    || { echo "FAIL: the chroot shell collapsed the interactive deadline to $(cat "$prop_root/timeout/rc") instead of 124" >&2; cat "$prop_root/timeout/output" >&2; exit 1; }
grep -Fq 'Chroot shell command timed out after 3 seconds.' "$prop_root/timeout/output" \
    || { echo 'FAIL: the 124 exit lost its timeout report' >&2; exit 1; }
PROP_TIMEOUT=30 PROP_WINDOW=1 run_prop_case "$prop_root/cancel" "printf 'Q? '; IFS= read -r a"
[[ "$(cat "$prop_root/cancel/rc")" == "125" ]] \
    || { echo "FAIL: the chroot shell collapsed the prompt cancel to $(cat "$prop_root/cancel/rc") instead of 125" >&2; cat "$prop_root/cancel/output" >&2; exit 1; }
grep -Fq 'Chroot shell command was cancelled at an interactive prompt.' "$prop_root/cancel/output" \
    || { echo 'FAIL: the 125 exit lost its cancel report' >&2; exit 1; }

# ---------------------------------------------------------------------------
# A5-06 (behavioural): the apt-upgrade retry rewrite is binary-aware --
# apt maps to full-upgrade, apt-get maps to dist-upgrade, and every other
# binary is refused.
# ---------------------------------------------------------------------------
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    [[ "$(apt_shell_full_upgrade_command 'apt upgrade')" == 'apt full-upgrade' ]] || exit 1
    [[ "$(apt_shell_full_upgrade_command 'apt-get upgrade')" == 'apt-get dist-upgrade' ]] || exit 2
    [[ "$(apt_shell_full_upgrade_command 'apt-get -y upgrade')" == 'apt-get -y dist-upgrade' ]] || exit 3
    [[ "$(apt_shell_full_upgrade_command 'sudo apt -y upgrade')" == 'sudo apt -y full-upgrade' ]] || exit 4
    apt_shell_full_upgrade_command 'dnf update' >/dev/null 2>&1 && exit 5
    apt_shell_full_upgrade_command 'aptitude upgrade' >/dev/null 2>&1 && exit 6
    exit 0
) || { echo 'FAIL: the binary-aware apt-upgrade retry matrix broke' >&2; exit 1; }

# ---------------------------------------------------------------------------
# A5-07 (behavioural): shell_command_string uses POSIX single-quote quoting;
# a round-trip through dash -c must reproduce every argument byte-for-byte.
# ---------------------------------------------------------------------------
if grep -Fq -- "printf -v arg '%q'" "$HELPER"; then
    echo "FAIL: shell_command_string still uses bash's %q instead of POSIX quoting" >&2
    exit 1
fi
grep -Fq "quoted+=\"'\\\\''\"" <<<"$(sed -n '/^shell_command_string()/,/^}/p' "$HELPER")" \
    || { echo 'FAIL: shell_command_string lost the POSIX single-quote escaping' >&2; exit 1; }
if command -v dash >/dev/null 2>&1; then
    quote_root="$shell_stub_root/quoting"
    mkdir -p "$quote_root"
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        quote_args=("plain" "with space" "quote'inside" 'dollar$HOME' 'star*' '' 'a"b' "back\\slash" "-dash" "tab	in")
        quote_cmd="$(shell_command_string printf '%s\n' "${quote_args[@]}")"
        quote_expected="$(printf '%s\n' "${quote_args[@]}")"
        quote_got="$(dash -c "$quote_cmd")"
        [[ "$quote_got" == "$quote_expected" ]] || exit 1
        exit 0
    ) || { echo 'FAIL: the POSIX quoting round-trip through dash -c broke' >&2; exit 1; }
fi

# ---------------------------------------------------------------------------
# A1-06 (behavioural): the protocol reader bounds every line (header 512,
# ARG/SECRET 262144 encoded bytes, 4 MiB decoded per request).  An overrun
# consumes the offending line, reports SESSION_ERROR + DONE 2 and skips to
# the request's END so the next request still parses.
# ---------------------------------------------------------------------------
bounded_root="$shell_stub_root/bounded-reader"
mkdir -p "$bounded_root"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    # Unit: exact bound, EOF boundary, overrun consumption and clean EOF.
    printf 'hello\n' | { session_read_bounded 512 && [[ "$REPLY" == hello ]] || exit 1; }
    printf '0123\n' | { session_read_bounded 4 && [[ "$REPLY" == 0123 ]] || exit 1; }
    { printf 'x%.0s' $(seq 1 300); printf '\nNEXT\n'; } | {
        if session_read_bounded 16; then exit 1; fi
        session_read_bounded 16 || exit 1
        [[ "$REPLY" == NEXT ]] || exit 1
    }
    printf '' | { if session_read_bounded 16; then exit 1; fi; }
    exit 0
) || { echo 'FAIL: session_read_bounded does not bound/consume protocol lines' >&2; exit 1; }
# Broker level: an oversized ARG record is consumed, the request errors with
# DONE 2 and the stream recovers at END; a secret on a non-unlock verb is
# refused (A1-07).
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    STATE_ROOT="$bounded_root/state"
    mkdir -p "$STATE_ROOT"
    ensure_state_root() { :; }
    mktemp() { printf '%s\n' "$STATE_ROOT/session-helper.stub"; }
    cp() { :; }
    chown() { :; }
    chmod() { :; }
    {
        printf 'BEGIN\t1\t1\t0\n'
        head -c 307200 /dev/zero | tr '\0' 'A'
        printf '\nEND\t1\n'
        printf 'BEGIN\t2\t1\t1\n'
        printf 'ARG\t2\t%s\n' "$(printf 'shell' | base64 | tr -d '\n')"
        printf 'SECRET\t2\t%s\n' "$(printf 'hunter2' | base64 | tr -d '\n')"
        printf 'END\t2\n'
        printf 'QUIT\n'
    } | session_server
) > "$bounded_root/output" 2>&1
grep -Fq 'SESSION_READY	1' "$bounded_root/output" \
    || { echo 'FAIL: the bounded-reader broker never became ready' >&2; cat "$bounded_root/output" >&2; exit 1; }
grep -Fq 'SESSION_ERROR	1	Privileged-session argument record exceeds the protocol bound.' "$bounded_root/output" \
    || { echo 'FAIL: the oversized ARG record was not reported' >&2; cat "$bounded_root/output" >&2; exit 1; }
grep -Fq 'DONE	1	2' "$bounded_root/output" \
    || { echo 'FAIL: the oversized-ARG request did not finish with DONE 2' >&2; exit 1; }
grep -Fq 'SESSION_ERROR	2	A privileged-session secret is only permitted for the unlock command.' "$bounded_root/output" \
    || { echo 'FAIL: a secret on a non-unlock verb was not refused' >&2; cat "$bounded_root/output" >&2; exit 1; }
grep -Fq 'DONE	2	2' "$bounded_root/output" \
    || { echo 'FAIL: the secret-refused request did not finish with DONE 2' >&2; exit 1; }
if grep -Fq 'hunter2' "$bounded_root/output"; then
    echo 'FAIL: a secret leaked into the broker output' >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# Evidence-based snapper guard (behavioural).  Fake targets replay the hook's
# own logic through stubbed chroot/mount so the guard chain is exercised end
# to end.  With snapper evidence present, the read-only stub bind must make
# the hook observe DISABLE_APT_SNAPSHOT="yes" and skip snapper, and the
# target's real /etc/default/snapper must come back byte-identical.  Without
# the apt-config reference the guard must stay silent and the hook must still
# run (no hardcoded assumptions).
# ---------------------------------------------------------------------------
snapper_case_root="$shell_stub_root/snapper-guard"
mkdir -p "$snapper_case_root/target/etc/apt/apt.conf.d" "$snapper_case_root/target/etc/default" \
    "$snapper_case_root/target/usr/bin"
printf 'DISABLE_APT_SNAPSHOT="no"\n' > "$snapper_case_root/target/etc/default/snapper"
cp -p "$snapper_case_root/target/etc/default/snapper" "$snapper_case_root/expected-default"
printf 'DPkg::Pre-Invoke { "snapper create -d apt -c number -t pre -p || true"; };\n' \
    > "$snapper_case_root/target/etc/apt/apt.conf.d/80snapper"
printf '#!/bin/sh\nexit 0\n' > "$snapper_case_root/target/usr/bin/snapper"
chmod 0755 "$snapper_case_root/target/usr/bin/snapper"
: > "$snapper_case_root/mount-log"
: > "$snapper_case_root/seen-value"
: > "$snapper_case_root/snapper-invocations"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_DIR="$snapper_case_root"
    SESSION_LOG="$snapper_case_root/session.log"
    TARGET_ROOT="$snapper_case_root/target"
    prepare_target() { :; }
    log() { printf '%s\n' "$*" | tee -a "$SESSION_LOG" >&2; }
    timeout()
    {
        while (( $# )) && [[ "$1" == -* || "$1" =~ ^[0-9]+$ ]]; do
            if [[ "$1" == "--kill-after" ]]; then shift 2; else shift; fi
        done
        "$@"
    }
    # Emulate the guard's read-only bind: preserve the real file and swap the
    # stub over it for the lifetime of the session, exactly like a file bind.
    mount()
    {
        if [[ "$1" == "--bind" && "$3" == "$TARGET_ROOT/etc/default/snapper" ]]; then
            printf '%s\n' "$*" >> "$snapper_case_root/mount-log"
            cp -p "$TARGET_ROOT/etc/default/snapper" "$snapper_case_root/real-default-preserved" || return 1
            cp -p "$2" "$TARGET_ROOT/etc/default/snapper" || return 1
        fi
        return 0
    }
    # Replay the hook's own logic inside the fake target.
    chroot()
    {
        if . "$TARGET_ROOT/etc/default/snapper" 2>/dev/null \
            && [[ -x "$TARGET_ROOT/usr/bin/snapper" ]] \
            && [[ "x$DISABLE_APT_SNAPSHOT" != "xyes" ]]; then
            printf 'invoked\n' >> "$snapper_case_root/snapper-invocations"
        fi
        printf 'DISABLE=%s\n' "${DISABLE_APT_SNAPSHOT:-unset}" >> "$snapper_case_root/seen-value"
        return 0
    }
    ( run_chroot_shell 'apt autoremove' ) > "$snapper_case_root/output" 2>&1 || true
)
# The harness swapped the file while emulating the bind; restore the real one
# before asserting the end state.
cp -p "$snapper_case_root/real-default-preserved" "$snapper_case_root/target/etc/default/snapper"
grep -Fq -- '--bind' "$snapper_case_root/mount-log" \
    || { echo 'FAIL: the snapper guard never bound its stub during the session' >&2; cat "$snapper_case_root/mount-log" >&2; exit 1; }
grep -Fq 'DISABLE=yes' "$snapper_case_root/seen-value" \
    || { echo 'FAIL: the hook did not observe the guarded kill-switch value' >&2; cat "$snapper_case_root/seen-value" >&2; exit 1; }
[[ -s "$snapper_case_root/snapper-invocations" ]] \
    && { echo 'FAIL: the guarded hook still invoked snapper' >&2; cat "$snapper_case_root/snapper-invocations" >&2; exit 1; }
cmp -s "$snapper_case_root/expected-default" "$snapper_case_root/target/etc/default/snapper" \
    || { echo 'FAIL: the target /etc/default/snapper content changed' >&2; cat -A "$snapper_case_root/target/etc/default/snapper" >&2; exit 1; }
grep -Fq 'Target has snapper apt hooks; guarding the chroot shell against snapshots (temporary, read-only)' "$snapper_case_root/output" \
    || { echo 'FAIL: the guarded session did not log the guard line' >&2; cat "$snapper_case_root/output" >&2; exit 1; }

# Negative case: the apt config carries no snapper reference, so the guard
# must stay silent (no bind, no log line) and the hook must still run.
snapper_negative_root="$shell_stub_root/snapper-guard-no-evidence"
mkdir -p "$snapper_negative_root/target/etc/apt/apt.conf.d" "$snapper_negative_root/target/etc/default" \
    "$snapper_negative_root/target/usr/bin"
printf 'DISABLE_APT_SNAPSHOT="no"\n' > "$snapper_negative_root/target/etc/default/snapper"
printf 'APT::Get::Assume-Yes "true";\n' > "$snapper_negative_root/target/etc/apt/apt.conf.d/00assume-yes"
printf '#!/bin/sh\nexit 0\n' > "$snapper_negative_root/target/usr/bin/snapper"
chmod 0755 "$snapper_negative_root/target/usr/bin/snapper"
: > "$snapper_negative_root/mount-log"
: > "$snapper_negative_root/seen-value"
: > "$snapper_negative_root/snapper-invocations"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_DIR="$snapper_negative_root"
    SESSION_LOG="$snapper_negative_root/session.log"
    TARGET_ROOT="$snapper_negative_root/target"
    prepare_target() { :; }
    log() { printf '%s\n' "$*" | tee -a "$SESSION_LOG" >&2; }
    timeout()
    {
        while (( $# )) && [[ "$1" == -* || "$1" =~ ^[0-9]+$ ]]; do
            if [[ "$1" == "--kill-after" ]]; then shift 2; else shift; fi
        done
        "$@"
    }
    mount()
    {
        printf '%s\n' "$*" >> "$snapper_negative_root/mount-log"
        return 0
    }
    chroot()
    {
        if . "$TARGET_ROOT/etc/default/snapper" 2>/dev/null \
            && [[ -x "$TARGET_ROOT/usr/bin/snapper" ]] \
            && [[ "x$DISABLE_APT_SNAPSHOT" != "xyes" ]]; then
            printf 'invoked\n' >> "$snapper_negative_root/snapper-invocations"
        fi
        printf 'DISABLE=%s\n' "${DISABLE_APT_SNAPSHOT:-unset}" >> "$snapper_negative_root/seen-value"
        return 0
    }
    ( run_chroot_shell 'apt autoremove' ) > "$snapper_negative_root/output" 2>&1 || true
)
[[ -s "$snapper_negative_root/mount-log" ]] \
    && { echo 'FAIL: the snapper guard applied a bind without apt-config evidence' >&2; cat "$snapper_negative_root/mount-log" >&2; exit 1; }
grep -Fq 'DISABLE=no' "$snapper_negative_root/seen-value" \
    || { echo 'FAIL: the unguarded hook did not observe the target value' >&2; cat "$snapper_negative_root/seen-value" >&2; exit 1; }
grep -Fq 'invoked' "$snapper_negative_root/snapper-invocations" \
    || { echo 'FAIL: the unguarded hook should still run snapper' >&2; exit 1; }
if grep -Fq 'guarding the chroot shell against snapshots' "$snapper_negative_root/output"; then
    echo 'FAIL: the snapper guard logged without evidence' >&2
    cat "$snapper_negative_root/output" >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# A1-01/A2-02 (behavioural): cleanup() appends the session log into the target
# only through a contained path.  A symlinked /var/log or a symlinked log file
# must produce the refusal evidence line and create nothing outside the target;
# the plain-dir positive path still appends.
# ---------------------------------------------------------------------------
cleanup_append_root="$(mktemp -d)"
cleanup_dev_contract_append() { rm -rf -- "$cleanup_append_root"; }
trap 'cleanup_dev_contract; cleanup_dev_contract_append' EXIT

# Positive: plain /var/log directory, missing log file -> append happens.
mkdir -p "$cleanup_append_root/positive/target/var/log"
printf 'positive session evidence\n' > "$cleanup_append_root/positive/session.log"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_WRITE_INTENT=1
    TARGET_ROOT="$cleanup_append_root/positive/target"
    SESSION_LOG="$cleanup_append_root/positive/session.log"
    SESSION_DIR="" SESSION_HELPER_COPY=""
    TEMP_TARGET_PATHS=() MOUNTS=() SESSION_OWNED_MAPPERS=() TEMP_MAPPER_ALIASES=()
    target_path_is_mounted_rw() { return 0; }
    (cleanup) > "$cleanup_append_root/positive/output" 2>&1 || true
)
grep -Fq '===== Boot Bitch session ' "$cleanup_append_root/positive/target/var/log/boot-repair-session.log" \
    || { echo 'FAIL: the positive session-log append never happened' >&2; cat "$cleanup_append_root/positive/output" >&2; exit 1; }
grep -Fq 'positive session evidence' "$cleanup_append_root/positive/target/var/log/boot-repair-session.log" \
    || { echo 'FAIL: the appended session log does not carry the session content' >&2; exit 1; }
if grep -Fq 'was NOT appended' "$cleanup_append_root/positive/output"; then
    echo 'FAIL: the positive session-log append was refused' >&2
    exit 1
fi

# Refusal: /var/log is a symlink out of the target root.
mkdir -p "$cleanup_append_root/symlog/host-var-log" "$cleanup_append_root/symlog/target/var"
ln -s "$cleanup_append_root/symlog/host-var-log" "$cleanup_append_root/symlog/target/var/log"
printf 'symlog session evidence\n' > "$cleanup_append_root/symlog/session.log"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_WRITE_INTENT=1
    TARGET_ROOT="$cleanup_append_root/symlog/target"
    SESSION_LOG="$cleanup_append_root/symlog/session.log"
    SESSION_DIR="" SESSION_HELPER_COPY=""
    TEMP_TARGET_PATHS=() MOUNTS=() SESSION_OWNED_MAPPERS=() TEMP_MAPPER_ALIASES=()
    target_path_is_mounted_rw() { return 0; }
    (cleanup) > "$cleanup_append_root/symlog/output" 2>&1 || true
)
grep -Fq 'Session log was NOT appended into the target: unsafe target log path.' "$cleanup_append_root/symlog/output" \
    || { echo 'FAIL: the symlinked /var/log refusal evidence line is missing' >&2; cat "$cleanup_append_root/symlog/output" >&2; exit 1; }
[[ ! -e "$cleanup_append_root/symlog/host-var-log/boot-repair-session.log" ]] \
    || { echo 'FAIL: the session log was written through the symlinked target /var/log' >&2; exit 1; }

# Refusal: the log file itself exists as a symlink to a host-side file.
mkdir -p "$cleanup_append_root/symfile/target/var/log" "$cleanup_append_root/symfile/host-logdir"
printf 'host original content\n' > "$cleanup_append_root/symfile/host-logdir/boot-repair-session.log"
ln -s "$cleanup_append_root/symfile/host-logdir/boot-repair-session.log" \
    "$cleanup_append_root/symfile/target/var/log/boot-repair-session.log"
printf 'symfile session evidence\n' > "$cleanup_append_root/symfile/session.log"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    TARGET_WRITE_INTENT=1
    TARGET_ROOT="$cleanup_append_root/symfile/target"
    SESSION_LOG="$cleanup_append_root/symfile/session.log"
    SESSION_DIR="" SESSION_HELPER_COPY=""
    TEMP_TARGET_PATHS=() MOUNTS=() SESSION_OWNED_MAPPERS=() TEMP_MAPPER_ALIASES=()
    target_path_is_mounted_rw() { return 0; }
    (cleanup) > "$cleanup_append_root/symfile/output" 2>&1 || true
)
grep -Fq 'Session log was NOT appended into the target: unsafe target log path.' "$cleanup_append_root/symfile/output" \
    || { echo 'FAIL: the symlinked log file refusal evidence line is missing' >&2; cat "$cleanup_append_root/symfile/output" >&2; exit 1; }
[[ "$(cat "$cleanup_append_root/symfile/host-logdir/boot-repair-session.log")" == 'host original content' ]] \
    || { echo 'FAIL: the session log append followed the target symlink to a host file' >&2; exit 1; }

echo "PASS: chroot shell helper contract is wired and ordinary commands are accepted."
