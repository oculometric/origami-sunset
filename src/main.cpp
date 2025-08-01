#if defined(OPENGL)
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <vector>
#endif

#include "../inc/screen.h"
#include "../inc/compat.h"
#include "../inc/log.h"
#include "../inc/colours.h"
#include "../inc/font8x16.h"
#include "../inc/constellation.h"

#include "../catalogue-tool/constel.h"
#include "../catalogue-tool/ngc2000.h"

std::vector<CTNGCEntry> ngc_data;
std::map<std::string, std::vector<std::pair<float, float>>> boundary_data;
std::map<std::string, std::pair<float, float>> center_data;

void setup()
{
    //pinMode(21, OUTPUT);
    //digitalWrite(21, HIGH);
    delay(1000);

    ORISerial::initialise();
    ORISerial::printLn("Begin!");

    // initialise display control
    ORIScreen::initialise();
    ORIScreen::setBacklightBrightness(32);

    ORISerial::printLn("Hello, cassette!");

    ORIScreen::clear(0b0000000000100010);
    ORISerial::printLn("bink");

    ORIConstellationViewer::initialiseConstellations();

    ORISerial::printLn("bonk");

    ngc_data = readTDat_NGC("catalog/heasarc_ngc2000.tdat");
    boundary_data = readBounds("catalog/bound_in_20.txt");
    center_data = readCenters("catalog/centers_20.txt");
    center_data.erase("SER");
}

int16_t box_x = 0;
int16_t box_y = 0;
int16_t box_sx = 1;
int16_t box_sy = 1;

int16_t box_dx = 1;
int16_t box_dy = 1;

int flash_counter = 0;

float camera_right = 247.0f;
float camera_up = -24.8f;
float camera_fov = 90.0f;

float total_time = 0.0f;
int total_its = 0;

bool show_overlay = true;

