# legacy/overlay.sh — legacy-only helper behaviour.
# shellcheck shell=bash
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

# A10-05 cancel surface.  The Qt3 GUI passes --cancel-file <path> (parsed by
# main() before the command verb) and its cancel() touches that file; the
# documented token form is CANCEL_TOKEN=<value> (environment) or
# --cancel-token <value>, checked against $SESSION_DIR/cancel.  Both may also
# arrive through the environment (an env CANCEL_TOKEN survives a direct
# invocation; sudo strips it, which is why the GUI uses the argv form).
CANCEL_FILE="${CANCEL_FILE:-}"
CANCEL_TOKEN="${CANCEL_TOKEN:-}"

# Set by read_target_os when the root was confirmed through legacy release
# evidence instead of /etc/os-release.  Evidence wording only; never a gate.
TARGET_OS_LEGACY=0

# A12-02: the helper runs with whatever PATH its launcher inherited.  sudo's
# secure_path normally carries the sbin directories, but a root-launched GUI
# (`sudo env ...`), `--no-elevate` or the BOOT_REPAIR_LEGACY_ELEVATE escape
# hatch can start the helper with a desktop PATH that lacks them, and on Etch
# cryptsetup (plus tune2fs, dmsetup, update-grub and the LVM tooling) lives
# in /sbin and /usr/sbin.  The standard system directories are prepended when
# missing (the helper is the root-only backend, so this only ever widens the
# lookup to the fixed system locations) and PATH is exported so every
# inherited `command -v`/`need`/`type -P` probe in the ported body resolves
# the same tools on every launch path.
case ":$PATH:" in
    *:/sbin:*) ;;
    *) PATH="/sbin:$PATH" ;;
esac
case ":$PATH:" in
    *:/usr/sbin:*) ;;
    *) PATH="/usr/sbin:$PATH" ;;
esac
export PATH
# The explicit cryptsetup gates additionally probe the standard locations
# through legacy_standard_tool_path (compat.sh, A12-02), so they never depend
# on the normalized PATH alone.

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
LEGACY_GRUB_MANAGED=""
LEGACY_GRUB_VARIANT_MANIFEST=""

legacy_grub_legacy_target()
{
    [[ -f "$TARGET_ROOT/boot/grub/menu.lst" ]] || return 1
    target_has_executable /usr/sbin/update-grub /usr/bin/update-grub || return 1
    target_has_executable /usr/sbin/grub-mkconfig /usr/bin/grub-mkconfig && return 1
    return 0
}

# Escape a token for a BRE (sed) pattern. The bracket class carries the
# literal `]` first (the POSIX idiom: GNU sed/BRE has no escaped-] form) and
# the backslash immediately after it (a backslash cannot be the first member
# while `]` stays in the class: `[\\]` closes the bracket expression, so any
# class starting with `\` loses the `]` member). A9-06: the backslash and `|`
# are now members too - `|` is the delimiter the call sites use, so a raw `|`
# in a token would terminate the s-command pattern.
legacy_grub_sed_escape()
{
    printf '%s' "$1" | sed 's/[]\.*^$|[]/\\&/g'
}

# Escape a token for an ERE (grep -E) pattern. Same bracket-expression
# constraint as legacy_grub_sed_escape: `]` first, backslash second.
legacy_grub_grep_escape()
{
    printf '%s' "$1" | sed 's/[]\.[*^$+?|(){}]/\\&/g'
}

# The kernel-line arguments managed by the menu.lst defoptions/kopt comments
# (Etch's update-grub expands these into the generated kernel lines).
legacy_grub_managed_options()
{
    local config="$1" line="" rest="" combined=""
    [[ -s "$config" ]] || return 0
    while IFS= read -r line; do
        [[ "$line" == \#* ]] || continue
        line="${line#\#}"
        line="${line# }"
        case "$line" in
            defoptions=*|kopt=*)
                rest="${line#*=}"
                rest="$(printf '%s' "$rest" | tr '\t' ' ')"
                rest="$(printf '%s' "$rest" | sed 's/^ *//; s/ *$//')"
                if [[ -n "$rest" ]]; then
                    combined="$combined $rest"
                fi
                ;;
        esac
    done < "$config" 2>/dev/null || true
    printf '%s' "$combined" | sed 's/^ *//; s/ *$//'
}

# Normalized title/kernel/initrd/root/module declarations.  These are the
# boot-critical identity of a GRUB legacy menu; GRUB_DISTRIBUTOR and title
# wording are not trusted as entry identity.  The defoptions/kopt-managed
# arguments are removed from kernel lines so an update-grub regeneration that
# expands them is not mistaken for entry removal.
legacy_grub_entry_declarations()
{
    local config="$1" managed="${2:-}" line="" normalized="" token="" escaped=""
    [[ -s "$config" ]] || return 0
    legacy_sed_ext -n '/^[[:space:]]*(title|kernel|initrd|root|module)[[:space:]]/p' "$config" 2>/dev/null \
        | legacy_sed_ext 's/[[:space:]]+/ /g; s/^ //; s/ $//' \
        | while IFS= read -r line; do
            if [[ "$line" == kernel\ * && -n "$managed" ]]; then
                normalized="$line"
                # A9-12: disable globbing while the managed tokens are split,
                # so an argument carrying wildcard characters is never
                # expanded against the working directory.
                set -f
                for token in $managed; do
                    escaped="$(legacy_grub_sed_escape "$token")"
                    normalized="$(printf '%s' "$normalized" \
                        | sed "s| $escaped | |g; s| $escaped\$||; s|^$escaped ||")"
                done
                set +f
                printf '%s\n' "$normalized"
            else
                printf '%s\n' "$line"
            fi
        done \
        | LC_ALL=C sort -u
}

# The kernel lines of a menu.lst (normalized whitespace), for the positive
# defoptions-carry check.
legacy_grub_kernel_lines()
{
    local config="$1"
    [[ -s "$config" ]] || return 0
    legacy_sed_ext -n '/^[[:space:]]*kernel[[:space:]]/p' "$config" 2>/dev/null \
        | legacy_sed_ext 's/[[:space:]]+/ /g; s/^ //; s/ $//'
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
    LEGACY_GRUB_MANAGED="$(legacy_grub_managed_options "$TARGET_ROOT/boot/grub/menu.lst")"
    legacy_grub_entry_declarations "$TARGET_ROOT/boot/grub/menu.lst" "$LEGACY_GRUB_MANAGED" > "$LEGACY_GRUB_DECLARATIONS"
    if [[ -n "$LEGACY_GRUB_MANAGED" ]]; then
        log "SIMULATE/PREFLIGHT: GRUB legacy defoptions/kopt-managed arguments captured: $LEGACY_GRUB_MANAGED" | tee -a "$SESSION_LOG"
    fi
    # A12-04: capture every derived single-user variant (an entry block whose
    # kernel line differs from a main entry only by the standalone `single`
    # token plus defoptions-managed arguments) so the regeneration can restore
    # it; stock Etch update-grub does not apply defoptions to altoptions
    # alternatives, so the variant's own serial-console arguments would be
    # stripped or the whole entry dropped.
    LEGACY_GRUB_VARIANT_MANIFEST="$SESSION_DIR/grub-single-variants.list"
    legacy_grub_single_variant_capture "$TARGET_ROOT/boot/grub/menu.lst" "$SESSION_DIR"
    if [[ -s "$LEGACY_GRUB_VARIANT_MANIFEST" ]]; then
        log "SIMULATE/PREFLIGHT: GRUB legacy single-user variant(s) captured for post-regeneration restoration ($(grep -c . "$LEGACY_GRUB_VARIANT_MANIFEST") variant(s))." | tee -a "$SESSION_LOG"
    fi
    log "SIMULATE/PREFLIGHT: GRUB legacy menu.lst backed up to $LEGACY_GRUB_BACKUP and entry declarations captured." | tee -a "$SESSION_LOG"
}

# A12-04: emit the entry blocks of a GRUB legacy config (paragraph mode),
# separated by the ASCII file-separator character so bash `read -d` can
# iterate them without splitting on the blocks' embedded newlines.
legacy_grub_entry_blocks()
{
    local config="$1"
    [[ -s "$config" ]] || return 0
    awk '
        BEGIN { RS = ""; ORS = "\n\034" }
        { print }
    ' "$config"
}

# A12-04: the first kernel line of one entry block, whitespace-normalized
# (kernel path + arguments, one space separator, no leading/trailing space).
legacy_grub_block_kernel()
{
    local block="$1"
    printf '%s\n' "$block" | awk '
        /^[ \t]*kernel[ \t]/ {
            line = $0
            sub(/^[ \t]*kernel[ \t]+/, "", line)
            gsub(/[ \t]+/, " ", line)
            gsub(/^ | $/, "", line)
            print line
            exit
        }'
}

# A12-04: the same normalized kernel line with every standalone `single`
# token removed (the parent form of a single-user variant).
legacy_grub_kernel_without_single()
{
    local line="$1"
    line=" $line "
    while [[ "$line" == *" single "* ]]; do
        line="${line/ single / }"
    done
    line="${line# }"
    printf '%s\n' "${line% }"
}

# A12-04: strip the defoptions/kopt-managed tokens from ONE normalized kernel
# line (the same per-line expansion legacy_grub_entry_declarations applies).
legacy_grub_kernel_strip_managed()
{
    local line="$1" managed="${2:-}" token="" escaped=""
    [[ -n "$managed" ]] || { printf '%s\n' "$line"; return 0; }
    # A9-12: noglob around the managed-token split; restored on every exit.
    set -f
    for token in $managed; do
        escaped="$(legacy_grub_sed_escape "$token")"
        line="$(printf '%s' "$line" \
            | sed "s| $escaped | |g; s| $escaped\$||; s|^$escaped ||")"
    done
    set +f
    printf '%s\n' "$line"
}

# A12-04: capture every derived single-user variant of the current menu.lst.
# A variant is an entry block whose kernel line carries the standalone
# `single` token and whose parent (the same line minus `single`) exists as a
# kernel line of another block.  Each variant block is stored verbatim under
# $outdir/grub-single-variant.<n> and recorded in the manifest
# (parent-kernel-line<TAB>variant-file per line) for the post-regeneration
# restoration; blocks without a parent (or with an unproven parent) are left
# for the guard, which refuses when update-grub drops them.
legacy_grub_single_variant_capture()
{
    local config="$1" outdir="$2" manifest="" parents_file="" block="" kline="" parent="" i=0
    manifest="$outdir/grub-single-variants.list"
    [[ -s "$config" && -d "$outdir" ]] || return 0
    : > "$manifest" || return 1
    parents_file="$outdir/grub-kernels.parents"
    # Pass 1: the non-single kernel lines are the candidate parents.
    while IFS= read -r -d $'\034' block; do
        kline="$(legacy_grub_block_kernel "$block")"
        [[ -n "$kline" ]] || continue
        case " $kline " in
            *" single "*) ;;
            *) printf '%s\n' "$kline" ;;
        esac
    done < <(legacy_grub_entry_blocks "$config") > "$parents_file"
    # Pass 2: record every single-variant block whose parent exists.
    i=0
    while IFS= read -r -d $'\034' block; do
        kline="$(legacy_grub_block_kernel "$block")"
        [[ -n "$kline" ]] || continue
        case " $kline " in
            *" single "*) ;;
            *) continue ;;
        esac
        parent="$(legacy_grub_kernel_without_single "$kline")"
        [[ -n "$parent" ]] || continue
        grep -Fqx -- "$parent" "$parents_file" || continue
        i=$((i + 1))
        printf '%s' "$block" > "$outdir/grub-single-variant.$i"
        printf '%s\t%s\n' "$parent" "$outdir/grub-single-variant.$i" >> "$manifest"
    done < <(legacy_grub_entry_blocks "$config")
    rm -f -- "$parents_file"
    return 0
}

