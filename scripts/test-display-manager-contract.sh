#!/usr/bin/env bash
# This contract test sources the helper under test dynamically and sets the
# helper's globals directly so ShellCheck cannot track their use.
# shellcheck disable=SC1090,SC2034
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="$ROOT_DIR/scripts/boot-bitch-helper.sh"

[[ -x "$HELPER" ]] || { echo "FAIL: helper is not executable" >&2; exit 1; }
bash -n "$HELPER"

# The graphical-login stage must discover the configured manager and support
# common Debian-family managers without silently forcing SDDM.
grep -q '^detect_display_manager()' "$HELPER"
grep -q 'display_manager_package_for_service()' "$HELPER"
grep -q 'sddm.service).*sddm' "$HELPER"
grep -q 'gdm.service|gdm3.service).*gdm3' "$HELPER"
grep -q 'lightdm.service).*lightdm' "$HELPER"
grep -q 'greetd.service).*greetd' "$HELPER"
grep -q 'ly.service).*ly' "$HELPER"
grep -q 'Multiple display managers are installed' "$HELPER"
grep -q 'DISPLAY_MANAGER_SERVICE' "$HELPER"
grep -q 'enable "\$DISPLAY_MANAGER_SERVICE"' "$HELPER"
grep -q 'display-manager.service does not resolve to \$DISPLAY_MANAGER_LABEL' "$HELPER"

# Diagnostic evidence must include all supported manager names and graphics
# journal signals, while repair remains offline and never starts a GUI.
grep -q 'Installed display managers and units' "$HELPER"
grep -q 'Recent .*display-manager boot evidence' "$HELPER"
grep -q 'sddm|gdm|lightdm|greetd' "$HELPER"
grep -q 'msg_log dm-enable-offline' "$HELPER"
grep -q '^trial_display_manager_headless()' "$HELPER"
grep -q 'systemd-analyze --root=' "$HELPER"
grep -q 'msg_log dm-systemd-verify-pass' "$HELPER"

# Fedora/RPM display path: the same offline systemd flow with GDM (Fedora
# naming), rpm-aware installed-state queries and /etc/gdm/custom.conf.
grep -q '^target_rpm_package_installed()' "$HELPER"
grep -q 'gdm.service|gdm) printf' "$HELPER"
grep -q 'gdm3.service|gdm3) printf' "$HELPER"
grep -q 'known_services=(sddm.service gdm.service gdm3.service' "$HELPER"
grep -q '/etc/gdm/custom.conf' "$HELPER"
grep -q 'No supported package query backend is available' "$HELPER"

# The helper's stdout is a machine-readable contract consumed by the GUI and the
# contract tests, so it must pin a deterministic C locale before its first
# external command: every spawned tool (systemctl, dpkg, apt-get, grub-probe,
# ...) has to emit stable English regardless of the user's locale, otherwise the
# display-manager is-enabled grep and the grub-probe error greps would stop
# matching on a non-English system.  Assert the export sits in the top section
# of the helper (before the first function definition), and that the regenerated
# legacy helper carries the same pin near its top.
helper_top="$(sed -n '1,/^[A-Za-z_][A-Za-z0-9_]*()[[:space:]]*$/p' "$HELPER")"
grep -Fxq 'export LC_ALL=C' <<<"$helper_top" \
    || { echo 'FAIL: helper does not export LC_ALL=C before its first external command' >&2; exit 1; }
grep -Fxq 'export LANG=C' <<<"$helper_top" \
    || { echo 'FAIL: helper does not export LANG=C before its first external command' >&2; exit 1; }

LEGACY_HELPER="$ROOT_DIR/legacy/boot-bitch-helper.sh"
[[ -f "$LEGACY_HELPER" ]] || { echo 'FAIL: legacy helper is missing' >&2; exit 1; }
legacy_top="$(sed -n '1,/^# =/p' "$LEGACY_HELPER")"
grep -Fxq 'export LC_ALL=C' <<<"$legacy_top" \
    || { echo 'FAIL: legacy helper does not export LC_ALL=C near its top' >&2; exit 1; }
grep -Fxq 'export LANG=C' <<<"$legacy_top" \
    || { echo 'FAIL: legacy helper does not export LANG=C near its top' >&2; exit 1; }

# GDM is discovered from the installed unit when display-manager.service is
# missing (the Fedora default layout), labelled GDM, and its rpm package name
# resolves without dpkg evidence.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    gdm_root="$(mktemp -d)"
    trap 'rm -rf -- "$gdm_root"' EXIT
    mkdir -p "$gdm_root/usr/lib/systemd/system" "$gdm_root/etc/systemd/system" "$gdm_root/usr/bin"
    : > "$gdm_root/usr/lib/systemd/system/gdm.service"
    : > "$gdm_root/usr/lib/systemd/system/graphical.target"
    TARGET_ROOT="$gdm_root"
    TARGET_OS_ID=fedora
    TARGET_DISTRO_FAMILY=fedora
    RUNNING_HOST_MODE=0
    target_package_installed() { [[ "$1" == "gdm" ]]; }
    detect_display_manager
    [[ "$DISPLAY_MANAGER_SERVICE" == gdm.service ]] \
        || { echo "FAIL: GDM was not detected from the installed unit: $DISPLAY_MANAGER_SERVICE" >&2; exit 1; }
    [[ "$DISPLAY_MANAGER_LABEL" == GDM ]] \
        || { echo "FAIL: Fedora GDM label is wrong: $DISPLAY_MANAGER_LABEL" >&2; exit 1; }
    [[ "$DISPLAY_MANAGER_PACKAGE" == gdm ]] \
        || { echo "FAIL: Fedora GDM package name is wrong: $DISPLAY_MANAGER_PACKAGE" >&2; exit 1; }
    [[ "$DISPLAY_MANAGER_UNIT_REL" == /usr/lib/systemd/system/gdm.service ]] \
        || { echo "FAIL: Fedora GDM unit path is wrong: $DISPLAY_MANAGER_UNIT_REL" >&2; exit 1; }
    [[ "$(display_manager_label_for_service gdm3.service)" == GDM3 ]] \
        || { echo 'FAIL: Debian gdm3 must stay GDM3' >&2; exit 1; }
)

