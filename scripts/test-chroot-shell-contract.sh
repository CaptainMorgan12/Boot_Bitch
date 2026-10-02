#!/usr/bin/env bash
# This contract test sources the helper under test dynamically and sets the
# helper's globals directly so ShellCheck cannot track their use.  The
# per-disk lock helpers assign fds through printf -v (dynamic fdvar), which
# ShellCheck cannot follow either.
# shellcheck disable=SC1090,SC2034,SC2154
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
# A5-14: an exit status of 127 (command not found) must teach the user which
# package tools the TARGET exposes: the hint names the detected package
# manager backend(s) from the backend profile instead of leaving the user
# guessing why the recovery host's tool is absent in the chroot.
grep -q '^shell_command_not_found_hint()' "$HELPER" \
    || { echo 'FAIL: the command-not-found hint helper is missing' >&2; exit 1; }
grep -Fq 'if (( rc == 127 )); then' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not branch on exit status 127' >&2; exit 1; }
grep -Fq 'shell_command_not_found_hint "in the target"' <<<"$chroot_shell_body" \
    || { echo 'FAIL: the chroot shell does not explain a command-not-found with the target backend' >&2; exit 1; }
grep -Fq 'NOTE: command not found $in_scope; $scope_possessive package manager backend is' "$HELPER" \
    || { echo 'FAIL: the command-not-found hint does not name the package backend' >&2; exit 1; }
grep -Fq 'package manager backends are: $(join_comma' "$HELPER" \
    || { echo 'FAIL: the command-not-found hint does not cover multiple detected backends' >&2; exit 1; }

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
# A5-14: the running-host shell names the live host's detected package
# backends on exit 127 as well, with host-scoped wording.
grep -Fq 'shell_command_not_found_hint "on the running host"' <<<"$host_shell_body" \
    || { echo 'FAIL: the running-host shell does not explain a command-not-found with the host backend' >&2; exit 1; }
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
grep -Fq 'msg_log session-log-not-appended' <<<"$cleanup_block" \
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

# A9-01: config-write transports the payload through a private content file
# (--content-file/--content-owner) so file contents never appear in argv or
# /proc cmdline; the deprecated argv form is only a legacy fallback.
grep -q '^config_content_file_path_safe()' "$HELPER" \
    || { echo 'FAIL: config_content_file_path_safe is missing' >&2; exit 1; }
grep -q '^config_content_file_owner_proven()' "$HELPER" \
    || { echo 'FAIL: config_content_file_owner_proven is missing' >&2; exit 1; }
grep -Fq -- '--content-file <path> --content-owner <uid>' "$HELPER" \
    || { echo 'FAIL: the usage text does not document the content-file transport' >&2; exit 1; }
grep -Fq 'config-write accepts either' "$HELPER" \
    || { echo 'FAIL: the config-write dispatcher does not reject a malformed option shape' >&2; exit 1; }
config_write_body="$(awk '/^config_content_file_path_safe\(\)/{f=1} f{print} /^run_target_config\(\)/{g=1} g && /^}/{exit}' "$HELPER")"
grep -Fq 'PKEXEC_UID' <<<"$config_write_body" \
    || { echo 'FAIL: the content-file owner proof does not use PKEXEC_UID' >&2; exit 1; }
grep -Fq 'SUDO_UID' <<<"$config_write_body" \
    || { echo 'FAIL: the content-file owner proof does not use SUDO_UID' >&2; exit 1; }
grep -Fq 'symlink-free regular file' <<<"$config_write_body" \
    || { echo 'FAIL: the content-file path safety refusal is missing' >&2; exit 1; }
grep -Fq 'owner cannot be proven' <<<"$config_write_body" \
    || { echo 'FAIL: the content-file owner refusal is missing' >&2; exit 1; }
grep -Fq 'limited to 1 MiB' <<<"$config_write_body" \
    || { echo 'FAIL: the content-file size cap is not 1 MiB' >&2; exit 1; }
grep -Fq 'cp -- "$content_file" "$tmp"' <<<"$config_write_body" \
    || { echo 'FAIL: the content file is not copied into the atomic temp file' >&2; exit 1; }
grep -Fq 'copy failed verification' <<<"$config_write_body" \
    || { echo 'FAIL: the content copy is not re-verified against the size cap' >&2; exit 1; }
if grep -q 'rm -f -- "\$content_file"' "$HELPER"; then
    echo 'FAIL: the helper must never delete the caller-supplied content file' >&2
    exit 1
fi
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
grep -Fq 'msg_log apt-upgrade-disabled' "$HELPER" \
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
grep -Fq "msg_log apt-intent-translated " "$HELPER"
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
grep -Fq 'msg_log host-shell-cancelled' "$HELPER" \
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
grep -Fq 'msg_log snapper-guard-active' <<<"$guard_block" \
    || { echo 'FAIL: the snapper guard lost its evidence-named session log line' >&2; exit 1; }
