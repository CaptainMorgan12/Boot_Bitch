# Boot Bitch 0.2.25

Boot Bitch 0.2.25 is a maintenance release with the changes made since the
published 0.2.24 release. The primary changes are the new RPM and APK release
artifacts and the Alpine, Arch, and Fedora distribution support; the remaining
refinements are listed below.

Released 2026-09-19.

- Add running-host Btrfs snapshot rollback to Host Maintenance. The Snapshots
  tab lists Snapper root snapshots and Boot Bitch `@rollback-before-*` undo
  points through the new `host-snapshots` helper command, stages the selected
  snapshot with the name-preserving transaction (preserve the running `@`,
  promote a writable copy, migrate a nested `@/.snapshots` child subvolume,
  reconcile initramfs/UKI/GRUB in a scratch chroot, automatically restore on
  failure) and reports `Host snapshot rollback` capability evidence without
  adding a 14th `Repair tool` key. A successful rollback persists a **Reboot
  required** reminder, cleared by kernel boot-id reconciliation, with `Reboot
  Now` offered only after a second explicit confirmation through the new
  `host-reboot` command; rollback never reboots automatically. Running-host
  rollback is limited to Snapper top-level `@` roots with no `subvolid=` pin,
  no separate `/boot` and no other nested `@` child subvolumes.
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
- Add transaction-specific Arch repair preflights and guarded apply paths for
  full pacman package transactions, mkinitcpio, conventional GRUB and EFI.
  Conventional EFI repair restores one verified vendor-loader firmware entry
  when a guarded grub-install leaves only files on the ESP.
  Repository/download errors, removals, unresolved dependencies, standalone
  APT/dpkg operations and unknown layouts stay explicitly gated.
- Add a read-only distribution and boot backend profiler for the running host
  and selected repair target. Debian/APT and Arch/pacman families are detected
  separately, alongside initramfs generator, GRUB/systemd-boot/UKI layout, ESP
  mount and kernel naming evidence.
- Add a backend-profile contract test and portable kernel/initramfs pairing
  diagnostics for Arch-style `vmlinuz-linux` and `initramfs-*.img` files.
- Add Alpine VM validation: provision the QEMU guest agent once in the guest,
  then `local-refresh.sh --alpine-vm` provisions the unprivileged abuild user,
  builds the signed package through `guest-exec` and
  copies it back while asserting the guest keeps running from its own disk.
- Add Fedora VM validation (a single domain with a protected running host and
  a disposable target disk, virtiofs share, SELinux permissive for the guest
  agent, BIOS) and build the Fedora 44 RPM in the guest; the artifact is
  validated by `scripts/test-rpm.sh` without installing it and synced into the
  0.2.25 release capture.
- Make build dependency setup and installation package-manager aware for
  APT/dpkg, pacman, DNF/RPM and zypper/RPM hosts. Add native Arch, RPM and
  package-manager-neutral TGZ workflows while keeping `.deb` generation
  explicitly Debian-family.
