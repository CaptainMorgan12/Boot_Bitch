# Changelog

## Unreleased

- Consolidate the MainWindow regression suite from 161 to 148 cases by merging
  overlapping fixture setups (no coverage removed) and add a test seam for the
  automatic evidence-refresh delay, cutting the UI tier from ~49 s to ~31 s.
- Add running-host Btrfs snapshot rollback to Host Maintenance: the Snapshots
  tab lists Snapper root snapshots and Boot Bitch `@rollback-before-*` undo
  points through a new `host-snapshots` helper command, stages the selected
  snapshot with the proven name-preserving transaction (preserve the running
  `@`, promote a writable copy, migrate a nested `@/.snapshots` child
  subvolume, reconcile initramfs/UKI/GRUB in a scratch chroot, auto-restore on
  failure) and reports `Host snapshot rollback` capability evidence without
  adding a 14th `Repair tool` key. A successful rollback persists a
  **Reboot required** reminder that is cleared by kernel boot-id
  reconciliation and offers `Reboot Now` only after a second explicit
  confirmation through the new `host-reboot` command; rollback never reboots
  automatically. Contract tests cover the preflight/refusal matrix, the
  transaction, nested migration, undo targets and reboot confirmation, with
  offscreen UI coverage for the flow, banner and gating.
- Make the Available repair targets table's wrap and elision deterministic
  across styles: a new `DeviceStatusDelegate` lays the Status text out against
  the live column width (at most two lines, with the remainder elided on the
  second) and a dynamic column policy sizes the other columns to their content
  while only ever shrinking them, so the Status column keeps the remaining
  viewport width and wrapped row heights and painting are identical on Breeze,
  Fusion and Adwaita. The policy re-applies on viewport resize, restored
  header state and every refresh; UI tests cover the two-line/elided contract,
  row-height growth, column shrinkage and stability across sort and refresh.

## 0.2.25 — 2026-09-19

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
  Debian-family; AppStream ships eight remote screenshots, the current release
  entry, a `<pkgname>` association and no deprecated `developer_name`, with
  hicolor icon-cache refreshes, the Discover stock-icon workaround and the
  `/usr/local` icon-cache cleanup on uninstall; file system check tools are
  declared package dependencies; and the native Arch, Alpine and Fedora
  packages are validated with `scripts/test-rpm.sh`, `scripts/test-apk.sh` and
  `scripts/test-deb.sh` without installing them.

## 0.2.24 — 2026-09-16

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

## 0.2.23 — 2026-09-15

Adds guarded running-host maintenance, repair-system folder browsing and
verified EFI destination and label maintenance.

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

## 0.2.22 — 2026-09-14

- Fix Host → Repair destination browsing to decode directory records
  correctly, and make the repaired-system browser compact by default, freely
  resizable and usable at small window sizes.

## 0.2.21 — 2026-09-14

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

## 0.2.20 — 2026-09-13

- Bundle the semantic Qt/KDE icon atlas for tabs, diagnostics, repair stages
  and actions, preferring a usable host theme or freedesktop aliases before
  the bundled artwork and native high-contrast fallbacks; add filesystem-aware
  device icons and reject solid theme placeholders so controls remain readable
  on incomplete host themes.

## 0.2.19 — 2026-09-13

- Add CaptainMorgan12 attribution to the About dialog, AppStream metadata,
  Debian package metadata and copyright files, and add a Qt-aware AppImage
  build workflow with optional linuxdeploy Qt bundling.
- UI polish: keep GNOME/GTK snapshot rows compact and improve dark-theme icon
  contrast, including terminal and disabled-stage icons.

## 0.2.18 — 2026-09-13

- UI/CI polish: compact GNOME/GTK table rows that grow only when wrapped, dark
  theme glyph tinting with colored status artwork preserved, and
  `actions/checkout@v5` for Node.js 24 runners.

## 0.2.17 — 2026-09-13

