#include "../inc/constellation.h"

#include "../inc/screen.h"
#include "../inc/colours.h"
#include "../inc/compat.h"
#include "../inc/font8x16.h"
#include "../inc/log.h"

#include "../inc/constellation_data.h"

static float getDegrees(ORIStar::RightAscension ra)
{
    return (float)(ra.hours * 15) + ((float)ra.minutes * 0.25f) + (ra.seconds * 0.004166666667f);
}

static float getDegrees(ORIStar::Declination dec)
{
    return (float)(dec.degrees) + ((((float)dec.minutes * 0.0166666666667f) + ((float)dec.seconds * 0.0002777777778f)) * csign(dec.degrees));
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
//inline void oldProject(float asc, float dec, float tf[2], float sz[2], int16_t& ix, int16_t& iy)
//{
//    float ar = asc * pi_180;
//    float dr = dec * pi_180;
//    float fx = (tan(ar) / cos(dr)) + tf[0]; // the cosine hack
//    float fy = (tan(dr) / cos(ar)) + tf[1];
//
//    ix = (fx * sz[0]);
//    iy = (fy * sz[1]);
//}

inline uint8_t project(const float vector[3], float camera[9], float tfish[2], float sh[2], int32_t sz[2], int16_t& ix, int16_t& iy)
{
    float v[3] = { vector[0], vector[1], vector[2] };
    multiplyMatVec(camera, v);
    float inv_depth = -1.0f / v[2];
    int32_t tix = (v[0] * inv_depth * tfish[0]) + sh[0];
    int32_t tiy = (v[1] * inv_depth * tfish[1]) + sh[1];
    ix = tix;
    iy = tiy;

    if (inv_depth <= 0.0f)
        return 0b11111111; // offscreen behind
    uint8_t ret = 0;
    if (tix < 0)
        ret |= 0b00001000;
    else if (tix >= sz[0])
        ret |= 0b00000100;
    if (tiy < 0)
        ret |= 0b00000010;
    else if (tiy >= sz[1])
        ret |= 0b00000001;
    return ret;
}

static float* star_vectors = nullptr;

void ORIConstellationViewer::initialiseConstellations()
{
    size_t database_size = 0;
    size_t constels = 0;
    size_t stars = 0;
    size_t max_star_name_length = 0;

    for (const ORIConstellation& constel : constellations)
    {
        database_size += sizeof(constel);
        constels++;
        auto it = constel.stars.begin();
        while (it != constel.stars.end())
        {
            stars++;
            database_size += sizeof(*it);
            database_size += strlen((*it).name) + 1;
            size_t name_len = strlen((*it).name) + strlen(constel.name) + 3;
            if (name_len > max_star_name_length)
                max_star_name_length = name_len;
            it++;
        }
        database_size += constel.edges.size() * sizeof(uint16_t);
    }

    star_vectors = new float[3 * stars];
    float* vector = star_vectors;

    for (const ORIConstellation& constel : constellations)
    {
        ORISerial::print("computing vectors within ");
        ORISerial::printLn(constel.name);
        auto it = constel.stars.begin();
        while (it != constel.stars.end())
        {
            computeNormal(getDegrees((*it).ra), getDegrees((*it).dec), vector);
            it++;
            vector += 3;
        }
        database_size += constel.edges.size() * sizeof(uint16_t);
    }
    ORISerial::print("constellations      : ");
    ORISerial::print((uint32_t)constels);
    ORISerial::print("\nstars               : ");
    ORISerial::print((uint32_t)stars);
    ORISerial::print("\nlongest summary name: ");
    ORISerial::print((uint32_t)max_star_name_length);
    ORISerial::print("\ntotal database size : ");
    ORISerial::print((uint32_t)database_size);
    ORISerial::printLn(" bytes.");
}

// before modifications: 0.14ms
// after improving mult in project: 0.12ms
// after adding more constellations: 0.44ms
// with optimisations: 0.13ms
// disabling grid: 0.03ms!!!
// with grid dot product filtering: 0.11ms
// on the device: 7.95ms (no optimisation?)

static const ORIConstellation* active_constellation = nullptr;
static const ORIStar* active_star = nullptr;
static int16_t active_dist = 0;
static int16_t active_x = 0;
static int16_t active_y = 0;

void ORIConstellationViewer::drawConstellations(float ascension, float declination, float fov)
{
    int32_t sz[2] = { ORIScreen::getWidth(), ORIScreen::getHeight() };
    float tan_fov[2] = { 0 };
        tan_fov[0] = tanf((fov * pi_180) / 2.0f);
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
    int16_t shi[2] = { sz[0] / 2, sz[1] / 2 };

    const float vfov = atanf(tan_fov[1]) * 2.0f / pi_180;
    const float mcf = cosf((vfov > fov ? vfov : fov) * pi_180) / 2.5f;

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
    uint8_t vg[ll_count] = { 0b11111111 };
    int n = 0;
    for (int i = -1; i < lat_lines; i++)
    {
        for (int j = -1; j < long_lines; j++)
        {
            float v[3] = { 0.0f };
            // TODO: pregenerate all of these
            computeNormal(j * long_ang, i * lat_ang, v);
            float dn = (cam_mat[6] * v[0]) + (cam_mat[7] * v[1]) + (cam_mat[8] * v[2]);
           /* if (-dn < mcf)
            {
                n++;
                vg[n] = 0b11111111;
                continue;
            }*/
            vg[n] = project(v, cam_mat, tfish, sh, sz, ixg[n], iyg[n]);
            /*if (vg[n])
                ORIScreen::setPixel(ixg[n], iyg[n], LGREY);*/

            if (j >= 0 && (vg[n - 1] & vg[n]) == 0)
                ORIScreen::drawLine(ixg[n], iyg[n], ixg[n - 1], iyg[n - 1], DGREY);

            if (i >= 0 && (vg[n - (long_lines + 1)] & vg[n]) == 0)
                ORIScreen::drawLine(ixg[n - (long_lines + 1)], iyg[n - (long_lines + 1)], ixg[n], iyg[n], DGREY);

            n++;
        }
    }

    // draw constellations
    active_dist = INT16_MAX;
    active_star = nullptr;
    active_constellation = nullptr;

    float* vector = star_vectors;
    for (const ORIConstellation& constel : constellations)
    {
        int16_t ixs[255] = { INT16_MAX };
        int16_t iys[255] = { INT16_MAX };
        uint8_t vs[255] = { 0b11111111 };
        int i = 0;
        for (const ORIStar& star : constel.stars)
        {
            vs[i] = project(vector, cam_mat, tfish, sh, sz, ixs[i], iys[i]);
            vector += 3;
            if (vs[i] != 0)
            {
                i++;
                continue;
            }

            int16_t r = 6;
            if (star.app_mag > 0.5f) r = 5;
            if (star.app_mag > 1.0f) r = 4;
            if (star.app_mag > 2.0f) r = 3;
            if (star.app_mag > 4.0f) r = 2;
            if (star.app_mag > 5.0f) r = 1;

            if (r > 1)
                ORIScreen::drawCircle(ixs[i], iys[i], r - 1, GOLD, GOLD);
            else if (r > 0)
                ORIScreen::setPixel(ixs[i], iys[i], GOLD);

            // TOOD: implement behaviour for detecting duplicate/overlaid stars?
            int16_t dist = abs(ixs[i] - shi[0]) + abs(iys[i] - shi[1]) + (int16_t)(star.app_mag * 4.5);
            if (dist < 100 && dist < active_dist)
            {
                active_constellation = &constel;
                active_star = &star;
                active_dist = dist;
                active_x = ixs[i];
                active_y = iys[i];
            }

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

            if ((vs[i0] & vs[i1]) == 0)
                ORIScreen::drawLine(sx, sy, ex, ey, GOLD);
        }
    }
}

void ORIConstellationViewer::drawOverlay()
{
    int16_t hx = ORIScreen::getWidth() / 2;
    int16_t hy = ORIScreen::getHeight() / 2;
    ORIScreen::drawLine(hx - 4, hy, hx + 5, hy, RED);
    ORIScreen::drawLine(hx, hy - 4, hx, hy + 5, RED);

    if (active_star == nullptr)
        return;

    static const int box_sz = 6;
    // bl
    ORIScreen::drawLine(active_x - box_sz, active_y - 3, active_x - box_sz, active_y - box_sz, LGREY);
    ORIScreen::drawLine(active_x - box_sz, active_y - box_sz, active_x - 2, active_y - box_sz, LGREY);
    // br
    ORIScreen::drawLine(active_x + 3, active_y - box_sz, active_x + box_sz, active_y - box_sz, LGREY);
    ORIScreen::drawLine(active_x + box_sz, active_y - box_sz, active_x + box_sz, active_y - 2, LGREY);
    // tr
    ORIScreen::drawLine(active_x + box_sz, active_y + 3, active_x + box_sz, active_y + box_sz + 1, LGREY);
    ORIScreen::drawLine(active_x + box_sz, active_y + box_sz, active_x + 3, active_y + box_sz, LGREY);
    // tl
    ORIScreen::drawLine(active_x - 2, active_y + box_sz, active_x - box_sz, active_y + box_sz, LGREY);
    ORIScreen::drawLine(active_x - box_sz, active_y + box_sz, active_x - box_sz, active_y + 2, LGREY);
    // label
    ORIScreen::drawText(hx + 10, hy, active_star->name, LGREY, &terminal_8x16_font);

    ORIScreen::fillPixels(0, 0, hx * 2, 20, DGREY);
    int16_t ex_sizex = ((16 * 9) + 12);
    int16_t ex_sizey = ((9 * 18) + 2);
    int16_t ex_offsetx = 0;//(hx * 2) - ex_sizex;
    int16_t ex_offsety = (hy * 2) - (ex_sizey + 40);
    ORIScreen::fillPixels(ex_offsetx, ex_offsety, ex_sizex, ex_sizey, DGREY);
    ex_offsety = (hy * 2) - (2 + 16 + 40);

    unsigned int buffer_size = 256;
    char* namebuf = new char[buffer_size + 1];
    int16_t offset = 0;

    // star and constellation name
    {
        memset(namebuf, 0, buffer_size);
        memcpy(namebuf, active_star->name, strlen(active_star->name));
        memcpy(namebuf + strlen(active_star->name) + 2, active_constellation->name, strlen(active_constellation->name));
        namebuf[strlen(active_star->name)] = ' ';
        namebuf[strlen(active_star->name) + 1] = '(';
        namebuf[strlen(active_star->name) + strlen(active_constellation->name) + 2] = ')';
        ORIScreen::drawText(6, 2, namebuf, LGREY, &terminal_8x16_font);
    }

    // ra + dec
    {
        memset(namebuf, 0, buffer_size);
        namebuf[0] = 'R';
        namebuf[1] = 'A';
        namebuf[2] = ' ';
        if (active_star->ra.hours < 10)
            namebuf[3] = '0';
        _itoa_s(active_star->ra.hours, namebuf + (active_star->ra.hours < 10 ? 4 : 3), 4, 10);
        offset = hx;
        namebuf[5] = 'h';
        namebuf[6] = ' ';
        if (active_star->ra.minutes < 10)
            namebuf[7] = '0';
        _itoa_s(active_star->ra.minutes, namebuf + (active_star->ra.minutes < 10 ? 8 : 7), 4, 10);
        namebuf[9] = 'm';
        namebuf[10] = ' ';
        if (active_star->ra.seconds < 10)
            namebuf[11] = '0';
        _itoa_s(floorf(active_star->ra.seconds), namebuf + (active_star->ra.seconds < 10 ? 12 : 11), 4, 10);
        namebuf[13] = '.';
        float s2 = (abs(active_star->ra.seconds) - floorf(abs(active_star->ra.seconds))) * 100.0f;
        if (s2 < 10)
            namebuf[14] = '0';
        _itoa_s(floorf(s2), namebuf + (s2 < 10 ? 15 : 14), 4, 10);
        namebuf[16] = 's';

        namebuf[17] = ' ';
        namebuf[18] = ' ';
        namebuf[19] = ' ';

        namebuf[20] = 'D';
        namebuf[21] = 'E';
        namebuf[22] = 'C';
        namebuf[23] = ' ';
        char* tmp = namebuf + 25;
        if (active_star->dec.degrees < 0)
            tmp[-1] = '-';
        else
            tmp[-1] = '+';
        if (abs(active_star->dec.degrees) < 10)
            tmp[0] = '0';
        _itoa_s(abs(active_star->dec.degrees), tmp + (abs(active_star->dec.degrees) < 10 ? 1 : 0), 4, 10);
        tmp[2] = 'd';
        tmp[3] = ' ';
        if (active_star->dec.minutes < 10)
            tmp[4] = '0';
        _itoa_s(active_star->dec.minutes, tmp + (active_star->dec.minutes < 10 ? 5 : 4), 4, 10);
        tmp[6] = 'm';
        tmp[7] = ' ';
        if (active_star->dec.seconds < 10)
            tmp[8] = '0';
        _itoa_s(floorf(active_star->dec.seconds), tmp + (active_star->dec.seconds < 10 ? 9 : 8), 4, 10);
        tmp[10] = '.';
        s2 = (abs(active_star->dec.seconds) - floorf(abs(active_star->dec.seconds))) * 100.0f;
        if (s2 < 10)
            tmp[11] = '0';
        _itoa_s(floorf(s2), tmp + (s2 < 10 ? 12 : 11), 4, 10);
        tmp[13] = 's';

        offset = (hx * 2) - ((39 * 9) + 6);
        ORIScreen::drawText(offset, 2, namebuf, LGREY, &terminal_8x16_font);
    }

    // magnitude
    {
        memset(namebuf, 0, buffer_size);
        namebuf[0] = 'A';
        namebuf[1] = 'P';
        namebuf[2] = 'P';
        namebuf[3] = '_';
        namebuf[4] = 'M';
        namebuf[5] = 'A';
        namebuf[6] = 'G';
        namebuf[7] = ' ';
        size_t coff = 8;
        if (active_star->app_mag < 0)
            namebuf[coff] = '-';
        else namebuf[coff] = '+';
        if (abs(active_star->app_mag) < 1)
            namebuf[coff + 1] = '0';
        else
            _itoa_s(floorf(abs(active_star->app_mag)), namebuf + coff + 1, 4, 10);
        namebuf[coff + 2] = '.';
        float s2 = (abs(active_star->app_mag) - floorf(abs(active_star->app_mag))) * 100.0f;
        if (s2 < 10)
            namebuf[coff + 3] = '0';
        _itoa_s(floorf(s2), namebuf + coff + (s2 < 10 ? 4 : 3), 4, 10);

        ORIScreen::drawText(ex_offsetx + 6, ex_offsety, namebuf, LGREY, &terminal_8x16_font);
        ex_offsety -= 18;

        memset(namebuf, 0, buffer_size);
        namebuf[0] = 'A';
        namebuf[1] = 'B';
        namebuf[2] = 'S';
        namebuf[3] = '_';
        namebuf[4] = 'M';
        namebuf[5] = 'A';
        namebuf[6] = 'G';
        namebuf[7] = ' ';

        coff = 8;
        if (active_star->abs_mag < 0)
            namebuf[coff] = '-';
        else namebuf[coff] = '+';
        if (abs(active_star->abs_mag) < 1)
            namebuf[coff + 1] = '0';
        else
            _itoa_s(floorf(abs(active_star->abs_mag)), namebuf + coff + 1, 4, 10);
        namebuf[coff + 2] = '.';
        s2 = (abs(active_star->abs_mag) - floorf(abs(active_star->abs_mag))) * 100.0f;
        if (s2 < 10)
            namebuf[coff + 3] = '0';
        _itoa_s(floorf(s2), namebuf + coff + (s2 < 10 ? 4 : 3), 4, 10);

        ORIScreen::drawText(ex_offsetx + 6, ex_offsety, namebuf, LGREY, &terminal_8x16_font);
        ex_offsety -= 18;
    }

    // distance
    {
        memset(namebuf, 0, buffer_size);
        namebuf[0] = 'D';
        namebuf[1] = 'I';
        namebuf[2] = 'S';
        namebuf[3] = 'T';
        namebuf[4] = '_';
        namebuf[5] = 'L';
        namebuf[6] = 'Y';
        namebuf[7] = ' ';
        _itoa_s((int)active_star->dist, namebuf + 8, 8, 10);

        ORIScreen::drawText(ex_offsetx + 6, ex_offsety, namebuf, LGREY, &terminal_8x16_font);
        ex_offsety -= 18;
    }

    // bayer
    {
        memset(namebuf, 0, buffer_size);
        namebuf[0] = ' ';
        namebuf[1] = ' ';
        namebuf[2] = 'B';
        namebuf[3] = 'A';
        namebuf[4] = 'Y';
        namebuf[5] = 'E';
        namebuf[6] = 'R';
        namebuf[7] = ' ';
        if (active_star->byr_ident[0] == '\0')
            namebuf[8] = '-';
        else
            memcpy(namebuf + 8, active_star->byr_ident, sizeof(ORIStar::byr_ident));

        ORIScreen::drawText(ex_offsetx + 6, ex_offsety, namebuf, LGREY, &terminal_8x16_font);
        ex_offsety -= 18;
    }

    // flm
    {
        memset(namebuf, 0, buffer_size);
        namebuf[0] = 'F';
        namebuf[1] = 'L';
        namebuf[2] = 'A';
        namebuf[3] = 'M';
        namebuf[4] = 'S';
        namebuf[5] = 'T';
        namebuf[6] = 'D';
        namebuf[7] = ' ';
        if (active_star->flm_ident == 0)
            namebuf[8] = '-';
        else
            _itoa_s((int)active_star->flm_ident, namebuf + 8, 12, 10);

        ORIScreen::drawText(ex_offsetx + 6, ex_offsety, namebuf, LGREY, &terminal_8x16_font);
        ex_offsety -= 18;
    }

    // hd
    {
        memset(namebuf, 0, buffer_size);
        namebuf[0] = ' ';
        namebuf[1] = ' ';
        namebuf[2] = ' ';
        namebuf[3] = ' ';
        namebuf[4] = ' ';
        namebuf[5] = 'H';
        namebuf[6] = 'D';
        namebuf[7] = ' ';
        if (active_star->hd_ident == 0)
            namebuf[8] = '-';
        else
            _itoa_s((int)active_star->hd_ident, namebuf + 8, 12, 10);

        ORIScreen::drawText(ex_offsetx + 6, ex_offsety, namebuf, LGREY, &terminal_8x16_font);
        ex_offsety -= 18;
    }

    // hip
    {
        memset(namebuf, 0, buffer_size);
        namebuf[0] = ' ';
        namebuf[1] = ' ';
        namebuf[2] = ' ';
        namebuf[3] = ' ';
        namebuf[4] = 'H';
        namebuf[5] = 'I';
        namebuf[6] = 'P';
        namebuf[7] = ' ';
        if (active_star->hip_ident == 0)
            namebuf[8] = '-';
        else
            _itoa_s((int)active_star->hip_ident, namebuf + 8, 12, 10);

        ORIScreen::drawText(ex_offsetx + 6, ex_offsety, namebuf, LGREY, &terminal_8x16_font);
        ex_offsety -= 18;
    }

    // ngc
    {
        memset(namebuf, 0, buffer_size);
        namebuf[0] = ' ';
        namebuf[1] = ' ';
        namebuf[2] = ' ';
        namebuf[3] = ' ';
        namebuf[4] = 'N';
        namebuf[5] = 'G';
        namebuf[6] = 'C';
        namebuf[7] = ' ';
        if (active_star->ngc_ident == 0)
            namebuf[8] = '-';
        else
            _itoa_s((int)active_star->ngc_ident, namebuf + 8, 12, 10);

        ORIScreen::drawText(ex_offsetx + 6, ex_offsety, namebuf, LGREY, &terminal_8x16_font);
        ex_offsety -= 18;
    }

    // mes
    {
        memset(namebuf, 0, buffer_size);
        namebuf[0] = 'M';
        namebuf[1] = 'E';
        namebuf[2] = 'S';
        namebuf[3] = 'S';
        namebuf[4] = 'I';
        namebuf[5] = 'E';
        namebuf[6] = 'R';
        namebuf[7] = ' ';
        if (active_star->mes_ident == 0)
            namebuf[8] = '-';
        else
            _itoa_s((int)active_star->mes_ident, namebuf + 8, 12, 10);

        ORIScreen::drawText(ex_offsetx + 6, ex_offsety, namebuf, LGREY, &terminal_8x16_font);
        ex_offsety -= 18;
    }

    delete[] namebuf;
}