# The rpm package-state branch answers installed queries without dpkg, and
# package_query_available accepts an rpm-only target.  Round 17: the query
# consults the once-per-run package list (evidence_package_list rpm), so the
# stub feeds the cached-list shape (<name>@<version>) and a changed package
# database is modelled with evidence_cache_invalidate (the same hook a
# changed-status repair uses).
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    rpm_dm_root="$(mktemp -d)"
    trap 'rm -rf -- "$rpm_dm_root"' EXIT
    mkdir -p "$rpm_dm_root/usr/bin" "$rpm_dm_root/var/lib/rpm"
    : > "$rpm_dm_root/usr/bin/rpm"; chmod +x "$rpm_dm_root/usr/bin/rpm"
    # Round 71: rpm backend detection requires a NON-EMPTY rpm package database
    # (the fixture's stubbed `rpm -qa` provides the package records).
    printf 'sqlite' > "$rpm_dm_root/var/lib/rpm/rpmdb.sqlite"
    TARGET_ROOT="$rpm_dm_root"
    TARGET_OS_ID=fedora
    TARGET_DISTRO_FAMILY=fedora
    RUNNING_HOST_MODE=0
    package_query_available \
        || { echo 'FAIL: an rpm-only target has no package query backend' >&2; exit 1; }
    run_selected_chroot() { printf 'gdm@47.3-1.fc44\n'; return 0; }
    target_package_installed gdm \
        || { echo 'FAIL: rpm-installed gdm was not recognized' >&2; exit 1; }
    evidence_cache_invalidate
    run_selected_chroot() { printf 'something-else@9\n'; return 0; }
    if target_package_installed gdm; then
        echo 'FAIL: a different rpm package satisfied the gdm query' >&2
        exit 1
    fi
    [[ "$(display_manager_package_for_service gdm.service)" == gdm ]] \
        || { echo 'FAIL: Fedora GDM package mapping is wrong' >&2; exit 1; }
)

# OpenRC/Alpine diagnostics must never claim systemd graphical.target or
# display-manager.service: the configured manager is the init service that
# provides display-manager and is enabled in a runlevel.
grep -q '^diagnostic_display_openrc()' "$HELPER"
grep -q '^alpine_display_manager_services()' "$HELPER"
grep -q '^alpine_service_enabled_runlevels()' "$HELPER"
grep -q '^alpine_package_state()' "$HELPER"
dm_services_body="$(sed -n '/^alpine_display_manager_services()/,/^}/p' "$HELPER")"
grep -q 'provide' <<<"$dm_services_body" \
    || { echo 'FAIL: Alpine display-manager discovery does not read OpenRC provide lines' >&2; exit 1; }
grep -q 'display-manager' <<<"$dm_services_body" \
    || { echo 'FAIL: Alpine display-manager discovery does not look for display-manager' >&2; exit 1; }
grep -q 'OpenRC service manager:' "$HELPER"
grep -q 'No OpenRC service provides display-manager' "$HELPER"
grep -q 'non_journald_log_source_label' "$HELPER"
grep -q '^syslog_non_user_filter()' "$HELPER"

# diagnostic_display dispatches to the OpenRC branch before any systemd probe.
display_body="$(sed -n '/^diagnostic_display()/,/^}/p' "$HELPER")"
grep -q 'diagnostic_display_openrc' <<<"$display_body" \
    || { echo 'FAIL: diagnostic_display does not dispatch to the OpenRC branch' >&2; exit 1; }
openrc_body="$(sed -n '/^diagnostic_display_openrc()/,/^}/p' "$HELPER")"
[[ -n "$openrc_body" ]] || { echo 'FAIL: diagnostic_display_openrc is missing' >&2; exit 1; }
for forbidden in 'display-manager.service is not enabled' 'Unable to determine default target' \
    'dpkg-query is not available' 'Systemd default target' 'graphical.target'; do
    if grep -Fq "$forbidden" <<<"$openrc_body"; then
        echo "FAIL: OpenRC display diagnostics claim systemd state: $forbidden" >&2
        exit 1
    fi
done

# ---------------------------------------------------------------------------
# Alpine/OpenRC display-manager repair contract: the repair path detects the
# service that provides display-manager, validates its apk package and init
# script offline, restores only the default-runlevel symlink and never runs
# systemctl or starts the graphical session.
# ---------------------------------------------------------------------------
grep -q '^detect_alpine_display_manager()' "$HELPER"
grep -q '^alpine_display_manager_command_path()' "$HELPER"
grep -q '^preflight_alpine_display_manager()' "$HELPER"
grep -q '^adaptive_alpine_display_manager_repair()' "$HELPER"
grep -q '^target_apk_package_installed()' "$HELPER"
grep -q 'Multiple OpenRC services provide display-manager' "$HELPER"
grep -q 'sh -n' "$HELPER"
grep -q 'rc-service -e' "$HELPER"
grep -q 'msg_log dm-openrc-restore-link' "$HELPER"
grep -q 'reason openrc-runlevel-enabled' "$HELPER"

