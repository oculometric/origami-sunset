#include <Arduino.h>
#include <SPI.h>
#include "../inc/screen.h"

void setup()
{
    pinMode(21, OUTPUT);
    digitalWrite(21, HIGH);

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

int flash_counter = 0;

void loop()
{
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

    ORIScreen::fillPixels(old_x, old_y, box_sx, box_sy, 0xFFFF);
    ORIScreen::fillPixels(box_x, box_y, box_sx, box_sy, 0b1111100000000000);

    ORIScreen::blit();
    delay(1);
}