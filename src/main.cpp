#if defined(OPENGL)
#include <GLFW/glfw3.h>
#include <chrono>
#endif

#include "screen.h"
#include "compat.h"
#include "log.h"
#include "colours.h"
#include "font8x16.h"

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
}

int16_t box_x = 0;
int16_t box_y = 0;
int16_t box_sx = 16;
int16_t box_sy = 16;

int16_t box_dx = 1;
int16_t box_dy = 1;

int flash_counter = 0;

void loop()
{
#if defined(OPENGL)
    auto start = std::chrono::high_resolution_clock::now();
#endif

    if (flash_counter == 0)
        digitalWrite(21, HIGH);
    if (flash_counter >= 0)
        flash_counter--;

    int16_t old_x = box_x;
    int16_t old_y = box_y;

    box_x += box_dx;
    box_y += box_dy;

    int hits_this_frame = 0;

    if (box_x >= ORIScreen::getWidth() - box_sx - 1)
    {
        box_x = ORIScreen::getWidth() - box_sx - 1;
        box_dx = -1;

        hits_this_frame++;
    }
    if (box_x <= 0)
    {
        box_x = 0;
        box_dx = 1;

        hits_this_frame++;
    }
    if (box_y >= ORIScreen::getHeight() - box_sy - 1)
    {
        box_y = ORIScreen::getHeight() - box_sy - 1;
        box_dy = -1;

        hits_this_frame++;
    }
    if (box_y <= 0)
    {
        box_y = 0;
        box_dy = 1;

        hits_this_frame++;
    }

    if (hits_this_frame > 0)
    {
        digitalWrite(21, LOW);
        flash_counter = hits_this_frame == 1 ? 15 : 200;
    }

    ORIScreen::clear(0b0000000000100010);

    //ORIScreen::fillPixels(box_x, box_y, box_sx, box_sy, ORIColour::BLUE);
    ORIScreen::drawText(box_x, box_y + 8, "ALBEDO", ORIColour::WHITE, &terminal_8x16_font);
    ORIScreen::drawText(-4, box_y, "test", ORIColour::WHITE, &terminal_8x16_font);

    uint32_t line_height = terminal_8x16_font.getGlyphHeight() + 1;
    ORIScreen::drawText(0, ORIScreen::getHeight() - line_height, "The quick brown fox jumps over the lazy dog. He might even do a flip...", ORIColour::GOLD, &terminal_8x16_font);
    /*ORIScreen::drawText(0, ORIScreen::getHeight() - (line_height * 2), "Line 2", 0x0000, &terminal_8x16_font);
    ORIScreen::drawText(0, ORIScreen::getHeight() - (line_height * 3), "Line 3", 0x0000, &terminal_8x16_font);
    ORIScreen::drawText(0, ORIScreen::getHeight() - (line_height * 4), "Line 4", 0x0000, &terminal_8x16_font);
    ORIScreen::drawText(0, ORIScreen::getHeight() - (line_height * 5), "Line 5", 0x0000, &terminal_8x16_font);
    ORIScreen::drawText(0, ORIScreen::getHeight() - (line_height * 6), "Line 6", 0x0000, &terminal_8x16_font);
    ORIScreen::drawText(0, ORIScreen::getHeight() - (line_height * 7), "Line 7", 0x0000, &terminal_8x16_font);
    ORIScreen::drawText(0, ORIScreen::getHeight() - (line_height * 8), "Line 8", 0x0000, &terminal_8x16_font);
    ORIScreen::drawText(0, ORIScreen::getHeight() - (line_height * 9), "Line 9", 0x0000, &terminal_8x16_font);
    ORIScreen::drawText(0, ORIScreen::getHeight() - (line_height * 10), "Line 10 (end)", 0x0000, &terminal_8x16_font);*/

    ORIScreen::drawLine(160, 85, box_x + (box_sx / 2), box_y + (box_sy / 2), ORIColour::GOLD);
    ORIScreen::drawCircle(box_x, 4, 4, ORIColour::GOLD, ORIColour::RED);
    ORIScreen::drawCircle(320-4-1, box_y, 4, ORIColour::GOLD, ORIColour::RED);

    ORIScreen::blit();
    delay(1);
#if defined(OPENGL)
    glfwPollEvents();
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> diff = stop - start;
    ORISerial::print((uint32_t)((1.0f / diff.count())), 10);
    ORISerial::printLn("");
#endif
}

#if defined(OPENGL)
int main()
{
    setup();

    while (true)
        loop();

    return 1;
}
#endif