# adaptive_display_manager_repair dispatches to the OpenRC branch before the
# systemd preflight/restore functions.
adaptive_display_body="$(sed -n '/^adaptive_display_manager_repair()/,/^}/p' "$HELPER")"
grep -q 'adaptive_alpine_display_manager_repair' <<<"$adaptive_display_body" \
    || { echo 'FAIL: adaptive_display_manager_repair does not dispatch to the OpenRC branch' >&2; exit 1; }
openrc_repair_body="$(sed -n '/^adaptive_alpine_display_manager_repair()/,/^}/p' "$HELPER")"
[[ -n "$openrc_repair_body" ]] || { echo 'FAIL: adaptive_alpine_display_manager_repair is missing' >&2; exit 1; }
for forbidden in 'systemctl' 'restore_display_manager' 'graphical.target' 'display-manager.service' 'rc-service .* start'; do
    if grep -Eq "$forbidden" <<<"$openrc_repair_body"; then
        echo "FAIL: OpenRC display-manager repair claims systemd or starts the GUI: $forbidden" >&2
        exit 1
    fi
done
grep -q 'ln -sfn "/etc/init.d/\$DISPLAY_MANAGER_SERVICE"' <<<"$openrc_repair_body" \
    || { echo 'FAIL: OpenRC repair does not restore the runlevel symlink' >&2; exit 1; }

# Behaviour with stubbed preflight/chroot runners: an already-correct runlevel
# link is unchanged, a missing link is restored and reports changed, and an
# ambiguous provide display-manager set fails closed.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    dm_root="$(mktemp -d)"
    trap 'rm -rf -- "$dm_root"' EXIT
    SESSION_LOG="$dm_root/session.log"
    SESSION_DIR="$dm_root/session"
    mkdir -p "$SESSION_DIR" "$dm_root/etc/init.d" "$dm_root/etc/runlevels/default" \
        "$dm_root/usr/bin" "$dm_root/lib/apk/db"
    : > "$SESSION_LOG"
    printf '#!/sbin/openrc-run\nprovide display-manager\ncommand=/usr/bin/lightdm\n' > "$dm_root/etc/init.d/lightdm"
    chmod +x "$dm_root/etc/init.d/lightdm"
    : > "$dm_root/usr/bin/lightdm"; chmod +x "$dm_root/usr/bin/lightdm"
    printf 'P:lightdm\n\n' > "$dm_root/lib/apk/db/installed"
    TARGET_ROOT="$dm_root"
    TARGET_DISTRO_FAMILY=alpine
    RUNNING_HOST_MODE=0
    run_chroot_try() { CHROOT_TRY_RC=0; CHROOT_TRY_OUTPUT=''; }
    ln -sfn /etc/init.d/lightdm "$dm_root/etc/runlevels/default/lightdm"
    dm_out="$(adaptive_alpine_display_manager_repair)"
    grep -Fqx 'Repair change status display: unchanged|reason:openrc-runlevel-enabled|param:lightdm' <<<"$dm_out" \
        || { echo 'FAIL: OpenRC unchanged status is wrong' >&2; printf '%s\n' "$dm_out" >&2; exit 1; }
    rm -f "$dm_root/etc/runlevels/default/lightdm"
    dm_out="$(adaptive_alpine_display_manager_repair)"
    grep -Fqx 'Repair change status display: changed' <<<"$dm_out" \
        || { echo 'FAIL: OpenRC changed status is wrong' >&2; printf '%s\n' "$dm_out" >&2; exit 1; }
    [[ "$(readlink "$dm_root/etc/runlevels/default/lightdm")" == /etc/init.d/lightdm ]] \
        || { echo 'FAIL: OpenRC runlevel link was not restored' >&2; exit 1; }
    printf '#!/sbin/openrc-run\nprovide display-manager\n' > "$dm_root/etc/init.d/sddm"
    chmod +x "$dm_root/etc/init.d/sddm"
    if dm_out="$(adaptive_alpine_display_manager_repair 2>&1)"; then
        echo 'FAIL: ambiguous OpenRC display-manager set was accepted' >&2
        exit 1
    fi
    grep -Fq 'Multiple OpenRC services provide display-manager' <<<"$dm_out" \
        || { echo 'FAIL: ambiguous OpenRC reason is missing' >&2; exit 1; }
)

# Static regression guard: Ubuntu gdm3.service has `[Install] Alias=display-manager.service`
# (no WantedBy), so `systemctl is-enabled gdm3` returns "alias", not "enabled".
# The systemd restore/already-correct checks must accept every valid enabled
# state (enabled, enabled-runtime, alias, indirect), not just "enabled".
grep -Eq "grep -Exq 'enabled\|enabled-runtime\|alias\|indirect'" "$HELPER" \
    || { echo 'FAIL: systemd display-manager is-enabled check no longer accepts the alias/indirect enabled states' >&2; exit 1; }

# ---------------------------------------------------------------------------
# greetd landing contract (rounds 58/78/83b): the landing brings the wlgreet
# GRAPHICAL greeter AND the post-login desktop-session command, every landing
# failure routes through the component-level refusal (the distinguished exit
# code + the truthful unchanged status — never a stage ERROR), and the
# round-83b write-verify trap (the Debian-family config.toml whose
# [default_session] is the LAST section) is pinned.
# ---------------------------------------------------------------------------
grep -q 'GREETD_LANDING_REFUSAL_RC=42' "$HELPER" \
    || { echo 'FAIL: greetd landing refusal code is missing' >&2; exit 1; }
