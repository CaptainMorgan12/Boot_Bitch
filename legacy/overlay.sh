# legacy/overlay.sh — legacy-only helper behaviour.
#
# shellcheck disable=SC2034
# The globals assigned here (STATE_ROOT, TARGET_OS_LEGACY, CHROOT_TRY_RC, ...)
# are consumed by the modern helper body this overlay is appended to, so
# ShellCheck cannot see their readers when the overlay is checked standalone.
#
# Appended verbatim to the generated legacy helper (legacy/boot-repair-helper.sh)
# just before `main "$@"`, after the transformed modern body.  It must stay
# bash 3.1-parseable (it is not passed through the syntax transform) and must
# only use functions/variables the generated helper defines.
#
# Every override here is evidence-gated: the modern behaviour is reused
# whenever the modern prerequisite is present (read_target_os_modern,
# grub_unavailable_reason_modern, adaptive_grub_repair_modern,
# diagnostic_repair_capabilities_modern, mount_special_modern, ..._modern).
# The modern helper (scripts/boot-repair-helper.sh) is never modified.

# State root: /run does not exist on Debian Etch; fall back to /var/run.
if [[ -n "${BOOT_REPAIR_STATE_ROOT:-}" ]]; then
    STATE_ROOT="$BOOT_REPAIR_STATE_ROOT"
elif [[ -d /run ]]; then
    STATE_ROOT=/run/boot-repair
else
    STATE_ROOT=/var/run/boot-repair
fi

# Set by read_target_os when the root was confirmed through legacy release
# evidence instead of /etc/os-release.  Evidence wording only; never a gate.
TARGET_OS_LEGACY=0

# ---------------------------------------------------------------------------
# Legacy root confirmation (/etc/os-release-less roots)
# ---------------------------------------------------------------------------

read_target_os()
{
    local version="" label=""
    if [[ -f "$TARGET_ROOT/etc/os-release" ]]; then
        read_target_os_modern
        return 0
    fi
    TARGET_OS_LEGACY=1
    # Etch-era roots may split /var and /usr onto separate LVs (the etch2
    # split-LV layout keeps the dpkg database on debian-var), so the dpkg
    # status pair alone is not reliable evidence: accept /etc/debian_version
    # paired with any one /etc-resident release file or the dpkg status pair.
    if [[ -f "$TARGET_ROOT/etc/debian_version" ]] \
        && { [[ -f "$TARGET_ROOT/var/lib/dpkg/status" ]] \
            || [[ -f "$TARGET_ROOT/var/lib/dpkg/status-old" ]] \
            || [[ -f "$TARGET_ROOT/etc/apt/sources.list" ]] \
            || [[ -f "$TARGET_ROOT/etc/inittab" ]]; }; then
        version="$(head -n1 "$TARGET_ROOT/etc/debian_version" 2>/dev/null || true)"
        TARGET_OS_ID="debian"
        TARGET_OS_LIKE=""
        TARGET_PRETTY="Debian (legacy root ${version:-unknown}, /etc/debian_version + release evidence)"
        log "Legacy root evidence accepted: /etc/debian_version + release evidence (split-mount safe; no /etc/os-release present)." | tee -a "$SESSION_LOG"
        return 0
    fi
    if [[ -f "$TARGET_ROOT/etc/redhat-release" ]] \
        && { [[ -d "$TARGET_ROOT/var/lib/rpm" ]] || [[ -f "$TARGET_ROOT/var/lib/rpm/Packages" ]]; }; then
        TARGET_OS_ID="rhel"
        TARGET_OS_LIKE=""
        TARGET_PRETTY="Red Hat (legacy root, /etc/redhat-release + rpm database)"
        log "Legacy root evidence accepted: /etc/redhat-release + rpm database (no /etc/os-release present)." | tee -a "$SESSION_LOG"
        return 0
    fi
    if [[ -f "$TARGET_ROOT/etc/SuSE-release" && -d "$TARGET_ROOT/var/lib/rpm" ]]; then
        TARGET_OS_ID="suse"
        TARGET_OS_LIKE=""
        TARGET_PRETTY="SUSE (legacy root, /etc/SuSE-release + rpm database)"
        log "Legacy root evidence accepted: /etc/SuSE-release + rpm database (no /etc/os-release present)." | tee -a "$SESSION_LOG"
        return 0
    fi
    label="$(legacy_root_evidence_label "$TARGET_ROOT")"
    fail "Mounted filesystem does not contain /etc/os-release and no legacy root evidence was found (/etc/debian_version + release evidence, /etc/redhat-release + rpm database, /etc/SuSE-release + rpm database; probed label: $label); root selection is not confirmed."
}

# ---------------------------------------------------------------------------
# dpkg ${db:Status-*} probe (dpkg 1.13 has no virtual status fields)
# ---------------------------------------------------------------------------

legacy_dpkg_query_virtual_status_supported()
{
    local out="" rc=0
    if [[ -n "${LEGACY_DPKG_VIRTUAL_STATUS:-}" ]]; then
        [[ "$LEGACY_DPKG_VIRTUAL_STATUS" == yes ]]
        return
    fi
    out="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
        dpkg-query -W -f='${db:Status-Status}' dpkg 2>/dev/null)"
    rc=$?
    if (( rc == 0 )); then
        case "$out" in
            installed|not-installed|config-files|half-installed|unpacked|half-configured|triggers-awaited|triggers-pending|reinstreq)
                LEGACY_DPKG_VIRTUAL_STATUS=yes
                ;;
            *) LEGACY_DPKG_VIRTUAL_STATUS=no ;;
        esac
    else
        LEGACY_DPKG_VIRTUAL_STATUS=no
    fi
    [[ "$LEGACY_DPKG_VIRTUAL_STATUS" == yes ]]
}

# Print "installed" when the package is fully installed, nothing otherwise.
legacy_dpkg_status_field()
{
    local package="$1" status=""
    if legacy_dpkg_query_virtual_status_supported; then
        run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
            dpkg-query -W -f='${db:Status-Status}' "$package" 2>/dev/null || true
        return 0
    fi
    status="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
        dpkg-query -W -f='${Status}' "$package" 2>/dev/null || true)"
    if [[ "$status" == "install ok installed" ]]; then
        printf 'installed\n'
    fi
    return 0
}

# Print "installed <version>" when fully installed, nothing otherwise.
legacy_dpkg_status_version()
{
    local package="$1" status=""
    if legacy_dpkg_query_virtual_status_supported; then
        run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
            dpkg-query -W -f='${db:Status-Status} ${Version}' "$package" 2>/dev/null || true
        return 0
    fi
    status="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
        dpkg-query -W -f='${Status} ${Version}' "$package" 2>/dev/null || true)"
    case "$status" in
        "install ok installed "*) printf 'installed %s\n' "${status#install ok installed }" ;;
        "") ;;
        *) printf '%s\n' "$status" ;;
    esac
    return 0
}

dpkg_configuration_pending()
{
    local status_output="" rc=0
    set +e
    if legacy_dpkg_query_virtual_status_supported; then
        status_output="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
            dpkg-query -W -f='${db:Status-Abbrev}\n' 2>/dev/null)"
    else
        status_output="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
            dpkg-query -W -f='${Status}\n' 2>/dev/null)"
    fi
    rc=$?
    set -e
    (( rc == 0 )) || return 0
    if [[ "${LEGACY_DPKG_VIRTUAL_STATUS:-}" == yes ]]; then
        awk '$0 ~ /^ii( |$)/ { next } $0 ~ /^i/ { found=1 } END { exit found ? 0 : 1 }' <<<"$status_output"
    else
        awk '$0 == "install ok installed" { next } $1 == "install" { found=1 } END { exit found ? 0 : 1 }' <<<"$status_output"
    fi
}

target_package_installed()
{
    local package="$1" status=""
    [[ -n "$package" ]] || return 1
    if target_has_executable /usr/bin/dpkg-query /usr/sbin/dpkg-query /bin/dpkg-query; then
        status="$(legacy_dpkg_status_field "$package")"
        if [[ "$status" == "installed" ]]; then
            return 0
        fi
        return 1
    fi
    if target_has_executable /usr/bin/pacman /usr/bin/pacman-static; then
        run_selected_chroot /usr/bin/env PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            pacman --root / -Q "$package" >/dev/null 2>&1
        return $?
    fi
    if target_has_executable /sbin/apk /usr/sbin/apk /usr/bin/apk; then
        target_apk_package_installed "$package"
        return $?
    fi
    if [[ -f "$TARGET_ROOT/lib/apk/db/installed" ]]; then
        target_apk_package_installed "$package"
        return $?
    fi
    if target_has_executable /usr/bin/rpm /bin/rpm /usr/sbin/rpm; then
        target_rpm_package_installed "$package"
        return $?
    fi
    return 1
}

# ---------------------------------------------------------------------------
# GRUB legacy (update-grub / menu.lst) guarded branch
# ---------------------------------------------------------------------------

LEGACY_GRUB_BACKUP=""
LEGACY_GRUB_DECLARATIONS=""

legacy_grub_legacy_target()
{
    [[ -f "$TARGET_ROOT/boot/grub/menu.lst" ]] || return 1
    target_has_executable /usr/sbin/update-grub /usr/bin/update-grub || return 1
    target_has_executable /usr/sbin/grub-mkconfig /usr/bin/grub-mkconfig && return 1
    return 0
}

