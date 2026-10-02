# legacy/compat.sh — bash 3.1 / Debian Etch compatibility shims.
# shellcheck shell=bash
#
# This file is embedded verbatim into the generated legacy helper
# (legacy/boot-repair-helper.sh) by legacy/port.sh and is also sourced directly
# by the legacy contract tests.  It must stay bash 3.1-parseable and must never
# use bash 4 syntax: the array-read builtin, associative arrays, case-conversion
# expansions, job-control wait, namerefs, printf -v or append redirection.
#
# Two classes of shims live here:
#   * language shims  — array/case/assoc emulation used by the transform
#   * tool shims      — functions named after the missing Etch tools that
#                       delegate to the real binary when it exists and fall
#                       back to /sys + /proc + blkid + coreutils otherwise.
#
# BOOT_REPAIR_LEGACY_SHIMS=auto|force forces the fallback path even when the
# real tool is installed (test/development only; the installed helper never
# sets it).  All probes fail closed: an option set the fallback cannot prove
# returns nonzero instead of printing a guessed answer.

# ---------------------------------------------------------------------------
# Language shims
# ---------------------------------------------------------------------------

# Case conversion via tr.  LC_ALL=C keeps identifiers (UUIDs, PARTUUIDs,
# loader names) ASCII-stable.
legacy_lc()
{
    printf '%s' "$1" | LC_ALL=C tr '[:upper:]' '[:lower:]'
}

legacy_uc()
{
    printf '%s' "$1" | LC_ALL=C tr '[:lower:]' '[:upper:]'
}

legacy_ucfirst()
{
    local s="$1"
    printf '%s%s' "$(printf '%s' "${s:0:1}" | LC_ALL=C tr '[:lower:]' '[:upper:]')" "${s:1}"
}

# Array-read replacement.  Accepts only the option subset the helper uses
# (-t and the -- end-of-options marker) and assigns into the caller's
# dynamically scoped array by name.  Everything else fails closed with a
# reason: -n/-d/-O/-s (and any other unknown option) would change the read
# semantics this shim cannot honour, so they are refused instead of being
# silently consumed (A11-03).  Every ported call site is `mapfile -t`.
legacy_readarray()
{
    local __name="" __line="" __arg=""
    while (( $# > 0 )); do
        __arg="$1"
        case "$__arg" in
            -t) ;;
            --) ;;
            -n|-d|-O|-s|-*)
                printf 'legacy_readarray: unsupported option %s (only -t is accepted)\n' "$__arg" >&2
                return 1
                ;;
            *) __name="$__arg" ;;
        esac
        shift
    done
    [[ -n "$__name" ]] || return 1
    eval "$__name=()"
    # A12-01: a here-string feed (`<<<"$(producer)"`, the form the mount
    # sweep and the block walkers now use instead of process substitution)
    # always appends one trailing newline, which an empty producer turns into
    # a single empty record; the skip keeps empty input from adding a
    # spurious "" element.  No caller ever relied on preserving an
    # intentional empty line (every consumer filters empty entries).
    while IFS= read -r __line || [[ -n "$__line" ]]; do
        [[ -n "$__line" ]] || continue
        eval "$__name+=(\"$__line\")"
    done
    return 0
}

# Associative-array emulation over parallel indexed arrays.  The transform
# rewrites an associative declaration to a parallel indexed pair and every
# access to legacy_assoc_*.  The parallel arrays are dynamically scoped, so the
# caller's `local` declaration is visible inside these helpers.
legacy_assoc_get()
{
    local __name="$1" __key="$2" __i=0 __n=0 __entry=""
    eval "__n=\${#${__name}_keys[@]}"
    while (( __i < __n )); do
        eval "__entry=\${${__name}_keys[$__i]}"
        if [[ "$__entry" == "$__key" ]]; then
            eval "printf '%s' \"\${${__name}[$__i]}\""
            return 0
        fi
        __i=$((__i + 1))
    done
    return 0
}

legacy_assoc_get_num()
{
    local __value=""
    __value="$(legacy_assoc_get "$1" "$2")"
    [[ -n "$__value" ]] || __value=0
    printf '%s' "$__value"
}

legacy_assoc_has()
{
    local __name="$1" __key="$2" __i=0 __n=0 __entry=""
    eval "__n=\${#${__name}_keys[@]}"
    while (( __i < __n )); do
        eval "__entry=\${${__name}_keys[$__i]}"
        [[ "$__entry" == "$__key" ]] && return 0
        __i=$((__i + 1))
    done
    return 1
}

legacy_assoc_keys()
{
    local __name="$1" __i=0 __n=0 __entry=""
    eval "__n=\${#${__name}_keys[@]}"
    while (( __i < __n )); do
        eval "__entry=\${${__name}_keys[$__i]}"
        printf '%s\n' "$__entry"
        __i=$((__i + 1))
    done
    return 0
}

legacy_assoc_set()
{
    local __name="$1" __key="$2" __val="$3" __i=0 __n=0 __entry=""
    eval "__n=\${#${__name}_keys[@]}"
    while (( __i < __n )); do
        eval "__entry=\${${__name}_keys[$__i]}"
        if [[ "$__entry" == "$__key" ]]; then
            eval "${__name}[$__i]=\"\$__val\""
            return 0
        fi
        __i=$((__i + 1))
    done
    eval "${__name}_keys+=(\"\$__key\")"
    eval "${__name}+=(\"\$__val\")"
    return 0
}

legacy_assoc_unset()
{
    local __name="$1" __key="$2" __i=0 __n=0 __entry="" __j=0
    eval "__n=\${#${__name}_keys[@]}"
    while (( __i < __n )); do
        eval "__entry=\${${__name}_keys[$__i]}"
        if [[ "$__entry" == "$__key" ]]; then
            __j=$__i
            while (( __j + 1 < __n )); do
                eval "${__name}_keys[$__j]=\${${__name}_keys[$((__j + 1))]}"
                eval "${__name}[$__j]=\${${__name}[$((__j + 1))]}"
                __j=$((__j + 1))
            done
            eval "unset '${__name}_keys[$((__n - 1))]'"
            eval "unset '${__name}[$((__n - 1))]'"
            return 0
        fi
        __i=$((__i + 1))
    done
    return 1
}

# Extended-regex sed: Etch sed 4.1.5 has -r but no -E; modern GNU sed and
# BusyBox accept -r.  The flag is probed once and cached.
legacy_sed_ext()
{
    local real=""
    real="$(legacy_real_tool_path sed || true)"
    [[ -n "$real" ]] || return 1
    if [[ -z "${LEGACY_SED_EXT_FLAG:-}" ]]; then
        if "$real" -E '' </dev/null >/dev/null 2>&1; then
            LEGACY_SED_EXT_FLAG="-E"
        else
            LEGACY_SED_EXT_FLAG="-r"
        fi
    fi
    "$real" "$LEGACY_SED_EXT_FLAG" "$@"
}

# ---------------------------------------------------------------------------
# Root evidence (os-release-less legacy roots)
# ---------------------------------------------------------------------------