# A2-08: a failed guard bind must warn (session log evidence) and continue;
# it must never abort the reviewed command.
grep -Fq 'msg_log snapper-guard-bind-failed' <<<"$guard_block" \
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
dev_filter_root=""
shell_stub_root=""
cleanup_dev_contract()
{
    [[ -n "$dev_populate_root" ]] && rm -rf -- "$dev_populate_root"
    [[ -n "$dev_filter_root" ]] && rm -rf -- "$dev_filter_root"
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

# A1-02/A3-01: when the source is the real /dev, populate_writable_dev
# dispatches to the filtered populate, and the permissive tree copy stays
# available for the contract-test override source (the block above).
grep -q '^populate_writable_dev_filtered()' "$HELPER" \
    || { echo 'FAIL: the filtered private /dev populate function is missing' >&2; exit 1; }
grep -Fq 'populate_writable_dev_filtered "$destination" "$source"' "$HELPER" \
    || { echo 'FAIL: the real-/dev populate is not filtered' >&2; exit 1; }
grep -Fq 'cp -a "$entry" "$destination/$name" 2>/dev/null || true' "$HELPER" \
    || { echo 'FAIL: the permissive populate copy for the contract-test override source is gone' >&2; exit 1; }
grep -Fq "stat -c '%F %t %T'" "$HELPER" \
    || { echo 'FAIL: the filtered populate does not match devices by maj:min via stat' >&2; exit 1; }
grep -Fq "'character special file'|'character device'" "$HELPER" \
    || { echo 'FAIL: the filtered populate does not accept both GNU and BusyBox stat spellings' >&2; exit 1; }
grep -Fq 'disk/by-*/*' "$HELPER" \
    || { echo 'FAIL: the filtered populate does not recreate disk/by-* links' >&2; exit 1; }
grep -Fq 'retains its mknod capability' "$HELPER" \
    || { echo 'FAIL: the filtered populate lost the documented mknod escape-hatch intent' >&2; exit 1; }

# Behavioural filter check with a fake source tree: device identity is
# reported through a stat stub (both the GNU and the BusyBox %F spelling),
# the top-disk resolution is stubbed, and only the selected-disk devices plus
# the essential nodes may reach the destination.
dev_filter_root="$(mktemp -d)"
mkdir -p "$dev_filter_root/src/disk/by-uuid" "$dev_filter_root/src/disk/by-id" \
    "$dev_filter_root/src/mapper" "$dev_filter_root/src/input" \
    "$dev_filter_root/src/pts" "$dev_filter_root/src/shm"
for filter_node in null zero full random urandom tty console sda sda1 sdb sdb1 dm-1 kvm plain-file sock; do
    : > "$dev_filter_root/src/$filter_node"
done
: > "$dev_filter_root/src/mapper/control"
: > "$dev_filter_root/src/mapper/crypt-target"
: > "$dev_filter_root/src/mapper/vg-foreign"
: > "$dev_filter_root/src/input/event0"
mkfifo "$dev_filter_root/src/fifo"
ln -s ../../sda1 "$dev_filter_root/src/disk/by-uuid/target-uuid"
ln -s ../../sdb1 "$dev_filter_root/src/disk/by-uuid/foreign-uuid"
ln -s ../../sda "$dev_filter_root/src/disk/by-id/ata-target"
ln -s ../../sdb "$dev_filter_root/src/disk/by-id/ata-foreign"
# A1-02/A5-13: mirror the real recovery-host mapper geometry (for example
# /dev/mapper/luks-* -> ../dm-N): a mapper symlink to an allowed dm node must
# be recreated with its original link text so grub-probe's
# canonicalize_file_name resolves it inside the private /dev; a mapper
# symlink to a foreign device must never be recreated.
ln -s ../dm-1 "$dev_filter_root/src/mapper/luks-root"
ln -s ../sdb "$dev_filter_root/src/mapper/luks-foreign"
ln -s /proc/kcore "$dev_filter_root/src/core"

run_dev_filter_case()
{
    local style="$1"
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        MOUNTS=()
        TARGET_DISK=/dev/sda
        SESSION_LOG="$dev_filter_root/session-$style.log"
        : > "$SESSION_LOG"
        stat()
        {
            shift 2
            [[ "${1:-}" == -- ]] && shift
            local path="${1:-}" name ftype="" major="" minor=""
            name="$(basename -- "$path")"
            case "$name" in
                null) major=1; minor=3 ;;
                zero) major=1; minor=5 ;;
                full) major=1; minor=7 ;;
                random) major=1; minor=8 ;;
                urandom) major=1; minor=9 ;;
                tty) major=5; minor=0 ;;
                console) major=5; minor=1 ;;
                control) major=a; minor=ec ;;
                kvm) major=a; minor=e8 ;;
                event0) major=d; minor=40 ;;
                sda|sda1|sdb|sdb1|dm-1|crypt-target|vg-foreign) major=8; minor=0 ;;
            esac
            case "$name" in
                null|zero|full|random|urandom|tty|console|control|kvm|event0)
                    [[ "$style" == busybox ]] && ftype='character device' || ftype='character special file' ;;
                sda|sda1|sdb|sdb1|crypt-target|vg-foreign|dm-1)
                    [[ "$style" == busybox ]] && ftype='block device' || ftype='block special file' ;;
                *) ftype='regular file' ;;
            esac
            printf '%s %s %s\n' "$ftype" "$major" "$minor"
        }
        top_disks_for()
        {
            case "$1" in
                */sdb|*/sdb1|*/vg-foreign) printf '%s\n' /dev/sdb ;;
                */sda|*/sda1|*/crypt-target|*/dm-1) printf '%s\n' /dev/sda ;;
                *) return 1 ;;
            esac
        }
        populate_writable_dev_filtered "$dev_filter_root/dst-$style" "$dev_filter_root/src" || exit 1
    ) > "$dev_filter_root/filter-$style.log" 2>&1
}

if ! run_dev_filter_case gnu; then
    echo 'FAIL: the filtered private /dev populate failed (GNU stat spelling)' >&2
    cat "$dev_filter_root/filter-gnu.log" >&2
    exit 1
fi
if ! run_dev_filter_case busybox; then
    echo 'FAIL: the filtered private /dev populate failed (BusyBox stat spelling)' >&2
    cat "$dev_filter_root/filter-busybox.log" >&2
    exit 1
fi
for filter_style in gnu busybox; do
    filter_dst="$dev_filter_root/dst-$filter_style"
    for filter_essential in null zero full random urandom tty console; do
        [[ -e "$filter_dst/$filter_essential" ]] \
            || { echo "FAIL: essential node $filter_essential is missing from the filtered /dev ($filter_style)" >&2; exit 1; }
    done
    [[ -e "$filter_dst/mapper/control" ]] \
        || { echo "FAIL: the device-mapper control node is missing from the filtered /dev ($filter_style)" >&2; exit 1; }
    [[ -e "$filter_dst/sda" && -e "$filter_dst/sda1" ]] \
        || { echo "FAIL: selected-disk block devices are missing from the filtered /dev ($filter_style)" >&2; exit 1; }
    [[ -e "$filter_dst/mapper/crypt-target" ]] \
        || { echo "FAIL: the selected-disk mapper is missing from the filtered /dev ($filter_style)" >&2; exit 1; }
    [[ ! -e "$filter_dst/sdb" && ! -e "$filter_dst/sdb1" ]] \
        || { echo "FAIL: a foreign-disk block device reached the filtered /dev ($filter_style)" >&2; exit 1; }
    [[ ! -e "$filter_dst/mapper/vg-foreign" ]] \
        || { echo "FAIL: a foreign-disk mapper reached the filtered /dev ($filter_style)" >&2; exit 1; }
    [[ -L "$filter_dst/disk/by-uuid/target-uuid" && "$(readlink "$filter_dst/disk/by-uuid/target-uuid")" == ../../sda1 ]] \
        || { echo "FAIL: the allowed by-uuid link was not recreated ($filter_style)" >&2; exit 1; }
    [[ -L "$filter_dst/disk/by-id/ata-target" ]] \
        || { echo "FAIL: the allowed by-id link was not recreated ($filter_style)" >&2; exit 1; }
    [[ ! -L "$filter_dst/disk/by-uuid/foreign-uuid" && ! -e "$filter_dst/disk/by-uuid/foreign-uuid" ]] \
        || { echo "FAIL: a by-uuid link to a foreign device was recreated ($filter_style)" >&2; exit 1; }
    [[ ! -L "$filter_dst/disk/by-id/ata-foreign" && ! -e "$filter_dst/disk/by-id/ata-foreign" ]] \
        || { echo "FAIL: a by-id link to a foreign device was recreated ($filter_style)" >&2; exit 1; }
    # A1-02/A5-13: the mapper alias link to an allowed dm node is recreated
    # with its ORIGINAL link text (../dm-1) and resolves inside the private
    # /dev to the copied node -- exactly what grub-probe's canonicalize step
    # needs -- while a mapper link to a foreign device stays refused.
    [[ -e "$filter_dst/dm-1" ]] \
        || { echo "FAIL: the selected-disk dm node is missing from the filtered /dev ($filter_style)" >&2; exit 1; }
    [[ -L "$filter_dst/mapper/luks-root" && "$(readlink "$filter_dst/mapper/luks-root")" == ../dm-1 ]] \
        || { echo "FAIL: the allowed mapper symlink was not recreated with its original link text ($filter_style)" >&2; exit 1; }
    [[ "$(readlink -f "$filter_dst/mapper/luks-root" 2>/dev/null || true)" == "$dev_filter_root/dst-$filter_style/dm-1" ]] \
        || { echo "FAIL: the recreated mapper symlink does not resolve to the copied dm node inside the private /dev ($filter_style)" >&2; exit 1; }
    [[ ! -L "$filter_dst/mapper/luks-foreign" && ! -e "$filter_dst/mapper/luks-foreign" ]] \
        || { echo "FAIL: a mapper link to a foreign device was recreated ($filter_style)" >&2; exit 1; }
    [[ ! -e "$filter_dst/kvm" && ! -e "$filter_dst/input/event0" ]] \
        || { echo "FAIL: a non-essential character device reached the filtered /dev ($filter_style)" >&2; exit 1; }
    [[ ! -e "$filter_dst/plain-file" && ! -e "$filter_dst/sock" && ! -e "$filter_dst/fifo" && ! -e "$filter_dst/core" && ! -L "$filter_dst/core" ]] \
        || { echo "FAIL: a plain file, socket, fifo or non-device symlink reached the filtered /dev ($filter_style)" >&2; exit 1; }
    [[ -L "$filter_dst/fd" && "$(readlink "$filter_dst/fd")" == /proc/self/fd ]] \
        || { echo "FAIL: the /dev/fd proc link is missing from the filtered /dev ($filter_style)" >&2; exit 1; }
    [[ -d "$filter_dst/pts" && -d "$filter_dst/shm" ]] \
        || { echo "FAIL: the pts/shm directories are missing from the filtered /dev ($filter_style)" >&2; exit 1; }
    [[ "$(stat -c '%a' -- "$filter_dst/shm")" == 1777 ]] \
        || { echo "FAIL: the filtered /dev/shm mode is not 1777 ($filter_style)" >&2; exit 1; }
