# Boot Bitch 0.2.25 — maintenance release

Boot Bitch is a native Qt 6 Linux recovery utility. The application command and Debian package retain the stable technical name `boot-repair` for compatibility; the source repository is `Boot_Bitch`. Version 0.2.25 includes the guarded diagnostics cache, simulation-first repair stack, Btrfs snapshot recovery, audited chroot shell, verified file copy, responsive Qt interface, and cross-desktop packaging prepared for public distribution. Each repair layer performs the strongest practical read-only or trial preflight available, applies only recognized deterministic corrections, and stops on unknown failures instead of guessing. The application was written to address the lack of coherent tooling that lets a non-developer restore a system so it can boot after something goes wrong. Common boot issues can be addressed directly, while the diagnostics, chroot shell and file-copy workflows also support troubleshooting and manual fixes.

This project was written with LLM assistance under human guidance, requirements and manually verified tests.

**Developer:** CaptainMorgan12

## Choose the system to repair

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
- mount target `/boot`, `/boot/efi`, and `/efi` entries when they are safely resolvable on the selected disk;
- profile the selected system or protected running host as Debian/APT, Arch/pacman, another known family, or unknown; report its initramfs generator, bootloader, ESP location, kernel naming layout, and current repair capability in the read-only diagnostics;
- inspect Arch-family systems, including common `mkinitcpio`, `dracut`, GRUB, systemd-boot, and generic UKI layouts; Arch package, initramfs, GRUB and conventional EFI repairs are enabled only after their transaction-specific preflights pass;
- bind the minimum runtime filesystems needed for a chroot repair;
- on Debian/Ubuntu/TUXEDO-family targets, run guarded package repair, DKMS rebuild, recovery of the detected graphical login manager, initramfs rebuild, distribution-aware EFI/UKI recovery, boot-stack reconciliation, and `update-grub`; Arch targets use the corresponding guarded pacman, mkinitcpio, conventional EFI and GRUB paths when their transaction preflights pass;
- in Host Maintenance scope, run the supported package, DKMS, graphical-login, initramfs, EFI, GRUB and boot-stack stages natively on the active Debian/Ubuntu or Arch system, with package-lock checks, writable-mount checks, and a private read-only EFI-variable namespace for indirect hooks;
- inspect the running host with the full read-only diagnostic set, including EFI/UKI files, embedded kernel/cmdline, PARTUUID-owned firmware entries, BootOrder, GRUB handoff, and journal evidence;
- require an explicit confirmation in the GUI before every modifying operation;
- request administrator authorization once per Boot Bitch window and retain only the narrow whitelisted helper session until explicitly locked or the app exits;
- show modifying operations and Run All diagnostics in a modal privileged-output window, while individual diagnostics write directly into the persistent Results pane; persist the session log to `/var/log/boot-repair-session.log` only for requests that deliberately crossed the read-write boundary and still have a writable target log mount;
- copy files Host → Repair or Repair → Host with guarded `rsync -aHAX --numeric-ids`, smart ownership validation, no `--delete`, and post-copy verification;
- keep Repair → Host target mounts read-only and promote Host → Repair read-write only after the same independent target safety checks;
- perform a guarded transactional Btrfs `@` rollback after a read-only preflight, while keeping the source snapshot unchanged and retaining the previous root under a timestamped `@rollback-before-*` name;
- in Host Maintenance, inspect Snapper root snapshots and Boot Bitch `@rollback-before-*` undo points and stage a running-host rollback with the same name-preserving transaction: the running `@` is preserved first, the selected snapshot is promoted to `@`, a nested `@/.snapshots` child subvolume is migrated, the boot stack is reconciled in a scratch chroot, and a persistent **Reboot required** reminder requires a separate `Reboot Now` confirmation before the promoted root is used;
- build a Release executable and Debian package with the privileged helper included.

Firmware destination roles are reported explicitly in EFI diagnostics. `uki`
is the vendor TUXEDO UKI path (`\\EFI\\BOOT\\TUX.EFI`), `fallback` is the
ordinary `BOOTX64.EFI` path, and `wfai` is the WebFAI/iPXE recovery path when
that file exists. A conventional `shimx64.efi` or another distribution loader
is reported as `vendor-loader`; a firmware-generated device-path-only UEFI
fallback is reported separately so it can be recognized as a duplicate of an
explicit fallback path without guessing at vendor-specific device data.

