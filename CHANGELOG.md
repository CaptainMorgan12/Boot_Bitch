# Changelog

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

## 0.2.14 — 2026-09-12

Development release used to validate the guarded repair stack. It introduced
persistent repair transcripts, stale diagnostic highlighting, asynchronous
snapshot loading, compact responsive layouts, tab navigation controls, target
resolver support for chroot networking, and adaptive APT refresh/error handling.
Those changes are included in 0.2.15; development build artifacts and audit
outputs are not part of the release tree.
