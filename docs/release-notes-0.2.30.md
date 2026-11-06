# Boot Bitch 0.2.30

The changes in this release are listed below.

Released 2026-11-06.

- Input-device and network repair walk a ranked driver-fallback ladder through the delta menus, honoring the live-keyboard rule; the no-adapter classifier, the boot-log fallback and per-device firmware evidence with versions keep the verdicts honest. The Boot capabilities table isolates the selected scope, shows Installed/Available/Missing, and fills the Packages cells from a shared per-distribution table plus probes.
- A module-build stage with an empty DKMS/akms/akmods state proves nothing to build and reports unchanged without a package-change confirmation. Kernel evidence names why a repository kernel is not installable, including the gcc-16-for-host blocker class, and the diagnostics are faster: a cached kernel inventory, a re-snapshotted broker, bounded initramfs readers and a re-executing broker cut the preamble stalls.
- Snapshot management is complete: one guarded action installs the missing packages, creates the Snapper root configuration and the first snapshot; the transactional rollback accepts single, pre and post snapshots on any detected root name, captures a separate /boot, optionally rolls a target /home back with the root, keeps the in-window confirmation and automatic restore, and re-rolls the chained replacement.
- Completion-side regeneration and reliability: a clean repair schedules the affected diagnostic regeneration, File Copy verifies checksums, helper-owned mounts are released before session cleanup, and the Fedora stall and menu-answer bound are fixed; declined or cancelled snapshot outcomes stay honest, and the input include re-verify closes on the fresh inclusion evidence and the honest Etch-era rebuild proof.
- The Debian Etch legacy edition keeps full parity: interactive input and network repairs, the read-only EFI boot-entries and boot-menu views, the shared capability-packages table, the live-keyboard fallback, translated interface catalogs, the same confirm and cancel behavior, and a complete popup floor and content-hug contract; the secret-file NUL class, the tab gate and the settings-persistence fixes landed in both editions.
- Every standard dialog follows one measured popup contract — fixed historical openings, a 470-pixel drag floor, no maximum width, the content hug with the settled re-fit — and the File Copy and Logs rows reflow responsively at the narrowest width while Cancel stays available during interactive menus. Localization ships exactly the 23 supported catalogs, and the artifacts are validated across the Debian, AppImage, Arch, RPM, APK and Etch packages.

The Debian package, AppImage, Arch package, RPM, APK and Debian Etch legacy package are built from the
complete source tree and validated in their target environments. Verify
downloaded artifacts against the SHA256SUMS attached to this release. The
AppImage update metadata (boot-repair_0.2.30_x86_64.AppImage.zsync) is attached
alongside the AppImage so Gear Lever/AppImageUpdate detect the release.

[Full source diff: v0.2.29...v0.2.30](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.29...v0.2.30).