grep -q 'display_manager_greetd_write_session_command' "$HELPER" \
    || { echo 'FAIL: greetd session-command write is missing' >&2; exit 1; }
grep -q 'display_manager_greetd_session_shell' "$HELPER" \
    || { echo 'FAIL: greetd session-shell detection is missing' >&2; exit 1; }
grep -q 'reason greetd-no-session-shell' "$HELPER" \
    || { echo 'FAIL: greetd-no-session-shell refusal is missing' >&2; exit 1; }
grep -q 'reason greetd-config-missing' "$HELPER" \
    || { echo 'FAIL: greetd-config-missing refusal is missing' >&2; exit 1; }
grep -q 'reason greetd-config-write-failed' "$HELPER" \
    || { echo 'FAIL: greetd-config-write-failed refusal is missing' >&2; exit 1; }
grep -q 'reason greetd-config-write-verify-failed' "$HELPER" \
    || { echo 'FAIL: greetd-config-write-verify-failed refusal is missing' >&2; exit 1; }
grep -q 'reason greetd-gnome-wayland-unsupported' "$HELPER" \
    || { echo 'FAIL: greetd-gnome-wayland-unsupported refusal is missing (the round-83c GNOME class)' >&2; exit 1; }
grep -q '^greetd_greeter_is_bare_wlgreet()' "$HELPER" \
    || { echo 'FAIL: the bare-non-compositor greeter residue detector is missing (the round-83c heal)' >&2; exit 1; }
grep -q '^display_manager_greetd_shell_is_gnome_wayland()' "$HELPER" \
    || { echo 'FAIL: the GNOME-Wayland session classifier is missing (the round-83c honest handling)' >&2; exit 1; }
grep -q '^display_manager_services_same_manager()' "$HELPER" \
    || { echo 'FAIL: the §15.6 alias-pair comparison helper is missing (the round-83c purge deferral)' >&2; exit 1; }
grep -q 'grep -Eq .*AutomaticLoginEnable' "$HELPER" \
    || { echo 'FAIL: the GDM autologin disable probe is missing' >&2; exit 1; }
grep -q '/etc/gdm3/custom.conf' "$HELPER" \
    || { echo 'FAIL: the Ubuntu /etc/gdm3/custom.conf autologin path is not probed (the round-83c finding 2)' >&2; exit 1; }
grep -q 'switch_rc == GREETD_LANDING_REFUSAL_RC' "$HELPER" \
    || { echo 'FAIL: the walk-through no longer re-displays the menu on a greetd-landing refusal' >&2; exit 1; }
write_config_body="$(sed -n '/^display_manager_greetd_write_config()/,/^}/p' "$HELPER")"
if grep -q '[[:space:]]fail ' <<<"$write_config_body"; then
    echo 'FAIL: greetd config write still hard-fails (the round-83b stage-ERROR class)' >&2
    exit 1
fi
if grep -q 'command = "wlgreet"' <<<"$write_config_body"; then
    echo 'FAIL: the greetd write still carries the bare-wlgreet tail-flush shape (the write-verify trap)' >&2
    exit 1
fi

# The round-83b landing shape: the stock agreety greeter + the
# [default_session]-LAST config.toml (the Debian-family trap shape) + the
# stock `command = "sway"` wlgreet.toml (the black-session shape) →
# the landing writes the wlgreet compositor greeter AND the detected desktop
# session command (a NON-GNOME desktop session — the GNOME-Wayland class is
# the round-83c refusal, drilled separately), rc 0, both verifies pass.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    gd_root="$(mktemp -d)"
    trap 'rm -rf -- "$gd_root"' EXIT
    SESSION_LOG="$gd_root/session.log"; : > "$SESSION_LOG"
    SESSION_DIR="$gd_root/session"; mkdir -p "$SESSION_DIR"
    mkdir -p "$gd_root/etc/greetd" "$gd_root/usr/share/wayland-sessions" "$gd_root/usr/bin"
    printf '[terminal]\nvt = 1\n\n[default_session]\ncommand = "agreety --cmd /bin/sh"\nuser = "greeter"\n' \
        > "$gd_root/etc/greetd/config.toml"
    printf 'command = "sway"\noutputMode = "all"\nscale = 1\n' \
        > "$gd_root/etc/greetd/wlgreet.toml"
    printf '[Desktop Entry]\nName=Plasma\nExec=/usr/bin/startplasma-wayland\n' \
        > "$gd_root/usr/share/wayland-sessions/plasma.desktop"
    printf '#!/bin/sh\n' > "$gd_root/usr/bin/startplasma-wayland"; chmod +x "$gd_root/usr/bin/startplasma-wayland"
    TARGET_ROOT="$gd_root"
    TARGET_OS_ID=ubuntu
    TARGET_DISTRO_FAMILY=debian
    RUNNING_HOST_MODE=0
    INSTALLED_DESKTOP_PROBED=1
    INSTALLED_DESKTOP_FACT="kde-plasma-desktop (installed desktop metapackage)"
    target_package_installed() { [[ "$1" == wlgreet ]]; }
    display_manager_repo_availability() { printf 'available\n'; }
    display_manager_greetd_landing
    grep -Fq 'command = "sway --config /etc/greetd/sway-config"' "$gd_root/etc/greetd/config.toml" \
        || { echo 'FAIL: the [default_session]-last config.toml did not receive the wlgreet compositor command' >&2; exit 1; }
    grep -Fq 'command = "wlgreet"' "$gd_root/etc/greetd/config.toml" \
        && { echo 'FAIL: the bare-wlgreet tail-flush shape landed in the config' >&2; exit 1; }
    grep -Fq 'command = "/usr/bin/startplasma-wayland"' "$gd_root/etc/greetd/wlgreet.toml" \
        || { echo 'FAIL: the stock wlgreet.toml command was not rewritten to the detected desktop session' >&2; exit 1; }
)