- Packaging/theme docs: optional GNOME/GTK Qt platform-theme, XDG portal and
  GNOME Qt theme suggestions, with environment-driven GNOME theme selection
  documented while KDE keeps its native Plasma style.

## 0.2.16 — 2026-09-13

- Fix compact snapshot-row sizing on Qt 6.4/Ubuntu CI, install `pkexec` in the
  GitHub Actions test environment so repair-readiness UI checks run with the
  capability present, tolerate a lost executable bit on source-tree helpers by
  invoking readable shell helpers through `/bin/bash`, and revalidate the UI,
  Chroot Shell, display-manager, package, desktop-entry and AppStream checks.

## 0.2.15 — 2026-09-13

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

## Pre-release changelog

Versions 0.2.14 and earlier were development iterations before the first
public release. The entries below preserve the complete historical notes in
one section; they are not public GitHub releases.

### 0.2.14

- Added persistent categorized repair transcripts, section separators and stale diagnostic highlighting in Logs.
- Loaded Btrfs snapshots asynchronously and kept unsupported filesystems read-only with a clear explanation.
- Added compact responsive layouts, tab navigation controls, stable scrollbar gutters and adjustable Repair splitters.
- Added temporary recovery-host resolver support for chroot networking and improved APT refresh/error handling.
- These refinements were validated during development and are included in the 0.2.15 public release.

### 0.2.13

- Extend the adaptive repair policy beyond APT: DKMS, graphical login/SDDM, initramfs, EFI/UKI, GRUB and Boot stack reconciliation now run a simulation or read-only/trial preflight before their modifying command.
- DKMS inspects registered modules and installed-kernel header/build readiness first. When a matching `linux-headers-<kernel>` package is available, Boot Bitch simulates the package correction under the same protected-removal policy, installs it only if safe, then runs `dkms autoinstall` and verifies status. A recognized missing-header failure is corrected/retried once.
- SDDM recovery preflights `sddm`, `graphical.target`, the service unit and (on TUXEDO) the desktop meta-package. Missing/reinstall package corrections are APT-simulated and safety-checked before execution; the graphical session is still never started inside the repair chroot.
- Initramfs repair trial-builds every installed kernel with `mkinitramfs` into temporary `/run/boot-repair` session output before touching `/boot`. It validates crypttab/mapper state, creates a missing initrd instead of blindly updating it, verifies the resulting archive, and can restore a recognized stale recovery-mapper compatibility alias before one retry.
- TUXEDO UKI repair validates the ESP, newest kernel/modules, matching initramfs and free ESP working space before calling the vendor builder. A missing newest-kernel initramfs is repaired first. Conventional GRUB EFI repair preflights its exact target/id/mode and retries once with `--no-nvram` only for recognized firmware-variable/NVRAM failures.
- GRUB repair first generates a complete `grub-mkconfig` candidate into temporary session output, keeps stderr separate from the candidate, syntax-checks with `grub-script-check` when available, and only then runs `update-grub`. A recognized stale mapper/canonical-path failure is corrected and retried once.
- Boot stack reconciliation composes those same adaptive component workflows in order; it no longer hides initramfs/UKI/GRUB failures behind one compound command.
- Unknown DKMS/build, initramfs, EFI/UKI or GRUB failures remain hard stops. Boot Bitch does not auto-delete kernels, snapshots, EFI entries or arbitrary configuration in response to an unrecognized error.

### 0.2.12

- Individual target diagnostics write their output directly to the persistent Results pane; no redundant modal progress/output dialog is shown for a single diagnostic.
- **Run All** retains the consolidated privileged progress/output dialog and still populates the per-diagnostic cache.
- The Individual repair tools list now mirrors every Settings-controlled Full Repair stage one-for-one: dpkg configuration, broken dependencies, APT metadata refresh, adaptive package upgrade, DKMS, SDDM, initramfs, EFI/UKI, and GRUB.
- The Full Repair column explicitly reports whether each matching stage is enabled or disabled in Settings. Validate environment remains an automatic safety preflight; Boot stack reconciliation remains a manual recovery tool.
- Individual package-maintenance tools now invoke exactly the same helper stages used by Full Repair, avoiding UI/backend drift.

