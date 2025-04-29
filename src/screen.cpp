#include "../inc/screen.h"

#include <SPI.h>

static uint16_t* framebuffer = nullptr;
static uint16_t framebuffer_width;
static uint16_t framebuffer_height;
static uint16_t dirty_region_sx;
static uint16_t dirty_region_sy;
static uint16_t dirty_region_ex;
static uint16_t dirty_region_ey;

void ORIScreen::initialise()
{
    // initialise static variables
    framebuffer_width = 320;
    framebuffer_height = 170;
    if (framebuffer != nullptr)
        delete[] framebuffer;
    framebuffer = new uint16_t[framebuffer_width * framebuffer_height];
    clear(0xFFFF);

    // initialise the control pins
    pinMode(backlight_pin, OUTPUT);
    pinMode(chip_select_pin, OUTPUT);
    pinMode(reset_pin, OUTPUT);
    pinMode(data_command_switch_pin, OUTPUT);

    // set backlight to full brightness
    setBacklightBrightness(255);

    // initialise the SPI interface
    SPI.setDataMode(SPI_MODE3);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.begin(SCK, MISO, MOSI, SS);
    SPI.setFrequency(27000000);
    // reset the LCD display
    reset();

    // set the orientation to horizontal
    sendCommand(0x36);
    sendDataByte(0x00);

    // random commands i dont know what they do
    sendCommand(0x3A);
    sendDataByte(0x55);

    sendCommand(0xB2);
    sendDataByte(0x0C);
    sendDataByte(0x0C);
    sendDataByte(0x00);
    sendDataByte(0x33);
    sendDataByte(0x33);

    sendCommand(0xB7);
    sendDataByte(0x35);

    sendCommand(0xBB);
    sendDataByte(0x13);

    sendCommand(0xC0);
    sendDataByte(0x2C);

    sendCommand(0xC2);
    sendDataByte(0x01);

    sendCommand(0xC3);
    sendDataByte(0x0B);

    sendCommand(0xC4);
    sendDataByte(0x20);

    sendCommand(0xC6);
    sendDataByte(0x0F);

    sendCommand(0xD0);
    sendDataByte(0xA4);
    sendDataByte(0xA1);

    sendCommand(0xD6);
    sendDataByte(0xA1);

    sendCommand(0xE0);
    sendDataByte(0x00);
    sendDataByte(0x03);
    sendDataByte(0x07);
    sendDataByte(0x08);
    sendDataByte(0x07);
    sendDataByte(0x15);
    sendDataByte(0x2A);
    sendDataByte(0x44);
    sendDataByte(0x42);
    sendDataByte(0x0A);
    sendDataByte(0x17);
    sendDataByte(0x18);
    sendDataByte(0x25);
    sendDataByte(0x27);

    sendCommand(0xE1);
    sendDataByte(0x00);
    sendDataByte(0x03);
    sendDataByte(0x08);
    sendDataByte(0x07);
    sendDataByte(0x07);
    sendDataByte(0x23);
    sendDataByte(0x2A);
    sendDataByte(0x43);
    sendDataByte(0x42);
    sendDataByte(0x09);
    sendDataByte(0x18);
    sendDataByte(0x17);
    sendDataByte(0x25);
    sendDataByte(0x27);

    sendCommand(0x21);

    sendCommand(0x11);
    delay(120);
    sendCommand(0x29);

    blit();
}

void ORIScreen::reset()
{
    setActive(true);
    delay(20);
    digitalWrite(reset_pin, LOW);
    delay(20);
    digitalWrite(reset_pin, HIGH);
    delay(50);
}

void ORIScreen::sendCommand(uint8_t command)
{
    setActive(true);
    setCommandMode();
    SPI.transfer(command);
    setActive(false);
}

void ORIScreen::sendDataByte(uint8_t data)
{
    setActive(true);
    setDataMode();
    SPI.transfer(data);
    setActive(false);
}

void ORIScreen::sendDataWord(uint16_t data)
{
    setActive(true);
    setDataMode();
    SPI.transfer16(data);
    setActive(false);
}

