#include "constellation.h"

#include "screen.h"
#include "colours.h"
#include "compat.h"
#include "font8x16.h"

const ORIConstellation constellations[2] = 
{
    {
        "scorpius",
        {
            ORIStar{ "Antares A",   { 16, 29, 24.47f }, { -26, 25, 55.0f }, 553.0f, -5.24f, 0.91f }, // 0
            ORIStar{ "lambda Sco",  { 17, 33, 36.53f }, { -37,  6, 13.5f }, 703.0f, -5.05f, 1.62f },
            ORIStar{ "theta Sco",   { 17, 37, 19.13f }, { -42, 59, 52.2f }, 272.0f, -2.75f, 1.86f }, // 2
            ORIStar{ "delta Sco",   { 16,  0, 20.01f }, { -22, 37, 17.8f }, 401.0f, -3.16f, 2.29f },
            ORIStar{ "epsilon Sco", { 16, 50, 10.25f }, { -34, 17, 33.4f },  65.0f,  0.78f, 2.29f }, // 4
            ORIStar{ "kappa Sco",   { 17, 42, 29.28f }, { -38,  1, 47.7f }, 464.0f, -3.38f, 2.39f },
            ORIStar{ "Acrab",       { 16,  5, 26.23f }, { -19, 48, 19.4f }, 530.0f, -3.44f, 2.62f }, // 6
            ORIStar{ "upsilon Sco", { 17, 30, 45.84f }, { -37, 17, 44.7f }, 518.0f, -3.31f, 2.70f },
            ORIStar{ "tau Sco",     { 16, 35, 52.96f }, { -28, 12, 57.5f }, 430.0f, -2.78f, 2.82f }, // 8
            ORIStar{ "pi Sco",      { 15, 58, 51.12f }, { -26,  6, 50.6f }, 459.0f, -2.89f, 2.89f },
            ORIStar{ "sigma Sco",   { 16, 21, 11.32f }, { -25, 35, 33.9f }, 734.0f, -3.86f, 2.90f }, // 10
        },
        {
            0, 10,
            0, 8,
            10, 3,
            3, 6,
            3, 9,
            8, 4,
            4, 2,
            2, 5,
            5, 7,
            7, 1
        }
    },
    {
        "cassiopeia",
        {
            ORIStar{ "alpha Cas",   {  0, 40, 30.39f }, { 56, 32, 14.7f }, 228.0f, -1.99f, 2.24f },
            ORIStar{ "Caph",        {  0,  9, 10.09f }, { 59,  9,  0.8f }, 54.0f,   1.17f, 2.28f },
            ORIStar{ "gamma Cas",   {  0, 56, 42.50f }, { 60, 43,  0.3f }, 613.0f, -4.22f, 2.47f },
            ORIStar{ "delta Cas",   {  1, 25, 48.60f }, { 60, 14,  7.5f }, 99.0f,   0.25f, 2.68f },
            ORIStar{ "epsilon Cas", {  1, 54, 23.68f }, { 63, 40, 12.5f }, 442.0f, -2.31f, 3.35f }
        },
        {
            1, 0,
            0, 2,
            2, 3,
            3, 4
        }
    }
};

static float getDegrees(ORIStar::RightAscension ra)
{
    return (float)(ra.hours * 15) + ((float)ra.minutes * 0.25f) + (ra.seconds * 0.004166666667f);
}

static float getDegrees(ORIStar::Declination dec)
{
    return (float)(dec.degrees) + ((((float)dec.minutes * 0.0166666666667) + ((float)dec.seconds * 0.0002777777778f)) * csign(dec.degrees));
}

float angleDistance(float a)
{
    float f = fmod(a, 360.0f);
    if (f > 180.0f)
        return 360.0f - f;
    else if (f < -180.0f)
        return 360.0f + f;
    else if (f < 0)
        return -f;
    else
        return f;
}

const float pi_180 = 3.14159f / 180.0f;

inline void computeNormal(float asc, float dec, float vec[3])
{
    float ap = asc * pi_180;
    float dp = dec * pi_180;
    float cd = cos(dp);
    vec[0] = -sin(ap) * cd;
    vec[1] = cos(ap) * cd;
    vec[2] = sin(dp);
}