### 0.2.11

- Add the Boot Bitch boot-on-drive artwork as the real application icon. The source contains a 1024px RGBA master, the Qt executable embeds a resource fallback for build-tree execution, and Debian installs Freedesktop/KDE `hicolor` PNGs at 16, 22, 24, 32, 48, 64, 128, 256, 512 and 1024 pixels.
- Change the desktop entry to `Icon=org.bootrepair.BootRepair` and use the same icon for the Qt window/header instead of the generic `drive-harddisk` application icon. Device rows continue using storage/security theme icons because those communicate device state rather than application identity.
- Replace the hard-coded `apt-get -y upgrade` repair step with an adaptive simulation-first upgrade. Boot Bitch first simulates ordinary `upgrade`; when the target distribution explicitly rejects it or packages remain pending, it evaluates `full-upgrade`, with `dist-upgrade` as the compatible fallback.
- Refuse simulated transactions that remove essential packages, protected TUXEDO/desktop/kernel/boot packages, or more than 20 packages. The selected upgrade mode and complete simulation output are written to the repair session log before the actual command runs.
- Run a non-destructive `dpkg --audit` after a successful upgrade.
- Extend staged-install and `.deb` validation to verify the packaged hicolor icons and desktop icon name.

### 0.2.10

- Keep rollback planning, diagnostics, validation, snapshot inventory/inspection and File Copy preview strictly read-only all the way through helper cleanup; they no longer attempt to append the helper session log to a read-only target `/var/log`.
- Track the exact point a helper request crosses the read-write boundary. Target-side session-log persistence is permitted only after that point and only while `/var/log` is actually mounted read-write.
- Remove GNU awk warnings from snapshot OS-name parsing by using portable quote matching in the rollback inventory/preflight code.
- Retain the hardware-validated 0.2.9 rollback plan and all 0.2.8 unlock/session, diagnostics, snapshot inventory and verified File Copy behavior.

### 0.2.9

- **Transactional Btrfs snapshot rollback is enabled.** Selecting **Roll Back to Selected** first runs a privileged read-only rollback plan. The GUI shows that plan, requires a second warning confirmation, and requires typing `ROLLBACK` exactly before any Btrfs root switch occurs.
- The source snapshot is never renamed, modified or deleted. Boot Bitch creates a **writable Btrfs snapshot copy**, preserves the existing top-level `@` as `@rollback-before-<timestamp>`, promotes the writable candidate to `@`, and sets that new `@` as the Btrfs default subvolume so the next ordinary boot no longer depends on the snapshot menu.
- Separate Btrfs subvolumes listed by the selected snapshot's `fstab` (for example `/home`, `/root`, `/var/log`, swap and snapshot storage) remain outside the root rollback and are remounted from their real target subvolumes for post-switch validation.
- Post-switch reconciliation validates `fstab`/`crypttab`, kernel-to-modules state and dpkg consistency, rebuilds all target initramfs images, rebuilds the TUXEDO UKI through `create_boot_uki_base.sh` when present, verifies the rebuilt UKI root/LUKS/Btrfs-`@` command-line binding, regenerates GRUB, and verifies that the promoted root is actually mounted from `@`.
- The TUXEDO UKI path continues to preserve the target ESP's existing firmware `BootOrder`/`BootNext` relationship and does not remove or reorder entries belonging to other physical disks.
- **Automatic root restoration:** if critical post-switch validation, initramfs, UKI or GRUB reconciliation fails, Boot Bitch unmounts the failed candidate, restores the preserved old root to `@`, restores the previous Btrfs default subvolume, and reconciles the original root's boot stack. The failed candidate is retained as `@rollback-failed-<timestamp>` for investigation rather than deleted.
- A successful rollback retains the previous root as `@rollback-before-<timestamp>` for manual recovery. The snapshot inventory and cached target diagnostics are invalidated because the active normal root changed.
- Rollback refuses a target with no normal top-level `@`, a snapshot missing Linux root metadata, or less than 1 GiB of Btrfs free working space. Read-only **Inspect Selected** and **Load Snapshots** remain available independently of rollback.

