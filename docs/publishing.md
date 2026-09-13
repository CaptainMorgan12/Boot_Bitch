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

## GitHub web upload

Choose **Add file → Upload files**, then drag the contents of this project
folder while keeping the directory structure. Confirm that `src/`, `tests/`,
`scripts/`, `data/`, `resources/`, `docs/`, and `.github/` appear in the upload
before committing. The eight PNG files in `docs/screenshots/` must be included
for the README images to render.

Do not copy generated `build/` directories, CMake cache files, `.deb` packages,
`.AppImage` files, or temporary debug output. They are excluded by `.gitignore`.