done

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

# A5-14: a command that does not exist in the target (exit 127) is explained
# with the target's detected package manager backend instead of leaving the
# user guessing which tool exists inside the chroot.
mkdir -p "$shell_stub_root/not-found"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_DIR="$shell_stub_root/not-found"
    SESSION_LOG="$shell_stub_root/not-found/session.log"
    TARGET_ROOT="$shell_stub_root/not-found/target"
    mkdir -p "$TARGET_ROOT"
    prepare_target() { :; }
    profile_target_backends() { TARGET_PACKAGE_MANAGERS=(pacman); }
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
        printf "/bin/sh: line 1: apk: command not found\n" >&2
        return 127
    }
    ( run_chroot_shell 'apk update' ) > "$shell_stub_root/not-found/output" 2>&1 || true
)
grep -Fq "NOTE: command not found in the target; the target's package manager backend is pacman." \
    "$shell_stub_root/not-found/output" \
    || { echo 'FAIL: the chroot shell does not name the target package backend on exit 127' >&2; exit 1; }
grep -Fq "NOTE: command not found in the target; the target's package manager backend is pacman." \
    "$shell_stub_root/not-found/session.log" \
    || { echo 'FAIL: the chroot-shell backend hint is missing from the session log' >&2; exit 1; }
grep -Fq 'Chroot shell command failed (exit code 127)' "$shell_stub_root/not-found/output" \
    || { echo 'FAIL: the exit-127 chroot shell failure is not reported' >&2; exit 1; }

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
grep -Fq "msg:apt-upgrade-disabled|param:apt-get -y dist-upgrade" "$apt_retry_root/output" \
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
    if grep -Fq 'msg:apt-upgrade-disabled' "$untouched_root/output"; then
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
    # Model the TARGET's own apt (an offline target is probed through its
    # chroot's apt, never the host's).  APT_INTENT_NO_TARGET_APT=1 skips the
    # fake apt so a target with no /usr/bin/apt (Debian etch) is exercised.
    if [[ -z "${APT_INTENT_NO_TARGET_APT:-}" ]]; then
        mkdir -p "$case_root/target/usr/bin"
        : > "$case_root/target/usr/bin/apt"
        chmod +x "$case_root/target/usr/bin/apt"
    else
        # The no-apt case reuses a shared case root, so drop any fake apt a
        # previous case created; a target with no /usr/bin/apt is the point.
        rm -f "$case_root/target/usr/bin/apt"
    fi
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
        timeout()
        {
            while (( $# )) && [[ "$1" == -* || "$1" =~ ^[0-9]+$ ]]; do
                if [[ "$1" == "--kill-after" ]]; then shift 2; else shift; fi
            done
            "$@"
        }
        chroot()
        {
            # Record every chroot invocation (the real command and the probe)
            # so the `/bin/sh -c ...` greps below observe the exact command.
            printf '%s\n' "$*" >> "$case_root/calls"
            # Emulate the read-only probe `chroot <root> apt <action> --help`
            # from the reject list; let the real command invocation through.
            if [[ "${2:-}" == "apt" && "${4:-}" == "--help" ]]; then
                if grep -Fxq "${3:-}" "$case_root/reject" 2>/dev/null; then
                    return 1
                fi
                return 0
            fi
            return 0
        }
        ( run_chroot_shell "$command" ) > "$case_root/output" 2>&1 || true
    )
}

printf 'full-upgrade\n' > "$apt_intent_root/reject"
run_apt_intent_case "$apt_intent_root" 'apt full-upgrade'
grep -Fq '/bin/sh -c apt-get dist-upgrade' "$apt_intent_root/calls" \
    || { echo 'FAIL: a rejecting apt did not translate full-upgrade to apt-get dist-upgrade' >&2; exit 1; }
grep -Fq 'msg:apt-intent-translated|param:apt-get dist-upgrade' "$apt_intent_root/output" \
    || { echo 'FAIL: the apt-intent mapping line is missing from the request output' >&2; exit 1; }
grep -Fq 'msg:apt-intent-translated|param:apt-get dist-upgrade' "$apt_intent_root/session.log" \
    || { echo 'FAIL: the apt-intent mapping line is missing from the session log' >&2; exit 1; }

printf 'update\n' > "$apt_intent_root/reject"
run_apt_intent_case "$apt_intent_root" 'apt update'
grep -Fq '/bin/sh -c apt-get update' "$apt_intent_root/calls" \
    || { echo 'FAIL: a rejecting apt did not translate apt update to apt-get update' >&2; exit 1; }
grep -Fq 'msg:apt-intent-translated|param:apt-get update' "$apt_intent_root/output" \
    || { echo 'FAIL: the apt update mapping line is missing' >&2; exit 1; }

# An apt that supports the action runs the reviewed command verbatim.
: > "$apt_intent_root/reject"
: > "$apt_intent_root/calls"
run_apt_intent_case "$apt_intent_root" 'apt full-upgrade'
grep -Fq '/bin/sh -c apt full-upgrade' "$apt_intent_root/calls" \
    || { echo 'FAIL: a supporting apt command was rewritten' >&2; exit 1; }
if grep -Fq 'msg:apt-intent-translated' "$apt_intent_root/output"; then
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

# An offline target with no /usr/bin/apt (e.g. Debian etch) must still
# translate `apt <action>` to apt-get: the host apt probe is host-mode-only,
# so the absent target apt can never fall through to the host's modern apt.
: > "$apt_intent_root/reject"
: > "$apt_intent_root/calls"
APT_INTENT_NO_TARGET_APT=1 run_apt_intent_case "$apt_intent_root" 'apt update'
grep -Fq '/bin/sh -c apt-get update' "$apt_intent_root/calls" \
    || { echo 'FAIL: a target without /usr/bin/apt did not translate apt update to apt-get update' >&2; exit 1; }

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
grep -Fq "msg:apt-upgrade-disabled|param:apt full-upgrade" "$host_retry_root/output" \
    || { echo 'FAIL: the host shell retry mapping is not reported to the caller' >&2; exit 1; }
