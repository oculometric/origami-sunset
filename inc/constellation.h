#pragma once

#include <stdint.h>
#include <vector>

#pragma pack(push)
#pragma pack(2)
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

    const char byr_ident[16];
    uint16_t flm_ident;
    uint32_t hd_ident;
    uint32_t hip_ident;
    uint16_t ngc_ident;
    uint8_t mes_ident;

    /*constexpr ORIStar(const char* _name = nullptr,
        RightAscension _ra = { 0, 0, 0.0f }, Declination _dec = { 0, 0, 0.0f },
        float _dist = 0.0f,
        float _abs_mag = 0.0f, float _app_mag = 0.0f,
        const char _byr_ident[16],
        uint32_t _hd_ident = 0,
        uint32_t _hip_ident = 0,
        uint16_t _flm_ident = 0,
        uint16_t _ngc_ident = 0,
        uint8_t _mes_ident = 0
        ) : name(_name), 
        ra(_ra), dec(_dec), 
        dist(_dist), 
        abs_mag(_abs_mag), app_mag(_app_mag),
        byr_ident(_byr_ident),
        hd_ident(_hd_ident),
        hip_ident(_hip_ident),
        flm_ident(_flm_ident),
        ngc_ident(_ngc_ident),
        mes_ident(_mes_ident) 
        { }*/
};

struct ORIConstellation
{
    const char* name;

    std::initializer_list<ORIStar> stars;
    std::initializer_list<uint16_t> edges;
    std::initializer_list<std::pair<float, float>> boundary;

    /*inline ORIConstellation(const char* _name = nullptr, std::vector<ORIStar> _stars = { }, std::vector<uint16_t> _edges = { })
        : name(_name), stars(_stars), edges(_edges)
        { }*/
};

struct ORIStar2
{
    const char* names; // multiple names separated by '%'
    const char* description;
    uint8_t constellation_index; // 0 - 87
    uint8_t internal_type; // star, multi-star, var-star, star cluster, galaxy, nebula, etc

    uint16_t classification;
    double ra_dec[2];
    double lat_long[2];
    float distance;
    float visual_magnitude;

    const char byr_ident[10];
    const char flm_ident[6];
    uint32_t hd_ident;
    uint32_t hip_ident;
    uint16_t ngc_ident;
    uint8_t mes_ident;
    uint16_t ic_ident;
    uint16_t hr_ident;
    uint32_t sao_ident;
};
#pragma pack(pop)

class ORIConstellationViewer
{
public:
    static void initialiseConstellations();
    static void drawConstellations(float ascension, float declination, float fov);
    static void drawOverlay();
};