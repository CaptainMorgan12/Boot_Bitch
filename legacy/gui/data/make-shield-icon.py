#!/usr/bin/env python3
"""Generate the legacy running-system shield badge from the modern icon atlas.

Qt3 has no SVG support and the legacy package ships PNG icons only. The
modern running-system badge is the COLORED semantic status asset in the
shared icon atlas, resources/icons/atlas/security.svg (the green
#2e9d50 shield with a checkmark, mapped from the themedIcon("security-high")
request — MainWindow.cpp; the same asset the modern Systems tab shows next to
the resolved running-system name). This script rasterizes that one atlas SVG
into the shipped legacy PNG with the project's own SVG engine family
(PySide6.QtSvg — the same renderer stack as the modern GUI's runtime QIcon
path), so the Etch badge is an extraction of the modern artwork, never a
hand-drawn copy. Run it after changing the atlas asset:

    python3 legacy/gui/data/make-shield-icon.py

Fails with a clear message when the atlas asset is missing or PySide6
(host-side convenience only; never needed on Etch) is unavailable.
"""
import os
import sys

from PySide6.QtCore import QRectF, Qt
from PySide6.QtGui import QImage, QPainter
from PySide6.QtSvg import QSvgRenderer

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
SOURCE = os.path.join(ROOT, "resources", "icons", "atlas", "security.svg")
SIZE = 32
OUT = os.path.join(HERE, "boot-repair-legacy-shield-%dx%d.png" % (SIZE, SIZE))


def main():
    if not os.path.isfile(SOURCE):
        sys.exit(
            "missing modern atlas asset: %s\n"
            "make-shield-icon.py must run from the repository checkout; the "
            "colored shield-check badge ships in the shared icon atlas under "
            "resources/icons/atlas/." % SOURCE)
    with open(SOURCE, "rb") as handle:
        svg = handle.read()
    renderer = QSvgRenderer(svg)
    if not renderer.isValid():
        sys.exit("resources/icons/atlas/security.svg is not a valid SVG asset.")
    image = QImage(SIZE, SIZE, QImage.Format_ARGB32_Premultiplied)
    image.fill(Qt.transparent)
    painter = QPainter(image)
    painter.setRenderHint(QPainter.Antialiasing, True)
    renderer.render(painter, QRectF(0.0, 0.0, float(SIZE), float(SIZE)))
    painter.end()
    if not image.save(OUT, "PNG"):
        sys.exit("could not write %s" % OUT)
    print("wrote", os.path.relpath(OUT, HERE))


if __name__ == "__main__":
    main()
