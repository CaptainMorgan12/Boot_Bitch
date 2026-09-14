# Boot Bitch 0.2.21

Boot Bitch 0.2.21 adds guarded maintenance for the running Debian/Ubuntu-family
host alongside the existing protected repair-target workflow.

- Run read-only diagnostics for either the running host or a selected repair
  system, including EFI/UKI, GRUB, initramfs, package, display-manager, disk,
  Btrfs, and firmware evidence.
- Restore a missing host TUXEDO UKI firmware entry and make it the default while
  preserving unrelated ESP entries, BootOrder, and BootNext state.
- Rebuild target or host EFI/UKI paths with vendor NVRAM mutation isolated to
  Boot Bitch, then reconcile model-aware TUXEDO, fallback, loader, and WFAI/
  iPXE labels without duplicating existing model names.
- Browse Host → Repair copy destinations through temporary read-only target
  mounts so a host folder cannot be mistaken for the repaired system.
- Keep diagnostic freshness gates, protected-host checks, mapper/crypttab
  validation, trial initramfs/GRUB checks, and post-write verification across
  both scopes.

The Debian package and AppImage are built locally from the complete source tree.
