#pragma once

#include <stdint.h>
#include <vector>

struct ORIStar
{
    struct RightAscension
    {
        int8_t hours;
        int8_t minutes;
        float seconds;
    };

    struct Declination
    {
        int16_t degrees;
        int16_t minutes;
        float seconds;
    };

    const char* name = nullptr;

    RightAscension ra;
    Declination dec;

    float dist;
    float abs_mag;
};

struct ORIConstellation
{
    const char* name = nullptr;

    std::vector<ORIStar> stars;
    std::vector<uint16_t> edges;
};

class ORIConstellationViewer
{
public:
    static void drawConstellations(float ascension, float declination);
};