#!/usr/bin/env bash
set -euo pipefail

# Prepare the per-version release documentation for Boot Bitch:
#
#   - promote the CHANGELOG "## Unreleased" entries (or an existing
#     "## <version>" section) into a dated "## <version> — <date>" section,
#     keeping "## Unreleased" with a fresh placeholder;
#   - generate docs/release-notes-<version>.md scoped to this version only;
#   - insert "## <version> refinements" in README.md above the newest existing
#     refinements section without touching any previous section;
#   - update the README's top-of-file version references.
#
# The script is refusal-first: it never overwrites an existing release-notes
# file or README section, and it validates its output before installing
# anything. A failure before installation leaves the tree unchanged; a failure
# during installation rolls the touched files back from private backups.
#
# Usage: scripts/prepare-release.sh <version> [previous-tag]
#
#   version       new release version, for example 0.2.25
#   previous-tag  previous release tag, for example v0.2.24. When omitted the
#                 highest vX.Y.Z tag in the README/release-note compare links
#                 or in `git tag` is used; outside a git tree the compare links
#                 must exist or the tag must be passed explicitly.
#
# Environment: BOOT_REPAIR_REPO_URL overrides the GitHub compare-link base URL,
#              RELEASE_DATE overrides the release date (YYYY-MM-DD).

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
REPO_URL="${BOOT_REPAIR_REPO_URL:-https://github.com/CaptainMorgan12/Boot_Bitch}"
RELEASE_DATE="${RELEASE_DATE:-$(date +%Y-%m-%d)}"

README="$ROOT_DIR/README.md"
CHANGELOG="$ROOT_DIR/CHANGELOG.md"

usage()
{
    cat >&2 <<'EOF'
Usage: scripts/prepare-release.sh <version> [previous-tag]

  version       new release version, for example 0.2.25
  previous-tag  previous release tag, for example v0.2.24 (default: highest
                vX.Y.Z compare-link tag or `git tag`)
EOF
}

fail()
{
    echo "ERROR: $*" >&2
    exit 1
}

section_body()
{
    local file="$1" start="$2" end
    end="$(awk -v start="$start" 'NR > start && /^## / { print NR; exit }' "$file")"
    if [[ -n "$end" ]]; then
        sed -n "$((start + 1)),$((end - 1))p" "$file"
    else
        sed -n "$((start + 1)),\$p" "$file"
    fi
}

trim_blank_edges()
{
    awk '
        { line[NR] = $0 }
        END {
            first = 1
            while (first <= NR && line[first] ~ /^[[:space:]]*$/) first++
            last = NR
            while (last >= first && line[last] ~ /^[[:space:]]*$/) last--
            for (i = first; i <= last; i++) print line[i]
        }
    '
}

# Collapse each "- " entry (including its indented continuation lines) into a
# single logical line. Changelog entries are already written in release-note
# style, so no other rewording is applied.
extract_bullets()
{
    awk '
        /^- / {
            if (buffer != "") print buffer
            buffer = substr($0, 3)
            next
        }
        /^[[:space:]]+[^[:space:]]/ {
            if (buffer != "") {
                continuation = $0
                sub(/^[[:space:]]+/, "", continuation)
                buffer = buffer " " continuation
            }
            next
        }
        {
            if (buffer != "") { print buffer; buffer = "" }
        }
        END { if (buffer != "") print buffer }
    '
}

wrap_bullet()
{
    printf '%s\n' "$1" | awk -v width=78 '
        {
            count = split($0, words, /[ \t]+/)
            line = ""
            first = 1
            for (i = 1; i <= count; i++) {
                word = words[i]
                if (word == "") continue
                limit = first ? width - 2 : width
                if (line == "") {
                    line = word
                } else if (length(line) + 1 + length(word) <= limit) {
                    line = line " " word
                } else {
                    if (first) { print "- " line; first = 0 } else print line
                    line = "  " word
                }
            }
            if (line != "") {
                if (first) print "- " line; else print line
            }
        }
    '
}

compare_headings()
{
    grep -oE '^## [0-9]+\.[0-9]+\.[0-9]+ refinements[[:space:]]*$' "$1" \
        | sed 's/[[:space:]]*$//' | sort
}