# The Ubuntu live shape: the greeter is ALREADY configured (the compositor
# command) — the landing still rewrites the stock `command = "sway"`
# wlgreet.toml to the detected shell and leaves the greeter config untouched.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    gd_root="$(mktemp -d)"
    trap 'rm -rf -- "$gd_root"' EXIT
    SESSION_LOG="$gd_root/session.log"; : > "$SESSION_LOG"
    SESSION_DIR="$gd_root/session"; mkdir -p "$SESSION_DIR"
    mkdir -p "$gd_root/etc/greetd" "$gd_root/usr/share/wayland-sessions" "$gd_root/usr/bin"
    printf '[terminal]\nvt = 7\n\n[default_session]\ncommand = "sway --config /etc/greetd/sway-config"\nuser = "_greetd"\n' \
        > "$gd_root/etc/greetd/config.toml"
    printf 'command = "sway"\n' > "$gd_root/etc/greetd/wlgreet.toml"
    printf '[Desktop Entry]\nName=Plasma\nExec=/usr/bin/startplasma-wayland\n' \
        > "$gd_root/usr/share/wayland-sessions/plasma.desktop"
    printf '#!/bin/sh\n' > "$gd_root/usr/bin/startplasma-wayland"; chmod +x "$gd_root/usr/bin/startplasma-wayland"
    TARGET_ROOT="$gd_root"
    TARGET_OS_ID=ubuntu
    TARGET_DISTRO_FAMILY=debian
    RUNNING_HOST_MODE=0
    INSTALLED_DESKTOP_PROBED=1
    INSTALLED_DESKTOP_FACT="kde-plasma-desktop (installed desktop metapackage)"
    target_package_installed() { [[ "$1" == wlgreet ]]; }
    display_manager_repo_availability() { printf 'available\n'; }
    before_config="$(sha256sum "$gd_root/etc/greetd/config.toml" | awk '{print $1}')"
    display_manager_greetd_landing
    after_config="$(sha256sum "$gd_root/etc/greetd/config.toml" | awk '{print $1}')"
    [[ "$before_config" == "$after_config" ]] \
        || { echo 'FAIL: the already-configured greeter config was rewritten' >&2; exit 1; }
    grep -Fq 'command = "/usr/bin/startplasma-wayland"' "$gd_root/etc/greetd/wlgreet.toml" \
        || { echo 'FAIL: the already-configured-greeter landing skipped the session-command write' >&2; exit 1; }
)

# The no-session-shell refusal: no session entries AND no resolvable existing
# command → the preflight AND the landing both refuse with the distinguished
# code and the truthful unchanged status (never a stage ERROR).
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    gd_root="$(mktemp -d)"
    trap 'rm -rf -- "$gd_root"' EXIT
    SESSION_LOG="$gd_root/session.log"; : > "$SESSION_LOG"
    SESSION_DIR="$gd_root/session"; mkdir -p "$SESSION_DIR"
    mkdir -p "$gd_root/etc/greetd"
    printf '[default_session]\ncommand = "sway --config /etc/greetd/sway-config"\nuser = "_greetd"\n' \
        > "$gd_root/etc/greetd/config.toml"
    TARGET_ROOT="$gd_root"
    RUNNING_HOST_MODE=0
    INSTALLED_DESKTOP_PROBED=1
    INSTALLED_DESKTOP_FACT=""
    target_package_installed() { [[ "$1" == wlgreet ]]; }
    display_manager_repo_availability() { printf 'available\n'; }
    repair_change_status() { printf 'Repair change status %s: %s\n' "$1" "$2"; }
    rc=0
    display_manager_greetd_landing_preflight || rc=$?
    [[ "$rc" == "$GREETD_LANDING_REFUSAL_RC" ]] \
        || { echo 'FAIL: the no-session-shell preflight did not refuse (rc='"$rc"')' >&2; exit 1; }
    [[ "${DISPLAY_MANAGER_GREETD_REFUSAL_REASON:-}" == greetd-no-session-shell ]] \
        || { echo 'FAIL: the no-session-shell refusal reason is missing' >&2; exit 1; }
    rc=0
    display_manager_greetd_landing || rc=$?
    [[ "$rc" == "$GREETD_LANDING_REFUSAL_RC" ]] \
        || { echo 'FAIL: the no-session-shell landing did not refuse (rc='"$rc"')' >&2; exit 1; }
    emit_out="$(display_manager_greetd_emit_refusal_status)"
    grep -Fqx 'Repair change status display: unchanged|reason:greetd-no-session-shell' <<<"$emit_out" \
        || { echo 'FAIL: the no-session-shell refusal status line is wrong' >&2; printf '%s\n' "$emit_out" >&2; exit 1; }
)

