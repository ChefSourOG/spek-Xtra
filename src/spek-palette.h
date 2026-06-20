#pragma once

#include <stdint.h>

class wxBitmap;

enum palette {
    PALETTE_SPECTRUM,
    PALETTE_SOX,
    PALETTE_MONO,
    PALETTE_RAINBOW,
    PALETTE_TEAL,
    PALETTE_HEAT,
    PALETTE_ICE,
    PALETTE_GRAYSCALE,
    PALETTE_COUNT,
    PALETTE_DEFAULT = PALETTE_SPECTRUM,
};

uint32_t spek_palette(enum palette palette, double level);
wxBitmap spek_palette_bitmap(enum palette palette, int width, int height);
