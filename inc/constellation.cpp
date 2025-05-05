#include "constellation.h"

#include "screen.h"
#include "colours.h"
#include "compat.h"

const ORIConstellation constellations[1] = 
{
    ORIConstellation
    {
        "scorpius",
        {
            ORIStar{ "Antares A", { 16, 29, 24.47f }, { -26, 25, 55.0f }, 553.0f, -5.24f },
            ORIStar{ "sigma Sco", { 16, 00, 20.01f }, { -22, 37, 17.8f }, 401.0f, -3.16f },
            ORIStar{ "Graffias",  { 16, 05, 26.23f }, { -19, 48, 19.4f }, 530.0f, -3.44f },
            ORIStar{ "pi Sco",    { 15, 58, 51.12f }, { -26, 06, 50.6f }, 459.0f, -2.89f }, // TODO: add more stars in scorpius
        },
        {
            0, 1,
            1, 2,
            1, 3
        }
    }
};

static float getDegrees(ORIStar::RightAscension ra)
{
    return (float)(ra.hours * 15) + ((float)ra.minutes * 0.25f) + (ra.seconds * 0.004166666667f);
}

static float getDegrees(ORIStar::Declination dec)
{
    return (float)(dec.degrees) + ((((float)dec.minutes * 0.0166666666667) + ((float)dec.seconds * 0.0002777777778f)) * sign(dec.degrees));
}

void ORIConstellationViewer::drawConstellations(float ascension, float declination)
{
    for (const ORIConstellation& constel : constellations)
    {
        for (const ORIStar& star : constel.stars)
        {
            float ascension_angle = getDegrees(star.ra);
            float declination_angle = getDegrees(star.dec);
            ORIScreen::drawCircle(160, 85, 2, GOLD, GOLD);
        }
    }
}
