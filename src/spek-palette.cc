#include <assert.h>
#include <math.h>

#include <wx/bitmap.h>
#include <wx/image.h>

#include "spek-palette.h"

static uint32_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 16) | (g << 8) | b;
}

static uint32_t lerp_color(uint32_t c1, uint32_t c2, double t)
{
    uint8_t r1 = (c1 >> 16) & 0xFF;
    uint8_t g1 = (c1 >> 8) & 0xFF;
    uint8_t b1 = c1 & 0xFF;
    uint8_t r2 = (c2 >> 16) & 0xFF;
    uint8_t g2 = (c2 >> 8) & 0xFF;
    uint8_t b2 = c2 & 0xFF;

    uint8_t r = (uint8_t)(r1 + (r2 - r1) * t + 0.5);
    uint8_t g = (uint8_t)(g1 + (g2 - g1) * t + 0.5);
    uint8_t b = (uint8_t)(b1 + (b2 - b1) * t + 0.5);
    return rgb(r, g, b);
}

static uint32_t gradient(
    const uint32_t *stops, const double *positions, int count, double level)
{
    if (level <= positions[0]) {
        return stops[0];
    }
    if (level >= positions[count - 1]) {
        return stops[count - 1];
    }
    for (int i = 0; i < count - 1; ++i) {
        if (level >= positions[i] && level < positions[i + 1]) {
            double t = (level - positions[i]) / (positions[i + 1] - positions[i]);
            return lerp_color(stops[i], stops[i + 1], t);
        }
    }
    return stops[count - 1];
}

// Modified version of Dan Bruton's algorithm:
// http://www.physics.sfasu.edu/astro/color/spectra.html
static uint32_t spectrum(double level)
{
    level *= 0.6625;
    double r = 0.0, g = 0.0, b = 0.0;
    if (level >= 0 && level < 0.15) {
        r = (0.15 - level) / (0.15 + 0.075);
        g = 0.0;
        b = 1.0;
    } else if (level >= 0.15 && level < 0.275) {
        r = 0.0;
        g = (level - 0.15) / (0.275 - 0.15);
        b = 1.0;
    } else if (level >= 0.275 && level < 0.325) {
        r = 0.0;
        g = 1.0;
        b = (0.325 - level) / (0.325 - 0.275);
    } else if (level >= 0.325 && level < 0.5) {
        r = (level - 0.325) / (0.5 - 0.325);
        g = 1.0;
        b = 0.0;
    } else if (level >= 0.5 && level < 0.6625) {
        r = 1.0;
        g = (0.6625 - level) / (0.6625 - 0.5f);
        b = 0.0;
    }

    // Intensity correction.
    double cf = 1.0;
    if (level >= 0.0 && level < 0.1) {
        cf = level / 0.1;
    }
    cf *= 255.0;

    // Pack RGB values into a 32-bit uint.
    uint32_t rr = (uint32_t) (r * cf + 0.5);
    uint32_t gg = (uint32_t) (g * cf + 0.5);
    uint32_t bb = (uint32_t) (b * cf + 0.5);
    return (rr << 16) + (gg << 8) + bb;
}

// The default palette used by SoX and written by Rob Sykes.
static uint32_t sox_compute(double level)
{
    double r = 0.0;
    if (level >= 0.13 && level < 0.73) {
        r = sin((level - 0.13) / 0.60 * M_PI / 2.0);
    } else if (level >= 0.73) {
        r = 1.0;
    }

    double g = 0.0;
    if (level >= 0.6 && level < 0.91) {
        g = sin((level - 0.6) / 0.31 * M_PI / 2.0);
    } else if (level >= 0.91) {
        g = 1.0;
    }

    double b = 0.0;
    if (level < 0.60) {
        b = 0.5 * sin(level / 0.6 * M_PI);
    } else if (level >= 0.78) {
        b = (level - 0.78) / 0.22;
    }

    // Pack RGB values into a 32-bit uint.
    uint32_t rr = (uint32_t) (r * 255.0 + 0.5);
    uint32_t gg = (uint32_t) (g * 255.0 + 0.5);
    uint32_t bb = (uint32_t) (b * 255.0 + 0.5);
    return (rr << 16) + (gg << 8) + bb;
}