### 0.2.8

- **Committed target is now visually persistent.** Clicking rows continues to change only the inspection selection, while pressing **Select Target** marks that physical drive with a persistent `✓ SELECTED TARGET` status, a subtle full-row target tint, and a separate **Committed target:** label beside the Systems actions. The marker survives clicking/inspecting other drives and is rebuilt after device refreshes.
- The committed physical target remains the target for Repair, Diagnostics, Snapshots and File Copy until another physical drive is explicitly committed with **Select Target**. This separates transient row focus from destructive-operation intent.
- The LUKS passphrase dialog is narrower and purpose-built instead of using the oversized static text-input dialog. The secret remains password-masked and is still sent only over the privileged helper pipe.
- A cryptsetup **bad-passphrase** result is now distinguished from other unlock failures. Boot Bitch offers **Retry** and asks only for the LUKS passphrase again; the already-authorized administrator helper session remains active, so Polkit is not repeated.
- Every privileged target request continues to unmount only the temporary repair mounts it created before returning. Ending **File → Lock Administrator Session** or closing Boot Bitch then closes only LUKS mappings opened by Boot Bitch itself; pre-existing/external mappings are never closed.
- Administrator-session shutdown now allows more time for owned mount/mapper cleanup before escalating from graceful QUIT to termination. Locking the session refreshes device topology afterward so a successfully closed Boot Bitch-owned mapper immediately appears locked in Systems.

### 0.2.7

- **Fix retained administrator-session exit code 126.** The broker still copies the authenticated helper to a root-owned runtime file, but child operations are now invoked explicitly through `bash`. This works when `/run` is mounted `noexec` while retaining the protection against re-executing a user-writable source-tree helper after authorization. Diagnostics, File Copy, validation and repair therefore reuse the authorized session instead of all failing with exit code 126.
- **Snapshots are now loaded through the guarded helper, not the desktop user's mount permissions.** The Snapshots tab can enumerate Snapper-style `@.snapshots/<id>/snapshot` or `.snapshots/<id>/snapshot` trees through a temporary read-only Btrfs top-level mount. This avoids permission failures on root-owned snapshot metadata.
- **Inspect Selected** performs a second read-only helper request for the chosen snapshot and reports Btrfs metadata, Snapper type/description, `/etc/os-release`, `fstab`, `crypttab` and visible kernel/initramfs files. No snapshot/subvolume/boot state is modified.
- Snapshot rows retain the relative Btrfs path internally and show creation/type/description/status in the table. The snapshot inventory is cleared when the selected target identity changes.
- Failed **Run All** diagnostics are no longer logged as if successful results had been generated/cached.
- Existing-mapper handling remains unchanged: a pre-opened LUKS mapping is shown as **Already Unlocked** and is never closed by Boot Bitch.
- In 0.2.7, transactional Btrfs rollback was deliberately still disabled while privileged snapshot inventory/inspection was being hardware-validated.

### 0.2.6

