#!/usr/bin/env bash
# legacy/port.sh — deterministic bash-3.1 port of the modern helper.
#
#   legacy/port.sh [--check] [--quiet]
#
# Generates legacy/boot-repair-helper.sh from scripts/boot-repair-helper.sh by
# applying the reviewed transformation set below, embedding legacy/compat.sh as
# a prelude and appending legacy/overlay.sh.  --check regenerates to a
# temporary file, lists every transformation with its count and fails when the
# committed file differs (drift gate).
#
# Generation is fail-closed (A11-01/A11-04/A11-08):
#   * every replace_block must match at least once or generation aborts;
#   * after the syntax transforms the ported body must hold zero residual
#     modern constructs (mapfile, sed -E, sort -V, ${v,,}/${v^^}/${v^}) and
#     exactly the audited shim-call totals (60 legacy_readarray, 56
#     legacy_assoc_set, ...) or generation aborts;
#   * the assembled helper must define every function at most once more than
#     the modern source does (the port must never introduce duplicates);
#   * the assembled helper must pass the verify_no_bash4 blacklist.
#
# This script runs on Etch too (scripts/package-legacy.sh calls it), so it must
# stay bash 3.1-clean: no mapfile, no declare -A, no ${v,,}/${v^^}/${v^}.
set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname -- "$SCRIPT_DIR")"
MODERN="$ROOT_DIR/scripts/boot-repair-helper.sh"
COMPAT="$SCRIPT_DIR/compat.sh"
OVERLAY="$SCRIPT_DIR/overlay.sh"
OUTPUT="$SCRIPT_DIR/boot-repair-helper.sh"

CHECK=0
QUIET=0

usage()
{
    cat <<'USAGE'
Usage: legacy/port.sh [--check] [--quiet]

  --check   regenerate to a temporary file, print the transformation summary
            and fail when legacy/boot-repair-helper.sh is not byte-identical
  --quiet   only print the final result line
  -h, --help
USAGE
}

while (( $# > 0 )); do
    case "$1" in
        --check) CHECK=1 ;;
        --quiet) QUIET=1 ;;
        -h|--help) usage; exit 0 ;;
        *) usage >&2; exit 2 ;;
    esac
    shift
done

note()
{
    if (( QUIET == 0 )); then
        printf '  %-42s %s\n' "$1" "$2"
    fi
}

count_literal()
{
    # Count non-overlapping occurrences of the literal, not matching lines:
    # busybox grep -o prints at most one match per line while GNU grep -o
    # prints one per occurrence, so the audited drift counts (A11-04) silently
    # shift depending on which grep is in PATH (seen on Alpine's /bin/grep ->
    # /bin/busybox, where the array [@] count dropped from 299 to 287). awk
    # index() is a pure literal search with identical semantics on every awk
    # (gawk, mawk, busybox awk). The needle travels through the environment so
    # awk -v escape processing can never reinterpret its bytes.
    NEEDLE="$1" awk '
        BEGIN { s = ENVIRON["NEEDLE"]; n = length(s) }
        n == 0 { exit }
        {
            line = $0
            while ((pos = index(line, s)) > 0) {
                count++
                line = substr(line, pos + n)
            }
        }
        END { print count + 0 }' "$2"
}

count_regex()
{
    grep -oE -- "$1" "$2" 2>/dev/null | wc -l | tr -d '[:space:]'
}

apply_sed()
{
    local file="$1"
    shift
    local tmp
    tmp="$(mktemp "${TMPDIR:-/tmp}/port-sed.XXXXXX")" || return 1
    if sed "$@" "$file" > "$tmp"; then
        mv -- "$tmp" "$file"
    else
        rm -f -- "$tmp"
        return 1
    fi
}

# Literal multi-line replacement (awk line-array matching).  Old/new are exact
# text blocks without a trailing newline.  A transform that matches nothing is
# a drift signal: awk prints the failing block's first line to stderr, exits
# nonzero and the caller aborts generation (A11-01).
replace_block()
{
    local file="$1" old="$2" new="$3" tmp oldfile newfile rc=0
    tmp="$(mktemp "${TMPDIR:-/tmp}/port-block.XXXXXX")" || return 1
    oldfile="$(mktemp "${TMPDIR:-/tmp}/port-old.XXXXXX")" || { rm -f -- "$tmp"; return 1; }
    newfile="$(mktemp "${TMPDIR:-/tmp}/port-new.XXXXXX")" || { rm -f -- "$tmp" "$oldfile"; return 1; }
    printf '%s' "$old" > "$oldfile"
    printf '%s' "$new" > "$newfile"
    awk -v oldfile="$oldfile" -v newfile="$newfile" '
        function load(path, arr,   line, n) {
            n = 0
            while ((getline line < path) > 0) {
                n++
                arr[n] = line
            }
            close(path)
            return n
        }
        BEGIN {
            oldn = load(oldfile, old)
            newn = load(newfile, new)
        }
        { lines[NR] = $0 }
        END {
            replaced = 0
            i = 1
            while (i <= NR) {
                found = 1
                if (i + oldn - 1 > NR) {
                    found = 0
                } else {
                    for (j = 1; j <= oldn; j++) {
                        if (lines[i + j - 1] != old[j]) {
                            found = 0
                            break
                        }
                    }
                }
                if (found) {
                    for (j = 1; j <= newn; j++) {
                        print new[j]
                    }
                    replaced++
                    i += oldn
                } else {
                    print lines[i]
                    i++
                }
            }
            if (replaced == 0) {
                printf "ERROR: replace_block matched 0 occurrences (block starts with: %s)\n", old[1] > "/dev/stderr"
                exit 1
            }
        }
    ' "$file" > "$tmp" || rc=$?
    if (( rc == 0 )); then
        mv -- "$tmp" "$file" || rc=$?
    fi
    if (( rc != 0 )); then
        rm -f -- "$tmp"
    fi
    rm -f -- "$oldfile" "$newfile"
    return "$rc"
}

