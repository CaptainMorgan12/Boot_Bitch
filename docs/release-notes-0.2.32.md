# Boot Bitch 0.2.32

The changes in this release are listed below.

Released 2026-11-26.

- Display manager: the graphical-login repair is a walk-through preferring the last successful login, then the in-use manager, then the vendor default; the greetd landing installs the greeter and writes the post-login session command for the detected desktop shell, or refuses where none exists, and after a restore-default the active manager stays current, hence not a switch candidate, until the reboot completes the staged transition.
- Kernel repair: the module-versus-kernel incompatibility evidence distinguishes genuine build failures (the module's own build log and its declared highest compatible kernel version) from half-configured state blockers; per-kernel remedies install the prior compatible kernel with a next-boot switch or update the vendor module package, and the module repair only points at these actions.
- Kernel removal: removing a kernel completes forcefully — packages, module tree, headers, pinned-only metapackages and DKMS entries all go with it, the surviving kernel's state stays intact, and the removal plan plus its safety validator derive from the then-current package state so a mid-session headers install is never refused as protected.
- Boot menu: the boot-entries view gains Move Up, Move Down and Save buttons that commit the reorder through a verified grub-reorder step into a persisted order file which survives the tool's own GRUB regeneration, plus a Current default indicator reconciled through the guarded saved-entry write.
- Num Lock: the early-boot Num Lock repair is now complete across every family — the Fedora getty drop-in fallback chain, the automatic propagation-drop detection and promotion, the Alpine OpenRC invocation fix, the mkinitcpio hook drop-in and the host payload self-placement — with the diagnostic reporting the console state and the revert restoring it.
- Fixes: the Arch kernel pairing false negative (a present flavour-named initramfs was reported missing), the display-manager switch refusal re-displays the candidate menu instead of failing the stage, the leftover %1 placeholder in the kernel downgrade confirmation, the busybox-awk stream-filter portability on Alpine, the GDM/GDM3 alias duplication, the cancel output leak in the transcript and the volatile-NVRAM EFI entry verdict with its durability note.

The Debian package, AppImage, Arch package, RPM, APK and Debian Etch legacy package are built from the
complete source tree and validated in their target environments. Verify
downloaded artifacts against the SHA256SUMS attached to this release. The
AppImage update metadata (boot-bitch_0.2.32_x86_64.AppImage.zsync) is attached
alongside the AppImage so Gear Lever/AppImageUpdate detect the release.

[Full source diff: v0.2.31...v0.2.32](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.31...v0.2.32).
