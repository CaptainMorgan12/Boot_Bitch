# Boot Bitch 0.2.26

Boot Bitch 0.2.26 ships the fully functional Debian Etch (Qt 3 / KDE 3.5) legacy edition, interactive shell-prompt answering, a large security-hardening pass across the helper, both GUIs and the legacy port generator, and the accumulated parity and repair polish since 0.2.25.

Released 2026-09-28.

- Debian Etch support: a complete Qt 3 legacy edition — the same privileged
  helper with LUKS unlock, split-LVM targets, guarded menu.lst regeneration,
  file copy, host maintenance and Make Default — validated end-to-end on
  real Etch systems.
- Interactive shell answers: the Chroot and Host Shells run under a pty and
  answer prompts through a GUI popup (free-text answers, repeated for
  debconf flows, the timeout pauses while a prompt waits), with clean
  transcripts and answers that are never interpreted.
- Security hardening across the helper, both GUIs and the legacy port
  generator: contained target mounts, a filtered private /dev for repair
  chroots, prompt-spoofing and protocol-bound hardening, per-disk session
  locks, process-group cleanup, verified helper and elevation paths,
  secret-file config writes and fail-loud port transforms.
- Repair and parity polish: evidence-gated, simulation-first package stages
  for apt/dpkg, pacman, apk and dnf5, single-build dracut, GRUB/GRUB2/
  extlinux regeneration with entry preservation, TUXEDO UKI rebuilds, EFI
  entry management, Make Default on every boot chain, and the Settings plan
  with per-scope availability in both editions.
- Fixes from user testing across Alpine, Fedora, Debian, Arch, TUXEDO OS
  and the Etch rigs, plus a faster quick development gate.

The AppImage carries the gh-releases-zsync update metadata boot-repair_0.2.26_x86_64.AppImage.zsync for Gear Lever and AppImageUpdate. The Debian package, AppImage, Arch package, RPM and APK are built from the
complete source tree and validated in their target environments. Verify
downloaded artifacts against the SHA256SUMS attached to this release.
[Full source diff: v0.2.25...v0.2.26](https://github.com/CaptainMorgan12/Boot_Bitch/compare/v0.2.25...v0.2.26).
