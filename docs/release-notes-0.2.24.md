# Boot Bitch 0.2.24

Boot Bitch 0.2.24 is a maintenance release with the changes made since the
published 0.2.23 release.

Released 2026-09-16.

- Bundle the semantic icon atlas as the deterministic UI source across desktop
  themes and add the Qt SVG runtime dependency required by Arch and other
  minimal installations. The atlas gains dedicated monochrome kernel,
  initramfs, GRUB and distribution icons (GRUB and the penguin adapted from the
  TUXEDO OS KDE/Breeze theme; kernel and initramfs drawn in the atlas line-art
  style), and is now authoritative: the repair tools, Full Repair stages and
  diagnostic sections no longer fall back to placeholder or off-theme icons
  from the active desktop theme.
- Add Arch host-maintenance package repair and upgrade stages through
  transaction-specific preflights and guarded apply paths for full pacman
  package transactions, mkinitcpio, conventional GRUB and EFI. Repository and
  download errors, removals, unresolved dependencies, standalone APT/dpkg
  operations and unknown layouts stay explicitly gated.
- Remove stale `/usr/local` source installs during package uninstall so
  application-menu launches resolve the current packaged binary.
- Fix Debian/TUXEDO GRUB preflight to inspect an isolated `grub-mkconfig`
  output rather than `update-grub` progress text.
- Restore the protected-host shield/check as a bundled green status icon.
- Add a read-only distribution and boot backend profiler for the running host
  and selected repair target. Debian/APT and Arch/pacman families are detected
  separately, alongside initramfs generator, GRUB/systemd-boot/UKI layout, ESP
  mount and kernel naming evidence.
- Add a backend-profile contract test and portable kernel/initramfs pairing
  diagnostics for Arch-style `vmlinuz-linux` and `initramfs-*.img` files.
- Make build dependency setup and installation package-manager aware for
  APT/dpkg, pacman, DNF/RPM and zypper/RPM hosts. Add native Arch, RPM and
  package-manager-neutral TGZ workflows while keeping `.deb` generation
  explicitly Debian-family.
- Show brief completion notices and visible diagnostic prerequisites for
  disabled repair actions, with full output retained in Results and Logs.
- Gate every repair tool and Full Repair stage on the selected scope's
  read-only diagnostics. Actions whose prerequisites are missing, such as DKMS
  on a system without it or dpkg/standalone APT metadata on Arch, are disabled
  with the diagnostic reason while helper runtime preflights remain. Arch
  probes also require pacman configuration and database, a detected display
  manager, installed kernel module directories and a resolvable ESP where
  those stages need them. Every decision is recorded with supporting evidence
  in the combined diagnostic log, alongside the GUI's launch-time device,
  protected-host and helper detection.
- Arch: a full sandboxed pacman transaction that recovers from individual
  mirror download failures now completes with a warning instead of a false
  refusal; repository, signature, integrity, dependency and transaction errors
  still refuse.
- An idle PackageKit daemon no longer blocks host package repairs; active
  apt/dpkg/pacman processes and held package-manager locks still do.
- Detect the target initramfs generator from what is actually installed
  (initramfs-tools, dracut, mkinitcpio) instead of a stray binary, and require
  the TUXEDO UKI builder for that layout's EFI repair.
- Administrator authorization is requested once when a scope is selected and
  reused for diagnostics and repairs; a cancelled prompt leaves a visible
  Authorize action. A global busy indicator covers diagnostics, repairs,
  snapshots, unlock, copy, shell and authorization.
- Journal evidence only keeps boot-relevant system entries and collapses
  near-identical lines, so the combined log stays actionable.
- Re-running diagnostics refreshes only the affected diagnostic sections in
  the application log; repair, snapshot and other entries are preserved. Log
  rendering is coalesced and incremental so automatic regeneration no longer
  causes UI lag, and the busy indicator sits in a reserved header slot so it
  cannot shift the layout.
- Logs and File Copy layouts adapt to narrow windows: the session-log frame
  defaults smaller and moves below the log view when space is tight, File Copy
  buttons shrink instead of overlapping, and drive summary text wraps instead
  of clipping.
- Host Maintenance provides a guarded Host Shell (bash on the running host with
  firmware write isolation) instead of a chroot shell, fixing a crash when the
  shell was invoked from host maintenance.
- Entering Host Maintenance shows a single Polkit password prompt; concurrent
  authorization requests coalesce, and the separate LUKS passphrase prompt is
  unchanged.
- Log search supports `@section` terms and a filter dropdown to show complete
  diagnostic sections; re-running diagnostics refreshes only those sections.
