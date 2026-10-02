# Boot Bitch 0.2.27

Boot Bitch 0.2.27 headlines full localization — a 22-language GUI with a language-independent backend — alongside the startup-crash fix, process-leak repairs, helper hardening, the Debian Etch apt fix and a development-speed overhaul.

Released 2026-10-02.

- Localization: the full GUI is translated into 22 languages via offline
  machine translation, with a language-independent backend — the privileged
  helper emits stable machine-readable `reason:`/`msg:` keys that the GUI
  renders in the active locale, while the machine-parsed session log stays
  English (`LC_ALL=C`) and only display-only transcripts follow the locale.
- Reliability and UI polish: the startup crash is fixed (the
  privileged-session readiness poll no longer nests an event loop); the Logs
  tab drops the redundant Clear button and fixes the German Clear/Delete
  collision; long titles elide and the Repair splitter is rebalanced.
- Helper hardening: the parsed output is pinned English, ZFS and
  display-manager detection are normalized, TUXEDO UKI `subvol=` and EFI
  boot-number hex handling are corrected, and the Arch GRUB fallback loader is
  reinstalled with an ESP backup/rollback.
- Process-leak fixes: a timed-out or cancelled command reaps its whole process
  tree (group kill plus a descendant cleanup) in the modern helper, the
  Etch port and the test harness, so apt/dpkg/pacman grandchildren can no
  longer outlive a run.
- Debian Etch: `apt <action>` shell commands translate to `apt-get` on targets
  without the modern apt binary, and the legacy GUI gains busy-indicator, Save
  As… share-default and parsed `--log-dir` parity.
- Development speed and test pipeline: a fast CI lane, parallel and
  cache-aware gates, an incremental hygiene scan, checksum-gated builds and a
  test-manifest budget contract; the UI suite is split into six translation
  units for parallel compiles.

The Debian package, AppImage, Arch package, RPM, APK and Debian Etch legacy package are built from the
complete source tree and validated in their target environments. Verify
downloaded artifacts against the SHA256SUMS attached to this release. The
AppImage update metadata (boot-repair_0.2.27_x86_64.AppImage.zsync) is attached
alongside the AppImage so Gear Lever/AppImageUpdate detect the release.

[Full source diff: v0.2.26...v0.2.27](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.26...v0.2.27).