# Rewrite every access to one associative array into legacy_assoc_* calls.
# The read forms are anchored on the literal ${name[ prefix; the set forms are
# anchored with a word boundary (\<) so a name that is a substring of another
# array name (seen inside removed_seen / seen_destination_names) is never
# rewritten as the wrong array (A9-07).  Names are additionally rewritten
# longest-first by the caller.
rewrite_assoc()
{
    local file="$1" name="$2"
    apply_sed "$file" \
        -e "s/\"\\\${!${name}\[@\]}\"/\\\$(legacy_assoc_keys ${name})/g" \
        -e "s/\\\${!${name}\[@\]}/\\\$(legacy_assoc_keys ${name})/g" \
        -e "s/\\\${${name}\[\"\([^\"]*\)\"\]:-0}/\\\$(legacy_assoc_get_num ${name} \"\\1\")/g" \
        -e "s/\\\${${name}\[\"\([^\"]*\)\"\]:-}/\\\$(legacy_assoc_get ${name} \"\\1\")/g" \
        -e "s/\\\${${name}\[\([^]]*\)\]:-0}/\\\$(legacy_assoc_get_num ${name} \"\\1\")/g" \
        -e "s/\\\${${name}\[\([^]]*\)\]:-}/\\\$(legacy_assoc_get ${name} \"\\1\")/g" \
        -e "s/\\\${${name}\[\"\([^\"]*\)\"\]}/\\\$(legacy_assoc_get ${name} \"\\1\")/g" \
        -e "s/\\\${${name}\[\([^]]*\)\]}/\\\$(legacy_assoc_get ${name} \"\\1\")/g" \
        -e "s/\<${name}\[\"\([^\"]*\)\"\]=/legacy_assoc_set ${name} \"\1\" /g" \
        -e "s/\<${name}\[\([^]]*\)\]=/legacy_assoc_set ${name} \"\1\" /g" \
        || return 1
}

# 1. Bash-4 syntax rewrites on the modern body.
transform_syntax()
{
    local file="$1"

    note "assoc declarations (declare/local -A)" "$(count_regex '^[[:space:]]*(declare|local) -A ' "$file")"
    apply_sed "$file" \
        -e 's/^\([[:space:]]*\)local -A seen=()$/\1local -a seen=() seen_keys=()/' \
        -e 's/^\([[:space:]]*\)local -A seen_destination_names=()$/\1local -a seen_destination_names=() seen_destination_names_keys=()/' \
        -e 's/^declare -A FS_SCOPE_DEVICE_INDEX=()$/declare -a FS_SCOPE_DEVICE_INDEX=() FS_SCOPE_DEVICE_INDEX_keys=()/' \
        -e 's/^\([[:space:]]*\)local -A reasons=()$/\1local -a reasons=() reasons_keys=()/' \
        -e 's/^\([[:space:]]*\)local -A destination_id=() destination_priority=()$/\1local -a destination_id=() destination_id_keys=() destination_priority=() destination_priority_keys=()/' \
        -e 's/^\([[:space:]]*\)local -A removed_seen=()$/\1local -a removed_seen=() removed_seen_keys=()/' \
        -e 's/^\([[:space:]]*\)local -A final_destination_count=()$/\1local -a final_destination_count=() final_destination_count_keys=()/' \
        -e 's/^\([[:space:]]*\)local -A order_index=() seen=()$/\1local -a order_index=() order_index_keys=() seen=() seen_keys=()/' \
        -e 's/^\([[:space:]]*\)local -A current_by_id=() current_by_key=() current_by_partition_label=()$/\1local -a current_by_id=() current_by_id_keys=() current_by_key=() current_by_key_keys=() current_by_partition_label=() current_by_partition_label_keys=()/' \
        -e 's/^\([[:space:]]*\)FS_SCOPE_DEVICE_INDEX=()$/\1FS_SCOPE_DEVICE_INDEX=() FS_SCOPE_DEVICE_INDEX_keys=()/' \
        -e 's/^\([[:space:]]*\)current_by_id=()$/\1current_by_id=() current_by_id_keys=()/' \
        -e 's/^\([[:space:]]*\)current_by_key=()$/\1current_by_key=() current_by_key_keys=()/' \
        -e 's/^\([[:space:]]*\)current_by_partition_label=()$/\1current_by_partition_label=() current_by_partition_label_keys=()/' \
        || return 1

    # populate_writable_dev_filtered (A9-01 batch) declares two associative
    # arrays with inline ['k']=v initializers: a shape the name-based sed
    # rewrites cannot handle.  The whole declaration block is replaced with
    # indexed arrays plus legacy_assoc_set calls (drift-gated: a modern edit
    # to the initializer list aborts generation, A11-01).
    replace_block "$file" \
        '    local -A allowed_nodes=() essential_by_dev=(
        ['"'"'1:3'"'"']=null ['"'"'1:5'"'"']=zero ['"'"'1:7'"'"']=full ['"'"'1:8'"'"']=random ['"'"'1:9'"'"']=urandom
        ['"'"'5:0'"'"']=tty ['"'"'5:1'"'"']=console
    )' \
        '    local -a allowed_nodes=() allowed_nodes_keys=()
    local -a essential_by_dev=() essential_by_dev_keys=()
    legacy_assoc_set essential_by_dev '"'"'1:3'"'"' null
    legacy_assoc_set essential_by_dev '"'"'1:5'"'"' zero
    legacy_assoc_set essential_by_dev '"'"'1:7'"'"' full
    legacy_assoc_set essential_by_dev '"'"'1:8'"'"' random
    legacy_assoc_set essential_by_dev '"'"'1:9'"'"' urandom
    legacy_assoc_set essential_by_dev '"'"'5:0'"'"' tty
    legacy_assoc_set essential_by_dev '"'"'5:1'"'"' console' \
        || return 1

    # The same function sets allowed_nodes with a nested-quoted value; the
    # generic set rewrite cannot keep the quoting shape, so the line is
    # replaced verbatim (drift-gated) before the name loop reaches it.
    replace_block "$file" \
        '                    allowed_nodes["$(readlink -f -- "$entry" 2>/dev/null || true)"]=1' \
        '                    legacy_assoc_set allowed_nodes "$(readlink -f -- "$entry" 2>/dev/null || true)" 1' \
        || return 1

    # Longest-first so a shorter name that is a substring of a longer one
    # (seen inside removed_seen / seen_destination_names) is rewritten before
    # the longer name is gone; the \< anchor in rewrite_assoc is the second,
    # independent guard (A9-07).
    local name
    for name in current_by_partition_label final_destination_count \
        seen_destination_names FS_SCOPE_DEVICE_INDEX destination_priority \
        destination_id current_by_key removed_seen current_by_id order_index \
        reasons seen essential_by_dev allowed_nodes; do
        note "assoc accesses: ${name}" \
            "$(( $(count_regex "[$][{]${name}[[]" "$file") \
                + $(count_regex "${name}\[[^]]*\]=" "$file") \
                + $(count_regex "[$][{]!${name}\[@\]}" "$file") ))"
        rewrite_assoc "$file" "$name" || return 1
    done

    # ${NAME[$k]+present} has no accessor shape: rewrite the single condition.
    replace_block "$file" \
        '        if [[ -n "$current_order" && -z "${order_index[$id]+present}" ]]; then' \
        '        if [[ -n "$current_order" && -z "$(legacy_assoc_has order_index "$id" && printf present)" ]]; then' \
        || return 1

    note "case conversion ,," "$(count_regex '\$\{[A-Za-z_][A-Za-z0-9_]*,,|\$\{[0-9]+,,' "$file")"
    note "case conversion ^^" "$(count_regex '\$\{[A-Za-z_][A-Za-z0-9_]*\^\^|\$\{[0-9]+\^\^' "$file")"
    note "case conversion ^" "$(count_regex '\$\{[A-Za-z_][A-Za-z0-9_]*\^\}|\$\{[0-9]+\^\}' "$file")"
    apply_sed "$file" \
        -e 's/\${\([A-Za-z_][A-Za-z0-9_]*\|[0-9][0-9]*\),,}/$(legacy_lc "$\1")/g' \
        -e 's/\${\([A-Za-z_][A-Za-z0-9_]*\|[0-9][0-9]*\)\^\^}/$(legacy_uc "$\1")/g' \
        -e 's/\${\([A-Za-z_][A-Za-z0-9_]*\|[0-9][0-9]*\)\^}/$(legacy_ucfirst "$\1")/g' \
        || return 1

    note "array [@] expansions (all forms)" "$(count_literal '[@]}' "$file")"
    apply_sed "$file" -e 's/\${\([A-Za-z_][A-Za-z0-9_]*\)\[@\]}/${\1[@]:-}/g' \
        || return 1

    note "mapfile call sites" "$(count_literal 'mapfile ' "$file")"
    apply_sed "$file" -e 's/\bmapfile /legacy_readarray /g' \
        || return 1

    note "sed -i -E" "$(count_literal 'sed -i -E ' "$file")"
    note "sed -nE" "$(count_literal 'sed -nE ' "$file")"
    note "sed -E" "$(count_literal 'sed -E ' "$file")"
    apply_sed "$file" \
        -e 's/sed -i -E /legacy_sed_ext -i /g' \
        -e 's/sed -nE /legacy_sed_ext -n /g' \
        -e 's/sed -E /legacy_sed_ext /g' \
        || return 1

    note "sort -V" "$(count_literal 'sort -V' "$file")"
    apply_sed "$file" -e 's/sort -V/legacy_sort_versions/g' \
        || return 1

    note "date --iso-8601" "$(count_literal '$(date --iso-8601=seconds 2>/dev/null || date)' "$file")"
    apply_sed "$file" -e 's#\$(date --iso-8601=seconds 2>/dev/null || date)#$(legacy_date_iso)#g' \
        || return 1
}

