#!/usr/bin/env python3
"""Generate the legacy info-popup button icon from the modern icon atlas.

Qt3 has no SVG support and the legacy package ships PNG icons only. The
modern tab "i" button renders the bundled MONOCHROME circle-i atlas asset
(resources/icons/atlas/info.svg — the #333 circle with the lowercase-i stem
and dot, mapped from the themedIcon("help-contextual") request —
MainWindow.cpp; the same asset every modern tab shows next to its page
title). This script rasterizes that one atlas SVG into the shipped legacy
PNG with the project's own SVG engine family (PySide6.QtSvg — the same
renderer stack as the modern GUI's runtime QIcon path), so the Etch button
is an extraction of the modern artwork, never a hand-drawn copy. Run it
after changing the atlas asset:

    python3 legacy/gui/data/make-info-icon.py

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
SOURCE = os.path.join(ROOT, "resources", "icons", "atlas", "info.svg")
SIZE = 18
OUT = os.path.join(HERE, "boot-repair-legacy-info-%dx%d.png" % (SIZE, SIZE))


def main():
    if not os.path.isfile(SOURCE):
        sys.exit(
            "missing modern atlas asset: %s\n"
            "make-info-icon.py must run from the repository checkout; the "
            "monochrome circle-i badge ships in the shared icon atlas under "
            "resources/icons/atlas/." % SOURCE)
    with open(SOURCE, "rb") as handle:
        svg = handle.read()
    renderer = QSvgRenderer(svg)
    if not renderer.isValid():
        sys.exit("resources/icons/atlas/info.svg is not a valid SVG asset.")
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