derive_previous_tag()
{
    local files=("$README") file tag
    for file in "$ROOT_DIR"/docs/release-notes-*.md; do
        [[ -f "$file" ]] && files+=("$file")
    done
    tag="$(grep -hoE 'compare/v[0-9]+\.[0-9]+\.[0-9]+\.\.\.v[0-9]+\.[0-9]+\.[0-9]+' "${files[@]}" 2>/dev/null \
        | sed -E 's#^compare/##; s#\.\.\.#\n#' | sort -Vu | tail -n1 || true)"
    if [[ -z "$tag" ]] && git -C "$ROOT_DIR" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
        tag="$(git -C "$ROOT_DIR" tag --list 'v[0-9]*.[0-9]*.[0-9]*' | sort -V | tail -n1 || true)"
    fi
    [[ -n "$tag" ]] || fail \
        "cannot derive the previous tag; pass it explicitly: scripts/prepare-release.sh <version> vX.Y.Z"
    printf '%s\n' "$tag"
}

normalize_tag()
{
    local tag="$1"
    if [[ "$tag" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        tag="v$tag"
    fi
    [[ "$tag" =~ ^v[0-9]+\.[0-9]+\.[0-9]+$ ]] \
        || fail "previous tag '$1' is not vX.Y.Z"
    printf '%s\n' "$tag"
}

[[ $# -ge 1 && $# -le 2 ]] || { usage; exit 2; }

VERSION="$1"
[[ "$VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || fail "version '$VERSION' is not X.Y.Z"

if [[ $# -eq 2 ]]; then
    PREV_TAG="$2"
else
    PREV_TAG="$(derive_previous_tag)"
fi
PREV_TAG="$(normalize_tag "$PREV_TAG")"
PREV_VERSION="${PREV_TAG#v}"
NEW_TAG="v$VERSION"
NOTES="$ROOT_DIR/docs/release-notes-$VERSION.md"

# ---------------------------------------------------------------------------
# Refusals: never overwrite an already prepared release.
# ---------------------------------------------------------------------------
[[ ! -e "$NOTES" ]] || fail \
    "docs/release-notes-$VERSION.md already exists; refusing to overwrite it. Remove it deliberately if you really intend to regenerate it."

if grep -qE "^## ${VERSION//./\\.} refinements[[:space:]]*$" "$README"; then
    fail "README.md already contains a '## $VERSION refinements' section; refusing to prepare the release twice."
fi

# ---------------------------------------------------------------------------
# Source the version's entries from the changelog.
# ---------------------------------------------------------------------------
VERSION_LINE="$(grep -nE "^## ${VERSION//./\\.}([[:space:]]|\$)" "$CHANGELOG" | head -n1 | cut -d: -f1 || true)"
if [[ -n "$VERSION_LINE" ]]; then
    SOURCE_DESCRIPTION="existing CHANGELOG section"
    BODY="$(section_body "$CHANGELOG" "$VERSION_LINE" | trim_blank_edges)"
    CHANGELOG_CHANGED=0
else
    UNRELEASED_LINE="$(grep -nE '^## Unreleased[[:space:]]*$' "$CHANGELOG" | head -n1 | cut -d: -f1 || true)"
    [[ -n "$UNRELEASED_LINE" ]] || fail \
        "CHANGELOG.md has neither a '## $VERSION' section nor a '## Unreleased' section; nothing to release."
    SOURCE_DESCRIPTION="## Unreleased"
    BODY="$(section_body "$CHANGELOG" "$UNRELEASED_LINE" | trim_blank_edges)"
    CHANGELOG_CHANGED=1
fi

mapfile -t BULLETS < <(printf '%s\n' "$BODY" | extract_bullets)
if (( CHANGELOG_CHANGED )) && (( ${#BULLETS[@]} == 1 )) \
        && [[ "${BULLETS[0]}" == "(no unreleased changes yet)" ]]; then
    fail "CHANGELOG.md '## Unreleased' still has only the placeholder; add the $VERSION changes before preparing it."
fi
(( ${#BULLETS[@]} > 0 )) || fail \
    "CHANGELOG.md $SOURCE_DESCRIPTION has no '- ' entries for $VERSION; add the release changes before preparing it."

# ---------------------------------------------------------------------------
# Private work area: backups for rollback and generated files for validation.
# ---------------------------------------------------------------------------
TMP_DIR="$(mktemp -d)"
BACKUP_DIR="$TMP_DIR/backup"
mkdir -p "$BACKUP_DIR"
cp -p "$README" "$BACKUP_DIR/README.md"
cp -p "$CHANGELOG" "$BACKUP_DIR/CHANGELOG.md"
installed=0

cleanup()
{
    local status=$?
    trap - EXIT
    if (( status != 0 && installed )); then
        echo "Rolling back partially installed release documentation." >&2
        cp -p "$BACKUP_DIR/README.md" "$README" 2>/dev/null || true
        cp -p "$BACKUP_DIR/CHANGELOG.md" "$CHANGELOG" 2>/dev/null || true
        rm -f -- "$NOTES"
    fi
    rm -rf -- "$TMP_DIR"
    exit "$status"
}
trap cleanup EXIT

BULLETS_BLOCK="$TMP_DIR/bullets.md"
: > "$BULLETS_BLOCK"
for bullet in "${BULLETS[@]}"; do
    wrap_bullet "$bullet" >> "$BULLETS_BLOCK"
done

# ---------------------------------------------------------------------------
# Generate the release notes (this version only).
# ---------------------------------------------------------------------------
TMP_NOTES="$TMP_DIR/release-notes-$VERSION.md"
{
    printf '# Boot Bitch %s\n\n' "$VERSION"
    printf 'Boot Bitch %s is a maintenance release with the changes made since the\n' "$VERSION"
    printf 'published %s release.\n\n' "$PREV_VERSION"
    printf 'Released %s.\n\n' "$RELEASE_DATE"
    cat "$BULLETS_BLOCK"
    printf '\nThe Debian package and AppImage are built locally from the complete source tree.\n'
    printf 'Verify downloaded artifacts against the SHA256SUMS attached to this release.\n\n'
    printf '[Full source diff: %s...%s](%s/compare/%s...%s).\n' \
        "$PREV_TAG" "$NEW_TAG" "$REPO_URL" "$PREV_TAG" "$NEW_TAG"
} > "$TMP_NOTES"

# ---------------------------------------------------------------------------
# Generate the updated changelog (only when there is no version section yet).
# ---------------------------------------------------------------------------
if (( CHANGELOG_CHANGED )); then
    NEXT_HEADING_LINE="$(awk -v start="$UNRELEASED_LINE" 'NR > start && /^## / { print NR; exit }' "$CHANGELOG")"
    TMP_CHANGELOG="$TMP_DIR/CHANGELOG.md"
    {
        sed -n "1,${UNRELEASED_LINE}p" "$CHANGELOG"
        printf '\n- (no unreleased changes yet)\n\n## %s — %s\n\n' "$VERSION" "$RELEASE_DATE"
        printf '%s\n' "$BODY"
        printf '\n'
        if [[ -n "$NEXT_HEADING_LINE" ]]; then
            sed -n "${NEXT_HEADING_LINE},\$p" "$CHANGELOG"
        fi
    } > "$TMP_CHANGELOG"
fi

# ---------------------------------------------------------------------------
# Generate the README section and insert it above the newest existing one.
# ---------------------------------------------------------------------------
TMP_BLOCK="$TMP_DIR/readme-section.md"
{
    printf '## %s refinements\n\n' "$VERSION"
    printf 'Boot Bitch %s is a maintenance release with the changes made since the\n' "$VERSION"
    printf 'published %s release. See the [%s release notes](docs/release-notes-%s.md).\n\n' \
        "$PREV_VERSION" "$VERSION" "$VERSION"
    cat "$BULLETS_BLOCK"
    printf '\n'
} > "$TMP_BLOCK"

TMP_README="$TMP_DIR/README.md"
awk -v block="$TMP_BLOCK" '
    !inserted && /^## [0-9]+\.[0-9]+\.[0-9]+ refinements[[:space:]]*$/ {
        while ((getline line < block) > 0) print line
        close(block)
        inserted = 1
    }
    { print }
    END { if (!inserted) exit 42 }
' "$README" > "$TMP_README" || fail \
    "README.md has no existing '## <version> refinements' section to insert above; update README.md first."

update_reference()
{
    local label="$1" old_needle="$2" new_needle="$3"
    if grep -qF -- "$old_needle" "$TMP_README"; then
        sed -i "s/${old_needle//./\\.}/${new_needle}/g" "$TMP_README"
    fi
    if ! grep -qF -- "$new_needle" "$TMP_README"; then
        fail "README.md is missing the top-of-file $label reference ('$new_needle'); update it manually and re-run."
    fi
}

update_reference "title" \
    "# Boot Bitch $PREV_VERSION — maintenance release" \
    "# Boot Bitch $VERSION — maintenance release"
update_reference "intro version" \
    "Version $PREV_VERSION includes" \
    "Version $VERSION includes"
update_reference "constraints version" \
    "Still intentionally constrained in $PREV_VERSION:" \
    "Still intentionally constrained in $VERSION:"

# ---------------------------------------------------------------------------
# Validate everything before installing it.
# ---------------------------------------------------------------------------
[[ -s "$TMP_NOTES" ]] || fail "generated release notes are empty."
grep -qF "Boot Bitch $VERSION is a maintenance release with the changes made since the" "$TMP_NOTES" \
    || fail "generated release notes are missing the scoped intro sentence."
grep -qF "published $PREV_VERSION release." "$TMP_NOTES" \
    || fail "generated release notes do not name the previous release $PREV_VERSION."
grep -qF "$REPO_URL/compare/$PREV_TAG...$NEW_TAG" "$TMP_NOTES" \
    || fail "generated release notes are missing the compare link $PREV_TAG...$NEW_TAG."
if grep -Eiq 'cumulative|rolls[ -]?up' "$TMP_NOTES"; then
    fail "generated release notes contain cumulative wording; scope them to $VERSION only."
fi
while IFS= read -r link; do
    [[ -z "$link" ]] && continue
    [[ "$link" == "compare/$PREV_TAG...$NEW_TAG" ]] \
        || fail "generated release notes contain a foreign compare link '$link'."
done < <(grep -oE 'compare/v[0-9]+\.[0-9]+\.[0-9]+\.\.\.v[0-9]+\.[0-9]+\.[0-9]+' "$TMP_NOTES" || true)

BEFORE_HEADINGS="$(compare_headings "$README")"
AFTER_HEADINGS="$(compare_headings "$TMP_README")"
while IFS= read -r heading; do
    [[ -z "$heading" ]] && continue
    grep -qxF -- "$heading" <<< "$AFTER_HEADINGS" \
        || fail "README verification failed: the previous section '$heading' is missing after preparation."
done <<< "$BEFORE_HEADINGS"
grep -qxF -- "## $VERSION refinements" <<< "$AFTER_HEADINGS" \
    || fail "README verification failed: the new '## $VERSION refinements' section is missing."
BEFORE_COUNT="$(grep -c . <<< "$BEFORE_HEADINGS" || true)"
AFTER_COUNT="$(grep -c . <<< "$AFTER_HEADINGS" || true)"
(( AFTER_COUNT == BEFORE_COUNT + 1 )) \
    || fail "README verification failed: expected exactly one new refinements section (before=$BEFORE_COUNT after=$AFTER_COUNT)."

if (( CHANGELOG_CHANGED )); then
    grep -qE "^## ${VERSION//./\\.}([[:space:]]|\$)" "$TMP_CHANGELOG" \
        || fail "CHANGELOG verification failed: the new '## $VERSION' section is missing."
    grep -qE '^## Unreleased[[:space:]]*$' "$TMP_CHANGELOG" \
        || fail "CHANGELOG verification failed: the '## Unreleased' section was lost."
fi

# ---------------------------------------------------------------------------
# Install the validated files.
# ---------------------------------------------------------------------------
installed=1
if (( CHANGELOG_CHANGED )); then
    cp "$TMP_CHANGELOG" "$CHANGELOG"
    CHANGELOG_SUMMARY="added '## $VERSION — $RELEASE_DATE' and kept '## Unreleased' with a placeholder."
else
    CHANGELOG_SUMMARY="already had a '## $VERSION' section; it was used as the source and left unchanged."
fi
cp "$TMP_README" "$README"
install -m 0644 "$TMP_NOTES" "$NOTES"

# ---------------------------------------------------------------------------
# Report and print the exact next steps.
# ---------------------------------------------------------------------------

PROJECT_VERSION="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\{1,\}\([0-9][0-9.]*\)[[:space:]]*$/\1/p' \
    "$ROOT_DIR/CMakeLists.txt" | head -n1)"
if [[ -n "$PROJECT_VERSION" && "$PROJECT_VERSION" != "$VERSION" ]]; then
    echo "WARN: CMakeLists.txt still declares VERSION $PROJECT_VERSION; bump it to $VERSION before building." >&2
fi

cat <<EOF

Prepared Boot Bitch $VERSION release documentation:
  - CHANGELOG.md: $CHANGELOG_SUMMARY
  - README.md: added '## $VERSION refinements' above the previous sections.
  - docs/release-notes-$VERSION.md: scoped to the changes since $PREV_VERSION.

Next steps (see docs/publishing.md):
  1. Bump CMakeLists.txt VERSION to $VERSION if it is not already.
  2. Build and validate: scripts/build.sh (and scripts/package-arch.sh in the Arch VM).
  3. Review and polish README.md, CHANGELOG.md and docs/release-notes-$VERSION.md.
  4. Sync to the staging clone, commit and push main.
  5. Verify: scripts/verify-release.sh $VERSION
  6. Tag: git tag -a $NEW_TAG -m "Boot Bitch $VERSION" && git push origin $NEW_TAG
  7. Publish: gh release create $NEW_TAG <artifacts> docs/release-notes-$VERSION.md \\
       --verify-tag --title "Boot Bitch $VERSION" --notes-file docs/release-notes-$VERSION.md
EOF
