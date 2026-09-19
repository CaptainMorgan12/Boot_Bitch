#!/usr/bin/env bash
set -euo pipefail

# Contract test: the release-preparation tooling adds a new README refinements
# section without losing any previous one, generates release notes scoped to
# the new version, derives the previous tag from the compare links when it is
# not passed explicitly, refuses to prepare the same release twice, and lets
# verify-release.sh reject a tampered tree. The real tree must pass
# verify-release.sh for the currently published version (0.2.24).

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
PREPARE="$ROOT_DIR/scripts/prepare-release.sh"
VERIFY="$ROOT_DIR/scripts/verify-release.sh"
FAKE_VERSION="9.9.9"
FAKE_TAG="v$FAKE_VERSION"
PREVIOUS_TAG="v0.2.24"
PUBLISHED_VERSION="0.2.24"

fail()
{
    echo "FAIL: $*" >&2
    exit 1
}

for script in "$PREPARE" "$VERIFY"; do
    [[ -x "$script" ]] || fail "release script is not executable: $script"
    bash -n "$script"
done

WORK_DIR="$(mktemp -d)"
trap 'rm -rf "$WORK_DIR"' EXIT

# Build a disposable copy of the documentation tree. The markers make the
# scoping assertions independent of the real changelog wording.
make_tree()
{
    local dir="$1" marker_unreleased="$2" marker_prior="$3"
    mkdir -p "$dir/scripts" "$dir/docs"
    cp "$ROOT_DIR/README.md" "$ROOT_DIR/CHANGELOG.md" "$ROOT_DIR/CMakeLists.txt" "$dir/"
    cp "$ROOT_DIR/docs/"*.md "$dir/docs/"
    cp "$PREPARE" "$VERIFY" "$dir/scripts/"
    awk -v unreleased="$marker_unreleased" -v prior="$marker_prior" '
        /^## Unreleased[[:space:]]*$/ { print; print "- " unreleased; next }
        /^## 0\.2\.24([[:space:]]|$)/ { print; print "- " prior; next }
        { print }
    ' "$dir/CHANGELOG.md" > "$dir/CHANGELOG.md.tmp"
    mv "$dir/CHANGELOG.md.tmp" "$dir/CHANGELOG.md"
}

MARKER_UNRELEASED="CONTRACT-UNRELEASED-MARKER"
MARKER_PRIOR="CONTRACT-PRIOR-RELEASE-MARKER"
TREE="$WORK_DIR/tree"
make_tree "$TREE" "$MARKER_UNRELEASED" "$MARKER_PRIOR"

grep -qF -- "- $MARKER_UNRELEASED" "$TREE/CHANGELOG.md" \
    || fail "test setup did not inject the Unreleased marker"
grep -qF -- "- $MARKER_PRIOR" "$TREE/CHANGELOG.md" \
    || fail "test setup did not inject the prior-release marker"

mapfile -t sections_before < <(
    grep -E '^## [0-9]+\.[0-9]+\.[0-9]+ refinements' "$TREE/README.md" | sort
)

# ---------------------------------------------------------------------------
# prepare-release.sh creates the new section and the scoped notes.
# ---------------------------------------------------------------------------
prepare_output="$(
    bash "$TREE/scripts/prepare-release.sh" "$FAKE_VERSION" "$PREVIOUS_TAG" 2>&1
)" || {
    printf '%s\n' "$prepare_output" >&2
    fail "prepare-release.sh failed for $FAKE_VERSION"
}

grep -q "## $FAKE_VERSION refinements" <<< "$prepare_output" \
    || fail "prepare-release.sh did not report the new README section"

NOTES="$TREE/docs/release-notes-$FAKE_VERSION.md"
[[ -f "$NOTES" ]] || fail "prepare-release.sh did not create docs/release-notes-$FAKE_VERSION.md"

# The notes are scoped: they carry the new markers, never the prior release's.
grep -qF -- "$MARKER_UNRELEASED" "$NOTES" \
    || fail "release notes do not contain the Unreleased entry"
if grep -qF -- "$MARKER_PRIOR" "$NOTES"; then
    fail "release notes contain a bullet from the previous release"
fi
grep -qF "Boot Bitch $FAKE_VERSION is a maintenance release with the changes made since the" "$NOTES" \
    || fail "release notes are missing the scoped intro sentence"
grep -qF "published 0.2.24 release." "$NOTES" \
    || fail "release notes do not name the previous release"
grep -qF "compare/$PREVIOUS_TAG...$FAKE_TAG" "$NOTES" \
    || fail "release notes are missing the compare link $PREVIOUS_TAG...$FAKE_TAG"
if grep -Eiq 'cumulative|rolls[ -]?up' "$NOTES"; then
    fail "release notes contain cumulative wording"
fi

# Every previous README section survived and exactly one was added.
mapfile -t sections_after < <(
    grep -E '^## [0-9]+\.[0-9]+\.[0-9]+ refinements' "$TREE/README.md" | sort
)
for section in "${sections_before[@]}"; do
    grep -qxF -- "$section" <<< "$(printf '%s\n' "${sections_after[@]}")" \
        || fail "previous README section was lost: $section"
done
grep -qxF -- "## $FAKE_VERSION refinements" <<< "$(printf '%s\n' "${sections_after[@]}")" \
    || fail "README is missing the new '## $FAKE_VERSION refinements' section"