Boot evidence also records the expected chain for the selected distribution.
TUXEDO Debian-base systems that provide `create_boot_uki_base.sh` use firmware
→ TUXEDO UKI → initramfs → LUKS unlock → root → login; TUXEDO Ubuntu and other
systems without that UKI builder use firmware → GRUB menu → initramfs → LUKS
unlock → root → login. The report compares the root LUKS identity and marks
whether an interactive unlock is expected once, so an EFI repair does not add
another decryption prompt.

Still intentionally constrained in 0.2.25:

- automatic/implicit EFI-loader reinstall: EFI repair remains an explicit action or opt-in Full Repair stage;
- transactional rollback is limited to Btrfs layouts with a normal top-level `@` root and Snapper-style root snapshots; running-host rollback additionally requires a Snapper root configuration, no `subvolid=` pin, no separate `/boot` and no other nested `@` child subvolumes, and only migrates a nested `@/.snapshots` child subvolume; unsupported layouts are refused rather than guessed;
- unsupported package-manager operations on Arch (such as standalone APT/dpkg stages), unknown boot layouts, and package transactions whose preflight reports repository errors, removals or unresolved dependencies; these remain hard-gated;




## Screenshots

These anonymized screenshots show Boot Bitch running in a disposable recovery VM. The target is a separate virtual disk; personal host paths and physical-device identifiers have been removed.

### Diagnostics

![Diagnostics report](docs/screenshots/02-diagnostics-report.png)

### Repair

![Repair plan and diagnostic freshness gate](docs/screenshots/03-repair-confirmation.png)

### Snapshots

![Snapshot handling on an ext4 target](docs/screenshots/04-snapshots.png)

### Chroot Shell

![Chroot shell](docs/screenshots/05-chroot-shell.png)

### Logs

![Application log](docs/screenshots/07-application-log.png)

### Settings

![Settings](docs/screenshots/08-settings.png)

## 0.2.25 refinements

Adds native RPM and APK release artifacts and the Alpine, Arch and Fedora
repair backends; see the [0.2.25 release notes](docs/release-notes-0.2.25.md).

- Add native RPM and APK release artifacts: `scripts/package-rpm.sh` builds
  the CPack RPM with family-specific dependency names and the POSIX `/bin/sh`
  `scripts/rpm-postinst.sh` scriptlet, and `scripts/package-alpine.sh` builds
  the signed `boot-bitch-<version>-r0.apk` with abuild and runs the project
  tests in its `check()` phase; `scripts/test-rpm.sh` and
  `scripts/test-apk.sh` validate both artifacts (metadata, dependencies,
  signature, installed file set) without installing them.
- Add a Fedora/RPM-family repair backend from the same read-only probe
  evidence as the other families: guarded dnf5 package transactions (`rpm -Va`
  + `dnf reinstall` fix-broken, a `dnf makecache` metadata stage, one
  simulated `dnf upgrade`), dracut initramfs rebuilds with kernel/image
  pairing and `lsinitrd` verification, GRUB2 configuration regeneration
  (`grub2-mkconfig --no-grubenv-update` with BLS and menuentry preservation)
  plus a guarded GRUB2 boot-code reinstall on BIOS, boot-stack reconciliation
  over dracut + GRUB2, and the systemd GDM display path
  (`/etc/gdm/custom.conf`). Journald logging and the 13 capability keys are
  unchanged; every Fedora label and gate derives from probe evidence, never
  from the distribution ID.
- Add an Alpine/apk repair backend alongside Debian/APT and Arch/pacman:
  read-only profile and capability evidence, simulation-first `apk fix` and
  `apk upgrade` transactions, and targeted missing-package-file repair driven
  by `apk audit --system` plus `apk info --who-owns`. The new `extlinux`
  capability key gates the Alpine extlinux stage; `mkinitfs` initramfs
  rebuilds, config-only `update-extlinux` regeneration with entry preservation
  and rollback, and OpenRC display-manager runlevel restore that never starts
  a graphical session inside the target chroot complete the stage set. Alpine
  Host Maintenance uses the same guards through the running apk/OpenRC, and
  EFI/UKI stages remain Debian/Arch-only.