# True when DIR carries installed-root evidence: /etc/os-release or a legacy
# release file paired with its package database.  Probe-based; never keyed on a
# distribution ID.
legacy_root_evidence_present()
{
    local dir="$1"
    [[ -n "$dir" ]] || return 1
    [[ -f "$dir/etc/os-release" ]] && return 0
    # Etch-era installed roots can split /var and /usr onto separate LVs
    # (the etch2 split-LV layout keeps the dpkg database on the debian-var
    # LV), so the dpkg status pair alone is not reliable evidence.  Accept
    # /etc/debian_version paired with ANY ONE of the dpkg status files or the
    # /etc-resident release files - every one of them lives on the root even
    # with separate /var and /usr mounts.
    if [[ -f "$dir/etc/debian_version" ]]; then
        [[ -f "$dir/var/lib/dpkg/status" || -f "$dir/var/lib/dpkg/status-old" \
            || -f "$dir/etc/apt/sources.list" || -f "$dir/etc/inittab" ]] && return 0
    fi
    if [[ -f "$dir/etc/redhat-release" ]]; then
        [[ -d "$dir/var/lib/rpm" || -f "$dir/var/lib/rpm/Packages" ]] && return 0
    fi
    if [[ -f "$dir/etc/SuSE-release" && -d "$dir/var/lib/rpm" ]]; then
        return 0
    fi
    return 1
}

legacy_root_evidence_label()
{
    local dir="$1"
    if [[ -f "$dir/etc/os-release" ]]; then
        printf '%s\n' '/etc/os-release'
    elif [[ -f "$dir/etc/debian_version" ]]; then
        printf '%s\n' '/etc/debian_version + release evidence (split-mount safe)'
    elif [[ -f "$dir/etc/redhat-release" ]]; then
        printf '%s\n' '/etc/redhat-release + rpm database'
    elif [[ -f "$dir/etc/SuSE-release" ]]; then
        printf '%s\n' '/etc/SuSE-release + rpm database'
    else
        printf '%s\n' 'none'
    fi
}

# ---------------------------------------------------------------------------
# Generic probe helpers
# ---------------------------------------------------------------------------

# Resolve the external executable for NAME, ignoring shell functions (needed
# inside the tool shims, where `command -v` would find the shim itself).
legacy_real_tool_path()
{
    type -P "$1" 2>/dev/null
}

# A12-02: probe an external tool through the standard Etch system directories
# first (/sbin, /usr/sbin) and then PATH.  cryptsetup 1.0 is at
# /sbin/cryptsetup while the desktop user PATH has no /sbin, so the plain
# `command -v` lookups reported it missing on launch paths without the sbin
# directories; this probe is the fail-closed lookup every legacy cryptsetup
# gate uses.  BOOT_REPAIR_LEGACY_TOOL_DIRS overrides the probed directory
# list (test/development only, same convention as BOOT_REPAIR_LEGACY_SHIMS;
# the installed helper never sets it).
legacy_standard_tool_path()
{
    local name="$1" candidate="" dirs=""
    [[ -n "$name" && "$name" != */* ]] || return 1
    if [[ -n "${BOOT_REPAIR_LEGACY_TOOL_DIRS:-}" ]]; then
        dirs="$BOOT_REPAIR_LEGACY_TOOL_DIRS"
    else
        dirs="/sbin /usr/sbin"
    fi
    for candidate in $dirs; do
        if [[ -x "$candidate/$name" ]]; then
            printf '%s\n' "$candidate/$name"
            return 0
        fi
    done
    legacy_real_tool_path "$name"
}

legacy_force_shims()
{
    [[ "${BOOT_REPAIR_LEGACY_SHIMS:-auto}" == force ]]
}

# ---------------------------------------------------------------------------
# lsblk fallback (/sys/block + blkid + /proc/self/mountinfo)
# ---------------------------------------------------------------------------

# sysfs block layout: modern kernels expose every block device under
# /sys/class/block; Debian Etch's 2.6.18 kernel predates that class and only
# has /sys/block, with partitions as subdirectories of their disk.  Every
# attribute path goes through this resolver.  It falls back to the canonical
# (missing) /sys/block/<k> path so callers keep plain `[[ -f ]]` semantics.
legacy_sys_block_dir()
{
    local k="$1" d=""
    if [[ -e "/sys/class/block/$k" ]]; then
        printf '/sys/class/block/%s\n' "$k"
        return 0
    fi
    if [[ -e "/sys/block/$k" ]]; then
        printf '/sys/block/%s\n' "$k"
        return 0
    fi
    for d in /sys/block/*/"$k"; do
        if [[ -e "$d" ]]; then
            printf '%s\n' "$d"
            return 0
        fi
    done
    printf '/sys/block/%s\n' "$k"
}

# Every sysfs block directory (disks and partitions) of the running kernel,
# for major:minor identity lookups.
legacy_sys_block_all_dirs()
{
    local d=""
    if [[ -d /sys/class/block ]]; then
        for d in /sys/class/block/*; do
            [[ -e "$d" ]] && printf '%s\n' "$d"
        done
        return 0
    fi
    for d in /sys/block/*; do
        [[ -e "$d" ]] && printf '%s\n' "$d"
    done
    for d in /sys/block/*/*; do
        [[ -e "$d/dev" ]] && printf '%s\n' "$d"
    done
    return 0
}

# Resolve a device path or alias (/dev/mapper/<name>, /dev/dm-N, /dev/hdaN)
# to its kernel name.  Kernel 2.6.18 creates no /sys/class/block and no
# /dev/mapper symlinks, so identity falls back to major:minor matching.
legacy_block_kname()
{
    local dev="$1" base="" resolved="" maj="" min="" want="" d=""
    base="$(basename -- "$dev" 2>/dev/null || true)"
    if [[ -n "$base" && -e "/sys/class/block/$base" ]]; then
        printf '%s\n' "$base"
        return 0
    fi
    if [[ -n "$base" && -e "/sys/block/$base" ]]; then
        printf '%s\n' "$base"
        return 0
    fi
    resolved="$(readlink -f -- "$dev" 2>/dev/null || true)"
    base="${resolved##*/}"
    if [[ -n "$base" ]] && { [[ -e "/sys/class/block/$base" ]] || [[ -e "/sys/block/$base" ]]; }; then
        printf '%s\n' "$base"
        return 0
    fi
    maj="$(stat -c '%t' "$dev" 2>/dev/null || true)"
    min="$(stat -c '%T' "$dev" 2>/dev/null || true)"
    [[ -n "$maj" && -n "$min" ]] || return 1
    want="$((16#$maj)):$((16#$min))"
    # A12-05: here-string feed instead of process substitution - bash 3.1.17
    # corrupts its /dev/fd fifo bookkeeping (subst.c add_fifo_list) when
    # process substitution runs repeatedly inside the block-device sweep
    # loops, and this lookup runs once per lsblk call (per swept device).
    while IFS= read -r d; do
        [[ -n "$d" ]] || continue
        if [[ "$(cat "$d/dev" 2>/dev/null || true)" == "$want" ]]; then
            basename -- "$d"
            return 0
        fi
    done <<<"$(legacy_sys_block_all_dirs)"
    return 1
}

# Kernel 2.6.18 has no /sys/block/<disk>/<part>/partition attribute; its
# partitions are recognizable by the partition start sector instead.
legacy_lsblk_is_partition()
{
    local k="$1" dir=""
    dir="$(legacy_sys_block_dir "$k")"
    [[ -f "$dir/partition" || -f "$dir/start" ]]
}

legacy_lsblk_type()
{
    local k="$1" uuid="" dir="" name="" target=""
    dir="$(legacy_sys_block_dir "$k")"
    if legacy_lsblk_is_partition "$k"; then
        printf 'part\n'
        return 0
    fi
    if [[ -d "$dir/dm" || "$k" == dm-* ]]; then
        # Modern kernels expose dm/uuid; 2.6.18 does not and its dmsetup only
        # supports -o name, so read the target type from the device table.
        uuid="$(cat "$dir/dm/uuid" 2>/dev/null || true)"
        if [[ -z "$uuid" ]]; then
            name="$(legacy_lsblk_dm_name "$k" 2>/dev/null || true)"
            if [[ -n "$name" ]] && command -v dmsetup >/dev/null 2>&1; then
                target="$(dmsetup table "$name" 2>/dev/null | awk 'NR == 1 {print $3}' || true)"
                case "$target" in
                    crypt*) uuid="CRYPT-" ;;
                    *) uuid="LVM-" ;;
                esac
            fi
        fi
        case "$uuid" in
            CRYPT-*) printf 'crypt\n' ;;
            *) printf 'lvm\n' ;;
        esac
        return 0
    fi
    if [[ -d "$dir/md" ]]; then
        printf 'raid\n'
        return 0
    fi
    case "$k" in
        loop*) printf 'loop\n' ;;
        sr*) printf 'rom\n' ;;
        *) printf 'disk\n' ;;
    esac
}