grep -Fq 'mock host full-upgrade completed' "$host_retry_root/output" \
    || { echo 'FAIL: the host shell retry result is missing from the output' >&2; exit 1; }
grep -Fq 'msg:host-shell-pass' "$host_retry_root/output" \
    || { echo 'FAIL: a successful host shell retry was not reported as a pass' >&2; exit 1; }

# A5-14: the running-host shell names the live host's detected package
# backends on exit 127, in host-scoped wording (plural form when several
# backends are detected).
mkdir -p "$shell_stub_root/host-not-found"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    SESSION_DIR="$shell_stub_root/host-not-found"
    SESSION_LOG="$shell_stub_root/host-not-found/session.log"
    CURRENT_STAGE=""
    prepare_running_host() { :; }
    prepare_host_command_guard() { :; }
    profile_target_backends() { TARGET_PACKAGE_MANAGERS=(apt apk); }
    log() { printf '%s\n' "$*" | tee -a "$SESSION_LOG" >&2; }
    apt() { return 0; }
    run_host_command_isolated() { return 127; }
    ( run_host_shell /dev/test-disk /dev/test-root 'apk update' ) \
        > "$shell_stub_root/host-not-found/output" 2>&1 || true
)
grep -Fq "NOTE: command not found on the running host; the running host's package manager backends are: apt, apk." \
    "$shell_stub_root/host-not-found/output" \
    || { echo 'FAIL: the running-host shell does not name the host package backends on exit 127' >&2; exit 1; }
grep -Fq 'msg:host-shell-fail|param:127' "$shell_stub_root/host-not-found/output" \
    || { echo 'FAIL: the exit-127 host shell failure is not reported' >&2; exit 1; }

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

# Make the helper's bounded tree-teardown available to this harness shell: the
# per-case subshells source the full helper, but the parent shell's EXIT trap
# (re-armed by start_interactive_case below) must be able to reap a runner whose
# case was aborted.  Only these three self-contained functions are sourced, so
# the harness shell never inherits the helper's globals (LC_ALL/PATH/etc.).
source <(sed -n '/^is_process_group_leader()/,/^}/p; /^pid_is_zombie()/,/^}/p; /^terminate_helper_tree()/,/^}/p' "$HELPER")

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
        # The setsid stub stays so `command -v setsid` still resolves to a
        # function (keeping the script stub in the same spawn chain).  Job
        # control (set -m) gives the background runner its own process group,
        # which restores the helper's terminate_helper_tree group-kill path for
        # a continuously-printing command (deadline-124 and friends).
        setsid() { "$@"; }
        set -m
        set +e
        shell_run_interactive "$root/transcript" "$deadline" "$command"
        printf '%s\n' "$?" > "$root/rc"
        exit 0
    ) <"$root/answers" >"$root/output" 2>&1 &
    runner_pid="$!"
    printf '%s\n' "$runner_pid" > "$root/pid"
    # Defense-in-depth: if this harness shell is torn down before
    # finish_interactive_case awaits the runner (a later FAIL, a signal, or a
    # set -e abort), reap the runner's whole tree.  finish_interactive_case
    # restores the fixture-cleanup trap once the runner has exited.
    trap 'terminate_helper_tree "$runner_pid" 2>/dev/null || true; cleanup_dev_contract' EXIT
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
    # The runner has been awaited: drop the runner-reap arm and restore the
    # plain fixture-cleanup trap so a later case never reaps a stale runner.
    trap cleanup_dev_contract EXIT
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
    set -m
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
        # Job control so the interactive runner spawned by run_chroot_shell gets
        # its own process group (the helper's terminate_helper_tree group-kill
        # then reaches a continuously-printing command under the 124 path).
        set -m
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
grep -Fq 'msg:snapper-guard-active' "$snapper_case_root/output" \
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
grep -Fq 'msg:session-log-not-appended' "$cleanup_append_root/symlog/output" \
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
grep -Fq 'msg:session-log-not-appended' "$cleanup_append_root/symfile/output" \
    || { echo 'FAIL: the symlinked log file refusal evidence line is missing' >&2; cat "$cleanup_append_root/symfile/output" >&2; exit 1; }
[[ "$(cat "$cleanup_append_root/symfile/host-logdir/boot-repair-session.log")" == 'host original content' ]] \
    || { echo 'FAIL: the session log append followed the target symlink to a host file' >&2; exit 1; }

# ---------------------------------------------------------------------------
# A5-02 (static): process-tree lifecycle wiring.  The active-children registry
# exists, main() registers every run, cleanup() unregisters and kills the
# registered survivors BEFORE the session-log append and the unmount loop, and
# the group-kill is only ever attempted on a ps-proven process-group leader.
# ---------------------------------------------------------------------------
grep -q '^active_children_file()' "$HELPER" \
    || { echo 'FAIL: the active-children registry path helper is missing' >&2; exit 1; }
grep -q '^register_active_child()' "$HELPER" \
    || { echo 'FAIL: the helper-run registry entry point is missing' >&2; exit 1; }
grep -q '^unregister_active_child()' "$HELPER" \
    || { echo 'FAIL: the helper-run registry removal point is missing' >&2; exit 1; }
grep -q '^kill_registered_children()' "$HELPER" \
    || { echo 'FAIL: the registered-children reaper is missing' >&2; exit 1; }
grep -q '^terminate_helper_tree()' "$HELPER" \
    || { echo 'FAIL: the bounded tree teardown helper is missing' >&2; exit 1; }
grep -q '^is_process_group_leader()' "$HELPER" \
    || { echo 'FAIL: the process-group-leader proof is missing' >&2; exit 1; }
main_block="$(sed -n '/^main()/,/^}/p' "$HELPER")"
grep -Fq 'register_active_child' <<<"$main_block" \
    || { echo 'FAIL: main() does not register the helper run in the active-children registry' >&2; exit 1; }
kill_tree_block="$(sed -n '/^terminate_helper_tree()/,/^}/p' "$HELPER")"
grep -Fq 'kill -KILL -- -"$pid"' <<<"$kill_tree_block" \
    || { echo 'FAIL: the tree teardown lost the final group-KILL' >&2; exit 1; }
grep -Fq 'for child in /proc/[0-9]*' <<<"$kill_tree_block" \
    || { echo 'FAIL: the tree teardown lost the /proc child-group scan' >&2; exit 1; }
grep -Fq 'kill -- -"$child_pid"' <<<"$kill_tree_block" \
    || { echo 'FAIL: the tree teardown does not group-kill proven child leaders' >&2; exit 1; }
leader_block="$(sed -n '/^is_process_group_leader()/,/^}/p' "$HELPER")"
grep -Fq 'ps -o pgid=' <<<"$leader_block" \
    || { echo 'FAIL: the process-group-leader proof is not read from ps' >&2; exit 1; }
cleanup_block="$(sed -n '/^cleanup()/,/^}/p' "$HELPER")"
cleanup_kill_line="$(grep -n '^[[:space:]]*kill_registered_children' <<<"$cleanup_block" | head -n1 | cut -d: -f1 || true)"
cleanup_unregister_line="$(grep -n '^[[:space:]]*unregister_active_child' <<<"$cleanup_block" | head -n1 | cut -d: -f1 || true)"
cleanup_append_line="$(grep -n 'TARGET_WRITE_INTENT == 1' <<<"$cleanup_block" | head -n1 | cut -d: -f1 || true)"
cleanup_unmount_line="$(grep -n 'for (( idx=${#MOUNTS\[@\]}-1' <<<"$cleanup_block" | head -n1 | cut -d: -f1 || true)"
[[ -n "$cleanup_kill_line" && -n "$cleanup_unregister_line" && -n "$cleanup_append_line" && -n "$cleanup_unmount_line" ]] \
    || { echo 'FAIL: cleanup() lost the kill/append/unmount steps' >&2; exit 1; }
