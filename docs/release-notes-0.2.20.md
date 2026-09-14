# Boot Bitch 0.2.20

This release completes the bundled Qt/KDE icon atlas and restores the
0.2.13-era semantic defaults for tabs, diagnostics, repair stages and actions.
Installed Debian packages and portable AppImages now share the same fallback
behavior when a host icon theme is incomplete or unavailable.

Highlights:

- Bundle the complete semantic atlas in the Qt resource file, including drive,
  filesystem, repair, diagnostic, terminal, security, snapshot and action
  artwork.
- Prefer the active GNOME, GTK or KDE theme, then use equivalent freedesktop
  aliases and the bundled atlas before native Qt glyphs.
- Keep monochrome icons readable on dark palettes and detect solid placeholder
  theme icons so they do not replace useful artwork.
- Add filesystem-aware device icons and preserve the stable 0.2.13 defaults
  for the tab and diagnostic icon names.
- Make AppImage tooling use the local `Development/tools` installation for
  repeatable release builds.

Validation completed for the release workspace:

- Icon atlas contract: 40 requests passed.
- Qt UI, shell-contract and display-manager tests passed.
- Debian package install and desktop/icon checks passed.
- Debian package: `boot-repair_0.2.20_amd64.deb`.
- AppImage smoke test: `boot-repair_0.2.20_x86_64.AppImage`.