- Repair and Systems layouts are content-sized and responsive: plan frame
  tracks its content, buttons shrink/elide instead of overlapping or spilling,
  and Snapshots uses the same framed design as the other tabs.
- Log section filters include the related repair entries (for example
  Kernel / initramfs shows the kernel diagnostic plus initramfs and DKMS
  repairs), never render empty, and the Individual Repair Tools pane defaults
  wider. Vendor apt release-metadata changes are accepted with a warning and
  one Acquire::AllowReleaseInfoChange retry; all other apt errors stay strict.
- Each application launch starts a fresh session log: the live view begins
  empty and a new timestamped session file is created when a scope is
  identified, while a second window in the same process reuses the active
  session file. One canonical session log per run carries scope markers for the
  protected host and each selected repair target, visible timestamps, a
  prior-session list with read-only viewing, clear and add-note actions, and
  automatic retention.
- Stale read-only diagnostics are regenerated automatically after repairs or
  target changes once a privileged session exists, so available repair
  actions stay enabled without a manual Run All (Settings toggle, default
  on; never prompts for authorization by itself).
- Code review pass: fixed the committed-target tooltip newline, removed the
  dead update-grub preflight fallback, consolidated duplicate diagnostic
  cache helpers and Full Repair stage gating, plus dead-pattern and
  ShellCheck cleanups with no behavior changes.
- Software centers now show the app correctly: the installed AppStream
  component ships six remote screenshots, links the component to the
  `boot-repair` package for GNOME Software and KDE Discover, reports the
  installed size and the Boot Bitch logo, and refreshes the icon cache (plus a
  packaged icon-path workaround for Discover's stock-icon bug).
- File system check tools are now package dependencies (common tools hard,
  the rest recommended/optional), dev/VM setups install them all, install.sh
  warns about missing tools, and no-op pacman transactions report unchanged.
- Fix conventional GRUB EFI repair with same-labelled host/repair ESP
  entries: the efibootmgr warning no longer pollutes the captured firmware ID
  (which broke the BootOrder restore), and a failed plan now marks only the
  failing stage as failed while completed stages keep their own result and
  skipped stages are "not run".
- Repair result summaries now use action-accurate wording (metadata refresh,
  initramfs rebuild, package upgrades, ...) and report skipped/unchecked
  filesystems explicitly; the colored ✓/✗/▪ glyphs survive every log
  rendering path, and a Full Repair plan reads chronologically with manual
  Check File Systems runs clearly distinguished from the plan pre-stage.
- Evidence-driven diagnostics regeneration: repairs that changed nothing
  keep the cache valid and leave every action enabled; changed repairs
  refresh only the affected sections (or the full set for package/boot-stack
  work), with one regeneration at the end of a Full Repair plan. Repair log
  sections carry a colored result summary (✓ successful / ✗ failed / ▪ no
  repair needed) and the plan adds an aggregate with per-stage lines. The
  host file system scope now includes the ESP and /boot, and boot-stack
  reconciliation reuses the EFI/UKI and GRUB work from the same session.
- Combined Run All reports show the capability probe/evidence block once, and
  re-running a diagnostic or repair updates its existing log section in place
  instead of appending duplicates (GRUB, initramfs, kernel, file system, and
  other repeated sections).
- New read-only-first File system repair tool and Full Repair stage
  (ordered first): resolves the root, /boot, ESP and /home filesystems with
  their UUIDs, runs the matching read-only check, and only offers per-device
  repair modes after issues are found and the user confirms, with offline
  tools refusing mounted filesystems and btrfs/zpool scrub offered online.
- Automatic read-only diagnostics stay asynchronous and manual Run All or
  individual diagnostics can be triggered at any time; privileged actions
  such as Make Default wait for a running request with a visible status
  instead of an empty dialog. Make Default is gated on the same single truth
  log as the repair tools (host identity plus cached running-host EFI/UKI
  capability evidence).
- The Systems page fits the window width like the other tabs: its device tree
  and selected-drive details frame scroll horizontally inside their own
  sections, and the running-host card stacks its summary above the action
  buttons, which wrap to a second row right-anchored to the card's trailing
  edge so every control stays visible at any width.
- Scrollable pages show a soft edge shadow with a small chevron whenever more
  content continues below or above the viewport, and splitter panes keep a
  relative one-fifth minimum so dragging left/right or up/down never hides a
  pane; the Full Repair plan list starts at a few rows and is fully
  splitter-adjustable. Selecting Host Maintenance fills the Selected drive
  details panel with the running host drive.

The Debian package and AppImage are built locally from the complete source tree.

[Full source diff: v0.2.23...v0.2.24](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.23...v0.2.24).