- Add transaction-specific Arch repair preflights and guarded apply paths for
  full pacman package transactions, mkinitcpio, conventional GRUB and EFI;
  conventional EFI repair restores one verified vendor-loader firmware entry
  when a guarded grub-install leaves only files on the ESP, and individual
  mirror retrieval failures are warnings when the sandboxed transaction still
  succeeds while repository, signature, integrity, dependency, removal and
  transaction errors remain fatal. The read-only distribution and boot backend
  profiler detects Debian/APT and Arch/pacman families separately alongside
  the initramfs generator, GRUB/systemd-boot/UKI layout, ESP mount and kernel
  naming evidence, with a backend-profile contract test and portable
  kernel/initramfs pairing diagnostics; every repair tool and Full Repair
  stage is gated on read-only per-tool capability evidence (`Repair tool
  <key>: available|unavailable|<reason>`), unavailable actions disable with
  their reason (for example DKMS on a system without it, or dpkg/standalone
  APT metadata on Arch), helper runtime preflights stay mandatory, and Arch
  probes require pacman configuration/database, a detected display manager,
  installed kernel module directories and a resolvable ESP where those stages
  need them.
- Add running-host Btrfs snapshot rollback to Host Maintenance: the Snapshots
  tab lists Snapper root snapshots and Boot Bitch `@rollback-before-*` undo
  points through the new `host-snapshots` helper command and stages the
  name-preserving transaction (preserve the running `@`, promote a writable
  copy, migrate a nested `@/.snapshots` child subvolume, reconcile
  initramfs/UKI/GRUB in a scratch chroot, automatically restore on failure)
  while reporting `Host snapshot rollback` capability evidence without adding
  a 14th `Repair tool` key. A successful rollback persists a **Reboot
  required** reminder, cleared by kernel boot-id reconciliation, with `Reboot
  Now` offered only after a second explicit confirmation through the new
  `host-reboot` command; rollback never reboots automatically and is limited
  to Snapper top-level `@` roots with no `subvolid=` pin, no separate `/boot`
  and no other nested `@` child subvolumes.
- Add a read-only-first file system repair engine: diagnostics resolve the
  root, `/boot`, ESP and `/home` filesystems for the selected scope (repair
  target or running host) with their UUIDs and run the matching read-only
  check (`e2fsck -n`, `xfs_repair -n`, `btrfs check --readonly`, `fsck.fat
  -n`, `fsck.exfat -n`, `ntfsfix -n`, `jfs_fsck -n`, `reiserfsck --check`,
  `zpool status`) without writing. Only when a check reports issues does the
  tool offer repair: per-device modes are limited to what is valid for the
  mounted state (offline tools refuse mounted filesystems while btrfs and
  zpool scrub stay online), every repair requires explicit confirmation,
  dangerous modes such as `btrfs check --repair` carry an extra warning, and
  each helper invocation and its per-tool exit classification is logged. The
  new `filesystem`-gated "File system repair" tool/stage runs first in the
  Full Repair plan.
- Logging, diagnostics and UI/UX: each launch starts a fresh session log with
  scope markers, visible timestamps, prior-session viewing, clear/add-note
  actions and automatic retention (20 files or 10 MB); stale read-only
  diagnostics regenerate automatically after invalidating operations once a
  privileged session exists (affected sections only, or the union once at the
  end of Full Repair, and repairs that changed nothing keep the cache valid),
  Run All stays asynchronous with the capability block emitted once and
  repeated sections updated in place, journal evidence is filtered to system
  boot components with near-identical lines collapsed, and log rendering is
  coalesced and incremental with colored ✓/✗/▪ result summaries and
  action-accurate verbs. The UI adds a global busy indicator; one
  authorization request per scope with a visible Authorize action after
  cancellation (the separate LUKS prompt is unchanged); a guarded running-host
  Host Shell through the new `host-shell` command with firmware write
  isolation, also fixing the Host Maintenance shell crash; `@section` log
  search with section filters and a wider Individual Repair Tools pane;
  deterministic responsive repair-target rows (`DeviceStatusDelegate`); and
  narrow-window Systems, Logs, File Copy and Repair layouts with one-fifth
  splitter minimums, scroll edge shadows and the bundled monochrome
  kernel/initramfs/GRUB/distribution icons.