legacy_lsblk_parent_kname()
{
    local k="$1" dir="" resolved=""
    legacy_lsblk_is_partition "$k" || return 1
    dir="$(legacy_sys_block_dir "$k")"
    resolved="$(readlink -f -- "$dir" 2>/dev/null || true)"
    [[ -n "$resolved" ]] || return 1
    basename -- "$(dirname -- "$resolved")"
}

legacy_lsblk_slaves()
{
    local k="$1" dir="" entry=""
    dir="$(legacy_sys_block_dir "$k")"
    [[ -d "$dir/slaves" ]] || return 0
    for entry in "$dir/slaves"/*; do
        [[ -e "$entry" ]] || continue
        basename -- "$entry"
    done
    return 0
}

legacy_lsblk_children()
{
    local k="$1" dir="" entry=""
    dir="$(legacy_sys_block_dir "$k")"
    for entry in "$dir"/*; do
        [[ -f "$entry/partition" || -f "$entry/start" ]] || continue
        basename -- "$entry"
    done
    if [[ -d "$dir/holders" ]]; then
        for entry in "$dir/holders"/*; do
            [[ -e "$entry" ]] || continue
            basename -- "$entry"
        done
    fi
    return 0
}

# Deterministic major:minor ordering (sysfs directory order is not stable).
legacy_lsblk_order()
{
    local k="" dev="" dir=""
    while IFS= read -r k; do
        [[ -n "$k" ]] || continue
        dir="$(legacy_sys_block_dir "$k")"
        dev="$(cat "$dir/dev" 2>/dev/null || true)"
        [[ -n "$dev" ]] || dev="0:0"
        printf '%s\t%s\n' "$dev" "$k"
    done | sort -t: -k1,1n -k2,2n | cut -f2
}

legacy_lsblk_walk()
{
    local k="$1" child=""
    printf '%s\n' "$k"
    legacy_lsblk_children "$k" | legacy_lsblk_order | while IFS= read -r child; do
        [[ -n "$child" ]] || continue
        legacy_lsblk_walk "$child"
    done
}

legacy_lsblk_walk_inverse()
{
    local k="$1" p="" resolved=""
    printf '%s\n' "$k"
    resolved="$(legacy_lsblk_parent_kname "$k" 2>/dev/null || true)"
    if [[ -n "$resolved" ]]; then
        legacy_lsblk_walk_inverse "$resolved"
        return 0
    fi
    legacy_lsblk_slaves "$k" | legacy_lsblk_order | while IFS= read -r p; do
        [[ -n "$p" ]] || continue
        legacy_lsblk_walk_inverse "$p"
    done
}

legacy_blkid_value()
{
    local k="$1" tag="$2" real=""
    real="$(legacy_real_tool_path blkid)" || return 0
    "$real" -o value -s "$tag" "/dev/$k" 2>/dev/null | head -n1 || true
}

legacy_lsblk_pttype()
{
    local k="$1" value="" first="" last=""
    value="$(legacy_blkid_value "$k" PTTYPE)"
    if [[ -n "$value" ]]; then
        printf '%s\n' "$value"
        return 0
    fi
    if legacy_lsblk_is_partition "$k"; then
        return 0
    fi
    # GPT magic at LBA 1, otherwise the MBR signature in LBA 0.
    first="$(dd if="/dev/$k" bs=512 skip=1 count=1 2>/dev/null | head -c8 || true)"
    if [[ "$first" == "EFI PART" ]]; then
        printf 'gpt\n'
        return 0
    fi
    last="$(dd if="/dev/$k" bs=1 skip=510 count=2 2>/dev/null | od -An -tx1 2>/dev/null | tr -d ' \n' || true)"
    if [[ "$last" == "55aa" ]]; then
        printf 'dos\n'
    fi
    return 0
}

legacy_lsblk_parttype()
{
    local k="$1" value=""
    value="$(legacy_blkid_value "$k" PART_ENTRY_TYPE)"
    [[ -n "$value" ]] && { printf '%s\n' "$value"; return 0; }
    # Low-level probe (blkid >= 2.17); the Etch blkid does not support -p, so
    # this stays a best-effort fallback and prints nothing when unavailable.
    value="$("$(legacy_real_tool_path blkid)" -p -o value -s PART_ENTRY_TYPE "/dev/$k" 2>/dev/null | head -n1 || true)"
    [[ -n "$value" ]] && printf '%s\n' "$value"
    return 0
}

legacy_lsblk_model()
{
    local k="$1" dir="" model=""
    dir="$(legacy_sys_block_dir "$k")"
    if [[ -r "$dir/device/model" ]]; then
        model="$(cat "$dir/device/model" 2>/dev/null || true)"
    elif [[ -r "$dir/device/name" ]]; then
        model="$(cat "$dir/device/name" 2>/dev/null || true)"
    fi
    # Trim leading/trailing whitespace without relying on extglob.
    model="${model#"${model%%[![:space:]]*}"}"
    model="${model%"${model##*[![:space:]]}"}"
    printf '%s\n' "$model"
}

legacy_lsblk_size()
{
    local k="$1" bytes="$2" sectors=""
    sectors="$(cat "$(legacy_sys_block_dir "$k")/size" 2>/dev/null || true)"
    [[ "$sectors" =~ ^[0-9]+$ ]] || return 0
    if [[ "$bytes" == yes ]]; then
        printf '%s\n' "$((sectors * 512))"
        return 0
    fi
    awk -v b="$((sectors * 512))" 'BEGIN {
        split("B K M G T P", u, " ");
        i = 1;
        while (i < 6 && b >= 1024) { b = b / 1024; i++ }
        if (i == 1) printf "%d%s\n", b, u[i];
        else if (b >= 10 || b == int(b)) printf "%.0f%s\n", b, u[i];
        else printf "%.1f%s\n", b, u[i];
    }'
}

# Mountpoints of a block device, from the mount table.  When mountinfo is
# available the major:minor identity is exact; the /proc/mounts fallback (Etch
# kernel 2.6.18 predates mountinfo) matches the canonicalized source path.
legacy_mountpoints_from_table()
{
    local dev="$1" dev_real="" dev_id="" src_id="" _id target source _fstype _options
    # Prefer major:minor identity: on Etch the running root is mounted from
    # /dev/mapper/<name> while sysfs names it dm-N, so path comparison misses.
    dev_id="$(stat -c '%t:%T' "$dev" 2>/dev/null || true)"
    if [[ -z "$dev_id" ]]; then
        dev_real="$(readlink -f -- "$dev" 2>/dev/null || true)"
        [[ -n "$dev_real" ]] || return 0
    fi
    while IFS=$'\t' read -r _id target source _fstype _options; do
        if [[ -n "$dev_id" ]]; then
            src_id="$(stat -c '%t:%T' "$source" 2>/dev/null || true)"
            [[ -n "$src_id" && "$src_id" == "$dev_id" ]] || continue
        else
            [[ "$(readlink -f -- "$source" 2>/dev/null || true)" == "$dev_real" ]] || continue
        fi
        printf '%s\n' "$target"
    done
    return 0
}

legacy_mountinfo_mountpoints()
{
    local k="$1" devid=""
    if [[ -r /proc/self/mountinfo ]]; then
        devid="$(cat "$(legacy_sys_block_dir "$k")/dev" 2>/dev/null || true)"
        [[ -n "$devid" ]] || return 0
        sed -e 's/\\040/ /g; s/\\011/ /g; s/\\012/ /g; s/\\134/\\/g' /proc/self/mountinfo \
            | awk -v dev="$devid" '$3 == dev { print $5 }'
        return 0
    fi
    # A12-05: here-string feed (see legacy_block_kname).
    legacy_mountpoints_from_table "/dev/$k" <<<"$(legacy_mountinfo_table)"
}

legacy_lsblk_dm_name()
{
    local k="$1" dir="" name="" devid="" maj="" min="" entry="" want=""
    dir="$(legacy_sys_block_dir "$k")"
    [[ -d "$dir/dm" || "$k" == dm-* ]] || return 1
    name="$(cat "$dir/dm/name" 2>/dev/null || true)"
    [[ -n "$name" ]] && { printf '%s\n' "$name"; return 0; }
    # Kernel 2.6.18 has no dm attribute directory and no /dev/mapper symlinks;
    # resolve the device-mapper name through dmsetup or the /dev/mapper nodes.
    devid="$(cat "$dir/dev" 2>/dev/null || true)"
    [[ -n "$devid" ]] || return 1
    maj="${devid%%:*}"
    min="${devid#*:}"
    if command -v dmsetup >/dev/null 2>&1; then
        name="$(dmsetup info -c --noheadings -o name -j "$maj" -m "$min" 2>/dev/null | head -n1 || true)"
        [[ -n "$name" ]] && { printf '%s\n' "$name"; return 0; }
    fi
    want="$(printf '%x:%x' "$maj" "$min")"
    for entry in /dev/mapper/*; do
        [[ -b "$entry" ]] || continue
        [[ "$(stat -c '%t:%T' "$entry" 2>/dev/null || true)" == "$want" ]] || continue
        basename -- "$entry"
        return 0
    done
    return 1
}

legacy_lsblk_column_value()
{
    local k="$1" col="$2" fullpath="$3"
    local dm_name=""
    case "$col" in
        NAME)
            dm_name="$(legacy_lsblk_dm_name "$k" 2>/dev/null || true)"
            if [[ "$fullpath" == yes ]]; then
                if [[ -n "$dm_name" ]]; then
                    printf '/dev/mapper/%s\n' "$dm_name"
                else
                    printf '/dev/%s\n' "$k"
                fi
            else
                if [[ -n "$dm_name" ]]; then
                    printf '%s\n' "$dm_name"
                else
                    printf '%s\n' "$k"
                fi
            fi
            ;;
        KNAME) printf '%s\n' "$k" ;;
        PKNAME) legacy_lsblk_parent_kname "$k" ;;
        TYPE) legacy_lsblk_type "$k" ;;
        FSTYPE) legacy_blkid_value "$k" TYPE ;;
        LABEL) legacy_blkid_value "$k" LABEL ;;
        UUID) legacy_blkid_value "$k" UUID ;;
        PARTLABEL) legacy_blkid_value "$k" PARTLABEL ;;
        PARTUUID) legacy_blkid_value "$k" PARTUUID ;;
        PTTYPE) legacy_lsblk_pttype "$k" ;;
        PARTTYPE) legacy_lsblk_parttype "$k" ;;
        PARTN) cat "$(legacy_sys_block_dir "$k")/partition" 2>/dev/null || true ;;
        MODEL) legacy_lsblk_model "$k" ;;
        SIZE) legacy_lsblk_size "$k" "$4" ;;
        MOUNTPOINTS) legacy_mountinfo_mountpoints "$k" ;;
        *) return 1 ;;
    esac
    return 0
}

legacy_lsblk_emit()
{
    local k="$1" cols_csv="$2" pairs="$3" fullpath="$4" bytes="$5"
    local i=0 j=0 col="" value="" line="" pairsline="" mp=""
    local -a cols=() values=()
    IFS=',' read -r -a cols <<< "$cols_csv"
    for ((i = 0; i < ${#cols[@]}; i++)); do
        col="${cols[$i]}"
        value="$(legacy_lsblk_column_value "$k" "$col" "$fullpath" "$bytes")" || return 1
        values+=("$value")
    done
    if [[ "$pairs" == yes ]]; then
        for ((i = 0; i < ${#cols[@]}; i++)); do
            value="${values[$i]}"
            value="${value//\"/}"
            pairsline+="${cols[$i]}=\"${value}\" "
        done
        printf '%s\n' "${pairsline% }"
        return 0
    fi
    # MOUNTPOINTS expands to one row per mountpoint (lsblk behaviour); every
    # other column repeats on each row.
    for ((i = 0; i < ${#cols[@]}; i++)); do
        [[ "${cols[$i]}" == "MOUNTPOINTS" ]] || continue
        while IFS= read -r mp; do
            line=""
            for ((j = 0; j < ${#cols[@]}; j++)); do
                if (( j == i )); then
                    value="$mp"
                else
                    value="${values[$j]}"
                fi
                line+="${line:+ }${value}"
            done
            printf '%s\n' "$line"
        done <<< "${values[$i]}"
        return 0
    done
    line=""
    for ((i = 0; i < ${#cols[@]}; i++)); do
        line+="${line:+ }${values[$i]}"
    done
    printf '%s\n' "$line"
    return 0
}

legacy_lsblk_header()
{
    local cols_csv="$1" i=0 col="" out=""
    local -a cols=()
    IFS=',' read -r -a cols <<< "$cols_csv"
    for ((i = 0; i < ${#cols[@]}; i++)); do
        col="${cols[$i]}"
        out+="${out:+ }${col}"
    done
    printf '%s\n' "$out"
}

legacy_lsblk()
{
    local cols_csv="" device="" pairs=no fullpath=no inverse=no noheader=no bytes=no toponly=no
    local opt="" flags="" kname="" k=""
    local -a args=("$@")
    local -a devs=()
    local i=0
    while (( i < ${#args[@]} )); do
        opt="${args[$i]}"
        case "$opt" in
            -P) pairs=yes ;;
            -p) fullpath=yes ;;
            -s) inverse=yes ;;
            -b) bytes=yes ;;
            -d) toponly=yes ;;
            -n) noheader=yes ;;
            -r) ;;
            -o)
                i=$((i + 1))
                cols_csv="${args[$i]:-}"
                ;;
            -*)
                if [[ "$opt" == *o ]]; then
                    flags="${opt%o}"
                    flags="${flags#-}"
                    case "$flags" in *P*) pairs=yes ;; esac
                    case "$flags" in *p*) fullpath=yes ;; esac
                    case "$flags" in *s*) inverse=yes ;; esac
                    case "$flags" in *b*) bytes=yes ;; esac
                    case "$flags" in *d*) toponly=yes ;; esac
                    case "$flags" in *n*) noheader=yes ;; esac
                    case "$flags" in *r*) ;; esac
                    # Any other option letter is not part of the used set:
                    # fail closed instead of printing a guessed answer.
                    case "$flags" in *[!Ppsbdnr]*) return 1 ;; esac
                    i=$((i + 1))
                    cols_csv="${args[$i]:-}"
                else
                    case "$opt" in
                        *P*) pairs=yes ;;
                        *p*) fullpath=yes ;;
                        *s*) inverse=yes ;;
                        *b*) bytes=yes ;;
                        *d*) toponly=yes ;;
                        *n*) noheader=yes ;;
                        *r*) ;;
                        *) return 1 ;;
                    esac
                fi
                ;;
            *) device="$opt" ;;
        esac
        i=$((i + 1))
    done
    [[ -n "$cols_csv" && -n "$device" ]] || return 1
    kname="$(legacy_block_kname "$device")" || return 1
    # lsblk suppresses the header in -P (pairs) mode.
    if [[ "$noheader" != yes && "$pairs" != yes ]]; then
        legacy_lsblk_header "$cols_csv"
    fi
    if [[ "$toponly" == yes ]]; then
        devs=("$kname")
    elif [[ "$inverse" == yes ]]; then
        # A12-05: here-string feed (see legacy_block_kname); this walk runs
        # once per lsblk invocation, and the block sweep invokes lsblk once
        # per swept device entry.
        while IFS= read -r k; do
            [[ -n "$k" ]] || continue
            devs+=("$k")
        done <<<"$(legacy_lsblk_walk_inverse "$kname")"
    else
        while IFS= read -r k; do
            [[ -n "$k" ]] || continue
            devs+=("$k")
        done <<<"$(legacy_lsblk_walk "$kname")"
    fi
    for k in "${devs[@]:-}"; do
        [[ -n "$k" ]] || continue
        legacy_lsblk_emit "$k" "$cols_csv" "$pairs" "$fullpath" "$bytes" || return 1
    done
    return 0
}

lsblk()
{
    local real=""
    if ! legacy_force_shims; then
        real="$(legacy_real_tool_path lsblk || true)"
        if [[ -n "$real" ]]; then
            "$real" "$@"
            return $?
        fi
    fi
    legacy_lsblk "$@"
}

# ---------------------------------------------------------------------------
# findmnt fallback (/proc/self/mountinfo)
# ---------------------------------------------------------------------------

legacy_normalize_path()
{
    local p="$1"
    [[ -n "$p" ]] || p="/"
    while [[ "$p" == *//* ]]; do
        p="${p//\/\//\/}"
    done
    while [[ "$p" != "/" && "$p" == */ ]]; do
        p="${p%/}"
    done
    printf '%s\n' "$p"
}

