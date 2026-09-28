# Boot Bitch 0.2.25

Boot Bitch 0.2.25 ships native RPM and APK release artifacts, the Alpine,
Arch and Fedora repair backends, running-host Btrfs snapshot rollback, the
read-only-first file system repair engine, and the usual logging, UI and
packaging polish.

Released 2026-09-19.

- Native RPM and APK release artifacts: `scripts/package-rpm.sh` builds the
  CPack RPM with family-specific dependencies and the POSIX `/bin/sh`
  scriptlet, `scripts/package-alpine.sh` builds the signed APK with abuild,
  and `scripts/test-rpm.sh`/`test-apk.sh` validate both without installing
  them.
- Alpine, Arch and Fedora repair backends: guarded apk, pacman and dnf5
  package transactions with mkinitfs, mkinitcpio, dracut, GRUB/extlinux and
  display-manager stages join the Debian/APT backends; Arch host-maintenance
  package repair and upgrade stages are included, while standalone APT/dpkg
  stages remain unavailable on Arch.
- Running-host Btrfs snapshot rollback: the Snapshots tab lists Snapper root
  snapshots and Boot Bitch undo points and stages the name-preserving
  rollback (promote a writable copy, reconcile initramfs/UKI/GRUB, restore on
  failure), with a **Reboot required** reminder and never an automatic
  reboot.
- Read-only-first file system repair: diagnostics resolve the root, `/boot`,
  ESP and `/home` filesystems, run the matching read-only check and offer
  per-device repair only after issues are reported, with an extra warning for
  dangerous modes.
- Logging, UI and fixes: a fresh per-launch session log with retention and
  `@section` search, one authorization request per scope, a guarded Host
  Shell and responsive layouts; plus fixes from user testing (PackageKit, apt
  metadata retry, GRUB EFI firmware-ID handling) and package-manager-aware
  dependency setup.

The Debian package, AppImage, Arch package, RPM and APK are built from the
complete source tree and validated in their target environments. Verify
downloaded artifacts against the SHA256SUMS attached to this release.

[Full source diff: v0.2.24...v0.2.25](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.24...v0.2.25).
