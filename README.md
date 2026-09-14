# Boot Bitch 0.2.21 — maintenance release

Boot Bitch is a native Qt 6 Linux recovery utility. The application command and Debian package retain the stable technical name `boot-repair` for compatibility; the source repository is `Boot_Bitch`. Version 0.2.21 includes the guarded diagnostics cache, simulation-first repair stack, Btrfs snapshot recovery, audited chroot shell, verified file copy, responsive Qt interface, and cross-desktop packaging prepared for public distribution. Each repair layer performs the strongest practical read-only or trial preflight available, applies only recognized deterministic corrections, and stops on unknown failures instead of guessing. The application was written to address the lack of coherent tooling that lets a non-developer restore a system so it can boot after something goes wrong. Common boot issues can be addressed directly, while the diagnostics, chroot shell and file-copy workflows also support troubleshooting and manual fixes.

This project was written with LLM assistance under human guidance, requirements and manually verified tests.

**Developer:** CaptainMorgan12

## Minimum recovery requirement

Boot Bitch must be launched from a **different booted Linux environment for ordinary repair-target work**. Use a Linux live USB, recovery stick, or another Linux installation on a different physical drive when repairing another system. The running host remains protected from ordinary target selection; an explicit Host Maintenance scope is available for guarded native maintenance of the active Debian/Ubuntu-family system.

## Safety boundary

This build can:

- enumerate and rank Linux storage devices with the existing read-only scanner;
- protect the top-level device backing `/`, `/boot` and `/boot/efi`;
- keep the protected running host separate from ordinary repair targets, while allowing deliberate Host Maintenance selection after repeating the host identity and mount checks;
- select a physical repair disk and its best visible Linux root component;
- unlock a selected non-host LUKS repair component through `cryptsetup` + Polkit without placing the passphrase in command arguments or logs;
- prefer the newly visible Linux filesystem inside an unlocked LUKS stack;
- mount the selected target in a private `/run/boot-repair` session;
- independently re-check that the selected target does not back the running host before any write;
- validate `/etc/os-release`, `/etc/fstab`, `/etc/crypttab`, boot files and target-family support;
- mount target `/boot` and `/boot/efi` entries when they are safely resolvable on the selected disk;
- bind the minimum runtime filesystems needed for a chroot repair;
- on Debian/Ubuntu/TUXEDO-family targets, run guarded package repair, DKMS rebuild, offline SDDM graphical-login recovery, initramfs rebuild, distribution-aware EFI/UKI recovery, boot-stack reconciliation, and `update-grub`;
- in Host Maintenance scope, run the same supported repair stages natively on the active Debian/Ubuntu-family system, with package-lock checks, writable-mount checks, and a private read-only EFI-variable namespace for indirect hooks;
- inspect the running host with the full read-only diagnostic set, including EFI/UKI files, embedded kernel/cmdline, PARTUUID-owned firmware entries, BootOrder, GRUB handoff, and journal evidence;
- require an explicit confirmation in the GUI before every modifying operation;
- request administrator authorization once per Boot Bitch window and retain only the narrow whitelisted helper session until explicitly locked or the app exits;
- show modifying operations and Run All diagnostics in a modal privileged-output window, while individual diagnostics write directly into the persistent Results pane; persist the session log to `/var/log/boot-repair-session.log` only for requests that deliberately crossed the read-write boundary and still have a writable target log mount;
- copy files Host → Repair or Repair → Host with guarded `rsync -aHAX --numeric-ids`, smart ownership validation, no `--delete`, and post-copy verification;
- keep Repair → Host target mounts read-only and promote Host → Repair read-write only after the same independent target safety checks;
- perform a guarded transactional Btrfs `@` rollback after a read-only preflight, while keeping the source snapshot unchanged and retaining the previous root under a timestamped `@rollback-before-*` name;
- build a Release executable and Debian package with the privileged helper included.

Still intentionally constrained in 0.2.21:

- automatic/implicit EFI-loader reinstall: EFI repair remains an explicit action or opt-in Full Repair stage;
- transactional rollback is limited to Btrfs layouts with a normal top-level `@` root and Snapper-style root snapshots; it refuses unsupported layouts rather than guessing;
- modifying repair backends for non-Debian package families; native Host Maintenance changes use the same Debian/Ubuntu-family limitation;




## Screenshots