static uint32_t sox(double level)
{
    static uint32_t table[256];
    static bool initialized = false;
    if (!initialized) {
        for (int i = 0; i < 256; ++i) {
            table[i] = sox_compute(i / 255.0);
        }
        initialized = true;
    }

    double idx = level * 255.0;
    int i = (int)idx;
    if (i < 0) {
        return table[0];
    }
    if (i >= 255) {
        return table[255];
    }
    return lerp_color(table[i], table[i + 1], idx - i);
}

static uint32_t mono(double level)
{
    uint8_t v = (uint8_t)(level * 255.0 + 0.5);
    return rgb(0, v, 0);
}

static uint32_t rainbow(double level)
{
    static const uint32_t stops[] = {
        rgb(128, 0, 128),   // purple
        rgb(0, 0, 255),     // blue
        rgb(0, 255, 255),   // cyan
        rgb(0, 255, 0),     // green
        rgb(255, 255, 0),   // yellow
        rgb(255, 0, 0),     // red
        rgb(255, 255, 255), // white
    };
    static const double positions[] = {0.0, 0.16, 0.33, 0.5, 0.66, 0.83, 1.0};
    return gradient(stops, positions, 7, level);
}

static uint32_t teal(double level)
{
    static const uint32_t stops[] = {
        rgb(0, 0, 0),       // black
        rgb(0, 128, 128),   // teal
        rgb(0, 255, 255),   // cyan
        rgb(255, 255, 255), // white
    };
    static const double positions[] = {0.0, 0.33, 0.66, 1.0};
    return gradient(stops, positions, 4, level);
}

static uint32_t heat(double level)
{
    static const uint32_t stops[] = {
        rgb(0, 0, 0),       // black
        rgb(255, 0, 0),     // red
        rgb(255, 128, 0),   // orange
        rgb(255, 255, 0),   // yellow
        rgb(255, 255, 255), // white
    };
    static const double positions[] = {0.0, 0.25, 0.5, 0.75, 1.0};
    return gradient(stops, positions, 5, level);
}

static uint32_t ice(double level)
{
    static const uint32_t stops[] = {
        rgb(0, 0, 0),       // black
        rgb(0, 0, 128),     // navy
        rgb(0, 0, 255),     // blue
        rgb(255, 255, 255), // white
    };
    static const double positions[] = {0.0, 0.33, 0.66, 1.0};
    return gradient(stops, positions, 4, level);
}

static uint32_t grayscale(double level)
{
    uint32_t v = (uint32_t)(level * 255.0 + 0.5);
    return rgb(v, v, v);
}

uint32_t spek_palette(enum palette palette, double level)
{
    switch (palette) {
    case PALETTE_SPECTRUM:
        return spectrum(level);
    case PALETTE_SOX:
        return sox(level);
    case PALETTE_MONO:
        return mono(level);
    case PALETTE_RAINBOW:
        return rainbow(level);
    case PALETTE_TEAL:
        return teal(level);
    case PALETTE_HEAT:
        return heat(level);
    case PALETTE_ICE:
        return ice(level);
    case PALETTE_GRAYSCALE:
        return grayscale(level);
    default:
        assert(false);
        return 0;
    }
}

wxBitmap spek_palette_bitmap(enum palette palette, int width, int height)
{
    wxImage image(width, height);
    for (int x = 0; x < width; ++x) {
        double level = (width > 1) ? (double)x / (width - 1) : 1.0;
        uint32_t color = spek_palette(palette, level);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        for (int y = 0; y < height; ++y) {
            image.SetRGB(x, y, r, g, b);
        }
    }
    return wxBitmap(image);
}