- **Unlock state is explicit.** A locked LUKS child can be unlocked even when another mapped filesystem exists on the same physical drive; an already-open mapping now shows **Already Unlocked** instead of looking like a broken disabled Unlock button. Encrypted child rows report **LUKS unlocked — mapped Linux filesystem visible** when `lsblk` exposes their mapped child.
- New mappings use the conventional **`luks-<UUID>`** name rather than a `boot-repair-*` name. If another recovery tool already opened the same LUKS device under a different mapper name, Boot Bitch reuses it and never closes/reopens it blindly.
- A LUKS mapping created by Boot Bitch stays open while the authorized app session is active so later diagnostics/repairs can reuse it. **Lock Administrator Session** or exiting Boot Bitch closes only mappings opened by Boot Bitch; mappings that were already open before Boot Bitch attached to them are left untouched.
- Every privileged target operation now avoids propagating a foreign recovery-only mapper name into chroot mount metadata. Boot Bitch prefers an existing `luks-*` alias; otherwise it mounts through the canonical `/dev/dm-N` node, reads the target `crypttab`, and creates a **temporary `/dev/mapper/<expected>` compatibility symlink** when the installed system expects another name. The alias is removed on cleanup. This prevents `grub-probe`, `cryptsetup-initramfs`, package postinst hooks and initramfs rebuilds from failing on a stale recovery-only mapper path.
- Adds **Graphical login / SDDM** to Diagnostics. It reports the target default systemd target, `display-manager.service`, SDDM/Plasma package state, the SDDM unit file, persistent SDDM journal output, and recent graphics/NVIDIA/DRM-related target log lines.
- Adds **Restore SDDM graphical login** as an individual repair tool and opt-in Full Repair stage. It works offline: verifies SDDM is installed, sets `graphical.target` as the target default, enables SDDM, repairs `display-manager.service`, verifies both symlinks, and deliberately **does not start SDDM inside the repair chroot**.
- Adds **EFI / UKI boot state** Diagnostics. On a selected target it inventories only that target ESP, decodes a visible TUXEDO `TUX.EFI` `.uname`/`.cmdline` when `objcopy` is available, checks that the embedded kernel exists under target `/boot`, and filters firmware entries by the selected ESP PARTUUID so unrelated disks are not treated as repair targets.
- EFI repair is now **distribution-aware**. When a TUXEDO target provides `/usr/sbin/create_boot_uki_base.sh`, Boot Bitch uses that vendor builder instead of blindly running `grub-install`. Before the build it records the existing `TUXEDO UKI` entry, `BootOrder`, and `BootNext` for the selected ESP; after the builder recreates the entry it substitutes only the recreated ID back into the old order and leaves entries for other disks untouched. Conventional GRUB EFI targets retain the guarded `grub-install` path.
- Adds manual **Boot stack reconciliation** for a partial update or snapshot/root rollback mismatch. It validates mapper/crypttab, rebuilds all target initramfs images, rebuilds the TUXEDO UKI when the target supplies the official builder, verifies the embedded UKI kernel when possible, and regenerates the GRUB fallback. It does **not** remove kernels, snapshots, EFI entries, or unrelated-drive boot records.
- Host Capabilities now includes offline `systemctl`, `efibootmgr`, and `objcopy` support. The one-command dependency setup installs `efibootmgr` and `binutils` as well.
- In 0.2.6, Btrfs snapshot rollback was still disabled while the mapper/chroot and boot-stack reconciliation primitives were being validated; 0.2.9 builds the transactional rollback path on those primitives.

### 0.2.5