# Parse a mountinfo-format table ($1 ID, $5 target, "-" separator, source,
# fstype, super options).  Print one row per mount:
# ID<TAB>TARGET<TAB>SOURCE<TAB>FSTYPE<TAB>OPTIONS.  OPTIONS merges the
# per-mount options (field 6) with the superblock options, de-duplicated in
# order, matching findmnt's OPTIONS column.  Rows stay in mount order, so the
# effective (topmost) mount of a stack is the last matching row.
legacy_mountinfo_table_from()
{
    sed -e 's/\\040/ /g; s/\\011/ /g; s/\\012/ /g; s/\\134/\\/g' "$1" \
        | awk '{
            sep = 0
            for (i = 1; i <= NF; i++) { if ($i == "-") { sep = i; break } }
            if (sep == 0) next
            merged = $6
            n = split($(sep + 3), parts, ",")
            for (i = 1; i <= n; i++) {
                if (parts[i] == "") continue
                # The per-mount ro/rw flag is authoritative; findmnt drops the
                # conflicting superblock flag (a ro remount keeps a rw super).
                if (parts[i] == "rw" && index("," merged ",", ",ro,") > 0) continue
                if (parts[i] == "ro" && index("," merged ",", ",rw,") > 0) continue
                if (index("," merged ",", "," parts[i] ",") > 0) continue
                merged = merged "," parts[i]
            }
            printf "%s\t%s\t%s\t%s\t%s\n", $1, $5, $(sep + 2), $(sep + 1), merged
        }'
}

