# Publishing a Boot Bitch release

The release tree is meant to be copied recursively. The source code lives in
`src/`, with tests, scripts, packaging metadata, resources, documentation and
screenshots alongside it. A root-only upload is incomplete and will make the
README links appear broken.

The version is declared once in `CMakeLists.txt`
(`project(BootRepair VERSION ...)`). Bump it there first, then prepare the
release documentation with the scripts below. The canonical tree
`/home/amiga/Projects/Boot_Repair` has no Git history of its own;
`/home/amiga/Projects/Boot_Bitch-github` is the staging clone whose `main` is
pushed to GitHub.

## Release invariants

The 0.2.24 release was published with three documentation mistakes. The
release tooling and this procedure exist so they cannot be repeated:

1. **Every previous README section is permanent.** A release adds a new
   `## <version> refinements` section *above* the existing ones. Never replace,
   rewrite or delete an earlier version's section. The 0.2.23 section had to be
   recovered from Git history after 0.2.24 replaced it.
2. **Release notes are scoped to one release.** `docs/release-notes-<version>.md`
   describes only the changes since the previous tag. It never rolls up
   earlier releases and never uses cumulative wording.
3. **The compare link points at the previous tag:**
   `[Full source diff: v<previous>...v<version>](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v<previous>...v<version>)`.
4. **`CHANGELOG.md` keeps `## Unreleased`** with a fresh placeholder after the
   new `## <version> — <date>` section is added. Keep the Unreleased entries
   limited to changes made since the last published release; they are the exact
   source of the next release's bullets.
5. **The README top references match the version:** the title
   `# Boot Bitch <version> — maintenance release`, `Version <version> includes`
   and `Still intentionally constrained in <version>`.

`scripts/verify-release.sh` checks all of these and must pass before tagging.

## 1. Prepare the release documentation

From the canonical tree:

```bash
cd /home/amiga/Projects/Boot_Repair
VERSION=0.2.25
PREVIOUS=v0.2.24    # optional; derived from compare links or `git tag` when omitted

./scripts/prepare-release.sh "$VERSION" "$PREVIOUS"
```

`prepare-release.sh` is refusal-first: it stops with a clear error when
`docs/release-notes-<version>.md` or a `## <version> refinements` README
section already exists. It:

- promotes the CHANGELOG `## Unreleased` entries into a dated
  `## <version> — <date>` section (or reuses an existing `<version>` section
  when the changelog already has one), leaving `## Unreleased` with a fresh
  placeholder;
- generates `docs/release-notes-<version>.md` containing only this version's
  bullets, the scoped intro sentence, the compare link to the previous tag and
  the artifact note;
- inserts `## <version> refinements` above the newest existing README section
  and updates the README's top-of-file version references;
- verifies that every previous `## X refinements` section survived and that the
  generated notes are scoped before installing anything. A failure before
  installation leaves the tree unchanged; a failure during installation rolls
  the touched files back.

The generated notes reuse the changelog bullets (wrapped but not otherwise
rewritten); review and polish the wording, then verify:

```bash
./scripts/verify-release.sh "$VERSION"
```

`verify-release.sh` is read-only. It fails when the notes are missing,
cumulative or linked to the wrong compare tag, when any permanent README
section (the maintained list plus every version in `CHANGELOG.md` and the
previous release) is missing, when the top-of-file version references or the
changelog section are absent, or when
`Development/release-<version>/release-notes-<version>.md` exists but differs
from the `docs/` copy.

## 2. Build and validate the artifacts

Build the complete release from the source tree; `scripts/build.sh` configures
Release, validates the staged install, creates the `.deb` on Debian-family
hosts and builds the versioned AppImage when the AppImage tooling is available:

```bash
cd /home/amiga/Projects/Boot_Repair
./scripts/build.sh
```

Build the Arch package in the Arch VM as the unprivileged `amiga` user (see
`docs/testing.md` for the VM workflow) and copy the verified artifacts,
`release-notes-<version>.md` and `SHA256SUMS` into
`Development/release-<version>/`. Generate `SHA256SUMS` from the final Debian
package, AppImage, Arch package and release notes with asset basenames so a
download can be checked with `sha256sum -c SHA256SUMS`:

```bash
VERSION=0.2.25
mkdir -p "Development/release-$VERSION"
cp "build-release/boot-repair_${VERSION}_amd64.deb" \
   "build-release/boot-repair_${VERSION}_x86_64.AppImage" \
   "docs/release-notes-$VERSION.md" "Development/release-$VERSION/"
cp "Development/build-arch-package/boot-bitch-${VERSION}-1-x86_64.pkg.tar.zst" \
   "Development/release-$VERSION/"
(
  cd "Development/release-$VERSION"
  sha256sum "boot-repair_${VERSION}_amd64.deb" \
            "boot-repair_${VERSION}_x86_64.AppImage" \
            "boot-bitch-${VERSION}-1-x86_64.pkg.tar.zst" \
            "release-notes-$VERSION.md" > SHA256SUMS
)
```

`Development/` is disposable and is never synced into the source tree or the
packages. The release assets are attached to the GitHub release and are not
copied into the source tree.

## 3. Sync the complete tree to the staging clone

Synchronize the complete local tree into the existing clone; the staging clone
keeps its own `.git` directory:

