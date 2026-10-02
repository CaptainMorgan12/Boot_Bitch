#!/usr/bin/env bash
set -Eeuo pipefail

# Boot Bitch test manifest and budget gate.
#
# The manifest (tests/test-manifest.tsv) declares every registered ctest test
# with its tier, measured cost and quick-gate membership, and carries per-tier
# budget directives. This script validates that contract against CMakeLists.txt
# (add_test names and set_tests_properties LABELS) and against the exceptions
# file (tests/test-budget-exceptions.tsv), which holds budget approvals.
#
# Subcommands:
#   check [--quick]           validate format, registration and budgets;
#                             --quick additionally enforces the quick budget
#   update                    regenerate the manifest rows from CMakeLists.txt,
#                             preserving declared cost_s (new rows get cost_s 0)
#   approve <test> '<note>'   approve a budget overage: append an exception row
#                             and print a BUDGET-APPROVED line
#   quick-exclude             print the -E regex of the fast quick=n rows
#                             (dev-check quick ctest tier); empty when none
#   quick-excluded            print the same rows' short names joined with '/'
#                             (dev-check skipped-list text)
#
# `check` prints exactly one actionable line per failure and exits 1; on
# success it prints the budget summary (and a BUDGET-APPROVED line per
# exception row) and exits 0. A CMakeLists.txt test registration it cannot
# parse is a hard failure: "cannot parse test registration near line N".

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
# The tree root is the nearest CMakeLists.txt above the script, so this also
# works from a sandbox copy of the tests/ directory.
ROOT_DIR="$SCRIPT_DIR"
while [[ ! -f "$ROOT_DIR/CMakeLists.txt" && "$ROOT_DIR" != "/" ]]; do
    ROOT_DIR="$(dirname -- "$ROOT_DIR")"
done
[[ -f "$ROOT_DIR/CMakeLists.txt" ]] || {
    echo "ERROR: cannot find the Boot Bitch tree root above $SCRIPT_DIR" >&2
    exit 1
}
MANIFEST="$SCRIPT_DIR/test-manifest.tsv"
EXCEPTIONS="$SCRIPT_DIR/test-budget-exceptions.tsv"
CMAKELISTS="$ROOT_DIR/CMakeLists.txt"

BUDGET_TIERS=(fast quick ui slow packaging)
KNOWN_TIERS=(fast ui slow packaging)

usage()
{
    cat <<'EOF'
Usage: tests/test-manifest.sh <subcommand> [args]

  check [--quick]           validate manifest format, registration and budgets
                            (--quick also enforces the quick-gate budget)
  update                    regenerate manifest rows from CMakeLists.txt,
                            preserving cost_s (new tests get cost_s 0)
  approve <test> '<note>'   approve a budget overage (appends an exception row)
  quick-exclude             print the -E regex of the fast quick=n rows
  quick-excluded            print their short names joined with '/'
  -h, --help                show this help
EOF
}

