# Boot Bitch 0.2.29

Boot Bitch 0.2.29 completes the recovery-host/target isolation, adds proven capability installs and in-run confirmations, repairs graphical login everywhere, fixes cancel, cache, shell and log behavior in both editions, and polishes the Debian Etch legacy edition.

Released 2026-10-27.

- Host/Target isolation, snapshots and kernel evidence: target-scope dnf5 transactions skip the running-kernel protection; the dracut rebuild drops host-only mode, the fresh-procfs and identity-reading scope guarantees are contract-asserted, the Snapshots tab appears whenever a snapshot inventory exists (the Rollback button keeps its guard), already-mounted non-Linux disks report the clean verdict, and the kernel diagnostic names why a repository kernel is not installable.
- UI alignment, confirmations and capability installs: splitter handles drag down, capabilities sort on header click, package-mutating stages confirm inside the progress popup (a declined answer skips the stage, the plan continues), a missing display manager or boot-stack prerequisite gets a proven Install action that re-probes and regenerates the affected diagnostics, Add Note gains a resizable memo, and dialogs render one button order and default.
- Graphical login repair on every service manager: both editions detect a missing display manager (gdm, gdm3, kdm, lightdm and similar) and offer a proven Install that configures and enables it, so a graphical login returns after reboot, including on SysV-init targets. The display diagnostic flags the black-screen-after-unlock class and initramfs-backend switchover evidence, and the display repair appends console=tty0 last.
- Cancel, cache, indicator, shell and log fixes in both editions: cancelling an individual diagnostic clears only that result and caches nothing partial, a queued snapshot indicator always resolves, session logs are listed, deleted mid-run and appended to the selected prior log through the authenticated helper, declined shell commands report nothing-changed without invalidating the cache, and the interactive shells keep a controlling terminal where script(1) is absent.
- Security hardening in the backend and both GUIs: the Alpine kernel-suffix gains an allowlist, initramfs verification passes the image positionally, legacy menu-answer files never come from the environment, session logs are 0600, kernel zombie cleanups remove only realpath-contained trees, staged helper copies — AppImage's embedded helper and digest-pinned EFI label updater — are re-hashed before pkexec reuse, and a post-efi reconcile consolidates duplicate UKI entries.
- Debian Etch edition, bug fixes and polish: the Settings splitter drags, apt actions translate to apt-get, the SysV probe detects kdm and xdm, prompts commit on Enter, the Add Note memo mirrors the modern dialog, notes append correctly to the selected prior log, and the host-to-target switch no longer crashes; the mounts list collapses stacked and kernel pseudo mounts, and File Copy waits out a running diagnostic.

The Debian package, AppImage, Arch package, RPM, APK and Debian Etch legacy package are built from the
complete source tree and validated in their target environments. Verify
downloaded artifacts against the SHA256SUMS attached to this release. The
AppImage update metadata (boot-repair_0.2.29_x86_64.AppImage.zsync) is attached
alongside the AppImage so Gear Lever/AppImageUpdate detect the release.

Known notes: eight modern-only strings remain in the documented parity backlog; the legacy unlock keyfile writer keeps the trailing byte its consumer tolerates; the Ukrainian catalog's "Try Again" label still carries a machine-translation error (the Korean labels were corrected this cycle); and the Alpine EFI rig installs the BIOS-built package, verified byte-identical. The README and app-store gallery screenshots are nine new captures; when this release is published, the GitHub-hosted screenshots are replaced by these files.

[Full source diff: v0.2.28...v0.2.29](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.28...v0.2.29).