# Normalized title/kernel/initrd/root/module declarations.  These are the
# boot-critical identity of a GRUB legacy menu; GRUB_DISTRIBUTOR and title
# wording are not trusted as entry identity.
legacy_grub_entry_declarations()
{
    local config="$1"
    [[ -s "$config" ]] || return 0
    legacy_sed_ext -n '/^[[:space:]]*(title|kernel|initrd|root|module)[[:space:]]/p' "$config" 2>/dev/null \
        | legacy_sed_ext 's/[[:space:]]+/ /g; s/^ //; s/ $//' \
        | LC_ALL=C sort -u
}

legacy_grub_preflight()
{
    [[ -s "$TARGET_ROOT/boot/grub/menu.lst" ]] \
        || fail "GRUB legacy /boot/grub/menu.lst is missing or empty."
    target_has_executable /usr/sbin/update-grub /usr/bin/update-grub \
        || fail "update-grub is not installed in the target system."
    LEGACY_GRUB_BACKUP="$SESSION_DIR/grub-menu.lst.before"
    LEGACY_GRUB_DECLARATIONS="$SESSION_DIR/grub-menu-declarations.before"
    cp -a -- "$TARGET_ROOT/boot/grub/menu.lst" "$LEGACY_GRUB_BACKUP" \
        || fail "Unable to back up /boot/grub/menu.lst before regeneration."
    legacy_grub_entry_declarations "$TARGET_ROOT/boot/grub/menu.lst" > "$LEGACY_GRUB_DECLARATIONS"
    log "SIMULATE/PREFLIGHT: GRUB legacy menu.lst backed up to $LEGACY_GRUB_BACKUP and entry declarations captured." | tee -a "$SESSION_LOG"
}

legacy_grub_guard_entries_preserved()
{
    local before="$1" after="$2" after_keys="$SESSION_DIR/grub-menu-declarations.after"
    local missing="$SESSION_DIR/grub-menu-declarations.missing"
    [[ -s "$before" ]] || return 0
    legacy_grub_entry_declarations "$after" > "$after_keys"
    comm -23 "$before" "$after_keys" > "$missing" || true
    if [[ -s "$missing" ]]; then
        log "ERROR: GRUB legacy regeneration would remove existing menu declarations; the target configuration was rolled back." | tee -a "$SESSION_LOG"
        sed 's/^/  preserved-declaration-required: /' "$missing" | tee -a "$SESSION_LOG"
        return 1
    fi
    return 0
}

legacy_grub_repair()
{
    local grub_before="" grub_after=""
    [[ -x "$TARGET_ROOT/usr/sbin/update-grub" || -x "$TARGET_ROOT/usr/bin/update-grub" ]] \
        || fail "update-grub is not installed in the target system."
    if command -v sha256sum >/dev/null 2>&1; then
        grub_before="$(repair_file_fingerprint "$TARGET_ROOT/boot/grub/menu.lst")"
    fi
    legacy_grub_preflight
    run_chroot_try "Regenerate GRUB legacy configuration" update-grub
    if (( CHROOT_TRY_RC != 0 )); then
        cp -a -- "$LEGACY_GRUB_BACKUP" "$TARGET_ROOT/boot/grub/menu.lst"
        fail "update-grub failed after preflight; /boot/grub/menu.lst was restored."
    fi
    if [[ ! -s "$TARGET_ROOT/boot/grub/menu.lst" ]]; then
        cp -a -- "$LEGACY_GRUB_BACKUP" "$TARGET_ROOT/boot/grub/menu.lst"
        fail "update-grub completed but /boot/grub/menu.lst is missing or empty; the backup was restored."
    fi
    if ! legacy_grub_guard_entries_preserved "$LEGACY_GRUB_DECLARATIONS" "$TARGET_ROOT/boot/grub/menu.lst"; then
        cp -a -- "$LEGACY_GRUB_BACKUP" "$TARGET_ROOT/boot/grub/menu.lst"
        fail "GRUB legacy regeneration was rolled back because it removed an existing boot entry."
    fi
    log "PASS: GRUB legacy menu.lst regenerated and verified." | tee -a "$SESSION_LOG"
    if [[ -z "$grub_before" ]]; then
        repair_change_status grub changed
        return 0
    fi
    grub_after="$(repair_file_fingerprint "$TARGET_ROOT/boot/grub/menu.lst")"
    if [[ "$grub_before" == "$grub_after" ]]; then
        repair_change_status grub "unchanged|menu.lst is byte-identical"
    else
        repair_change_status grub changed
    fi
}

grub_config_path()
{
    if legacy_grub_legacy_target; then
        printf '%s\n' '/boot/grub/menu.lst'
    else
        grub_config_path_modern
    fi
}

grub_unavailable_reason()
{
    if legacy_grub_legacy_target; then
        return 0
    fi
    if ! target_has_executable /usr/sbin/grub-mkconfig /usr/bin/grub-mkconfig \
        && target_has_executable /usr/sbin/update-grub /usr/bin/update-grub; then
        printf 'GRUB legacy update-grub is installed but no /boot/grub/menu.lst was found; guarded legacy regeneration cannot be preflighted'
        return 1
    fi
    grub_unavailable_reason_modern
}

adaptive_grub_repair()
{
    if legacy_grub_legacy_target; then
        legacy_grub_repair
    else
        adaptive_grub_repair_modern
    fi
}

# ---------------------------------------------------------------------------
# Evidence-gated EFI / boot-stack gating for legacy BIOS targets
# ---------------------------------------------------------------------------

legacy_target_efi_evidence()
{
    profile_target_backends 2>/dev/null || true
    if [[ "${TARGET_ESP_MOUNT:-unresolved}" != unresolved ]]; then
        return 0
    fi
    target_has_path /boot/efi/EFI /efi/EFI /boot/EFI /boot/efi/EFI/Linux /efi/EFI/Linux
}

efi_unavailable_reason()
{
    if ! legacy_target_efi_evidence; then
        printf 'legacy BIOS target; no EFI boot path is available'
        return 1
    fi
    efi_unavailable_reason_modern
}

bootstack_unavailable_reason()
{
    local reason=""
    # The legacy boot-stack pass reconciles a GRUB-legacy BIOS target:
    # mapper/crypttab validation, initramfs rebuild and GRUB-legacy
    # regeneration in one guarded pass (the Etch equivalent of the modern
    # boot-stack reconciliation).  Availability follows the same probe-based
    # prerequisites as the two component repairs.
    if legacy_grub_legacy_target; then
        if ! reason="$(initramfs_unavailable_reason)"; then
            printf 'boot-stack reconciliation requires the initramfs repair: %s' "$reason"
            return 1
        fi
        if ! reason="$(grub_unavailable_reason)"; then
            printf 'boot-stack reconciliation requires the GRUB repair: %s' "$reason"
            return 1
        fi
        return 0
    fi
    bootstack_unavailable_reason_modern
}

# Legacy boot-stack reconciliation: the three Etch components run in one
# guarded pass and publish their own change statuses plus one aggregate.
legacy_boot_stack_repair()
{
    local entry key state changed=0
    log "Legacy boot-stack reconciliation: mapper/crypttab validation, initramfs rebuild and GRUB-legacy regeneration in one guarded pass." | tee -a "$SESSION_LOG"
    CURRENT_STAGE="boot-stack"
    REPAIR_CHANGE_STATUS_COLLECTED=()
    REPAIR_CHANGE_STATUS_COLLECT=1
    validate_mapper_crypttab
    adaptive_initramfs_repair
    adaptive_grub_repair
    REPAIR_CHANGE_STATUS_COLLECT=0
    for entry in "${REPAIR_CHANGE_STATUS_COLLECTED[@]:-}"; do
        key="${entry%%$'\t'*}"
        state="${entry#*$'\t'}"
        printf 'Repair change status %s: %s\n' "$key" "$state"
        if [[ -n "$SESSION_LOG" ]]; then
            printf 'Repair change status %s: %s\n' "$key" "$state" >> "$SESSION_LOG" 2>/dev/null || true
        fi
        case "$state" in
            unchanged|unchanged\|*) ;;
            *) changed=1 ;;
        esac
    done
    if (( changed == 1 )); then
        repair_change_status bootstack changed
    else
        repair_change_status bootstack "unchanged|mapper/crypttab, initramfs and GRUB-legacy reconciliation proved no change"
    fi
    log "PASS: legacy boot-stack reconciliation completed." | tee -a "$SESSION_LOG"
}

repair_boot_stack()
{
    if legacy_grub_legacy_target; then
        legacy_boot_stack_repair
        return 0
    fi
    repair_boot_stack_modern
}

legacy_host_has_efi_firmware()
{
    case "${BOOT_REPAIR_LEGACY_HOST_EFI:-auto}" in
        yes) return 0 ;;
        no) return 1 ;;
        *) [[ -d /sys/firmware/efi ]] ;;
    esac
}

