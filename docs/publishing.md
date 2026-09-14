# Publishing the complete source tree

The release tree is meant to be copied recursively. The source code lives in
`src/`, with tests, scripts, packaging metadata, resources, documentation and
screenshots alongside it. A root-only upload is incomplete and will make the
README links appear broken.

## Recommended Git upload

Because the GitHub repository may already contain a partial upload, clone it and
then synchronize the complete local tree. This preserves the directory layout
and removes old root-level packages or files that are no longer part of the
release.

```bash
sudo apt install git rsync

SOURCE=/home/amiga/Projects/Boot_Repair
DEST=/home/amiga/Projects/Boot_Bitch-github

git clone https://github.com/CaptainMorgan12/Boot_Bitch.git "$DEST"
rsync -a --delete \
  --exclude='.git/' \
  --exclude='build*/' \
  --exclude='*.deb' \
  --exclude='*.AppImage' \
  "$SOURCE/" "$DEST/"

# Remove any old packages tracked by the partial upload.
find "$DEST" -maxdepth 1 -type f \
  \( -name '*.deb' -o -name '*.AppImage' \) -print -delete

cd "$DEST"
git add -A
git status
git commit -m "Prepare Boot Bitch 0.2.15 release"
git push origin main
```

Authenticate with the GitHub CLI before cloning:

```bash
sudo apt install gh
gh auth login
gh auth status
```

Choose **GitHub.com**, **HTTPS**, and browser authentication when prompted. If
Git prompts for HTTPS credentials instead, enter the GitHub username
`CaptainMorgan12` and paste a personal access token as the password; a GitHub
account password or email address will not work. Never put the token in a
command or commit it to the repository. For later updates, run the same
`rsync` command from an existing clone, then `git add -A`, `git commit`, and
`git push`.

For a completely empty repository, the shorter alternative is: `git init`,
`git branch -M main`, `git remote add origin <repository-url>`, `git add -A`,
`git commit`, and `git push -u origin main`.

## Create the two GitHub releases

Create the first release while the clone is still at the original 0.2.15
commit. The package saved at `/tmp/boot-repair_0.2.15_amd64.deb` is the
package built before the CI fixes.

```bash
cd /home/amiga/Projects/Boot_Bitch-github-fixed
git status
git log -1 --oneline
git tag -a v0.2.15 -m "Boot Bitch 0.2.15 first public release"
git push origin v0.2.15
gh release create v0.2.15 \
  /tmp/boot-repair_0.2.15_amd64.deb \
  --title "Boot Bitch 0.2.15" \
  --notes "First official Boot Bitch release."
```

Then synchronize the local maintenance fixes, push them to `main`, rebuild,
and create the second release:

```bash
SOURCE=/home/amiga/Projects/Boot_Repair
DEST=/home/amiga/Projects/Boot_Bitch-github-fixed

rsync -a --delete \
  --exclude='.git/' \
  --exclude='build*/' \
  --exclude='*.deb' \
  --exclude='*.AppImage' \
  "$SOURCE/" "$DEST/"
find "$DEST" -maxdepth 1 -type f \
  \( -name '*.deb' -o -name '*.AppImage' \) -print -delete

cd "$DEST"
git add -A
git commit -m "Boot Bitch 0.2.16 CI compatibility fixes"
git push origin main
./scripts/build.sh
QT_QPA_PLATFORM=offscreen ctest --test-dir build-release --output-on-failure
gh release create v0.2.16 \
  build-release/boot-repair_0.2.16_amd64.deb \
  --title "Boot Bitch 0.2.16" \
  --notes "Maintenance release: fixed Qt 6.4 CI row sizing, installed pkexec in CI, and made helper launching tolerant of lost executable bits."
```

The release assets are attached to GitHub Releases and are not copied into the source tree.

## Build and attach an AppImage

AppImages are generated locally because `linuxdeploy` and `appimagetool` are
separate upstream release utilities. Install
[`linuxdeploy`](https://github.com/linuxdeploy/linuxdeploy/releases), its
`linuxdeploy-plugin-qt`, and
[`appimagetool`](https://github.com/AppImage/appimagetool/releases), place
both executables on `PATH`, and run this from the source tree:

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
gh release upload v0.2.19 \
  /home/amiga/Projects/Boot_Repair/build-release/boot-repair_0.2.19_x86_64.AppImage
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
