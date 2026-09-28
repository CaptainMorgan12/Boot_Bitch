# Changelog

## Unreleased

- Legacy GUI parity: the busy indicator moves out of the header row into a
  reserved row below the GUARDED REPAIR badge (right-aligned under it, like
  the modern Qt6 window), shows the modern working text for the operation in
  flight (Running all diagnostics / Running diagnostic: <title> /
  Regenerating diagnostics automatically / Unlocking <device> or the
  tool/operation title) instead of the generic Working..., and animates with
  a lightweight QTimer ellipsis cycle (0..3 trailing dots; no threads) while
  keeping the header layout stable. The Logs Save As... default is pinned
  down: with a writable /host share mounted the dialog starts there, the rig
  is shut down and the share is mirrored host-side, so the saved log lands
  under the host's shared folder; the dialog still allows any directory. The
  legacy GUI contract gains static markers for the busy-indicator parity and
  the save-as /host default. The parsed `--log-dir` is now applied before the
  window is constructed, so a root-run session started with a preserved HOME
  can no longer drop its first log lines into the invoking user's log tree
  (the constructor's readiness preamble previously predated the `--log-dir`
  move).

## 0.2.26 — 2026-09-26

- Stabilization fixes from user testing across every rig: the TUXEDO UKI
  rebuild now passes its post-build verification for both valid Btrfs
  subvolume spellings (subvol=@ and subvol=/@), the Full Repair stage rows
  show the helper's stage-specific failure reason instead of an unrelated
  earlier error line, unavailable plan stages are shown disabled and
  unchecked (the user's saved preferences are restored when a tool becomes
  available again), the dnf5 upgrade apply step announces its approximate
  download size so a large transaction no longer looks like a hang, and the
  Fedora/dracut initramfs stage builds each kernel once into a verified
  temporary image before installing it (roughly halving the stage time
  while keeping the build-before-install safety net). The efi tool is only
  offered when an EFI System Partition is actually present or derivable on
  the selected disk (by GPT partition type or fstab), and the interactive
  shell answer flow, the filtered private /dev, and the TUXEDO UKI vendor
  integration were validated end-to-end on Arch, Alpine (BIOS and EFI),
  Fedora, Debian trixie and the Debian Etch legacy edition. The Debian
  Etch edition additionally fixed its repair-summary parsing, its
  Settings plan now lists the same capability keys as the modern app,
  cryptsetup is resolved from /sbin and /usr/sbin, multi-LV targets mount
  cleanly, and menu.lst regeneration preserves the single-user variant
  entries with their serial-console arguments. A quick per-change
  development gate keeps iteration fast while the full gate still runs at
  checkpoints and release.

- Security sweep: a comprehensive security review of the modern helper, both GUIs,
  the legacy port generator and the packaging tooling produced a large
  hardening pass. Mount containment: target-controlled fstab mountpoints
  (btrfs subvolumes and data partitions) are lexically, realpath- and
  symlink-validated before mounting; the session-log append into the repaired
  system refuses symlinked target log paths; the resolver validates before
  creating; the pacman sandbox no longer resolves through a target /tmp
  symlink and cleanup never removes a host-absolute path; offline filesystem
  repair fails closed on busy mounts instead of lazy-detaching; fs scope
  resolution gates fstab devices to the selected disk; recovery mounts force
  nosuid,nodev and drop context=/seclabel; LUKS UUIDs are validated before
  mapper-name composition; EFI/bootloader apply stages re-assert the
  protected-host identity gate; and a per-target-disk lock refuses
  concurrent repair sessions on the same disk. Interactive shell and wire
  protocol: a per-pump random token gates PROMPT forwarding so a command
  cannot forge prompt dialogs; the shell deadline is evaluated on every pump
  iteration (continuously-streaming commands time out); the runner-death
  drain is bounded; the pty echo of answers is redacted so typed secrets
  never reach the wire, session log or transcript; a dead runner never
  produces a prompt; SIGPIPE can no longer kill the root session; CSI/OSC
  sequences are bounded; chroot-shell exit codes 124/125 propagate; apt
  retries are binary-aware (apt full-upgrade / apt-get dist-upgrade); shell
  command quoting is POSIX-safe; apt probing happens inside the target
  chroot; protocol records are length-capped; a session secret is only
  accepted for unlock. Process lifecycle: helper runs register in a
  root-owned PID registry and cleanup kills still-alive registered children
  before unmounting; the interactive pump group-kills the runner's whole
  tree; the broker dispatches under setsid; the host-shell timeout gains
  --kill-after; the GUI watchdog sends QUIT before terminating and pauses
  while a prompt popup is open. Repair chroots receive a filtered private
  /dev (essential nodes, dm control and devices on the selected target disk
  only). File Copy to the host strips setuid/setgid and drops
  security.capability (probed flags, suid-masked verification, fail-closed
  post-copy scan) and refuses sticky/world-writable destinations with
  TOCTOU re-validation before every transfer. GRUB regeneration runs with
  GRUB_DISABLE_OS_PROBER=true (a deliberate dual-boot behavior change; the
  native-entry preservation guard is unchanged). GUI hardening: helper
  resolution never uses the current directory and the environment override
  is test-only, portable helpers must be owner-matched and not
  group/world-writable; privileged arguments are validated per argument
  class; the capability gate reads only the dedicated capabilities entry
  (target-controlled file content can no longer forge gating lines); the
  root-component fallback requires the same disk and a non-protected
  component; password-shaped prompt answers are masked in the transcript
  while the raw answer still reaches the helper; session logs are 0600 and
  the log directory 0700; settings are 0600; one prompt dialog at a time
  with a bounded prompt budget; per-request transcript caps; scanner reads
  are bounded and device display strings are control-character-stripped;
  config-write content now travels through a guarded mode-600 secret file
  instead of process arguments (1 MiB cap, never logged, never deleted by
  the helper). Legacy edition: the port generator fails loudly on no-op
  transforms and completed its bash-4 blacklist; the unlock keyfile can no
  longer land at the filesystem root and requires provable ownership (the
  helper never deletes a caller-supplied file); sudo/gksu/gksudo resolve
  from fixed trusted paths only and the elevated helper is verified before
  running; cancel now carries a helper-side cancel token with bounded
  TERM/KILL escalation; the host shell is timeout-bounded; the config-write
  secret-file transport is ported; logs are 0600 with O_NOFOLLOW opens.
  Tooling: build scripts whitelist their rm -rf targets, the EFI label
  updater refuses symlinked backups and reports rollback failures honestly,
  the zsync update channel is documented as transport-only-authenticated,
  zsyncmake temp dirs are cleaned up, and the dev rig scripts scrub guest
  passwords from serial logs, validate and quote guest-shell interpolations
  and serialize release-artifact syncs.