legacy_host_maintenance_reason()
{
    # The guarded plain-chroot fallback (legacy_chroot) covers the host
    # initramfs/GRUB and display-manager stages on BIOS-only hosts: there are
    # no EFI firmware variables to isolate, so prepare_host_command_guard
    # skips the private mount namespace and the stages run through a plain
    # chroot.  An EFI host still needs unshare for firmware-variable
    # isolation and keeps the fail-closed modern guard.
    if legacy_host_has_efi_firmware; then
        if [[ -z "$(legacy_real_tool_path unshare)" ]]; then
            printf 'unshare is not installed in the recovery environment (this EFI host needs firmware-variable isolation)'
            return 1
        fi
    fi
    if [[ -z "$(legacy_real_tool_path chroot)" ]]; then
        printf 'chroot is not installed in the recovery environment'
        return 1
    fi
    return 0
}

legacy_feature_reason()
{
    local feature="$1" fstype=""
    case "$feature" in
        file-copy)
            # The legacy file-copy backend copies with cp -a and restores
            # ownership with chown --reference (no rsync --chown on Etch) and
            # verifies every regular file with cmp.
            if [[ -z "$(legacy_real_tool_path cp)" ]]; then
                printf 'cp is not installed in the recovery environment'
                return 1
            fi
            if [[ -z "$(legacy_real_tool_path cmp)" ]]; then
                printf 'cmp is not installed in the recovery environment (the verified copy needs cmp)'
                return 1
            fi
            if [[ -z "$(legacy_real_tool_path find)" ]]; then
                printf 'find is not installed in the recovery environment'
                return 1
            fi
            if [[ -z "$(legacy_real_tool_path chown)" ]]; then
                printf 'chown is not installed in the recovery environment'
                return 1
            fi
            if ! "$(legacy_real_tool_path chown)" --help 2>&1 | grep -q -- '--reference'; then
                printf 'the installed chown does not support --reference (legacy ownership preservation needs chown --reference)'
                return 1
            fi
            return 0
            ;;
        shell)
            # The offline chroot shell runs through the 4b legacy_chroot
            # guarded plain chroot (no unshare).  Etch's timeout has no
            # --foreground/--kill-after, so the bounded-kill containment is
            # not expressible here and the chroot command runs to completion
            # (documented deviation; stdin stays /dev/null so an interactive
            # question aborts instead of blocking).
            if [[ -z "$(legacy_real_tool_path chroot)" ]]; then
                printf 'chroot is not installed in the recovery environment'
                return 1
            fi
            return 0
            ;;
        host-shell)
            # On a BIOS-only host the legacy host shell runs one reviewed
            # command directly on the live host (no firmware namespace to
            # isolate, no chroot); an EFI host keeps the fail-closed modern
            # path, which needs the namespace and the bounded timeout.
            if legacy_host_has_efi_firmware; then
                if [[ -z "$(legacy_real_tool_path unshare)" ]]; then
                    printf 'unshare is not installed in the recovery environment (this EFI host needs firmware-variable isolation)'
                    return 1
                fi
                if [[ -z "$(legacy_real_tool_path timeout)" ]]; then
                    printf 'timeout with --foreground/--kill-after is not installed in the recovery environment'
                    return 1
                fi
                if ! legacy_timeout_real_supports_foreground "$(legacy_real_tool_path timeout)"; then
                    printf 'the installed timeout does not support --foreground/--kill-after'
                    return 1
                fi
            fi
            return 0
            ;;
        host-maintenance)
            legacy_host_maintenance_reason
            return $?
            ;;
        snapshots)
            if (( RUNNING_HOST_MODE == 1 )); then
                host_snapshot_rollback_unavailable_reason
                return $?
            fi
            fstype="$(lsblk -ndo FSTYPE "$ROOT_CANONICAL" 2>/dev/null | head -n1 || true)"
            if [[ "$fstype" != "btrfs" ]]; then
                printf 'the selected target filesystem is %s, not Btrfs' "${fstype:-unknown}"
                return 1
            fi
            if [[ -z "$(legacy_real_tool_path btrfs)" ]]; then
                printf 'btrfs tooling is not installed in the repair environment'
                return 1
            fi
            return 0
            ;;
        host-default)
            if (( RUNNING_HOST_MODE != 1 )); then
                printf 'host default selection requires the running-host scope'
                return 1
            fi
            host_default_unavailable_reason
            return $?
            ;;
        *)
            printf 'unknown legacy feature: %s' "$feature"
            return 1
            ;;
    esac
}

legacy_feature_gating_report()
{
    local feature="" reason=""
    echo "Legacy feature gating (read-only):"
    for feature in file-copy shell host-shell host-maintenance snapshots host-default; do
        if reason="$(legacy_feature_reason "$feature")"; then
            printf 'Legacy feature %s: available\n' "$feature"
        else
            printf 'Legacy feature %s: unavailable|%s\n' "$feature" "$reason"
        fi
    done
}

legacy_require_feature()
{
    local feature="$1" reason=""
    if reason="$(legacy_feature_reason "$feature")"; then
        return 0
    fi
    printf 'unavailable|%s: %s\n' "$feature" "$reason" >&2
    fail "$feature is unavailable: $reason"
}