# The custom-command keep: no session entries, but the existing wlgreet
# command resolves inside the scope → the landing keeps it (rc 0).
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    gd_root="$(mktemp -d)"
    trap 'rm -rf -- "$gd_root"' EXIT
    SESSION_LOG="$gd_root/session.log"; : > "$SESSION_LOG"
    SESSION_DIR="$gd_root/session"; mkdir -p "$SESSION_DIR"
    mkdir -p "$gd_root/etc/greetd" "$gd_root/usr/local/bin"
    printf '[default_session]\ncommand = "sway --config /etc/greetd/sway-config"\nuser = "_greetd"\n' \
        > "$gd_root/etc/greetd/config.toml"
    printf 'command = "/usr/local/bin/myshell"\n' > "$gd_root/etc/greetd/wlgreet.toml"
    printf '#!/bin/sh\n' > "$gd_root/usr/local/bin/myshell"; chmod +x "$gd_root/usr/local/bin/myshell"
    TARGET_ROOT="$gd_root"
    RUNNING_HOST_MODE=0
    INSTALLED_DESKTOP_PROBED=1
    INSTALLED_DESKTOP_FACT=""
    target_package_installed() { [[ "$1" == wlgreet ]]; }
    display_manager_repo_availability() { printf 'available\n'; }
    rc=0
    display_manager_greetd_landing || rc=$?
    [[ "$rc" == 0 ]] \
        || { echo 'FAIL: the resolvable custom post-login command was not kept (rc='"$rc"')' >&2; exit 1; }
    grep -Fq 'command = "/usr/local/bin/myshell"' "$gd_root/etc/greetd/wlgreet.toml" \
        || { echo 'FAIL: the custom post-login command was rewritten' >&2; exit 1; }
)

# The missing-config refusal: no /etc/greetd/config.toml → the landing
# refuses with the config-missing reason + the truthful status (never a stage
# ERROR).
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    gd_root="$(mktemp -d)"
    trap 'rm -rf -- "$gd_root"' EXIT
    SESSION_LOG="$gd_root/session.log"; : > "$SESSION_LOG"
    SESSION_DIR="$gd_root/session"; mkdir -p "$SESSION_DIR"
    mkdir -p "$gd_root/etc/greetd" "$gd_root/usr/share/wayland-sessions" "$gd_root/usr/bin"
    printf 'command = "sway"\n' > "$gd_root/etc/greetd/wlgreet.toml"
    printf '[Desktop Entry]\nName=Ubuntu\nExec=/usr/bin/gnome-session --session=ubuntu\n' \
        > "$gd_root/usr/share/wayland-sessions/ubuntu.desktop"
    printf '#!/bin/sh\n' > "$gd_root/usr/bin/gnome-session"; chmod +x "$gd_root/usr/bin/gnome-session"
    TARGET_ROOT="$gd_root"
    RUNNING_HOST_MODE=0
    INSTALLED_DESKTOP_PROBED=1
    INSTALLED_DESKTOP_FACT="ubuntu-desktop (installed desktop metapackage)"
    target_package_installed() { [[ "$1" == wlgreet ]]; }
    display_manager_repo_availability() { printf 'available\n'; }
    repair_change_status() { printf 'Repair change status %s: %s\n' "$1" "$2"; }
    rc=0
    display_manager_greetd_landing || rc=$?
    [[ "$rc" == "$GREETD_LANDING_REFUSAL_RC" ]] \
        || { echo 'FAIL: the missing-config landing did not refuse (rc='"$rc"')' >&2; exit 1; }
    [[ "${DISPLAY_MANAGER_GREETD_REFUSAL_REASON:-}" == greetd-config-missing ]] \
        || { echo 'FAIL: the missing-config refusal reason is wrong' >&2; exit 1; }
    emit_out="$(display_manager_greetd_emit_refusal_status)"
    grep -Fqx 'Repair change status display: unchanged|reason:greetd-config-missing' <<<"$emit_out" \
        || { echo 'FAIL: the missing-config refusal status line is wrong' >&2; printf '%s\n' "$emit_out" >&2; exit 1; }
)

# ---------------------------------------------------------------------------
# Round 83c (the DM runtime fix — the FIVE applied findings): the
# bare-non-compositor residue heal, the GNOME-Wayland-under-greetd refusal,
# the gdm3-vs-gdm purge deferral (the live-serving daemon survives), the
# Ubuntu /etc/gdm3/custom.conf autologin path, and the §15.6 candidate
# canonicalization.
# ---------------------------------------------------------------------------

# FINDING 3 — the pre-fix residue heal: a config.toml carrying the broken
# tail-flush residue `command = "wlgreet"` (a bare greeter client, no
# compositor) must NOT be read as an already-configured greeter — the
# landing rewrites it through the single-path write (the compositor command)
# and still writes the detected non-GNOME session command.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    gd_root="$(mktemp -d)"
    trap 'rm -rf -- "$gd_root"' EXIT
    SESSION_LOG="$gd_root/session.log"; : > "$SESSION_LOG"
    SESSION_DIR="$gd_root/session"; mkdir -p "$SESSION_DIR"
    mkdir -p "$gd_root/etc/greetd" "$gd_root/usr/share/wayland-sessions" "$gd_root/usr/bin"
    printf '[terminal]\nvt = 1\n\n[default_session]\ncommand = "wlgreet"\nuser = "_greetd"\n' \
        > "$gd_root/etc/greetd/config.toml"
    printf 'command = "sway"\n' > "$gd_root/etc/greetd/wlgreet.toml"
    printf '[Desktop Entry]\nName=Plasma\nExec=/usr/bin/startplasma-wayland\n' \
        > "$gd_root/usr/share/wayland-sessions/plasma.desktop"
    printf '#!/bin/sh\n' > "$gd_root/usr/bin/startplasma-wayland"; chmod +x "$gd_root/usr/bin/startplasma-wayland"
    TARGET_ROOT="$gd_root"
    RUNNING_HOST_MODE=0
    INSTALLED_DESKTOP_PROBED=1
    INSTALLED_DESKTOP_FACT="kde-plasma-desktop (installed desktop metapackage)"
    target_package_installed() { [[ "$1" == wlgreet ]]; }
    display_manager_repo_availability() { printf 'available\n'; }
    rc=0
    display_manager_greetd_landing || rc=$?
    [[ "$rc" == 0 ]] \
        || { echo 'FAIL: the bare-wlgreet residue landing did not succeed (rc='"$rc"')' >&2; exit 1; }
    grep -Fq 'command = "sway --config /etc/greetd/sway-config"' "$gd_root/etc/greetd/config.toml" \
        || { echo 'FAIL: the bare-wlgreet residue was not rewritten to the compositor greeter' >&2; exit 1; }
    grep -Fq 'command = "wlgreet"' "$gd_root/etc/greetd/config.toml" \
        && { echo 'FAIL: the bare-wlgreet residue survived the rewrite' >&2; exit 1; }
    grep -Fq 'msg:dm-greetd-greeter-residue-heal' "$SESSION_LOG" \
        || { echo 'FAIL: the residue-heal marker was not logged' >&2; cat "$SESSION_LOG" >&2; exit 1; }
    grep -Fq 'command = "/usr/bin/startplasma-wayland"' "$gd_root/etc/greetd/wlgreet.toml" \
        || { echo 'FAIL: the healed landing skipped the session-command write' >&2; exit 1; }
)