These anonymized screenshots show Boot Bitch 0.2.15 running in an Ubuntu recovery VM. The target is a separate virtual disk; no personal host paths or physical-device identifiers are included.

### Systems

![Systems target selection](docs/screenshots/01-systems-target-selection.png)

### Diagnostics

![Diagnostics report](docs/screenshots/02-diagnostics-report.png)

### Repair

![Repair plan and diagnostic freshness gate](docs/screenshots/03-repair-confirmation.png)

### Snapshots

![Snapshot handling on an ext4 target](docs/screenshots/04-snapshots.png)

### Chroot Shell

![Chroot shell](docs/screenshots/05-chroot-shell.png)

### File Copy

![File copy](docs/screenshots/06-file-copy.png)

### Logs

![Application log](docs/screenshots/07-application-log.png)

### Settings

![Settings](docs/screenshots/08-settings.png)

## 0.2.21 refinements

- Bundle the completed semantic Qt/KDE icon atlas and restore the stable 0.2.13
  defaults for tabs, diagnostics, repair stages and actions.
- Prefer the active desktop theme, equivalent freedesktop aliases and bundled
  artwork before using native Qt glyphs when a theme is incomplete.
- Add filesystem-aware device icons, solid-placeholder detection and native
  high-contrast fallbacks so AppImage controls remain visible without a host
  icon theme.
- Keep repeatable AppImage development builds pointed at the local
  `Development/tools` appimagetool installation.

## 0.2.19 refinements

- Add CaptainMorgan12 attribution to the About dialog, AppStream metadata,
  Debian package metadata and copyright files.
- Add a Qt-aware AppImage build workflow with optional linuxdeploy Qt bundling.
- Keep GNOME/GTK snapshot rows compact and improve dark-theme icon contrast,
  including terminal and disabled-stage icons.

## 0.2.18 refinements

- Keep snapshot and host-capability table rows compact under GNOME/GTK styles;
  only wrapped text receives additional height.
- Tint monochrome theme glyphs to the active readable text color in dark
  palettes while preserving colored status artwork.
- Update GitHub Actions checkout to the Node.js 24-compatible `actions/checkout@v5`.

## 0.2.17 refinements

- Add optional Debian package suggestions for the Qt GTK platform theme, XDG desktop portal integration, and the dedicated GNOME Qt platform theme. KDE installations continue using their native Plasma style.
- Document GNOME launch options (`QT_QPA_PLATFORMTHEME=gtk3` or `gnome`) and keep theme selection environment-driven so dark/light settings and accessibility preferences come from the desktop.

## 0.2.16 refinements

- Fix Qt 6.4/Ubuntu CI compatibility for compact single-line snapshot rows.
- Install `pkexec` in the GitHub Actions runner so repair-readiness UI tests exercise the same guarded capability path as supported desktop systems.
- Accept a readable uploaded shell helper when a web-based copy loses its executable bit, invoking it explicitly through `/bin/bash`.

## 0.2.15 refinements

- Add the **Chroot Shell** page for one reviewed command at a time inside a fresh target chroot. The selected root, Btrfs subvolumes, `/boot`, `/boot/efi`, device access and temporary resolver are prepared by the guarded helper; output is shown in the page and Logs.
- Load Btrfs snapshots asynchronously when the application starts so they are ready when the Snapshots page is opened. Unsupported filesystems report a clear read-only explanation.
- Mark diagnostic evidence stale after target edits, repairs, snapshot rollback, Host → Repair copy, or shell commands that may have changed files. Failed commands with no reported target change do not invalidate evidence.
- Keep the newest application, diagnostic, repair, shell, unlock, snapshot and file-copy entries first, with visual section separators and stale target blocks.
- Add responsive tab navigation, stable scrollbar gutters, content-sized table rows, adjustable Repair splitters and theme-neutral log highlighting for KDE and GNOME.
- Add anonymized Ubuntu VM screenshots and a disposable VM testing guide.

## Pre-release development notes (0.2.14 and earlier)

Versions 0.2.14 and earlier were development iterations before the first
public release. Their detailed history is retained in [CHANGELOG.md](CHANGELOG.md)
and the consolidated [development summary](docs/development-summary.md); they
are not presented as public GitHub releases.

## Development summary

The 0.1–0.2 development series established the recovery foundation, target safety boundary, diagnostics cache, simulation-first repair stack, encrypted-volume handling, Btrfs rollback, graphical-login recovery, Chroot Shell, verified File Copy, responsive interface and Debian packaging. The consolidated historical summary is in [docs/development-summary.md](docs/development-summary.md); the full version-by-version record remains in [CHANGELOG.md](CHANGELOG.md).

