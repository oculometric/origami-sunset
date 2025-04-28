#pragma once

#include <stdint.h>

class ORIFont
{
private:
    size_t glyph_width;
    size_t glyph_height;

    uint8_t* glyph_buffer;

public:
    inline size_t getGlyphWidth() const { return glyph_width; }
    inline size_t getGlyphHeight() const { return glyph_height; }

    inline uint8_t* getGlyphData(unsigned char glyph) const { return glyph_buffer + ((glyph * (glyph_width + 4)) / 8); }


};