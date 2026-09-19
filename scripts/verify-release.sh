#!/usr/bin/env bash
set -euo pipefail

# Read-only verification of the release documentation for one Boot Bitch
# version. The script never writes to the tree; it exits non-zero with a
# specific message for every failed check:
#
#   a) docs/release-notes-<version>.md exists, carries the scoped intro
#      sentence, links the diff against the previous tag, contains no
#      "cumulative"/"rolls up" wording and no foreign compare link;
#   b) README.md has "## <version> refinements" and still has a section for
#      every version in MAINTAINED_README_VERSIONS, every version in
#      CHANGELOG.md and the previous release, plus the matching top-of-file
#      version references;
#   c) CHANGELOG.md has a "## <version>" section;
#   d) Development/release-<version>/release-notes-<version>.md, when present,
#      is identical to the docs copy.
#
# Usage: scripts/verify-release.sh <version>

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
REPO_URL="${BOOT_REPAIR_REPO_URL:-https://github.com/CaptainMorgan12/Boot_Bitch}"

# Permanent README "## <version> refinements" sections. Extend this list when a
# new release is published; the verifier additionally requires every version
# present in CHANGELOG.md and the previous release, so a lost section cannot
# hide behind a simultaneous changelog edit.
MAINTAINED_README_VERSIONS=(
    0.2.15 0.2.16 0.2.17 0.2.18 0.2.19 0.2.20
    0.2.21 0.2.22 0.2.23 0.2.24
)

usage()
{
    cat >&2 <<'EOF'
Usage: scripts/verify-release.sh <version>

Read-only verification of README.md, CHANGELOG.md and
docs/release-notes-<version>.md for one release version.
EOF
}

[[ $# -eq 1 ]] || { usage; exit 2; }

VERSION="$1"
[[ "$VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || {
    echo "FAIL: version '$VERSION' is not X.Y.Z" >&2
    exit 2
}

README="$ROOT_DIR/README.md"
CHANGELOG="$ROOT_DIR/CHANGELOG.md"
NOTES="$ROOT_DIR/docs/release-notes-$VERSION.md"
NEW_TAG="v$VERSION"

failures=0
fail()
{
    echo "FAIL: $*" >&2
    failures=$((failures + 1))
}

mapfile -t readme_versions < <(
    grep -E '^## [0-9]+\.[0-9]+\.[0-9]+ refinements[[:space:]]*$' "$README" \
        | sed -E 's/^## ([0-9]+\.[0-9]+\.[0-9]+).*/\1/'
)
mapfile -t changelog_versions < <(
    grep -E '^## [0-9]+\.[0-9]+\.[0-9]+([[:space:]]|$)' "$CHANGELOG" \
        | sed -E 's/^## ([0-9]+\.[0-9]+\.[0-9]+).*/\1/'
)

previous_version="$(
    {
        printf '%s\n' "$VERSION"
        printf '%s\n' "${MAINTAINED_README_VERSIONS[@]}"
        printf '%s\n' "${readme_versions[@]}"
        printf '%s\n' "${changelog_versions[@]}"
    } | grep -E '^[0-9]+\.[0-9]+\.[0-9]+$' | sort -Vu \
      | awk -v target="$VERSION" '$0 == target { print prev; exit } { prev = $0 }'
)"
if [[ -z "$previous_version" ]]; then
    fail "cannot determine the previous release below $VERSION; verify a version that has a published predecessor."
fi

# ---------------------------------------------------------------------------
# (a) The release notes are scoped to this version.
# ---------------------------------------------------------------------------
if [[ ! -f "$NOTES" ]]; then
    fail "missing docs/release-notes-$VERSION.md"
else
    flat_notes="$(tr '\n' ' ' < "$NOTES" | tr -s '[:space:]' ' ')"
    if [[ -n "$previous_version" ]]; then
        grep -qF "Boot Bitch $VERSION is a maintenance release with the changes made since the published $previous_version release." \
            <<< "$flat_notes" \
            || fail "docs/release-notes-$VERSION.md does not contain the scoped intro sentence for the published $previous_version release."
        grep -qF "$REPO_URL/compare/v$previous_version...$NEW_TAG" "$NOTES" \
            || fail "docs/release-notes-$VERSION.md is missing the compare link v$previous_version...$NEW_TAG."
    else
        grep -qF "Boot Bitch $VERSION is a maintenance release with the changes made since the published" \
            <<< "$flat_notes" \
            || fail "docs/release-notes-$VERSION.md does not contain the scoped maintenance-release intro sentence."
        grep -qE "compare/v[0-9]+\.[0-9]+\.[0-9]+\.\.\.$NEW_TAG" "$NOTES" \
            || fail "docs/release-notes-$VERSION.md is missing a compare link ending in $NEW_TAG."
    fi
    if grep -Eiq 'cumulative|rolls[ -]?up' "$NOTES"; then
        fail "docs/release-notes-$VERSION.md contains cumulative wording; notes must cover only $VERSION."
    fi
    while IFS= read -r link; do
        [[ -z "$link" ]] && continue
        [[ "$link" == *"...$NEW_TAG" ]] \
            || fail "docs/release-notes-$VERSION.md contains a foreign compare link '$link'."
    done < <(grep -oE 'compare/v[0-9]+\.[0-9]+\.[0-9]+\.\.\.v[0-9]+\.[0-9]+\.[0-9]+' "$NOTES" || true)
fi

# ---------------------------------------------------------------------------
# (b) README.md keeps every permanent section and the current references.
# ---------------------------------------------------------------------------
required_versions=("${MAINTAINED_README_VERSIONS[@]}" "${changelog_versions[@]}" "$VERSION")
if [[ -n "$previous_version" ]]; then
    required_versions+=("$previous_version")
fi
while IFS= read -r required; do
    [[ -z "$required" ]] && continue
    grep -qE "^## ${required//./\\.} refinements[[:space:]]*$" "$README" \
        || fail "README.md is missing the permanent '## $required refinements' section."
done < <(printf '%s\n' "${required_versions[@]}" | sort -Vu)

grep -qF "# Boot Bitch $VERSION — maintenance release" "$README" \
    || fail "README.md title does not reference $VERSION."
grep -qF "Version $VERSION includes" "$README" \
    || fail "README.md does not describe the version as 'Version $VERSION includes'."
grep -qF "Still intentionally constrained in $VERSION:" "$README" \
    || fail "README.md does not describe the constraints as 'Still intentionally constrained in $VERSION:'."

# ---------------------------------------------------------------------------
# (c) CHANGELOG.md has the version section.
# ---------------------------------------------------------------------------
grep -qE "^## ${VERSION//./\\.}([[:space:]]|\$)" "$CHANGELOG" \
    || fail "CHANGELOG.md is missing the '## $VERSION' section."

# ---------------------------------------------------------------------------
# (d) The captured release workspace matches docs/ when it exists.
# ---------------------------------------------------------------------------
dev_notes="$ROOT_DIR/Development/release-$VERSION/release-notes-$VERSION.md"
if [[ -f "$dev_notes" ]]; then
    cmp -s "$dev_notes" "$NOTES" \
        || fail "Development/release-$VERSION/release-notes-$VERSION.md differs from docs/release-notes-$VERSION.md."
elif [[ -d "$ROOT_DIR/Development/release-$VERSION" ]]; then
    echo "WARN: Development/release-$VERSION/ exists without release-notes-$VERSION.md." >&2
fi

if (( failures > 0 )); then
    echo "FAIL: $failures release documentation check(s) failed for $VERSION." >&2
    exit 1
fi

echo "PASS: release documentation for $VERSION is scoped, complete and consistent."
