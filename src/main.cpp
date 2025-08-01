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

#if defined(OPENGL)
    auto s = std::chrono::high_resolution_clock::now();
#elif defined(ARDUINO)
    unsigned long s = micros();
#endif
    ORIConstellationViewer::drawConstellations(camera_right, camera_up, camera_fov);
    if (show_overlay) ORIConstellationViewer::drawOverlay();
#if defined(OPENGL)
    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> d = e - s;
    total_time += d.count();
#elif defined(ARDUINO)
    unsigned long e = micros();
    total_time += (float)(e-s) / 1000000.0f;
#endif
    total_its++;

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