- Fixes and tooling: an idle PackageKit daemon no longer blocks host package
  repairs; vendor apt release-metadata changes warn and retry once with
  `Acquire::AllowReleaseInfoChange` while other apt errors and all package
  transactions stay strict; real Polkit/pkexec authorization failures are
  surfaced and logged; the initramfs generator is detected from installed
  packages with the TUXEDO UKI builder required for that EFI layout; the
  conventional GRUB EFI repair no longer lets the efibootmgr same-label
  warning pollute the captured firmware ID and fails closed on a missing or
  invalid mapping; a failed Full Repair plan marks only the failing stage and
  reports stages that never ran as "not run"; internal cleanups change no
  behavior. Dependency setup and installation are package-manager aware
  (APT/dpkg, pacman, DNF/RPM and zypper/RPM) with native Arch, RPM and
  package-manager-neutral TGZ workflows while `.deb` generation stays
  Debian-family; AppStream ships six remote screenshots, the current release
  entry, a `<pkgname>` association and no deprecated `developer_name`, with
  hicolor icon-cache refreshes, the Discover stock-icon workaround and the
  `/usr/local` icon-cache cleanup on uninstall; file system check tools are
  declared package dependencies; `prepare-release.sh`, `verify-release.sh` and
  `local-refresh.sh` cover the release documentation and the staging sync; and
  the Alpine/Fedora VM validation provisions abuild through the QEMU guest
  agent, builds the signed package via `guest-exec` and validates the Fedora
  44 RPM with `scripts/test-rpm.sh` without installing it.

## 0.2.24 refinements

Adds the semantic icon atlas, the read-only-first file system repair engine,
Arch host-maintenance package repair and the distribution/boot backend
profiler; see the [0.2.24 release notes](docs/release-notes-0.2.24.md).

- Bundle the semantic icon atlas as the deterministic UI source across desktop
  themes and add the Qt SVG runtime dependency required on Arch and other
  minimal installations.
- Add the read-only-first File system repair tool and Full Repair stage
  (ordered first): resolve the root, `/boot`, ESP and `/home` filesystems with
  their UUIDs, run the matching read-only check, and offer per-device repair
  modes only after issues are found and the user confirms; offline tools
  refuse mounted filesystems while btrfs/zpool scrub stay online.
- Add Arch host-maintenance package repair and upgrade stages through
  transaction-specific preflights and guarded full pacman transactions;
  standalone APT/dpkg stages remain unavailable on Arch.
- Add the read-only distribution/boot backend profiler, backend-profile
  contract test and kernel/initramfs pairing diagnostics; gate every repair
  tool and Full Repair stage on the scope's read-only diagnostics (DKMS, APT
  metadata and Arch prerequisites disable with a reason while helper
  preflights remain).
- UI/UX polish: busy indicator; one authorization request per scope; journal
  evidence filtered to boot components; section-scoped diagnostics refresh
  with coalesced incremental rendering; narrow-window Logs/File Copy; guarded
  Host Shell with the host-maintenance crash fix; single Polkit prompt on Host
  Maintenance entry; `@section` log search and section filters; responsive
  Repair/Systems layouts; scroll edge shadows; one-fifth splitter minimums;
  Systems page and Host Maintenance details fit; compact snapshot rows; and
  software-center metadata with six screenshots, the installed size, package
  association and icon-cache refreshes.
- Fixes: same-labelled GRUB EFI entries no longer pollute the captured
  firmware ID and a failed plan attributes each stage correctly; an idle
  PackageKit daemon no longer blocks host package repairs; the initramfs
  generator is detected from installed packages; apt release-metadata changes
  warn and retry once; no-op pacman transactions report unchanged; repair
  summaries are action-accurate with durable ✓/✗/▪ glyphs; diagnostics
  regeneration is evidence-driven and Run All no longer duplicates log
  sections; stale `/usr/local` source installs are removed on uninstall; the
  Debian/TUXEDO GRUB preflight inspects an isolated `grub-mkconfig` output;
  the protected-host shield/check returns as a bundled green status icon;
  file-system check tools are declared package dependencies (common tools
  hard, the rest recommended/optional); code-review cleanups with no behavior
  change.

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

## Development summary