# A12-04: restore the captured single-user variants into a freshly
# regenerated menu.lst.  For every captured variant whose parent kernel line
# still exists in the regenerated file, the update-grub-generated alternative
# that immediately follows the parent block is replaced with the captured
# block (or the block is inserted right after the parent when update-grub
# generated no alternative).  A variant whose parent disappeared is left out;
# the repair verification and the entry-preservation guard then refuse the
# regeneration.  POSIX awk only (mawk 1.3.3): no gensub, no POSIX classes.
legacy_grub_restore_single_variants()
{
    local config="$1" manifest="$2" tmp=""
    [[ -s "$config" ]] || return 1
    [[ -s "$manifest" ]] || return 0
    tmp="$(mktemp "${TMPDIR:-/tmp}/grub-restore.XXXXXX")" || return 1
    LEGACY_GRUB_VARIANT_MANIFEST="$manifest" awk '
        function emit_all_pending(    k, vtext, vline) {
            for (k = 1; k <= npending; k++) {
                vtext = ""
                while ((getline vline < vfiles[plist[k]]) > 0) {
                    vtext = vtext vline "\n"
                }
                close(vfiles[plist[k]])
                if (vtext != "") {
                    printf "%s", vtext
                    printf "\n"
                }
            }
            npending = 0
        }
        BEGIN {
            RS = ""
            ORS = "\n\n"
            manifest = ENVIRON["LEGACY_GRUB_VARIANT_MANIFEST"]
            np = 0
            if (manifest != "") {
                while ((getline chunk < manifest) > 0) {
                    n = split(chunk, entries, "\n")
                    for (e = 1; e <= n; e++) {
                        if (entries[e] == "") continue
                        f = split(entries[e], parts, "\t")
                        if (f >= 2) {
                            np++
                            parents[np] = parts[1]
                            vfiles[np] = parts[2]
                        }
                    }
                }
                close(manifest)
            }
            npending = 0
        }
        {
            block = $0
            kline = ""
            n = split(block, lines, "\n")
            for (i = 1; i <= n; i++) {
                if (lines[i] ~ /^[ \t]*kernel[ \t]/) {
                    s = lines[i]
                    sub(/^[ \t]*kernel[ \t]+/, "", s)
                    gsub(/[ \t]+/, " ", s)
                    gsub(/^ | $/, "", s)
                    kline = s
                    break
                }
            }
            is_single = (kline ~ /(^| )single( |$)/)
            if (npending > 0) {
                if (is_single) {
                    # update-grub generated alternative: replace it with the
                    # first pending captured variant.
                    emit_all_pending()
                    next
                }
                # The block after the parent is not an alternative: emit the
                # remaining captured variants right after the parent block,
                # then fall through to the current block.
                emit_all_pending()
            }
            print block
            for (j = 1; j <= np; j++) {
                if (kline != "" && kline == parents[j]) {
                    npending++
                    plist[npending] = j
                }
            }
        }
        END {
            if (npending > 0) { emit_all_pending() }
        }
    ' "$config" > "$tmp" || { rm -f -- "$tmp"; return 1; }
    if ! mv -- "$tmp" "$config"; then
        rm -f -- "$tmp"
        return 1
    fi
    return 0
}

# A12-04: hard verification that every captured single-user variant is
# present (declaration-by-declaration, managed tokens stripped) in the
# regenerated declarations.  A variant whose parent survived regeneration
# must have been restored by legacy_grub_restore_single_variants; a missing
# declaration here fails the repair (rollback), so a variant can never be
# silently dropped while its parent survives.
legacy_grub_variants_verified()
{
    local manifest="$1" after_declarations="$2" parent="" variant_file="" line=""
    local missing_file="$SESSION_DIR/grub-variants.missing"
    [[ -s "$manifest" ]] || return 0
    : > "$missing_file" || return 1
    while IFS=$'\t' read -r parent variant_file; do
        [[ -n "$parent" && -s "$variant_file" ]] || continue
        legacy_grub_entry_declarations "$variant_file" "$LEGACY_GRUB_MANAGED" \
            | while IFS= read -r line; do
                grep -Fqx -- "$line" "$after_declarations" \
                    || printf '%s\n' "$line" >> "$missing_file"
            done
    done < "$manifest"
    if [[ -s "$missing_file" ]]; then
        log "ERROR: the legacy repair could not restore a derived single-user variant whose parent entry survived regeneration; the target configuration was rolled back." | tee -a "$SESSION_LOG"
        sed 's/^/  derived-single-variant-missing: /' "$missing_file" | tee -a "$SESSION_LOG"
        return 1
    fi
    return 0
}