inline void createCameraRotationMatrix(float asc, float dec, float mat[9])
{
    // rotation around X, look up-down aka declination
    float ap = asc * pi_180;
    float dp = (dec + 90) * pi_180;
    //float ca = cos(ap);
    //float sa = sin(ap);
    //float cd = cos(dp);
    //float sd = sin(dp);
    /*
    float rx[9] = 
    {
        1.0f, 0.0f, 0.0f,
        0.0f,   cd,  -sd, 
        0.0f,   sd,   cd
    };
    float rz[9] =
    {
          ca,  -sa, 0.0f,
          sa,   ca, 0.0f,
        0.0f, 0.0f, 1.0f
    }; 
    */

    //// rz * rx
    //// and then the inverse

    //mat[0] = ca;
    //mat[1] = -sa * cd;
    //mat[2] = sa * sd;
    //mat[3] = sa;
    //mat[4] = ca * cd;
    //mat[5] = -sd * ca;
    //mat[6] = 0.0f;
    //mat[7] = sd;
    //mat[8] = cd;

    // or, -rx * -rz (rotate in opposite directions, in the opposite order)
    // eliminates the need for inverse
    float ca = cos(-ap);
    float sa = sin(-ap);
    float cd = cos(-dp);
    float sd = sin(-dp);
    mat[0] = ca;
    mat[1] = -sa;
    mat[2] = 0.0f;
    mat[3] = cd * sa;
    mat[4] = ca * cd;
    mat[5] = -sd;
    mat[6] = sa * sd;
    mat[7] = ca * sd;
    mat[8] = cd;
}

inline void multiplyMatVec(float mat[9], float vec[3])
{
    // 0, 1, 2       0
    // 3, 4, 5   x   1
    // 6, 7, 8       2

    float v0 = (mat[0] * vec[0]) + (mat[1] * vec[1]) + (mat[2] * vec[2]);
    float v1 = (mat[3] * vec[0]) + (mat[4] * vec[1]) + (mat[5] * vec[2]);
    float v2 = (mat[6] * vec[0]) + (mat[7] * vec[1]) + (mat[8] * vec[2]);
    
    vec[0] = v0;
    vec[1] = v1;
    vec[2] = v2;
}

// ht = tan((fov * pi_180) / 2)
// ta = tan(asc * pi_180)
// ix = (((ta / ht) + 1) / 2) * width
// ix = (((ta + ht) / ht) / 2) * width
// sw = width / (2 * ht)
// ix = (ta + ht) * sw
inline void oldProject(float asc, float dec, float tf[2], float sz[2], int16_t& ix, int16_t& iy)
{
    float ar = asc * pi_180;
    float dr = dec * pi_180;
    float fx = (tan(ar) / cos(dr)) + tf[0]; // the cosine hack
    float fy = (tan(dr) / cos(ar)) + tf[1];

    ix = (fx * sz[0]);
    iy = (fy * sz[1]);
}

inline void project(float asc, float dec, float camera[9], float tfi[2], float sh[2], int16_t& ix, int16_t& iy)
{
    float v[3] = { 0.0f };
    computeNormal(asc, dec, v);
    multiplyMatVec(camera, v);
    v[0] = (v[0] * tfi[0]) / -v[2];
    v[1] = (v[1] * tfi[1]) / -v[2];
    v[2] = 1.0f;

    ix = (v[0] + 1.0f) * sh[0];
    iy = (v[1] + 1.0f) * sh[1];
}