# ---------------------------------------------------------------------------
# Guarded plain-chroot fallback (unshare-free selected-chroot execution)
# ---------------------------------------------------------------------------
# Etch has no util-linux unshare, so the private mount namespace that makes
# EFI firmware variables read-only cannot exist there.  On a BIOS-only host
# there are no firmware variables to protect: prepare_host_command_guard
# keeps HOST_COMMAND_GUARD=0 and every selected chroot command (including the
# host and offline initramfs stage) runs through this plain-chroot fallback
# with the same mount/preflight discipline - the target root is mounted by
# prepare_target/prepare_running_host, proc is available to the package and
# initramfs tools, and the session teardown still unmounts every helper-owned
# mount.  No write guard is weakened: the mapper/crypttab preflight, the
# trial builds and the backup behavior stay untouched.
legacy_chroot()
{
    local -a args=("$@")
    local chroot_bin=""
    ((${#args[@]} > 0)) || fail "legacy_chroot requires a command to run."
    chroot_bin="$(legacy_real_tool_path chroot)"
    [[ -n "$chroot_bin" ]] \
        || fail "chroot is not installed in the recovery environment."
    [[ -n "$TARGET_ROOT" && -d "$TARGET_ROOT" ]] \
        || fail "legacy_chroot has no prepared target root (the mount preflight must run first)."
    "$chroot_bin" "$TARGET_ROOT" "${args[@]}"
}

run_selected_chroot()
{
    local -a args=("$@")
    if (( HOST_COMMAND_GUARD == 1 )); then
        # EFI host: keep the fail-closed modern private namespace.
        run_selected_chroot_modern "${args[@]}"
    else
        legacy_chroot "${args[@]}"
    fi
}

# ---------------------------------------------------------------------------
# Legacy SysV display-manager probe and guarded repair (host scope)
# ---------------------------------------------------------------------------
# Etch configures the display manager through /etc/X11/default-display-manager
# (kdm/gdm/xdm/...) and starts it from a SysV runlevel S-symlink; the modern
# probe only knows systemd units and OpenRC services, which is why the
# display capability reports unavailable on Etch.  The read-only probe below
# accepts the configured legacy entry when its binary and init script exist;
# the repair restores the entry and the missing runlevel S-symlink with a
# backup and rollback and never starts the GUI or touches unrelated services.

# Print the configured legacy SysV display-manager path (the first non-comment
# token of /etc/X11/default-display-manager), or return 1.
legacy_sysv_display_manager_entry()
{
    local entry=""
    [[ -r "$TARGET_ROOT/etc/X11/default-display-manager" ]] || return 1
    while read -r entry _ignored; do
        [[ -n "$entry" && "${entry:0:1}" != "#" ]] || continue
        case "$entry" in
            /usr/bin/*|/usr/sbin/*) printf '%s\n' "$entry"; return 0 ;;
            *) return 1 ;;
        esac
    done < "$TARGET_ROOT/etc/X11/default-display-manager"
    return 1
}

# SysV-init-era evidence, read from TARGET_ROOT only: a /etc/init.d directory
# without a systemd or OpenRC presence inside the selected root.  Unlike
# target_sysvinit_present this never consults the live /run/systemd, so the
# host-scope probe stays evidence-driven on both Etch and modern recovery
# environments.
legacy_sysvinit_era()
{
    [[ -d "$TARGET_ROOT/etc/init.d" ]] \
        && ! target_has_executable /usr/lib/systemd/systemd /lib/systemd/systemd \
            /usr/bin/systemctl /bin/systemctl \
        && ! target_has_executable /sbin/openrc /usr/sbin/openrc /usr/bin/openrc \
        && ! target_has_path /etc/runlevels
}

# Read-only legacy display-manager probe: the configured entry, its executable
# binary and its init script.  Prints the short evidence line on success and
# the exact reason on failure.
legacy_display_manager_probe()
{
    local entry="" name=""
    entry="$(legacy_sysv_display_manager_entry)" || {
        printf 'no display manager is configured in /etc/X11/default-display-manager'
        return 1
    }
    name="$(basename -- "$entry")"
    [[ -x "$TARGET_ROOT$entry" ]] || {
        printf 'the configured display manager binary %s is not executable' "$entry"
        return 1
    }
    [[ -f "$TARGET_ROOT/etc/init.d/$name" ]] || {
        printf 'the init script /etc/init.d/%s is missing' "$name"
        return 1
    }
    printf 'sysvinit display manager %s (%s, init script /etc/init.d/%s)' \
        "$name" "$entry" "$name"
    return 0
}

LEGACY_DISPLAY_BACKUP_DIR=""
LEGACY_DISPLAY_CREATED_LINK=""

# Restore the pre-repair state after a failed legacy display-manager repair:
# the created S-symlink is removed and the backed-up entry file is restored.
legacy_display_manager_rollback()
{
    if [[ -n "$LEGACY_DISPLAY_CREATED_LINK" \
        && -L "$TARGET_ROOT$LEGACY_DISPLAY_CREATED_LINK" ]]; then
        rm -f -- "$TARGET_ROOT$LEGACY_DISPLAY_CREATED_LINK"
    fi
    if [[ -n "$LEGACY_DISPLAY_BACKUP_DIR" \
        && -f "$LEGACY_DISPLAY_BACKUP_DIR/default-display-manager.before" ]]; then
        if ! cp -a -- "$LEGACY_DISPLAY_BACKUP_DIR/default-display-manager.before" \
            "$TARGET_ROOT/etc/X11/default-display-manager"; then
            log "ERROR: unable to restore the display-manager backup file." | tee -a "$SESSION_LOG"
        fi
    fi
    log "Legacy display-manager repair was rolled back." | tee -a "$SESSION_LOG"
}

legacy_display_manager_repair()
{
    local entry="" name="" default_runlevel="" changed=0 current=""
    (( RUNNING_HOST_MODE == 1 )) \
        || fail "the legacy display-manager repair is a host-scope stage."
    entry="$(legacy_sysv_display_manager_entry)" \
        || fail "No legacy SysV display manager is configured in /etc/X11/default-display-manager."
    name="$(basename -- "$entry")"
    # Read-only check first: the configured binary must be executable and the
    # init script must exist before anything is written.
    [[ -x "$TARGET_ROOT$entry" ]] \
        || fail "The configured display manager binary $entry is not executable."
    [[ -f "$TARGET_ROOT/etc/init.d/$name" ]] \
        || fail "The init script /etc/init.d/$name is missing; refusing to change the display-manager configuration."
    default_runlevel="$(legacy_sed_ext -n 's/^id:\([0-9]\):initdefault:.*/\1/p' \
        "$TARGET_ROOT/etc/inittab" 2>/dev/null | head -n1 || true)"
    [[ -n "$default_runlevel" ]] || default_runlevel=2

    LEGACY_DISPLAY_BACKUP_DIR="$SESSION_DIR/display-manager-backup"
    mkdir -p -- "$LEGACY_DISPLAY_BACKUP_DIR"
    cp -a -- "$TARGET_ROOT/etc/X11/default-display-manager" \
        "$LEGACY_DISPLAY_BACKUP_DIR/default-display-manager.before" \
        || fail "Unable to back up /etc/X11/default-display-manager before the display-manager repair."
    log "SIMULATE/PREFLIGHT: legacy display manager $name (entry and the runlevel $default_runlevel S-symlink) backed up to $LEGACY_DISPLAY_BACKUP_DIR." | tee -a "$SESSION_LOG"

    current="$(head -n1 "$TARGET_ROOT/etc/X11/default-display-manager" 2>/dev/null | tr -d '[:space:]' || true)"
    if [[ "$current" != "$entry" ]]; then
        printf '%s\n' "$entry" > "$TARGET_ROOT/etc/X11/default-display-manager" \
            || { legacy_display_manager_rollback; fail "Unable to write /etc/X11/default-display-manager."; }
        changed=1
    fi

    # Restore the missing S-symlink in the default runlevel (S99 order, the
    # Debian-Etch display-manager convention).  The init script is never
    # invoked and unrelated services are never touched.
    if [[ ! -L "$TARGET_ROOT/etc/rc$default_runlevel.d/S99$name" ]]; then
        ln -s -- "../init.d/$name" "$TARGET_ROOT/etc/rc$default_runlevel.d/S99$name" \
            || { legacy_display_manager_rollback; fail "Unable to create the runlevel S-symlink for $name."; }
        LEGACY_DISPLAY_CREATED_LINK="/etc/rc${default_runlevel}.d/S99$name"
        changed=1
    fi

    # Post-write verification before reporting success.
    current="$(head -n1 "$TARGET_ROOT/etc/X11/default-display-manager" 2>/dev/null | tr -d '[:space:]' || true)"
    [[ "$current" == "$entry" ]] \
        || { legacy_display_manager_rollback; fail "The display-manager entry verification failed after the write."; }
    [[ -L "$TARGET_ROOT/etc/rc$default_runlevel.d/S99$name" ]] \
        || { legacy_display_manager_rollback; fail "The runlevel S-symlink verification failed after the repair."; }

    if (( changed == 0 )); then
        repair_change_status display "unchanged|default-display-manager entry and the runlevel S-symlink were already correct"
    else
        repair_change_status display changed
    fi
    log "PASS: legacy SysV display manager $name configuration restored; the display manager was never started." | tee -a "$SESSION_LOG"
    return 0
}

display_unavailable_reason()
{
    local legacy_reason=""
    if legacy_reason="$(legacy_display_manager_probe 2>/dev/null)"; then
        # The legacy stage is a host-scope action on this frontend; the
        # offline target form stays disabled with this exact reason.
        if (( RUNNING_HOST_MODE != 1 )); then
            printf 'the legacy SysV display-manager repair is a host-scope stage on this frontend (select Host Maintenance)'
            return 1
        fi
        return 0
    fi
    if legacy_sysvinit_era; then
        printf '%s' "$legacy_reason"
        return 1
    fi
    display_unavailable_reason_modern
}

adaptive_display_manager_repair()
{
    local backend="" legacy_ok=""
    backend="$(target_display_manager_backend)"
    legacy_ok="$(legacy_display_manager_probe 2>/dev/null || true)"
    if [[ "$backend" != systemd && "$backend" != OpenRC && -n "$legacy_ok" ]]; then
        legacy_display_manager_repair
        return 0
    fi
    adaptive_display_manager_repair_modern
}

repair_capability_evidence()
{
    local key="$1" legacy_out=""
    if [[ "$key" == bootstack ]] && legacy_grub_legacy_target; then
        printf 'legacy boot-stack: mapper/crypttab validation, initramfs rebuild and GRUB-legacy regeneration in one guarded pass'
        return 0
    fi
    if [[ "$key" == grub ]] && legacy_grub_legacy_target; then
        printf 'legacy GRUB: guarded menu.lst regeneration (update-grub, entry preservation and rollback)'
        return 0
    fi
    if [[ "$key" == display ]] && legacy_sysvinit_era; then
        legacy_out="$(legacy_display_manager_probe 2>/dev/null || true)"
        printf 'legacy SysV: %s' "${legacy_out:-no configured display manager}"
        return 0
    fi
    repair_capability_evidence_modern "$@"
}

# ---------------------------------------------------------------------------
# Etch-era target configuration keys (probe-based availability)
# ---------------------------------------------------------------------------
# The modern key list targets systemd/GRUB2-era files that do not exist on
# Etch, so the legacy helper adds the Etch equivalents (inittab, GRUB legacy
# menu.lst, modules, network interfaces, APT sources and APT configuration).
# `config-read`/`config-write` stay the same guarded verbs; only the key to
# path resolution changes, and every path keeps the modern guards (within the
# mounted target, no symlink, no creation).  Availability is reported by a
# read-only probe appended to the target diagnostics, so the GUI can grey or
# omit absent files with the helper's exact reason; the report is never
# emitted for the running-host scope, where target-file editing stays hidden.

legacy_config_path_for_key()
{
    case "${1:-}" in
        fstab)        printf '%s\n' '/etc/fstab' ;;
        inittab)      printf '%s\n' '/etc/inittab' ;;
        menu-lst)     printf '%s\n' '/boot/grub/menu.lst' ;;
        crypttab)     printf '%s\n' '/etc/crypttab' ;;
        modules)      printf '%s\n' '/etc/modules' ;;
        interfaces)   printf '%s\n' '/etc/network/interfaces' ;;
        sources-list) printf '%s\n' '/etc/apt/sources.list' ;;
        apt-conf)     printf '%s\n' '/etc/apt/apt.conf' ;;
        *) return 1 ;;
    esac
}

config_path_for_key()
{
    local key="${1:-}" path=""
    if path="$(legacy_config_path_for_key "$key")"; then
        printf '%s\n' "$path"
        return 0
    fi
    config_path_for_key_modern "$@"
}

legacy_config_file_report()
{
    local key="" path=""
    # Target scope only: Host Maintenance has no target-file editing.
    (( RUNNING_HOST_MODE == 0 )) || return 0
    echo "Legacy configuration files (read-only probe):"
    for key in fstab inittab menu-lst crypttab modules interfaces sources-list apt-conf; do
        path="$(legacy_config_path_for_key "$key")"
        if [[ -L "$TARGET_ROOT$path" ]]; then
            printf 'Legacy config %s: unavailable|%s is a symbolic link; the guarded reader refuses links\n' \
                "$key" "$path"
        elif [[ -f "$TARGET_ROOT$path" && -r "$TARGET_ROOT$path" ]]; then
            printf 'Legacy config %s: available\n' "$key"
        else
            printf 'Legacy config %s: unavailable|%s is not a readable regular file in the selected target\n' \
                "$key" "$path"
        fi
    done
}

