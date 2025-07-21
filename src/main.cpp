#if defined(OPENGL)
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#endif

#include "screen.h"
#include "compat.h"
#include "log.h"
#include "colours.h"
#include "font8x16.h"
#include "constellation.h"

void setup()
{
    pinMode(21, OUTPUT);
    digitalWrite(21, HIGH);

    ORISerial::initialise();
    ORISerial::printLn("Begin!");

    // initialise display control
    ORIScreen::initialise();
    ORIScreen::setBacklightBrightness(32);

    ORISerial::printLn("Hello, cassette!");

    ORIScreen::clear(0b0000000000100010);

    ORIConstellationViewer::initialiseConstellations();

        ORISerial::printLn("bonk");

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

void loop()
{
#if defined(OPENGL)
    auto start = std::chrono::high_resolution_clock::now();
#endif
    //if (flash_counter == 0)
    //    digitalWrite(21, HIGH);
    //if (flash_counter >= 0)
    //    flash_counter--;

    //int16_t old_x = box_x;
    //int16_t old_y = box_y;

    //box_x += box_dx;
    //box_y += box_dy;

    //int hits_this_frame = 0;

    //if (box_x >= ORIScreen::getWidth() - box_sx - 1)
    //{
    //    box_x = ORIScreen::getWidth() - box_sx - 1;
    //    box_dx = -1;

    //    hits_this_frame++;
    //}
    //if (box_x <= 0)
    //{
    //    box_x = 0;
    //    box_dx = 1;

    //    hits_this_frame++;
    //}
    //if (box_y >= ORIScreen::getHeight() - box_sy - 1)
    //{
    //    box_y = ORIScreen::getHeight() - box_sy - 1;
    //    box_dy = -1;

    //    hits_this_frame++;
    //}
    //if (box_y <= 0)
    //{
    //    box_y = 0;
    //    box_dy = 1;

    //    hits_this_frame++;
    //}

    //if (hits_this_frame > 0)
    //{
    //    digitalWrite(21, LOW);
    //    flash_counter = hits_this_frame == 1 ? 15 : 200;
    //}

    // FIXME: it simply is not possible to push the entire contents of the screen across in one frame. 2fps is the maximum rate at which we can draw whole frames
    ORIScreen::clear(0b0000000000100010);

    //ORIScreen::drawText(box_x, box_y - 8, "ALBEDO", ORIColour::WHITE, &terminal_8x16_font);
    //ORIScreen::drawText(-4, box_y, "test", ORIColour::WHITE, &terminal_8x16_font);

    //uint32_t line_height = terminal_8x16_font.getGlyphHeight() + 1;
    //ORIScreen::drawText(0, ORIScreen::getHeight() - line_height, "The quick brown fox jumps over the lazy dog. He might even do a flip...", ORIColour::GOLD, &terminal_8x16_font);

    //ORIScreen::drawLine(160, 85, box_x + (box_sx / 2), box_y + (box_sy / 2), ORIColour::GOLD);
    //ORIScreen::drawCircle(box_x, 4, 4, ORIColour::GOLD, ORIColour::RED);
    //ORIScreen::drawCircle(320-4-1, box_y, 4, ORIColour::GOLD, ORIColour::RED);
#if defined(OPENGL)
    auto s = std::chrono::high_resolution_clock::now();
#endif
    ORIConstellationViewer::drawConstellations(camera_right, camera_up, camera_fov);
#if defined(OPENGL)
    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> d = e - s;
    total_time += d.count();
#endif
    total_its++;
    /*ORISerial::print("camera RA: ");
    ORISerial::print(camera_right);
    ORISerial::print(", camera DC: ");
    ORISerial::print(camera_up);
    ORISerial::print(", camera fov: ");
    ORISerial::print(camera_fov);
    ORISerial::printLn("");*/

    int16_t cx = ORIScreen::getWidth() / 2;
    int16_t cy = ORIScreen::getHeight() / 2;
    ORIScreen::drawLine(cx - 4, cy, cx + 5, cy, RED);
    ORIScreen::drawLine(cx, cy - 4, cx, cy + 5, RED);

    //ORIScreen::blit();
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