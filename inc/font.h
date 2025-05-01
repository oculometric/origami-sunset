#pragma once

#include <stdint.h>

// glyph data must be stored in a continuous array where each row of the glyph is represented by one or more bytes (padded on the right hand side to be a whole number of bytes)

class ORIFont
{
private:
    uint32_t glyph_width = 8;
    uint32_t glyph_height = 16;

    uint8_t* glyph_buffer;

public:
    inline uint32_t getGlyphWidth() const { return glyph_width; }
    inline uint32_t getGlyphHeight() const { return glyph_height; }

    inline uint8_t* getGlyphData(unsigned char glyph) const { return glyph_buffer + (getGlyphDataSize() * glyph); }
    inline uint32_t getGlyphDataSize() const { return ((glyph_width + 4) / 8) * glyph_height; }

    inline ORIFont(uint32_t width, uint32_t height, uint8_t* data) : glyph_width(width), glyph_height(height), glyph_buffer(data) { }
};