# 1c. A12-05: bash 3.1.17 corrupts its /dev/fd fifo bookkeeping
# (subst.c add_fifo_list: "malloc: ../bash/subst.c:4135: assertion botched",
# SIGABRT/rc 134 - reproduced on the Etch rig) when process substitution runs
# repeatedly inside a loop.  The legacy mount sweep calls these sites once
# per swept /dev entry (530+ on the Etch rig, ~4 process substitutions per
# entry across this site, top_disks_for, _top_disks_for_sysfs and the lsblk
# shim), which crashed every rw repair right after the target data mounts
# were promoted.  Each site switches its feed from `< <(producer)` to the
# here-string `<<<"$(producer)"` (here-strings use a temp file, never a
# fifo; verified crash-free over thousands of iterations on bash 3.1.17).
# The legacy_readarray empty-record skip (compat.sh A12-01) keeps an empty
# producer from adding a spurious "" element through the here-string's
# trailing newline.
transform_legacy_mount_sweep()
{
    local file="$1" count=0

    sweep_replacement()
    {
        local old="$1" new="$2"
        replace_block "$file" "$old" "$new" || return 1
        count=$((count + 1))
    }

    # populate_writable_dev_filtered: the per-entry top-disk walk (the
    # hottest site: once per swept /dev entry).
    sweep_replacement \
        '                legacy_readarray -t entry_tops < <(top_disks_for "$entry" 2>/dev/null | sort -u || true)' \
        '                legacy_readarray -t entry_tops <<<"$(top_disks_for "$entry" 2>/dev/null | sort -u || true)"'

    # top_disks_for: the per-call lsblk inverse walk.
    sweep_replacement \
        '    legacy_readarray -t disks < <(
        lsblk -srnpo NAME,TYPE "$dev" 2>/dev/null |
            awk '"'"'$2 == "disk" {print $1}'"'"' |
            while IFS= read -r disk; do
                canonical_block "$disk" 2>/dev/null || true
            done |
            awk '"'"'NF'"'"' |
            sort -u
    )' \
        '    legacy_readarray -t disks <<<"$(
        lsblk -srnpo NAME,TYPE "$dev" 2>/dev/null |
            awk '"'"'$2 == "disk" {print $1}'"'"' |
            while IFS= read -r disk; do
                canonical_block "$disk" 2>/dev/null || true
            done |
            awk '"'"'NF'"'"' |
            sort -u
    )"'

    # _top_disks_for_sysfs: PKNAME parents.
    sweep_replacement \
        '        while IFS= read -r parent; do
            [[ -n "$parent" ]] && parents+=("/dev/$parent")
        done < <(lsblk -ndo PKNAME "$current" 2>/dev/null | awk '"'"'NF'"'"' | sort -u)' \
        '        while IFS= read -r parent; do
            [[ -n "$parent" ]] && parents+=("/dev/$parent")
        done <<<"$(lsblk -ndo PKNAME "$current" 2>/dev/null | awk '"'"'NF'"'"' | sort -u)"'

    # _top_disks_for_sysfs: sysfs slave links.
    sweep_replacement \
        '            while IFS= read -r parent; do
                [[ -n "$parent" ]] && parents+=("/dev/$parent")
            done < <(find "/sys/class/block/$kname/slaves" -mindepth 1 -maxdepth 1 -printf '"'"'%f\n'"'"' 2>/dev/null | sort -u)' \
        '            while IFS= read -r parent; do
                [[ -n "$parent" ]] && parents+=("/dev/$parent")
            done <<<"$(find "/sys/class/block/$kname/slaves" -mindepth 1 -maxdepth 1 -printf '"'"'%f\n'"'"' 2>/dev/null | sort -u)"'

    # _top_disks_for_sysfs: the multi-parent re-sort.
    sweep_replacement \
        '        legacy_readarray -t parents < <(printf '"'"'%s\n'"'"' "${parents[@]:-}" | sort -u)' \
        '        legacy_readarray -t parents <<<"$(printf '"'"'%s\n'"'"' "${parents[@]:-}" | sort -u)"'

    # populate_writable_dev_filtered: the allowed-top-disk capture (once per
    # prepare, but the sweep's own hot path calls the same top_disks_for).
    sweep_replacement \
        '        legacy_readarray -t allowed_tops < <(top_disks_for "$TARGET_DISK" 2>/dev/null | sort -u || true)' \
        '        legacy_readarray -t allowed_tops <<<"$(top_disks_for "$TARGET_DISK" 2>/dev/null | sort -u || true)"'

    note "mount-sweep process substitutions rewritten" "$count"
}

