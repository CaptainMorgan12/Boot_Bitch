# Boot Bitch 0.2.28

Boot Bitch 0.2.28 adds the EFI boot entries manager and a read-only boot menu, a smoother LUKS unlock flow, bash-style shell command history, and tooltip and responsive-layout polish.

Released 2026-10-03.

- EFI boot entries: a new tab lists the UEFI firmware entries (`Boot####`/`BootOrder`) for the running host or a selected repair target, with reorder, rename and delete (the active BootCurrent entry is protected, blank labels are refused, NVRAM writes are verified and rolled back), a "Boots" meaning column, an "EFI-/UKI Restore" action, and automatic refresh after diagnostics or a boot-affecting repair.
- Read-only boot menu: a dropdown shows the boot menu as a collapsible tree with submenu nesting and config-file surfacing, falling back through GRUB, extlinux/syslinux and GRUB legacy so BIOS-only systems get an equivalent view.
- LUKS unlock: queues behind an in-flight diagnostic or repair instead of superseding it, reuses the established authorization (no second password prompt), removes the confirmation dialog, and no longer triggers an automatic diagnostics regeneration.
- Shell history: the chroot and host shells add bash-style Up/Down command history (session-only) and clear the command field at dispatch, so a command typed during a running command is not wiped.
- Tooltips and responsive layout: tooltips are shortened to concise one-liners with missing ones added, and header rows reflow and button rows wrap at the minimum window width.
- Localization and Debian Etch: all new and changed strings are translated into 22 languages with German short action-word overrides, and a masked drift-gate `pipefail` bug is fixed in the legacy port.

The Debian package, AppImage, Arch package, RPM, APK and Debian Etch legacy package are built from the complete source tree and validated in their target environments. Verify downloaded artifacts against the SHA256SUMS attached to this release. The AppImage update metadata (boot-repair_0.2.28_x86_64.AppImage.zsync) is attached alongside the AppImage so Gear Lever/AppImageUpdate detect the release.

[Full source diff: v0.2.27...v0.2.28](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.27...v0.2.28).