# FINDING 4 — the GNOME-Wayland-under-greetd refusal: the detected session
# shell is a wayland-sessions gnome-session entry → the classifier names the
# class, the preflight AND the landing refuse with the distinguished code +
# the truthful unchanged status, and the wlgreet.toml stays untouched.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    gd_root="$(mktemp -d)"
    trap 'rm -rf -- "$gd_root"' EXIT
    SESSION_LOG="$gd_root/session.log"; : > "$SESSION_LOG"
    SESSION_DIR="$gd_root/session"; mkdir -p "$SESSION_DIR"
    mkdir -p "$gd_root/etc/greetd" "$gd_root/usr/share/wayland-sessions" "$gd_root/usr/bin"
    printf '[default_session]\ncommand = "sway --config /etc/greetd/sway-config"\nuser = "_greetd"\n' \
        > "$gd_root/etc/greetd/config.toml"
    printf 'command = "sway"\n' > "$gd_root/etc/greetd/wlgreet.toml"
    printf '[Desktop Entry]\nName=Ubuntu\nExec=/usr/bin/gnome-session --session=ubuntu\nDesktopNames=ubuntu;GNOME\n' \
        > "$gd_root/usr/share/wayland-sessions/ubuntu.desktop"
    printf '#!/bin/sh\n' > "$gd_root/usr/bin/gnome-session"; chmod +x "$gd_root/usr/bin/gnome-session"
    TARGET_ROOT="$gd_root"
    RUNNING_HOST_MODE=0
    INSTALLED_DESKTOP_PROBED=1
    INSTALLED_DESKTOP_FACT="ubuntu-desktop (installed desktop metapackage)"
    target_package_installed() { [[ "$1" == wlgreet ]]; }
    display_manager_repo_availability() { printf 'available\n'; }
    repair_change_status() { printf 'Repair change status %s: %s\n' "$1" "$2"; }
    shell="$(display_manager_greetd_session_shell)"
    [[ "$shell" == "/usr/bin/gnome-session --session=ubuntu" ]] \
        || { echo 'FAIL: the GNOME fixture did not detect the gnome-session shell' >&2; exit 1; }
    display_manager_greetd_shell_is_gnome_wayland "$shell" \
        || { echo 'FAIL: the gnome-session wayland shell was not classified GNOME-Wayland' >&2; exit 1; }
    rc=0
    display_manager_greetd_landing_preflight || rc=$?
    [[ "$rc" == "$GREETD_LANDING_REFUSAL_RC" ]] \
        || { echo 'FAIL: the GNOME-wayland preflight did not refuse (rc='"$rc"')' >&2; exit 1; }
    [[ "${DISPLAY_MANAGER_GREETD_REFUSAL_REASON:-}" == greetd-gnome-wayland-unsupported ]] \
        || { echo 'FAIL: the GNOME-wayland refusal reason is wrong' >&2; exit 1; }
    rc=0
    display_manager_greetd_landing || rc=$?
    [[ "$rc" == "$GREETD_LANDING_REFUSAL_RC" ]] \
        || { echo 'FAIL: the GNOME-wayland landing did not refuse (rc='"$rc"')' >&2; exit 1; }
    emit_out="$(display_manager_greetd_emit_refusal_status)"
    grep -Fqx 'Repair change status display: unchanged|reason:greetd-gnome-wayland-unsupported|param:/usr/bin/gnome-session --session=ubuntu' <<<"$emit_out" \
        || { echo 'FAIL: the GNOME-wayland refusal status line is wrong' >&2; printf '%s\n' "$emit_out" >&2; exit 1; }
    grep -Fq 'command = "sway"' "$gd_root/etc/greetd/wlgreet.toml" \
        || { echo 'FAIL: the GNOME-wayland refusal wrote the wlgreet.toml' >&2; exit 1; }
    # The X11 class is NOT refused: a shell resolved from an xsessions entry
    # (the same gnome-session Exec text) classifies as non-GNOME-Wayland.
    mkdir -p "$gd_root/usr/share/xsessions"
    printf '[Desktop Entry]\nName=GNOME Xorg\nExec=/usr/bin/gnome-session --session=ubuntu-xorg\n' \
        > "$gd_root/usr/share/xsessions/ubuntu-xorg.desktop"
    if display_manager_greetd_shell_is_gnome_wayland "/usr/bin/gnome-session --session=ubuntu-xorg"; then
        echo 'FAIL: an xsessions-only gnome-session Exec was classified GNOME-Wayland' >&2
        exit 1
    fi
)