- Cycle 16 (interactive shell): the Chroot Shell and Host Shell can now
  answer interactive prompts. The helper runs shell commands under a PTY
  (with a plain stdin-pipe fallback) and, whenever the command is alive but
  its output has been quiet, sends the trailing output to the GUI as a
  base64 `PROMPT` wire record — no prompt text is ever pattern-matched or
  interpreted. Boot Bitch shows a "Shell command is asking for input" popup
  with the output and a free-text answer field; the answer is written back to
  the command (repeatedly for multi-question commands such as debconf flows),
  while cancel, an empty answer, or no answer within the window fails the
  command closed with the non-interactive hint. The per-command timeout
  pauses while a prompt awaits the user. Non-interactive flags (`dnf update
  -y`, `apt-get -y upgrade`) remain recommended for unattended runs; the
  legacy Qt3 shell keeps its non-interactive behavior for now.

- Cycle 15 (modern parity): Select Target now mirrors the modern
  diskIsSelectableRepairTarget predicate — enabled for every selectable
  non-host, non-optical drive including blank/non-Linux data disks (a locked
  LUKS container without a visible Linux filesystem stays unlock-only), the
  committed drive keeps the button enabled with the committed tooltip, and a
  root-less commit stores the disk itself so diagnostics fail closed with the
  helper's reason; the protected running host is excluded from the candidate
  tree; the status-bar scope text is transient (timeout) plus the transient
  "Repair drive selected: …" commit message; the Repair plan buttons reflow to
  their own row and the tool title can shrink so "Reconcile Boot Stack" never
  clips at the minimum width; the (i) buttons hug their page titles; the
  drive-row details roll up the UUID (unlocked root > LUKS container > Linux
  root partition).

- Cycle 14 UI fixes: the (i) help affordance is now a frameless hyperlink
  label beside each page title that opens a width-constrained read-only info
  dialog on click (no tooltip-only behavior); the File Copy sections are
  restored inside the vertical splitter (the panes own the groups from
  construction); the PROTECTED and GUARDED REPAIR badges lose their frames;
  the protected-host details pane shows the root component's UUID and
  Mounts; the session unlock state survives rescans while the mapping exists
  (leaving Host Maintenance no longer loses the unlocked target) and the
  helper now emits `UNLOCKED_ROOT_UUID=`; the individual repair tools list
  sorts by header click; the Logs session pane gets a wider floor, the
  application-log pane a 480px minimum, and long log lines wrap at the
  widget width with the horizontal scrollbar off (while wrap is on).

- Cycle 13 cleanup: the host-capabilities table sorts by header click; Unlock
  stays available for a selected offline drive while Host Maintenance is
  active (only the protected host disk is refused); the committed-target
  summary stays right-aligned when the Authorize affordance is hidden; Select
  Target disables for the already-committed drive and stays enabled for other
  selectable drives; the Details button now populates the right details pane
  with the protected host's facts (no popup) and selecting any drive restores
  the per-drive pane; the Repair tab's legacy Privilege elevation section is
  gone (elevation stays in Settings and the status bar); File Copy gains a
  draggable vertical splitter and drops its inline probe status text; the
  Chroot Shell page shows the modern scope notices instead of the probe
  status; every page title gains an (i) help button carrying the
  informational text (the Logs page is now titled "Application log", and the
  Settings storage note moved into its popup); lower pane floors keep the
  Diagnostics and Logs action buttons visible when the window shrinks.