[[ "$cleanup_kill_line" -lt "$cleanup_unregister_line" \
    && "$cleanup_unregister_line" -lt "$cleanup_append_line" \
    && "$cleanup_append_line" -lt "$cleanup_unmount_line" ]] \
    || { echo 'FAIL: cleanup() must kill registered children BEFORE the session-log append and the unmount loop' >&2; exit 1; }

# ---------------------------------------------------------------------------
# A5-02 (behavioural): registry kill-on-exit.  A helper run registers its $$
# (one pid per line, 0600) and a finishing run terminates its still-alive
# registered child (TERM, bounded grace <= 5 s, then KILL), reaps an
# init-reparented orphan, but never touches a pid whose live parent is
# another registered helper (a healthy broker) or an unregistered live parent
# (a foreign run).
# ---------------------------------------------------------------------------
registry_root="$(mktemp -d)"
: > "$registry_root/skip-sleep.pid"

# Case 1: a finishing run kills its registered child.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    STATE_ROOT="$registry_root/state"
    mkdir -p "$STATE_ROOT"
    register_active_child "$BASHPID"
    ( register_active_child "$BASHPID"; sleep 300 ) &
    reg_child=$!
    reg_n=0
    while (( reg_n < 200 )) && ! grep -qE "^${reg_child}[[:space:]]" "$STATE_ROOT/active-children" 2>/dev/null; do
        sleep 0.05
        reg_n=$((reg_n + 1))
    done
    grep -qE "^${reg_child}[[:space:]]" "$STATE_ROOT/active-children" \
        || { echo 'FAIL: the child helper run never registered its pid' >&2; kill "$reg_child" 2>/dev/null; exit 1; }
    [[ "$(stat -c '%a' "$STATE_ROOT/active-children" 2>/dev/null || true)" == "600" ]] \
        || { echo 'FAIL: the active-children registry is not mode 0600' >&2; kill "$reg_child" 2>/dev/null; exit 1; }
    kill_registered_children "$BASHPID"
    unregister_active_child "$BASHPID"
    if kill -0 "$reg_child" 2>/dev/null; then
        echo 'FAIL: a finishing run left its registered child alive' >&2
        kill -KILL "$reg_child" 2>/dev/null
        exit 1
    fi
    exit 0
) || exit 1
if [[ -s "$registry_root/state/active-children" ]]; then
    echo 'FAIL: unregister/kill left stale entries in the registry' >&2
    cat "$registry_root/state/active-children" >&2
    exit 1
fi

# Case 2: an orphan whose recorded owner died without cleanup is reaped by
# the next reaper pass (even when the kernel reparented it to a subreaper
# instead of init); a pid whose recorded owner is still alive (a healthy
# helper tree or a live foreign run) is left alone.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    STATE_ROOT="$registry_root/state"
    mkdir -p "$STATE_ROOT"
    # A helper "session" dies without cleanup, leaving a registered child.
    (
        sleep 300 &
        printf '%s %s\n' "$!" "$BASHPID" >> "$STATE_ROOT/active-children"
        printf '%s\n' "$!" > "$registry_root/orphan.pid"
        exit 0
    )
    wait "$!" 2>/dev/null || true
    orphan_pid="$(cat "$registry_root/orphan.pid")"
    [[ -n "$orphan_pid" ]] || { echo 'FAIL: the orphan fixture never started' >&2; exit 1; }
    # A foreign run (recorded owner alive but not a registered helper) is in
    # the registry but must not be touched.  Its intermediate owner stays
    # alive across the reaper pass: a long sleep (instead of a short one)
    # keeps the owner alive even while the reaper's /proc child scan forks
    # awk for every process on a busy host.  Plain background sleeps are used
    # (no subshell), so the owner can be stopped and waited on explicitly
    # after the assertions without an exec-optimized wrapper keeping the wait
    # from returning.
    sleep 300 &
    foreign_parent=$!
    sleep 300 &
    foreign_pid=$!
    printf '%s %s\n' "$foreign_pid" "$foreign_parent" >> "$STATE_ROOT/active-children"
    printf '%s\n' "$foreign_pid" > "$registry_root/foreign.pid"
    foreign_n=0
    while (( foreign_n < 100 )) && [[ ! -s "$registry_root/foreign.pid" ]]; do
        sleep 0.05
        foreign_n=$((foreign_n + 1))
    done
    [[ -n "$foreign_pid" ]] || { echo 'FAIL: the foreign fixture never started' >&2; exit 1; }
    kill_registered_children "$BASHPID"
    if kill -0 "$orphan_pid" 2>/dev/null; then
        echo 'FAIL: the reaper left an orphaned helper child alive' >&2
        kill -KILL "$orphan_pid" 2>/dev/null || true
        kill "$foreign_pid" 2>/dev/null || true
        kill "$foreign_parent" 2>/dev/null || true
        exit 1
    fi
    if ! kill -0 "$foreign_pid" 2>/dev/null; then
        echo 'FAIL: the reaper killed a foreign run whose recorded owner is still alive' >&2
        kill "$foreign_parent" 2>/dev/null || true
        exit 1
    fi
    kill "$foreign_parent" 2>/dev/null || true
    wait "$foreign_parent" 2>/dev/null || true
    kill "$foreign_pid" 2>/dev/null || true
    exit 0
) || exit 1

# Case 3: a third run never kills a pid whose recorded owner is still alive
# and registered (the healthy-broker guard), but the owner's own exit reaps
# it.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    STATE_ROOT="$registry_root/state"
    mkdir -p "$STATE_ROOT"
    register_active_child "$BASHPID"
    ( register_active_child "$BASHPID"; sleep 300 ) &
    owned_child=$!
    own_n=0
    while (( own_n < 200 )) && ! grep -qE "^${owned_child}[[:space:]]" "$STATE_ROOT/active-children" 2>/dev/null; do
        sleep 0.05
        own_n=$((own_n + 1))
    done
    (
        register_active_child "$BASHPID"
        kill_registered_children "$BASHPID"
        unregister_active_child "$BASHPID"
        exit 0
    )
    # The third run above was synchronous.  The guard under test is: a pid
    # whose recorded owner is still alive and registered is never touched, so
    # the owned child must still be alive here (never wait on it: the owned
    # child sleeps for minutes and is reaped by its owner below).
    if ! kill -0 "$owned_child" 2>/dev/null; then
        echo 'FAIL: a third run killed a child whose live registered owner still owns it' >&2
        exit 1
    fi
    kill_registered_children "$BASHPID"
    unregister_active_child "$BASHPID"
    if kill -0 "$owned_child" 2>/dev/null; then
        echo 'FAIL: the owning run did not reap its own registered child at exit' >&2
        kill -KILL "$owned_child" 2>/dev/null
        exit 1
    fi
    exit 0
) || exit 1
rm -rf -- "$registry_root"

