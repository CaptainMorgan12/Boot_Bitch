# Boot Bitch 0.2.23

Boot Bitch 0.2.23 keeps firmware destinations unambiguous across multiple Linux
ESP installations.

- Retain one UKI, fallback, and WebFAI destination per maintained ESP while
  removing only verified duplicate routes.
- Group each drive's entries in BootOrder by primary use and attach the drive
  model to labels without repeating existing model text.
- Distinguish TUXEDO Debian-base UKI boot from TUXEDO Ubuntu/GRUB boot and record
  the expected root-LUKS unlock handoff in read-only boot evidence.

The Debian package and AppImage are built locally from the complete source tree.
