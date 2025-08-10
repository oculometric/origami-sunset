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
#include "../inc/input.h"

ORIInputAction pan_right;
ORIInputAction pan_left;
ORIInputAction pan_up;
ORIInputAction pan_down;
ORIInputAction zoom_in;
ORIInputAction zoom_out;
ORIInputAction toggle_overlay;

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

    pan_right = ORIInput::registerAction("pan_right", GLFW_KEY_RIGHT);
    pan_left = ORIInput::registerAction("pan_left", GLFW_KEY_LEFT);
    pan_up = ORIInput::registerAction("pan_up", GLFW_KEY_UP);
    pan_down = ORIInput::registerAction("pan_down", GLFW_KEY_DOWN);

    zoom_in = ORIInput::registerAction("zoom_in", ']');
    zoom_out = ORIInput::registerAction("zoom_out", '[');

    toggle_overlay = ORIInput::registerAction("toggle_overlay", '\'');
}

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
    ORIInput::updateActionStates();

    if (ORIInput::wasActionPressed(toggle_overlay))
        show_overlay = !show_overlay;

    float horizontal_velocity = (float)std::min(ORIInput::getActionHoldCount(pan_left), (size_t)32)
        - (float)std::min(ORIInput::getActionHoldCount(pan_right), (size_t)32);
    horizontal_velocity /= 32.0f;
    horizontal_velocity = sqrt(abs(horizontal_velocity)) * csign(horizontal_velocity);
    camera_right += (camera_fov / 160.0f) * horizontal_velocity;
    if (camera_right < 0.0f)
        camera_right += 360.0;
    if (camera_right > 360.0f)
        camera_right -= 360.0f;

    float vertical_velocity = (float)std::min(ORIInput::getActionHoldCount(pan_up), (size_t)32)
        - (float)std::min(ORIInput::getActionHoldCount(pan_down), (size_t)32);
    vertical_velocity /= 32.0f;
    vertical_velocity = sqrt(abs(vertical_velocity)) * csign(vertical_velocity);
    camera_up += (camera_fov / 160.0f) * vertical_velocity;
    if (camera_up < -100.0f)
        camera_up = -100.0f;
    if (camera_up > 100.0f)
        camera_up = 100.0f;

    float fov_velocity = (float)std::min(ORIInput::getActionHoldCount(zoom_out), (size_t)16)
        - (float)std::min(ORIInput::getActionHoldCount(zoom_in), (size_t)16);
    fov_velocity /= 16.0f;
    camera_fov += fov_velocity;
    if (camera_fov < 5.0f)
        camera_fov = 5.0f;
    if (camera_fov > 120.0f)
        camera_fov = 120.0f;
#if defined(OPENGL)
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