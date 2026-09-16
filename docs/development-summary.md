# Boot Bitch development summary

Boot Bitch 0.2.15 is the first public release after the 0.1–0.2 development series. Versions before 0.2.15 were development iterations and are retained as historical notes rather than public releases. Earlier versions were hardware and workflow candidates; this summary groups the changes by capability for readers who do not need the full version-by-version changelog.

## Recovery foundation

- Added a native Qt 6 Widgets application with a responsive Systems, Diagnostics, Repair, Snapshots, Chroot Shell, File Copy, Logs and Settings workflow.
- Added physical-device ranking, Linux-root detection, partition inspection, filesystem identification and persistent committed-target selection.
- Added mandatory running-host protection in both the GUI and privileged helper. The helper traces device-mapper ancestry back to physical disks before allowing target access.
- Added isolated `/run/boot-repair/session.*` target mounts with reverse-order cleanup.

## Privilege and encrypted storage

- Added a narrow Polkit/pkexec helper session while keeping the GUI unprivileged.
- Added explicit administrator-session locking and cleanup of only mappings opened by Boot Bitch.
- Added LUKS unlock through helper standard input; passphrases never appear in arguments or logs.
- Added reuse of existing LUKS mappings, conventional `luks-<UUID>` names, mapper/crypttab compatibility aliases and bad-passphrase retry handling.

## Diagnostics and evidence

- Added read-only target diagnostics for environment, boot state, boot evidence, kernels/initramfs, GRUB, EFI/UKI, graphical login, errors, usage, `fstab`, Btrfs, mapper and LUKS/crypttab.
- Added a read-only distribution/backend profile that distinguishes Debian/APT
  and Arch/pacman systems, detects mkinitcpio/dracut/initramfs-tools,
  GRUB/systemd-boot/generic UKI layouts, `/efi` versus `/boot/efi`, and common
  kernel naming conventions. Arch modifying actions use transaction-specific
  pacman, mkinitcpio, GRUB and EFI preflights; conventional EFI repair also
  restores a single verified vendor-loader entry when grub-install only leaves
  files on the ESP. Unsupported stages remain gated.
- Added **Run All** to populate every per-diagnostic cache through one read-only target session.
- Added capture timestamps and cache identity based on physical disk and root filesystem.
- Added authoritative cache invalidation after target edits, repairs, snapshot rollback, Host → Repair copies and shell commands that may modify files.
- Added stale evidence markers in Diagnostics and Logs, with clear guidance to rerun the affected diagnostic.
- Added direct Results-pane output for individual diagnostics and consolidated output for Run All.

## Repair stack

- Added ordered Full Repair stages and matching individual repair tools with Settings parity.
- Added adaptive APT repair: simulation chooses `upgrade`, `full-upgrade` or `dist-upgrade` according to target policy and rejects unsafe transactions.
- Added package configuration, broken-dependency repair, metadata refresh and post-upgrade `dpkg` auditing.
- Added DKMS header/build preflight, guarded rebuild and post-repair verification.
- Added initramfs trial builds, mapper/crypttab gates, archive verification and kernel pairing checks.
- Added GRUB temporary generation, syntax checking, guarded update and retry for recognized stale mapper paths.
- Added distribution-aware EFI/UKI handling, ESP validation, vendor UKI rebuilding, firmware-order preservation and conventional GRUB EFI repair.
- Added boot-stack reconciliation for partial repairs and snapshot root changes.
- Added Full Repair freshness gating so a plan cannot run against missing or stale matching evidence.

## Graphical login recovery

- Expanded graphical-login inspection beyond one desktop: detect the configured display manager, installed manager packages and units, default target, recent boot evidence and graphics errors.
- Added ambiguity checks and headless `systemd`/`ExecStart` preflight before restoring SDDM, GDM3, LightDM, greetd, ly or a configured custom manager.
- Repairs configure the target offline and never start a graphical session inside the chroot.

## Snapshots, shell and file recovery

- Added asynchronous Snapper-style Btrfs snapshot loading and read-only inspection.
- Added guarded transactional top-level-`@` rollback with preserved previous root, post-switch reconciliation and automatic restoration after critical failure.
- Added the Chroot Shell page with selected root, subvolume, boot, device and temporary resolver setup. Commands run one at a time after confirmation and are recorded in Logs.
- Added bidirectional File Copy with preview, containment checks, ownership policy, no `--delete`, and post-copy checksum/metadata verification.

## Interface, packaging and verification

- Added responsive tab navigation with arrows, stable scrollbar gutters, content-sized rows, adjustable Repair splitters and narrow-window layouts.
- Added theme-neutral palette handling so stale log entries remain readable in KDE and GNOME light/dark themes.
- Added application icon resources, desktop entry, AppStream metadata, man page and Debian packaging with the helper installed at a stable root-owned path.
- Added package-manager-aware setup and installation workflows for APT/dpkg,
  pacman, DNF/RPM and zypper/RPM hosts, plus native Arch, RPM and portable TGZ
  package scripts. Debian package generation remains available through CPack.
- Added development dependency setup, environment checks, UI/contract tests, shell syntax checks and AppStream validation.
- Added anonymized Ubuntu VM screenshots and a disposable VM testing guide.

For the complete historical detail, see [CHANGELOG.md](../CHANGELOG.md).