# ---------------------------------------------------------------------------
# A1-04/A2-06 (static): per-target-disk mutual exclusion wiring.  The broker
# must canonicalize the request disk, lock every modifying request through a
# non-blocking flock, refuse a busy disk with the named error, release the
# lock at request end and skip read-only verbs.
# ---------------------------------------------------------------------------
grep -q '^disk_lock_file()' "$HELPER" \
    || { echo 'FAIL: the per-disk lock file helper is missing' >&2; exit 1; }
grep -q '^acquire_disk_lock()' "$HELPER" \
    || { echo 'FAIL: the per-disk lock acquisition helper is missing' >&2; exit 1; }
grep -q '^release_disk_lock()' "$HELPER" \
    || { echo 'FAIL: the per-disk lock release helper is missing' >&2; exit 1; }
server_block="$(sed -n '/^session_server()/,/^}/p' "$HELPER")"
grep -Fq 'canonical_block "${op_args[0]:-}"' <<<"$server_block" \
    || { echo 'FAIL: the broker does not canonicalize the request disk before locking' >&2; exit 1; }
grep -Fq 'acquire_disk_lock "$request_disk" request_lock_fd' <<<"$server_block" \
    || { echo 'FAIL: the broker does not take the per-disk lock for modifying requests' >&2; exit 1; }
grep -Fq 'Another Boot Bitch request is already working on $request_disk.' <<<"$server_block" \
    || { echo 'FAIL: the disk-busy refusal does not name the busy disk' >&2; exit 1; }
grep -Fq 'release_disk_lock "$request_lock_fd"' <<<"$server_block" \
    || { echo 'FAIL: the broker does not release the per-disk lock at request end' >&2; exit 1; }
grep -Fq 'list|inspect|plan) request_lock_required=0' <<<"$server_block" \
    || { echo 'FAIL: read-only snapshot verbs do not skip the per-disk lock' >&2; exit 1; }
grep -Fq 'diagnose|validate|config-read|fs-inspect|browse-target|host-diagnose|host-validate|host-fs-inspect' <<<"$server_block" \
    || { echo 'FAIL: read-only request verbs do not skip the per-disk lock' >&2; exit 1; }
grep -Fq 'command -v setsid >/dev/null 2>&1 && dispatch_setsid=setsid' <<<"$server_block" \
    || { echo 'FAIL: the broker does not prepare the setsid dispatch' >&2; exit 1; }
grep -Fq 'dispatch_cmd=(setsid bash "$SESSION_HELPER_COPY" "$command")' <<<"$server_block" \
    || { echo 'FAIL: the broker dispatch is not wrapped in setsid' >&2; exit 1; }
grep -Fq 'rc=${PIPESTATUS[1]}' <<<"$server_block" \
    || { echo 'FAIL: the secret-pipe exit-code indexing changed' >&2; exit 1; }
grep -Fq 'rc=${PIPESTATUS[0]}' <<<"$server_block" \
    || { echo 'FAIL: the plain-pipe exit-code indexing changed' >&2; exit 1; }

# ---------------------------------------------------------------------------
# A1-04/A2-06 (behavioural): flock refusal of a second holder on the same
# disk.  A lock held by one holder refuses a second acquire (rc 2) and the
# broker reports the named SESSION_ERROR/DONE without dispatching; releasing
# the holder lets the next acquire through; read-only verbs skip the lock.
# ---------------------------------------------------------------------------
lock_root="$(mktemp -d)"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    STATE_ROOT="$lock_root/state"
    mkdir -p "$STATE_ROOT"
    # Unit: second holder refused, release lets a re-acquire through.
    exec {lock_test_fd}>"$STATE_ROOT/disk-lock._dev_test-disk"
    flock -n "$lock_test_fd"
    acquire_disk_lock "/dev/test-disk" lock_fd_a
    lock_unit_rc=$?
    [[ "$lock_unit_rc" == "2" ]] \
        || { echo 'FAIL: a second holder acquired the per-disk lock (rc '"$lock_unit_rc"')' >&2; exit 1; }
    flock -u "$lock_test_fd"
    acquire_disk_lock "/dev/test-disk" lock_fd_b \
        || { echo 'FAIL: the released per-disk lock could not be re-acquired' >&2; exit 1; }
    release_disk_lock "$lock_fd_b"
    exit 0
) || { rm -rf -- "$lock_root"; exit 1; }

# Broker level: with the lock held the modifying request is refused by name
# and the dispatch never runs; a read-only verb still runs.
# The redirect opens in the outer shell, so the broker state directory must
# exist before the subshell runs (the subshell cannot create the directory
# the redirect already needs).
mkdir -p "$lock_root/broker"
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    STATE_ROOT="$lock_root/broker"
    mkdir -p "$STATE_ROOT"
    ensure_state_root() { :; }
    mktemp() { printf '%s\n' "$STATE_ROOT/session-helper.stub"; }
    cp() { :; }
    chown() { :; }
    chmod() { :; }
    canonical_block() { printf '%s\n' "$1"; }
    : > "$STATE_ROOT/dispatched"
    setsid() { "$@"; }
    bash()
    {
        if [[ "${3:-}" == repair ]]; then
            printf 'dispatched\n' >> "$STATE_ROOT/dispatched"
        fi
        return 0
    }
    exec {lock_broker_fd}>"$STATE_ROOT/disk-lock._dev_test-disk"
    flock -n "$lock_broker_fd"
    {
        printf 'BEGIN\t1\t3\t0\n'
        printf 'ARG\t1\t%s\n' "$(printf 'repair' | base64 | tr -d '\n')"
        printf 'ARG\t1\t%s\n' "$(printf '/dev/test-disk' | base64 | tr -d '\n')"
        printf 'ARG\t1\t%s\n' "$(printf '/dev/test-root' | base64 | tr -d '\n')"
        printf 'END\t1\n'
        printf 'BEGIN\t2\t3\t0\n'
        printf 'ARG\t2\t%s\n' "$(printf 'diagnose' | base64 | tr -d '\n')"
        printf 'ARG\t2\t%s\n' "$(printf '/dev/test-disk' | base64 | tr -d '\n')"
        printf 'ARG\t2\t%s\n' "$(printf '/dev/test-root' | base64 | tr -d '\n')"
        printf 'END\t2\n'
        printf 'QUIT\n'
    } | session_server
) > "$lock_root/broker/output" 2>&1
grep -Fq 'SESSION_ERROR	1	Another Boot Bitch request is already working on /dev/test-disk.' "$lock_root/broker/output" \
    || { echo 'FAIL: the busy disk was not refused with the named error' >&2; cat "$lock_root/broker/output" >&2; exit 1; }
grep -Fq 'DONE	1	2' "$lock_root/broker/output" \
    || { echo 'FAIL: the refused request did not finish with DONE 2' >&2; exit 1; }
[[ -s "$lock_root/broker/dispatched" ]] \
    && { echo 'FAIL: the modifying request was dispatched while the disk lock was held' >&2; exit 1; }
grep -Fq 'DONE	2	0' "$lock_root/broker/output" \
    || { echo 'FAIL: the read-only verb did not run with the lock held' >&2; cat "$lock_root/broker/output" >&2; exit 1; }
rm -rf -- "$lock_root"

# ---------------------------------------------------------------------------
# A5-02 (pump): the interactive pump teardown kills the runner's whole group.
# Static: the rc 124/125 teardown and the EOF death path route through the
# shared tree teardown, which group-kills proven leaders and scans /proc for
# child groups.
# ---------------------------------------------------------------------------
interactive_body="$(sed -n '/^shell_run_interactive()/,/^}/p' "$HELPER")"
grep -Fq 'terminate_helper_tree "$runner_pid"' <<<"$interactive_body" \
    || { echo 'FAIL: the interactive pump teardown does not use the shared tree teardown' >&2; exit 1; }
