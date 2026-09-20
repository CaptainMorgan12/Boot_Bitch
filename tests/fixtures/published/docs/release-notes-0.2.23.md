# Boot Bitch 0.2.23

Boot Bitch 0.2.23 rolls up the local 0.2.21 and 0.2.22 iterations and the final
0.2.23 fixes into one release after GitHub's 0.2.20. It adds guarded running-host
maintenance, repair-system folder browsing, and verified EFI destination and
label maintenance.

- Run the complete read-only diagnostic set for either the protected Running
  Host or a selected repair system, and run the supported maintenance stages
  in either scope after the matching safety checks.
- Restore a missing host TUXEDO UKI registration and make it the default while
  preserving unrelated ESP entries, BootOrder and BootNext state.
- Browse Host → Repair destinations through temporary read-only mounts of the
  selected repair filesystem, with a compact resizable folder browser and a
  guarded virtual-path recheck before copying.
- Retain one UKI, fallback and WebFAI destination per maintained ESP while
  removing only verified duplicate routes.
- Group each drive's entries in BootOrder by primary use and attach the drive
  model to labels without repeating existing model text.
- Distinguish TUXEDO Debian-base UKI boot from TUXEDO Ubuntu/GRUB boot and
  record the expected root-LUKS unlock handoff in read-only boot evidence.
- Change existing EFI entry labels only after validating their EFI variable's
  selected-ESP PARTUUID and loader, then reading the variable back.
- Show brief completion notices and visible diagnostic prerequisites for
  disabled repair actions, with full output retained in Results and Logs.

The README now distinguishes desktop widget styling from the bundled semantic
icon atlas and documents both Running Host and selected repair-system scopes.

The Debian package and AppImage are built locally from the complete source tree.

[Full source diff: v0.2.20...v0.2.23](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.20...v0.2.23).