# Parse a /proc/mounts table (SOURCE TARGET FSTYPE OPTIONS freq passno).  The
# effective mount of a stacked target is its last entry; the first-seen row
# position is kept (so head -n1 and tail -n1 agree) with the last entry's
# source/fstype/options.  ID is a synthetic sequence number.  Etch's 2.6.18
# kernel has no /proc/self/mountinfo, so this is the real Etch path.
legacy_mounts_table_from()
{
    sed -e 's/\\040/ /g; s/\\011/ /g; s/\\012/ /g; s/\\134/\\/g' "$1" \
        | awk '
            {
                target = $2
                if (!(target in seen)) { order[++n] = target }
                seen[target] = 1
                src[target] = $1
                fstype[target] = $3
                opts[target] = $4
            }
            END {
                for (i = 1; i <= n; i++) {
                    t = order[i]
                    printf "%d\t%s\t%s\t%s\t%s\n", i, t, src[t], fstype[t], opts[t]
                }
            }'
}

# Print one row per mount: ID<TAB>TARGET<TAB>SOURCE<TAB>FSTYPE<TAB>OPTIONS.
legacy_mountinfo_table()
{
    if [[ -r /proc/self/mountinfo ]]; then
        legacy_mountinfo_table_from /proc/self/mountinfo
    elif [[ -r /proc/mounts ]]; then
        legacy_mounts_table_from /proc/mounts
    else
        return 1
    fi
}

