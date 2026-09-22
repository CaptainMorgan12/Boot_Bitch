#!/usr/bin/env python3
"""Generate the legacy Boot Bitch PNG icons (build-time only).

Qt3 has no SVG support and the legacy package ships PNG icons only. This
script is optional: the generated PNGs are committed under data/. Run it after
changing the artwork:

    python3 legacy/gui/data/make-icons.py

Requires Pillow (host-side convenience only; never needed on Etch).
"""
import os
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
SIZES = (16, 22, 32, 48)


def draw_master(size=256):
    image = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    margin = size * 0.04
    radius = size * 0.18
    # Rounded dark slate background.
    draw.rounded_rectangle(
        [margin, margin, size - margin, size - margin],
        radius=radius, fill=(31, 58, 95, 255))
    # Disk platter.
    platter = [size * 0.22, size * 0.22, size * 0.78, size * 0.78]
    draw.ellipse(platter, fill=(226, 232, 240, 255))
    draw.ellipse([size * 0.33, size * 0.33, size * 0.67, size * 0.67],
                 fill=(148, 163, 184, 255))
    draw.ellipse([size * 0.43, size * 0.43, size * 0.57, size * 0.57],
                 fill=(31, 58, 95, 255))
    # Green check mark across the lower right.
    width = max(2, int(size * 0.075))
    points = [(size * 0.44, size * 0.66), (size * 0.56, size * 0.78),
              (size * 0.84, size * 0.34)]
    draw.line(points, fill=(34, 197, 94, 255), width=width, joint="curve")
    return image


def main():
    master = draw_master()
    for size in SIZES:
        icon = master.resize((size, size), Image.LANCZOS)
        path = os.path.join(HERE, "boot-repair-legacy-%dx%d.png" % (size, size))
        icon.save(path)
        print("wrote", os.path.relpath(path, HERE))


if __name__ == "__main__":
    main()