void ORIScreen::setBacklightBrightness(uint8_t brightness)
{
    analogWrite(backlight_pin, brightness);
}

void ORIScreen::setPixel(uint16_t x, uint16_t y, uint16_t colour)
{
    if (x >= framebuffer_width || y >= framebuffer_height)
        return;
    
    // set the relevant value in the framebuffer
    framebuffer[x + (y * framebuffer_width)] = colour;

    // expand the dirty region to include it
    if (x < dirty_region_sx)
        dirty_region_sx = x;
    else if (x > dirty_region_ex)
        dirty_region_ex = x;
    if (dirty_region_ex == UINT16_MAX)
        dirty_region_ex = x;
    if (y < dirty_region_sy)
        dirty_region_sy = y;
    else if (y > dirty_region_ey)
        dirty_region_ey = y;
    if (dirty_region_ey == UINT16_MAX)
        dirty_region_ey = y;
}

void ORIScreen::fillPixels(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour)
{
    if (w == 0 || h == 0)
        return;

    if (x + w > framebuffer_width || y + h > framebuffer_height)
        return;

    // fill the area
    size_t row_start = x + (y * framebuffer_width);
    size_t current = row_start;
    for (uint16_t j = 0; j < h; j++)
    {
        for (uint16_t i = 0; i < w; i++)
        {
            framebuffer[current] = colour;
            current++;
        }
        row_start += framebuffer_width;
        current = row_start;
    }

    // expand the dirty region to include it
    setRegionDirty(x, y, w, h);
}


void ORIScreen::clear(uint16_t colour)
{
    for (size_t i = 0; i < framebuffer_width * framebuffer_height; i++)
        framebuffer[i] = colour;

    dirty_region_sx = 0;
    dirty_region_ex = framebuffer_width - 1;
    dirty_region_sy = 0;
    dirty_region_ey = framebuffer_height - 1;
}

void ORIScreen::blit()
{
    setActive(true);
    
    // set cursor start & end X command
    setCommandMode();
    SPI.transfer(0x2A);
    setDataMode();
    SPI.transfer16(dirty_region_sy + 35);
    SPI.transfer16(dirty_region_ey + 35);

    // set cursor start & end Y command
    setCommandMode();
    SPI.transfer(0x2B);
    setDataMode();
    SPI.transfer16(dirty_region_sx);
    SPI.transfer16(dirty_region_ex);

    // send finished command???
    setCommandMode();
    SPI.transfer(0x2C);

    // send pixel data
    setDataMode();

    uint16_t w = (dirty_region_ex - dirty_region_sx) + 1;
    uint16_t h = (dirty_region_ey - dirty_region_sy) + 1;
    // read from framebuffer at correct pixel locations, in the correct order, for the correct rect!
    size_t col_start = dirty_region_sx + (dirty_region_sy * framebuffer_width);
    size_t current = col_start;
    for (uint16_t i = 0; i < w; i++)
    {
        for (uint16_t j = 0; j < h; j++)
        {
            SPI.transfer16(framebuffer[current]);
            current += framebuffer_width;
        }
        col_start++;
        current = col_start;
    }
    
    setActive(false);

    // reset dirty region
    dirty_region_sx = UINT16_MAX;
    dirty_region_ex = UINT16_MAX;
    dirty_region_sy = UINT16_MAX;
    dirty_region_ey = UINT16_MAX;
}

void ORIScreen::setRegionDirty(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    // expand the dirty region
    if (x < dirty_region_sx)
        dirty_region_sx = x;
    if ((x + w - 1 > dirty_region_ex) || (dirty_region_ex == UINT16_MAX))
        dirty_region_ex = min(x + w - 1, framebuffer_width - 1);
    if (y < dirty_region_sy)
        dirty_region_sy = y;
    if ((y + h - 1 > dirty_region_ey) || (dirty_region_ey == UINT16_MAX))
        dirty_region_ey = min(y + h - 1, framebuffer_height - 1);
}

uint16_t ORIScreen::getWidth() { return framebuffer_width; }

uint16_t ORIScreen::getHeight() { return framebuffer_height; }