# True when mountpoint MP covers PATH: either the exact mountpoint or a
# proper ancestor ("/" covers every absolute path as the implicit root).
legacy_findmnt_path_match()
{
    local mp="$1" path="$2"
    [[ "$mp" == "$path" ]] && return 0
    [[ "$mp" == "/" ]] && return 0
    [[ "$path" == "$mp"/* ]] && return 0
    return 1
}

legacy_findmnt_emit_row()
{
    local row="$1" cols_csv="$2" out="" value=""
    local -a cols=() fields=()
    IFS=',' read -r -a cols <<< "$cols_csv"
    IFS=$'\t' read -r -a fields <<< "$row"
    local i=0 col=""
    for ((i = 0; i < ${#cols[@]}; i++)); do
        col="${cols[$i]}"
        case "$col" in
            TARGET) value="${fields[1]:-}" ;;
            SOURCE) value="${fields[2]:-}" ;;
            FSTYPE) value="${fields[3]:-}" ;;
            OPTIONS) value="${fields[4]:-}" ;;
            ID) value="${fields[0]:-}" ;;
            *) return 1 ;;
        esac
        out+="${out:+ }${value}"
    done
    printf '%s\n' "$out"
}

legacy_findmnt()
{
    local cols_csv="" target="" prefix="" noheader=no
    local opt="" arg="" path="" exact=0 best="" best_mp=""
    local -a args=("$@")
    local i=0
    while (( i < ${#args[@]} )); do
        opt="${args[$i]}"
        case "$opt" in
            -R|--recursive)
                i=$((i + 1))
                prefix="${args[$i]:-}"
                ;;
            -T|--target)
                i=$((i + 1))
                target="${args[$i]:-}"
                ;;
            -o|--output)
                i=$((i + 1))
                cols_csv="${args[$i]:-}"
                ;;
            -n|--noheadings) noheader=yes ;;
            -r|--raw) ;;
            --target=*) target="${opt#*=}" ;;
            --output=*) cols_csv="${opt#*=}" ;;
            --recursive=*) prefix="${opt#*=}" ;;
            -*)
                # Combined short flags (-rn, -nr, ...).
                arg="${opt#-}"
                while [[ -n "$arg" ]]; do
                    case "${arg:0:1}" in
                        n) noheader=yes ;;
                        r) ;;
                        R)
                            i=$((i + 1))
                            prefix="${args[$i]:-}"
                            ;;
                        T)
                            i=$((i + 1))
                            target="${args[$i]:-}"
                            ;;
                        o)
                            i=$((i + 1))
                            cols_csv="${args[$i]:-}"
                            ;;
                        *) return 1 ;;
                    esac
                    arg="${arg:1}"
                done
                ;;
            *) target="${target:-$opt}" ;;
        esac
        i=$((i + 1))
    done
    [[ -n "$cols_csv" ]] || cols_csv="TARGET,SOURCE,FSTYPE,OPTIONS"
    if [[ -n "$prefix" ]]; then
        path="$(legacy_normalize_path "$prefix")"
        if [[ "$noheader" != yes ]]; then
            legacy_lsblk_header "$cols_csv"
        fi
        # A12-05: here-string feed (see legacy_block_kname); the recursive
        # listing stays a covering filter and never resolves a single mount.
        while IFS=$'\t' read -r _id mp _source _fstype _options; do
            [[ -n "$_id" || -n "$mp" ]] || continue
            [[ "$mp" == "$path" || "$mp" == "$path"/* ]] || continue
            legacy_findmnt_emit_row "$_id"$'\t'"$mp"$'\t'"$_source"$'\t'"$_fstype"$'\t'"$_options" "$cols_csv" || return 1
        done <<<"$(legacy_mountinfo_table)"
        return 0
    fi
    if [[ -n "$target" ]]; then
        path="$(legacy_normalize_path "$target")"
        if [[ "$noheader" != yes ]]; then
            legacy_lsblk_header "$cols_csv"
        fi
        # A12-03: findmnt --target resolves the MOST SPECIFIC covering mount:
        # every entry stacked on the exact mountpoint, otherwise the single
        # deepest ancestor ("/" only when nothing else covers the path).  The
        # old first-match loop returned the host root "/" for any path below
        # a session mount, so the selected repair root's mount evidence
        # (source and options) reported the RUNNING HOST's root instead of
        # the target's session mount.  A tie keeps the later row (mount
        # order: the topmost entry wins).
        exact=0
        best=""
        best_mp=""
        while IFS=$'\t' read -r _id mp _source _fstype _options; do
            [[ -n "$_id" || -n "$mp" ]] || continue
            if [[ "$mp" == "$path" ]]; then
                legacy_findmnt_emit_row "$_id"$'\t'"$mp"$'\t'"$_source"$'\t'"$_fstype"$'\t'"$_options" "$cols_csv" || return 1
                exact=1
                continue
            fi
            legacy_findmnt_path_match "$mp" "$path" || continue
            [[ -z "$best_mp" || ${#mp} -ge ${#best_mp} ]] || continue
            best_mp="$mp"
            best="$_id"$'\t'"$mp"$'\t'"$_source"$'\t'"$_fstype"$'\t'"$_options"
        done <<<"$(legacy_mountinfo_table)"
        if (( exact == 0 )) && [[ -n "$best" ]]; then
            legacy_findmnt_emit_row "$best" "$cols_csv" || return 1
        fi
        return 0
    fi
    if [[ "$noheader" != yes ]]; then
        legacy_lsblk_header "$cols_csv"
    fi
    while IFS=$'\t' read -r _id mp _source _fstype _options; do
        [[ -n "$_id" || -n "$mp" ]] || continue
        legacy_findmnt_emit_row "$_id"$'\t'"$mp"$'\t'"$_source"$'\t'"$_fstype"$'\t'"$_options" "$cols_csv" || return 1
    done <<<"$(legacy_mountinfo_table)"
    return 0
}

findmnt()
{
    local real=""
    if ! legacy_force_shims; then
        real="$(legacy_real_tool_path findmnt || true)"
        if [[ -n "$real" ]]; then
            "$real" "$@"
            return $?
        fi
    fi
    legacy_findmnt "$@"
}

# ---------------------------------------------------------------------------
# mountpoint fallback
# ---------------------------------------------------------------------------

legacy_mountpoint_quiet()
{
    local path=""
    path="$(legacy_normalize_path "$1")"
    legacy_mountinfo_table | awk -F'\t' -v p="$path" '$2 == p { found = 1 } END { exit(found ? 0 : 1) }'
}

# Fallback mountpoint: prints the GNU message when not quiet, returns 0/1.
legacy_mountpoint()
{
    local quiet=0 path="" arg=""
    for arg in "$@"; do
        case "$arg" in
            -q) quiet=1 ;;
            -d|-x) ;;
            -*) ;;
            *) path="$arg" ;;
        esac
    done
    [[ -n "$path" ]] || return 1
    if legacy_mountpoint_quiet "$path"; then
        [[ "$quiet" == 1 ]] || printf '%s is a mountpoint\n' "$path"
        return 0
    fi
    [[ "$quiet" == 1 ]] || printf '%s is not a mountpoint\n' "$path"
    return 1
}

mountpoint()
{
    local real=""
    if ! legacy_force_shims; then
        real="$(legacy_real_tool_path mountpoint || true)"
        if [[ -n "$real" ]]; then
            "$real" "$@"
            return $?
        fi
    fi
    legacy_mountpoint "$@"
}

# ---------------------------------------------------------------------------
# realpath fallback (coreutils readlink -e/-m/-f)
# ---------------------------------------------------------------------------

# Fallback realpath over coreutils readlink (Etch coreutils 5.97 supports
# readlink -e/-f/-m).
legacy_realpath()
{
    local flag="-f" path="" arg="" endopts=0
    for arg in "$@"; do
        if (( endopts == 0 )); then
            case "$arg" in
                --) endopts=1; continue ;;
                -e|--canonicalize-existing) flag="-e"; continue ;;
                -m|--canonicalize-missing) flag="-m"; continue ;;
                -f|--canonicalize) flag="-f"; continue ;;
                -q|--quiet|-s|--strip|--relative-to=*|--relative-base=*) continue ;;
                -*) return 1 ;;
            esac
        fi
        path="$arg"
    done
    [[ -n "$path" ]] || return 1
    readlink "$flag" -- "$path" 2>/dev/null
}

realpath()
{
    local real=""
    if ! legacy_force_shims; then
        real="$(legacy_real_tool_path realpath || true)"
        if [[ -n "$real" ]]; then
            "$real" "$@"
            return $?
        fi
    fi
    legacy_realpath "$@"
}

# ---------------------------------------------------------------------------
# timeout fallback (pure-bash watchdog)
# ---------------------------------------------------------------------------

legacy_duration_seconds()
{
    local d="$1" n=1
    case "$d" in
        *s) d="${d%s}" ;;
        *m) d="${d%m}"; n=60 ;;
        *h) d="${d%h}"; n=3600 ;;
        *d) d="${d%d}"; n=86400 ;;
    esac
    awk -v v="$d" -v m="$n" 'BEGIN { s = v * m; if (s < 0) s = 0; printf "%d\n", s }'
}

legacy_timeout_real_supports_foreground()
{
    local real="$1"
    "$real" --foreground 1 true >/dev/null 2>&1
}

# Read one whitespace field of /proc/<pid>/stat, counting from the field AFTER
# the parenthesized comm field (which may itself contain spaces and
# parentheses).  The remainder of the line begins at the original field 3
# (state); `which` selects a field of that remainder: 1 -> state (3),
# 2 -> ppid (4), 3 -> pgrp (5).
legacy_proc_stat_field()
{
    local pid="$1" which="$2" rest="" n=0
    rest="$(cat "/proc/$pid/stat" 2>/dev/null || true)"
    [[ -n "$rest" ]] || return 1
    rest="${rest#*') '}"
    [[ -n "$rest" ]] || return 1
    n="$which"
    while (( n > 1 )); do
        rest="${rest#* }"
        n=$((n - 1))
    done
    printf '%s\n' "${rest%% *}"
    return 0
}

# Print every live descendant pid of <root> (children, grandchildren, ...),
# one per line, by a breadth-first walk over /proc/<pid>/stat (field 4 =
# ppid).  The snapshot is taken BEFORE the first signal is delivered, so a
# child whose parent dies first is reparented to init/subreaper but still
# enumerated here.
legacy_timeout_descendants()
{
    local root="$1" frontier="$1" seen=" $1 " next="" p="" child="" ppid="" cpid=""
    [[ "$root" =~ ^[0-9]+$ ]] || return 0
    while [[ -n "$frontier" ]]; do
        next=""
        for p in $frontier; do
            for child in /proc/[0-9]*; do
                [[ -e "$child" ]] || continue
                ppid="$(legacy_proc_stat_field "${child##*/}" 2)" || continue
                [[ -n "$ppid" && "$ppid" == "$p" ]] || continue
                cpid="${child##*/}"
                [[ "$seen" == *" $cpid "* ]] && continue
                seen="$seen$cpid "
                next="$next$cpid "
            done
        done
        frontier="$next"
    done
    for cpid in $seen; do
        [[ "$cpid" == "$root" ]] && continue
        printf '%s\n' "$cpid"
    done
    return 0
}

# Signal one command tree: the snapshotted descendants are signalled first,
# then the root pid last (with its process group via kill -- -$pid when
# leader=1).  The root is signalled LAST so the caller's `wait` on it never
# returns before the descendant sweep has run - signalling the root first
# would let its death reparent the children to init and strand them.  A
# descendant that is itself a group leader (proven by reading
# /proc/<pid>/stat field 5 == pid) is also group-signalled so its own group
# members stop too.
legacy_timeout_terminate()
{
    local pid="$1" leader="${2:-0}" signal="${3:-TERM}" descendants="${4:-}"
    local snap_pid="" child_leader=0 pgrp=""
    for snap_pid in $descendants; do
        [[ "$snap_pid" =~ ^[0-9]+$ ]] || continue
        [[ "$snap_pid" == "$pid" ]] && continue
        child_leader=0
        pgrp="$(legacy_proc_stat_field "$snap_pid" 3 || true)"
        if [[ -n "$pgrp" && "$pgrp" == "$snap_pid" ]]; then
            child_leader=1
        fi
        kill -"$signal" "$snap_pid" 2>/dev/null || true
        if (( child_leader == 1 )); then
            kill -"$signal" -- -"$snap_pid" 2>/dev/null || true
        fi
    done
    kill -"$signal" "$pid" 2>/dev/null || true
    if (( leader == 1 )); then
        kill -"$signal" -- -"$pid" 2>/dev/null || true
    fi
    return 0
}

legacy_timeout_watchdog()
{
    local signal="TERM" kill_after=0 duration="" arg="" rc=0
    local tmp="" pid=0 wd=0 seconds=0 leader=0 setsid_bin="" descendants=""
    local -a cmd=()
    while (( $# > 0 )); do
        arg="$1"
        case "$arg" in
            --foreground|--preserve-status|-v|--verbose) shift ;;
            -s|--signal) shift; signal="${1:-TERM}"; shift ;;
            --signal=*) signal="${arg#*=}"; shift ;;
            -k|--kill-after) shift; kill_after="${1:-0}"; shift ;;
            --kill-after=*) kill_after="${arg#*=}"; shift ;;
            -*) shift ;;
            *) duration="$arg"; shift; break ;;
        esac
    done
    cmd=("$@")
    (( ${#cmd[@]} > 0 )) || return 1
    [[ "$kill_after" =~ ^[0-9]+$ ]] || kill_after=0
    seconds="$(legacy_duration_seconds "$duration")"
    tmp="$(mktemp "${TMPDIR:-/tmp}/legacy-timeout.XXXXXX" 2>/dev/null)" || return 1
    rm -f -- "$tmp"
    # Never toggle `set -e` here: shims must not change the caller's shell
    # options, and `cmd || rc=$?` is errexit-safe on its own.
    # Run the command in its own session (setsid) when available so the
    # watcher can signal the whole group; otherwise fall back to signalling
    # the direct child plus a /proc descendant sweep of the snapshotted tree.
    setsid_bin="$(legacy_real_tool_path setsid 2>/dev/null || true)"
    if [[ -n "$setsid_bin" ]]; then
        leader=1
        "$setsid_bin" "${cmd[@]}" &
    else
        leader=0
        "${cmd[@]}" &
    fi
    pid=$!
    (
        sleep "$seconds" 2>/dev/null
        : > "$tmp" 2>/dev/null
        # Snapshot the full descendant set before the first signal so a child
        # reparented to init by its parent's death is still enumerated.
        descendants="$(legacy_timeout_descendants "$pid")"
        legacy_timeout_terminate "$pid" "$leader" "$signal" "$descendants"
        if (( kill_after > 0 )); then
            sleep "$kill_after" 2>/dev/null
            # Reuse the SAME snapshot so reparented children are still reached.
            legacy_timeout_terminate "$pid" "$leader" KILL "$descendants"
        fi
    ) &
    wd=$!
    rc=0
    # 2>/dev/null keeps bash 3.1's job-control "Terminated" notice out of the
    # helper transcript when the watchdog kills the command.
    wait "$pid" 2>/dev/null || rc=$?
    kill "$wd" 2>/dev/null || true
    wait "$wd" 2>/dev/null || true
    if [[ -e "$tmp" ]]; then
        rc=124
    fi
    rm -f -- "$tmp"
    return "$rc"
}

timeout()
{
    local real=""
    if ! legacy_force_shims; then
        real="$(legacy_real_tool_path timeout || true)"
        if [[ -n "$real" ]] && legacy_timeout_real_supports_foreground "$real"; then
            "$real" "$@"
            return $?
        fi
    fi
    legacy_timeout_watchdog "$@"
}

# ---------------------------------------------------------------------------
# base64 fallback (openssl enc -base64)
# ---------------------------------------------------------------------------

legacy_b64e()
{
    local real=""
    if ! legacy_force_shims; then
        real="$(legacy_real_tool_path base64 || true)"
        if [[ -n "$real" ]]; then
            "$real"
            return $?
        fi
    fi
    real="$(legacy_real_tool_path openssl || true)"
    [[ -n "$real" ]] || return 1
    "$real" enc -base64 -A
}

legacy_b64d()
{
    local real=""
    if ! legacy_force_shims; then
        real="$(legacy_real_tool_path base64 || true)"
        if [[ -n "$real" ]]; then
            "$real" -d
            return $?
        fi
    fi
    real="$(legacy_real_tool_path openssl || true)"
    [[ -n "$real" ]] || return 1
    # -A accepts the single-line payloads the helper's protocol uses (no
    # trailing newline); it also decodes wrapped input.
    "$real" enc -base64 -d -A
}

base64()
{
    local decode=0 arg="" real=""
    for arg in "$@"; do
        case "$arg" in
            -d|--decode) decode=1 ;;
            -i|--ignore-garbage) ;;
            -w|--wrap) ;;
            -w*|--wrap=*) ;;
            -*) ;;
        esac
    done
    if ! legacy_force_shims; then
        real="$(legacy_real_tool_path base64 || true)"
        if [[ -n "$real" ]]; then
            "$real" "$@"
            return $?
        fi
    fi
    if (( decode )); then
        legacy_b64d
    else
        legacy_b64e
    fi
}

