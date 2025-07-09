#include "constellation.h"

#include "screen.h"
#include "colours.h"
#include "compat.h"
#include "font8x16.h"
#include "log.h"

#include "constellation_data.h"

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

    // -rx * -rz (rotate in opposite directions, in the opposite order)
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

inline bool project(const float vector[3], float camera[9], float tfish[2], float sh[2], int16_t sz[2], int16_t& ix, int16_t& iy)
{
    float v[3] = { vector[0], vector[1], vector[2] };
    multiplyMatVec(camera, v);
    float inv_depth = -1.0f / v[2];
    ix = (v[0] * inv_depth * tfish[0]) + sh[0];
    iy = (v[1] * inv_depth * tfish[1]) + sh[1];

    if (inv_depth < 0.0f)
        return false;
    if (ix < 0 || ix >= sz[0])
        return false;
    if (iy < 0 || iy >= sz[1])
        return false;
    return true;
}

void ORIConstellationViewer::initialiseConstellations()
{
    size_t database_size = 0;
    for (const ORIConstellation& constel : constellations)
    {
        database_size += sizeof(constel);
        auto it = constel.stars.begin();
        while (it != constel.stars.end())
        {
            computeNormal(getDegrees((*it).ra), getDegrees((*it).dec), (float*)((*it).vector));
            database_size += sizeof(*it);
            database_size += strlen((*it).name) + 1;
            it++;
        }
        database_size += constel.edges.size() * sizeof(uint16_t);
    }
    ORISerial::print((uint32_t)database_size, 10);
    ORISerial::printLn("");
}

// before modifications: 0.14ms
// after improving mult in project: 0.12ms
// after adding more constellations: 0.44ms
// with optimisations: 0.13ms
// disabling grid: 0.03ms!!!
// with grid dot product filtering: 0.11ms

void ORIConstellationViewer::drawConstellations(float ascension, float declination, float fov)
{
    int16_t sz[2] = { ORIScreen::getWidth(), ORIScreen::getHeight() };
    float tan_fov[2] = { 0 };
        tan_fov[0] = tan((fov * pi_180) / 2.0f);
        tan_fov[1] = tan_fov[0] * ((float)sz[1] / (float)sz[0]);

    float tfi[2] = { 1.0f / tan_fov[0], 1.0f / tan_fov[1] };
    float sh[2] = { (float)sz[0] * 0.5f, (float)sz[1] * 0.5f };
    float tfish[2] = { tfi[0] * sh[0], tfi[1] * sh[1] };

    int16_t hrad = sz[0] / (fov / 1.5f);
    int16_t hlim[4] =
    {
        (sz[0] / 2) - hrad,
        (sz[0] / 2) + hrad,
        (sz[1] / 2) - hrad,
        (sz[1] / 2) + hrad
    };

    const float vfov = atan(tan_fov[1]) * 2.0f / pi_180;
    const float mcf = cos((vfov > fov ? vfov : fov) * pi_180);

    float cam_mat[9] = { 0.0f };
    createCameraRotationMatrix(ascension, declination, cam_mat);

    const int lat_lines = 36;
    const int long_lines = 18;
    const float lat_ang = 360.0f / (float)lat_lines;
    const float long_ang = 180.0f / (float)long_lines;
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
            float v[3] = { 0.0f };
            // TODO: pregenerate all of these
            computeNormal(j * long_ang, i * lat_ang, v);
            float dn = (cam_mat[6] * v[0]) + (cam_mat[7] * v[1]) + (cam_mat[8] * v[2]);
            if (-dn < mcf)
            {
                n++;
                continue;
            }
            vg[n] = project(v, cam_mat, tfish, sh, sz, ixg[n], iyg[n]);
            //if (vg[n])
            //    ORIScreen::drawCircle(ixg[n], iyg[n], 1, LGREY, LGREY);

            if (j >= 0 && (vg[n - 1] || vg[n]))
                ORIScreen::drawLine(ixg[n], iyg[n], ixg[n - 1], iyg[n - 1], DGREY);

            if (i >= 0 && (vg[n - (long_lines + 1)] || vg[n]))
                ORIScreen::drawLine(ixg[n - (long_lines + 1)], iyg[n - (long_lines + 1)], ixg[n], iyg[n], DGREY);

            n++;
        }
    }

    // draw constellations
    for (const ORIConstellation& constel : constellations)
    {
        int16_t ixs[255] = { INT16_MAX };
        int16_t iys[255] = { INT16_MAX };
        bool vs[255] = { false };
        int i = 0;
        for (const ORIStar& star : constel.stars)
        {
            vs[i] = project(star.vector, cam_mat, tfish, sh, sz, ixs[i], iys[i]);
            if (!vs[i])
            {
                i++;
                continue;
            }

            // TODO: star radius should depend on zoom
            // TODO: better offscreen trimming so that lines that cross the screen are still drawn
            uint16_t r = 5;
            if (star.app_mag > 0.5f) r = 4;
            if (star.app_mag > 1.0f) r = 3;
            if (star.app_mag > 2.0f) r = 2;
            if (star.app_mag > 4.0f) r = 1;
            if (star.app_mag > 6.0f) r = 0;
            ORIScreen::drawCircle(ixs[i], iys[i], r, GOLD, GOLD);

            if (ixs[i] > hlim[0] && ixs[i] < hlim[1] && iys[i] > hlim[2] && iys[i] < hlim[3])
                ORIScreen::drawText(ixs[i] + 10, iys[i], star.name, WHITE, &terminal_8x16_font);

            i++;
        }

        auto pit = constel.edges.begin();
        while (pit != constel.edges.end())
        {
            uint16_t i0 = *pit;
            int16_t sx = ixs[i0];
            int16_t sy = iys[i0];
            pit++;
            uint16_t i1 = *pit;
            int16_t ex = ixs[i1];
            int16_t ey = iys[i1];
            pit++;

            if (!vs[i0] && !vs[i1])
                continue;

            ORIScreen::drawLine(sx, sy, ex, ey, GOLD);
        }
    }
}