# FINDING 1 — the gdm3-vs-gdm purge deferral: the session-serving canonical
# gdm.service must defer its gdm3 ALIAS member (the §15.6 pair collapse —
# the round-83c Ubuntu breach killed the live GNOME session through the
# alias name).  The CONTROL: a different serving manager leaves the gdm3
# stray stoppable.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    pr_root="$(mktemp -d)"
    trap 'rm -rf -- "$pr_root"' EXIT
    SESSION_LOG="$pr_root/session.log"; : > "$SESSION_LOG"
    SESSION_DIR="$pr_root/session"; mkdir -p "$SESSION_DIR"
    STATE_ROOT="$pr_root/state"; mkdir -p "$STATE_ROOT"
    mkdir -p "$pr_root/etc/systemd/system"
    TARGET_ROOT="$pr_root"
    RUNNING_HOST_MODE=1
    stopped=()
    systemctl() {
        if [[ "$1" == "--root" ]]; then
            shift 2
        fi
        if [[ "$1" == "stop" ]]; then
            stopped+=("$2")
            return 0
        fi
        if [[ "$1" == "is-enabled" ]]; then
            printf 'disabled\n'
            return 0
        fi
        return 1
    }
    pgrep() { [[ "$1" == "-x" && "$2" == "gdm3" ]] && return 0; return 1; }
    pkill() { return 1; }
    display_manager_session_serving_service() { printf 'gdm.service\n'; }
    mutating_cancel_checkpoint() { return 0; }
    display_manager_purge_others greetd.service
    ((${#stopped[@]} == 0)) \
        || { echo "FAIL: the session-serving GDM alias (gdm3) was stopped: ${stopped[*]}" >&2; exit 1; }
    grep -Fq 'msg:dm-purge-session-deferred|param:GDM3' "$SESSION_LOG" \
        || { echo 'FAIL: the gdm3 alias of the session-serving GDM was not deferred' >&2; cat "$SESSION_LOG" >&2; exit 1; }
    stopped=()
    display_manager_session_serving_service() { printf 'lightdm.service\n'; }
    display_manager_purge_others greetd.service
    [[ " ${stopped[*]} " == *"gdm3.service"* || " ${stopped[*]} " == *" gdm3 "* ]] \
        || { echo "FAIL: the non-serving gdm3 stray was not stopped: ${stopped[*]}" >&2; exit 1; }
)

# FINDING 2 — the Ubuntu /etc/gdm3/custom.conf autologin path: a GDM landing
# disables the autologin in EVERY GDM config layout, the Ubuntu custom.conf
# included (AutomaticLoginEnable -> False + the AutomaticLogin line removed).
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    al_root="$(mktemp -d)"
    trap 'rm -rf -- "$al_root"' EXIT
    SESSION_DIR="$al_root/session"; mkdir -p "$SESSION_DIR"
    mkdir -p "$al_root/etc/gdm3"
    printf '[daemon]\nAutomaticLoginEnable=true\nAutomaticLogin=ubuntu\nWaylandEnable=true\n' \
        > "$al_root/etc/gdm3/custom.conf"
    TARGET_ROOT="$al_root"
    RUNNING_HOST_MODE=0
    display_manager_disable_autologin gdm.service
    grep -Fxq 'AutomaticLoginEnable=False' "$al_root/etc/gdm3/custom.conf" \
        || { echo 'FAIL: the Ubuntu custom.conf autologin was not disabled' >&2; cat "$al_root/etc/gdm3/custom.conf" >&2; exit 1; }
    if grep -Fq 'AutomaticLogin=' "$al_root/etc/gdm3/custom.conf"; then
        echo 'FAIL: the Ubuntu custom.conf AutomaticLogin line survived' >&2
        exit 1
    fi
)

# FINDING 5 — the candidate canonicalization: the gdm3 alias member of the
# restore target (gdm) is never a separate switch candidate (the §15.6
# pair collapse); a different restore target keeps it.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    cd_root="$(mktemp -d)"
    trap 'rm -rf -- "$cd_root"' EXIT
    TARGET_ROOT="$cd_root"
    RUNNING_HOST_MODE=0
    DISPLAY_MANAGER_CACHED_PARSED=1
    DISPLAY_MANAGER_CACHED_CANDIDATES=(sddm.service:installed gdm3.service:installed lightdm.service:installed)
    out="$(display_manager_switch_candidates greetd.service gdm.service)"
    grep -Fq 'sddm.service|SDDM|installed' <<<"$out" \
        || { echo 'FAIL: the canonicalized candidate list lost SDDM' >&2; printf '%s\n' "$out" >&2; exit 1; }
    grep -Fq 'lightdm.service|LightDM|installed' <<<"$out" \
        || { echo 'FAIL: the canonicalized candidate list lost LightDM' >&2; printf '%s\n' "$out" >&2; exit 1; }
    grep -Fq 'gdm3.service' <<<"$out" \
        && { echo 'FAIL: the gdm3 alias of the restore target remained a switch candidate' >&2; printf '%s\n' "$out" >&2; exit 1; }
    out="$(display_manager_switch_candidates greetd.service sddm.service)"
    grep -Fq 'gdm3.service|GDM3|installed' <<<"$out" \
        || { echo 'FAIL: the pair member vanished with an unrelated restore target' >&2; printf '%s\n' "$out" >&2; exit 1; }
)

echo "PASS: graphical login manager detection, diagnostics, and repair contract is wired."
