# Boot Bitch 0.2.31

The changes in this release are listed below.

Released 2026-11-13.

- Renamed: the package, command, desktop entry, helper path and settings namespace are now `boot-bitch` (`boot-bitch-legacy` for the Debian Etch edition; catalogs are `boot-bitch_*.qm`). The rename ends the collision with the unrelated upstream `boot-repair` package whose higher version once replaced this application during an upgrade; the new packages are fully independent of the old name and the transition is manual.
- Snapshots: a successful target rollback keeps the list and shows the staged banner (no reboot buttons); preserved undo-point names resolve alongside numeric ids and appear in the list; the running-host rollback gains a Cancel that un-stages it, snapshot deletion lands with a guard on the staged source, and a repeated rollback click on the same staged snapshot reports a no-op.
- Interactive: the graphical-login manager repair becomes a walk-through — restore the vendor default (derived from package evidence) or switch to another manager behind a from→to confirmation — and the kernel, graphics, input and network menus regenerate exactly the affected diagnostic evidence at completion. User-supplied local-package installs cover Arch (`pacman -U`) and Alpine (`apk add <file>`) through the same guarded flow as the Debian and RPM verbs.
- Fixes: the Alpine mkinitfs inclusion evidence parse and the modules-file merge; the driver-conflict comma artifact and the honest lspci-without-libkmod note; the zpool capture-first hardening; the target rollback cancel resolves the root's undo point and mounts the promoted root before firmware isolation, undoing the /home swap fail-closed; the TUXEDO UKI post-install hook failure class is tolerated honestly.
- Interface: the Systems drive list hugs its content with a three-row floor; the Logs reflow, elided title and floors; the Diagnostics results floor and scroll-to-bottom fix; the Snapshots finalize and the Settings capability-column tuning; the EFI boot-entries growth cap; the host rollback banner stays a few lines tall at the minimum window size; one reboot confirmation after a running-host rollback.
- Translations and documentation: all 23 shipped catalogs refresh to zero unfinished strings; the GitHub issue-reporting form and the CONTRIBUTING guidance landed; the README opening paragraph is now a stable release-independent description, the install section documents the SHA256SUMS verification and the source-change patch upgrade, and the exact-23 pipeline is documented; the host rollback keeps a Timeshift store untouched.

The Debian package, AppImage, Arch package, RPM, APK and Debian Etch legacy package are built from the
complete source tree and validated in their target environments. Verify
downloaded artifacts against the SHA256SUMS attached to this release. The
AppImage update metadata (boot-bitch_0.2.31_x86_64.AppImage.zsync) is attached
alongside the AppImage so Gear Lever/AppImageUpdate detect the release.

[Full source diff: v0.2.30...v0.2.31](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.30...v0.2.31).