- The first privileged operation opens one **administrator helper session** through Polkit. Subsequent privileged actions in the same Boot Bitch window reuse that session, so selecting another target diagnostic, validating, copying files, or running a repair does not repeatedly ask for the administrator password.
- The main Qt GUI still runs as the normal desktop user. The privileged process exposes only the existing whitelisted helper commands; it is not a root shell and does not accept arbitrary commands.
- **File → Lock Administrator Session** explicitly closes the privileged helper. Closing Boot Bitch also closes it. The next privileged action then requests authorization again.
- For source-tree testing, the broker copies the authenticated helper to a root-owned mode-0700 runtime file before serving requests. This prevents a user-writable development helper from being modified and re-executed as root after the one-time authorization.
- Diagnostic results are cached **per diagnostic**, not merely in the currently visible result pane. Switching among Environment, Boot, Kernel/initramfs, GRUB, Errors, Usage, fstab, Btrfs, Mapper, LUKS and Full report preserves each result until it is explicitly re-run or invalidated.
- Cached diagnostics display their **capture date/time**. Target cache identity uses the physical repair disk plus the visible root filesystem UUID when available, so harmless `/dev/mapper/name` versus `/dev/dm-N` alias changes do not discard results.
- **Run All** uses one read-only privileged request, fills every individual diagnostic cache, and then displays the cached **Full diagnostic report**. Browsing any individual result afterward does not re-run it or re-authorize.
- A successful target write still invalidates target diagnostics. Changing to a different target/root also invalidates them. Read-only validation does not.
- An already-open mapper created by another recovery tool is intentionally treated as **already unlocked**. In that state the Unlock button is disabled because Boot Bitch must not blindly close/reopen a mapper it did not create.
- Host Capabilities row height is recalculated from the current column widths: rows collapse to one line when the text fits and expand only when text actually wraps.

### 0.2.4

- **Run All Available** now caches each repair-target diagnostic result in the GUI. Selecting Environment, Boot, Kernel/initramfs, GRUB, Errors, Usage, fstab, Btrfs, Mapper, or LUKS after Run All displays the stored result immediately instead of invoking Polkit again.
- An individual cached diagnostic is labeled **Re-run Diagnostic**; it authorizes again only when fresh target data is explicitly requested. Changing the selected target/root component or successfully modifying the target invalidates the target cache.
- Adds **EFI bootloader** as a separate repair tool and optional Full Repair stage. It is off by default in Settings so EFI writes never appear implicitly in the repair plan.
- Before EFI repair the helper validates `/boot/efi` **while the target is still read-only**, confirms it is mounted from the selected physical repair disk and is a FAT filesystem, and only then allows read-write promotion. It re-checks the same conditions immediately before writing, derives or reuses the target EFI vendor/bootloader ID, and runs target `grub-install --target=x86_64-efi --efi-directory=/boot/efi --recheck`.
- When writable UEFI efivars are available, `grub-install` may update the firmware NVRAM entry. When efivars are unavailable/read-only, Boot Bitch uses `--no-nvram`, repairs the ESP files only, and reports that firmware registration was skipped.
- The helper verifies that a GRUB/shim EFI binary exists under the resulting `/boot/efi/EFI/<bootloader-id>/` directory. An individual EFI repair also regenerates GRUB configuration afterward.
- EFI reinstall never uses `grub-install --force`; an unsafe/unsupported ESP or target layout is refused instead of being forced.
- Host capability table rows now compute their height from the **current** column widths: ordinary rows collapse to one line and only genuinely wrapped Feature/Notes content expands vertically.

### 0.2.3

- Fixes the shared safety check that previously rejected an unlocked `/dev/mapper/boot-repair-*` Btrfs root as not belonging to its selected physical disk. The helper now uses `lsblk --inverse` to trace dm-crypt/LVM-style dependency stacks to their physical disk and retains a conservative sysfs fallback. Multi-disk stacks are still refused.
- Partition, LUKS, mapper and filesystem child rows in **Systems** are selectable for inspection. **Select Target** still resolves to the parent physical disk and preferred Linux root, so detail selection cannot bypass the physical-target boundary.
- When an unlocked Linux-capable child is visible beneath LUKS, the top-level status now says **Unlocked Linux filesystem — inspect to confirm** instead of continuing to look locked.
- Repair-target Diagnostics now call the privileged helper, mount the selected target and safely resolvable `/boot`/`/boot/efi` entries **read-only**, inspect the target, then unmount on helper exit.
- Target diagnostics now report real target OS identity, root subvolume, boot contents/mounts, kernel/initramfs pairing, GRUB configuration, persistent journal errors, free-space usage, `fstab`, Btrfs subvolumes, mapper status and `crypttab`/mapper references.
- **Run All Available** uses one read-only privileged helper invocation for the entire target report, avoiding one authorization prompt per diagnostic item.
- Running-host diagnostics remain unprivileged and unchanged. The repair helper still requires a separate authorization when a later modifying repair or Host → Repair copy is started.