- Cycle 12: split-LV targets and GRUB defoptions preservation. The legacy
  prepare_target now mounts the target's remaining standard-system fstab
  entries (/usr, /var, /tmp, /home, /opt, /srv) under the repair root after
  the root and boot mounts — host-relative device remapping, same-disk
  containment, pseudo-entry skipping, read-only diagnostics tolerant of a
  failed data mount (logged) and repair stages fail closed — so the package
  stages see /var/lib/dpkg and apt's /var/lib/apt on split-LV layouts; the
  existing mount records mean teardown unmounts them in reverse order. The
  GRUB-legacy regeneration guard now parses the menu's `# defoptions=`/`#
  kopt=` comments, strips those managed arguments from the kernel-line
  comparison (an update-grub expansion is no longer mistaken for entry
  removal), and still rolls back on genuine removals or when a managed
  argument is dropped from the regenerated kernel lines.

- Cycle 11: unlock completion parity. After a successful unlock of the
  selected drive's LUKS component the Unlock button reads "Already
  Unlocked" and is disabled (modern parity; it returns to the normal gating
  on another selection or a rescan, and stays "Unlock" with the retry path
  on any failure). The helper now emits `UNLOCKED_ROOT=<component>` (plus
  `UNLOCKED_ROOT_FSTYPE=<type>`) after the unlock — a read-only blkid probe
  over the opened mapper chain, never a mount — so the GUI can enable Select
  Target with the helper-confirmed mapped root that its read-only inventory
  cannot see (no line keeps the existing fail-closed behavior). The Authorize
  button and status label now appear only in the deferred-authorization state
  (a ready scope without an active session), like the modern frontend.

- Cycle 10: ASCII-only rendering, the desktop-elevation unlock channel and
  the /host log fetch. Every new cycle-9 UI string (host-card identity/storage
  line, check indicator, repair summary) now renders with ASCII-only markers
  (`|` separators, `[OK]`/`[FAIL]`/`[-]`, `==== REPAIR ====` brackets) because
  Etch's fonts garble the modern glyphs. The LUKS unlock passes `--key-owner
  <uid>` with the mode-600 keyfile, so gksu/gksudo elevation (no SUDO_UID)
  accepts the GUI's file while foreign-owned files still fail. The Logs
  Save As... dialog starts in a writable /host mount when one exists
  (fallback: the log directory). Every
  long confirmation (Full Repair, individual repairs, Make Default, unlock,
  config write, host-shell command, copy-and-verify, log deletion) now uses a
  shared width-constrained wrapped dialog instead of an unwrapped message
  box.

- Cycle 9 unlock channel and smoke isolation: the LUKS passphrase now travels
  through a mode-600 keyfile argument (the GUI writes an O_EXCL-created file
  in its log directory, never argv, and deletes it on every path; the helper
  accepts `unlock --key-file <path>`, verifies a regular caller-owned file,
  reads it with the newline-tolerant key handling and deletes it before the
  open attempt; the stdin channel remains for scripted callers). Qt 3.3.7's
  QProcess cannot deliver stdin, so the helper runner's stdin channel is
  unused by design. `--smoke-test` now runs with settings isolation: the
  user's persisted ~/.qt/devicesrc|logsrc|diagnosticsrc|repairrc overrides are
  never read or written during the smoke and the in-code defaults apply, so a
  persisted override can never flip a smoke assertion.

- Cycle 9: the protected running-host card and the small-parity set. The
  Systems page now leads with a permanently-protected host card (green-check
  indicator, probe-based identity/storage line, PROTECTED badge, and the
  Details / Host Maintenance / Make Default actions on its right; Refresh
  Devices stays top-right and the candidate list moves below the card). The
  Details button opens a read-only dialog that reuses exactly the details
  rows the Systems pane shows. The device tree starts collapsed (disks rolled
  up; expand with +). The GUARDED REPAIR badge now reaches the window's right
  edge. The Diagnostics results pane keeps minimum sizes so Copy Results /
  Save Results never disappear when the window shrinks, and the per-check
  button reads "Re-run Diagnostic" once cached results exist for the current
  scope+key. The Diagnostics/Repair/Shell/File Copy scope label renders the
  modern two-line form (Host maintenance: / Target: over the disk path).
  Settings persistence is confirmed per-user with a Settings-page note
  naming the actual Qt 3.3.7 files: each settings group lives in its own file
  under ~/.qt/ (devicesrc, logsrc, diagnosticsrc, repairrc); the GUI never
  elevates itself, so the invoking user owns the files and a root-launched
  GUI keeps its own copies.

- Cycle 8: Full Repair diagnostic regeneration, shell apt-intent translation
  and the modern File Copy page. Running the Full Repair plan now invalidates
  the scope's cached diagnostics once after the last plan stage (success or
  aborted, mirroring the modern finishFullRepairPlan) and schedules one quiet
  Run All when the auto-refresh setting, the administrator session and the
  idle state allow it (log: "Automatic read-only diagnostics regeneration
  scheduled after Full Repair."); individual repairs keep their stale-based
  scheduling. The legacy shell paths translate a reviewed `apt
  update|upgrade|full-upgrade|dist-upgrade|install|remove|purge|autoremove|
  clean|autoclean` intent to the Etch apt-get equivalent (`full-upgrade` ->
  `dist-upgrade`) with an "apt intent translated:" log line; anything
  ambiguous runs unchanged. The repair result popup actually wraps now: Qt
  3.3.7's LogText mode disables word wrap entirely, so the popup uses
  PlainText plus a defensive wrap re-application while it streams. The File
  Copy tab matches the modern page: heading row (title | live scope label |
  Preview Changes | Copy and Verify), direction row, "1. Select source..."
  group with the Add/Remove/Clear button row, "2. Choose destination..."
  group with Browse Target Folders... (helper browse-target records, raw
  percent-encoded names because Qt 3.3.7 lacks QByteArray::fromBase64) and
  "3. Ownership and copy policy" with the Smart/Preserve ownership combo.

- Cycle 7: Systems tree + unlock gating + repair summary, and the legacy
  capability wave. The Systems tab now shows a device tree (disks as
  top-level items with partitions/mappers as indented children; Device/Size/
  Type/Filesystem columns, expandable/collapsible) instead of the flat list,
  and a drive with a locked LUKS component no longer resolves its unencrypted
  /boot partition as the root: Select Target stays disabled with "Unlock the
  encrypted volume first; Select Target becomes available after a Linux
  filesystem is detected." until the unlock exposes the mapped Linux root.
  The Unlock status pane keeps the modern field order (State / Component /
  Mapper / Method, plus the exact last-error line). After every repair run
  the register gains a `──────── REPAIR ────────`-bracketed summary block
  (✓ changed / ✗ failed / ▪ no-repair-needed per stage plus headline counts,
  Repair-tagged so the Repairs filter shows it; a stage with no parsed status
  counts as failed, never as success). The capability wave lands the helper +
  GUI for the Chroot Shell / Host Shell tab (offline shell through the
  guarded plain chroot, host shell through the legacy direct path on BIOS
  hosts; both probes report available on Etch), the File Copy tab (direction
  combo, staging, Preview Changes / Copy and Verify through the cp -a +
  chown --reference + cmp backend), the Boot stack reconciliation tool (the
  guarded mapper/crypttab + initramfs + GRUB-legacy one-pass stage, kept out
  of the plan) and Make Default beside Host Maintenance (menu.lst
  `default <N>` ensure with backup/rollback, gated by Host Maintenance + the
  cached host-default probe + the session).

- Fix the Etch host-relative device naming in target fstab/crypttab
  resolution. Etch-era entries name devices with bare paths (`/dev/hda1` as
  seen from the installed system), which on the repair host point at the
  repair host's own disk and made `same_single_top_disk` refuse a valid
  target reference (`/boot resolves outside the selected target disk`). The
  legacy port now remaps such a source onto the selected target disk
  (`/dev/hda1` → `/dev/hdb1` on a `/dev/hdb` target, only when the target
  partition exists) before the guard runs; UUID=/LABEL=/mapper/by-id forms
  pass through and the same-disk guard still re-validates the remapped path.
- Relax the legacy root-evidence probe for split-mount Etch roots. The etch2
  target keeps its dpkg database on a separate `debian-var` LV, so the old
  probe (`/etc/debian_version` + dpkg status) refused a correctly unlocked
  root; the legacy port now confirms an os-release-less root from
  `/etc/debian_version` paired with any one of the dpkg status pair,
  `/etc/apt/sources.list` or `/etc/inittab` — all `/etc`-resident, so the
  probe stays correct with separate `/var` and `/usr` mounts (the
  `/etc/redhat-release`+rpm and `/etc/SuSE-release`+rpm chains are unchanged;
  the modern helper keeps its strict os-release probe for modern roots).
- Fix two Etch follow-ups on the repair-target flow. Read-only ext mounts no
  longer use `noload`: Etch's util-linux 2.12r rejects `ro,noload` on ext3
  (`ext3: No journal on filesystem`), so the legacy helper strips the option
  for every ro ext mount (root, boot entry and the os-release probe all go
  through the same filter) and mounts plain `ro` — a dirty-journal ro mount
  failing stays fail-closed. After a successful LUKS unlock the helper now
  runs `vgscan --mknodes` + `vgchange -ay` best-effort (logging
  `LVM scan/activation after unlock: N logical volume(s) activated`) so the
  target's LVM logical volumes appear on the GUI rescan and the root
  auto-resolves; the `UNLOCKED=`/`UNLOCK_AUTH_FAILED=1` markers are unchanged.
- Fix the legacy helper's LUKS unlock for Etch's cryptsetup 1.0. The modern
  unlock path calls `cryptsetup open --type luks --key-file -`, which does not
  exist in cryptsetup 1.0 (`--type: unknown option`); the legacy helper now
  opens the selected LUKS component with the 1.0 action
  `cryptsetup --key-file - luksOpen <device> luks-<uuid>` while keeping every
  modern preflight — the protected-host and same-disk asserts, the
  `cryptsetup isLuks` verification, the `luks-<uuid>` mapper naming, the
  existing-mapper reuse, the mapper-name-collision refusal, the rc-2
  passphrase-retry marker and the `UNLOCKED=<mapper>` output are all
  unchanged (the GUI needs no change: it consumes the same markers).
- Harden the legacy GUI's settings persistence and polish three panels.
  Every user toggle — the three device-discovery filters, the wrap toggle
  (Settings checkbox and View menu), the diagnostics auto-refresh checkbox
  and the six Full Repair plan checkboxes — now persists into one canonical
  per-user file (`~/.qt/boot-bitchrc`; the previous organization/application
  pairing scattered the keys across per-subkey files so overrides did not
  survive a restart), every handler saves immediately, window close flushes
  again, and `dpkg -r`/`dpkg -i` reinstalls never touch the file. The
  Settings host-capability table now selects whole rows (`QTable::SingleRow`,
  first row selected) — the selected missing capability is the conceptual
  guard the disabled **Install Missing Support...** documents. The selected
  drive details panel always shows all ten modern rows (Model/label with the
  owning-disk fallback, UUID with the helper-confirmed root fallback, joined
  Mounts; unknown values render as `-`), and the Available repair targets
  Filesystem column is never empty (mounted/udev-probed/`[swap]`/`unknown`
  for components, whole disks aggregating their children as `ext3 + LUKS`).
  The repair result popup continues to follow the wrap toggle at creation
  (re-verified; an unwrapped popup means a stale build is installed).
- Extend the legacy GUI's diagnostics auto-refresh and add the header busy
  indicator. With **Automatically regenerate read-only diagnostics** on, one
  quiet Run All is now also scheduled after entering Host Maintenance and
  after committing a repair target (both change the scope identity), and
  after any repair whose change status invalidates the cached diagnostics —
  on top of the existing unlock/target-config-edit paths — whenever a scope
  is ready, no command is running and the administrator session is active;
  it never opens an authorization prompt by itself and logs the exact
  skip/pending reason otherwise. A reserved-slot **Working...** busy
  indicator now appears in the header row (next to the GUARDED REPAIR badge)
  only while a helper command runs; its fixed width keeps the header layout
  stable and the smoke layout gate covers it.
- Make the two Etch "unavailable" repairs work: the initramfs tool and the
  graphical-login tool. The legacy helper gains a guarded plain-chroot
  fallback (`legacy_chroot`): on BIOS-only hosts there are no EFI firmware
  variables to isolate, so the host initramfs/GRUB/display-manager stages run
  through a plain `chroot` with the existing mount/preflight discipline
  (mapper/crypttab preflight, trial builds, backups and session unmount
  cleanup all unchanged) and `Legacy feature host-maintenance:` now reports
  `available` instead of `unshare is not installed in the recovery
  environment`; an EFI host keeps the fail-closed unshare guard, and
  `shell`/`host-shell` keep their strict timeout/unshare gates. The display
  capability is now probed for legacy SysV targets: a configured
  `/etc/X11/default-display-manager` entry (kdm/gdm/xdm/...) with an
  executable binary and an init script emits `Repair tool display: available`
  plus a `legacy SysV:` evidence line, and a new guarded `display-manager`
  stage (host scope; dispatched through `host-repair`) restores the entry and
  the missing runlevel S-symlink with a backup and rollback, never starting
  the GUI or touching unrelated services. The GUI's display tool row is now
  runnable ("Restore Graphical Login", stage `display-manager`, host-only:
  the offline target form stays disabled with the host-scope reason and the
  tool stays out of the six-stage Full Repair plan). `unshare` remains a
  displayed-missing capability row whose note documents the fallback. The
  contract/smoke/parser tests cover the fallback gates, the display probe and
  repair (entry restore, symlink creation, refuse-on-missing-binary and
  rollback) and the updated Etch evidence fixture.
- Finish the legacy Qt3 GUI parity round (cycle 4a). The Repair page now keeps
  the **Full Repair plan** frame and the **Individual repair tools** splitter
  in a draggable vertical splitter (like the modern page), the repair result
  popup follows the Settings **Wrap long log lines** toggle while staying
  monospace, and every section title expands horizontally with `Qt::AlignLeft`
  so its text starts at the same left edge as the frame content below it (the
  smoke locks this in with a per-title left-edge geometry probe). The Logs tab
  replaces the all/errors filter with the modern 1:1 kind filter (**All
  entries** / **Diagnostics** / **Repairs** / **File system repair** /
  **Package repair** / **File copy** / the 16 diagnostic section titles):
  every register entry is tagged at capture time — lines while a diagnostic
  runs carry the section key from the stream's `Diagnostic: <key>` markers,
  lines while a repair runs carry the helper stage (the Full Repair run
  carries its stage set), fs-inspect maps to File system repair and
  fix-broken/dpkg-configure/apt-update/apt-upgrade map to Package repair, and
  File copy stays for the 1:1 order with no lines in this frontend; Save As
  always writes every entry. **Save As...** and **Clear Register** moved into
  the top row right of the **Application log** title, above the search row.
  The About tab is gone (Help -> **About Boot Bitch** opens a rich-text
  dialog with the modern heading **Boot Bitch <version>** and **Developer:
  CaptainMorgan12** and a legacy-adapted guarded-repair description), the
  tab count drops to seven, and the legacy window/app icon is now the real
  Boot Bitch artwork (the `make-icons.py` script resizes the modern master
  PNG with LANCZOS into the 16/22/32/48 shipped sizes and fails fast when the
  master is missing). Settings -> **Host capabilities and dependencies**
  mirrors the modern structure: Distribution / Package manager family /
  adapted authorization-support summary labels, the 6-column **Feature |
  Command | Scope | Status | Suggested package | Notes** table with the modern
  23 probe rows (read-only PATH searches, never executed) plus the legacy
  `Process namespace isolation / unshare` row, **Refresh Capabilities** and
  the disabled **Install Missing Support...** button with the modern tooltip.
  The smoke controls/layout gates cover the new splitter, the capability
  table, the 1:1 filter combo and the section-title left edge; the contract
  test asserts the seven tabs, the About-tab absence, the filter/table/icon
  markers and the regenerated icon files.
- Rebuild the legacy Qt3 Repair tab to mirror the modern page and fix the
  clipped section titles. The Repair tab now opens with the modern plan
  paragraph, then the **Full Repair plan** section (selected-stage count, the
  numbered stage list, the modern readiness text, **Configure Plan...** which
  switches to Settings, and **Run Full Repair**), followed by the 13 modern
  individual tools as a list (columns **Tool** | **Full Repair**) with a
  **Selected tool** pane (dynamic title, per-tool run button, description and
  plan status). The legacy-runnable tools (Validate, Check File Systems,
  Repair Dependencies, Complete Configuration, Refresh Metadata, Simulate and
  Upgrade, Rebuild Initramfs, Regenerate GRUB) keep their exact helper command
  set and fail-closed gating; DKMS, display, EFI, extlinux and boot-stack are
  display-only rows whose run button stays disabled with the exact reason, and
  the old "Modern features not available" section is gone. The Full Repair
  plan runs the six legacy stages (dpkg-configure, fix-broken, apt-update,
  apt-upgrade, initramfs, grub) in helper rank order in one invocation, driven
  by the new Settings **Full Repair plan** checkboxes (modern labels,
  persisted under the modern `repair/*` QSettings keys); Run Full Repair is
  enabled only with a ready scope, an active administrator session, fresh
  cached diagnostics and every selected stage's capability line `available`
  (plus the host-maintenance probe for host initramfs/grub). Starting an
  individual tool or the plan opens a modern-style result popup (tool title, a
  bold status line, the streamed helper transcript and a Close button enabled
  on finish) while the Logs tab keeps the complete transcript. Every tab's
  group frame is now titleless with a `sectionTitle` label because the Etch
  Qt3 style clips `QGroupBox` titles at the top, the Repair page scrolls like
  Settings, and the smoke layout assertion checks the section titles for
  clipping. The `--smoke-test` controls/layout gates cover the new Repair
  widgets, the plan/tool behavior and the display-only reasons, and the layout
  gate now iterates every tab (a clip on a non-active page fails the smoke);
  the Selected tool pane keeps a 300px floor and its dynamic title wraps with
  the Qt3 `Qt::WordBreak` alignment, and every tool title plus the placeholder
  is re-verified against the pane at 1024x768. The display-only plan status
  no longer starts with a cosmetic blank line.
- Complete the legacy Qt3 GUI's modern-GUI parity for menus, Logs, Settings
  and the Diagnostics check list. The menus now mirror the modern structure:
  **File** carries **Refresh Devices**, **Lock Administrator Session** (drops
  the cached elevation decision with a best-effort non-blocking `sudo -k`,
  logs the action and refreshes the inventory, so the next privileged action
  asks for the password again) and **Quit**; **View** carries the **Systems**,
  **Diagnostics**, **Logs** and **Settings** tab shortcuts plus **Auto-size
  Device Columns** and the checkable **Wrap Log Lines** (kept in sync both
  ways with the Settings wrap toggle); **Help** carries **Using Boot Bitch**
  and **About Boot Bitch**. The Logs tab gains the modern session management:
  **Save As...**, **Clear Register** (live register/view only), **New Session
  Log** (closes the active file; it becomes a prior session), **Add Note**
  (`NOTE: <text>`), **Delete** (selected prior file only, confirmed, never the
  live file or anything outside the log directory) and **Refresh**, plus a
  read-only prior-session banner. The Settings tab makes the three
  device-discovery filters functional with the modern rules (a hidden drive
  hides its partitions/mappers, an encrypted drive stays visible while "show
  encrypted" is on, and the selected/committed target state stays valid when
  its row is hidden), makes the diagnostics auto-refresh toggle functional
  (after a LUKS unlock or target configuration edit, only with an active
  authorized session and no running command; it never opens an authorization
  prompt by itself) and adds the read-only **Host capabilities and
  dependencies** group (Distribution, package manager, service manager,
  display manager, initramfs, bootloader and logging backends from the cached
  diagnostics) with **Refresh Capabilities**; the filters, wrap toggle and
  auto-refresh toggle persist through Qt3 `QSettings` under the modern key
  names, the page scrolls like the modern one, and no install button is
  offered because host installs are forbidden. The Diagnostics check list now
  shows the modern friendly titles while the stable helper keys stay internal
  for the command and the log. The `--smoke-test` assertions cover the new
  menus, Logs controls, Settings filters/capabilities and diagnostic titles at
  1024x768.
- Bring the legacy Qt3 GUI to modern-GUI parity and fix the reported
  functional defects. The window now carries the modern global header (packaged
  icon, "Boot Bitch" in the 1.65x bold title font, the "Linux recovery and
  boot-repair utility" subtitle and the right-aligned "GUARDED REPAIR • 0.2.25"
  framed badge) and section-title headings on Systems, Diagnostics, Repair,
  Chroot Shell, File Copy, Logs and Settings. Systems replaces the
  "Devices - read-only kernel inventory" frame, the scope/target combo grid and
  the "Set as repair target" wording with the modern page: an "Available repair
  targets" list, an action row of **Select Target** / **Unlock** / **Host
  Maintenance** plus the deferred **Authorize** control and the right-aligned
  "Committed target:" summary, the **Unlock status** frame directly below it and
  the **Selected drive details** pane. Drive selection comes from the list row
  and the best Linux root component is auto-resolved from the read-only
  inventory plus udev metadata (mapper with a Linux filesystem first, then a
  Linux partition, then the disk itself); the protected running host is shown
  as `PROTECTED` and its Select Target/Unlock tooltips name Host Maintenance.
  Unlock auto-resolves the first locked `crypto_LUKS` component on a non-host
  drive (read-only udev `ID_FS_TYPE` probe, never a block-device read) and
  opens it through the helper's stdin-only passphrase flow; a locked LUKS disk
  without a visible Linux filesystem stays "Unlock required before selection".
  Diagnostics now matches the modern page (heading + scope label + **Run All**,
  a single-column **Diagnostic checks** list, the **Selected diagnostic** pane
  with title/description/availability, a **Results** heading with the
  right-aligned **Run Diagnostic**, and **Copy Results** / **Save Results...**
  below the pane); the "Cancel running command" button and the "Reads" column
  are gone (window close keeps the runner's cancel path), and the target
  configuration row is unchanged and still target-only/probe-based. Chroot
  Shell switches its heading, tab label ("Host Shell"), notice and button
  ("Run on Host") with Host Maintenance exactly like `updateChrootShellMode`,
  and the individual Repair tools now use the modern button wording
  (Validate, Check File Systems, Repair Dependencies, Complete Configuration,
  Refresh Metadata, Simulate and Upgrade, Rebuild Initramfs, Regenerate GRUB).
  Authentication carries forward: the cached `sudo -S -v` session is probed
  non-blockingly (stdin closed) before every privileged command, an expired or
  refused session fails closed with the explicit **Authorize** remedy instead
  of asking the user to re-commit the target, and only a transcript that
  actually shows a sudo session problem drops the cached decision. The
  `--smoke-test` layout/control assertions cover the new widget set (header,
  section titles, action row, scope labels, Selected diagnostic pane,
  Copy/Save, host-mode shell labels) at 1024x768. Validated on the real Etch
  guest with `local-refresh.sh --legacy-vm`: `SMOKE OK: ... controls: ok;
  layout: ok (54 widgets checked)`, the extended Qt3 auth harness reports
  `AUTH-PIPE OK` plus `SESSION-EXPIRY OK` (the expired cached sudo is detected
  non-blockingly and `resetElevation()` restores the Authorize path), the
  in-guest `legacy GUI contract: PASS`, and the guest udev record confirms the
  `crypto_LUKS` probe used by Unlock. The first guest build also caught a
  clipped runtime button and a Qt3 layout double-parent crash, both fixed.
- Align the legacy Qt3 GUI with the modern Boot Bitch layout and fix the
  reported Diagnostics/modal defects: the Diagnostics tab no longer shows the
  "Repair tool / State / Reason" capability list or the availability filter
  (the modern page has neither; the cached capability lines still gate the
  Repair tab) and now uses the modern Run All/checks/results structure; the
  **Edit Target File...** row is shown only for a committed repair target and
  never for Host Maintenance, lists the Etch-era files (`fstab`, `inittab`,
  GRUB-legacy `menu.lst`, `crypttab`, `modules`, `interfaces`, `sources.list`,
  `apt.conf`) and greys/omits absent files with the helper's read-only
  `Legacy config <key>:` probe reason (the legacy helper gained those guarded
  `config-read`/`config-write` keys and the probe report; a saved edit
  invalidates diagnostics); the long hidden-input administrator/LUKS modals
  are now width-constrained with word-wrapped text; and Systems/Repair/Settings
  group order plus the unlock-status placement were aligned with the Qt6
  pages (comparison note in `legacy/gui/README.md`). Validated on the Etch
  guest with the native qmake-qt3 build and the Xvfb `--smoke-test`.
- Automate the legacy Etch build+install workflow and prove user access:
  `scripts/package-legacy.sh --install-vm` (Etch guest only, run as root)
  removes the previously installed package with `dpkg -r`, installs the new
  artifact with `dpkg -i` and verifies the installed GUI/helper sha256 hashes,
  the 0755 binary/0644 desktop-entry modes and the PATH-resolved desktop entry
  against the artifact payload; off-Etch it keeps the clear
  "Nothing was built or installed." refusal. The maintainer path
  `Development/scripts/local-refresh.sh --legacy-vm` drives the whole flow
  over the Etch pty serial console (offline guestmount `/boot` source
  transfer, LUKS unlock + login, in-guest build/install, unprivileged
  `Xvfb --print-config` and `kbuildsycoca --menutest` user checks, `.deb`
  copy-back and host-side installed-vs-artifact hash comparison). The legacy
  package contract also asserts the user-accessible modes now.
- Fix the reported legacy Qt3 GUI defects: remove the Systems `Scope:`
  dropdown (scope follows the committed repair target or Host Maintenance, so
  any selectable non-running-host disk — including a second Etch disk — can be
  committed) and never add a standalone authenticate control; request the
  modal hidden-input `sudo -S -v` authorization once per scope on Host
  Maintenance entry or target commit, cache it for the session and never
  prompt on Run All; fix the crash where a password write to a sudo that
  already held a valid timestamp (and exited without reading stdin) killed the
  GUI with SIGPIPE; wire the Chroot Shell tab to the helper's guarded
  `shell`/`host-shell` verbs behind the scope probe plus the cached session
  (greyed with the exact reason otherwise); keep the protected running-host
  disk excluded from commit and unlock; and raise the title/header font-metric
  layout floors and stretch the last evidence column to the window edge.
  Validated on the Etch guest: Xvfb smoke `controls: ok; layout: ok
  (59 widgets checked)`, the Qt3 `auth-pipe` harness (`AUTH-PIPE OK`, pre-fix
  runner exits 141) and an XTest-driven modal-auth/Run All drill
  (`MODAL-AUTH DRILL: PASS`).
- Stretch the final visible column of the modern GUI tables to the viewport
  edge: the Repair individual-tools "Full Repair" column and the Settings
  capability table's "Notes" column now fill the window's right edge instead
  of leaving an empty gutter, while every section stays `Interactive` and
  draggable, the header-click sorting is unchanged, and a narrow pane shrinks
  the trailing column to its minimum rather than forcing a horizontal
  scrollbar.
- Add the Qt3 legacy frontend for Debian Etch / KDE 3.5-era systems:
  `legacy/gui` is a Qt3-only C++98 frontend with the modern Boot Bitch layout
  (Systems Selected drive details, diagnostics/log filters, LUKS Unlock over
  the helper's stdin, modal `sudo -S -v` authorization, greyed modern-only
  tabs), fail-closed gating from the helper's probe-based `Legacy feature
  <key>:` evidence, per-key diagnostic runs, a read-only config-read viewer
  and the guarded GRUB-legacy action. `scripts/package-legacy.sh` builds and
  packages it with `libqt3-mt` and drops the shell-only TUI launcher; the
  generated bash-3.1 helper resolves the kernel 2.6.18 `/sys/block` layout,
  partitions by the `start` attribute, major:minor mapper aliases and
  `dmsetup` output; the legacy tree ships in the Alpine and Arch source
  archives so their `check()` phases run the legacy contracts.
- Extend Make Default across every supported boot chain: firmware entries are
  identified by PARTUUID plus decoded loader path (never by label) and
  selected-ESP labels are annotated with the drive model when present; Fedora
  BIOS `saved_entry` is written only for the running non-rescue BLS id with
  every other grubenv key and the rescue entry preserved (byte-identical
  rollback on failure); the Alpine extlinux default is ensured with a
  config-only `overwrite=0` trial, entry-preservation guard, MENU DEFAULT
  verification, byte-identical backup/rollback and read-only stale-default
  diagnostics. The fast host-default contract covers the cross-distro matrix.
- Auto-mount a configured but unmounted ESP before the writability preflight
  refuses, and probe the effective (topmost) mount row instead of a reused
  mount ID: a fail-closed `esp_writable_preflight` clears only read-only
  layers of the selected ESP (idempotent, logged, foreign/rw layers
  untouched) or refuses with the exact cleanup command; pre-existing
  systemd/foreign boot-entry mounts are recorded and never claimed,
  unmounted or remounted; post-cleanup leaks are verified, reported as
  `MOUNT_LEAK` evidence and persisted instead of swallowed.
- Generate the read-only File systems section in the combined `diagnose all`/
  `host-diagnose all` report (fs-inspect runs last, after the helper's
  read-only mounts are released) and cache it on scope entry and target
  selection like every other section; an individual re-run is merged into the
  cached Full report instead of replacing it, and only a successful capture
  replaces its section, so the Full report stays coherent and otherwise shows
  the explicit "re-run all diagnostics" notice.
- Make the AppImage build portable across modern distributions:
  `scripts/build-appimage.sh` probes the payload for SHT_RELR sections,
  prefers a system `patchelf` 0.18 or newer, sets `NO_STRIP=1` when
  linuxdeploy's bundled strip cannot parse RELR and
  `APPIMAGE_EXTRACT_AND_RUN=1` on musl or FUSE-less hosts, and fails with
  install instructions only when no RELR-capable `patchelf` exists; the
  validated matrix covers Arch, Fedora 44, Alpine 3.24 and Debian 13.
- Debian-native support and UI fixes: normalize efibootmgr 18's
  `File(\EFI\...)` device-path form so canonical-loader identity, labels and
  BootOrder reconciliation work on Debian 13 and keep the rollback's output
  off stdout; require a real apt candidate before installing optional
  development packages; make the missing-Polkit hint follow the detected
  backend family and make Authorize re-check the live session (no silent
  no-op, no cross-distribution package names); unlock the Individual repair
  tools and capability column dividers with content-derived defaults, and use
  one Select Target eligibility predicate (blank/non-Linux data disks
  selectable for inspection, locked LUKS unlock-only, optical/live media
  excluded, running host protected).
- Packaging and CI hardening: `umask 022` in the build/packaging wrappers,
  sanitized Arch `.BUILDINFO`, signed-APK and world-traversable-RPM hygiene
  gates, Alpine/apk dev-environment support, and the public-tree hygiene
  guard that runs in CI.
- Fix Debian 13/trixie log filtering and Arch package ownership metadata: the
  helper's `package_log_filter` no longer uses unbounded awk interval
  expressions (mawk 1.3.4 masks short hex runs and drops real error text); the
  backend-profile contract fails on any `{n,}` interval and pins a fixture
  where long hashes are masked while short hex words survive; the sanitized
  Arch package forces `--uid 0 --gid 0` in its regenerated `.MTREE` and fails
  the build unless the metadata records root ownership, so `pacman -Qkk` no
  longer flags every installed file.
- Fix the AppImage file dialogs (File Copy → Add Folder) listing no folders
  and resolving sidebar places such as Documents against the process working
  directory: the image loads the build host's Qt platform theme, whose native
  KDE/KIO (or GTK/portal) file dialog needs worker and portal services the
  image does not bundle. The GUI now forces Qt's own file dialog whenever it
  runs from an AppImage (application attribute plus explicit
  `DontUseNativeDialog` options on every `QFileDialog` call), keyed off the
  AppImage runtime's mount so an installed build launched from an AppImage
  terminal does not inherit the policy, while installed builds keep their
  native dialogs. The UI suite pins the policy and a non-empty listing with
  absolute sidebar paths, and `scripts/test-appimage-contract.sh` guards the
  policy fragments.
- Enforce per-drive EFI entry policy and rank grouped `BootOrder` entries by
  boot-use class before drive: the boot-stack reconcile keeps exactly one
  managed UKI, fallback and WebFAI destination per selected drive and at most
  one shim entry, relabels/reuses existing entries instead of recreating
  them, preserves firmware-created device-path records exactly and leaves
  foreign-drive entries untouched; each drive's primary destinations (UKI,
  then the managed fallback and the firmware-owned fallback-device-path
  record) stay contiguous, the WebFAI recovery entries follow drive-major,
  then the remaining managed entries (shim, vendor loader, other), removable
  no-PARTUUID records are always last, and drives keep host-first first-seen
  order. The read-only EFI inventory annotates firmware-created device-path
  options (for example a USB stick's `UEFI: <media>, Partition 1` record) as
  no-managed-OS entries preserved untouched and reports a per-drive
  `removable` count. Repair transcripts collapse the verbose
  tpm2-tools/LUKS enrollment dumps (tpm2-* fields, hex blobs, keyslot and
  digest details) into one summary line while passing every error, warning
  and unrelated line through unchanged.
- Fix the AppImage administrator-authorization failure where Unlock and Host
  Maintenance reported `Permission denied` for
  `/tmp/.mount_*/usr/libexec/boot-repair/boot-repair-helper`: an AppImage is
  served through a FUSE mount that only the user who mounted it may read, so
  pkexec (root) could not open the bundled helper even though it was mode
  0755. The GUI now stages a sha256-verified private copy of the helper
  (mode 0700, in `$XDG_RUNTIME_DIR/boot-repair/`) whenever the resolved helper
  lives on an AppImage/FUSE mount or a noexec filesystem, hands that path to
  pkexec, logs the copy and the reason, and removes it when the window closes;
  installed `/usr/libexec` helpers keep their existing behavior.
  `scripts/build-appimage.sh` re-asserts mode 0755 on every bundled executable
  (before and after linuxdeploy) so the squashfs can never ship a
  non-executable helper.
- Add AppImage update metadata for Gear Lever/AppImageUpdate:
  `scripts/build-appimage.sh` now embeds a `gh-releases-zsync` update
  information string and writes the matching
  `boot-repair_<version>_x86_64.AppImage.zsync` next to the AppImage (via
  `zsyncmake`, with a clear failure when it cannot be found instead of
  silently omitting the update metadata), and the release tooling captures,
  mirrors, checksums and verifies the `.zsync` alongside the AppImage so the
  GitHub source is detected automatically.
- Fix the bundled icon atlas falling back to generic style icons on Debian
  trixie and newer: Debian moved Qt's SVG image-format plugin out of
  `libqt6svg6` into `qt6-svg-plugins`, so the `.deb` now depends on
  `qt6-svg-plugins | libqt6svg6 (<< 6.7.2-5~)` (bookworm and Ubuntu 24.04
  keep the plugin inside `libqt6svg6`). `scripts/check-icon-atlas.sh` now
  audits that SVG-plugin dependency across the deb, RPM, Arch and APK package
  profiles, and `scripts/test-deb.sh` / `scripts/test-rpm.sh` assert it in the
  built package metadata.
- Bundle the AppImage's runtime Qt integration and guard it with a contract:
  `scripts/build-appimage.sh` now forces the Qt SVG icon engine
  (`EXTRA_QT_MODULES=svg`, otherwise the SVG icon atlas silently degrades to
  generic style icons) and bundles the build host's Qt platform themes and
  widget styles (`DEPLOY_PLATFORM_THEMES`, on by default) so dialogs and file
  pickers match the desktop; it fails when the icon engine is missing, warns
  non-fatally on non-Debian hosts and when no platform theme was bundled, and
  TUXEDO OS is documented as the reference AppImage build host. The new fast
  `scripts/test-appimage-contract.sh` asserts the embedded `.upd_info` string,
  the `.zsync` `Filename`/`URL`/`SHA-1` pairing, the bundled executable helper
  and the helper-staging messages; `verify-release.sh` re-checks the bundled
  icon engine, platform theme and helper in the capture.

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