# A12-04: split a missing-declarations list into a fatal remainder (printed
# to $fatal) and derived single-user variants (logged): a missing declaration
# line that belongs to a captured variant whose parent kernel line (managed
# tokens stripped) survives in the regenerated declarations is a derived
# variant of a preserved boot entry, not a removed entry.  Every other
# missing declaration stays fatal, so genuinely distinct entries keep the
# fail-closed protection.
legacy_grub_relax_derived_variants()
{
    local missing="$1" after_keys="$2" manifest="${3:-}" fatal="$4"
    local line="" parent="" variant_file="" relaxed=0
    : > "$fatal" || return 1
    while IFS= read -r line; do
        [[ -n "$line" ]] || continue
        relaxed=0
        if [[ -s "$manifest" ]]; then
            while IFS=$'\t' read -r parent variant_file; do
                [[ -n "$parent" && -s "$variant_file" ]] || continue
                # The manifest stores the parent as its kernel ARGUMENTS line;
                # after_keys holds full declarations, so compare with the
                # `kernel ` prefix and the managed tokens stripped.
                grep -Fqx -- "kernel $(legacy_grub_kernel_strip_managed "$parent" "$LEGACY_GRUB_MANAGED")" "$after_keys" \
                    || continue
                if legacy_grub_entry_declarations "$variant_file" "$LEGACY_GRUB_MANAGED" \
                    | grep -Fqx -- "$line"; then
                    relaxed=1
                    break
                fi
            done < "$manifest"
        fi
        if (( relaxed == 1 )); then
            log "GRUB legacy derived single-user variant declaration (parent entry preserved): $line" | tee -a "$SESSION_LOG"
        else
            printf '%s\n' "$line" >> "$fatal"
        fi
    done < "$missing"
    return 0
}