# The modern guard installs an efibootmgr shim and proves efivarfs can be
# remounted read-only inside a private mount namespace; both need unshare,
# which Etch does not have.  On a host without EFI firmware there is no
# firmware variable to protect, so the guard is skipped; an EFI host keeps
# the fail-closed modern guard (and therefore still needs host-maintenance).
prepare_host_command_guard()
{
    if [[ ! -d /sys/firmware/efi ]]; then
        HOST_COMMAND_GUARD=0
        HOST_COMMAND_GUARD_DIR=""
        log "Legacy host command guard: no EFI firmware present; firmware-variable isolation is not required." | tee -a "$SESSION_LOG"
        return 0
    fi
    prepare_host_command_guard_modern
}

# The 13-key gating contract is emitted by the modern function; the legacy
# reports are appended so the 13 keys and their evidence lines stay untouched.
# The configuration probe only runs for a mounted repair target (never for the
# running host), matching the GUI's target-only Edit Target File control.
diagnostic_repair_capabilities()
{
    diagnostic_repair_capabilities_modern "$@"
    legacy_feature_gating_report
    legacy_config_file_report
}

# ---------------------------------------------------------------------------
# Gated command entry points
# ---------------------------------------------------------------------------

run_file_copy()
{
    legacy_require_feature file-copy
    legacy_run_file_copy "$@"
}

# Ownership policy for one legacy copy item.  Mirrors smart_chown_for_item:
# when the source's uid:gid maps to the same user/group names on both sides
# cp -a already preserves numeric ownership and no chown is needed; otherwise
# print the destination reference directory whose owner/group the copy must
# adopt through chown --reference (no rsync --chown on Etch).
legacy_chown_reference_for_item()
{
    local direction="$1" source="$2" destination="$3"
    local source_uid source_gid source_user source_group peer_user peer_group
    read -r source_uid source_gid < <(stat -c '%u %g' -- "$source")
    if [[ "$direction" == "host-to-repair" ]]; then
        source_user="$(host_user_name "$source_uid")"
        source_group="$(host_group_name "$source_gid")"
        peer_user="$(target_user_name "$source_uid")"
        peer_group="$(target_group_name "$source_gid")"
    else
        source_user="$(target_user_name "$source_uid")"
        source_group="$(target_group_name "$source_gid")"
        peer_user="$(host_user_name "$source_uid")"
        peer_group="$(host_group_name "$source_gid")"
    fi
    if [[ -n "$source_user" && -n "$source_group" && "$source_user" == "$peer_user" && "$source_group" == "$peer_group" ]]; then
        log "Ownership validation: $source_uid:$source_gid maps to $source_user:$source_group on both sides; preserving numeric ownership" | tee -a "$SESSION_LOG" >&2
        return 0
    fi
    log "Ownership validation: source $source_uid:$source_gid (${source_user:-unknown}:${source_group:-unknown}) does not map identically on the destination side; using the destination owner" | tee -a "$SESSION_LOG" >&2
    printf '%s\n' "$destination"
}

# cp -a copy of one item with optional chown --reference ownership adoption.
legacy_run_copy_item()
{
    local mode="$1" source="$2" destination="$3" reference="$4"
    local source_name="" files=0 bytes=0
    source_name="$(basename -- "$source")"
    if [[ "$mode" == "preview" ]]; then
        files="$(find "$source" -type f 2>/dev/null | wc -l | tr -d '[:space:]')"
        bytes="$(du -sk "$source" 2>/dev/null | awk '{print $1}')"
        log "PREVIEW: would copy $source -> $destination/ (${files:-0} file(s), ${bytes:-0} KiB, ownership reference ${reference:-preserved})" | tee -a "$SESSION_LOG"
        return 0
    fi
    cp -a -- "$source" "$destination/" \
        || fail "cp -a failed for $source."
    if [[ -n "$reference" ]]; then
        chown -R --reference="$reference" "$destination/$source_name" \
            || fail "Ownership restoration (chown --reference) failed for $destination/$source_name."
    fi
}

# Post-copy verification: every regular file is byte-compared with cmp.
# Prints the verified file count.
legacy_verify_copy_item()
{
    local source="$1" destination="$2" source_name rel file count=0 failed=0
    source_name="$(basename -- "$source")"
    while IFS= read -r file; do
        [[ -n "$file" ]] || continue
        rel="${file#"$source"/}"
        if ! cmp -s -- "$file" "$destination/$source_name/$rel"; then
            log "COPY VERIFY FAIL: $file differs from $destination/$source_name/$rel" | tee -a "$SESSION_LOG"
            failed=1
        else
            count=$((count + 1))
        fi
    done < <(find "$source" -type f 2>/dev/null)
    (( failed == 0 )) || fail "Post-copy verification failed for $source."
    log "COPY VERIFIED: $count regular file(s) byte-identical (cmp)." | tee -a "$SESSION_LOG"
    printf '%s\n' "$count"
}

