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
    if [[ -f "$TARGET_ROOT/etc/debian_version" ]] \
        && { [[ -f "$TARGET_ROOT/var/lib/dpkg/status" ]] || [[ -f "$TARGET_ROOT/var/lib/dpkg/status-old" ]]; }; then
        version="$(head -n1 "$TARGET_ROOT/etc/debian_version" 2>/dev/null || true)"
        TARGET_OS_ID="debian"
        TARGET_OS_LIKE=""
        TARGET_PRETTY="Debian (legacy root ${version:-unknown}, /etc/debian_version + dpkg database)"
        log "Legacy root evidence accepted: /etc/debian_version + dpkg database (no /etc/os-release present)." | tee -a "$SESSION_LOG"
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
    fail "Mounted filesystem does not contain /etc/os-release and no legacy root evidence was found (/etc/debian_version + dpkg database, /etc/redhat-release + rpm database, /etc/SuSE-release + rpm database; probed label: $label); root selection is not confirmed."
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
    if ! grub2_layout_detected && ! legacy_target_efi_evidence; then
        printf 'boot-stack reconciliation requires a GRUB2 or EFI boot path; the detected GRUB legacy BIOS layout is not supported'
        return 1
    fi
    bootstack_unavailable_reason_modern
}

# ---------------------------------------------------------------------------
# Legacy feature gating (probe-based; no distribution prefilter)
# ---------------------------------------------------------------------------

legacy_host_maintenance_reason()
{
    if [[ -z "$(legacy_real_tool_path unshare)" ]]; then
        printf 'unshare is not installed in the recovery environment'
        return 1
    fi
    if [[ -z "$(legacy_real_tool_path timeout)" ]]; then
        printf 'timeout is not installed in the recovery environment'
        return 1
    fi
    return 0
}

legacy_feature_reason()
{
    local feature="$1" fstype="" rsync_help=""
    case "$feature" in
        file-copy)
            if [[ -z "$(legacy_real_tool_path rsync)" ]]; then
                printf 'rsync is not installed in the recovery environment'
                return 1
            fi
            rsync_help="$(rsync --help 2>&1 || true)"
            case "$rsync_help" in
                *--chown*) ;;
                *)
                    printf 'the installed rsync does not support --chown (verified file copy needs rsync >= 3.0)'
                    return 1
                    ;;
            esac
            return 0
            ;;
        shell)
            if [[ -z "$(legacy_real_tool_path chroot)" ]]; then
                printf 'chroot is not installed in the recovery environment'
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
            return 0
            ;;
        host-shell)
            legacy_host_maintenance_reason
            return $?
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
# report is appended so the 13 keys and their evidence lines stay untouched.
diagnostic_repair_capabilities()
{
    diagnostic_repair_capabilities_modern "$@"
    legacy_feature_gating_report
}

# ---------------------------------------------------------------------------
# Gated command entry points
# ---------------------------------------------------------------------------

run_file_copy()
{
    legacy_require_feature file-copy
    run_file_copy_modern "$@"
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
    legacy_require_feature host-maintenance
    legacy_require_feature host-default
    run_host_default_modern "$@"
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