[[ ${#sections_after[@]} -eq $(( ${#sections_before[@]} + 1 )) ]] \
    || fail "expected exactly one new README section (before=${#sections_before[@]} after=${#sections_after[@]})"

# The changelog gained the version section and kept Unreleased with a placeholder.
grep -qE "^## $FAKE_VERSION( — |$)" "$TREE/CHANGELOG.md" \
    || fail "CHANGELOG is missing the $FAKE_VERSION section"
grep -qE '^## Unreleased[[:space:]]*$' "$TREE/CHANGELOG.md" \
    || fail "CHANGELOG lost the Unreleased section"
grep -qF -- '- (no unreleased changes yet)' "$TREE/CHANGELOG.md" \
    || fail "CHANGELOG Unreleased section has no fresh placeholder"

# ---------------------------------------------------------------------------
# verify-release.sh accepts the prepared tree.
# ---------------------------------------------------------------------------
bash "$TREE/scripts/verify-release.sh" "$FAKE_VERSION" \
    || fail "verify-release.sh rejected the tree prepared by prepare-release.sh"

# ---------------------------------------------------------------------------
# verify-release.sh rejects a tampered tree.
# ---------------------------------------------------------------------------
cp "$TREE/README.md" "$TREE/README.md.ok"
cp "$NOTES" "$TREE/notes.ok"

awk '
    /^## 0\.2\.23 refinements/ { skip = 1; next }
    skip && /^## / { skip = 0 }
    !skip { print }
' "$TREE/README.md" > "$TREE/README.md.tmp"
mv "$TREE/README.md.tmp" "$TREE/README.md"
if bash "$TREE/scripts/verify-release.sh" "$FAKE_VERSION" > "$TREE/tamper-readme.log" 2>&1; then
    fail "verify-release.sh accepted a tree with a missing 0.2.23 README section"
fi
grep -q '0.2.23' "$TREE/tamper-readme.log" \
    || fail "verify-release.sh did not name the missing README section"
cp "$TREE/README.md.ok" "$TREE/README.md"

sed -i "s#compare/$PREVIOUS_TAG...$FAKE_TAG#compare/v0.2.22...$FAKE_TAG#" "$NOTES"
if bash "$TREE/scripts/verify-release.sh" "$FAKE_VERSION" > "$TREE/tamper-notes.log" 2>&1; then
    fail "verify-release.sh accepted release notes with a foreign compare link"
fi
grep -q 'compare link' "$TREE/tamper-notes.log" \
    || fail "verify-release.sh did not report the wrong compare link"
cp "$TREE/notes.ok" "$NOTES"

# ---------------------------------------------------------------------------
# Re-running prepare-release.sh refuses in both refusal paths.
# ---------------------------------------------------------------------------
if bash "$TREE/scripts/prepare-release.sh" "$FAKE_VERSION" "$PREVIOUS_TAG" \
        > "$TREE/rerun-notes.log" 2>&1; then
    fail "prepare-release.sh prepared the same release twice"
fi
grep -q 'already exists' "$TREE/rerun-notes.log" \
    || fail "prepare-release.sh did not explain the existing release-notes refusal"

rm -f "$NOTES"
if bash "$TREE/scripts/prepare-release.sh" "$FAKE_VERSION" "$PREVIOUS_TAG" \
        > "$TREE/rerun-readme.log" 2>&1; then
    fail "prepare-release.sh accepted an existing README refinements section"
fi
grep -qi 'README.md already' "$TREE/rerun-readme.log" \
    || fail "prepare-release.sh did not explain the existing README section refusal"

# ---------------------------------------------------------------------------
# Without an explicit previous tag the highest compare-link tag is used.
# ---------------------------------------------------------------------------
DEFAULT_TREE="$WORK_DIR/default"
make_tree "$DEFAULT_TREE" "CONTRACT-DEFAULT-MARKER" "CONTRACT-DEFAULT-PRIOR"
bash "$DEFAULT_TREE/scripts/prepare-release.sh" "9.9.10" > "$DEFAULT_TREE/prepare.log" 2>&1 \
    || {
        cat "$DEFAULT_TREE/prepare.log" >&2
        fail "prepare-release.sh could not derive the previous tag from the compare links"
    }
grep -qF 'compare/v0.2.24...v9.9.10' "$DEFAULT_TREE/docs/release-notes-9.9.10.md" \
    || fail "derived previous tag is not v0.2.24"
bash "$DEFAULT_TREE/scripts/verify-release.sh" "9.9.10" \
    || fail "verify-release.sh rejected the tree prepared with a derived previous tag"

# An Unreleased section that still only holds the placeholder is not a release.
if bash "$DEFAULT_TREE/scripts/prepare-release.sh" "9.9.11" \
        > "$DEFAULT_TREE/placeholder.log" 2>&1; then
    fail "prepare-release.sh released a placeholder-only Unreleased section"
fi
grep -q 'placeholder' "$DEFAULT_TREE/placeholder.log" \
    || fail "prepare-release.sh did not explain the placeholder-only Unreleased refusal"

# ---------------------------------------------------------------------------
# The real tree must pass verification for the currently published version.
# ---------------------------------------------------------------------------
bash "$VERIFY" "$PUBLISHED_VERSION" \
    || fail "verify-release.sh failed on the real tree for $PUBLISHED_VERSION"

echo "PASS: release-preparation tooling preserves README history and scopes per-version notes."