[[ "$(grep -c 'terminate_helper_tree "$runner_pid"' <<<"$interactive_body")" -ge 2 ]] \
    || { echo 'FAIL: the interactive pump does not tear the tree down on both the EOF and the 124/125 paths' >&2; exit 1; }
grep -Fq 'shell_runner_is_zombie' <<<"$interactive_body" \
    || { echo 'FAIL: the interactive pump teardown lost the zombie guard' >&2; exit 1; }

# ---------------------------------------------------------------------------
# A5-02 (pump, behavioural): a runner child that ignores TERM
# (trap '' TERM; sleep 600) must still exit within the bounded grace after a
# prompt cancel -- the runner is spawned through a real setsid so the
# group-kill path is exercised end to end.  script(1) is deliberately kept
# out of the fixture PATH so the /bin/sh runner is the proven group leader.
# ---------------------------------------------------------------------------
if command -v setsid >/dev/null 2>&1 && command -v mkfifo >/dev/null 2>&1; then
    groupkill_root="$shell_stub_root/group-kill"
    mkdir -p "$groupkill_root/bin"
    for groupkill_tool in setsid mkfifo base64 tr od tail sleep date ps awk; do
        ln -s "$(command -v "$groupkill_tool")" "$groupkill_root/bin/$groupkill_tool" 2>/dev/null || true
    done
    rm -f "$groupkill_root/bin/script"
    : > "$groupkill_root/output"
    : > "$groupkill_root/session.log"
    : > "$groupkill_root/transcript"
    : > "$groupkill_root/rc"
    : > "$groupkill_root/sleep.pid"
    rm -f "$groupkill_root/answers"
    mkfifo "$groupkill_root/answers"
    (
        source <(sed '/^main "\$@"/d' "$HELPER")
        trap - EXIT INT TERM HUP
        PATH="$groupkill_root/bin"
        export PATH
        SESSION_DIR="$groupkill_root"
        SESSION_LOG="$groupkill_root/session.log"
        BOOT_REPAIR_SESSION_PROTOCOL=1
        BOOT_REPAIR_SESSION_REQUEST=7
        SHELL_ANSWER_WINDOW_SECONDS=60
        export BOOT_REPAIR_SESSION_PROTOCOL BOOT_REPAIR_SESSION_REQUEST SHELL_ANSWER_WINDOW_SECONDS
        set +e
        shell_run_interactive "$groupkill_root/transcript" 30 \
            "printf 'Continue? [y/N] '; trap '' TERM; /bin/sleep 600 & echo \$! > '$groupkill_root/sleep.pid'; wait"
        printf '%s\n' "$?" > "$groupkill_root/rc"
        exit 0
    ) <"$groupkill_root/answers" >"$groupkill_root/output" 2>&1 &
    printf '%s\n' "$!" > "$groupkill_root/pid"
    exec 16>"$groupkill_root/answers"
    groupkill_n=0
    while (( groupkill_n < 200 )) && ! grep -q '^PROMPT	7	' "$groupkill_root/output" 2>/dev/null; do
        sleep 0.05
        groupkill_n=$((groupkill_n + 1))
    done
    grep -q '^PROMPT	7	' "$groupkill_root/output" 2>/dev/null \
        || { echo 'FAIL: the group-kill fixture never emitted its prompt' >&2; cat "$groupkill_root/output" >&2; exec 16>&-; exit 1; }
    cancel_epoch=$(date +%s)
    printf 'ANSWER\t7\t\n' >&16
    groupkill_pid="$(cat "$groupkill_root/pid")"
    wait "$groupkill_pid" 2>/dev/null || true
    exec 16>&-
    groupkill_sleep_pid="$(cat "$groupkill_root/sleep.pid" 2>/dev/null || true)"
    [[ -n "$groupkill_sleep_pid" ]] \
        || { echo 'FAIL: the group-kill fixture never recorded its child pid' >&2; exit 1; }
    if kill -0 "$groupkill_sleep_pid" 2>/dev/null; then
        echo 'FAIL: the TERM-ignoring sleep 600 child survived the cancel teardown' >&2
        kill -KILL "$groupkill_sleep_pid" 2>/dev/null || true
        exit 1
    fi
    groupkill_elapsed=$(( $(date +%s) - cancel_epoch ))
    # This bound guards against an unbounded teardown hang, not against precise
    # timing.  The product's grace stays bounded at 5 s inside
    # terminate_helper_tree; the wall-clock margin here only has to stay well
    # above that while tolerating a heavily loaded CI/dev host (load average
    # 100+, several VMs), where the same correctly-bounded teardown has been
    # observed taking 19-24 s.  A run whose runner deadline (30 s above) fires
    # first still fails loudly on the rc 125 check below, so the wider bound
    # cannot mask a real hang.
    (( groupkill_elapsed < 45 )) \
        || { echo "FAIL: the TERM-ignoring child exited only after ${groupkill_elapsed}s (bound ~45s)" >&2; exit 1; }
    [[ "$(cat "$groupkill_root/rc")" == "125" ]] \
        || { echo "FAIL: the group-kill cancel did not fail closed with 125 (got $(cat "$groupkill_root/rc"))" >&2; cat "$groupkill_root/output" >&2; exit 1; }
    grep -Fq 'interactive prompt was cancelled, so the command cannot continue' "$groupkill_root/output" \
        || { echo 'FAIL: the group-kill cancel lost its fail-closed message' >&2; exit 1; }
fi

# ---------------------------------------------------------------------------
# A5-02 (host shell): the non-interactive running-host runner must mirror the
# chroot-shell bounded runtime (timeout --foreground --kill-after=10).
# ---------------------------------------------------------------------------
grep -Fq 'timeout --foreground --kill-after=10 300' <<<"$host_shell_body" \
    || { echo 'FAIL: the host-shell non-interactive runner lost --kill-after=10' >&2; exit 1; }

