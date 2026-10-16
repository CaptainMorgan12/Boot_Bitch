# Boot Bitch 0.2.28

Boot Bitch 0.2.28 makes diagnostics the single source of truth for every repair, adds the EFI boot entries manager and a read-only boot menu, and finishes the Debian Etch legacy GUI's Settings splitter, unlock popup and window handling.

Released 2026-10-16.

- Diagnostics as the single source of truth: the Boot capabilities page reads the cached diagnostic evidence (Repair target / Host scope selector, change-aware refresh, translation-fit columns), and repairs consume multi-diagnostic baselines for kernel, GPU, boot-error, DKMS, initramfs, display, EFI and GRUB work, with a shared boot-reached classifier (systemd boot markers) and a read-only package-state gate.
- EFI, boot menu and flows: a new EFI boot entries tab lists the UEFI firmware entries with reorder, rename and delete (BootCurrent is protected, NVRAM writes are verified and rolled back), plus a read-only boot-menu tree view falling back through GRUB, extlinux and GRUB legacy; the LUKS unlock queues behind in-flight work, and the shells gain command history.
- Kernel, module and GPU across distributions: Fedora akmods and Alpine akms module-build systems (dynamic labels everywhere, never a hardcoded "DKMS"), Fedora/rpm kernel cleanup parity with the apt kernels tool, a relaxed GPU gate that still derives the expected driver from hardware evidence, and the file-system check refuses offline-only tools on a still-mounted btrfs device (a busy device is skipped, never a false "issues" prompt).
- Debian Etch legacy GUI: the Settings splitter now drags both ways with pinned frame heights — a setSizes-driven drag controller works around Qt 3.3.7's drag-path bugs (the panes' sum decayed on every incremental mouse move and posted layout events lagged the drag) and keeps the mandatory safety section visible at every handle position; closing the window actually exits the application, and the unlock-LUKS popup is sized correctly.
- Bug fixes and UI/UX polish: firmware and bootloader changes are transactional — writes are verified and automatically rolled back, recovering from the earlier incident where a failed edit removed the EFI boot entries; the shell no longer loses commands typed while a command is running (prompt and command-field handling); and host and target scope state no longer leaks between sessions.
- Localization and polish: every new string is translated into 22 languages with German short action-word overrides, tooltips are shortened to concise one-liners, header rows reflow and button rows wrap at the minimum window width, and a masked drift-gate `pipefail` bug is fixed in the legacy port.

The Debian package, AppImage, Arch package, RPM, APK and Debian Etch legacy package are built from the
complete source tree and validated in their target environments. Verify
downloaded artifacts against the SHA256SUMS attached to this release. The
AppImage update metadata (boot-repair_0.2.28_x86_64.AppImage.zsync) is attached
alongside the AppImage so Gear Lever/AppImageUpdate detect the release.

[Full source diff: v0.2.27...v0.2.28](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.27...v0.2.28).
