#pragma once

#include <stdint.h>

enum ORIColour : uint16_t
{
    BLACK   = 0x0000,
    WHITE   = 0xFFFF,
    LGREY   = 0b1100011000011000,
    DGREY   = 0b0010100100000111,
    RED     = 0xF800,
    GREEN   = 0x07E0,
    BLUE    = 0x001F,
    MAGENTA = RED | BLUE,
    YELLOW  = RED | GREEN,
    CYAN    = GREEN | BLUE,
    GOLD    = 0b1111010111100000
};

inline uint16_t colourFromRGB(float r, float g, float b)
{
    uint16_t red = (uint16_t)(r * 31.0f) << 11;
    uint16_t green = (uint16_t)(g * 63.0f) << 5;
    uint16_t blue = (uint16_t)(b * 31.0f);
    return red | green | blue;
}