# 1b. bash 3.1 rejects an unquoted `(` or `|` in the `[[ =~ ]]` operand
# (bash 3.2+ accepts it).  Hoist those regex literals into a variable, which is
# the portable idiom on 3.1 and on modern bash alike.  A block that matches
# nothing aborts the function (the sentinel is needed because this function
# runs with errexit suppressed by the caller's condition chain).
transform_regex_compat()
{
    local file="$1" count=0

    regex_replacement()
    {
        local old="$1" new="$2"
        (( count >= 0 )) || return 0
        replace_block "$file" "$old" "$new" || { count=-1; return 1; }
        count=$((count + 1))
    }

    regex_replacement \
        '        while [[ "$rest" =~ ([A-Z]+)=\"([^\"]*)\" ]]; do' \
        '        __legacy_re='"'"'([A-Z]+)=\"([^\"]*)\"'"'"'
        while [[ "$rest" =~ $__legacy_re ]]; do'

    regex_replacement \
        '    [[ "$TARGET_OS_ID" =~ ^(debian|ubuntu|tuxedo|linuxmint|pop)$ ]] || [[ " $TARGET_OS_LIKE " == *" debian "* ]] || [[ " $TARGET_OS_LIKE " == *" ubuntu "* ]]' \
        '    __legacy_re='"'"'^(debian|ubuntu|tuxedo|linuxmint|pop)$'"'"'
    [[ "$TARGET_OS_ID" =~ $__legacy_re ]] || [[ " $TARGET_OS_LIKE " == *" debian "* ]] || [[ " $TARGET_OS_LIKE " == *" ubuntu "* ]]'

    regex_replacement \
        '    [[ "$TARGET_OS_ID" =~ ^(arch|manjaro|endeavouros|garuda|artix)$ ]] \
        || [[ " $TARGET_OS_LIKE " == *" arch "* ]]' \
        '    __legacy_re='"'"'^(arch|manjaro|endeavouros|garuda|artix)$'"'"'
    [[ "$TARGET_OS_ID" =~ $__legacy_re ]] \
        || [[ " $TARGET_OS_LIKE " == *" arch "* ]]'

    regex_replacement \
        '    elif [[ "$TARGET_OS_ID" =~ ^(fedora|rhel|rocky|almalinux)$ ]] \
        || [[ " $TARGET_OS_LIKE " == *" fedora "* ]] \
        || [[ " $TARGET_OS_LIKE " == *" rhel "* ]]; then' \
        '    elif __legacy_re='"'"'^(fedora|rhel|rocky|almalinux)$'"'"'; [[ "$TARGET_OS_ID" =~ $__legacy_re ]] \
        || [[ " $TARGET_OS_LIKE " == *" fedora "* ]] \
        || [[ " $TARGET_OS_LIKE " == *" rhel "* ]]; then'

    regex_replacement \
        '    elif [[ "$TARGET_OS_ID" =~ ^(opensuse|opensuse-tumbleweed|suse)$ ]] \
        || [[ " $TARGET_OS_LIKE " == *" suse "* ]]; then' \
        '    elif __legacy_re='"'"'^(opensuse|opensuse-tumbleweed|suse)$'"'"'; [[ "$TARGET_OS_ID" =~ $__legacy_re ]] \
        || [[ " $TARGET_OS_LIKE " == *" suse "* ]]; then'

    regex_replacement \
        '            if [[ "$line" =~ ^[[:space:]]+([^[:space:]]+) ]]; then' \
        '            __legacy_re='"'"'^[[:space:]]+([^[:space:]]+)'"'"'
            if [[ "$line" =~ $__legacy_re ]]; then'

    regex_replacement \
        '        if [[ "$line" =~ (^|[[:space:]])([0-9]+)[[:space:]]+not[[:space:]]+upgraded\.?[[:space:]]*$ ]]; then' \
        '        __legacy_re='"'"'(^|[[:space:]])([0-9]+)[[:space:]]+not[[:space:]]+upgraded\.?[[:space:]]*$'"'"'
        if [[ "$line" =~ $__legacy_re ]]; then'

    regex_replacement \
        '        if [[ "$line" =~ ^[[:space:]]*Skipping[[:space:]]+packages[[:space:]]+with[[:space:]]+(conflicts|broken[[:space:]]+dependencies):[[:space:]]*$ ]]; then' \
        '        __legacy_re='"'"'^[[:space:]]*Skipping[[:space:]]+packages[[:space:]]+with[[:space:]]+(conflicts|broken[[:space:]]+dependencies):[[:space:]]*$'"'"'
        if [[ "$line" =~ $__legacy_re ]]; then'

    regex_replacement \
        '        if [[ "$line" =~ ^[[:space:]]*Skipping:[[:space:]]+([0-9]+)[[:space:]]+packages?[[:space:]]*$ ]]; then' \
        '        __legacy_re='"'"'^[[:space:]]*Skipping:[[:space:]]+([0-9]+)[[:space:]]+packages?[[:space:]]*$'"'"'
        if [[ "$line" =~ $__legacy_re ]]; then'

    regex_replacement \
        '        [[ "$line" =~ ^[[:space:]]*([^[:space:]]+) ]] || continue' \
        '        __legacy_re='"'"'^[[:space:]]*([^[:space:]]+)'"'"'
        [[ "$line" =~ $__legacy_re ]] || continue'

    regex_replacement \
        '        if [[ "$line" =~ ^[[:space:]][[:space:]]+replacing[[:space:]]+([^[:space:]]+) ]]; then' \
        '        __legacy_re='"'"'^[[:space:]][[:space:]]+replacing[[:space:]]+([^[:space:]]+)'"'"'
        if [[ "$line" =~ $__legacy_re ]]; then'

    (( count >= 0 )) || return 1
    note "=~ regex literal hoists" "$count"
}