The 0.1–0.2 development series established the recovery foundation, target safety boundary, diagnostics cache, simulation-first repair stack, encrypted-volume handling, Btrfs rollback, graphical-login recovery, Chroot Shell, verified File Copy, responsive interface and Debian packaging. The full version-by-version record is in [CHANGELOG.md](CHANGELOG.md).

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
├── docs/                   documentation index, release notes and screenshots
├── resources/              Qt resources and icons
├── scripts/                helper, build, packaging and validation tools
├── src/                    Qt application and target workflow
└── tests/                  UI and contract tests
```

Generated build output belongs in `build/` and is ignored by Git.

## Design goals

- Native Qt 6 Widgets application with standard resizable window controls.
- Follow the active Qt/KDE/desktop theme for widget styling, palettes, spacing,
  dialogs and accessibility without a hard-coded light/dark stylesheet. The
  application does not require a system icon theme: semantic icons use a
  usable host theme when available, then the bundled atlas and native
  high-contrast fallbacks.
- Keep the source tree and dependency set small.
- KDE Frameworks 6 KAuth remains optional; guarded builds use a separate root helper launched through Polkit/pkexec.
- Privileged work is isolated from the normal GUI process and exposes only whitelisted operations.
- Missing optional runtime tools disable only the related feature.
- Package changes in either Host Maintenance or the selected repair system require explicit repair confirmation.
- Storage discovery must not assume NVMe, internal disks, encryption or one filesystem layout.
- Current-system identity checks are mandatory. Ordinary repair-target actions refuse the running host; Host Maintenance must be selected explicitly.

### GNOME and KDE/Qt desktop integration

Boot Bitch is a Qt 6 Widgets application and deliberately does not force a
global stylesheet or the Fusion style. It follows the desktop's font, palette,
spacing, dialogs and accessibility settings, while resolving semantic icons
from the active theme first and the bundled atlas when a theme is incomplete.
KDE uses its normal Qt platform theme. On GNOME, install the optional Qt
platform-theme and portal plugins for GTK-like controls and native file dialogs:

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

To audit available diagnostic and tab icons on the current desktop theme, run
`./scripts/check-icon-theme.sh`. It reports missing names and suggests
equivalent freedesktop icons; missing host-theme names do not disable the
application because the corresponding semantic artwork is bundled.

## Build dependencies

Required:

- C++17 compiler (GCC or Clang)
- CMake 3.20+
- Ninja
- Qt 6 Core/Gui/Widgets development files

Runtime/packaging support used by guarded repair builds:

- Polkit / `pkexec`
- util-linux (`lsblk`, `findmnt`, `blkid`, mount tools)
- systemd `systemctl` for graphical-target and detected display-manager repair
- `efibootmgr` for guarded UEFI entry/order inspection when firmware variables are available
- binutils `objcopy` for TUXEDO UKI verification
- Python 3 for guarded EFI label updates and their regression test
- Debian packaging tools when generating `.deb` files
- RPM packaging tools (`rpmbuild`) when generating Fedora/openSUSE packages
- Arch packaging tools (`makepkg`) when generating an Arch package
- CPack's TGZ generator for a package-manager-neutral archive
- KDE Frameworks 6 KAuth development files remain optional

### Package-manager aware dependency setup

The setup script detects the build host from `/etc/os-release` and uses the
native package manager. It supports Debian/Ubuntu/TUXEDO, Arch-family,
Fedora/RHEL and openSUSE/SUSE hosts. It performs package installation only
when you invoke it explicitly:

```bash
./scripts/setup-dev-deps.sh
```

On Arch-family hosts it runs `pacman -Syu --needed`, keeping the package
database and installed libraries synchronized. Fedora/RHEL uses DNF;
openSUSE/SUSE uses zypper. Optional desktop-theme plugins remain
distribution-specific, so the setup script installs the common build/runtime
set and the environment checker reports optional additions.

The Debian/Ubuntu/TUXEDO equivalent direct package command (KF6Auth is
optional and the setup script adds it automatically when the distribution
provides it) is:

```bash
sudo apt update && sudo apt install --no-install-recommends -y \
    build-essential cmake ninja-build pkg-config \
    qt6-base-dev qt6-base-dev-tools extra-cmake-modules \
    dpkg-dev desktop-file-utils lintian \
    pkexec util-linux mount rsync cryptsetup btrfs-progs systemd \
    efibootmgr binutils python3 lvm2 mdadm
```

For a manual Arch setup, the corresponding core command is:

```bash
sudo pacman -Syu --needed \
    base-devel cmake ninja pkgconf qt6-base qt6-tools extra-cmake-modules \
    desktop-file-utils appstream polkit util-linux rsync cryptsetup \
    btrfs-progs systemd efibootmgr binutils python hicolor-icon-theme \
    lvm2 mdadm