# ---------------------------------------------------------------------------
# A9-01 (config-write content-file transport, behavioural): run_target_config
# accepts the --content-file/--content-owner form and writes the target file
# atomically byte-identically; a wrong/unprovable owner, an oversized file and
# a symlinked content file are all refused without deleting the content file;
# the deprecated argv form still works; the content never appears in the
# output or the session log.
# ---------------------------------------------------------------------------
cfg_root="$(mktemp -d)"
(
    source <(sed '/^main "$@"$/d' "$HELPER")
    trap - EXIT INT TERM HUP
    unset PKEXEC_UID SUDO_UID
    # The contract test never mounts a real target: stub the two mount entry
    # points run_target_config depends on.
    prepare_target() { :; }
    maybe_mount_target_path() { :; }
    cfg_uid="$(id -u)"
    SESSION_DIR="$cfg_root/session"
    mkdir -p "$SESSION_DIR"
    SESSION_LOG="$cfg_root/session.log"
    : > "$SESSION_LOG"
    TARGET_ROOT="$cfg_root/target"
    mkdir -p "$TARGET_ROOT/etc"
    printf 'original-fstab\n' > "$TARGET_ROOT/etc/fstab"
    chmod 0640 "$TARGET_ROOT/etc/fstab"
    printf 'original-crypttab\n' > "$TARGET_ROOT/etc/crypttab"
    chmod 0640 "$TARGET_ROOT/etc/crypttab"

    # Fixture content file: private, regular, caller-owned, byte-exact payload.
    content_file="$cfg_root/content"
    printf 'crypttab-secret-marker-90125\nsecond line\twith tab and  spaces\n' > "$content_file"
    chmod 0600 "$content_file"

    # (1) Content-file transport accepted: the payload round-trips
    # byte-identically into the target, mode/owner are preserved atomically,
    # the helper never deletes the content file, and the content never
    # appears in the output or the session log.
    out="$(PKEXEC_UID="$cfg_uid" run_target_config write crypttab --content-file "$content_file" --content-owner "$cfg_uid")"
    cmp -s "$content_file" "$TARGET_ROOT/etc/crypttab" \
        || { echo 'FAIL: the content-file transport did not write byte-identical content' >&2; exit 1; }
    [[ "$(stat -c '%a' -- "$TARGET_ROOT/etc/crypttab")" == "640" ]] \
        || { echo 'FAIL: the content-file write did not preserve the target mode' >&2; exit 1; }
    [[ "$(stat -c '%u:%g' -- "$TARGET_ROOT/etc/crypttab")" == "$cfg_uid:$(id -g)" ]] \
        || { echo 'FAIL: the content-file write did not preserve the target owner' >&2; exit 1; }
    grep -Fq 'Target configuration updated: /etc/crypttab' <<<"$out" \
        || { echo 'FAIL: the content-file write did not report success' >&2; exit 1; }
    [[ -f "$content_file" ]] \
        || { echo 'FAIL: the helper deleted the caller-supplied content file' >&2; exit 1; }
    if grep -q 'crypttab-secret-marker-90125' "$SESSION_LOG" 2>/dev/null; then
        echo 'FAIL: the content appeared in the session log' >&2
        exit 1
    fi
    if grep -q 'crypttab-secret-marker-90125' <<<"$out"; then
        echo 'FAIL: the content appeared in the helper output' >&2
        exit 1
    fi

    # (2) A recorded sudo invoker is honoured the same way.
    SUDO_UID="$cfg_uid" run_target_config write crypttab --content-file "$content_file" --content-owner "$cfg_uid" >/dev/null \
        || { echo 'FAIL: the SUDO_UID owner proof refused a matching owner' >&2; exit 1; }

    # (3) Wrong owner refused: the recorded invoker does not own the file.
    # The helper's fail() exits, so every expected-failure call runs in its
    # own subshell.
    printf 'original-crypttab\n' > "$TARGET_ROOT/etc/crypttab"
    if ( PKEXEC_UID=12345 run_target_config write crypttab --content-file "$content_file" --content-owner 12345 ) >"$cfg_root/refuse-owner.out" 2>&1; then
        echo 'FAIL: a mismatched PKEXEC_UID owner was accepted' >&2
        exit 1
    fi
    grep -Fq 'owner cannot be proven' "$cfg_root/refuse-owner.out" \
        || { echo 'FAIL: the mismatched-owner refusal does not name the reason' >&2; cat "$cfg_root/refuse-owner.out" >&2; exit 1; }
    [[ "$(cat "$TARGET_ROOT/etc/crypttab")" == "original-crypttab" ]] \
        || { echo 'FAIL: a refused write still modified the target' >&2; exit 1; }
    [[ -f "$content_file" ]] \
        || { echo 'FAIL: a refused write deleted the content file' >&2; exit 1; }

    # (4) Unprovable owner refused: no recorded invoker and a zero uid.
    if ( run_target_config write crypttab --content-file "$content_file" --content-owner 0 ) >"$cfg_root/refuse-zero.out" 2>&1; then
        echo 'FAIL: a zero --content-owner without a recorded invoker was accepted' >&2
        exit 1
    fi
    grep -Fq 'owner cannot be proven' "$cfg_root/refuse-zero.out" \
        || { echo 'FAIL: the unprovable-owner refusal does not name the reason' >&2; cat "$cfg_root/refuse-zero.out" >&2; exit 1; }

    # (5) Oversized content file refused before the target is touched.
    big_file="$cfg_root/oversized"
    dd if=/dev/zero of="$big_file" bs=1048576 count=1 status=none
    printf 'x' >> "$big_file"
    chmod 0600 "$big_file"
    if ( PKEXEC_UID="$cfg_uid" run_target_config write crypttab --content-file "$big_file" --content-owner "$cfg_uid" ) >"$cfg_root/refuse-big.out" 2>&1; then
        echo 'FAIL: an oversized content file was accepted' >&2
        exit 1
    fi
    grep -Fq '1 MiB' "$cfg_root/refuse-big.out" \
        || { echo 'FAIL: the oversized refusal does not name the cap' >&2; cat "$cfg_root/refuse-big.out" >&2; exit 1; }
    [[ "$(cat "$TARGET_ROOT/etc/crypttab")" == "original-crypttab" ]] \
        || { echo 'FAIL: an oversized refusal still modified the target' >&2; exit 1; }

    # (6) A symlinked content file is refused.
    ln -s "$content_file" "$cfg_root/content-link"
    if ( PKEXEC_UID="$cfg_uid" run_target_config write crypttab --content-file "$cfg_root/content-link" --content-owner "$cfg_uid" ) >"$cfg_root/refuse-link.out" 2>&1; then
        echo 'FAIL: a symlinked content file was accepted' >&2
        exit 1
    fi
    grep -Fq 'symlink-free regular file' "$cfg_root/refuse-link.out" \
        || { echo 'FAIL: the symlink refusal does not name the reason' >&2; cat "$cfg_root/refuse-link.out" >&2; exit 1; }

    # (7) The deprecated argv transport still works (legacy Qt3 GUI fallback).
    run_target_config write fstab $'line1\nline2\targv-fallback' >/dev/null \
        || { echo 'FAIL: the deprecated argv transport was refused' >&2; exit 1; }
    [[ "$(cat "$TARGET_ROOT/etc/fstab")" == $'line1\nline2\targv-fallback' ]] \
        || { echo 'FAIL: the deprecated argv transport did not write the content' >&2; exit 1; }

    # (8) Malformed option shapes are refused.
    if ( run_target_config write crypttab --content-file "$content_file" ) >"$cfg_root/refuse-argc.out" 2>&1; then
        echo 'FAIL: a truncated --content-file form was accepted' >&2
        exit 1
    fi
    grep -Fq 'invalid argument count' "$cfg_root/refuse-argc.out" \
        || { echo 'FAIL: the truncated-form refusal does not name the reason' >&2; cat "$cfg_root/refuse-argc.out" >&2; exit 1; }
    if ( run_target_config write crypttab --content-file "$content_file" --content-file-extra "$cfg_uid" ) >"$cfg_root/refuse-shape.out" 2>&1; then
        echo 'FAIL: a malformed --content-file option shape was accepted' >&2
        exit 1
    fi
    grep -Fq 'requires --content-owner' "$cfg_root/refuse-shape.out" \
        || { echo 'FAIL: the malformed-shape refusal does not name the reason' >&2; cat "$cfg_root/refuse-shape.out" >&2; exit 1; }
) || { echo 'FAIL: config-write content-file transport contract failed' >&2; exit 1; }
rm -rf -- "$cfg_root"

echo "PASS: chroot shell helper contract is wired and ordinary commands are accepted."