# 2. Legacy root evidence gates and dpkg status fallbacks.
transform_legacy_behaviour()
{
    local file="$1"

    note "os-release gates" "$(count_literal '[[ ! -f "$TARGET_ROOT/etc/os-release" ]]' "$file")"
    replace_block "$file" \
        '    if [[ ! -f "$TARGET_ROOT/etc/os-release" && "$fstype" == "btrfs" ]]; then' \
        '    if ! legacy_root_evidence_present "$TARGET_ROOT" && [[ "$fstype" == "btrfs" ]]; then' \
        || return 1
    replace_block "$file" \
        '    if [[ ! -f "$TARGET_ROOT/etc/os-release" ]]; then' \
        '    if ! legacy_root_evidence_present "$TARGET_ROOT"; then' \
        || return 1
    replace_block "$file" \
        '    [[ -f "$probe_dir/etc/os-release" ]] && found=0' \
        '    legacy_root_evidence_present "$probe_dir" && found=0' \
        || return 1

    # Cycle 9 loop 2: the unlock verb accepts an optional --key-file <path>
    # so the Qt3 GUI can deliver the LUKS passphrase through a mode-600 file
    # (Qt 3.3.7 QProcess cannot deliver stdin). Without the argument the
    # stdin channel is unchanged.
    replace_block "$file" \
        '        unlock)
            (($# == 0)) || fail "unlock does not accept extra arguments."
            unlock_target
            ;;' \
        '        unlock)
            if (($# == 0)); then
                unlock_target
            else
                [[ "$1" == "--key-file" ]] || fail "unlock does not accept extra arguments."
                case "$#" in
                    2) unlock_target --key-file "$2" ;;
                    4) [[ "$3" == "--key-owner" ]] || fail "unlock accepts only --key-file and --key-owner arguments."
                       unlock_target --key-file "$2" --key-owner "$4" ;;
                    *) fail "unlock --key-file requires exactly one path argument (plus an optional --key-owner uid)." ;;
                esac
            fi
            ;;' \
        || return 1

    # A9-02: the legacy unlock pre-creates the session directory (root-owned,
    # mode 0700, see legacy_unlock_ensure_session) so the unlock keyfile never
    # lands at /unlock-keyfile.  prepare_target must REUSE that directory
    # instead of replacing it with a fresh session; prepare_running_host keeps
    # its own unconditional creation (MOUNT_BASE="/" disambiguates the two
    # blocks, so only prepare_target is rewritten).
    replace_block "$file" \
        '    SESSION_DIR="$(mktemp -d "$STATE_ROOT/session.XXXXXX")"
    MOUNT_BASE="$SESSION_DIR/mount"' \
        '    [[ -n "$SESSION_DIR" ]] || SESSION_DIR="$(mktemp -d "$STATE_ROOT/session.XXXXXX")"
    MOUNT_BASE="$SESSION_DIR/mount"' \
        || return 1

    note "dpkg db:Status sites" "$(count_literal 'db:Status' "$file")"
    replace_block "$file" \
        '        desktop_status="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
            dpkg-query -W -f='"'"'${db:Status-Status}'"'"' tuxedoos-desktop 2>/dev/null || true)"' \
        '        desktop_status="$(legacy_dpkg_status_field tuxedoos-desktop)"' \
        || return 1
    replace_block "$file" \
        '            status="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
                dpkg-query -W -f='"'"'${db:Status-Status} ${Version}'"'"' "$pkg" 2>/dev/null || true)"' \
        '            status="$(legacy_dpkg_status_version "$pkg")"' \
        || return 1

    # A10-05: optional leading cancel-token options, accepted before the
    # command verb (the Qt3 GUI passes --cancel-file <path>).
    replace_block "$file" \
        '    register_active_child "$$"

    (($# >= 1)) || { usage; exit 2; }
    local command="$1"; shift' \
        '    register_active_child "$$"

    # A10-05: optional leading cancel-token options, accepted before the
    # command verb (the Qt3 GUI passes --cancel-file <path>; the documented
    # token form is CANCEL_TOKEN=<value> in the environment or
    # --cancel-token <value> on the command line).
    while (($# > 0)); do
        case "$1" in
            --cancel-file)
                [[ $# -ge 2 ]] || fail "--cancel-file requires a path."
                CANCEL_FILE="$2"
                shift 2
                ;;
            --cancel-token)
                [[ $# -ge 2 ]] || fail "--cancel-token requires a value."
                CANCEL_TOKEN="$2"
                shift 2
                ;;
            *)
                break
                ;;
        esac
    done

    (($# >= 1)) || { usage; exit 2; }
    local command="$1"; shift' \
        || return 1

    # A10-05: document the cancel surface in the helper usage text.
    replace_block "$file" \
        'Stage mode hints:
  --post-efi       The caller already ran the EFI/UKI stage for this scope in
                   the same plan/session.  boot-stack then skips its second
                   UKI/EFI rebuild and the duplicate GRUB regeneration while
                   still validating mapper/crypttab and reconciling initramfs.
                   The helper never infers this from the stage list.

Shell:' \
        'Stage mode hints:
  --post-efi       The caller already ran the EFI/UKI stage for this scope in
                   the same plan/session.  boot-stack then skips its second
                   UKI/EFI rebuild and the duplicate GRUB regeneration while
                   still validating mapper/crypttab and reconciling initramfs.
                   The helper never infers this from the stage list.

Cancellation:
  --cancel-file <path>    Polled cancel surface accepted before the command
                   verb: the helper aborts at the next stage boundary or
                   shell-command tick when the file appears (the GUI'"'"'s
                   cancel() touches it), with the full session cleanup.
  --cancel-token <value>  Token form: the helper aborts when
                   $SESSION_DIR/cancel appears and carries exactly this value.
  CANCEL_TOKEN=<value>    Environment form of --cancel-token.

Shell:' \
        || return 1

    # A9-09 follow-up (rc preservation): the legacy overlay defines its own
    # cleanup() wrapper (resolver restore + the modern teardown) and the
    # modern body is renamed to cleanup_modern later.  cleanup_modern
    # re-captures $? on entry, but the wrapper's resolver if-block resets it
    # to 0, so every helper run that failed via fail() (or was killed with
    # INT/TERM/HUP) exited 0 — the fail-closed rc contract broke.  The
    # wrapper exports the original status as LEGACY_CLEANUP_RC; this rewrite
    # makes the renamed body read it first and fall back to $? otherwise
    # (drift-gated: a modern edit to the capture line aborts generation,
    # A11-01).  The rewrite targets the pre-rename name because this
    # transform runs before transform_renames.
    replace_block "$file" \
        'cleanup()
{
    local rc=$?' \
        'cleanup()
{
    local rc="${LEGACY_CLEANUP_RC:-$?}"' \
        || return 1
}

# 3. Rename the modern definitions the overlay wraps or replaces.
transform_renames()
{
    local file="$1"
    local funcs="dpkg_configuration_pending target_package_installed read_target_os grub_config_path grub_unavailable_reason adaptive_grub_repair efi_unavailable_reason bootstack_unavailable_reason diagnostic_repair_capabilities config_path_for_key mount_special prepare_host_command_guard run_file_copy run_chroot_shell run_host_shell run_snapshots run_host_snapshots run_host_default run_host_repair run_host_reboot run_host_diagnostic validate_running_host fs_inspect fs_repair display_unavailable_reason adaptive_display_manager_repair run_selected_chroot repair_capability_evidence unlock_target mount_recorded resolve_fstab_source repair_boot_stack host_default_unavailable_reason browse_target_directory filesystem_release_all_mounts filesystem_mountpoint_for_device mount_target_resolver find_crypt_mapper_for_device cleanup run_package_stage adaptive_initramfs_repair adaptive_grub_stage"
    local func count=0
    for func in $funcs; do
        grep -qE "^${func}\(\)$" "$file" || {
            printf 'ERROR: expected function definition not found: %s()\n' "$func" >&2
            return 1
        }
        count=$((count + 1))
    done
    note "wrapped/replaced modern functions" "$count"
    for func in $funcs; do
        apply_sed "$file" -e "s/^${func}()$/${func}_modern()/" || return 1
    done
}

# Post-rewrite assertions (A11-04): after transform_syntax the ported body
# must hold zero residual modern constructs and exactly the audited shim-call
# totals.  A comment, heredoc or new upstream construct the transforms miss
# fails generation here instead of shipping a helper that dies on bash 3.1.
verify_transform_counts()
{
    local file="$1" rc=0 n

    check_zero()
    {
        local label="$1" pattern="$2"
        n="$(grep -oE -- "$pattern" "$file" 2>/dev/null | wc -l | tr -d '[:space:]')"
        note "post-rewrite residual: $label" "$n"
        if [[ "$n" != 0 ]]; then
            printf 'ERROR: %s residual %s remained in the ported body\n' "$n" "$label" >&2
            rc=1
        fi
    }

    check_count()
    {
        local label="$1" pattern="$2" want="$3"
        n="$(grep -oE -- "$pattern" "$file" 2>/dev/null | wc -l | tr -d '[:space:]')"
        note "shim calls: $label" "$n"
        if [[ "$n" != "$want" ]]; then
            printf 'ERROR: %s count drifted: expected %s, got %s\n' "$label" "$want" "$n" >&2
            rc=1
        fi
    }

    check_zero 'mapfile' 'mapfile'
    check_zero 'sed -i -E' 'sed -i -E'
    check_zero 'sed -nE' 'sed -nE'
    check_zero 'sed -E' 'sed -E'
    check_zero 'sort -V' 'sort -V'
    check_zero 'date --iso-8601' 'date --iso-8601'
    check_zero 'case conversion' '[$][{][0-9A-Za-z_][0-9A-Za-z_]*(\^\^|\^|,,)[}]'
    check_zero 'corrupted assoc set' 'removed_legacy_assoc_set|removed_seen_legacy'
    check_count 'legacy_readarray' 'legacy_readarray' 60
    check_count 'legacy_sed_ext' 'legacy_sed_ext' 81
    check_count 'legacy_sort_versions' 'legacy_sort_versions' 12
    check_count 'legacy_date_iso' 'legacy_date_iso' 4
    check_count 'legacy_lc' 'legacy_lc ' 64
    check_count 'legacy_uc' 'legacy_uc ' 15
    check_count 'legacy_ucfirst' 'legacy_ucfirst ' 23
    check_count 'legacy_assoc_get' 'legacy_assoc_get ' 26
    check_count 'legacy_assoc_get_num' 'legacy_assoc_get_num ' 2
    check_count 'legacy_assoc_keys' 'legacy_assoc_keys ' 1
    check_count 'legacy_assoc_has' 'legacy_assoc_has ' 1
    check_count 'legacy_assoc_set' 'legacy_assoc_set ' 56
    check_count 'legacy_assoc_unset' 'legacy_assoc_unset ' 0
    return "$rc"
}

# Every function name must be defined exactly once in the generated helper
# (A11-08).  The modern source may carry its own intentional aliases (today:
# alpine_openrc_present), so a duplicate is only rejected when the port
# pipeline introduced it — the overlay's dead run_chroot_shell/run_host_shell
# pair is the failure this catches.
verify_function_definitions()
{
    local file="$1" modern="$2" rc=0 name
    local modern_dups gen_dups count=0
    modern_dups="$(grep -oE '^[A-Za-z_][A-Za-z0-9_]*\(\)[[:space:]]*$' "$modern" \
        | sed -e 's/[()]//g' -e 's/[[:space:]]*$//' | sort | uniq -d)"
    gen_dups="$(grep -oE '^[A-Za-z_][A-Za-z0-9_]*\(\)[[:space:]]*$' "$file" \
        | sed -e 's/[()]//g' -e 's/[[:space:]]*$//' | sort | uniq -d)"
    for name in $gen_dups; do
        case " $modern_dups " in
            *" $name "*) ;;
            *)
                printf 'ERROR: function defined more than once in the generated helper: %s\n' "$name" >&2
                rc=1
                count=$((count + 1))
                ;;
        esac
    done
    note "port-introduced duplicate functions" "$count"
    return "$rc"
}

assemble()
{
    local out="$1" body="$2" prelude="$3" overlay="$4" modern="${5:-}"
    local path_line prelude_lines overlay_lines total body_lines tmp
    path_line="$(awk '/^export PATH$/{print NR; exit}' "$body")"
    [[ -n "$path_line" ]] || { printf 'ERROR: export PATH anchor not found\n' >&2; return 1; }
    total="$(wc -l < "$body" | tr -d '[:space:]')"
    [[ "$(sed -n "${total}p" "$body")" == 'main "$@"' ]] \
        || { printf 'ERROR: last line is not main "$@"\n' >&2; return 1; }
    body_lines=$((total - 1))
    prelude_lines="$(wc -l < "$prelude" | tr -d '[:space:]')"
    overlay_lines="$(wc -l < "$overlay" | tr -d '[:space:]')"
    note "prelude (legacy/compat.sh)" "${prelude_lines} lines"
    note "overlay (legacy/overlay.sh)" "${overlay_lines} lines"
    # A11-10: write to a temp file in the output directory, verify the
    # assembled content and only then move it into place, so the committed
    # helper is either the complete new file or the untouched old one — never
    # a partial or unverified write.  The temp is cleaned up on interrupt.
    tmp="$(mktemp "${out}.tmp.XXXXXX")" || return 1
    trap 'rm -f -- "$tmp"; trap - HUP INT TERM; exit 130' HUP INT TERM
    if ! {
        # The generated body assigns arrays through legacy_readarray and
        # legacy_assoc_* and converts case through legacy_lc/uc/ucfirst, which
        # ShellCheck cannot follow; the file-wide directive must sit directly
        # after the shebang and before any command.  The port source files
        # (compat.sh, overlay.sh, port.sh) stay ShellCheck-clean.
        head -n 1 "$body"
        printf '# shellcheck disable=SC2034,SC2120,SC2154,SC2155\n'
        printf '# Generated by legacy/port.sh: dynamic array assignment and probed\n'
        printf '# case conversion are invisible to ShellCheck; see legacy/compat.sh.\n'
        sed -n "2,${path_line}p" "$body"
        printf '\n'
        printf '# ===========================================================================\n'
        printf '# Legacy compatibility prelude — generated from legacy/compat.sh by legacy/port.sh.\n'
        printf '# ===========================================================================\n'
        cat "$prelude"
        printf '# ===========================================================================\n'
        printf '# End legacy compatibility prelude.\n'
        printf '# ===========================================================================\n'
        tail -n "+$((path_line + 1))" "$body" | head -n "$((body_lines - path_line))"
        printf '\n'
        printf '# ===========================================================================\n'
        printf '# Legacy-only behaviour — generated from legacy/overlay.sh by legacy/port.sh.\n'
        printf '# ===========================================================================\n'
        cat "$overlay"
        printf 'main "$@"\n'
    } > "$tmp"; then
        rm -f -- "$tmp"
        trap - HUP INT TERM
        return 1
    fi
    # The assembled temp must pass every gate before it is allowed to replace
    # the committed helper.
    if ! verify_function_definitions "$tmp" "$modern"; then
        rm -f -- "$tmp"
        trap - HUP INT TERM
        return 1
    fi
    if ! verify_no_bash4 "$tmp"; then
        rm -f -- "$tmp"
        trap - HUP INT TERM
        return 1
    fi
    if ! mv -- "$tmp" "$out"; then
        rm -f -- "$tmp"
        trap - HUP INT TERM
        return 1
    fi
    trap - HUP INT TERM
}

# Bash-4 (and later) construct blacklist applied to the assembled helper.
# Each entry is KIND~LABEL~PATTERN (F = fixed string, R = ERE).  The function
# is self-contained (grep/printf only) so the contract test can extract and
# source it to unit-test the blacklist against fixtures (A11-02).  Full-line
# comments are excluded before scanning.
verify_no_bash4()
{
    local file="$1" rc=0 kind label pattern
    local code
    code="$(grep -vE '^[[:space:]]*#' "$file")"
    while IFS='~' read -r kind label pattern; do
        [[ -n "$pattern" ]] || continue
        if [[ "$kind" == F ]]; then
            if printf '%s\n' "$code" | grep -qF -- "$pattern"; then
                printf 'ERROR: bash-4 construct remains in %s: %s\n' "$file" "$label" >&2
                rc=1
            fi
        else
            if printf '%s\n' "$code" | grep -qE -- "$pattern"; then
                printf 'ERROR: bash-4 construct remains in %s: %s\n' "$file" "$label" >&2
                rc=1
            fi
        fi
    done <<'VERIFY_NO_BASH4_BLACKLIST'
F~mapfile builtin~mapfile
F~declare -A~declare -A
F~local -A~local -A
F~coproc~coproc
F~printf -v~printf -v
F~case fallthrough ;&~;&
F~case fallthrough ;;&~;;&
F~pipe both streams |&~|&
R~&> / &>> append both streams~(^|[^&])&>>?
R~[[ -v var ]] test~\[\[[^]]*[[:space:]]-v[[:space:]]
R~declare -g~(^|[^_[:alnum:]])declare[[:space:]]+-g([[:space:]]|$)
R~local -g~(^|[^_[:alnum:]])local[[:space:]]+-g([[:space:]]|$)
R~globstar~(^|[^_[:alnum:]])globstar([^_[:alnum:]]|$)
R~readarray builtin~(^|[^_[:alnum:]])readarray([^_[:alnum:]]|$)
R~wait -n~(^|[^_[:alnum:]])wait[[:space:]]+-n([^_[:alnum:]]|$)
R~local -n~(^|[^_[:alnum:]])local[[:space:]]+-n([^_[:alnum:]]|$)
R~negative array subscript~[$][{][A-Za-z_][A-Za-z0-9_]*\[-[0-9]+\][}]
R~${var@Q} transform~[$][{][A-Za-z_][A-Za-z0-9_]*@[QAEPaK][}]
R~indirect ${!prefix@}~[$][{][!][A-Za-z_][A-Za-z0-9_]*[@*]([^]]|$)
R~case conversion ,,~[$][{][0-9A-Za-z_][0-9A-Za-z_]*,,[}]
R~case conversion ^^~[$][{][0-9A-Za-z_][0-9A-Za-z_]*\^\^[}]
R~case conversion ^~[$][{][0-9A-Za-z_][0-9A-Za-z_]*\^[}]
VERIFY_NO_BASH4_BLACKLIST
    return "$rc"
}

generate()
{
    local out="$1" work
    work="$(mktemp "${TMPDIR:-/tmp}/port-body.XXXXXX")" || return 1
    if ! cp -- "$MODERN" "$work" \
        || ! transform_syntax "$work" \
        || ! transform_legacy_mount_sweep "$work" \
        || ! verify_transform_counts "$work" \
        || ! transform_regex_compat "$work" \
        || ! transform_legacy_behaviour "$work" \
        || ! transform_renames "$work"; then
        rm -f -- "$work"
        return 1
    fi
    # assemble() verifies the assembled temp (function definitions, bash-4
    # blacklist) before atomically moving it into place.
    if ! assemble "$out" "$work" "$COMPAT" "$OVERLAY" "$MODERN"; then
        rm -f -- "$work"
        return 1
    fi
    rm -f -- "$work"
    chmod 0755 "$out" 2>/dev/null || true
}

main()
{
    local target="" rc=0
    [[ -f "$MODERN" && -f "$COMPAT" && -f "$OVERLAY" ]] \
        || { printf 'ERROR: missing input file (modern helper, compat.sh or overlay.sh)\n' >&2; exit 1; }

    if (( CHECK )); then
        printf 'port-helper-legacy: checking %s\n' "${OUTPUT#"$ROOT_DIR"/}"
        target="$(mktemp "${TMPDIR:-/tmp}/boot-repair-helper-legacy.XXXXXX")" || exit 1
        if ! generate "$target"; then
            rm -f -- "$target"
            printf 'port-helper-legacy: GENERATION FAILED\n' >&2
            exit 1
        fi
        if [[ -f "$OUTPUT" ]] && cmp -s "$OUTPUT" "$target"; then
            printf 'port-helper-legacy: in sync (%s bytes)\n' "$(wc -c < "$OUTPUT" | tr -d '[:space:]')"
        else
            printf 'port-helper-legacy: DRIFT — regenerate with legacy/port.sh\n' >&2
            if [[ -f "$OUTPUT" ]]; then
                diff -u "$OUTPUT" "$target" | head -n 60 >&2 || true
            else
                printf '  %s is missing\n' "${OUTPUT#"$ROOT_DIR"/}" >&2
            fi
            rc=1
        fi
        rm -f -- "$target"
        return "$rc"
    fi

    printf 'port-helper-legacy: generating %s\n' "${OUTPUT#"$ROOT_DIR"/}"
    generate "$OUTPUT"
    printf 'port-helper-legacy: wrote %s (%s bytes)\n' \
        "${OUTPUT#"$ROOT_DIR"/}" "$(wc -c < "$OUTPUT" | tr -d '[:space:]')"
}

main "$@"