void ORIConstellationViewer::drawConstellations(float ascension, float declination, float fov)
{
    float tan_fov[2] = { 0 };
        tan_fov[0] = tan((fov * pi_180) / 2.0f);
        tan_fov[1] = tan_fov[0] * ((float)ORIScreen::getHeight() / (float)ORIScreen::getWidth());
    float size_fac[2] = { 0 };
        size_fac[0] = (float)ORIScreen::getWidth() / (2.0f * tan_fov[0]);
        size_fac[1] = (float)ORIScreen::getHeight() / (2.0f * tan_fov[1]);

    float tfi[2] = { 1.0f / tan_fov[0], 1.0f / tan_fov[1] };
    float sh[2] = { (float)ORIScreen::getWidth() * 0.5f, (float)ORIScreen::getHeight() * 0.5f };

    const float vfov = atan(tan_fov[1]) * 2.0f / pi_180;

    float cam_mat[9] = { 0.0f };
    createCameraRotationMatrix(ascension, declination, cam_mat);

    const int lat_lines = 36;
    const int long_lines = 36;
    const float lat_ang = 360.0f / (float)lat_lines;
    const float long_ang = 360.0f / (float)long_lines;
    const int ll_count = (lat_lines + 1) * (long_lines + 1);

    // draw grid
    int16_t ixg[ll_count] = { INT16_MAX };
    int16_t iyg[ll_count] = { INT16_MAX };
    bool vg[ll_count] = { false };
    int n = 0;
    for (int i = -1; i < lat_lines; i++)
    {
        for (int j = -1; j < long_lines; j++)
        {
            float asc = (j * -long_ang) + ascension;
            if (angleDistance(asc) > 90.0f)
            {
                vg[n] = false;
                ixg[n] = INT16_MAX;
                iyg[n] = INT16_MAX;
                n++;
                continue;
            }
            float dec = (i * lat_ang) - declination;
            if (angleDistance(dec) > 90.0f)
            {
                vg[n] = false;
                ixg[n] = INT16_MAX;
                iyg[n] = INT16_MAX;
                n++;
                continue;
            }

            project(j * -long_ang, i * lat_ang, cam_mat, tfi, sh, ixg[n], iyg[n]);
            vg[n] = (ixg[n] > 0 && ixg[n] < ORIScreen::getWidth()) && (iyg[n] > 0 && iyg[n] < ORIScreen::getHeight());
            if (vg[n])
                ORIScreen::drawCircle(ixg[n], iyg[n], 1, LGREY, LGREY);

            if (j >= 0 && (vg[n - 1] || vg[n]))
                ORIScreen::drawLine(ixg[n - 1] - 1, iyg[n - 1], ixg[n] + 2, iyg[n], DGREY);

            if (i >= 0 && (vg[n - (long_lines + 1)] || vg[n]))
                ORIScreen::drawLine(ixg[n - (long_lines + 1)], iyg[n - (long_lines + 1)] + 2, ixg[n], iyg[n] - 2, DGREY);

            n++;
        }
    }

    // draw constellations
    for (const ORIConstellation& constel : constellations)
    {
        int16_t ixs[255] = { INT16_MAX };
        int16_t iys[255] = { INT16_MAX };
        int i = 0;
        for (const ORIStar& star : constel.stars)
        {
            float ascension_angle = -getDegrees(star.ra) + ascension;
            if (angleDistance(ascension_angle) > 90.0f)
            {
                ixs[i] = INT16_MAX;
                iys[i] = INT16_MAX;
                i++;
                continue;
            }
            float declination_angle = getDegrees(star.dec) - declination;
            if (angleDistance(declination_angle) > 90.0f)
            {
                ixs[i] = INT16_MAX;
                iys[i] = INT16_MAX;
                i++;
                continue;
            }

            //project(ascension_angle, declination_angle, tan_fov, size_fac, ixs[i], iys[i]);
            project(getDegrees(star.ra), getDegrees(star.dec), cam_mat, tfi, sh, ixs[i], iys[i]);
            
            uint16_t r = 5;
            if (star.app_mag > 0.5f)
                r = 4;
            if (star.app_mag > 1.0f)
                r = 3;
            if (star.app_mag > 2.0f)
                r = 2;
            if (star.app_mag > 2.5f)
                r = 1;
            if (star.app_mag > 3.0f)
                r = 0;
            ORIScreen::drawCircle(ixs[i], iys[i], r, GOLD, GOLD);

            if (angleDistance(ascension_angle) < 2.0f && angleDistance(declination_angle) < 2.0f)
            {
                ORIScreen::drawText(ixs[i] + 10, iys[i], star.name, WHITE, &terminal_8x16_font);
            }

            i++;
        }

        auto pit = constel.edges.begin();
        while (pit != constel.edges.end())
        {
            int16_t sx = ixs[*pit];
            int16_t sy = iys[*pit];
            pit++;
            int16_t ex = ixs[*pit];
            int16_t ey = iys[*pit];
            pit++;

            if (sx == INT16_MAX || ex == INT16_MAX)
                continue;

            ORIScreen::drawLine(sx, sy, ex, ey, GOLD);
        }
    }
}