### 0.2.2

- Enables **Preview Changes** and **Copy and Verify** for both Host → Repair and Repair → Host.
- Uses `rsync -aHAX --numeric-ids` and never adds `--delete`; unrelated destination files remain in place.
- Smart ownership compares the source numeric UID/GID identity names across the host and repaired system. If the same numeric IDs mean different users/groups, the copied item is assigned to the destination-directory owner instead of silently preserving the wrong account.
- Explicit **Preserve source numeric UID/GID** remains available for advanced recovery work.
- Host → Repair validates target containment, rejects path traversal/symlink escapes, performs a read-only target preflight, and only then remounts the selected target filesystem read-write.
- Sensitive target paths such as `/etc`, `/boot`, `/usr`, `/root` and `/var` require the GUI's extra confirmation token before the helper will write them.
- Repair → Host keeps the repaired filesystem read-only and refuses system-critical host destinations. Normal recovery destinations under `/home`, `/mnt`, `/media`, `/run/media`, `/tmp`, or a separate non-system mount are accepted.
- After each real copy, the helper performs a second checksum/metadata rsync dry-run and SHA-256 verification of regular files.
- Target destination directories may be created safely; newly created directories inherit the nearest existing target directory's numeric owner.
- Retains the Systems-tab **Unlock** button and correct **Bidirectional file copy** capability name.
- Expands `check-dev-env.sh` to verify the runtime tools needed for unlock, mounting, verified copy and guarded target work, while `setup-dev-deps.sh` installs them in one pass on Debian/Ubuntu/TUXEDO-family development hosts.

### 0.2.1

- Adds an **Unlock** button beside **Select Target** for locked LUKS repair candidates.
- Sends the LUKS passphrase to the privileged helper only through standard input; it is not put in argv or the application/helper logs.
- Uses deterministic `boot-repair-*` mapper names and refuses collisions rather than replacing an existing mapping.
- Refreshes the storage topology after unlock so the mapped ext4/Btrfs/Linux filesystem becomes the preferred repair component and can subsequently be repaired read-write through the existing guarded helper.
- Changes File Copy from a one-way Host → Repair concept to an explicit **Host → Repair / Repair → Host** direction selector. Changing direction clears staged paths so host and repair namespaces cannot be mixed accidentally.
- Adds host destination browsing for Repair → Host and safe absolute repair-path staging for the repair side. Version 0.2.2 completes the guarded execution/verification backend.
- Renames the rsync capability to **Bidirectional file copy** to avoid the mangled one-way feature label.
- Cleans Debian package metadata by removing the redundant explicit `util-linux` dependency, adding the runtime `cryptsetup` dependency for Unlock, and emitting the extended description as one properly wrapped value.

### 0.2.0

- Adds `scripts/boot-repair-helper.sh`, a whitelisted privileged backend used through `pkexec` when the GUI is not already root.
- Repeats host-protection checks inside the privileged helper rather than trusting only GUI state.
- Rejects root components that do not resolve exclusively to the selected physical disk.
- Mounts repairs in an isolated `/run/boot-repair/session.*` tree and unmounts in reverse order on exit.
- Adds guarded individual Package, DKMS, Initramfs and GRUB actions plus the configured Full Repair sequence.
- Keeps `apt upgrade` opt-in; it runs only when the existing Settings checkbox is explicitly enabled.
- Improves unlocked-LUKS target selection by preferring a visible Linux filesystem over the still-visible crypto container.
- Adds `scripts/check-dev-env.sh` and `scripts/setup-dev-deps.sh` for one-command Debian/Ubuntu/TUXEDO development setup and validation.
- Includes the privileged helper in the staged install and `.deb` validation workflow.