void loop()
{
#if defined(OPENGL)
    auto start = std::chrono::high_resolution_clock::now();
#endif

    ORIScreen::clear(0b0000000000100010);


    for (int i = 0; i < boundary_data["AND"].size(); i++)
    {
        auto p1 = boundary_data["AND"][i];
        auto p2 = boundary_data["AND"][(i + 1) % boundary_data["AND"].size()];

        ORIScreen::drawLine((p1.first / 24.0f) * ORIScreen::getWidth(),
            ((p1.second / 180.0f) + 0.5f) * ORIScreen::getHeight(),
            (p2.first / 24.0f) * ORIScreen::getWidth(),
            ((p2.second / 180.0f) + 0.5f) * ORIScreen::getHeight(), RED);
    }
    ORIScreen::setPixel((center_data["AND"].first / 24.0f) * ORIScreen::getWidth(),
                        ((center_data["AND"].second / 180.0f) + 0.5f) * ORIScreen::getHeight(), RED);

    ORIScreen::setPixel((ngc_data[0].ra / 360.0f) * ORIScreen::getWidth(),
        ((ngc_data[0].dec / 180.0f) + 0.5f) * ORIScreen::getHeight(), BLUE);

    {
        std::pair<float, float> center = center_data["AND"];
        std::vector<std::pair<float, float>> boundary = boundary_data["AND"];
        std::pair<float, float> alt_star = { ngc_data[0].ra * (24.0f / 360.0f), ngc_data[0].dec };

        // based on this https://stackoverflow.com/a/3838357/7332101
        std::pair<float, float> i2 = { std::min(alt_star.first, center.first), std::max(alt_star.first, center.first) };
        float a2 = (alt_star.second - center.second) / (alt_star.first - center.first);
        float b2 = alt_star.second - (a2 * alt_star.first);

        size_t num_intersections = 0;
        for (int i = 0; i < boundary.size(); i++)
        {
            auto p1 = boundary[i];
            auto p2 = boundary[(i + 1) % boundary.size()];
            if (abs(p1.first - p2.first) > 12.0f)
            {
                if (p2.first > p1.first)
                    p2.first -= 24.0f;
                else
                    p1.first -= 24.0f;
            }

            std::pair<float, float> i1 = { std::min(p1.first, p2.first), std::max(p1.first, p2.first) };
            std::pair<float, float> ia = { std::max(i1.first, i2.first), std::min(i1.second, i2.second) };

            if (i1.second < i2.first)
                continue;

            float a1 = (p1.second - p2.second) / (p1.first - p2.first);
            float b1 = p1.second - (a1 * p1.first);

            if (a1 == a2)
                continue;

            float xa = (b2 - b1) / (a1 - a2);

            if ((xa < ia.first) || xa > ia.second)
                continue;

            ORIScreen::setPixel((xa / 24.0f) * ORIScreen::getWidth(),
                ((((a1 * xa) + b1) / 180.0f) + 0.5f) * ORIScreen::getHeight(), GREEN);

            num_intersections++;
        }
    }
//#if defined(OPENGL)
//    auto s = std::chrono::high_resolution_clock::now();
//#elif defined(ARDUINO)
//    unsigned long s = micros();
//#endif
//    ORIConstellationViewer::drawConstellations(camera_right, camera_up, camera_fov);
//    if (show_overlay) ORIConstellationViewer::drawOverlay();
//#if defined(OPENGL)
//    auto e = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<float> d = e - s;
//    total_time += d.count();
//#elif defined(ARDUINO)
//    unsigned long e = micros();
//    total_time += (float)(e-s) / 1000000.0f;
//#endif
//    total_its++;

    /*ORISerial::print("camera RA: ");
    ORISerial::print(camera_right);
    ORISerial::print(", camera DC: ");
    ORISerial::print(camera_up);
    ORISerial::print(", camera fov: ");
    ORISerial::print(camera_fov);
    ORISerial::printLn("");
    ORISerial::print("mean constellation draw time: ");
    ORISerial::print((total_time / total_its) * 1000.0f);
    ORISerial::printLn("ms");*/

    ORIScreen::blit();
    delay(16);
#if defined(OPENGL)
    glfwPollEvents();
    if (glfwGetKey(ORIScreen::getWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera_right -= camera_fov / 160.0f;
    if (glfwGetKey(ORIScreen::getWindow(), GLFW_KEY_LEFT) == GLFW_PRESS)
        camera_right += camera_fov / 160.0f;
    if (glfwGetKey(ORIScreen::getWindow(), GLFW_KEY_UP) == GLFW_PRESS)
        camera_up += camera_fov / 160.0f;
    if (glfwGetKey(ORIScreen::getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
        camera_up -= camera_fov / 160.0f;
    if (glfwGetKey(ORIScreen::getWindow(), '.') == GLFW_PRESS)
        camera_fov += 1.0f;
    if (glfwGetKey(ORIScreen::getWindow(), ',') == GLFW_PRESS)
        camera_fov -= 1.0f;
    /*if (glfwGetKey(ORIScreen::getWindow(), 'L') == GLFW_PRESS)
        show_overlay = !show_overlay;*/

    if (camera_fov < 5.0f)
        camera_fov = 5.0f;
    if (camera_fov > 120.0f)
        camera_fov = 120.0f;
    if (camera_right < 0.0f)
        camera_right += 360.0;
    if (camera_right > 360.0f)
        camera_right -= 360.0f;
    if (camera_up < -100.0f)
        camera_up = -100.0f;
    if (camera_up > 100.0f)
        camera_up = 100.0f;
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> diff = stop - start;
    //ORISerial::print((uint32_t)((1.0f / diff.count())), 10);
    //ORISerial::printLn("");
#endif
}

#if defined(OPENGL)
int main()
{
    setup();

    while (!glfwWindowShouldClose(ORIScreen::getWindow()))
        loop();

    std::cout << "average time for constellation drawing: " << (total_time / (float)total_its) * 1000.0f << "ms" << std::endl;

    return 1;
}
#endif