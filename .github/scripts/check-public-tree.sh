#!/usr/bin/env bash
set -euo pipefail

# Public-tree guard for the CI checkout and local staging mirrors.
#
# The published tree carries install/build/test material only. This read-only
# check fails when maintainer-local files (agent notes, maintainer contracts,
# development tooling or backup bundles) were committed into the published set,
# and when the release version and its documentation/package filenames drift
# apart. It needs only bash and git:
#
#   bash .github/scripts/check-public-tree.sh

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
failures=0

fail()
{
    printf 'FAIL: %s\n' "$*" >&2
    failures=$((failures + 1))
}

# collect_public_files: the published file set. In a Git checkout the tracked
# files are authoritative (the staging mirror keeps gitignored release captures
# under its local development area, which are never published); without Git,
# scan the tree minus build output and that local development workspace.
collect_public_files()
{
    if git -C "$ROOT_DIR" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
        # Tracked plus untracked-but-unignored files: exactly what a commit
        # would publish (gitignored captures and build output stay excluded).
        git -C "$ROOT_DIR" ls-files -z --cached --others --exclude-standard \
            | while IFS= read -r -d '' path; do
                printf '%s\0' "$ROOT_DIR/$path"
            done
    else
        find "$ROOT_DIR" \
            \( -path "$ROOT_DIR/.git" -o -path "$ROOT_DIR/Development" \
               -o -path "$ROOT_DIR/_CPack_Packages" -o -path "$ROOT_DIR/build*" \) -prune -o \
            -type f -print0
    fi
}

# ---------------------------------------------------------------------------
# 1. No maintainer-local files in the published set
# ---------------------------------------------------------------------------
mapfile -d '' -t public_files < <(collect_public_files) || true
if (( ${#public_files[@]} == 0 )); then
    fail "no published files found under $ROOT_DIR"
fi
for path in "${public_files[@]}"; do
    rel="${path#"$ROOT_DIR"/}"
    case "$rel" in
        Development/*|development/*)
            fail "maintainer development content is published: $rel"
            ;;
        AGENTS.md|*/AGENTS.md)
            fail "maintainer agent notes are published: $rel"
            ;;
        */test-agent-safety-contract.sh|*/test-release-preparation-contract.sh|\
        */test-repo-hygiene-contract.sh|*/test-published-release-immutability-contract.sh|\
        */dev-check.sh|*/local-refresh.sh|*/prepare-release.sh|\
        */verify-release.sh|*/sync-release-artifacts.sh)
            fail "maintainer tooling is published: $rel"
            ;;
        *.bundle)
            fail "local backup bundle is published: $rel"
            ;;
    esac
done

# ---------------------------------------------------------------------------
# 2. Version and filename consistency
# ---------------------------------------------------------------------------
version="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\+\([0-9][0-9.]*\).*/\1/p' \
    "$ROOT_DIR/CMakeLists.txt" | head -1)"
if [[ -z "$version" ]]; then
    fail "cannot read the project version from CMakeLists.txt"
else
    grep -qF "CPACK_PACKAGE_VERSION \"\${PROJECT_VERSION}\"" "$ROOT_DIR/CMakeLists.txt" \
        || fail "CPack package version is no longer tied to PROJECT_VERSION"

    metainfo_version="$(sed -n 's/.*<release version="\([0-9][0-9.]*\)".*/\1/p' \
        "$ROOT_DIR/data/org.bootrepair.BootRepair.metainfo.xml" | head -1)"
    [[ "$metainfo_version" == "$version" ]] \
        || fail "AppStream newest release is '$metainfo_version', expected '$version'"

    grep -qE "^## ${version//./\\.}([[:space:]]|\$)" "$ROOT_DIR/CHANGELOG.md" \
        || fail "CHANGELOG.md has no '## $version' section"

    [[ -f "$ROOT_DIR/docs/release-notes-$version.md" ]] \
        || fail "docs/release-notes-$version.md is missing"
    grep -qF "release-notes-$version.md" "$ROOT_DIR/docs/README.md" \
        || fail "docs/README.md does not link release-notes-$version.md"

    grep -qF "# Boot Bitch $version" "$ROOT_DIR/README.md" \
        || fail "README.md title does not reference $version"

    grep -qF 'set(CPACK_PACKAGE_NAME "boot-repair")' "$ROOT_DIR/CMakeLists.txt" \
        || fail "the Debian package name is no longer boot-repair"
    grep -qF 'set(CPACK_RPM_PACKAGE_NAME "boot-bitch")' "$ROOT_DIR/CMakeLists.txt" \
        || fail "the RPM package name is no longer boot-bitch"
    grep -q '^pkgname=boot-bitch$' "$ROOT_DIR/scripts/package-arch.sh" \
        || fail "the Arch package name is no longer boot-bitch"
    grep -q '^pkgname=boot-bitch$' "$ROOT_DIR/scripts/package-alpine.sh" \
        || fail "the Alpine package name is no longer boot-bitch"
fi

if (( failures > 0 )); then
    printf 'FAIL: %d public-tree check(s) failed.\n' "$failures" >&2
    exit 1
fi

printf 'PASS: public tree is clean (%d files) and version %s is consistent.\n' \
    "${#public_files[@]}" "${version:-unknown}"