```bash
SOURCE=/home/amiga/Projects/Boot_Repair
DEST=/home/amiga/Projects/Boot_Bitch-github
VERSION=0.2.25

rsync -a --delete \
  --exclude='.git/' \
  --exclude='Development/' \
  --exclude='_CPack_Packages/' \
  --exclude='build*/' \
  --exclude='*.deb' \
  --exclude='*.rpm' \
  --exclude='*.pkg.tar.*' \
  --exclude='*.tar.gz' \
  --exclude='*.AppImage' \
  "$SOURCE/" "$DEST/"

# Remove any old packages tracked by a previous root-only upload.
find "$DEST" -maxdepth 1 -type f \
  \( -name '*.deb' -o -name '*.rpm' -o -name '*.pkg.tar.*' -o -name '*.tar.gz' -o -name '*.AppImage' \) -print -delete
```

Commit and push `main` before tagging:

```bash
cd "$DEST"
git add -A
git status
git diff --cached
git commit -m "Prepare Boot Bitch $VERSION release"
git push origin main
```

## 4. Tag and create the GitHub release

Create the annotated tag on the pushed commit and publish the release with the
per-version notes file:

```bash
cd /home/amiga/Projects/Boot_Bitch-github
VERSION=0.2.25

git tag -a "v$VERSION" -m "Boot Bitch $VERSION"
git push origin "v$VERSION"
gh release create "v$VERSION" \
  "/home/amiga/Projects/Boot_Repair/build-release/boot-repair_${VERSION}_amd64.deb" \
  "/home/amiga/Projects/Boot_Repair/build-release/boot-repair_${VERSION}_x86_64.AppImage" \
  "/home/amiga/Projects/Boot_Repair/Development/release-${VERSION}/boot-bitch-${VERSION}-1-x86_64.pkg.tar.zst" \
  "/home/amiga/Projects/Boot_Repair/Development/release-${VERSION}/SHA256SUMS" \
  "docs/release-notes-${VERSION}.md" \
  --verify-tag \
  --title "Boot Bitch ${VERSION}" \
  --notes-file "docs/release-notes-${VERSION}.md"
```

The tag is created only after `scripts/verify-release.sh "$VERSION"` and the
package/helper/EFI/UI contract checks pass.

## Recovering a lost README section

If a previous `## X refinements` section was replaced or deleted, recover it
from Git history instead of rewriting it from memory:

```bash
cd /home/amiga/Projects/Boot_Bitch-github
git log -S '## 0.2.23 refinements' --oneline -- README.md
# choose the commit before the section disappeared, then print it:
git show <commit>:README.md | sed -n '/^## 0.2.23 refinements/,/^## /p'
```

Add the recovered text back above the newer section, commit, and re-run
`scripts/verify-release.sh "$VERSION"` until it passes.

## Initial staging clone and GitHub authentication

For a new or missing clone, create the staging directory first:

```bash
git clone https://github.com/CaptainMorgan12/Boot_Bitch.git \
  /home/amiga/Projects/Boot_Bitch-github
```

Authenticate with the GitHub CLI before pushing:

```bash
sudo apt install gh
gh auth login
gh auth status
```

Choose **GitHub.com**, **HTTPS**, and browser authentication when prompted. If
Git prompts for HTTPS credentials instead, enter the GitHub username
`CaptainMorgan12` and paste a personal access token as the password; a GitHub
account password or email address will not work. Never put the token in a
command or commit it to the repository.

For a completely empty repository, the shorter alternative is: `git init`,
`git branch -M main`, `git remote add origin <repository-url>`, `git add -A`,
`git commit`, and `git push -u origin main`.

## Build and attach an AppImage

AppImages are generated locally because `linuxdeploy` and `appimagetool` are
separate upstream release utilities. `scripts/build.sh` runs the AppImage step
automatically as part of the standard release build when the tools are
available, so `build-release/` contains both the `.deb` and the versioned
AppImage. Install
[`linuxdeploy`](https://github.com/linuxdeploy/linuxdeploy/releases), its
`linuxdeploy-plugin-qt`, and
[`appimagetool`](https://github.com/AppImage/appimagetool/releases), make all
three available on `PATH`, and run this from the source tree to build only the
AppImage:

```bash
./scripts/build-appimage.sh
```

The script creates `build-release/boot-repair_<version>_x86_64.AppImage` and
uses linuxdeploy's Qt integration to bundle the GUI's shared libraries along
with desktop metadata, icons, documentation and guarded helper.
The helper still uses host `pkexec`/Polkit and repair commands, so the target
system must provide those runtime dependencies. Attach the generated artifact
to the matching GitHub release (the AppImage remains ignored by source-tree
syncs):

```bash
gh release upload "v$VERSION" \
  "/home/amiga/Projects/Boot_Repair/build-release/boot-repair_${VERSION}_x86_64.AppImage"
```

If a tool is not on `PATH`, provide its path explicitly, for example:
`LINUXDEPLOY=/path/to/linuxdeploy LINUXDEPLOY_PLUGIN_QT=/path/to/linuxdeploy-plugin-qt APPIMAGETOOL=/path/to/appimagetool ./scripts/build-appimage.sh`.
With only appimagetool available the script emits a host-library warning; that
fallback is useful for smoke tests but should not be uploaded as a portable
release.

## GitHub web upload

Choose **Add file → Upload files**, then drag the contents of this project
folder while keeping the directory structure. Confirm that `src/`, `tests/`,
`scripts/`, `data/`, `resources/`, `docs/`, and `.github/` appear in the upload
before committing. The eight PNG files in `docs/screenshots/` must be included
for the README images to render.

Do not copy generated `build/` directories, CMake cache files, `.deb` packages,
`.AppImage` files, or temporary debug output. They are excluded by `.gitignore`.