For recursive GitHub upload instructions, see [docs/publishing.md](docs/publishing.md).

## Source tree

```text
Boot_Bitch/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── CHANGELOG.md
├── .gitignore
├── .github/workflows/ci.yml
├── data/                   desktop entry, AppStream metadata, man page
├── docs/                   testing guide, publishing guide, summary and screenshots
├── resources/              Qt resources and icons
├── scripts/                helper, build, packaging and validation tools
├── src/                    Qt application and target workflow
└── tests/                  UI and contract tests
```

Generated build output belongs in `build/` and is ignored by Git.

## Design goals

- Native Qt 6 Widgets application with standard resizable window controls.
- Follow the active Qt/KDE/desktop theme; no hard-coded light/dark stylesheet.
- Keep the source tree and dependency set small.
- KDE Frameworks 6 KAuth remains optional; guarded builds use a separate root helper launched through Polkit/pkexec.
- Privileged work is isolated from the normal GUI process and exposes only whitelisted operations.
- Missing optional runtime tools disable only the related feature.
- Never silently install host packages; target package changes require explicit repair confirmation.
- Storage discovery must not assume NVMe, internal disks, encryption or one filesystem layout.
- Current-system protection is mandatory and cannot be disabled.

### GNOME and KDE/Qt desktop integration

Boot Bitch is a Qt 6 Widgets application and deliberately does not force a
global stylesheet or the Fusion style. It follows the desktop's font, palette,
spacing, icons, dialogs and accessibility settings. KDE uses its normal Qt
platform theme. On GNOME, install the optional Qt platform-theme and portal
plugins for GTK-like controls and native file dialogs:

```bash
sudo apt install qt6-gtk-platformtheme qt6-xdgdesktopportal-platformtheme
```

`qgnomeplatform-qt6` is another optional GNOME theme implementation when your
distribution provides it. With that package installed, an explicit
`QT_QPA_PLATFORMTHEME=gnome` launch selects it; otherwise leave the variable
unset and Qt will use the available desktop platform theme. The GTK plugin can
also be selected explicitly with `QT_QPA_PLATFORMTHEME=gtk3 boot-repair`.
These packages only affect presentation. They do not change Boot Bitch's
privilege boundary or repair logic, and the Debian package lists them as
optional suggestions so minimal KDE, GNOME and headless installations remain
supported.

To compare available diagnostic and tab icons on the current desktop theme,
run `./scripts/check-icon-theme.sh`. It reports missing names and suggests
equivalent freedesktop icons without changing the active theme.

## Build dependencies

Required:

- C++17 compiler (GCC or Clang)
- CMake 3.20+
- Ninja
- Qt 6 Core/Gui/Widgets development files

Runtime/packaging support used by guarded repair builds:

- Polkit / `pkexec`
- util-linux (`lsblk`, `findmnt`, `blkid`, mount tools)
- systemd `systemctl` for offline graphical-target/SDDM repair
- `efibootmgr` for guarded UEFI entry/order inspection when firmware variables are available
- binutils `objcopy` for TUXEDO UKI verification
- Debian packaging tools when generating `.deb` files
- KDE Frameworks 6 KAuth development files remain optional

### Debian / Ubuntu / TUXEDO OS — one command

```bash
./scripts/setup-dev-deps.sh
```

Equivalent direct package command (KF6Auth is optional and the setup script adds it automatically when the distribution provides it):

```bash
sudo apt update && sudo apt install --no-install-recommends -y \
    build-essential cmake ninja-build pkg-config \
    qt6-base-dev qt6-base-dev-tools extra-cmake-modules \
    dpkg-dev desktop-file-utils lintian \
    pkexec util-linux mount rsync cryptsetup btrfs-progs systemd \
    efibootmgr binutils lvm2 mdadm
```

To inspect the environment without installing anything:

```bash
./scripts/check-dev-env.sh
```

## One-command Release build + Debian package

Run:

```bash
./scripts/build.sh
```

This performs a clean Release build and validates an isolated staged install. On Debian-family build hosts with the normal packaging tools available, it also creates a `.deb` in the same run. Nothing is installed. Output is written under `build-release/`.

Run the executable directly from that tree:

```bash
./build-release/boot-repair
```

