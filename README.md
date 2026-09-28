# Boot Bitch 0.2.26 — maintenance release

Boot Bitch is a native Qt 6 Linux recovery utility. The application command and Debian package retain the stable technical name `boot-repair` for compatibility; the source repository is `Boot_Bitch`. Version 0.2.26 includes the guarded diagnostics cache, simulation-first repair stack, Btrfs snapshot recovery, audited chroot shell, verified file copy, responsive Qt interface, and native RPM/APK artifacts with Alpine, Arch and Fedora repair backends. Each repair layer performs the strongest practical read-only or trial preflight available, applies only recognized deterministic corrections, and stops on unknown failures instead of guessing. The application was written to address the lack of coherent tooling that lets a non-developer restore a system so it can boot after something goes wrong. Common boot issues can be addressed directly, while the diagnostics, chroot shell and file-copy workflows also support troubleshooting and manual fixes.

This project was written with LLM assistance under human guidance, requirements and manually verified tests.

**Developer:** CaptainMorgan12

## Supported systems

Boot Bitch must be launched from a **different booted Linux environment for ordinary repair-target work**: a Linux live USB, recovery stick, or another Linux installation on a different physical drive. The running host can also be selected explicitly as a guarded Host Maintenance target. The GUI needs a Qt 6.4+ desktop. Repair backends are selected from read-only probe evidence, never from the distribution name:

- **Debian / Ubuntu / TUXEDO and derivatives** — APT/dpkg package repair, initramfs-tools, DKMS, GRUB, TUXEDO UKI and EFI/UKI recovery.
- **Arch-family** — sandboxed pacman transactions, mkinitcpio, conventional GRUB/EFI and systemd-boot/UKI layouts.
- **Fedora / RHEL and openSUSE / SUSE** (0.2.25) — guarded DNF5/RPM package transactions, dracut initramfs with `lsinitrd` verification, GRUB2 configuration and boot-code repair, and the systemd GDM display path.
- **Alpine** (0.2.25) — `apk fix`/`apk upgrade` transactions, mkinitfs initramfs, config-only extlinux regeneration, OpenRC display-manager restore and guarded UEFI GRUB repair.

Host Maintenance runs the supported stages natively on the active Debian/Ubuntu, Arch or Alpine system. EFI/UKI stages remain Debian/Arch-only, and unknown boot layouts are refused instead of guessed.

## Install

Download the artifact for your system from the GitHub release page and verify it against the attached `SHA256SUMS`:

| System | Artifact | Install |
| --- | --- | --- |
| Debian/Ubuntu/TUXEDO | `boot-repair_0.2.25_amd64.deb` | `sudo apt install ./boot-repair_0.2.25_amd64.deb` |
| Arch | `boot-bitch-0.2.25-x86_64.pkg.tar.zst` | `sudo pacman -U boot-bitch-0.2.25-x86_64.pkg.tar.zst` |
| Fedora/RHEL | `boot-bitch-0.2.25.x86_64.rpm` | `sudo dnf install ./boot-bitch-0.2.25.x86_64.rpm` |
| openSUSE/SUSE | `boot-bitch-0.2.25.x86_64.rpm` | `sudo zypper install ./boot-bitch-0.2.25.x86_64.rpm` |
| Alpine | `boot-bitch-0.2.25.apk` | `sudo apk add --allow-untrusted ./boot-bitch-0.2.25.apk` |
| Any Linux desktop | `boot-repair_0.2.25_x86_64.AppImage` | `chmod +x boot-repair_0.2.25_x86_64.AppImage && ./boot-repair_0.2.25_x86_64.AppImage` |

The native packages install the GUI, bundled icons, desktop metadata, the manual page, the EFI label helper and the privileged helper at `/usr/libexec/boot-repair/boot-repair-helper`. Privileged actions are authorized through Polkit/`pkexec` and use the host's runtime tools (`mount`, `cryptsetup`, `btrfs`, `efibootmgr`, and the selected distribution's boot and package tools); install the tools needed for the repair you intend to run.

