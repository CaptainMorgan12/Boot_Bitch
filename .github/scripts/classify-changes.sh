#!/usr/bin/env bash
set -euo pipefail

# Classify the changed-path set of a push / pull_request / tag event into the
# cheapest CI lane that still validates it, so full builds only run when the
# change set genuinely needs them. The script takes no arguments and reads the
# GitHub Actions environment: GITHUB_EVENT_NAME, GITHUB_EVENT_PATH (the event
# payload JSON), GITHUB_SHA and GITHUB_OUTPUT. Outside Actions, or when the
# payload is unusable, it falls back to HEAD^..HEAD (or --root HEAD for a
# one-commit history) and always fails closed to the full lane on any doubt.
#
# Emitted GitHub Actions outputs:
#   lane            full | shell | docs
#   changed_sh      newline list of changed *.sh paths (empty when none)
#   data_changed    true | false   (any changed path under data/)
#   slow_contracts  newline list of ctest names for changed slow contracts
#
# Classification, evaluated per path (any doubt fails closed to full):
#   full   CMakeLists.txt, src/, resources/, scripts/boot-repair-helper.sh,
#          .github/, tests/ (except tests/fixtures/)
#   shell  scripts/, legacy/, data/, tests/fixtures/
#   docs   docs/, a top-level *.md, LICENSE, .gitignore
#
# A workflow_dispatch event has no diff to reason about, so it forces full.

# is_full_path / is_shell_path / is_docs_path: pattern membership of one path.
is_full_path()
{
    local path="$1"
    case "$path" in
        CMakeLists.txt|src/*|resources/*|scripts/boot-repair-helper.sh|.github/*)
            return 0 ;;
        tests/fixtures/*)
            return 1 ;;
        tests/*)
            return 0 ;;
        *)
            return 1 ;;
    esac
}

is_shell_path()
{
    local path="$1"
    case "$path" in
        scripts/*|legacy/*|data/*|tests/fixtures/*)
            return 0 ;;
        *)
            return 1 ;;
    esac
}

is_docs_path()
{
    local path="$1"
    [[ "$path" == docs/* || "$path" == LICENSE || "$path" == .gitignore ]] \
        && return 0
    [[ "$path" =~ ^[^/]+\.md$ ]]
}

# read_json_field FIELD: first value of the JSON field from the event payload.
# Prefers jq; falls back to a sed extraction for the two 40-hex fields used.
read_json_field()
{
    local field="$1"
    local payload="$2"
    local value=""

    if command -v jq >/dev/null 2>&1; then
        value="$(jq -r ".${field} // empty" "$payload" 2>/dev/null || true)"
    elif [[ "$field" == "before" ]]; then
        value="$(sed -n 's/.*"before"[[:space:]]*:[[:space:]]*"\([0-9a-f]\{40\}\)".*/\1/p' \
            "$payload" | head -1)"
    else
        value="$(sed -n \
            's/.*"base"[[:space:]]*:[[:space:]]*{[^}]*"sha"[[:space:]]*:[[:space:]]*"\([0-9a-f]\{40\}\)".*/\1/p' \
            "$payload" | head -1)"
    fi
    printf '%s\n' "$value"
}

# collect_changed_paths: prints the changed path list for this event.
collect_changed_paths()
{
    local event="$1"
    local payload="$2"
    local sha="$3"
    local before=""
    local base=""

    if [[ -n "$payload" && -f "$payload" ]]; then
        before="$(read_json_field "before" "$payload")"
        base="$(read_json_field "pull_request.base.sha" "$payload")"
    fi

    if [[ "$event" == "pull_request" && -n "$base" && "$base" != "null" ]]; then
        if git rev-parse -q --verify "${base}^{commit}" >/dev/null 2>&1; then
            git diff --name-only "$base...$sha" -- 2>/dev/null && return 0
        fi
    fi

    if [[ "$event" == "push" ]]; then
        # Tag pushes and new branches have no real "before": diff the whole
        # history so the lane stays full.
        if [[ "$before" == "0000000000000000000000000000000000000000" ]]; then
            git diff --name-only --root "$sha" -- 2>/dev/null && return 0
        fi
        if [[ -n "$before" && "$before" != "null" ]] \
            && git rev-parse -q --verify "${before}^{commit}" >/dev/null 2>&1; then
            git diff --name-only "$before..$sha" -- 2>/dev/null && return 0
        fi
    fi

    # Local fallback: the last commit, or the whole history when HEAD has no
    # parent (one-commit repository).
    if git rev-parse -q --verify HEAD^ >/dev/null 2>&1; then
        git diff --name-only HEAD^..HEAD -- 2>/dev/null || true
    else
        git diff --name-only --root HEAD -- 2>/dev/null || true
    fi
}

main()
{
    local event="${GITHUB_EVENT_NAME:-}"
    local payload="${GITHUB_EVENT_PATH:-}"
    local sha="${GITHUB_SHA:-HEAD}"
    local path=""
    local lane=""
    local any_full=0
    local any_shell=0
    local all_docs=1
    local count=0
    local data_changed=false
    declare -a changed=()
    declare -a changed_sh=()
    declare -a slow_contracts=()

    if [[ "$event" == "workflow_dispatch" ]]; then
        lane="full"
    else
        mapfile -t changed < <(collect_changed_paths "$event" "$payload" "$sha" \
            | LC_ALL=C sort -u)
    fi

    for path in "${changed[@]}"; do
        [[ -n "$path" ]] || continue
        count=$((count + 1))

        if is_full_path "$path"; then
            any_full=1
        fi
        if is_shell_path "$path"; then
            any_shell=1
        fi
        if ! is_docs_path "$path"; then
            all_docs=0
        fi
        if [[ "$path" == *.sh ]]; then
            changed_sh+=("$path")
        fi
        if [[ "$path" == data/* ]]; then
            data_changed=true
        fi
        if [[ "$path" =~ ^scripts/test-(chroot-shell|filesystem-repair|host-default|backend-profile|packaging-profile)-contract\.sh$ ]]; then
            slow_contracts+=("boot-repair-${BASH_REMATCH[1]}-contract")
        fi
    done

    if [[ -z "$lane" ]]; then
        if (( any_full )); then
            lane="full"
        elif (( any_shell )); then
            lane="shell"
        elif (( count > 0 && all_docs )); then
            lane="docs"
        else
            lane="full"
        fi
    fi

    printf 'lane=%s changed=%d changed_sh=%d data_changed=%s slow_contracts=%d\n' \
        "$lane" "$count" "${#changed_sh[@]}" "$data_changed" "${#slow_contracts[@]}"
    for path in "${changed[@]}"; do
        printf '  %s\n' "$path"
    done

    if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
        printf 'lane=%s\n' "$lane" >> "$GITHUB_OUTPUT"
        {
            printf 'changed_sh<<EOF\n'
            if (( ${#changed_sh[@]} > 0 )); then
                printf '%s\n' "${changed_sh[@]}"
            fi
            printf 'EOF\n'
        } >> "$GITHUB_OUTPUT"
        printf 'data_changed=%s\n' "$data_changed" >> "$GITHUB_OUTPUT"
        {
            printf 'slow_contracts<<EOF\n'
            if (( ${#slow_contracts[@]} > 0 )); then
                printf '%s\n' "${slow_contracts[@]}"
            fi
            printf 'EOF\n'
        } >> "$GITHUB_OUTPUT"
    fi
}

main