Validate the generated package without installing it:

```bash
./scripts/test-deb.sh build-release/*.deb
```

## Build from source

```bash
cd <staging-clone>
rm -rf build
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"
```

Run without installing:

```bash
./build/boot-repair
```

To preview the same build with a desktop platform theme, set the Qt platform
theme for that launch:

```bash
# Native Qt/KDE styling (the default on Plasma and other Qt desktops)
./build/boot-repair

# GNOME platform theme, when qgnomeplatform-qt6 is installed
QT_QPA_PLATFORMTHEME=gnome ./build/boot-repair

# GTK3 platform plugin, when qt6-gtk-platformtheme is installed
QT_QPA_PLATFORMTHEME=gtk3 ./build/boot-repair
```

For a release build:

```bash
rm -rf build
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

The project uses direct optional KF6 component discovery:

```cmake
find_package(KF6Auth QUIET NO_MODULE)
```

so distributions do not need to provide an umbrella `KF6Config.cmake`.

## Install

Build a package first, then either simulate or install it:

```bash
./scripts/build.sh
./scripts/install.sh --dry-run
./scripts/install.sh
```

`install.sh` does nothing destructive until you type `INSTALL` exactly. The normal Debian path installs the generated `.deb` through APT.

After package installation, launch the installed command as follows:

```bash
# Native Qt/KDE styling
boot-repair

# GNOME: install the optional integration packages once, then launch normally
sudo apt install qt6-gtk-platformtheme qt6-xdgdesktopportal-platformtheme
boot-repair