# ---------------------------------------------------------------------------
# CMakeLists.txt registration parse (awk). Emits, in file order:
#   TEST <name> <line>
#   LABEL <name> <label>[,<label>...]
# Anything else that looks like a test registration but does not fit these two
# forms is a hard parse failure on stderr and exit status 2.
# ---------------------------------------------------------------------------
parse_cmake()
{
    awk '
        function fail(line) {
            printf "cannot parse test registration near line %d\n", line > "/dev/stderr"
            exit 2
        }
        # process only non-blank, non-comment lines
        $0 !~ /^[[:space:]]*(#|$)/ {
            if ($0 ~ /add_test[[:space:]]*\(/) {
                start = NR
                buf = $0
                if (buf !~ /add_test[[:space:]]*\([[:space:]]*NAME[[:space:]]+/) {
                    while (buf !~ /\)/ && (getline > 0)) buf = buf " " $0
                    if (buf !~ /add_test[[:space:]]*\([[:space:]]*NAME[[:space:]]+/) fail(start)
                }
                rest = buf
                sub(/^.*add_test[[:space:]]*\([[:space:]]*/, "", rest)
                sub(/^NAME[[:space:]]+/, "", rest)
                sub(/[[:space:])].*$/, "", rest)
                if (rest == "") fail(start)
                printf "TEST %s %d\n", rest, start
                next
            }
            if ($0 ~ /set_tests_properties[[:space:]]*\(/) {
                start = NR
                buf = $0
                while (buf !~ /\)/ && (getline > 0)) buf = buf " " $0
                rest = buf
                sub(/^.*set_tests_properties[[:space:]]*\(/, "", rest)
                sub(/[[:space:])].*$/, "", rest)
                if (rest == "") fail(start)
                labs = ""
                if (match(buf, /LABELS[[:space:]]+("[^"]*"[[:space:]]*)+/)) {
                    labs = substr(buf, RSTART, RLENGTH)
                    sub(/^LABELS[[:space:]]+/, "", labs)
                    gsub(/"/, "", labs)
                    gsub(/[[:space:]]+/, ",", labs)
                    sub(/^,/, "", labs)
                    sub(/,$/, "", labs)
                }
                if (labs == "") fail(start)
                printf "LABEL %s %s\n", rest, labs
                next
            }
        }
    ' "$CMAKELISTS"
}

# Lenient single pass over the manifest: header comment lines, budget
# directives and well-formed rows. Malformed rows are ignored here; `check`
# re-reads the file strictly with line numbers. Globals: M_HEADER, BUDGETS,
# M_TIER, M_COST, M_OWNER, M_QUICK, M_ORDER.
read_manifest()
{
    M_HEADER=()
    M_ORDER=()
    BUDGETS=()
    M_TIER=()
    M_COST=()
    M_OWNER=()
    M_QUICK=()
    local raw fields
    while IFS= read -r raw; do
        if [[ "$raw" == '#'* ]]; then
            M_HEADER+=("$raw")
            if [[ "$raw" =~ ^#[[:space:]]*budget[[:space:]]+([a-z]+)[[:space:]]+([0-9]+)[[:space:]]*$ ]]; then
                BUDGETS["${BASH_REMATCH[1]}"]="${BASH_REMATCH[2]}"
            fi
            continue
        fi
        if [[ -z "$raw" ]]; then
            M_HEADER+=("")
            continue
        fi
        IFS=$'\t' read -r -a fields <<< "$raw" || true
        (( ${#fields[@]} == 6 )) || continue
        M_TIER["${fields[0]}"]="${fields[1]}"
        M_COST["${fields[0]}"]="${fields[2]}"
        M_OWNER["${fields[0]}"]="${fields[4]}"
        M_QUICK["${fields[0]}"]="${fields[5]}"
        M_ORDER+=("${fields[0]}")
    done < "$MANIFEST"
}

# Lenient single pass over the exceptions file. Globals: X_TIER, X_DATE,
# X_NOTE, X_ORDER.
read_exceptions()
{
    X_TIER=()
    X_DATE=()
    X_NOTE=()
    X_ORDER=()
    local raw fields
    while IFS= read -r raw; do
        [[ -z "$raw" || "$raw" == '#'* ]] && continue
        IFS=$'\t' read -r -a fields <<< "$raw" || true
        (( ${#fields[@]} == 4 )) || continue
        X_TIER["${fields[0]}"]="${fields[1]}"
        X_DATE["${fields[0]}"]="${fields[2]}"
        X_NOTE["${fields[0]}"]="${fields[3]}"
        X_ORDER+=("${fields[0]}")
    done < "$EXCEPTIONS"
}

derive_class()
{
    local cost="$1"
    if (( cost <= 5 )); then
        printf 'S'
    elif (( cost <= 60 )); then
        printf 'M'
    else
        printf 'L'
    fi
}

# ---------------------------------------------------------------------------
# check [--quick]
# ---------------------------------------------------------------------------
cmd_check()
{
    local quick_mode=0
    if [[ "${1:-}" == "--quick" ]]; then
        quick_mode=1
    elif [[ -n "${1:-}" ]]; then
        echo "ERROR: unknown check option: $1" >&2
        return 2
    fi

    local parsed
    parsed="$(parse_cmake)" # fails loud (exit 2) on unparseable registration

    # CMake registrations: TEST name->line, LABELS name->labels, in order.
    declare -A CM_LINE=() CM_LABELS=()
    local -a cm_order=()
    local kind name rest line
    while read -r kind name rest; do
        case "$kind" in
            TEST)
                line="${rest%% *}"
                CM_LINE["$name"]="$line"
                cm_order+=("$name")
                ;;
            LABEL) CM_LABELS["$name"]="$rest" ;;
        esac
    done <<< "$parsed"

    read_manifest
    read_exceptions

    local -a failures=()
    declare -A ROW_SEEN=()
    local lineno=0 raw fields
    local test tier cost class owner quick
    local stier etier edate enote
    local seen_tier
    local eff

    # Strict manifest pass (with line numbers).
    while IFS= read -r raw; do
        lineno=$((lineno + 1))
        [[ -z "$raw" || "$raw" == '#'* ]] && continue
        IFS=$'\t' read -r -a fields <<< "$raw" || true
        if (( ${#fields[@]} != 6 )); then
            failures+=("manifest: line $lineno of tests/test-manifest.tsv has ${#fields[@]} fields (expected 6: test, tier, cost_s, class, owner, quick)")
            continue
        fi
        test="${fields[0]}"
        tier="${fields[1]}"
        cost="${fields[2]}"
        class="${fields[3]}"
        owner="${fields[4]}"
        quick="${fields[5]}"

        if [[ -n "${ROW_SEEN[$test]:-}" ]]; then
            failures+=("manifest: $test appears twice in tests/test-manifest.tsv (line $lineno and line ${ROW_SEEN[$test]})")
            continue
        fi
        ROW_SEEN["$test"]="$lineno"

        if [[ -z "${CM_LINE[$test]:-}" ]]; then
            failures+=("manifest: $test is not registered in CMakeLists.txt (register it or remove the row)")
        fi
        case "$tier" in
            fast|ui|slow|packaging) ;;
            *) failures+=("manifest: $test has unknown tier \"$tier\" (expected fast|ui|slow|packaging)") ;;
        esac
        if [[ ! "$cost" =~ ^[0-9]+$ || "$cost" -eq 0 ]]; then
            failures+=("manifest: $test cost_s \"$cost\" must be a positive integer (declared seconds)")
        else
            case "$class" in
                S) (( cost > 5 )) && failures+=("manifest: $test cost_s $cost declares class S but S is for costs <= 5 s") ;;
                M) (( cost <= 5 || cost > 60 )) && failures+=("manifest: $test cost_s $cost declares class M but M is for costs 6-60 s") ;;
                L) (( cost <= 60 )) && failures+=("manifest: $test cost_s $cost declares class L but L is for costs > 60 s") ;;
                *) failures+=("manifest: $test has unknown class \"$class\" (expected S, M or L)") ;;
            esac
        fi
        if [[ "$quick" != "y" && "$quick" != "n" ]]; then
            failures+=("manifest: $test quick flag must be y or n")
        elif [[ "$quick" == "y" && "$tier" != "fast" ]]; then
            failures+=("manifest: $test is quick=y but tier $tier never runs in the quick gate (quick=y is fast-tier only)")
        fi
        if [[ -n "${CM_LABELS[$test]:-}" && ",${CM_LABELS[$test]}," != *",$tier,"* ]]; then
            failures+=("manifest: $test declares tier $tier but CMakeLists.txt LABELS it \"${CM_LABELS[$test]}\"")
        fi
    done < "$MANIFEST"

    # Registration cross-checks.
    for test in "${cm_order[@]}"; do
        if [[ -z "${ROW_SEEN[$test]:-}" ]]; then
            failures+=("manifest: $test is registered in CMakeLists.txt (line ${CM_LINE[$test]}) but missing from tests/test-manifest.tsv (run tests/test-manifest.sh update)")
        fi
        if [[ -z "${CM_LABELS[$test]:-}" ]]; then
            failures+=("manifest: $test has no LABELS tier in CMakeLists.txt (add a set_tests_properties LABELS block)")
        fi
    done

    # Budget directives.
    local btier
    for btier in "${BUDGET_TIERS[@]}"; do
        [[ -n "${BUDGETS[$btier]:-}" ]] || failures+=("manifest: missing # budget $btier directive in tests/test-manifest.tsv")
    done
    for btier in "${!BUDGETS[@]}"; do
        case "$btier" in
            fast|quick|ui|slow|packaging) ;;
            *) failures+=("manifest: unknown budget tier \"$btier\" (expected fast|quick|ui|slow|packaging)") ;;
        esac
    done
    for btier in "${KNOWN_TIERS[@]}"; do
        seen_tier=0
        for test in "${M_ORDER[@]}"; do
            [[ "${M_TIER[$test]}" == "$btier" ]] && seen_tier=1
        done
        (( seen_tier )) || failures+=("manifest: tier $btier has a budget directive but no rows (remove the directive or add rows)")
    done

    # Exceptions rows.
    for test in "${X_ORDER[@]}"; do
        etier="${X_TIER[$test]}"
        edate="${X_DATE[$test]}"
        enote="${X_NOTE[$test]}"
        if [[ -z "${M_TIER[$test]:-}" ]]; then
            failures+=("exceptions: row for $test references a test missing from tests/test-manifest.tsv")
        fi
        if [[ -n "${M_TIER[$test]:-}" && "$etier" != "${M_TIER[$test]}" ]]; then
            failures+=("exceptions: row for $test has tier $etier but tests/test-manifest.tsv declares ${M_TIER[$test]}")
        fi
        if [[ ! "$edate" =~ ^[0-9]{4}-[0-9]{2}-[0-9]{2}$ ]]; then
            failures+=("exceptions: row for $test has an invalid date \"$edate\" (use YYYY-MM-DD)")
        fi
        if [[ -z "$enote" ]]; then
            failures+=("exceptions: row for $test has an empty note")
        fi
    done

    # Per-tier sums with exception coverage. An exception excludes the row's
    # declared cost from its tier sum (and from the quick sum when quick=y).
    declare -A SUM=() EXC=()
    for test in "${M_ORDER[@]}"; do
        stier="${M_TIER[$test]}"
        cost="${M_COST[$test]}"
        [[ "$cost" =~ ^[0-9]+$ ]] || continue
        SUM["$stier"]=$(( ${SUM["$stier"]:-0} + cost ))
        if [[ -n "${X_TIER[$test]:-}" ]]; then
            EXC["$stier"]=$(( ${EXC["$stier"]:-0} + cost ))
        fi
        if [[ "${M_QUICK[$test]}" == "y" && "$stier" == "fast" ]]; then
            SUM["quick"]=$(( ${SUM["quick"]:-0} + cost ))
            [[ -n "${X_TIER[$test]:-}" ]] && EXC["quick"]=$(( ${EXC["quick"]:-0} + cost ))
        fi
    done

    for btier in "${BUDGET_TIERS[@]}"; do
        [[ -n "${BUDGETS[$btier]:-}" ]] || continue
        if [[ "$btier" == "quick" && "$quick_mode" -eq 0 ]]; then
            continue # quick budget is enforced only by check --quick
        fi
        eff=$(( ${SUM[$btier]:-0} - ${EXC[$btier]:-0} ))
        if (( eff > BUDGETS["$btier"] )); then
            failures+=("budget: $btier total $eff exceeds budget ${BUDGETS[$btier]} by $(( eff - BUDGETS["$btier"] )); reduce a declared cost or approve the largest row: tests/test-manifest.sh approve <test> '<note>'")
        fi
    done

    # Summary / results.
    local body="" sep=""
    for btier in "${BUDGET_TIERS[@]}"; do
        eff=$(( ${SUM[$btier]:-0} - ${EXC[$btier]:-0} ))
        body+="${sep}${btier} ${eff}/${BUDGETS[$btier]:--}"
        sep=", "
    done

    if (( ${#failures[@]} > 0 )); then
        local f
        for f in "${failures[@]}"; do
            printf '%s\n' "$f"
        done
        printf 'budget: FAIL (%s)\n' "$body"
        return 1
    fi

    printf 'budget: PASS (%s)\n' "$body"
    for test in "${X_ORDER[@]}"; do
        printf 'BUDGET-APPROVED: %s (%s) %s: %s\n' \
            "$test" "${X_TIER[$test]}" "${X_DATE[$test]}" "${X_NOTE[$test]}"
    done
    return 0
}

# ---------------------------------------------------------------------------
# update: regenerate rows from CMakeLists.txt, preserving declared cost_s.
# ---------------------------------------------------------------------------
cmd_update()
{
    local parsed
    parsed="$(parse_cmake)"

    declare -A CM_LABELS=()
    local -a cm_order=()
    local kind name rest
    while read -r kind name rest; do
        case "$kind" in
            TEST) cm_order+=("$name") ;;
            LABEL) CM_LABELS["$name"]="$rest" ;;
        esac
    done <<< "$parsed"

    read_manifest

    local tmp
    tmp="$(mktemp "$SCRIPT_DIR/.test-manifest.tmp.XXXXXX")"
    trap 'rm -f "$tmp"' RETURN

    local header tier cost class owner quick new_count=0
    {
        for header in "${M_HEADER[@]}"; do
            printf '%s\n' "$header"
        done
        for name in "${cm_order[@]}"; do
            tier="${CM_LABELS[$name]:-}"
            [[ -z "$tier" ]] && tier="unlabeled"
            tier="${tier%%,*}"
            if [[ -n "${M_TIER[$name]:-}" && "${M_COST[$name]:-}" =~ ^[0-9]+$ ]]; then
                cost="${M_COST[$name]}"
            else
                cost=0
                if [[ -z "${M_TIER[$name]:-}" ]]; then
                    printf 'manifest: new test %s added with cost_s 0 (declare its measured cost and class)\n' "$name"
                    new_count=$((new_count + 1))
                fi
            fi
            class="$(derive_class "$cost")"
            owner="${M_OWNER[$name]:--}"
            quick="${M_QUICK[$name]:-}"
            if [[ "$quick" == "y" && "$tier" != "fast" ]]; then
                quick="n" # y is fast-tier only
            elif [[ "$quick" != "y" && "$quick" != "n" ]]; then
                if [[ "$tier" == "fast" ]]; then
                    quick="y"
                else
                    quick="n"
                fi
            fi
            printf '%s\t%s\t%s\t%s\t%s\t%s\n' "$name" "$tier" "$cost" "$class" "$owner" "$quick"
        done
    } > "$tmp"

    mv "$tmp" "$MANIFEST"
    printf 'manifest: updated %d rows in tests/test-manifest.tsv (%d new)\n' \
        "${#cm_order[@]}" "$new_count"
    return 0
}

# ---------------------------------------------------------------------------
# approve <test> '<note>'
# ---------------------------------------------------------------------------
cmd_approve()
{
    if [[ $# -ne 2 ]]; then
        echo "ERROR: usage: tests/test-manifest.sh approve <test> '<note>'" >&2
        return 2
    fi
    local test="$1" note="$2"
    # Trim and reject multi-line / tab-bearing notes (the file is TSV).
    note="$(printf '%s' "$note" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')"
    if [[ -z "$note" ]]; then
        printf 'refusing: the note must not be empty\n' >&2
        return 1
    fi
    if [[ "$note" == *$'\t'* || "$note" == *$'\n'* ]]; then
        printf 'refusing: the note must be a single line without tabs\n' >&2
        return 1
    fi

    read_manifest
    read_exceptions

    local tier="${M_TIER[$test]:-}"
    if [[ -z "$tier" ]]; then
        printf 'refusing: %s is not in tests/test-manifest.tsv\n' "$test" >&2
        return 1
    fi
    case "$tier" in
        fast|ui|slow|packaging) ;;
        *) printf 'refusing: %s has unknown tier "%s" in tests/test-manifest.tsv\n' "$test" "$tier" >&2
           return 1 ;;
    esac
    if [[ -n "${X_TIER[$test]:-}" ]]; then
        printf 'refusing: %s already has an exception row (%s: %s)\n' \
            "$test" "${X_DATE[$test]}" "${X_NOTE[$test]}" >&2
        return 1
    fi

    local date
    date="$(date +%F)"
    printf '%s\t%s\t%s\t%s\n' "$test" "$tier" "$date" "$note" >> "$EXCEPTIONS"
    printf 'BUDGET-APPROVED: %s (%s) %s: %s\n' "$test" "$tier" "$date" "$note"
    return 0
}

# ---------------------------------------------------------------------------
# quick-exclude / quick-excluded: manifest-derived quick-tier exclusions.
# ---------------------------------------------------------------------------
cmd_quick_exclude()
{
    read_manifest
    local -a parts=()
    local test name
    for test in "${M_ORDER[@]}"; do
        if [[ "${M_TIER[$test]}" == "fast" && "${M_QUICK[$test]}" == "n" ]]; then
            name="${test#boot-repair-}"
            parts+=("$name")
        fi
    done
    if (( ${#parts[@]} == 0 )); then
        return 0
    fi
    local joined
    joined="$(IFS='|'; printf '%s' "${parts[*]}")"
    printf 'boot-repair-(%s)\n' "$joined"
    return 0
}

cmd_quick_excluded()
{
    read_manifest
    local -a parts=()
    local test name
    for test in "${M_ORDER[@]}"; do
        if [[ "${M_TIER[$test]}" == "fast" && "${M_QUICK[$test]}" == "n" ]]; then
            name="${test#boot-repair-}"
            name="${name%-contract}"
            parts+=("$name")
        fi
    done
    if (( ${#parts[@]} == 0 )); then
        return 0
    fi
    local joined
    joined="$(IFS='/'; printf '%s' "${parts[*]}")"
    printf '%s\n' "$joined"
    return 0
}

# Globals shared between the readers and the subcommands.
declare -A BUDGETS=() M_TIER=() M_COST=() M_OWNER=() M_QUICK=()
declare -A X_TIER=() X_DATE=() X_NOTE=()
declare -a M_HEADER=() M_ORDER=() X_ORDER=()

case "${1:-}" in
    check) shift; cmd_check "$@" ;;
    update) shift; cmd_update "$@" ;;
    approve) shift; cmd_approve "$@" ;;
    quick-exclude) shift; cmd_quick_exclude "$@" ;;
    quick-excluded) shift; cmd_quick_excluded "$@" ;;
    -h|--help|"") usage; exit 0 ;;
    *) usage >&2; exit 2 ;;
esac
