#!/usr/bin/env bash
# This contract test sources the helper under test dynamically and sets the
# helper's globals directly so ShellCheck cannot track their use.
# shellcheck disable=SC1090,SC2034
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="$ROOT_DIR/scripts/boot-repair-helper.sh"

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
grep -q 'will NOT be started inside the repair chroot' "$HELPER"
grep -q '^trial_display_manager_headless()' "$HELPER"
grep -q 'systemd-analyze --root=' "$HELPER"
grep -q 'headless systemd verification' "$HELPER"

# Fedora/RPM display path: the same offline systemd flow with GDM (Fedora
# naming), rpm-aware installed-state queries and /etc/gdm/custom.conf.
grep -q '^target_rpm_package_installed()' "$HELPER"
grep -q 'gdm.service|gdm) printf' "$HELPER"
grep -q 'gdm3.service|gdm3) printf' "$HELPER"
grep -q 'known_services=(sddm.service gdm.service gdm3.service' "$HELPER"
grep -q '/etc/gdm/custom.conf' "$HELPER"
grep -q 'No supported package query backend is available' "$HELPER"

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
# package_query_available accepts an rpm-only target.
(
    source <(sed '/^main "\$@"/d' "$HELPER")
    trap - EXIT INT TERM HUP
    rpm_dm_root="$(mktemp -d)"
    trap 'rm -rf -- "$rpm_dm_root"' EXIT
    mkdir -p "$rpm_dm_root/usr/bin"
    : > "$rpm_dm_root/usr/bin/rpm"; chmod +x "$rpm_dm_root/usr/bin/rpm"
    TARGET_ROOT="$rpm_dm_root"
    TARGET_OS_ID=fedora
    TARGET_DISTRO_FAMILY=fedora
    RUNNING_HOST_MODE=0
    package_query_available \
        || { echo 'FAIL: an rpm-only target has no package query backend' >&2; exit 1; }
    run_selected_chroot() { printf 'gdm\n'; return 0; }
    target_package_installed gdm \
        || { echo 'FAIL: rpm-installed gdm was not recognized' >&2; exit 1; }
    run_selected_chroot() { printf 'something-else\n'; return 0; }
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
grep -q 'OpenRC default-runlevel link' "$HELPER"
grep -q 'OpenRC default runlevel already enabled' "$HELPER"

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
    grep -Fqx 'Repair change status display: unchanged|OpenRC default runlevel already enabled lightdm' <<<"$dm_out" \
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

echo "PASS: graphical login manager detection, diagnostics, and repair contract is wired."
