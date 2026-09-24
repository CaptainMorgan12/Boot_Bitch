#!/usr/bin/env python3
"""Generate the legacy Boot Bitch PNG icons from the modern master artwork.

Qt3 has no SVG support and the legacy package ships PNG icons only. The
modern master is the packaged Boot Bitch icon,
data/icons/hicolor/1024x1024/apps/org.bootrepair.BootRepair.png; this script
resizes it (LANCZOS) into the shipped legacy sizes 16/22/32/48. Run it after
changing the artwork:

    python3 legacy/gui/data/make-icons.py

Fails with a clear message when the master is missing. Requires Pillow
(host-side convenience only; never needed on Etch).
"""
import os
import sys

from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
MASTER = os.path.join(
    ROOT, "data", "icons", "hicolor", "1024x1024", "apps",
    "org.bootrepair.BootRepair.png")
SIZES = (16, 22, 32, 48)


def main():
    if not os.path.isfile(MASTER):
        sys.exit(
            "missing modern master icon: %s\n"
            "make-icons.py must run from the repository checkout; the master "
            "ships in the public tree under data/icons/hicolor/1024x1024/apps/."
            % MASTER)
    with Image.open(MASTER) as master:
        for size in SIZES:
            icon = master.resize((size, size), Image.LANCZOS)
            path = os.path.join(
                HERE, "boot-repair-legacy-%dx%d.png" % (size, size))
            icon.save(path)
            print("wrote", os.path.relpath(path, HERE))


if __name__ == "__main__":
    main()
