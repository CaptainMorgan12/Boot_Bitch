# Boot Bitch 0.2.24

Adds the semantic icon atlas, the read-only-first file system repair engine
and the distribution/boot backend profiler.

Released 2026-09-16.

- Bundle the semantic icon atlas as the deterministic UI source across desktop
  themes and add the Qt SVG runtime dependency required on Arch and other
  minimal installations.
- Add the read-only-first File system repair tool and Full Repair stage
  (ordered first): resolve the root, `/boot`, ESP and `/home` filesystems with
  their UUIDs, run the matching read-only check, and offer per-device repair
  modes only after issues are found and the user confirms; offline tools
  refuse mounted filesystems while btrfs/zpool scrub stay online.
- Add the read-only distribution/boot backend profiler, backend-profile
  contract test and kernel/initramfs pairing diagnostics; gate every repair
  tool and Full Repair stage on the scope's read-only diagnostics (DKMS, APT
  metadata and missing prerequisites disable with a reason
  while helper preflights remain).
- UI/UX polish: busy indicator; one authorization request per scope;
  section-scoped diagnostics refresh with coalesced incremental rendering;
  narrow-window Logs/File Copy; guarded Host Shell with the host-maintenance
  crash fix; Polkit prompt on Host Maintenance entry; `@section` log search
  and filters; responsive Repair/Systems layouts; scroll edge shadows;
  one-fifth splitter minimums; compact snapshot rows; and software-center
  metadata with six screenshots, the installed size and package association.
- Fixes: duplicate-label GRUB EFI entries no longer pollute the firmware ID;
  an idle PackageKit daemon no longer blocks host package repairs; apt
  release-metadata changes warn and retry once; no-op pacman transactions
  report unchanged; Run All no longer duplicates log sections; stale
  `/usr/local` source installs are removed on uninstall; file-system check
  tools are declared package dependencies; the protected-host shield returns
  as a green status icon.

The Debian package and AppImage are built locally from the complete source tree.

[Full source diff: v0.2.23...v0.2.24](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.23...v0.2.24).