# ---------------------------------------------------------------------------
# sort -V fallback (version sort, awk zero-padded keys)
# ---------------------------------------------------------------------------

legacy_sort_supports_version()
{
    local real="$1"
    if [[ -n "${LEGACY_SORT_VERSION_OK:-}" ]]; then
        [[ "$LEGACY_SORT_VERSION_OK" == yes ]]
        return
    fi
    if printf 'a\n' | "$real" -V >/dev/null 2>&1; then
        LEGACY_SORT_VERSION_OK=yes
    else
        LEGACY_SORT_VERSION_OK=no
    fi
    [[ "$LEGACY_SORT_VERSION_OK" == yes ]]
}

legacy_sort_versions()
{
    local unique=0 keyfield=0 arg=""
    local real=""
    for arg in "$@"; do
        case "$arg" in
            -u) unique=1 ;;
            -k1,1|-k1) keyfield=1 ;;
        esac
    done
    if ! legacy_force_shims; then
        real="$(legacy_real_tool_path sort || true)"
        if [[ -n "$real" ]] && legacy_sort_supports_version "$real"; then
            "$real" -V "$@"
            return $?
        fi
    fi
    awk -v unique="$unique" -v keyfield="$keyfield" '
        function pad(s,   out, i, c, n, num) {
            out = ""
            i = 1
            while (i <= length(s)) {
                c = substr(s, i, 1)
                if (c >= "0" && c <= "9") {
                    num = ""
                    while (i <= length(s) && (c = substr(s, i, 1)) >= "0" && c <= "9") {
                        num = num c
                        i++
                    }
                    sub(/^0+/, "", num)
                    if (num == "") num = "0"
                    out = out sprintf("%015d", num + 0)
                } else {
                    out = out c
                    i++
                }
            }
            return out
        }
        function decorate(line,   f) {
            if (keyfield == 1) {
                split(line, f, /[ \t]+/)
                return pad(f[1])
            }
            return pad(line)
        }
        {
            lines[NR] = $0
            keys[NR] = decorate($0)
        }
        END {
            for (i = 2; i <= NR; i++) {
                l = lines[i]; k = keys[i]
                j = i - 1
                while (j >= 1 && keys[j] > k) {
                    lines[j + 1] = lines[j]
                    keys[j + 1] = keys[j]
                    j--
                }
                lines[j + 1] = l
                keys[j + 1] = k
            }
            last = ""
            for (i = 1; i <= NR; i++) {
                if (unique && lines[i] == last) continue
                print lines[i]
                last = lines[i]
            }
        }
    '
}