### 0.1.10

- Cleans the remaining Debian package QA warnings from the 0.1.9 validation pass.
- Uses a correctly named Debian changelog entry (`boot-repair (...)`) for `changelog.gz`.
- Wraps the Debian extended package description at conventional line lengths.
- Retains the one-command Release executable + `.deb` build in `scripts/build.sh`.
- Retains `install.sh --dry-run` for installation simulation and explicit `INSTALL` confirmation for future real installation.

### 0.1.9

- Licensed under the MIT License; see `LICENSE`.
- Adds `scripts/build.sh` for one-command Release executable + `.deb` generation without installing anything.
- Debian package now installs MIT copyright/license metadata, compressed changelog and a `boot-repair(1)` man page.
- Debian package Release binary is stripped during packaging.
- Desktop entry uses a single main category to avoid duplicate-menu hints.
- Removes the unnecessary explicit dependency on Debian's Essential `util-linux` package while runtime capability checks remain in the application.
- Package description and maintainer metadata are lint-friendly.
- `test-deb.sh` suppresses the harmless `/var/log/dpkg.log` dry-run permission warning.
- `install.sh` installs the generated `.deb` only after explicit `INSTALL` confirmation and supports `--dry-run`.
- `uninstall.sh` prefers package-manager removal for a `.deb` installation and retains a CMake-manifest fallback.

### 0.1.8

- Repair, Snapshots and File Copy target summaries now prefer a single line when space allows, but wrap cleanly at narrow widths instead of competing with headings.
- Target summary styling is consistent across those workflow pages.
- Debian package directory permissions are normalized to conventional 0755 regardless of the builder's umask.
- The `.deb` build/simulation workflow from 0.1.7 is retained unchanged.

### 0.1.7 and earlier

- Removes the developer-only **Preview Unlock Dialog** control; the real LUKS dialog will return with the privileged read-only backend.
- Keeps compact page target labels to the selected physical drive; automatically resolved partition/component details remain available in tooltips and device details.
- Adds native CPack Debian packaging with automatic shared-library dependency discovery.
- Adds `scripts/package-deb.sh` to build, stage-check and create a `.deb` without installing it.
- Adds `scripts/test-deb.sh` to inspect, extract, dependency-simulate and optionally lint the `.deb` without installing it.
- Adds a dedicated **Diagnostics** tab with vertical diagnostic navigation, running-host/repair-target scope selection, selectable results, Copy Results, Save Results and Run All Available.
- Diagnostics are always visible; Settings never hides troubleshooting tools. Availability is determined only by the selected system and visible capabilities.
- Adds read-only summaries for environment, boot, kernel/initramfs, GRUB, boot-error capability, disk usage, fstab, Btrfs, mapper/LUKS and combined-report views.
- Adds concise contextual **?** help to the major workflow pages while removing redundant explanatory prose.
- Adds **Help → Using Boot Bitch** with the minimum requirement to boot from a live/recovery system or another Linux installation on a different drive.
- Snapshot controls now reflow vertically on narrow windows and the page scrolls before controls can overlap the snapshot table.
- Selected-tool status text shares the same left edge as its title/description instead of using an inset status frame.
- Host capability identity fields use consistent non-wrapping form rows and spacing.
- The protected running-host summary is more compact and keeps drive/model/size/connection/mount information on one summary line when space allows.
- The Systems target-selection disclosure was removed because contextual help now carries that explanation.
- Tab labels shorten at narrow window widths instead of exposing ambiguous tab overflow artifacts.
- Informational labels and diagnostic/table contents remain selectable/copyable.
- Existing stable scrollbar gutters, wrapped logs, dynamic candidate sizing and responsive Systems/Repair layouts remain.
- No privileged or storage-changing functionality is enabled.