# Explicit GNOME or GTK platform theme selection
QT_QPA_PLATFORMTHEME=gnome boot-repair
QT_QPA_PLATFORMTHEME=gtk3 boot-repair
```

Equivalent standard CMake flow for a manual source install:

```bash
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build
sudo cmake --install build
```

Alternative prefix:

```bash
PREFIX=/opt/boot-repair ./scripts/install.sh
```


## Build a Debian package without installing it

On Debian-family systems, build a real `.deb` in an isolated `build-deb/` tree:

```bash
./scripts/package-deb.sh
```

The script configures a Release build, stages the CMake install under `build-deb/stage/`, verifies the expected executable and desktop entry, then invokes CPack. Nothing is installed on the host.

Validate the generated package without installing it:

```bash
./scripts/test-deb.sh
```

Validation includes `dpkg-deb` metadata/content inspection, extraction into a temporary directory, executable/desktop-file checks, `ldd`, an APT install simulation when APT is available, a `dpkg --dry-run`, and optional `desktop-file-validate`/`lintian` checks when those tools are installed.

The equivalent direct packaging target is:

```bash
cmake -S . -B build-deb -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build-deb
cmake --build build-deb --target package
```

The Debian package uses `/usr` paths even though a normal manual CMake installation can still use `/usr/local` or another chosen prefix.

An AppImage is also available for portable testing and systems without a local
package build. It bundles the GUI and guarded helper, while privileged actions
still use the host's `pkexec`/Polkit and system utilities. Install the normal
runtime tools listed above before attempting a repair. Build one locally with
`./scripts/build-appimage.sh`; the resulting artifact is written to
`build-release/` and is excluded from Git source uploads.

The AppImage is a convenience distribution format, not a sandbox: the helper
must be authorized by the host and reads or changes only the explicitly
selected repair target. For regular installation and desktop integration, the
Debian package remains the recommended format.

## Build an AppImage

Install [`linuxdeploy`](https://github.com/linuxdeploy/linuxdeploy/releases),
its `linuxdeploy-plugin-qt`, and
[`appimagetool`](https://github.com/AppImage/appimagetool/releases), make all
three available on `PATH`, then run:

```bash
./scripts/build-appimage.sh
```

To use tools installed at another path:

```bash
LINUXDEPLOY="$HOME/bin/linuxdeploy" \
APPIMAGETOOL="$HOME/bin/appimagetool" ./scripts/build-appimage.sh
```

The script creates a clean Release build, stages the complete CMake install
under an AppDir, validates the executable and privileged helper, lets
`linuxdeploy` bundle Qt/shared-library dependencies, and invokes
`appimagetool`. It does not install anything on the host. Run the artifact with
`chmod +x build-release/boot-repair_0.2.21_x86_64.AppImage` followed by
`./build-release/boot-repair_0.2.21_x86_64.AppImage`. If only `appimagetool`
is available, the script creates a diagnostic AppImage and warns that it uses
the host's Qt libraries; do not publish that fallback as a portable release.

## Uninstall

The uninstall helper uses CMake's generated `build/install_manifest.txt`, lists every installed file, and requires an explicit `UNINSTALL` confirmation:

```bash
./scripts/uninstall.sh
```

## Current pages

### Systems

Shows the protected running host separately, then ranks selectable physical repair drives. Technical child volumes are collapsed by default and are informational only. Locked LUKS candidates expose an Unlock button beside Select Target; after a successful unlock the device topology refreshes and the mapped Linux filesystem becomes the preferred repair component. Host Maintenance deliberately selects the protected active system for its guarded native repair stages.

### Diagnostics

Provides always-available troubleshooting navigation for the protected **Running Host** or the explicitly selected **repair drive**. Choose the scope from **Inspect**. Both scopes use the full read-only inspection backend; host EFI/UKI diagnostics classify every firmware entry by ESP PARTUUID and show the active host's actual boot files, embedded UKI kernel/cmdline and BootOrder. Results remain read-only, selectable, copyable and savable. The Repair → Validate action adds a privileged read-only mount validation when deeper repair-drive confirmation is needed.

### Chroot Shell

Runs one command at a time as root inside the selected repair system. Commands require explicit confirmation, are logged with their output, and invalidate affected diagnostics only when the command may have changed target files.

### Repair

Settings define the Full Repair plan. Enabled stages are shown in execution order. Every configurable stage also appears one-for-one under Individual repair tools: package configuration, broken-dependency repair, package metadata refresh, adaptive package upgrade, DKMS, SDDM graphical login, initramfs, EFI/UKI, and GRUB. Select a repair drive for offline repair, or choose **Host Maintenance** on the protected Running Host card to run the same supported stages natively on the active Debian/Ubuntu-family system. Host maintenance repeats disk/root/boot-mount identity checks, prevents package races, and isolates firmware variables from indirect hooks; the helper then reconciles only the active host's EFI entry while preserving every other disk's entries and BootOrder. After reconciliation, decoded entries on the selected ESP keep their distribution/vendor label and receive the selected disk model once (for example `tuxedo WD_BLACK SN8100 HS 4000GB`); an existing model name is never duplicated. If an `iPXE.efi` loader exists on that ESP but has no matching firmware entry, TUXEDO systems receive `WFAI <model>` and other distributions receive `iPXE <model>`, always within the same PARTUUID boundary. Validate environment remains an automatic preflight, and Boot stack reconciliation remains a manual recovery tool.

### Snapshots

Shows the guarded Btrfs rollback workflow. Loading and inspection remain read-only. **Roll Back to Selected** performs a separate read-only plan first, then creates a writable copy of the chosen root snapshot, preserves the prior `@`, promotes the copy to the normal writable `@`, sets it as the Btrfs default, reconciles initramfs/TUXEDO UKI/GRUB, and automatically restores the preserved root if critical post-switch validation fails.

### File Copy

A direction selector runs either Host → Repair or Repair → Host transfers. Host paths use native file dialogs; repair-side paths are absolute paths inside the selected repair system. **Browse Target Folders** reads the selected repair tree through a temporary read-only helper mount for each directory view, so it never displays the host folders as a substitute and never leaves the target mounted. A destination may still be entered as a new absolute path when the folder does not exist yet. Preview is a guarded rsync dry-run. Copy execution validates containment and ownership, uses `rsync -aHAX --numeric-ids` without `--delete`, and verifies the result with a checksum/metadata dry-run plus SHA-256 for regular files.

### Logs

Timestamped application events wrap responsively. Use **View → Wrap Log Lines** to switch to exact unwrapped formatting.

### Settings

Contains live device-display preferences, the Full Repair plan, mandatory safety rules, and host capability/dependency reporting.

## Runtime capability model

Boot Bitch checks capabilities independently. Examples include `lsblk`, `blkid`, `findmnt`, `cryptsetup`, `btrfs`, `rsync`, `chroot`, `grub-install`, `update-grub`, `update-initramfs`, `dkms`, optional LVM tools, and `mdadm`.

Missing optional tools disable the related action. Modifying repair execution is currently limited to supported Debian/Ubuntu-family targets and never installs missing host tools silently.

## License

Boot Bitch is released under the MIT License. See `LICENSE`.
