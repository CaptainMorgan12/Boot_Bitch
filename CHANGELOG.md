# Changelog

## Unreleased

- Make test-VM installs a hard workflow contract: after each major
  fix/implementation batch the test VMs (Fedora, Alpine BIOS/EFI) are rebuilt
  and installed so the user can test, while host packages are built and synced
  but never installed on the host. `AGENTS.md` and `docs/testing.md` document
  the rule and `scripts/test-agent-safety-contract.sh` (ctest `fast`) freezes
  its exact wording.
- Include `AGENTS.md` in the Alpine and Arch source archives so the packaged
  test suite can run the agent-safety contract from the unpacked tree.
- Add `scripts/test-agent-safety-contract.sh` and the `fast`-labeled
  `boot-repair-agent-safety-contract` ctest: it fails when a script installs
  the `boot-repair`/`boot-bitch` package on the host (the user
  installs packages; development dependencies stay allowed), invokes
  `local-refresh.sh --install`, or invokes a screenshot tool from `scripts/`
  or `tests/`, and it freezes both hard rules in `AGENTS.md`.
- Add `scripts/dev-check.sh`, the test-only per-change gate: it configures and
  builds only when sources changed, runs the fast ctest tier and prints a
  compact summary. It never builds a package, installs anything or touches a
  VM; `--ui` adds the UI suite only when UI/UX changed and `--full` runs every
  local test except the packaging-profile contract (the documented
  built-package trap).
- Add ctest tiers (`fast`, `ui`, `slow`, `packaging`, `release`) and document
  the iteration/checkpoint workflow in `AGENTS.md`, `docs/publishing.md` and
  `docs/testing.md`. Host installs,
  package builds and VM validation move to checkpoints or release approval.
- Consolidate the MainWindow regression suite from 161 to 148 cases by merging
  overlapping fixture setups (no coverage removed) and add a test seam for the
  automatic evidence-refresh delay, cutting the UI tier from ~49 s to ~31 s.
- Extend the repository-hygiene contract to personal account/host names, VM
  guest credentials, concrete UUIDs, disk vendor/model/serial text, MAC
  addresses, private IPv4 addresses, non-standard ports and SSH key material,
  with documented synthetic-fixture allowlists, and remove the two screenshots
  that showed a maintainer home path and a disk UUID.

## 0.2.25 — 2026-09-19

- Add a read-only distribution and boot backend profiler for the running host
  and selected repair target. Debian/APT and Arch/pacman families are detected
  separately, alongside initramfs generator, GRUB/systemd-boot/UKI layout, ESP
  mount and kernel naming evidence.
- Add transaction-specific Arch repair preflights and guarded apply paths for
  full pacman package transactions, mkinitcpio, conventional GRUB and EFI.
  Conventional EFI repair restores one verified vendor-loader firmware entry
  when a guarded grub-install leaves only files on the ESP.
  Repository/download errors, removals, unresolved dependencies, standalone
  APT/dpkg operations and unknown layouts stay explicitly gated.
- Add a backend-profile contract test and portable kernel/initramfs pairing
  diagnostics for Arch-style `vmlinuz-linux` and `initramfs-*.img` files.
- Make build dependency setup and installation package-manager aware for
  APT/dpkg, pacman, DNF/RPM and zypper/RPM hosts. Add native Arch, RPM and
  package-manager-neutral TGZ workflows while keeping `.deb` generation
  explicitly Debian-family.
