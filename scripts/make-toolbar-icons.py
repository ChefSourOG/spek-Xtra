#!/usr/bin/env python3
"""
Render Spek-Xtra toolbar assets into Windows ICO files and macOS PNGs.

Each toolbar resource can point to either an SVG or a PNG.  The assets are
resized and centred onto a transparent square before being assembled into a
multi-resolution ICO.
"""

import os
import subprocess
import tempfile

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ASSETS_DIR = os.path.join(PROJECT_ROOT, "assets")
CUSTOM_DIR = os.path.join(PROJECT_ROOT, "scripts", "custom-icons")
OUT_DIR = os.path.join(PROJECT_ROOT, "dist", "win")
MACOS_OUT_DIR = os.path.join(PROJECT_ROOT, "dist", "osx")
SIZES = [16, 20, 24, 32, 40, 48, 64, 96, 128, 256]

# Resource name -> source image (PNG or SVG)
ICONS = {
    "open":     os.path.join(ASSETS_DIR, "folder.png"),
    "save":     os.path.join(ASSETS_DIR, "export-file.png"),
    "copy":     os.path.join(ASSETS_DIR, "side-by-side.png"),
    "home":     os.path.join(ASSETS_DIR, "reload.png"),
    "fileopen": os.path.join(ASSETS_DIR, "correlation.png"),
    "help":     os.path.join(CUSTOM_DIR, "help.svg"),
    "close":    os.path.join(CUSTOM_DIR, "close.svg"),
}


def render_png(src_path: str, size: int, out_dir: str) -> str:
    """Render a source image to a centred PNG at the requested size."""
    base = os.path.splitext(os.path.basename(src_path))[0]
    png_path = os.path.join(out_dir, f"{base}-{size}.png")
    subprocess.run(
        [
            "magick",
            "-background", "none",
            src_path,
            "-resize", f"{size}x{size}",
            "-gravity", "center",
            "-extent", f"{size}x{size}",
            png_path,
        ],
        check=True,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    return png_path


def make_ico(resource_name: str, src_path: str, tmp_dir: str) -> None:
    """Create a multi-resolution ICO from a source image."""
    pngs = [render_png(src_path, size, tmp_dir) for size in SIZES]

    out_path = os.path.join(OUT_DIR, f"{resource_name}.ico")
    subprocess.run(
        ["magick"] + pngs + [out_path],
        check=True,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    print(f"  saved {out_path}")


def make_macos_png(resource_name: str, src_path: str, size: int, tmp_dir: str) -> None:
    """Create a single-resolution PNG for the macOS bundle."""
    png_path = render_png(src_path, size, tmp_dir)
    out_path = os.path.join(MACOS_OUT_DIR, f"{resource_name}.png")
    subprocess.run(
        ["magick", png_path, out_path],
        check=True,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    print(f"  saved {out_path}")


def make_preview(tmp_dir: str, out_path: str) -> None:
    """Build a left-to-right preview of the toolbar icons."""
    preview_size = 48
    padding = 8
    count = len(ICONS)
    width = count * preview_size + (count + 1) * padding
    height = preview_size + 2 * padding

    args = [
        "magick",
        "-size", f"{width}x{height}",
        "xc:#f0f0f0",
    ]

    for i, (resource, src_path) in enumerate(ICONS.items()):
        png_path = render_png(src_path, preview_size, tmp_dir)
        x = padding + i * (preview_size + padding)
        y = padding
        args.extend([png_path, "-geometry", f"+{x}+{y}", "-composite"])

    args.append(out_path)
    subprocess.run(args, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print(f"  saved {out_path}")


def main():
    print("Generating toolbar icons")
    os.makedirs(OUT_DIR, exist_ok=True)
    os.makedirs(MACOS_OUT_DIR, exist_ok=True)

    with tempfile.TemporaryDirectory(prefix="spek-icons-") as tmp_dir:
        for resource, src_path in ICONS.items():
            print(f"{resource}")
            make_ico(resource, src_path, tmp_dir)

        print("Generating macOS toolbar PNGs")
        make_macos_png("open", ICONS["open"], 32, tmp_dir)
        make_macos_png("save", ICONS["save"], 32, tmp_dir)
        make_macos_png("help", ICONS["help"], 24, tmp_dir)
        make_macos_png("close", ICONS["close"], 16, tmp_dir)

        print("Generating toolbar preview")
        make_preview(tmp_dir, os.path.join(CUSTOM_DIR, "toolbar_preview.png"))

    print("Done.")


if __name__ == "__main__":
    main()