Launch the installed command as `boot-repair`. On GNOME, install the optional `qt6-gtk-platformtheme` and `qt6-xdgdesktopportal-platformtheme` packages for GTK-like controls and native file dialogs; KDE uses its normal Qt platform theme. The AppImage uses the same privilege boundary and is intended for portable use, not as a replacement for the native package; it bundles the SVG icon engine and the desktop integration (Qt platform themes and widget styles) present on its build host, so the release AppImage is built on a Debian-family desktop (see [Build packages](#build-packages)). File dialogs inside the AppImage use Qt's own implementation because the KIO/GTK worker and portal services the native dialogs rely on are not bundled; installed builds keep their desktop's native dialogs.

`./scripts/install.sh` detects the host package family and installs a locally built `.deb`, RPM, Arch package or APK after an explicit `INSTALL` confirmation. `./scripts/install.sh --source` installs a staged CMake build under `/usr/local` (or another absolute `PREFIX`), and `./scripts/uninstall.sh` removes a detected native package or the source install after an explicit `UNINSTALL` confirmation.

## Build from source

### Dependencies

Required:

- C++17 compiler (GCC or Clang)
- CMake 3.20+
- Ninja
- Qt 6.4+ Core/Gui/Widgets development files (the CI suite builds against Ubuntu's Qt 6.4)

Runtime and packaging support used by guarded repair builds:

- Polkit / `pkexec`; util-linux (`lsblk`, `findmnt`, `blkid`, mount tools)
- `rsync` and `cryptsetup` for verified file copy and LUKS unlock
- systemd `systemctl` for graphical-target and display-manager repair
- `efibootmgr` for guarded UEFI entry/order inspection
- binutils `objcopy` for TUXEDO UKI verification; Python 3 for guarded EFI label updates
- file system check tools (for example `e2fsprogs`, `dosfstools`, `btrfs-progs`, `xfsprogs`) for the read-only-first file system repair engine
- Debian packaging tools for `.deb`, `rpmbuild` for RPM, `makepkg` for Arch, `abuild` for APK, and CPack's TGZ generator for the package-manager-neutral archive
- KDE Frameworks 6 KAuth development files remain optional

`./scripts/setup-dev-deps.sh` installs the build/runtime set for the detected package manager (APT/dpkg, pacman, DNF/RPM, zypper/RPM or apk), and `./scripts/check-dev-env.sh` inspects the environment without installing anything. The Debian/Ubuntu/TUXEDO command is:

```bash
sudo apt update && sudo apt install --no-install-recommends -y \
    build-essential cmake ninja-build pkg-config \
    qt6-base-dev qt6-base-dev-tools extra-cmake-modules \
    dpkg-dev desktop-file-utils lintian \
    pkexec util-linux mount rsync cryptsetup btrfs-progs systemd \
    efibootmgr binutils python3 lvm2 mdadm
```

### One-command Release build

```bash
./scripts/build.sh
```

This performs a clean Release build, validates an isolated staged install and writes the artifacts under `build-release/`. On Debian-family build hosts it also creates the `.deb`; when `linuxdeploy`/`appimagetool` are available it creates the versioned AppImage with its `.zsync` update metadata, so Gear Lever/AppImageUpdate can detect new GitHub releases. Nothing is installed.

```bash
./build-release/boot-repair                   # run the build directly
./scripts/test-deb.sh build-release/*.deb     # validate without installing
```

### Manual build

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
./build/boot-repair
```

A standard source install uses `-DCMAKE_INSTALL_PREFIX=/usr/local` (or another prefix) followed by `sudo cmake --install build`. KF6Auth is discovered directly with `find_package(KF6Auth QUIET NO_MODULE)`, so distributions do not need an umbrella `KF6Config.cmake`.

### Build packages

Each packaging script stages a Release build and creates the artifact without installing it; the generated path is printed and can be redirected with `BUILD_DIR`:

```bash
./scripts/package-deb.sh       # Debian package
./scripts/package-arch.sh      # native Arch package (makepkg)
./scripts/package-rpm.sh       # RPM (CPack/rpmbuild)
./scripts/package-alpine.sh    # signed APK (abuild, Alpine build user)
./scripts/package-tarball.sh   # package-manager-neutral TGZ
./scripts/build-appimage.sh    # AppImage + .zsync under build-release/
```

Build the release AppImage on a Debian-family desktop (the reference host is
TUXEDO OS) with `qt6-svg-plugins`, `plasma-integration`,
`qt6-gtk-platformtheme` and `qgnomeplatform-qt6` installed: `linuxdeploy` can
only bundle the Qt SVG icon engine and the platform themes/widget styles the
build host provides, so widget styling matches the desktop only when the
AppImage is built there. File dialogs stay Qt's own inside the AppImage (the
native dialogs need worker/portal services the image does not carry) while
installed builds keep the desktop's native dialogs. `scripts/build-appimage.sh`
and `scripts/build.sh` print a non-fatal warning on other hosts and when no
platform theme was bundled, and `DEPLOY_PLATFORM_THEMES=0` opts out of theme
bundling.

The AppImage build probes the payload for the `.relr.dyn` relocations current
distributions emit and picks safe fallbacks automatically: a system `patchelf`
0.18 or newer when available, no stripping when the bundled strip cannot read
the section, and self-extracting AppImage mode when FUSE is unavailable. This
lets Arch, Fedora, Alpine and current Debian-family hosts build a working
AppImage; the build stops with installation instructions only when no
RELR-capable `patchelf` exists. `PATCHELF`, `NO_STRIP` and
`APPIMAGE_EXTRACT_AND_RUN` override the detection.

The AppImage's zsync self-update channel (what Gear Lever/AppImageUpdate read
through the embedded `gh-releases-zsync` update information) is
transport-authenticated over HTTPS but not content-signed: the `.zsync`
metadata carries zsync's SHA-1 block checksums, which verify that the update
client received the bytes the server served, not who authored them. Gating
AppImage self-updates behind a signed checksum (for example verifying the
release `SHA256SUMS` before applying a fetched update) is future work.

Validate generated packages without installing them:

```bash
./scripts/test-deb.sh
./scripts/test-rpm.sh
./scripts/test-apk.sh
```

## Testing

Configure a Release build and run the test suite:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
```

The tests cover the MainWindow workflow plus the chroot-shell, display-manager, filesystem-repair, host-snapshot, icon-atlas, backend-profile and EFI-label contracts; `ctest -L fast` runs the quick tier. CI additionally runs `bash -n scripts/*.sh` and `appstreamcli validate --no-net` on the AppStream metadata. The package validation scripts (`test-deb.sh`, `test-rpm.sh`, `test-apk.sh`) inspect metadata, dependencies and the installed file set of a built artifact without installing it.

## Safety model

- Boot Bitch runs as the normal desktop user. Privileged work is isolated in a whitelisted helper behind Polkit/`pkexec`; one authorization per scope is reused until the session is locked or the application exits.
- Ordinary repair-target actions refuse the running host. The top-level devices backing `/`, `/boot` and `/boot/efi` stay protected, and Host Maintenance must be selected explicitly, repeating host identity and mount checks.
- Every repair layer runs the strongest practical read-only or simulation preflight first and applies only recognized deterministic corrections; unknown failures stop the stage instead of guessing.
- Every modifying operation requires an explicit GUI confirmation, and diagnostics are cached per scope and invalidated by writes.
- LUKS passphrases are sent to the helper over standard input only, never in command arguments or logs.
- File copy uses `rsync -aHAX --numeric-ids` without `--delete`, validates containment and ownership, and verifies the result.
- Btrfs rollback keeps the source snapshot unchanged, preserves the previous root as `@rollback-before-*`, and restores it automatically when critical post-switch validation fails.
- Missing optional runtime tools disable only the related feature; host tools are never installed silently.

Still intentionally constrained in 0.2.26:

- automatic/implicit EFI-loader reinstall: EFI repair remains an explicit action or opt-in Full Repair stage;
- transactional rollback is limited to Btrfs layouts with a normal top-level `@` root and Snapper-style root snapshots; running-host rollback additionally requires a Snapper root configuration, no `subvolid=` pin, no separate `/boot` and no other nested `@` child subvolumes, and only migrates a nested `@/.snapshots` child subvolume; unsupported layouts are refused rather than guessed;
- unsupported package-manager operations on Arch (such as standalone APT/dpkg stages), unknown boot layouts, and package transactions whose preflight reports repository errors, removals or unresolved dependencies; these remain hard-gated.

## Using Boot Bitch

- **Systems** — shows the protected running host separately from ranked repair drives; select a physical target (and unlock LUKS when needed) before any repair work.
- **Diagnostics** — read-only reports for the Running Host or the selected repair drive, including boot state, firmware entries and file systems.
- **Repair** — the configurable Full Repair plan and individual tools (file system repair, package configuration, broken dependencies, metadata refresh, adaptive package upgrade, DKMS, display manager, initramfs, EFI/UKI and GRUB), or Host Maintenance on the active system.
- **Snapshots** — read-only Btrfs/Snapper inventory and the guarded rollback workflow, including running-host rollback with a persistent **Reboot required** reminder.
- **Chroot Shell** — one confirmed root command at a time inside the selected repair system. When a command asks a question (apt/dnf/dpkg prompts), it appears in a popup and your answer is sent back to the command; cancel stops it. Non-interactive flags such as `-y`/`--noconfirm` remain recommended for unattended runs.
- **File Copy** — guarded Host → Repair and Repair → Host transfers with preview and verification; repair-to-host copies remove setuid/setgid bits and file capabilities and refuse shared scratch (sticky or world-writable) destination directories.
- **Logs / Settings** — per-session logs with section filters, and the repair plan, safety rules and capability report.

## Screenshots

These anonymized screenshots show Boot Bitch running in a disposable recovery VM. The target is a separate virtual disk; personal host paths and physical-device identifiers have been removed.

### Systems

![Systems: choose the protected running host or a detected repair target](docs/screenshots/01-systems-target-selection.png)

### Diagnostics

![Read-only diagnostics report for the selected scope](docs/screenshots/02-diagnostics-report.png)

### Repair

![Full Repair plan with per-stage gating from cached diagnostics](docs/screenshots/03-repair-confirmation.png)

### Snapshots

![Btrfs snapshot inventory with a read-only scan](docs/screenshots/04-snapshots.png)

### Chroot Shell

![Guarded Chroot Shell for the selected repair system](docs/screenshots/05-chroot-shell.png)

### File Copy

![Host to Repair file copy with a guarded destination path](docs/screenshots/06-file-copy.png)

### Logs

![Application log with session history and scoped diagnostics](docs/screenshots/07-application-log.png)

### Settings

![Settings for Full Repair stages, diagnostics refresh and safety controls](docs/screenshots/08-settings.png)

## 0.2.26 refinements

0.2.26 headlines the fully functional Debian Etch (Qt 3 / KDE 3.5) legacy
edition, interactive shell-prompt answering and a large security-hardening
pass, validated on Alpine, Fedora, Debian, Arch, TUXEDO OS and the Etch
rigs. Highlights:

- Debian Etch legacy edition: the same privileged helper with LUKS unlock,
  split-LVM targets, guarded menu.lst regeneration, file copy, host
  maintenance and Make Default, proven on real Etch systems.
- Interactive shell answers through a GUI popup, with clean transcripts.
- Security hardening: contained target mounts, a filtered private /dev for
  repair chroots, prompt-spoofing prevention, bounded protocol records,
  per-disk session locks, verified helper and elevation paths and
  secret-file config writes.
- Evidence-gated, simulation-first repairs for apt/dpkg, pacman, apk and
  dnf5, single-build dracut, GRUB/GRUB2/extlinux regeneration with entry
  preservation, TUXEDO UKI rebuilds, EFI entry management and Make Default
  on every boot chain.

See the [0.2.26 release notes](docs/release-notes-0.2.26.md).
## 0.2.25 refinements

0.2.25 ships the native RPM and APK release artifacts, the Alpine, Arch and
Fedora repair backends, running-host Btrfs snapshot rollback and the
read-only-first file system repair engine. Highlights:

- Native RPM and APK release artifacts: `scripts/package-rpm.sh` builds the
  CPack RPM with family-specific dependencies and the POSIX `/bin/sh`
  scriptlet, `scripts/package-alpine.sh` builds the signed APK with abuild,
  and `scripts/test-rpm.sh`/`test-apk.sh` validate both without installing
  them.
- Alpine, Arch and Fedora repair backends: guarded apk, pacman and dnf5
  package transactions with mkinitfs, mkinitcpio, dracut, GRUB/extlinux and
  display-manager stages join the Debian/APT backends; Arch host-maintenance
  package repair and upgrade stages are included, while standalone APT/dpkg
  stages remain unavailable on Arch.
- Running-host Btrfs snapshot rollback: the Snapshots tab lists Snapper root
  snapshots and Boot Bitch undo points and stages the name-preserving
  rollback (promote a writable copy, reconcile initramfs/UKI/GRUB, restore on
  failure), with a **Reboot required** reminder and never an automatic
  reboot.
- Read-only-first file system repair: diagnostics resolve the root, `/boot`,
  ESP and `/home` filesystems, run the matching read-only check and offer
  per-device repair only after issues are reported, with an extra warning for
  dangerous modes.
- Logging, UI and fixes: a fresh per-launch session log with retention and
  `@section` search, one authorization request per scope, a guarded Host
  Shell and responsive layouts; plus fixes from user testing (PackageKit, apt
  metadata retry, GRUB EFI firmware-ID handling) and package-manager-aware
  dependency setup.

## 0.2.24 refinements

0.2.24 adds the semantic icon atlas, the read-only-first file system repair
engine and the distribution/boot backend profiler. Highlights:

- Semantic icon atlas: the bundled icon set becomes the deterministic UI
  source across desktop themes, with the Qt SVG runtime dependency added for
  Arch and other minimal installations.
- Read-only-first File system repair: the new tool and Full Repair stage
  (ordered first) resolve the root, `/boot`, ESP and `/home` filesystems, run
  the matching read-only check, and offer per-device repair only after issues
  are found and the user confirms.
- Distribution/boot backend profiler: read-only diagnostics pair kernels with
  initramfs images and gate every repair tool and Full Repair stage on the
  scope's read-only evidence, so unavailable actions disable with a reason.
- UI polish and fixes: a busy indicator, one authorization request per scope,
  section-scoped diagnostics refresh, a guarded Host Shell, `@section` log
  search and responsive layouts; plus fixes from user testing (GRUB EFI
  firmware-ID handling, PackageKit, apt metadata retry, uninstall cleanup).

## 0.2.23 refinements

Adds guarded running-host maintenance, repair-system folder browsing and
verified EFI destination and label maintenance; see the
[0.2.23 release notes](docs/release-notes-0.2.23.md).

- Maintain one canonical UKI, firmware fallback and WebFAI destination per
  maintained ESP, remove safely identified duplicates, and group retained
  firmware entries in BootOrder by drive and normal use (UKI/vendor loader
  before fallback and WebFAI); update existing descriptions only through a
  validated EFI_LOAD_OPTION rewrite with read-back.
- Record the detected UKI/GRUB boot chain and root-LUKS handoff in boot
  evidence so repairs preserve the expected single unlock path.
- UI/docs: keep the repaired-system folder browser's navigation and
  confirmation rows visible at compact sizes, annotate maintained entries with
  the drive model without duplicating model text, and clarify EFI behavior,
  icon fallbacks and Running Host versus selected-repair support.

## 0.2.22 refinements

- Fix Host → Repair destination browsing to decode directory records
  correctly, and make the repaired-system browser compact by default, freely
  resizable and usable at small window sizes.

## 0.2.21 refinements

- Protect multi-disk EFI state during TUXEDO UKI and conventional GRUB
  rebuilds: isolate vendor delete/create calls, classify every firmware entry,
  restore entries by stable PARTUUID/label/loader identity, preserve relative
  order, reconcile BootOrder/BootNext and refuse vendor-specific paths rather
  than guessing; restore a missing running-host TUXEDO UKI registration and
  annotate only the selected ESP's labels.
- Extend read-only diagnostics and guarded repairs to the explicitly selected
  Running Host, guard GRUB regeneration against dropping existing menu entries
  (reject and roll back), and make Host → Repair destination browsing use
  temporary read-only target mounts so the repair tree cannot be confused with
  the host's folders.

## 0.2.20 refinements

- Bundle the semantic Qt/KDE icon atlas for tabs, diagnostics, repair stages
  and actions, preferring a usable host theme or freedesktop aliases before
  the bundled artwork and native high-contrast fallbacks; add filesystem-aware
  device icons and reject solid theme placeholders so controls remain readable
  on incomplete host themes.

## 0.2.19 refinements

- Add CaptainMorgan12 attribution to the About dialog, AppStream metadata,
  Debian package metadata and copyright files, and add a Qt-aware AppImage
  build workflow with optional linuxdeploy Qt bundling.
- UI polish: keep GNOME/GTK snapshot rows compact and improve dark-theme icon
  contrast, including terminal and disabled-stage icons.

## 0.2.18 refinements

- UI/CI polish: compact GNOME/GTK table rows that grow only when wrapped, dark
  theme glyph tinting with colored status artwork preserved, and
  `actions/checkout@v5` for Node.js 24 runners.

## 0.2.17 refinements

- Packaging/theme docs: optional GNOME/GTK Qt platform-theme, XDG portal and
  GNOME Qt theme suggestions, with environment-driven GNOME theme selection
  documented while KDE keeps its native Plasma style.

## 0.2.16 refinements

- Fix compact snapshot-row sizing on Qt 6.4/Ubuntu CI, install `pkexec` in the
  GitHub Actions test environment so repair-readiness UI checks run with the
  capability present, tolerate a lost executable bit on source-tree helpers by
  invoking readable shell helpers through `/bin/bash`, and revalidate the UI,
  Chroot Shell, display-manager, package, desktop-entry and AppStream checks.

## 0.2.15 refinements

- First official GitHub release: a guarded Qt 6 recovery workflow for
  selecting targets, unlocking LUKS, inspecting boot state and applying
  supported repairs, with authoritative per-target diagnostic caching (stale
  evidence on edits, writes, shell, rollback and copies) and simulation-first
  package, DKMS, initramfs, GRUB, EFI/UKI and boot-stack paths that stop on
  unknown or unsafe failures.
- Evidence-based graphical-login repair (SDDM, GDM3, LightDM, greetd, ly,
  custom systemd managers) and guarded transactional Btrfs/Snapper rollback
  with automatic restoration on critical reconciliation failure.
- Feature set and packaging: Chroot Shell, guarded bidirectional File Copy,
  persistent categorized Logs, asynchronous snapshot loading, target
  configuration editors, protected ESP/firmware handling, KDE/Freedesktop and
  AppStream metadata, the `boot-repair(1)` manual page, and UI,
  helper-contract and staged Debian-package validation tests.

## Pre-release development notes (0.2.14 and earlier)

Versions 0.2.14 and earlier were development iterations before the first
public release. Their detailed history is retained in [CHANGELOG.md](CHANGELOG.md);
they are not presented as public GitHub releases.

## License

Boot Bitch is released under the MIT License. See `LICENSE`.

Boot Bitch comes with ABSOLUTELY NO WARRANTY, to the extent permitted by applicable law.
