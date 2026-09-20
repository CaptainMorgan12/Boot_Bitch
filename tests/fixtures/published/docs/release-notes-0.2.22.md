# Boot Bitch 0.2.22

Boot Bitch 0.2.22 fixes Host → Repair destination browsing.

- Decode helper directory records correctly so the selected repair system's
  folders appear in the browser.
- Use a compact, freely resizable browser with an action row that remains
  usable at smaller window sizes.
- Keep each listing read-only and temporary; the guarded copy validates the
  selected virtual path again before writing.

The Debian package and AppImage are built locally from the complete source tree.
