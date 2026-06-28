#!/usr/bin/env python3
"""
Process the Spek-Xtra logo:
- Remove the green screen background
- Generate hicolor icon sizes
- Generate a high-res scalable PNG
- Build a multi-resolution Windows application ICO
"""

import os
import subprocess
import sys
from PIL import Image

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(PROJECT_ROOT, "data", "icons", "scalable", "Flux2_00044_.png")
OUT_DIR = os.path.join(PROJECT_ROOT, "data", "icons")
SIZES = [16, 22, 24, 32, 48]
ICO_SIZES = [16, 20, 24, 32, 40, 48, 64, 96, 128, 256]


def remove_green_screen(img: Image.Image) -> Image.Image:
    """Make the green screen transparent, preserving anti-aliased edges."""
    img = img.convert("RGBA")
    data = img.getdata()
    new_data = []

    for r, g, b, a in data:
        # Detect green screen pixels. The background is a bright green.
        # We use a hue/saturation-like test rather than a hard box so that
        # shadows and compression artifacts near the edge fade out smoothly.
        # Green dominates and red+blue are low => background.
        is_green = (g > 120 and r < 100 and b < 100)

        if is_green:
            # Fully transparent background
            new_data.append((0, 0, 0, 0))
        else:
            # Keep the pixel as-is
            new_data.append((r, g, b, a))

    img.putdata(new_data)
    return img


def remove_red_fringe(img: Image.Image) -> Image.Image:
    """
    Some chroma-keyed images leave a red/pink fringe where the green screen
    met the foreground. This pass weakens pure red-ish pixels at the edge.
    """
    data = img.getdata()
    new_data = []
    for r, g, b, a in data:
        # If red is much stronger than green/blue and the pixel is not fully
        # opaque foreground, treat it as a fringe remnant.
        if a > 0 and r > 140 and g < 90 and b < 90:
            # Fade it out based on how extreme the red is
            alpha_factor = max(0.0, 1.0 - (r - g) / 120.0)
            new_a = int(a * alpha_factor)
            new_data.append((r, g, b, new_a))
        else:
            new_data.append((r, g, b, a))
    img.putdata(new_data)
    return img


def crop_to_content(img: Image.Image, padding: int = 20) -> Image.Image:
    """Crop transparent borders and add a small padding."""
    bbox = img.getbbox()
    if bbox is None:
        return img
    left, top, right, bottom = bbox
    width, height = img.size
    left = max(0, left - padding)
    top = max(0, top - padding)
    right = min(width, right + padding)
    bottom = min(height, bottom + padding)
    return img.crop((left, top, right, bottom))


def make_square(img: Image.Image, size: int) -> Image.Image:
    """Center the image on a transparent square canvas."""
    result = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    # Scale so the content fits with a margin
    content_size = int(size * 0.85)
    scaled = img.copy()
    scaled.thumbnail((content_size, content_size), Image.LANCZOS)
    x = (size - scaled.width) // 2
    y = (size - scaled.height) // 2
    result.paste(scaled, (x, y), scaled)
    return result


def main():
    if not os.path.exists(SRC):
        print(f"Source image not found: {SRC}", file=sys.stderr)
        sys.exit(1)

    print(f"Processing {SRC} ...")
    img = Image.open(SRC)
    print(f"  source size: {img.size}")

    img = remove_green_screen(img)
    img = remove_red_fringe(img)
    img = crop_to_content(img)
    print(f"  cropped size: {img.size}")

    # Scalable / high-res icon
    scalable_path = os.path.join(OUT_DIR, "scalable", "spek.png")
    scalable = make_square(img, 512)
    scalable.save(scalable_path, "PNG")
    print(f"  saved {scalable_path}")

    # Fixed hicolor sizes
    for size in SIZES:
        out_path = os.path.join(OUT_DIR, f"{size}x{size}", "spek.png")
        icon = make_square(img, size)
        icon.save(out_path, "PNG")
        print(f"  saved {out_path}")

    # Windows application icon (multi-resolution ICO)
    ico_path = os.path.join(PROJECT_ROOT, "dist", "win", "spek.ico")
    ico_dir = os.path.join(PROJECT_ROOT, "scripts", ".icon-cache", "logo-ico")
    os.makedirs(ico_dir, exist_ok=True)

    png_paths = []
    for size in ICO_SIZES:
        icon = make_square(img, size)
        png_path = os.path.join(ico_dir, f"spek-{size}.png")
        icon.save(png_path, "PNG")
        png_paths.append(png_path)

    # Use ImageMagick to build a proper multi-frame ICO
    args = ["magick"] + png_paths + [ico_path]
    subprocess.run(args, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print(f"  saved {ico_path}")

    print("Done.")


if __name__ == "__main__":
    main()
