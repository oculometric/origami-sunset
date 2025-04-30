#if defined(ARDUINO)
#include <Arduino.h>
#include <SPI.h>
#endif
#if defined(OPENGL)
#include <thread>
#include <iostream>
#include <GLFW/glfw3.h>
#endif

#include "screen.h"
#include "font8x16.h"

void setup()
{
#if defined(ARDUINO)
    pinMode(21, OUTPUT);
    digitalWrite(21, HIGH);

    Serial.begin(115200);
    Serial.println("Begin!");
#endif
    // initialise display control
    ORIScreen::initialise();
    ORIScreen::setBacklightBrightness(32);

#if defined(ARDUINO)
    Serial.println("Hello, world!");
#endif
#if defined(OPENGL)
    std::cout << "Hello, world!" << std::endl;
#endif
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
    if (flash_counter == 0)
#if defined(ARDUINO)
        digitalWrite(21, HIGH);
#endif
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
#if defined(ARDUINO)
        digitalWrite(21, LOW);
#endif
        flash_counter = hits_this_frame == 1 ? 15 : 200;
    }

    ORIScreen::fillPixels(old_x, old_y, box_sx + 200, box_sy, 0xFFFF);
    ORIScreen::fillPixels(box_x, box_y, box_sx, box_sy, 0b1111100000000000);
    ORIScreen::drawCharacter(box_x, box_y, 'A', 0x0000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 9, box_y, 'L', 0x0000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 18, box_y, 'B', 0x0000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 27, box_y, 'E', 0x0000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 36, box_y, 'D', 0x0000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 45, box_y, 'O', 0x0000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 54, box_y, ' ', 0x0000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 63, box_y, 'I', 0x0000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 72, box_y, 'S', 0x0000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 81, box_y, ' ', 0x0000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 90, box_y, 'G', 0b1111100000000000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 99, box_y, 'A', 0b0000011111100000, &terminal_8x16_font);
    ORIScreen::drawCharacter(box_x + 108, box_y, 'Y', 0b0000000000011111, &terminal_8x16_font);

    ORIScreen::blit();
#if defined(ARDUINO)
    delay(1);
#endif
#if defined(OPENGL)
    glfwPollEvents();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
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