- Gate every repair tool and Full Repair stage on read-only per-tool capability
  evidence from the selected scope's diagnostics (`Repair tool <key>:
  available|unavailable|<reason>`). Unavailable actions such as DKMS on a
  system without it, or dpkg/standalone APT metadata on Arch, are disabled in
  both the Repair tools and Settings plan; helper runtime preflights remain
  mandatory. Arch probes also require pacman configuration and database, a
  detected display manager, installed kernel module directories and a
  resolvable ESP where those stages need them. Every capability decision is
  recorded with supporting evidence in the combined diagnostic log, alongside
  the GUI's launch-time device, protected-host and helper detection.
- Arch: treat individual mirror retrieval failures as warnings when the
  sandboxed full pacman transaction still succeeds. Repository, signature,
  integrity, dependency, removal and transaction errors remain fatal.
- Fix a false host-maintenance refusal when the idle PackageKit daemon is
  running. Real conflicts are still detected through active apt/dpkg/pacman
  processes and held package-manager locks.
- Detect the target initramfs generator from what is actually installed
  (initramfs-tools, dracut, mkinitcpio) instead of a stray binary, and require
  the TUXEDO UKI builder for that layout's EFI repair.
- Request administrator authorization once when Host Maintenance is selected
  or a repair target is confirmed, then reuse the session for diagnostics and
  repairs; a cancelled prompt leaves the scope usable with a visible
  Authorize action.
- Show a global busy indicator while diagnostics, repairs, snapshots, unlock,
  copy, shell or authorization operations are running.
- Keep boot-repair journal evidence focused on system boot components: filter
  out unrelated application entries and collapse near-identical lines into a
  single counted entry. The filter now covers every journal evidence section
  (boot evidence, unlock evidence and display evidence), not just boot errors.
- Re-running diagnostics refreshes only the affected diagnostic sections in
  the application log; repair, snapshot and other entries are preserved. Log
  rendering is coalesced and incremental so automatic regeneration no longer
  causes UI lag, and the busy indicator sits in a reserved header slot so it
  cannot shift the layout.
- Logs and File Copy layouts adapt to narrow windows: the session-log frame
  defaults smaller and moves below the log view when space is tight, File Copy
  buttons shrink instead of overlapping, and drive summary text wraps instead
  of clipping.
- Add a guarded running-host shell: in Host Maintenance the Chroot Shell tab
  becomes a Host Shell that runs a bash command on the running host through the
  new `host-shell` helper command with firmware write isolation. Fixes an
  application crash when the shell was invoked from host maintenance.
- Coalesce authorization requests so entering Host Maintenance produces a
  single Polkit password prompt even when the deferred scope request and the
  pending diagnostics refresh overlap; the separate LUKS passphrase prompt is
  unchanged.
- Log search now supports `@section` terms (for example `@errors`) and a filter
  dropdown (All entries / Diagnostics / Repairs / each diagnostic section) to
  show complete diagnostic sections.
- Repair and Systems layouts: the Full Repair plan frame sizes to its content
  so Individual Repair Tools moves up, the plan/run buttons shrink and elide
  instead of overlapping, Systems action buttons share a standardized column
  width, runtime button labels elide instead of spilling, and the Snapshots
  tab uses the same framed section design as the other tabs.
- Log section filters now also show the repair entries related to that section
  (for example Kernel / initramfs shows the kernel diagnostic plus initramfs
  and DKMS repairs), never render an empty view, and the Individual Repair
  Tools pane defaults wider so tool names are fully visible.
- Handle vendor apt release-metadata changes (Origin/Label/Suite/Codename/
  Version) during the metadata refresh: warn with the affected repositories
  and retry once with Acquire::AllowReleaseInfoChange, while every other apt
  error and all package transactions stay strict.
- Each application launch starts a fresh session log: the live view begins
  empty and a new timestamped session file is created when a scope is
  identified. Previous runs appear only as prior sessions; a second window in
  the same process reuses the active session file.
- Write one canonical session log per run to a local log directory, with
  scope markers for the protected host and each selected repair target,
  visible timestamps, a prior-session list with read-only viewing, clear and
  add-note actions, and automatic retention (20 files or 10 MB).
- Automatically regenerate stale read-only diagnostics after repairs or
  target changes once a privileged session exists, so available repair
  actions stay enabled without manually clearing and re-running Run All.
  Settings toggle, default on; it never triggers an authorization prompt on
  its own.
- Code review pass: fixed a literal `\n` in the committed-target row tooltip,
  removed the unreachable `update-grub` GRUB-preflight fallback (the isolated
  `grub-mkconfig` path is the only supported preflight), consolidated the
  duplicate diagnostic-bundle cache helpers, delegated Full Repair stage
  gating to the canonical stage-key mapping, and cleaned up dead patterns,
  redundant packaging checks and ShellCheck false positives without changing
  behavior.
- Software-center packaging: the AppStream metainfo now ships six remote
  screenshots, the current release entry, a `<pkgname>` package
  association (so Discover lists the installed app and GNOME Software shows
  the installed size), and the deprecated developer_name tag is gone. The
  Debian package refreshes the hicolor icon cache via a postinst and the Arch
  package via an install scriptlet, the packaged desktop entry points at the
  installed icon path as a workaround for Discover's broken stock-icon
  lookup (upstream KDE bug), and the uninstall cleanup removes the legacy
  /usr/local icon cache that made launchers fall back to a generic icon.
- Declare the file system check tools as package dependencies: e2fsprogs,
  dosfstools, btrfs-progs and xfsprogs are hard dependencies, with
  exfatprogs, ntfs-3g/ntfsprogs, f2fs-tools, jfsutils, reiserfsprogs and
  zfsutils-linux recommended/optional; setup-dev-deps.sh installs them all
  for dev and VM rigs, and install.sh warns which filesystems cannot be
  checked when a tool is missing. Arch pacman transactions that perform no
  package changes ("there is nothing to do") now report unchanged instead of
  changed, matching the apt behavior.
- Fix conventional GRUB EFI repair when the host and repair ESPs carry
  same-labelled entries: efibootmgr's same-label warning could leak into the
  captured firmware ID, which made the BootOrder restore fail after
  grub-install had already removed and recreated entries. The created entry
  ID is now captured cleanly and validated, and a missing/invalid mapping
  fails closed with a specific reason instead of a generic error. A failed
  Full Repair plan now attributes results correctly: completed stages keep
  their own success/no-change result, only the failing stage is failed, and
  stages that never ran are reported as "not run".
- Make repair result summaries action-accurate: package metadata refresh,
  initramfs rebuild, package upgrade and the other stages now use their own
  verbs ("package metadata refreshed", "initramfs rebuilt", "no packages to
  upgrade", ...) instead of a generic "repair successful", and a file system
  check that skipped mounted/offline-only devices is reported as "not all
  filesystems were checked" with the skipped devices listed, never as "no
  repair needed". Summary glyphs are colored through a document highlighter so
  the green check / red cross / blue square survive every rendering path
  (initial, incremental, filter rebuild, section replacement, prior logs).
  A Full Repair plan's file-system pre-stage, plan summary/output and the
  post-plan diagnostics now read in chronological order, and a manual Check
  File Systems run is clearly distinguished from the plan pre-stage.
- Make diagnostics regeneration evidence-driven and scoped: repairs that
  provably changed nothing no longer invalidate cached diagnostics or trigger
  a regeneration (all other repair actions stay enabled), while changed
  actions invalidate only the sections their tool affects (for example a
  display-manager repair refreshes only the display section; package
  transactions and boot-stack reconciliation refresh the full set). Run Full
  Repair accumulates the changed stages' union and regenerates once at the
  end, skipping regeneration entirely when no stage changed. Repair log
  sections now start with a colored result summary (green ✓ repair
  successful, red ✗ repair failed, blue ▪ no repair needed) and a plan-level
  aggregate with one auditable line per stage. The host file system scope now
  includes the ESP and a separate /boot, and boot-stack reconciliation reuses
  the EFI/UKI rebuild and GRUB regeneration already performed in the same
  session instead of repeating them.
- Combined Run All reports now emit the read-only capability probe/evidence
  block once at the top instead of repeating it inside every diagnostic
  section; individual diagnostics keep their self-contained block. Re-running
  a diagnostic or repair action updates that section's existing log block in
  place (newest position) instead of appending another copy, so GRUB,
  initramfs, kernel, file system and other repeated sections no longer pile
  up in the live log; different tools and scopes stay separate, and session
  files on disk remain full history.
- Add a read-only-first file system repair engine. Diagnostics resolve the
  root, /boot, ESP and /home filesystems for the selected scope (repair
  target or running host), report each device's UUID and run the matching
  read-only check (e2fsck -n, xfs_repair -n, btrfs check --readonly,
  fsck.fat -n, fsck.exfat -n, ntfsfix -n, jfs_fsck -n, reiserfsck --check,
  zpool status) without writing. Only when a check reports issues does the
  tool offer repair: per-device modes are limited to what is valid for the
  mounted state (offline tools refuse mounted filesystems, btrfs scrub and
  zpool scrub are online modes), every repair requires explicit
  confirmation, dangerous modes such as btrfs check --repair carry an extra
  warning, and each helper invocation and its per-tool exit classification
  is logged. The new "File system repair" tool/stage is gated on the
  `filesystem` capability evidence and runs first in the Full Repair plan.
- Keep the automatic read-only diagnostics asynchronous and always
  available: selecting a repair disk, entering Host Maintenance, switching
  the repair disk, unlocking and authorizing still schedule one coalesced
  run, and manual Run All or individual diagnostics can be triggered at any
  time. Run All during the automatic full report is acknowledged, during an
  individual run it is queued, and individual runs wait centrally behind the
  active privileged request. Any privileged action (for example Make Default)
  waits for the running request to finish with a visible status instead of
  queueing an empty dialog that looks hung. Make Default now follows the same
  single truth log as the repair tools: host identity plus the cached
  running-host EFI/UKI capability evidence, parsed once in a shared helper.
- The Systems page pins its content to the viewport width like the other
  tabs: the device tree and the selected-drive details frame fit the window
  and scroll horizontally inside their own sections instead of clipping at
  the page edge, and the running-host action buttons wrap onto a second row
  on narrow windows, right-anchored to the card's trailing edge so Details,
  Host Maintenance and Make Default all stay visible at any width. The
  OS/storage/mount summary stacks above the actions when narrow and stays
  beside them when wide.
- Every scrollable page now shows a soft edge shadow (with a small chevron)
  when more content continues below or above the viewport, so sections such
  as Host capabilities and dependencies are no longer hidden behind the
  scroll area; the hint follows the scroll position and clears at the end.
- Splitter panes keep a relative minimum of one fifth of the splitter size
  instead of fixed pixel or row minimums, so dragging left/right or up/down
  never hides a pane. The Full Repair plan list starts at a few visible rows
  and grows to whatever height the divider gives it, with no forced height.
- Selecting Host Maintenance fills the Selected drive details panel with the
  protected running host drive; leaving it restores the committed repair
  target's details, or clears the panel when no target is committed.
- Extend the bundled icon atlas with dedicated monochrome kernel, initramfs,
  GRUB and distribution icons in the same line-art style as the rest of the
  atlas (GRUB and the distribution penguin are adapted from the TUXEDO OS
  KDE/Breeze theme, see `resources/icons/atlas/LICENSE.breeze`; kernel and
  initramfs are drawn for the atlas). The atlas is now authoritative: repair
  tools, Full Repair stages and diagnostic sections no longer inherit
  placeholder icons from the active desktop theme, so the GRUB and
  Kernel/initramfs sections, the Initramfs and GRUB tools, and the
  Distribution and boot backend profile match the theme on every desktop.
- Add an Alpine/apk repair backend alongside Debian/APT and Arch/pacman:
  read-only profile and capability evidence, simulation-first `apk fix` and
  `apk upgrade` transactions, and targeted missing-package-file repair driven
  by `apk audit --system` plus `apk info --who-owns`. The new `extlinux`
  capability key gates the Alpine extlinux stage and the existing capability
  keys are unchanged; EFI/UKI stages remain Debian/Arch-only.
- Add Alpine repair stages: `mkinitfs` initramfs rebuilds with kernel/flavor
  pairing, config-only extlinux regeneration through a guarded
  `update-extlinux` with entry preservation and rollback, and OpenRC
  display-manager runlevel restore that never starts a graphical session
  inside the target chroot. Alpine Host Maintenance uses the same guards
  through the running apk/OpenRC.
- Add native RPM packaging and tooling: `scripts/package-rpm.sh` builds the
  CPack RPM with family-specific dependency names and
  `scripts/rpm-postinst.sh` as its POSIX `/bin/sh` scriptlet, and
  `scripts/test-rpm.sh` inspects, extracts and validates the artifact without
  installing it.
- Add Alpine packaging and tooling: `scripts/package-alpine.sh` builds the
  signed `boot-bitch-<version>-r0.apk` with abuild and runs the project tests
  in its `check()` phase, and `scripts/test-apk.sh` validates the artifact
  metadata, dependencies, signature and installed file set without installing
  it.
- Add release-preparation tooling: `scripts/prepare-release.sh` promotes the
  Unreleased changelog entries into a dated version section, generates the
  scoped per-version release notes and inserts the new README refinements
  section without touching earlier ones; `scripts/verify-release.sh` checks
  the publishing invariants read-only, and `scripts/local-refresh.sh` syncs
  the canonical tree to the local staging mirror without committing or
  pushing.
- Add Alpine VM validation: provision the QEMU guest agent once in the guest,
  then `local-refresh.sh --alpine-vm` provisions the unprivileged abuild user,
  builds the signed package through `guest-exec`
  and copies it back while asserting the guest keeps running from its own
  disk.
- Add a Fedora/RPM-family repair backend from the same read-only probe
  evidence as the other families: guarded dnf5 package transactions
  (`rpm -Va` + `dnf reinstall` fix-broken, a `dnf makecache` metadata stage
  and one simulated `dnf upgrade`), dracut initramfs rebuilds with
  kernel/image pairing, trial builds and `lsinitrd` verification, GRUB2
  configuration regeneration (`grub2-mkconfig --no-grubenv-update` with BLS
  and menuentry preservation) plus a guarded GRUB2 bootloader reinstall on
  BIOS when the boot-code probe finds the MBR or BIOS boot partition broken,
  boot-stack reconciliation over dracut + GRUB2, and the systemd GDM display
  path with Fedora naming (`/etc/gdm/custom.conf`). Journald logging and the
  13 capability keys are unchanged; every Fedora label and gate derives from
  the helper's probe evidence, never from the distribution ID.
- Add Fedora VM validation (a single domain with a protected running host and
  a disposable target disk, virtiofs share, SELinux permissive for the guest
  agent, BIOS) and build the Fedora 44 RPM in the guest; the artifact is
  validated by `scripts/test-rpm.sh` without installing it and synced into the
  0.2.25 release capture.
- Surface the real Polkit/pkexec authorization failure in the GUI (for
  example a missing or unregistered authentication agent) instead of only the
  generic cancellation text, and mirror the full authorization output into
  the session log.

## 0.2.24 — 2026-09-16

- Bundle the semantic icon atlas as the deterministic UI source across desktop themes and add the Qt SVG runtime dependency required by Arch and other minimal installations.
- Add Arch host-maintenance package repair and upgrade stages through guarded full pacman transactions; standalone APT/dpkg stages remain unavailable on Arch.
- Remove stale `/usr/local` source installs during package uninstall so application-menu launches resolve the current packaged binary.
- Fix Debian/TUXEDO GRUB preflight to inspect an isolated `grub-mkconfig` output rather than `update-grub` progress text.
- Restore the protected-host shield/check as a bundled green status icon.

## 0.2.23 — 2026-09-15

This GitHub release includes the local 0.2.21 and 0.2.22 iterations below.

- Maintain one canonical UKI, fallback, and WebFAI destination per maintained
  ESP. Safely remove duplicate selected-ESP routes such as a legacy TUXEDO
  shim or device-path-only fallback when the explicit destination is present;
  unrelated ESP entries remain untouched.
- Group retained firmware entries by drive and normal boot use, keeping each
  drive's UKI or vendor loader before its fallback and WebFAI route.
- Add a read-only detected boot-chain and root-LUKS handoff summary to boot
  evidence so TUXEDO Debian-base UKI and TUXEDO Ubuntu/GRUB paths remain
  distinct and repairs do not introduce a second unlock route.
- Clarify EFI repair behavior and model-aware destination labels in the UI and
  documentation.
- Update existing EFI descriptions through a guarded EFI_LOAD_OPTION rewrite
  with PARTUUID/loader validation and read-back verification; `efibootmgr -L`
  is used only for creation because it does not rename an existing entry.
- Keep the repaired-system folder browser's navigation row outside the folder
  list's minimum height so compact dialogs do not overlap their controls.
- Clarify bundled icon fallbacks and Running Host versus selected repair-system
  support in the README, installed manual and package descriptions.

## 0.2.22 — 2026-09-14

- Fix Host → Repair destination browsing to decode directory records correctly
  and display the selected repair system's folders instead of an empty list.
- Keep the repaired-system browser compact by default, make it freely
  resizable, and reserve enough layout space for navigation and confirmation
  controls at smaller window sizes.

## 0.2.21 — 2026-09-14

- Protect multi-disk EFI state during TUXEDO UKI and conventional GRUB EFI
  rebuilds. The vendor helper's global `TUXEDO UKI` delete/create calls are
  isolated, every firmware entry is classified as host, selected repair target,
  foreign, or unknown, and stable PARTUUID/label/loader identities restore
  entries even when firmware reuses a Boot#### number. Host and unrelated-drive
  entries keep their relative order; vendor-specific paths are refused rather
  than guessed. BootOrder and BootNext are reconciled after the repair.
- Guard GRUB regeneration against dropping existing menu entries. A generated
  configuration that omits a previously present Linux entry is rejected and a
  post-write mismatch is rolled back to the known-good configuration.
- Restore a missing running-host TUXEDO UKI registration before resolving the
  selected-system entry, and annotate only the selected ESP's decoded firmware
  labels with its distribution/model identity. Existing model text is not
  duplicated; a present `iPXE.efi` loader without a matching entry receives a
  uniquely named `WFAI <model>` registration on TUXEDO systems (or `iPXE
  <model>` on another distribution) on that ESP.
- Make Host → Repair File Copy destination browsing target-aware. The chooser
  requests each directory listing through a temporary read-only target mount,
  instead of opening a host `QFileDialog` that could be mistaken for the
  repaired system's folder tree; a new destination can still be entered as an
  absolute virtual path and is validated again by the guarded copy.

## 0.2.20 — 2026-09-13

- Add native Qt fallback glyphs and bundled icon search paths so AppImage
  controls remain visible when the host desktop theme is unavailable.
- Keep GNOME, GTK and KDE platform-theme styling while making portable
  launches use the same readable icon treatment as installed builds.

## 0.2.19 — 2026-09-13

- Add CaptainMorgan12 attribution to the About dialog, AppStream metadata,
  Debian package metadata and copyright files.
- Add a Qt-aware AppImage build workflow with optional linuxdeploy Qt bundling.
- Keep GNOME/GTK snapshot rows compact and improve dark-theme icon contrast,
  including terminal and disabled-stage icons.

## 0.2.18 — 2026-09-13

- Keep table rows compact under GNOME/GTK platform themes and grow only rows that wrap.
- Improve dark-theme icon contrast while preserving colored status artwork.
- Update GitHub Actions checkout to `actions/checkout@v5` for Node.js 24 runners.

## 0.2.17 — 2026-09-13

- Add optional GNOME/GTK Qt platform-theme and XDG portal package suggestions.
- Document environment-driven GNOME theme selection while retaining native KDE styling.

## 0.2.16 — 2026-09-13

Maintenance release following the first public 0.2.15 release.

- Fixed compact snapshot-row sizing on Qt 6.4/Ubuntu CI.
- Added `pkexec` to the GitHub Actions test environment so repair-readiness UI checks run with the required capability present.
- Made source-tree helper discovery tolerant of a lost executable bit by invoking readable shell helpers through `/bin/bash`.
- Revalidated the UI, Chroot Shell, display-manager contract, package, desktop-entry and AppStream checks.

## 0.2.15 — 2026-09-13

First official GitHub release of Boot Bitch (published as `Boot_Bitch`).

- Added a guarded Qt 6 recovery workflow for selecting physical targets,
  unlocking LUKS volumes, inspecting boot state, and applying supported repairs.
- Added authoritative per-target diagnostic caching. Full Repair requires fresh
  matching evidence for every selected stage; individual repairs require their
  matching fresh diagnostic. Run All remains the simplest way to populate the
  complete set.
  Target edits, writes, shell commands, snapshot rollback, and Host → Repair
  copies mark affected evidence stale and explain how to regenerate it.
- Added simulation-first package, DKMS, initramfs, GRUB, EFI/UKI, and boot-stack
  repair paths with hard stops for unknown or unsafe failures.
- Added evidence-based graphical-login repair for SDDM, GDM3, LightDM, greetd,
  ly, and configured custom systemd display managers. The repair inspects the
  target default, installed units, and last-boot evidence, performs headless
  preflight checks, and never starts a graphical session in the chroot.
- Added read-only Btrfs/Snapper snapshot inventory and guarded transactional
  rollback for supported top-level-`@` layouts, with automatic restoration on
  critical reconciliation failure.
- Added the Chroot Shell, guarded bidirectional File Copy, persistent categorized
  Logs, asynchronous snapshot loading, and target configuration editors.
- Added protected ESP/firmware handling, target-aware encrypted-device checks,
  KDE/Freedesktop packaging metadata, AppStream metadata, and the `boot-repair(1)`
  manual page.
- Added UI, helper-contract, and staged Debian-package validation tests.

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