```

To inspect the environment without installing anything:

```bash
./scripts/check-dev-env.sh
```

## One-command Release build + packages

Run:

```bash
./scripts/build.sh
```

This performs a clean Release build and validates an isolated staged install. On Debian-family build hosts with the normal packaging tools available, it also creates a `.deb`; when `linuxdeploy`/`appimagetool` are available on `PATH` or in `Development/tools/`, the same run creates the versioned AppImage. Both artifacts are written under `build-release/`. If the AppImage tooling is unavailable, the script prints a warning and still finishes with the application build and `.deb`. Nothing is installed.

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
cd /path/to/Boot_Repair
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

`install.sh` detects the host package family. It installs a generated `.deb`
through APT/dpkg, a generated RPM through DNF/zypper, or a generated Arch
package through pacman. It does nothing destructive until you type `INSTALL`
exactly. On any supported Linux desktop, `./scripts/install.sh --source`
installs the staged CMake build under `/usr/local` (or `PREFIX=/opt/boot-repair`
for another absolute prefix) after the same confirmation.

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

Alternative source-install prefix:

```bash
PREFIX=/opt/boot-repair ./scripts/install.sh --source
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

## Build native packages on Arch and RPM systems

Arch-family hosts can build a native package from the current working tree:

```bash
./scripts/package-arch.sh
sudo pacman -U Development/build-arch-package/boot-bitch-*.pkg.tar.*
```

The recipe defaults to `makepkg --force`; `makepkg` checks dependencies but
does not install them unless `--syncdeps` is explicitly requested. Set
`MAKEPKG_ARGS='--force --syncdeps'` when dependency installation is desired.
The package depends on Arch's native
names for Qt, Polkit, storage tools, EFI inspection and Python.

Fedora/RHEL and openSUSE/SUSE hosts can use the RPM workflow:

```bash
./scripts/package-rpm.sh
sudo dnf install Development/build-rpm/boot-bitch-*.rpm
# or: sudo zypper install Development/build-rpm/boot-bitch-*.rpm
```

When a native package tool is unavailable, CPack's portable archive keeps the
same `/usr` install tree available:

```bash
./scripts/package-tarball.sh
```

