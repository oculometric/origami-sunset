#pragma once

#include <stdint.h>
#include <vector>

struct ORIStar
{
    struct RightAscension
    {
        uint8_t hours;
        uint8_t minutes;
        float seconds;
    };

    struct Declination
    {
        int8_t degrees;
        int8_t minutes;
        float seconds;
    };

    const char* name;

    RightAscension ra;
    Declination dec;

    float dist;
    float abs_mag;
    float app_mag;

    /*inline ORIStar(const char* _name = nullptr, RightAscension _ra = { 0, 0, 0 }, Declination _dec = { 0, 0, 0 }, float _dist = 1.0f, float _abs_mag = 0.0f)
        : name(_name), ra(_ra), dec(_dec), dist(_dist), abs_mag(_abs_mag)
        { }*/
};

struct ORIConstellation
{
    const char* name;

    std::initializer_list<ORIStar> stars;
    std::initializer_list<uint16_t> edges;

    /*inline ORIConstellation(const char* _name = nullptr, std::vector<ORIStar> _stars = { }, std::vector<uint16_t> _edges = { })
        : name(_name), stars(_stars), edges(_edges)
        { }*/
};

class ORIConstellationViewer
{
public:
    static void initialiseConstellations();
    static void drawConstellations(float ascension, float declination, float fov);
};