legacy_grub_guard_entries_preserved()
{
    local before="$1" after="$2" after_keys="$SESSION_DIR/grub-menu-declarations.after"
    local missing="$SESSION_DIR/grub-menu-declarations.missing"
    local token="" escaped="" kernel_lines=""
    [[ -s "$before" ]] || return 0
    # Cycle 12: compare with the defoptions/kopt-managed arguments stripped
    # from the kernel lines, so a regeneration that expands the menu's own
    # defoptions/kopt comments is not mistaken for entry removal.
    legacy_grub_entry_declarations "$after" "$LEGACY_GRUB_MANAGED" > "$after_keys"
    comm -23 "$before" "$after_keys" > "$missing" || true
    if [[ -s "$missing" ]]; then
        # A12-04: a missing declaration that belongs to a derived single-user
        # variant whose parent entry survived regeneration is a derived
        # variant, not a removed boot entry (the legacy repair restores it);
        # every other missing declaration stays fatal.
        legacy_grub_relax_derived_variants "$missing" "$after_keys" \
            "$LEGACY_GRUB_VARIANT_MANIFEST" "$missing.fatal"
        if [[ -s "$missing.fatal" ]]; then
            log "ERROR: GRUB legacy regeneration would remove existing menu declarations; the target configuration was rolled back." | tee -a "$SESSION_LOG"
            sed 's/^/  preserved-declaration-required: /' "$missing.fatal" | tee -a "$SESSION_LOG"
            return 1
        fi
    fi
    # Positive check: the regenerated kernel lines must carry every
    # defoptions/kopt-managed argument (update-grub expands them).
    if [[ -n "$LEGACY_GRUB_MANAGED" ]]; then
        kernel_lines="$(legacy_grub_kernel_lines "$after")"
        # A9-12: noglob around the managed-token split (see the declarations
        # loop); restored on both exits so the caller never inherits it.
        set -f
        for token in $LEGACY_GRUB_MANAGED; do
            escaped="$(legacy_grub_grep_escape "$token")"
            if ! printf '%s\n' "$kernel_lines" | grep -qE "(^|[[:space:]])$escaped([[:space:]]|$)"; then
                log "ERROR: GRUB legacy regeneration dropped the defoptions/kopt-managed argument '$token'; the target configuration was rolled back." | tee -a "$SESSION_LOG"
                set +f
                return 1
            fi
        done
        set +f
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
    # A12-04: restore the captured single-user variants.  Stock Etch
    # update-grub regenerates altoptions single entries WITHOUT the
    # defoptions-managed arguments (defoptions apply "to the default boot
    # option, but not with the alternatives"), so a manually maintained
    # single-user entry carrying the serial-console arguments would otherwise
    # be stripped or dropped.  The captured blocks are re-inserted verbatim
    # next to their parent entries (replacing the regenerated alternative); a
    # variant whose parent did not survive is left out and the verification
    # below refuses the regeneration.
    legacy_grub_restore_single_variants "$TARGET_ROOT/boot/grub/menu.lst" "$LEGACY_GRUB_VARIANT_MANIFEST" \
        || { cp -a -- "$LEGACY_GRUB_BACKUP" "$TARGET_ROOT/boot/grub/menu.lst"
             fail "GRUB legacy single-user variant restoration failed; /boot/grub/menu.lst was restored."; }
    legacy_grub_entry_declarations "$TARGET_ROOT/boot/grub/menu.lst" "$LEGACY_GRUB_MANAGED" \
        > "$SESSION_DIR/grub-menu-declarations.after.restored"
    if ! legacy_grub_variants_verified "$LEGACY_GRUB_VARIANT_MANIFEST" \
        "$SESSION_DIR/grub-menu-declarations.after.restored"; then
        cp -a -- "$LEGACY_GRUB_BACKUP" "$TARGET_ROOT/boot/grub/menu.lst"
        fail "GRUB legacy regeneration was rolled back because a derived single-user variant could not be restored."
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
    legacy_cancel_stage_check
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
    # A10-05: the cancel surface is checked between the three components.
    legacy_cancel_stage_check
    adaptive_initramfs_repair
    legacy_cancel_stage_check
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
    legacy_cancel_stage_check
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
# Cancel token (A10-05)
# ---------------------------------------------------------------------------
# The helper-side cancel surface: the Qt3 GUI passes --cancel-file <path>
# (parsed by main() before the command verb) and its cancel() touches that
# file; the documented token form is CANCEL_TOKEN=<value> (environment or
# --cancel-token <value>), checked against $SESSION_DIR/cancel.  A small
# watcher polls the surface between repair stages and on shell-command ticks
# and aborts with the modern bounded TERM -> KILL tree escalation of the
# running command; it also reaps that command when the helper itself dies
# (poll `kill -0 $PPID`), so a SIGKILLed helper never leaves a shell command
# behind.  Etch deviation (documented): the GUI's SIGKILL reaches only the
# elevation wrapper (sudo), so a helper inside a long single stage finishes
# that stage and aborts at the next boundary instead of being interrupted.

# True when the configured cancel surface signals a cancel request.
# CANCEL_FILE: existence is the signal (the GUI's cancel() touches the file).
# CANCEL_TOKEN: $SESSION_DIR/cancel must exist AND carry exactly the token,
# so a stray file in the session directory can never abort a run.
legacy_cancel_requested()
{
    local cancel_path="" line=""
    if [[ -n "$CANCEL_FILE" ]]; then
        [[ -e "$CANCEL_FILE" ]] || return 1
        return 0
    fi
    if [[ -n "$CANCEL_TOKEN" && -n "${SESSION_DIR:-}" ]]; then
        cancel_path="$SESSION_DIR/cancel"
        [[ -e "$cancel_path" ]] || return 1
        line="$(head -n1 "$cancel_path" 2>/dev/null || true)"
        [[ "$line" == "$CANCEL_TOKEN" ]]
        return $?
    fi
    return 1
}

# Stage-boundary check: abort (fail -> exit 1 -> cleanup) when a cancel was
# requested.  Runs at the entry of every repair stage wrapper and between the
# legacy boot-stack components.
legacy_cancel_stage_check()
{
    if legacy_cancel_requested; then
        log "Cancel requested; aborting at the stage boundary (the session teardown unmounts everything)." | tee -a "$SESSION_LOG" >&2
        fail "Cancelled at the caller's request."
    fi
    return 0
}

# Shell-command tick watcher: polls the cancel surface every 0.2 s while
# `child` (the backgrounded command pipeline) runs.  On a cancel request - or
# when the helper itself died (kill -0 $PPID) - the child is terminated with
# the modern bounded TERM -> KILL escalation.
legacy_cancel_watcher()
{
    local child="${1:-}"
    [[ -n "$child" ]] || return 0
    while kill -0 "$child" 2>/dev/null; do
        if ! kill -0 "$PPID" 2>/dev/null; then
            log "Helper parent died; reaping the running shell command (bounded TERM then KILL)." | tee -a "$SESSION_LOG" >&2 || true
            terminate_helper_tree "$child" || true
            return 0
        fi
        if legacy_cancel_requested; then
            log "Cancel token observed; terminating the running shell command (bounded TERM then KILL)." | tee -a "$SESSION_LOG" >&2 || true
            terminate_helper_tree "$child" || true
            return 0
        fi
        sleep 0.2 2>/dev/null || return 0
    done
    return 0
}

# A10-05 stage entry wrappers: the wrapped modern definitions were renamed by
# port.sh; every stage entry (package stages, initramfs and GRUB) checks the
# cancel surface before running, which is what "between repair stages" means
# for the legacy Full Repair plan.
run_package_stage()
{
    legacy_cancel_stage_check
    run_package_stage_modern "$@"
}

adaptive_initramfs_repair()
{
    legacy_cancel_stage_check
    adaptive_initramfs_repair_modern
}

adaptive_grub_stage()
{
    legacy_cancel_stage_check
    adaptive_grub_stage_modern "$@"
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
    local entry="" canonical=""
    [[ -r "$TARGET_ROOT/etc/X11/default-display-manager" ]] || return 1
    while read -r entry _ignored; do
        [[ -n "$entry" && "${entry:0:1}" != "#" ]] || continue
        case "$entry" in
            /usr/bin/*|/usr/sbin/*) ;;
            *) return 1 ;;
        esac
        # A9-10: refuse entries whose lexical canonicalization differs from
        # the raw token (. / .. / // segments, e.g. /usr/bin/../sbin/kdm).
        # legacy_realpath -m resolves the segments without touching the
        # filesystem; an uncanonical token (or a failing resolver) is refused.
        canonical="$(legacy_realpath -m "$entry" 2>/dev/null || true)"
        [[ -n "$canonical" && "$canonical" == "$entry" ]] || return 1
        printf '%s\n' "$entry"
        return 0
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
    legacy_cancel_stage_check
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

# NOTE (A11-08): run_chroot_shell / run_host_shell are defined exactly once
# each, further down, where they branch on HOST_COMMAND_GUARD.  An earlier
# plain-delegate pair used to sit here and was shadowed by those definitions
# (dead code, removed); port.sh's verify_function_definitions now fails the
# build if any function name is ever defined twice again.

# Legacy mount-options filter.  The generated helper adds `noload` to the
# read-only ext2/ext3/ext4 mounts (the root ro mount, the boot-entry ro mount
# and the os-release probe mount all funnel through mount_recorded); Etch's
# util-linux 2.12r can reject `ro,noload` ("ext3: No journal on filesystem on
# dm-5" + "wrong fs type, bad option, bad superblock") while plain `ro` mounts
# the same filesystem fine.
#
# B6/A9-05: earlier behaviour stripped `noload` unconditionally.  That hides a
# dirty journal: a plain `ro` mount of a dirty ext filesystem replays the
# journal, silently turning the "read-only" mount into a modifying one.  The
# filter now keeps `noload` for the FIRST attempt; mount_recorded retries with
# the stripped options only when that first mount FAILS, after the tune2fs
# journal-state check (a dirty journal logs a WARNING naming the replay risk
# and the retry still proceeds - best-effort fail-soft; see mount_recorded).
# The xfs `norecovery` option is left as-is: no xfs filesystem exists on Etch,
# so the option is simply never exercised.
legacy_filter_mount_options()
{
    printf '%s' "$1"
}

# Strip exactly the `noload` option from a comma-separated mount-option list
# (the B6/A9-05 retry form).
legacy_mount_options_without_noload()
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

    # B6/A9-05: the first attempt keeps `noload` (see the filter above).  The
    # retry branch below applies to ext2/3/4 sources only: the generated
    # helper adds `noload` to no other mount, so its presence IS the ext gate;
    # non-ext and modern paths are unchanged.
    local rc=0 had_noload=0
    local -a retry_args=()
    local i=0 retry_value=""
    for ((i = 0; i < ${#args[@]}; i++)); do
        opt="${args[$i]}"
        case "$opt" in
            -o)
                retry_value="${args[$((i + 1))]:-}"
                if [[ ",$retry_value," == *,noload,* ]]; then
                    had_noload=1
                    retry_value="$(legacy_mount_options_without_noload "$retry_value")"
                fi
                [[ -n "$retry_value" ]] && retry_args+=("-o" "$retry_value")
                i=$((i + 1))
                ;;
            -o*)
                retry_value="${opt#-o}"
                if [[ ",$retry_value," == *,noload,* ]]; then
                    had_noload=1
                    retry_value="$(legacy_mount_options_without_noload "$retry_value")"
                fi
                [[ -n "$retry_value" ]] && retry_args+=("-o$retry_value")
                ;;
            *)
                retry_args+=("$opt")
                ;;
        esac
    done
    mount_recorded_modern "$source" "$destination" "${args[@]:-}" || rc=$?
    if (( rc == 0 || had_noload == 0 )); then
        return "$rc"
    fi

    # The noload-carrying first mount failed.  Consult the journal state
    # before retrying without noload: a clean journal makes the stripped
    # retry safe; a dirty journal means the stripped retry replays the journal
    # during a nominally read-only mount.  The dirty (and unknown) case logs a
    # WARNING naming that risk and PROCEEDS - never a hard refusal: the
    # 2.6.18/Etch behaviour is unconfirmed until the rig drill, so the retry
    # keeps the mount reachable while the risk stays explicit in the log.
    # TODO(rig): confirm Etch util-linux 2.12r mount/tune2fs output and, if
    # the risk is real there, tighten this gate.
    local journal_state="" tune2fs_output=""
    tune2fs_output="$(tune2fs -l -- "$source" 2>/dev/null || true)"
    journal_state="$(printf '%s\n' "$tune2fs_output" \
        | sed -n 's/^Filesystem state:[[:space:]]*//p' | head -n1)"
    if [[ "$journal_state" == "clean" ]]; then
        log "Read-only mount with noload failed for $source; the journal is clean, retrying with plain ro"
    else
        log "WARNING: read-only mount with noload failed for $source and the journal state is not clean (${journal_state:-unknown}); retrying with plain ro may replay the target's journal during a nominally read-only mount"
    fi
    # rc still holds the failed first attempt: a successful retry must return 0.
    rc=0
    mount_recorded_modern "$source" "$destination" "${retry_args[@]:-}" || rc=$?
    return "$rc"
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

# Read the LUKS passphrase from stdin, strip exactly one trailing CR/LF
# (the GUI writes the raw passphrase and closes stdin; a scripted caller may
# append a newline that cryptsetup 1.0 would otherwise treat as part of the
# key), and write it to a mode-600 keyfile under the session state. Prints the
# keyfile path; the caller deletes the keyfile after the open attempt. An
# empty passphrase keeps the cryptsetup rc-2 semantics (UNLOCK_AUTH_FAILED).
legacy_unlock_keyfile_from_stdin()
{
    local keyfile="${1:-}" passphrase=""
    [[ -n "$keyfile" ]] || fail "Internal unlock keyfile error."
    # A9-14: cap the first line at 1024 characters.  `read -n` stops at the
    # cap even when the line continues, so a full-cap read proves the line
    # was truncated (the exact-1024 case is refused too - the boundary is
    # indistinguishable from truncation).
    IFS= read -r -n 1024 passphrase || true
    if [[ ${#passphrase} -eq 1024 ]]; then
        printf 'UNLOCK_AUTH_FAILED=1\n' >&2
        fail "The LUKS passphrase is longer than 1024 characters; refusing."
    fi
    passphrase="${passphrase%$'\r'}"
    if [[ -z "$passphrase" ]]; then
        printf 'UNLOCK_AUTH_FAILED=1\n' >&2
        fail "An empty LUKS passphrase was received on stdin."
    fi
    # A9-02: create the session keyfile umask-insensitively so no
    # world-readable window exists between creation and the chmod.
    ( umask 077; : > "$keyfile" ) || fail "Cannot create the session keyfile."
    chmod 600 "$keyfile" || fail "Cannot restrict the session keyfile."
    printf '%s' "$passphrase" > "$keyfile" || fail "Cannot write the session keyfile."
    passphrase=""
    printf '%s\n' "$keyfile"
}

# Read the LUKS passphrase from the GUI's mode-600 keyfile argument
# (Qt 3.3.7 QProcess cannot deliver stdin), strip exactly one trailing CR/LF
# and write it to the mode-600 session keyfile. The caller deletes the
# session keyfile after the open attempt; the argument file itself is NEVER
# deleted here (the Qt3 GUI unlinks its own file when the command finishes).
legacy_unlock_keyfile_from_file()
{
    local source="${1:-}" keyfile="${2:-}" passphrase="" keyfile_size=""
    [[ -n "$source" && -n "$keyfile" ]] || fail "Internal unlock keyfile error."
    # A9-14: stat sanity cap before anything is read from the file.
    keyfile_size="$(stat -c '%s' -- "$source" 2>/dev/null || true)"
    [[ -n "$keyfile_size" ]] || fail "Unable to stat the unlock keyfile."
    [[ "$keyfile_size" -le 65536 ]] \
        || fail "The unlock keyfile exceeds the 65536-byte sanity cap; refusing."
    IFS= read -r -n 1024 passphrase < "$source" || true
    if [[ ${#passphrase} -eq 1024 ]]; then
        printf 'UNLOCK_AUTH_FAILED=1\n' >&2
        fail "The LUKS passphrase is longer than 1024 characters; refusing."
    fi
    passphrase="${passphrase%$'\r'}"
    if [[ -z "$passphrase" ]]; then
        printf 'UNLOCK_AUTH_FAILED=1\n' >&2
        fail "An empty LUKS passphrase was read from the keyfile."
    fi
    # A9-02: umask-insensitive creation (see the stdin form).
    ( umask 077; : > "$keyfile" ) || fail "Cannot create the session keyfile."
    chmod 600 "$keyfile" || fail "Cannot restrict the session keyfile."
    printf '%s' "$passphrase" > "$keyfile" || fail "Cannot write the session keyfile."
    passphrase=""
    printf '%s\n' "$keyfile"
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
# Cycle 12: the legacy read-only file system check keeps the target's
# helper-owned mounts and skips every device that is mounted under the
# target (the root, usr, var, tmp, home and boot entries) with the modern
# offline-only skip lines; running an offline fsck over a live filesystem on
# Etch's IDE/PIO disk takes tens of minutes. fs-repair flows are unaffected
# (they do not go through fs_inspect_scope).
filesystem_release_all_mounts()
{
    return 0
}

# Cycle 12: for the legacy target scope, the target's own helper mounts are
# the selected repair system and count as mounted for the offline-only skip
# (the modern helper releases those mounts first, so its scan excludes them).
filesystem_mountpoint_for_device()
{
    local device="$1" hint="${2:-}" source mountpoint
    if (( RUNNING_HOST_MODE == 1 )); then
        hint=""
    elif [[ -n "$hint" && -n "$TARGET_ROOT" ]] && mountpoint -q "$TARGET_ROOT$hint" 2>/dev/null; then
        printf '%s\n' "$hint"
        return 0
    fi
    while IFS=' ' read -r source mountpoint; do
        [[ -n "$source" && -n "$mountpoint" ]] || continue
        source="${source%%\[*}"
        [[ "$(canonical_block "$source" 2>/dev/null || true)" == "$device" ]] || continue
        printf '%s\n' "$mountpoint"
        return 0
    done < <(findmnt -rn -o SOURCE,TARGET 2>/dev/null || true)
    printf '\n'
}

# Cycle 12 loop 2: cryptsetup 1.0 status parsing. Etch prints
# `device:  /dev/.static/dev/hdb5` — the trailing colon breaks the modern
# field match and the /dev/.static prefix defeats readlink -f, so an
# already-open mapper was never recognized (spurious "Mapper name
# collision"). Strip both before the caller's canonical comparison.
# A12-02: the binary resolves through the standard-location probe so a
# launch path without /sbin in PATH still parses the status.
legacy_crypt_status_device()
{
    local name="$1" value="" cryptsetup_bin=""
    cryptsetup_bin="$(legacy_standard_tool_path cryptsetup)" || return 1
    value="$("$cryptsetup_bin" status "$name" 2>/dev/null \
        | awk -F: '$1 ~ /^[ \t]*device$/ {gsub(/^[ \t]+|[ \t]+$/, "", $2); print $2; exit}')"
    [[ -n "$value" ]] || return 1
    # /dev/.static/dev/hdb5 -> /dev/hdb5 (the .static tree mirrors /dev, so
    # the prefix is dropped while the /dev/ root stays).
    value="${value#/dev/.static}"
    printf '%s\n' "$value"
}

# Cycle 12 loop 2: the existing-mapper lookup uses the legacy status parser
# and the canonical-path comparison (the modern body is otherwise identical).
find_crypt_mapper_for_device()
{
    local device="$1" canonical alias name existing_device
    canonical="$(canonical_block "$device")" || return 1
    # A12-02: probe the standard Etch locations instead of a bare PATH
    # lookup, so an already-open mapper is recognized on every launch path
    # (legacy_crypt_status_device re-resolves the same binary per status).
    legacy_standard_tool_path cryptsetup >/dev/null 2>&1 || return 1
    for alias in /dev/mapper/*; do
        [[ -e "$alias" || -L "$alias" ]] || continue
        name="$(basename -- "$alias")"
        [[ "$name" == "control" ]] && continue
        existing_device="$(legacy_crypt_status_device "$name")" || continue
        existing_device="$(readlink -f -- "$existing_device" 2>/dev/null || true)"
        [[ -n "$existing_device" ]] || continue
        if [[ "$existing_device" == "$canonical" ]]; then
            printf '%s\n' "$alias"
            return 0
        fi
    done
    return 1
}

# Cycle 12 loop 3: on the 2.6.18 kernel `mount -o remount,bind,ro` of a
# single-file bind fails with EBUSY, so the legacy resolver is a file COPY:
# the host /etc/resolv.conf is copied over the target's file (the target's
# original is backed up first, mode preserved), both are recorded for the
# teardown restore, and nothing is ever remounted. Idempotent: an existing
# copy is reused, so promote_target_data_rw cannot re-trigger it.
LEGACY_RESOLVER_DESTINATION=""
LEGACY_RESOLVER_BACKUP=""
# A12-07: the persistent marker proves that THIS helper made the resolver
# copy (it survives an interrupted session whose in-memory state died with
# it).  The A9-09 leftover guard consults it instead of the content alone:
# two systems that share the identical resolver file (for example two
# recovery rigs behind the same user-mode-network DNS) are not an
# interrupted copy and must not be refused.
LEGACY_RESOLVER_MARKER="$STATE_ROOT/resolver-copy-marker"
mount_target_resolver()
{
    local target_link="$TARGET_ROOT/etc/resolv.conf" link destination root_real
    [[ -r /etc/resolv.conf ]] || return 0
    [[ -e "$target_link" || -L "$target_link" ]] || return 0
    if [[ -L "$target_link" ]]; then
        link="$(readlink -- "$target_link")"
        if [[ "$link" == /* ]]; then
            destination="$TARGET_ROOT$link"
        else
            destination="$(dirname -- "$target_link")/$link"
        fi
    else
        destination="$target_link"
    fi
    # Fail closed: the destination must stay inside TARGET_ROOT and must not
    # be a symlink (copying over a symlink would clobber the link itself).
    root_real="$(realpath_existing "$TARGET_ROOT" 2>/dev/null || true)"
    [[ -n "$root_real" ]] || return 0
    path_within "$(realpath -m -- "$destination")" "$root_real" \
        || fail "Target resolver path escapes the selected root: $destination"
    [[ -L "$destination" ]] \
        && fail "Target resolver destination is a symlink: $destination"
    if [[ -e "$LEGACY_RESOLVER_DESTINATION" \
        && "$LEGACY_RESOLVER_DESTINATION" == "$destination" ]]; then
        log "Target resolver copy already present at $destination (reused)" | tee -a "$SESSION_LOG"
        return 0
    fi
    mkdir -p -- "$(dirname -- "$destination")"
    if [[ -e "$destination" ]]; then
        # A9-09: a destination that already equals the recovery-host resolver
        # is the leftover copy of an interrupted session whose backup was
        # lost.  Backing it up again would preserve a polluted file as the
        # target's "original"; refuse and name the scenario instead.
        # A12-07: the refusal requires the persistent marker (the copy this
        # helper made earlier) at the same destination — content equality
        # alone is not evidence, because a target whose own resolver file is
        # byte-identical to the recovery host's (shared DNS behind user-mode
        # networking, as on the reference Etch rigs) must keep repairing.
        if cmp -s -- "$destination" /etc/resolv.conf 2>/dev/null \
            && [[ -s "$LEGACY_RESOLVER_MARKER" ]] \
            && [[ "$(head -n1 "$LEGACY_RESOLVER_MARKER" 2>/dev/null || true)" == "$destination" ]]; then
            fail "The target resolver already holds the recovery-host copy from an interrupted session; refusing to re-backup a polluted file. Restore the target's original $destination manually (delete $LEGACY_RESOLVER_MARKER when the two systems share the identical resolver file) and retry."
        fi
        rm -f -- "$LEGACY_RESOLVER_MARKER" 2>/dev/null || true
        LEGACY_RESOLVER_BACKUP="$SESSION_DIR/resolv.conf.target.before"
        cp -a -- "$destination" "$LEGACY_RESOLVER_BACKUP" \
            || fail "Unable to back up the target resolver before the temporary copy."
    fi
    cp -- /etc/resolv.conf "$destination" \
        || fail "Unable to copy the recovery-host resolver into the target."
    LEGACY_RESOLVER_DESTINATION="$destination"
    # A12-07: record the copy persistently so the next session can tell a
    # genuine leftover from an identical-content original.  A marker write
    # failure aborts the copy path (fail closed): an untracked copy must
    # never become an unprovable "original" later.
    printf '%s\n' "$destination" > "$LEGACY_RESOLVER_MARKER" \
        || fail "Unable to record the resolver copy marker; the target resolver copy was not established."
    log "Copied recovery-host resolver into the target chroot (temporary; restored on exit)" | tee -a "$SESSION_LOG"
}

# Cycle 12 loop 3: the teardown restores the target's original resolver file
# (or removes the temporary copy when there was none) before the modern
# cleanup runs; the resolver is no longer a mount, so MOUNTS never carries it.
cleanup()
{
    local rc=$?
    if [[ -n "$LEGACY_RESOLVER_DESTINATION" ]]; then
        # A9-09: restore only while the destination still equals the
        # recovery-host resolver (the copy we made).  A destination that was
        # changed by the user or another tool after the copy is never
        # overwritten: log the skip and keep the backup as evidence.
        if [[ -e "$LEGACY_RESOLVER_DESTINATION" ]] \
            && cmp -s -- "$LEGACY_RESOLVER_DESTINATION" /etc/resolv.conf 2>/dev/null; then
            if [[ -n "$LEGACY_RESOLVER_BACKUP" && -e "$LEGACY_RESOLVER_BACKUP" ]]; then
                cp -a -- "$LEGACY_RESOLVER_BACKUP" "$LEGACY_RESOLVER_DESTINATION" 2>/dev/null \
                    || log "WARN: could not restore the target resolver copy." | tee -a "$SESSION_LOG"
                rm -f -- "$LEGACY_RESOLVER_BACKUP" 2>/dev/null || true
            else
                rm -f -- "$LEGACY_RESOLVER_DESTINATION" 2>/dev/null || true
            fi
        else
            log "ERROR: the resolver destination no longer equals the recovery-host resolver (changed by the user or another tool); the target's original file was NOT overwritten and the pre-copy backup was kept at $LEGACY_RESOLVER_BACKUP." | tee -a "$SESSION_LOG"
        fi
        # A12-07: this session's copy is resolved (restored, or proven no
        # longer present), so the persistent marker goes with it — a stale
        # marker must never refuse a later, legitimate session.
        rm -f -- "$LEGACY_RESOLVER_MARKER" 2>/dev/null || true
        LEGACY_RESOLVER_DESTINATION=""
        LEGACY_RESOLVER_BACKUP=""
    fi
    # A9-09 follow-up (rc preservation): the modern teardown body
    # (cleanup_modern, renamed by port.sh) captures $? itself, and the
    # resolver block above runs before that capture, so a helper failure's
    # exit status (or an INT/TERM/HUP status) must be handed over explicitly.
    # cleanup_modern reads LEGACY_CLEANUP_RC first (port.sh rewrites its
    # capture line), so the original status survives every teardown path.
    LEGACY_CLEANUP_RC="$rc"
    cleanup_modern
    return $rc
}

# Cycle 12: blkid TYPE probe for an absolute device path. The unlock mapper
# chain passes real /dev/mapper nodes (on Etch they are block nodes, not
# symlinks, so a readlink/basename translation would probe a nonexistent
# /dev/<name>); Etch's blkid may predate -o/-s, hence the TYPE= fallback.
legacy_blkid_value_path()
{
    local path="$1" real="" value=""
    real="$(legacy_real_tool_path blkid)" || return 0
    value="$("$real" -o value -s TYPE -- "$path" 2>/dev/null | head -n1 || true)"
    if [[ -z "$value" ]]; then
        value="$("$real" -- "$path" 2>/dev/null | head -n1 | sed -n 's/.*TYPE="\([^"]*\)".*/\1/p' || true)"
    fi
    printf '%s\n' "$value"
}

# A9-02: create the root-owned mode-0700 session directory when none exists
# (the unlock verb runs before prepare_target in a session).  prepare_target
# reuses the directory afterwards instead of replacing it with a fresh one.
legacy_unlock_ensure_session()
{
    if [[ -z "$SESSION_DIR" ]]; then
        SESSION_DIR="$(mktemp -d "$STATE_ROOT/session.XXXXXX")" \
            || fail "Unable to create the session directory for unlock."
        chmod 0700 -- "$SESSION_DIR" \
            || fail "Unable to secure the session directory for unlock."
        SESSION_LOG="$SESSION_DIR/session.log"
        touch "$SESSION_LOG" || fail "Unable to create the session log for unlock."
    fi
}

# A9-01: the caller-supplied unlock keyfile must be a regular file whose
# resolved path equals the raw argument, so a path that resolves through ANY
# symlink (final component or intermediate directory) is refused before the
# file is ever read.
legacy_unlock_keyfile_path_safe()
{
    local keyfile_arg="$1" real=""
    [[ -f "$keyfile_arg" && ! -L "$keyfile_arg" ]] || return 1
    real="$(realpath -- "$keyfile_arg" 2>/dev/null || true)"
    [[ -n "$real" && "$real" == "$keyfile_arg" ]] || return 1
    return 0
}

# A9-01: prove the keyfile ownership instead of accepting the effective uid.
#   - SUDO_UID present (sudo recorded the invoker): the file uid must equal
#     SUDO_UID and a given --key-owner must equal it too;
#   - no SUDO_UID: a non-zero --key-owner must match the file uid (the
#     gksu/gksudo case, which records no SUDO_UID);
#   - anything else has no provable owner and fails closed.
legacy_unlock_keyfile_owner_proven()
{
    local keyfile_arg="$1" key_owner_arg="${2:-}" uid=""
    uid="$(stat -c '%u' -- "$keyfile_arg" 2>/dev/null || true)"
    [[ -n "$uid" ]] || return 1
    if [[ -n "${SUDO_UID:-}" ]]; then
        [[ "$uid" == "$SUDO_UID" ]] || return 1
        if [[ -n "$key_owner_arg" ]]; then
            [[ "$key_owner_arg" == "$SUDO_UID" ]] || return 1
        fi
        return 0
    fi
    if [[ -n "$key_owner_arg" && "$key_owner_arg" != "0" ]]; then
        [[ "$uid" == "$key_owner_arg" ]] || return 1
        return 0
    fi
    return 1
}

unlock_target()
{
    local fstype uuid mapper_name mapper_path existing_mapper crypt_rc
    local keyfile_arg="" key_owner_arg=""
    local CRYPTSETUP_BIN=""
    while (($# > 0)); do
        case "$1" in
            --key-file)
                keyfile_arg="${2:-}"
                [[ -n "$keyfile_arg" ]] || fail "unlock --key-file requires a path."
                shift 2
                ;;
            --key-owner)
                key_owner_arg="${2:-}"
                [[ -n "$key_owner_arg" ]] || fail "unlock --key-owner requires a uid."
                shift 2
                ;;
            *)
                fail "unlock does not accept extra arguments."
                ;;
        esac
    done

    need lsblk
    need findmnt
    need readlink
    # A12-02: probe the standard Etch locations (/sbin/cryptsetup,
    # /usr/sbin/cryptsetup) plus PATH instead of the bare `command -v` the
    # modern `need` performs, so the unlock preflight passes on launch paths
    # whose PATH lacks the sbin directories.
    CRYPTSETUP_BIN="$(legacy_standard_tool_path cryptsetup)" \
        || fail "Required host command not found: cryptsetup (checked /sbin/cryptsetup, /usr/sbin/cryptsetup and PATH)"

    TARGET_DISK="$(canonical_block "$TARGET_DISK")" || fail "Target disk is not a block device."
    ROOT_DEVICE="$(canonical_block "$ROOT_DEVICE")" || fail "LUKS component is not a block device."

    assert_target_not_host "$TARGET_DISK"
    same_single_top_disk "$TARGET_DISK" "$ROOT_DEVICE" \
        || fail "Selected encrypted component does not belong exclusively to the target disk."

    fstype="$(lsblk -ndo FSTYPE "$ROOT_DEVICE" 2>/dev/null | head -n1)"
    if [[ "$fstype" != "crypto_LUKS" ]] && ! "$CRYPTSETUP_BIN" isLuks "$ROOT_DEVICE" >/dev/null 2>&1; then
        fail "Selected component is not a LUKS container: $ROOT_DEVICE"
    fi

    uuid="$("$CRYPTSETUP_BIN" luksUUID "$ROOT_DEVICE" 2>/dev/null || true)"
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
        # Cycle 12: a fresh GUI process must still be able to resolve the root
        # on the already-open mapper chain, so the same read-only probe runs
        # here and UNLOCKED_ROOT follows UNLOCKED.
        legacy_unlock_root_probe "$existing_mapper"
        return 0
    fi

    if [[ -e "$mapper_path" || -L "$mapper_path" ]]; then
        fail "Mapper name collision at $mapper_path; refusing to replace an existing mapping."
    fi

    log "Protected host check: PASS"
    log "Unlocking LUKS target $ROOT_DEVICE"
    log "Mapper name: $mapper_name"

    # The passphrase arrives either on stdin (one trailing newline/CR is
    # tolerated) or through the GUI's --key-file argument (a mode-600 regular
    # file whose ownership can be proven, because Qt 3.3.7 QProcess cannot
    # deliver stdin). Both channels land in a mode-600 session keyfile that
    # is deleted on every path, failure included. The helper NEVER deletes a
    # caller-supplied keyfile: the Qt3 GUI unlinks its own file when the
    # command finishes.
    local keyfile=""
    if [[ -n "$keyfile_arg" ]]; then
        legacy_unlock_keyfile_path_safe "$keyfile_arg" \
            || fail "The unlock keyfile is not a plain, symlink-free regular file: $keyfile_arg"
        legacy_unlock_keyfile_owner_proven "$keyfile_arg" "$key_owner_arg" \
            || fail "The unlock keyfile owner cannot be proven; refusing."
        # A9-02: the unlock verb runs before prepare_target, so the session
        # directory does not exist yet.  Create it first (root-owned, mode
        # 0700) so the passphrase keyfile never lands at /unlock-keyfile and
        # the session log has a home; prepare_target reuses it afterwards.
        legacy_unlock_ensure_session
        keyfile="$(legacy_unlock_keyfile_from_file "$keyfile_arg" "$SESSION_DIR/unlock-keyfile")"
    else
        legacy_unlock_ensure_session
        keyfile="$(legacy_unlock_keyfile_from_stdin "$SESSION_DIR/unlock-keyfile")"
    fi

    # cryptsetup 1.0 exit code 2 is the documented "no permission" result,
    # which includes an incorrect LUKS passphrase. Emit the machine-readable
    # marker so the GUI can offer a passphrase retry without re-authorizing.
    set +e
    "$CRYPTSETUP_BIN" --key-file "$keyfile" luksOpen "$ROOT_DEVICE" "$mapper_name"
    crypt_rc=$?
    set -e
    rm -f -- "$keyfile"
    if (( crypt_rc != 0 )); then
        if (( crypt_rc == 2 )); then
            printf 'UNLOCK_AUTH_FAILED=1\n' >&2
            fail "LUKS passphrase was not accepted."
        fi
        fail "cryptsetup could not unlock the selected LUKS target (exit code $crypt_rc)."
    fi

    [[ -b "$mapper_path" ]] || {
        "$CRYPTSETUP_BIN" luksClose "$mapper_path" >/dev/null 2>&1 || true
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
    legacy_unlock_root_probe "$mapper_path"
}

# Cycle 14: blkid UUID probe for an absolute device path (same modern /
# Etch-era output handling as the TYPE probe).
legacy_blkid_uuid_path()
{
    local path="$1" real="" value=""
    real="$(legacy_real_tool_path blkid)" || return 0
    value="$("$real" -o value -s UUID -- "$path" 2>/dev/null | head -n1 || true)"
    if [[ -z "$value" ]]; then
        value="$("$real" -- "$path" 2>/dev/null | head -n1 | sed -n 's/.*UUID="\([^"]*\)".*/\1/p' || true)"
    fi
    printf '%s\n' "$value"
}

# Cycle 11/12: resolve the Linux root candidate on the opened mapper chain
# with the port's read-only fstype probe (blkid; never a mount), so the GUI
# can enable Select Target with the helper-confirmed component even though
# its read-only inventory cannot see the mapped LV's filesystem.  Prefer an LV
# whose name matches root (case-insensitive); otherwise the first LV with a
# known Linux filesystem.  No candidate emits no line and the GUI keeps its
# existing fail-closed behavior.  The same probe runs for a mapper that was
# already open (existing-mapper reuse).
#
# B6/A9-08: the candidates are scoped to the unlocked PV's OWN volume group
# (pvs names the VG for this exact PV, lvs lists its logical volumes), so a
# foreign running-host LV with a Linux filesystem can never become
# UNLOCKED_ROOT.  When the LVM tooling is missing or no VG is provable, the
# probe falls back to the previous unscoped behaviour with a WARNING - the
# candidate is never silently dropped and the later same-disk/host guards
# remain the hard backstop.
legacy_unlock_root_probe()
{
    local mapper_path="$1" unlocked_root="" unlocked_fstype=""
    local candidate_name="" fstype_probe="" root_match="" fallback_name="" fallback_fstype=""
    local pvs_bin="" lvs_bin="" vg_name="" lv_item="" lv_path="" in_vg=0 scope_vg=0
    local -a vg_lvs=()
    pvs_bin="$(legacy_real_tool_path pvs || true)"
    lvs_bin="$(legacy_real_tool_path lvs || true)"
    vg_name=""
    if [[ -n "$pvs_bin" ]]; then
        vg_name="$("$pvs_bin" --noheadings -o vg_name -- "$mapper_path" 2>/dev/null \
            | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//' | grep -v '^$' | head -n1 || true)"
    fi
    if [[ -n "$vg_name" && -n "$lvs_bin" ]]; then
        while IFS= read -r lv_path; do
            lv_path="$(printf '%s' "$lv_path" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')"
            [[ -n "$lv_path" ]] || continue
            vg_lvs+=("$lv_path")
        done < <("$lvs_bin" --noheadings -o lv_path -- "$vg_name" 2>/dev/null || true)
        if (( ${#vg_lvs[@]} > 0 )); then
            scope_vg=1
            log "Unlocked-root probe scoped to volume group '$vg_name' (${#vg_lvs[@]} logical volume(s))"
        else
            log "WARNING: volume group '$vg_name' was proven for $mapper_path but no logical volumes could be listed; falling back to the unscoped unlocked-root probe (the same-disk/host guards remain the hard backstop)"
        fi
    elif [[ -n "$vg_name" ]]; then
        log "WARNING: volume group '$vg_name' was proven for $mapper_path but lvs is not installed in the recovery environment; falling back to the unscoped unlocked-root probe (the same-disk/host guards remain the hard backstop)"
    else
        log "WARNING: no volume group could be proven for $mapper_path (LVM tooling missing or the device is not a PV); falling back to the unscoped unlocked-root probe (the same-disk/host guards remain the hard backstop)"
    fi
    while IFS= read -r entry; do
        [[ -n "$entry" ]] || continue
        [[ "$entry" == "$mapper_path" || "$entry" == *control* ]] && continue
        if (( scope_vg == 1 )); then
            in_vg=0
            for lv_item in "${vg_lvs[@]}"; do
                if [[ "$entry" == "$lv_item" \
                    || "$(basename -- "$entry")" == "$(basename -- "$lv_item")" ]]; then
                    in_vg=1
                    break
                fi
            done
            # Candidates not in the unlocked PV's volume group are skipped:
            # they belong to the running host (or another target) and its LVs
            # must never be offered as the unlocked root.
            (( in_vg == 1 )) || continue
        fi
        candidate_name="${entry##*/}"
        [[ -n "$candidate_name" ]] || continue
        # /dev/mapper entries on Etch are real block nodes (readlink -f is the
        # identity), so the path is probed directly.
        fstype_probe="$(legacy_blkid_value_path "$entry")"
        case "$fstype_probe" in
            ext2|ext3|ext4|reiserfs|xfs|jfs)
                case "$candidate_name" in
                    *root*|*ROOT*) root_match="$entry"; unlocked_fstype="$fstype_probe" ;;
                esac
                if [[ -z "$fallback_name" ]]; then
                    fallback_name="$entry"
                    fallback_fstype="$fstype_probe"
                fi
                ;;
        esac
    done < <(find /dev/mapper -maxdepth 1 \( -type b -o -type l \) 2>/dev/null | sort)
    if [[ -n "$root_match" ]]; then
        unlocked_root="$root_match"
    else
        unlocked_root="$fallback_name"
        unlocked_fstype="$fallback_fstype"
    fi
    if [[ -n "$unlocked_root" ]]; then
        local unlocked_uuid=""
        unlocked_uuid="$(legacy_blkid_uuid_path "$unlocked_root")"
        log "Unlocked root candidate: $unlocked_root (${unlocked_fstype:-unknown fstype})"
        printf 'UNLOCKED_ROOT=%s\n' "$unlocked_root"
        if [[ -n "$unlocked_fstype" ]]; then
            printf 'UNLOCKED_ROOT_FSTYPE=%s\n' "$unlocked_fstype"
        fi
        if [[ -n "$unlocked_uuid" ]]; then
            printf 'UNLOCKED_ROOT_UUID=%s\n' "$unlocked_uuid"
        fi
    fi
    return 0
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
# only the four record-breaking bytes (%, TAB, CR, LF); the Qt3 picker
# decodes them inline.  A directory whose name embeds a newline stays
# unlistable on the Qt3 picker (documented deviation).
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
        # A9-13: % first (so the later escapes are not re-escaped), then TAB,
        # CR and LF in that order.
        encoded="$(printf '%s' "$name" | sed -e 's/%/%25/g' -e 's/\t/%09/g' -e 's/\r/%0D/g' -e 's/\n/%0A/g' | tr -d '\n')"
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
    local command="${1:-}" first="" rest="" sub="" rest_args=""
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
    rest_args="${rest#* }"
    [[ "$sub" == "full-upgrade" ]] && sub="dist-upgrade"
    if [[ -n "$rest_args" && "$rest_args" != "$rest" ]]; then
        printf 'apt-get %s %s\n' "$sub" "$rest_args"
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
    local run_pid=0 watcher_pid=0
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
        # A10-05: the pipeline runs in a background subshell whose exit status
        # is the command's own (PIPESTATUS captured inside the subshell), and
        # the cancel watcher ticks alongside it: a cancel token (or a helper
        # that died) terminates the pipeline with the bounded TERM -> KILL
        # escalation.
        ( set +e
          chroot "$TARGET_ROOT" /usr/bin/env \
              HOME=/root \
              PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
              DEBIAN_FRONTEND=noninteractive \
              APT_LISTCHANGES_FRONTEND=none \
              /bin/sh -c "$run_command" < /dev/null 2>&1 | tee -a "$SESSION_LOG" "$transcript"
          rc=${PIPESTATUS[0]}
          exit "$rc" ) &
        run_pid=$!
        legacy_cancel_watcher "$run_pid" &
        watcher_pid=$!
        rc=0
        wait "$run_pid" 2>/dev/null || rc=$?
        kill "$watcher_pid" 2>/dev/null || true
        wait "$watcher_pid" 2>/dev/null || true
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
# skipped the guard; no chroot).  B6/A9-03: the exec runs under
# `timeout --foreground 300 --kill-after=10` - the compat.sh timeout shim
# prefers a real coreutils timeout that supports --foreground and falls back
# to the pure-bash watchdog on Etch (whose timeout binary predates
# --foreground/--kill-after).  Etch deviation (documented): the watchdog can
# only kill the direct child, so grandchildren of a killed shell may outlive
# the bound.  EFI hosts keep the modern isolation+refusal path
# (run_host_shell_modern): no firmware variables exist to isolate on this
# BIOS-only host, so the legacy path is used here.
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
    local run_command="$command" retry_command="" retried=0 run_pid=0 watcher_pid=0
    : > "$transcript"
    run_command="$(legacy_apt_intent_translate "$command")"
    if [[ "$run_command" != "$command" ]]; then
        log "apt intent translated: $run_command" | tee -a "$SESSION_LOG"
    fi
    while :; do
        set +e
        # A10-05: same backgrounded-pipeline + cancel-watcher shape as the
        # chroot shell; the subshell exit status is the command's own.
        ( set +e
          timeout --foreground 300 --kill-after=10 /usr/bin/env \
              HOME=/root \
              PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
              DEBIAN_FRONTEND=noninteractive \
              /bin/bash -lc "$run_command" 2>&1 | tee -a "$SESSION_LOG" "$transcript"
          rc=${PIPESTATUS[0]}
          exit "$rc" ) &
        run_pid=$!
        legacy_cancel_watcher "$run_pid" &
        watcher_pid=$!
        rc=0
        wait "$run_pid" 2>/dev/null || rc=$?
        kill "$watcher_pid" 2>/dev/null || true
        wait "$watcher_pid" 2>/dev/null || true
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
    # A10-05: the cancel surface is checked before the host repair starts (the
    # individual stage entry wrappers re-check it between stages).
    legacy_cancel_stage_check
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