# ---------------------------------------------------------------------------
# date fallback
# ---------------------------------------------------------------------------

legacy_date_iso()
{
    date --iso-8601=seconds 2>/dev/null || date '+%Y-%m-%dT%H:%M:%S%z' 2>/dev/null || date
}

# ---------------------------------------------------------------------------
# mount fallback (Etch util-linux 2.12r has no --make-rslave / rec)
# ---------------------------------------------------------------------------

legacy_mount_rslave_supported()
{
    local a="" b="" rc=0
    if [[ -n "${LEGACY_MOUNT_RSLAVE_OK:-}" ]]; then
        [[ "$LEGACY_MOUNT_RSLAVE_OK" == yes ]]
        return
    fi
    if legacy_force_shims; then
        LEGACY_MOUNT_RSLAVE_OK=no
        return 1
    fi
    a="$(mktemp -d "${TMPDIR:-/tmp}/legacy-mount-probe.XXXXXX" 2>/dev/null)" || {
        LEGACY_MOUNT_RSLAVE_OK=no
        return 1
    }
    b="$(mktemp -d "${TMPDIR:-/tmp}/legacy-mount-probe.XXXXXX" 2>/dev/null)" || {
        rmdir "$a" 2>/dev/null || true
        LEGACY_MOUNT_RSLAVE_OK=no
        return 1
    }
    if mount --bind "$a" "$b" 2>/dev/null && mount --make-rslave "$b" 2>/dev/null; then
        LEGACY_MOUNT_RSLAVE_OK=yes
    else
        LEGACY_MOUNT_RSLAVE_OK=no
        rc=1
    fi
    umount "$b" 2>/dev/null || true
    rmdir "$a" "$b" 2>/dev/null || true
    return "$rc"
}

# rbind/rbind-ro without mount-namespace propagation support: a non-recursive
# bind plus a read-only remount.  Nested mounts are not copied; callers must
# unmount in reverse order (the helper's cleanup does).
legacy_mount_special()
{
    local kind="$1" source="$2" destination="$3"
    case "$kind" in
        rbind)
            if legacy_mount_rslave_supported; then
                mount --rbind "$source" "$destination" \
                    && mount --make-rslave "$destination"
            else
                mount --bind "$source" "$destination"
            fi
            ;;
        rbind-ro)
            if legacy_mount_rslave_supported; then
                mount --rbind "$source" "$destination" \
                    && mount --make-rslave "$destination" \
                    && mount -o remount,bind,ro,rec "$destination"
            else
                mount --bind "$source" "$destination" \
                    && mount -o remount,bind,ro "$destination"
            fi
            ;;
        *) return 1 ;;
    esac
}