- Gate every repair tool and Full Repair stage on read-only per-tool
  capability evidence from the selected scope's diagnostics (`Repair tool
  <key>: available|unavailable|<reason>`). Unavailable actions such as DKMS on
  a system without it, or dpkg/standalone APT metadata on Arch, are disabled
  in both the Repair tools and Settings plan; helper runtime preflights remain
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
  repairs; a cancelled prompt leaves the scope usable with a visible Authorize
  action.
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
  becomes a Host Shell that runs a bash command on the running host through
  the new `host-shell` helper command with firmware write isolation. Fixes an
  application crash when the shell was invoked from host maintenance.
- Coalesce authorization requests so entering Host Maintenance produces a
  single Polkit password prompt even when the deferred scope request and the
  pending diagnostics refresh overlap; the separate LUKS passphrase prompt is
  unchanged.
- Log search now supports `@section` terms (for example `@errors`) and a
  filter dropdown (All entries / Diagnostics / Repairs / each diagnostic
  section) to show complete diagnostic sections.
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
- Write one canonical session log per run to a local log directory, with scope
  markers for the protected host and each selected repair target, visible
  timestamps, a prior-session list with read-only viewing, clear and add-note
  actions, and automatic retention (20 files or 10 MB).
- Automatically regenerate stale read-only diagnostics after repairs or target
  changes once a privileged session exists, so available repair actions stay
  enabled without manually clearing and re-running Run All. Settings toggle,
  default on; it never triggers an authorization prompt on its own.
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
  installed icon path as a workaround for Discover's broken stock-icon lookup
  (upstream KDE bug), and the uninstall cleanup removes the legacy /usr/local
  icon cache that made launchers fall back to a generic icon.
- Declare the file system check tools as package dependencies: e2fsprogs,
  dosfstools, btrfs-progs and xfsprogs are hard dependencies, with exfatprogs,
  ntfs-3g/ntfsprogs, f2fs-tools, jfsutils, reiserfsprogs and zfsutils-linux
  recommended/optional; setup-dev-deps.sh installs them all for dev and VM
  rigs, and install.sh warns which filesystems cannot be checked when a tool
  is missing. Arch pacman transactions that perform no package changes ("there
  is nothing to do") now report unchanged instead of changed, matching the apt
  behavior.
- Fix conventional GRUB EFI repair when the host and repair ESPs carry
  same-labelled entries: efibootmgr's same-label warning could leak into the
  captured firmware ID, which made the BootOrder restore fail after
  grub-install had already removed and recreated entries. The created entry ID
  is now captured cleanly and validated, and a missing/invalid mapping fails
  closed with a specific reason instead of a generic error. A failed Full
  Repair plan now attributes results correctly: completed stages keep their
  own success/no-change result, only the failing stage is failed, and stages
  that never ran are reported as "not run".
- Make repair result summaries action-accurate: package metadata refresh,
  initramfs rebuild, package upgrade and the other stages now use their own
  verbs ("package metadata refreshed", "initramfs rebuilt", "no packages to
  upgrade", ...) instead of a generic "repair successful", and a file system
  check that skipped mounted/offline-only devices is reported as "not all
  filesystems were checked" with the skipped devices listed, never as "no
  repair needed". Summary glyphs are colored through a document highlighter so
  the green check / red cross / blue square survive every rendering path
  (initial, incremental, filter rebuild, section replacement, prior logs). A
  Full Repair plan's file-system pre-stage, plan summary/output and the
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
  sections now start with a colored result summary (green ✓ repair successful,
  red ✗ repair failed, blue ▪ no repair needed) and a plan-level aggregate
  with one auditable line per stage. The host file system scope now includes
  the ESP and a separate /boot, and boot-stack reconciliation reuses the
  EFI/UKI rebuild and GRUB regeneration already performed in the same session
  instead of repeating them.
- Combined Run All reports now emit the read-only capability probe/evidence
  block once at the top instead of repeating it inside every diagnostic
  section; individual diagnostics keep their self-contained block. Re-running
  a diagnostic or repair action updates that section's existing log block in
  place (newest position) instead of appending another copy, so GRUB,
  initramfs, kernel, file system and other repeated sections no longer pile up
  in the live log; different tools and scopes stay separate, and session files
  on disk remain full history.
- Add a read-only-first file system repair engine. Diagnostics resolve the
  root, /boot, ESP and /home filesystems for the selected scope (repair target
  or running host), report each device's UUID and run the matching read-only
  check (e2fsck -n, xfs_repair -n, btrfs check --readonly, fsck.fat -n,
  fsck.exfat -n, ntfsfix -n, jfs_fsck -n, reiserfsck --check, zpool status)
  without writing. Only when a check reports issues does the tool offer
  repair: per-device modes are limited to what is valid for the mounted state
  (offline tools refuse mounted filesystems, btrfs scrub and zpool scrub are
  online modes), every repair requires explicit confirmation, dangerous modes
  such as btrfs check --repair carry an extra warning, and each helper
  invocation and its per-tool exit classification is logged. The new "File
  system repair" tool/stage is gated on the `filesystem` capability evidence
  and runs first in the Full Repair plan.
- Keep the automatic read-only diagnostics asynchronous and always available:
  selecting a repair disk, entering Host Maintenance, switching the repair
  disk, unlocking and authorizing still schedule one coalesced run, and manual
  Run All or individual diagnostics can be triggered at any time. Run All
  during the automatic full report is acknowledged, during an individual run
  it is queued, and individual runs wait centrally behind the active
  privileged request. Any privileged action (for example Make Default) waits
  for the running request to finish with a visible status instead of queueing
  an empty dialog that looks hung. Make Default now follows the same single
  truth log as the repair tools: host identity plus the cached running-host
  EFI/UKI capability evidence, parsed once in a shared helper.
- The Systems page pins its content to the viewport width like the other tabs:
  the device tree and the selected-drive details frame fit the window and
  scroll horizontally inside their own sections instead of clipping at the
  page edge, and the running-host action buttons wrap onto a second row on
  narrow windows, right-anchored to the card's trailing edge so Details, Host
  Maintenance and Make Default all stay visible at any width. The
  OS/storage/mount summary stacks above the actions when narrow and stays
  beside them when wide.
- Every scrollable page now shows a soft edge shadow (with a small chevron)
  when more content continues below or above the viewport, so sections such as
  Host capabilities and dependencies are no longer hidden behind the scroll
  area; the hint follows the scroll position and clears at the end.
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
- Add release-preparation tooling: `scripts/prepare-release.sh` promotes the
  Unreleased changelog entries into a dated version section, generates the
  scoped per-version release notes and inserts the new README refinements
  section without touching earlier ones; `scripts/verify-release.sh` checks
  the publishing invariants read-only, and `scripts/local-refresh.sh` syncs
  the canonical tree to the local staging mirror without committing or
  pushing.
- Surface the real Polkit/pkexec authorization failure in the GUI (for example
  a missing or unregistered authentication agent) instead of only the generic
  cancellation text, and mirror the full authorization output into the session
  log.

The Debian package, AppImage, Arch package, RPM and APK are built from the
complete source tree and validated in their target environments. Verify
downloaded artifacts against the SHA256SUMS attached to this release.

[Full source diff: v0.2.24...v0.2.25](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.24...v0.2.25).
