#include <Arduino.h>
#include <SPI.h>
#include "../inc/screen.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 170

uint16_t* back_buffer = nullptr;

void setup()
{
    pinMode(21, OUTPUT);
    Serial.begin(115200);
    Serial.println("Begin!");

    // initialise display control
    ORIScreen::initialise();
    ORIScreen::setBacklightBrightness(32);

    Serial.println("Hello, world!");
}

int16_t box_x = 0;
int16_t box_y = 0;
int16_t box_sx = 16;
int16_t box_sy = 16;

int16_t box_dx = 1;
int16_t box_dy = 1;

void loop()
{
    int16_t old_x = box_x;
    int16_t old_y = box_y;

    box_x += box_dx;
    box_y += box_dy;

    if (box_x >= SCREEN_WIDTH - box_sx - 1)
    {
        box_x = SCREEN_WIDTH - box_sx - 1;
        box_dx = -1;
    }
    if (box_x <= 0)
    {
        box_x = 0;
        box_dx = 1;
    }
    if (box_y >= SCREEN_HEIGHT - box_sy - 1)
    {
        box_y = SCREEN_HEIGHT - box_sy - 1;
        box_dy = -1;
    }
    if (box_y <= 0)
    {
        box_y = 0;
        box_dy = 1;
    }

    ORIScreen::fillPixels(old_x, old_y, box_sx, box_sy, 0xFFFF);
    ORIScreen::fillPixels(box_x, box_y, box_sx, box_sy, 0b1111100000000000);

    ORIScreen::blit();
    delay(1);
}