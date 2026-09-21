# Boot Bitch 0.2.23

Adds guarded running-host maintenance, repair-system folder browsing and
verified EFI destination and label maintenance.

- Maintain one canonical UKI, firmware fallback and WebFAI destination per
  maintained ESP, remove safely identified duplicates, and group retained
  firmware entries in BootOrder by drive and normal use (UKI/vendor loader
  before fallback and WebFAI); update existing descriptions only through a
  validated EFI_LOAD_OPTION rewrite with read-back.
- Record the detected UKI/GRUB boot chain and root-LUKS handoff in boot
  evidence so repairs preserve the expected single unlock path.
- UI/docs: keep the repaired-system folder browser's navigation and
  confirmation rows visible at compact sizes, annotate maintained entries with
  the drive model without duplicating model text, and clarify EFI behavior,
  icon fallbacks and Running Host versus selected-repair support.

The Debian package and AppImage are built locally from the complete source tree.

[Full source diff: v0.2.20...v0.2.23](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.20...v0.2.23).
