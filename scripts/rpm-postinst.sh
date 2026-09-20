#!/bin/sh
# RPM post-install scriptlet. rpmbuild executes scriptlets with /bin/sh, so
# this file must stay POSIX shell. Refreshing both caches here makes GTK-based
# launchers and software centers pick up the packaged icons and desktop entry
# immediately after install and upgrade.
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true
fi
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database -q /usr/share/applications || true
fi
exit 0