All three formats ship the same executable, bundled icons, desktop metadata,
Python EFI label helper and privileged shell helper. Repair-time commands
remain host-provided (`pkexec`, `mount`, `cryptsetup`, `btrfs`, `efibootmgr`,
and the selected distribution's boot tools), so installing the GUI does not
imply that a non-Debian repair backend is enabled.

An AppImage is also available for portable testing and systems without a local
package build. It bundles the GUI and guarded helper, while privileged actions
still use the host's `pkexec`/Polkit and system utilities. Install the normal
runtime tools listed above before attempting a repair. `./scripts/build.sh`
builds the AppImage together with the `.deb` when the AppImage tools are
available; to build only the AppImage, run `./scripts/build-appimage.sh`. The
resulting artifact is written to `build-release/` and is excluded from Git
source uploads.

The AppImage uses the same privilege boundary: the helper must be authorized
and acts on the selected repair system or explicitly selected Host Maintenance
scope. For regular installation and desktop integration, use the native
package for the host family, or the source-install flow when no native package
tool is available.

## Build an AppImage

Install [`linuxdeploy`](https://github.com/linuxdeploy/linuxdeploy/releases),
its `linuxdeploy-plugin-qt`, and
[`appimagetool`](https://github.com/AppImage/appimagetool/releases), make all
three available on `PATH`, then run:

```bash
./scripts/build-appimage.sh
```

The standard release build (`./scripts/build.sh`) runs this step automatically
when the tools are available and writes the same versioned artifact to
`build-release/`; run `build-appimage.sh` directly only to build the AppImage
by itself.

To use tools installed at another path:

```bash
LINUXDEPLOY="$HOME/bin/linuxdeploy" \
APPIMAGETOOL="$HOME/bin/appimagetool" ./scripts/build-appimage.sh
```

The script creates a clean Release build, stages the complete CMake install
under an AppDir, validates the executable and privileged helper, lets
`linuxdeploy` bundle Qt/shared-library dependencies, and invokes
`appimagetool`. It does not install anything on the host. Run the artifact with
`chmod +x build-release/boot-repair_0.2.24_x86_64.AppImage` followed by
`./build-release/boot-repair_0.2.24_x86_64.AppImage`. If only `appimagetool`
is available, the script creates a diagnostic AppImage and warns that it uses
the host's Qt libraries; do not publish that fallback as a portable release.

## Uninstall

The uninstall helper removes a detected native package through its package
manager. For a source install it uses the manifest recorded by
`install.sh --source`, lists every installed file, and requires an explicit
`UNINSTALL` confirmation. A manually reviewed CMake manifest can be supplied
with `INSTALL_MANIFEST=/absolute/path`:

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

Settings define the Full Repair plan. Enabled stages are shown in execution order. Every configurable stage also appears under Individual repair tools: package configuration, broken-dependency repair, package metadata refresh, adaptive package upgrade, DKMS, detected graphical login manager, initramfs, EFI/UKI, and GRUB. Select a repair drive for offline repair, or choose **Host Maintenance** on the protected Running Host card to run the same supported stages natively on the active Debian/Ubuntu-family system. Host maintenance repeats disk/root/boot-mount identity checks, checks package locks, and isolates firmware variables from indirect hooks. Validate environment remains an automatic preflight, and Boot stack reconciliation remains a manual recovery tool.

EFI maintenance validates entry ownership by ESP PARTUUID, removes only identified duplicate destinations on the maintained ESP, and groups retained entries by drive and normal boot use. Labels retain their distribution/vendor wording and receive the disk model once (for example `tuxedo Example NVMe 4000GB`); existing model text is not duplicated. If an `iPXE.efi` loader exists on that ESP but has no matching firmware entry, TUXEDO systems receive `WFAI <model>` and other distributions receive `iPXE <model>`. The helper retains unrelated firmware entries and verifies the resulting labels and BootOrder.

### Snapshots

Shows the guarded Btrfs rollback workflow. Loading and inspection remain read-only. **Roll Back to Selected** performs a separate read-only plan first, then creates a writable copy of the chosen root snapshot, preserves the prior `@`, promotes the copy to the normal writable `@`, sets it as the Btrfs default, reconciles initramfs/TUXEDO UKI/GRUB, and automatically restores the preserved root if critical post-switch validation fails.

In Host Maintenance the same tab serves the running host. Snapper root snapshots and Boot Bitch `@rollback-before-*` undo points are listed read-only; the running snapshot is marked and never offered. Rollback requires available `Host snapshot rollback` capability evidence, a read-only plan, a consequences dialog and a typed `ROLLBACK` confirmation. The running system keeps serving the old root until a reboot, so a persistent **Reboot required** banner with **Reboot Now** and **Later** actions appears after a successful rollback; **Reboot Now** always asks for a second, separate confirmation and is never automatic. The reminder is persisted and cleared only when the kernel boot id proves a real reboot happened. Failed rollbacks keep the banner state, never reboot, and surface the helper's automatic-recovery evidence.

### File Copy

A direction selector runs either Host → Repair or Repair → Host transfers. Host paths use native file dialogs; repair-side paths are absolute paths inside the selected repair system. **Browse Target Folders** reads the selected repair tree through a temporary read-only helper mount for each directory view, so it never displays the host folders as a substitute and never leaves the target mounted. A destination may still be entered as a new absolute path when the folder does not exist yet. Preview is a guarded rsync dry-run. Copy execution validates containment and ownership, uses `rsync -aHAX --numeric-ids` without `--delete`, and verifies the result with a checksum/metadata dry-run plus SHA-256 for regular files.

### Logs

Timestamped application events wrap responsively. Use **View → Wrap Log Lines** to switch to exact unwrapped formatting.

### Settings

Contains live device-display preferences, the Full Repair plan, mandatory safety rules, and host capability/dependency reporting.

## Runtime capability model

Boot Bitch checks capabilities independently. Examples include `lsblk`, `blkid`, `findmnt`, `cryptsetup`, `btrfs`, `rsync`, `chroot`, `efibootmgr`, `grub-install`, `update-grub`/`grub-mkconfig`, `update-initramfs`/`mkinitcpio`/`dracut`, `bootctl`, `dkms`, optional LVM tools, and `mdadm`. The backend profile records which package manager and boot tools the inspected system actually provides, so an Arch or RPM system is not mistaken for a Debian target.

Missing optional tools disable the related action. Modifying repair execution
supports supported Debian/Ubuntu-family and Arch-family systems in either
scope when backend and transaction-specific preflights pass. Arch uses a full
sandboxed pacman transaction and never runs standalone APT/dpkg stages.
Missing host tools are never installed silently.

## License

Boot Bitch is released under the MIT License. See `LICENSE`.
