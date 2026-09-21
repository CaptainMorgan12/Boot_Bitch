# Boot Bitch 0.2.20

Completes the bundled Qt/KDE icon atlas and restores the semantic defaults for
tabs, diagnostics, repair stages and actions, with the same fallback behavior
in Debian packages and portable AppImages when a host icon theme is
incomplete.

- Bundle the semantic Qt/KDE icon atlas for tabs, diagnostics, repair stages
  and actions, preferring a usable host theme or freedesktop aliases before
  the bundled artwork and native high-contrast fallbacks; add filesystem-aware
  device icons and reject solid theme placeholders so controls remain readable
  on incomplete host themes.

Validation completed for the release workspace:

- Icon atlas contract: 40 requests passed.
- Qt UI, shell-contract and display-manager tests passed.
- Debian package install and desktop/icon checks passed.
- Debian package: `boot-repair_0.2.20_amd64.deb`.
- AppImage smoke test: `boot-repair_0.2.20_x86_64.AppImage`.