# Legacy guarded File Copy for copy-preview and copy.  The direction/path and
# containment checks are the same modern helpers; the transfer is cp -a with
# chown --reference ownership adoption and a per-file cmp verification.
legacy_run_file_copy()
{
    local action="$1"; shift
    (($# >= 5)) || fail "File Copy requires direction, ownership, approval, destination and at least one source."

    local direction="$1" ownership="$2" approval="$3" destination_virtual="$4"; shift 4
    [[ "$destination_virtual" == "/" ]] || destination_virtual="${destination_virtual%/}"
    local -a requested_sources=("$@")
    ((${#requested_sources[@]} > 0)) || fail "No File Copy sources were provided."
    [[ "$direction" == "host-to-repair" || "$direction" == "repair-to-host" ]] || fail "Unknown File Copy direction: $direction"
    [[ "$ownership" == "smart" || "$ownership" == "preserve" ]] || fail "Unknown ownership policy: $ownership"
    [[ "$approval" == "normal" || "$approval" == "sensitive-ok" ]] || fail "Unknown File Copy approval token."

    need cp
    need chown
    need cmp
    need realpath
    need stat
    need find
    need getent

    local mode="preview"
    [[ "$action" == "copy" ]] && mode="copy"
    [[ "$action" == "copy-preview" || "$action" == "copy" ]] || fail "Internal File Copy action error: $action"
    # bash 3.1 has no case-conversion expansion: use the explicit label.
    local mode_label="PREVIEW"
    [[ "$mode" == "copy" ]] && mode_label="COPY"

    prepare_target ro

    local destination ownership_destination source virtual_path reference
    local -a sources=()

    if [[ "$direction" == "host-to-repair" ]]; then
        validate_virtual_path "$destination_virtual"
        if [[ "$mode" == "copy" ]] && target_destination_sensitive "$destination_virtual" && [[ "$approval" != "sensitive-ok" ]]; then
            fail "Sensitive repair-system destination requires explicit confirmation: $destination_virtual"
        fi
        maybe_mount_target_path "$destination_virtual" ro
        destination="$(target_destination_path "$destination_virtual" no)"
        ownership_destination="$(nearest_existing_directory "$destination")" || fail "No existing target destination parent was found."
        for source in "${requested_sources[@]}"; do
            [[ "$source" == "/" ]] || source="${source%/}"
            validate_host_source "$source"
            sources+=("$source")
        done
        if [[ "$mode" == "copy" ]]; then
            promote_target_data_rw
            maybe_mount_target_path "$destination_virtual" rw
            destination="$(target_destination_path "$destination_virtual" yes)"
            ownership_destination="$destination"
        fi
    else
        destination="$(validate_host_destination "$destination_virtual")"
        ownership_destination="$destination"
        for virtual_path in "${requested_sources[@]}"; do
            [[ "$virtual_path" == "/" ]] || virtual_path="${virtual_path%/}"
            validate_virtual_path "$virtual_path"
            maybe_mount_target_path "$virtual_path" ro
            source="$(target_source_path "$virtual_path")"
            sources+=("$source")
        done
    fi

    local source_name
    local -a seen_destination_names=() seen_destination_names_keys=()
    for source in "${sources[@]}"; do
        source_name="$(basename -- "$source")"
        [[ -n "$source_name" && "$source_name" != "." && "$source_name" != ".." ]] || fail "Invalid top-level source name: $source"
        if legacy_assoc_has seen_destination_names "$source_name"; then
            fail "Multiple selected sources would map to the same destination name '$source_name'. Copy them separately or rename one first."
        fi
        legacy_assoc_set seen_destination_names "$source_name" 1
    done

    if [[ "$mode" == "preview" && "$direction" == "host-to-repair" && ! -d "$destination" ]]; then
        log "PREVIEW: target destination directory would be created: $destination_virtual" | tee -a "$SESSION_LOG"
    elif [[ ! -d "$destination" ]]; then
        fail "Destination directory is unavailable: $destination_virtual"
    fi

    log "File Copy ${mode_label}: ${direction}" | tee -a "$SESSION_LOG"
    log "Destination: $destination_virtual" | tee -a "$SESSION_LOG"
    log "Ownership policy: $ownership" | tee -a "$SESSION_LOG"
    log "Sources: ${#sources[@]}" | tee -a "$SESSION_LOG"

    local item_count=0 cmp_count=0
    for source in "${sources[@]}"; do
        reference=""
        if [[ "$ownership" == "smart" ]]; then
            reference="$(legacy_chown_reference_for_item "$direction" "$source" "$ownership_destination")"
        fi
        log "${mode_label}: $(basename -- "$source")" | tee -a "$SESSION_LOG"
        legacy_run_copy_item "$mode" "$source" "$destination" "$reference"
        item_count=$((item_count + 1))
        if [[ "$mode" == "copy" ]]; then
            cmp_count=$((cmp_count + $(legacy_verify_copy_item "$source" "$destination")))
        fi
    done

    if [[ "$mode" == "copy" ]]; then
        sync
        log "COPY COMPLETE" | tee -a "$SESSION_LOG"
        log "  Source items: $item_count" | tee -a "$SESSION_LOG"
        log "  Regular files verified with cmp: $cmp_count" | tee -a "$SESSION_LOG"
        log "  Unrelated destination files: retained (no --delete used)" | tee -a "$SESSION_LOG"
    else
        log "PREVIEW COMPLETE - no files were changed." | tee -a "$SESSION_LOG"
    fi
}

run_chroot_shell()
{
    legacy_require_feature shell
    run_chroot_shell_modern "$@"
}

run_host_shell()
{
    legacy_require_feature host-shell
    run_host_shell_modern "$@"
}

# Legacy mount-options filter.  The generated helper adds `noload` to the
# read-only ext2/ext3/ext4 mounts (the root ro mount, the boot-entry ro mount
# and the os-release probe mount all funnel through mount_recorded); Etch's
# util-linux 2.12r rejects `ro,noload` ("ext3: No journal on filesystem on
# dm-5" + "wrong fs type, bad option, bad superblock") while plain `ro` mounts
# the same filesystem fine.  Strip the option so every ro ext mount uses plain
# `ro` (a dirty-journal ro mount failing is acceptable fail-closed behavior).
# The xfs `norecovery` option is left as-is: no xfs filesystem exists on Etch,
# so the option is simply never exercised.
legacy_filter_mount_options()
{
    printf '%s' "$1" | tr ',' '\n' | grep -v '^noload$' | paste -sd ',' -
}

mount_recorded()
{
    local source="$1" destination="$2" opt value
    local -a args=()
    shift 2
    while (($# > 0)); do
        opt="$1"
        case "$opt" in
            -o)
                [[ $# -ge 2 ]] || fail "mount_recorded: -o without an option list"
                value="$(legacy_filter_mount_options "$2")"
                if [[ -n "$value" ]]; then
                    args+=("-o" "$value")
                fi
                shift 2
                ;;
            -o*)
                value="$(legacy_filter_mount_options "${opt#-o}")"
                if [[ -n "$value" ]]; then
                    args+=("-o$value")
                fi
                shift
                ;;
            *)
                args+=("$opt")
                shift
                ;;
        esac
    done
    mount_recorded_modern "$source" "$destination" "${args[@]:-}"
}

# Etch-era fstab/crypttab entries name devices with bare host-relative paths
# (/dev/hda1 as seen from the installed system's own perspective).  On the
# repair host the target disk usually carries a different device name, so the
# same bare path points at the repair host's own disk and the
# same_single_top_disk guard refuses a valid target reference.  Remap a bare
# block-device path whose disk stem differs from TARGET_DISK onto the target
# disk (e.g. /dev/hda1 on a /dev/hdb target -> /dev/hdb1), only when the
# remapped partition actually exists on this host; every other form (UUID=,
# LABEL=, /dev/mapper, by-id, by-path, whole disks) passes through unchanged.
# The remap only rewrites the host-relative name: the caller still validates
# the result against the target disk with same_single_top_disk, so nothing
# is weakened.
legacy_remap_target_device_path()
{
    local source="$1" stem="" number="" target_base="" candidate=""
    [[ -n "$source" ]] || { printf '%s\n' "$source"; return 0; }
    case "$source" in
        /dev/mapper/*|/dev/disk/*|/dev/dm-*|/dev/md*|/dev/loop*) printf '%s\n' "$source"; return 0 ;;
    esac
    [[ "$source" == /dev/[a-z]*[0-9]* ]] || { printf '%s\n' "$source"; return 0; }
    [[ -n "$TARGET_DISK" && "$TARGET_DISK" == /dev/* ]] \
        || { printf '%s\n' "$source"; return 0; }
    stem="${source#/dev/}"
    target_base="${TARGET_DISK#/dev/}"
    number="$(printf '%s' "$stem" | legacy_sed_ext -n 's/^[a-z]+([0-9]+)$/\1/p')"
    [[ -n "$number" ]] || { printf '%s\n' "$source"; return 0; }
    # Already target-relative: the disk stem matches the target disk.
    [[ "$stem" == "$target_base$number" ]] && { printf '%s\n' "$source"; return 0; }
    candidate="/dev/${target_base}${number}"
    if [[ -b "$candidate" ]]; then
        printf '%s\n' "$candidate"
        return 0
    fi
    printf '%s\n' "$source"
    return 0
}

# The fstab/crypttab source resolution remaps the resolved bare device path
# through the legacy host-relative rewrite above; the modern resolution
# (UUID=/LABEL=/PARTUUID= and readlink -f for /dev paths) is unchanged.
resolve_fstab_source()
{
    local resolved=""
    resolved="$(resolve_fstab_source_modern "$@")" || return $?
    printf '%s\n' "$(legacy_remap_target_device_path "$resolved")"
}

# LUKS unlock for Etch's cryptsetup 1.0.  The modern helper opens with
# `cryptsetup open --type luks --key-file - "$device" "$name"`; cryptsetup 1.0
# has neither the `open` subcommand nor `--type` (its actions are
# `luksOpen <device> <name>` and its flag is `-d|--key-file`).  Every modern
# preflight is kept verbatim - the protected-host and same-disk asserts, the
# `cryptsetup isLuks` verification, the `luks-<uuid>` mapper naming, the
# existing-mapper reuse, the mapper-name-collision refusal, the rc-2
# passphrase-retry marker and the `UNLOCKED=<mapper>` output - only the
# opening invocation (and its failure cleanup) uses the 1.0 syntax.  The
# stdin key semantics are identical: the GUI closes stdin right after the
# passphrase, and --key-file - consumes the exact bytes.
unlock_target()
{
    local fstype uuid mapper_name mapper_path existing_mapper crypt_rc

    need lsblk
    need findmnt
    need cryptsetup
    need readlink

    TARGET_DISK="$(canonical_block "$TARGET_DISK")" || fail "Target disk is not a block device."
    ROOT_DEVICE="$(canonical_block "$ROOT_DEVICE")" || fail "LUKS component is not a block device."

    assert_target_not_host "$TARGET_DISK"
    same_single_top_disk "$TARGET_DISK" "$ROOT_DEVICE" \
        || fail "Selected encrypted component does not belong exclusively to the target disk."

    fstype="$(lsblk -ndo FSTYPE "$ROOT_DEVICE" 2>/dev/null | head -n1)"
    if [[ "$fstype" != "crypto_LUKS" ]] && ! cryptsetup isLuks "$ROOT_DEVICE" >/dev/null 2>&1; then
        fail "Selected component is not a LUKS container: $ROOT_DEVICE"
    fi

    uuid="$(cryptsetup luksUUID "$ROOT_DEVICE" 2>/dev/null || true)"
    [[ -n "$uuid" ]] || fail "Unable to determine the LUKS UUID."

    # Debian/TUXEDO crypttab convention uses luks-<UUID>; opening with the
    # installed-system style name from the beginning prevents a repair chroot
    # from later seeing a stale boot-repair-* mount source.
    mapper_name="luks-$uuid"
    mapper_path="/dev/mapper/$mapper_name"

    # Reuse any existing dm-crypt mapping of this exact LUKS device; Boot
    # Bitch must not create a second mapping or close one it does not own.
    existing_mapper="$(find_crypt_mapper_for_device "$ROOT_DEVICE" 2>/dev/null || true)"
    if [[ -n "$existing_mapper" ]]; then
        log "LUKS target is already unlocked by existing mapper: $existing_mapper"
        printf 'UNLOCKED=%s\n' "$existing_mapper"
        return 0
    fi

    if [[ -e "$mapper_path" || -L "$mapper_path" ]]; then
        fail "Mapper name collision at $mapper_path; refusing to replace an existing mapping."
    fi

    log "Protected host check: PASS"
    log "Unlocking LUKS target $ROOT_DEVICE"
    log "Mapper name: $mapper_name"

    # cryptsetup 1.0 exit code 2 is the documented "no permission" result,
    # which includes an incorrect LUKS passphrase. Emit the machine-readable
    # marker so the GUI can offer a passphrase retry without re-authorizing.
    set +e
    cryptsetup --key-file - luksOpen "$ROOT_DEVICE" "$mapper_name"
    crypt_rc=$?
    set -e
    if (( crypt_rc != 0 )); then
        if (( crypt_rc == 2 )); then
            printf 'UNLOCK_AUTH_FAILED=1\n' >&2
            fail "LUKS passphrase was not accepted."
        fi
        fail "cryptsetup could not unlock the selected LUKS target (exit code $crypt_rc)."
    fi

    [[ -b "$mapper_path" ]] || {
        cryptsetup luksClose "$mapper_path" >/dev/null 2>&1 || true
        fail "cryptsetup reported success but the mapper device did not appear."
    }

    # Etch LVM: the unlocked PV's volume group stays invisible to the GUI
    # rescan until the kernel scans and activates it, so the target's logical
    # volumes (/dev/mapper/debian-root etc.) would never appear and the root
    # could not auto-resolve.  Best-effort only - a scan that finds nothing
    # must not fail the unlock - and never deactivates anything.
    if [[ -n "$(legacy_real_tool_path vgscan)" \
        && -n "$(legacy_real_tool_path vgchange)" ]]; then
        local lv_count="" activation_output=""
        vgscan --mknodes >/dev/null 2>&1 || true
        activation_output="$(vgchange -ay 2>&1 || true)"
        lv_count="$(printf '%s\n' "$activation_output" \
            | awk '/logical volume/ { for (i = 1; i <= NF; i++) if ($i ~ /^[0-9]+$/) { sum += $i; break } } END { print sum + 0 }')"
        if [[ -n "$lv_count" && "$lv_count" != "0" ]]; then
            log "LVM scan/activation after unlock: $lv_count logical volume(s) activated"
        else
            log "LVM scan/activation after unlock: no new logical volumes reported"
        fi
    fi

    log "LUKS unlock complete. The mapper remains open for this recovery session."
    printf 'UNLOCKED=%s\n' "$mapper_path"
}

run_chroot_shell()
{
    legacy_require_feature shell
    if (( HOST_COMMAND_GUARD == 1 )); then
        # EFI host: keep the fail-closed modern isolated path.
        run_chroot_shell_modern "$@"
        return 0
    fi
    legacy_chroot_shell "$@"
}

# The modern browse-target protocol base64-encodes directory names, but Qt
# 3.3.7 has no QByteArray::fromBase64 for the GUI side.  The legacy port
# emits the same BROWSE_ENTRY records with the raw name, percent-encoding
# only the three record-breaking bytes (%, CR, LF); the Qt3 picker decodes
# them inline.  A directory whose name embeds a newline stays unlistable on
# the Qt3 picker (documented deviation).
browse_target_directory()
{
    local virtual_path="$1" candidate root_real candidate_real entry name encoded
    validate_browse_virtual_path "$virtual_path"
    need find

    prepare_target ro
    maybe_mount_target_path "$virtual_path" ro

    candidate="$TARGET_ROOT$virtual_path"
    [[ -d "$candidate" && ! -L "$candidate" ]] \
        || fail "Repair-system folder does not exist: $virtual_path"
    root_real="$(realpath_existing "$TARGET_ROOT")" \
        || fail "Unable to resolve mounted target root."
    candidate_real="$(realpath_existing "$candidate")" \
        || fail "Unable to resolve repair-system folder: $virtual_path"
    path_within "$candidate_real" "$root_real" \
        || fail "Repair-system folder escapes the selected target through a symlink: $virtual_path"

    while IFS= read -r -d '' entry; do
        name="${entry##*/}"
        encoded="$(printf '%s' "$name" | sed -e 's/%/%25/g' -e 's/\r/%0D/g' -e 's/\n/%0A/g' | tr -d '\n')"
        printf 'BROWSE_ENTRY\t%s\n' "$encoded"
    done < <(find "$candidate_real" -mindepth 1 -maxdepth 1 -type d ! -type l -print0)
}

# Etch's apt(8) predates the apt command frontend (Debian 4.0 ships apt 0.6
# plus apt-get): `apt update` fails with a usage error.  Rewrite the reviewed
# command's intent conservatively to the apt-get equivalent when the first
# word is exactly `apt` and the second is a known subcommand; `full-upgrade`
# maps to Etch's `dist-upgrade`.  Anything else (a bare `apt`, an unknown
# subcommand, quoting games) runs unchanged through the same guards.
legacy_apt_intent_translate()
{
    local command="${1:-}" first="" rest="" sub="" args=""
    case "$command" in
        *\ *) first="${command%% *}"; rest="${command#* }" ;;
        *) first="$command"; rest="" ;;
    esac
    [[ "$first" == "apt" && -n "$rest" ]] || { printf '%s\n' "$command"; return 0; }
    sub="${rest%% *}"
    case "$sub" in
        update|upgrade|full-upgrade|dist-upgrade|install|remove|purge|autoremove|clean|autoclean) ;;
        *) { printf '%s\n' "$command"; return 0; } ;;
    esac
    args="${rest#* }"
    [[ "$sub" == "full-upgrade" ]] && sub="dist-upgrade"
    if [[ -n "$args" && "$args" != "$rest" ]]; then
        printf 'apt-get %s %s\n' "$sub" "$args"
    else
        printf 'apt-get %s\n' "$sub"
    fi
}

# The offline chroot shell through the guarded plain chroot.  Mirrors the
# modern flow (prepare_target rw, clean environment, stdin /dev/null,
# interactive-prompt transcript scan) minus the timeout wrapper: Etch's
# timeout has no --foreground/--kill-after, so the bounded-kill containment is
# not expressible and the command runs to completion (documented deviation;
# stdin stays /dev/null so a prompt aborts instead of blocking).
legacy_chroot_shell()
{
    local command="${1:-}" rc=0 transcript run_command retry_command="" retried=0
    [[ $# -eq 1 ]] || fail "shell requires exactly one command string."
    [[ -n "$command" ]] || fail "shell command cannot be empty."
    need chroot
    prepare_target rw
    log "BEGIN: Chroot shell command (guarded plain chroot)" | tee -a "$SESSION_LOG"
    log "Command: $command" | tee -a "$SESSION_LOG"
    transcript="$SESSION_DIR/chroot-shell-output"
    : > "$transcript"
    run_command="$(legacy_apt_intent_translate "$command")"
    if [[ "$run_command" != "$command" ]]; then
        log "apt intent translated: $run_command" | tee -a "$SESSION_LOG"
    fi
    while :; do
        set +e
        chroot "$TARGET_ROOT" /usr/bin/env \
            HOME=/root \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            DEBIAN_FRONTEND=noninteractive \
            APT_LISTCHANGES_FRONTEND=none \
            /bin/sh -c "$run_command" < /dev/null 2>&1 | tee -a "$SESSION_LOG" "$transcript"
        rc=${PIPESTATUS[0]}
        set -e
        if (( rc == 0 || retried == 1 )); then
            break
        fi
        retry_command="$(apt_shell_full_upgrade_command "$run_command" || true)"
        if [[ -z "$retry_command" ]] || ! apt_shell_upgrade_policy_refused "$transcript"; then
            break
        fi
        log "apt upgrade is disabled by this distribution; running 'apt full-upgrade' instead" | tee -a "$SESSION_LOG"
        log "Command: $retry_command" | tee -a "$SESSION_LOG"
        run_command="$retry_command"
        retried=1
    done
    log "Chroot shell exit code: $rc" | tee -a "$SESSION_LOG"
    if (( rc != 0 )) && grep -Eq "$CHROOT_SHELL_PROMPT_REGEX" "$transcript" 2>/dev/null; then
        printf '%s\n' "Boot Bitch: the command asked an interactive question, which the Chroot Shell cannot answer (stdin is /dev/null). Re-run it with the non-interactive flag, for example 'apt-get -y upgrade'." | tee -a "$SESSION_LOG"
    fi
    (( rc == 0 )) || fail "Chroot shell command failed (exit code $rc)."
}

run_host_shell()
{
    legacy_require_feature host-shell
    if (( HOST_COMMAND_GUARD == 1 )); then
        # EFI host: keep the fail-closed modern isolated path.
        run_host_shell_modern "$@"
        return 0
    fi
    legacy_host_shell "$@"
}

# One reviewed command on the live running host through the legacy path: the
# same prepare_running_host identity/boot-mount checks and the reviewed
# command-string guard as the modern host shell, executed directly (no
# firmware namespace exists on this BIOS-only host, so prepare_host_command_guard
# skipped the guard; no chroot).  The timeout/kill containment is not
# expressible on Etch's timeout and is documented as such.
legacy_host_shell()
{
    local raw_disk="${1:-}" raw_root="${2:-}" command="${3:-}"
    (($# == 3)) || fail "host-shell requires a host disk, root component and command string."
    [[ -n "$raw_disk" && -n "$raw_root" ]] || fail "host-shell requires a host disk and root component."
    [[ -n "$command" ]] || fail "host-shell command cannot be empty."

    CURRENT_STAGE="host shell"
    RUNNING_HOST_MODE=1
    prepare_running_host "$raw_disk" "$raw_root" no
    DIAGNOSTIC_SCOPE="Running Host"
    prepare_host_command_guard

    log "BEGIN: Running-host shell command (legacy direct path)" | tee -a "$SESSION_LOG"
    log "Command: $command" | tee -a "$SESSION_LOG"
    local transcript="$SESSION_DIR/host-shell-output" rc=0
    local run_command="$command" retry_command="" retried=0
    : > "$transcript"
    run_command="$(legacy_apt_intent_translate "$command")"
    if [[ "$run_command" != "$command" ]]; then
        log "apt intent translated: $run_command" | tee -a "$SESSION_LOG"
    fi
    while :; do
        set +e
        /usr/bin/env \
            HOME=/root \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            DEBIAN_FRONTEND=noninteractive \
            /bin/bash -lc "$run_command" 2>&1 | tee -a "$SESSION_LOG" "$transcript"
        rc=${PIPESTATUS[0]}
        set -e
        if (( rc == 0 || retried == 1 )); then
            break
        fi
        retry_command="$(apt_shell_full_upgrade_command "$run_command" || true)"
        if [[ -z "$retry_command" ]] || ! apt_shell_upgrade_policy_refused "$transcript"; then
            break
        fi
        log "apt upgrade is disabled by this distribution; running 'apt full-upgrade' instead" | tee -a "$SESSION_LOG"
        log "Command: $retry_command" | tee -a "$SESSION_LOG"
        run_command="$retry_command"
        retried=1
    done
    log "Running-host shell exit code: $rc" | tee -a "$SESSION_LOG"
    if (( rc != 0 )); then
        log "FAIL: Running-host shell command (exit code $rc)" | tee -a "$SESSION_LOG"
        exit "$rc"
    fi
    log "PASS: Running-host shell command" | tee -a "$SESSION_LOG"
}

run_snapshots()
{
    legacy_require_feature snapshots
    run_snapshots_modern "$@"
}

run_host_snapshots()
{
    legacy_require_feature host-maintenance
    legacy_require_feature snapshots
    run_host_snapshots_modern "$@"
}

run_host_default()
{
    # host-default is inherently a host-scope action (the GUI only offers it
    # under Host Maintenance); the dispatch does not set the flag, so do it
    # here exactly like run_host_repair does.
    RUNNING_HOST_MODE=1
    legacy_require_feature host-maintenance
    legacy_require_feature host-default
    if legacy_grub_legacy_target; then
        legacy_host_default_repair "$@"
        return 0
    fi
    run_host_default_modern "$@"
}

# Legacy Make Default for a GRUB-legacy BIOS host: verify /boot/grub/menu.lst,
# locate the canonical bootable Linux entry (the first title whose kernel line
# boots /boot/vmlinuz-<ver> with a matching initrd line) and set the
# `default <N>` directive with a backup and rollback.  Never writes a boot
# sector and never touches firmware (there is none on this BIOS host).
host_default_unavailable_reason()
{
    if (( RUNNING_HOST_MODE != 1 )); then
        printf 'host default selection requires the running-host scope'
        return 1
    fi
    if legacy_grub_legacy_target; then
        return 0
    fi
    host_default_unavailable_reason_modern
}

# The zero-based menu.lst entry index of the canonical bootable Linux entry.
legacy_menu_lst_canonical_entry()
{
    local menu="$TARGET_ROOT/boot/grub/menu.lst" line="" idx=-1 kernel=""
    while IFS= read -r line; do
        line="${line#"${line%%[![:space:]]*}"}"
        case "$line" in
            title*)
                idx=$((idx + 1))
                kernel=""
                ;;
            kernel*)
                [[ -n "$kernel" ]] || kernel="$line"
                ;;
            initrd*)
                # Accept both /boot/vmlinuz-<ver> and /vmlinuz-<ver> kernel
                # paths (the GRUB root is often /boot itself), matching the
                # initrd line the same way.
                if [[ "$kernel" == *"vmlinuz-"* && "$kernel" != *"single"* ]] \
                    && [[ "$line" == *"initrd.img-"* ]]; then
                    printf '%s\n' "$idx"
                    return 0
                fi
                ;;
        esac
    done < "$menu"
    return 1
}

legacy_host_default_repair()
{
    local raw_disk="${1:-}" raw_root="${2:-}" menu="" before="" after="" entry="" default_line=""
    (($# == 2)) || fail "host-default requires a host disk and root component."
    [[ -n "$raw_disk" && -n "$raw_root" ]] || fail "host-default requires a host disk and root component."
    CURRENT_STAGE="host default"
    RUNNING_HOST_MODE=1
    prepare_running_host "$raw_disk" "$raw_root" no
    DIAGNOSTIC_SCOPE="Running Host"
    profile_target_backends
    legacy_grub_legacy_target \
        || fail "Make Default on this frontend requires the GRUB-legacy BIOS layout (menu.lst + update-grub)."

    menu="$TARGET_ROOT/boot/grub/menu.lst"
    [[ -s "$menu" ]] || fail "GRUB legacy /boot/grub/menu.lst is missing or empty."
    entry="$(legacy_menu_lst_canonical_entry)" \
        || fail "No canonical bootable Linux entry was found in /boot/grub/menu.lst."
    printf 'Host default probe: legacy menu.lst canonical entry index %s\n' "$entry" | tee -a "$SESSION_LOG"

    before="$(repair_file_fingerprint "$menu")"
    cp -a -- "$menu" "$SESSION_DIR/menu.lst.before-host-default" \
        || fail "Unable to back up /boot/grub/menu.lst before Make Default."
    log "SIMULATE/PREFLIGHT: Make Default backed up $menu to $SESSION_DIR/menu.lst.before-host-default." | tee -a "$SESSION_LOG"

    default_line="$(grep -n '^[[:space:]]*default[[:space:]]' "$menu" | head -n1 || true)"
    if [[ -z "$default_line" ]]; then
        printf '\ndefault %s\n' "$entry" >> "$menu" \
            || { cp -a -- "$SESSION_DIR/menu.lst.before-host-default" "$menu"; fail "Unable to write the default directive; the backup was restored."; }
    else
        # legacy_sed_ext runs sed -E (ERE): use plain (...) groups, and
        # rebuild the directive instead of \1<digits> (sed would read \10 as
        # back-reference 10).
        legacy_sed_ext -i "s|^([[:space:]]*)default[[:space:]].*$|\\1default $entry|" "$menu" \
            || { cp -a -- "$SESSION_DIR/menu.lst.before-host-default" "$menu"; fail "Unable to update the default directive; the backup was restored."; }
    fi

    # Post-write verification: the default directive must name the canonical
    # entry (the directive is zero-based) and the menu must still cover it.
    grep -qE "^[[:space:]]*default[[:space:]]+$entry([[:space:]]|$)" "$menu" \
        || { cp -a -- "$SESSION_DIR/menu.lst.before-host-default" "$menu"; fail "The default directive verification failed; the backup was restored."; }
    [[ "$(grep -c '^[[:space:]]*title' "$menu")" -gt "$entry" ]] \
        || { cp -a -- "$SESSION_DIR/menu.lst.before-host-default" "$menu"; fail "The menu no longer covers the canonical entry; the backup was restored."; }

    after="$(repair_file_fingerprint "$menu")"
    if [[ "$before" == "$after" ]]; then
        repair_change_status host-default "unchanged|the default entry was already the canonical installed kernel entry"
    else
        repair_change_status host-default changed
    fi
    log "PASS: Make Default set the GRUB-legacy default to the canonical installed kernel entry." | tee -a "$SESSION_LOG"
}

run_host_repair()
{
    local i=0 stage="" need_maintenance=0
    local -a args=("$@")
    # Package stages write only through the target package manager.  On a
    # BIOS-only legacy host there are no firmware variables to isolate, so
    # they do not need the unshare-based host command guard; every other host
    # stage keeps the host-maintenance gate.
    for ((i = 2; i < ${#args[@]}; i++)); do
        stage="${args[$i]}"
        case "$stage" in
            dpkg-configure|fix-broken|apt-update|apt-upgrade|--post-efi) ;;
            *) need_maintenance=1; break ;;
        esac
    done
    if (( need_maintenance == 1 )); then
        legacy_require_feature host-maintenance
    fi
    run_host_repair_modern "${args[@]:-}"
}

run_host_reboot()
{
    legacy_require_feature host-maintenance
    run_host_reboot_modern "$@"
}

# Read-only running-host commands mount nothing and only probe the live
# system, so they work on Etch without unshare.  The host-maintenance gate
# stays on every write action (repair, shell, default, snapshots, reboot).
run_host_diagnostic()
{
    run_host_diagnostic_modern "$@"
}

validate_running_host()
{
    validate_running_host_modern "$@"
}

fs_inspect()
{
    fs_inspect_modern "$@"
}

fs_repair()
{
    if (( RUNNING_HOST_MODE == 1 )); then
        legacy_require_feature host-maintenance
    fi
    fs_repair_modern "$@"
}

# ---------------------------------------------------------------------------
# Mount special: rbind without mount-namespace propagation on Etch
# ---------------------------------------------------------------------------

mount_special()
{
    local kind="$1" source="$2" destination="$3" root_real="" parent_real=""
    case "$kind" in
        rbind|rbind-ro)
            root_real="$(realpath_existing "$TARGET_ROOT" 2>/dev/null)" \
                || fail "Unable to resolve target root before mounting $kind."
            [[ "$destination" == "$TARGET_ROOT/"* && ! -L "$destination" ]] \
                || fail "Refusing to mount $kind through an unsafe target path: $destination"
            parent_real="$(realpath_existing "$(dirname -- "$destination")" 2>/dev/null)" \
                || fail "Unable to resolve target mount parent: $destination"
            path_within "$parent_real" "$root_real" \
                || fail "Target mount parent escapes the selected root: $destination"
            mkdir -p -- "$destination"
            [[ ! -L "$destination" ]] || fail "Refusing to mount $kind through a target symlink: $destination"
            legacy_mount_special "$kind" "$source" "$destination" \
                || fail "Unable to mount $kind at $destination."
            MOUNTS+=("$destination")
            ;;
        *)
            mount_special_modern "$@"
            ;;
    esac
}
