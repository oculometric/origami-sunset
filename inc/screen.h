#pragma once

#if defined(ARDUINO)
#include <Arduino.h>
#endif

#include <stdint.h>

class ORIFont;

class ORIScreen
{
private:
#if defined(ARDUINO)
    static const uint8_t backlight_pin = A2;
    static const uint8_t chip_select_pin = SS;
    static const uint8_t reset_pin = D4;
    static const uint8_t data_command_switch_pin = D3;
#endif

    inline ORIScreen() { }

private:
#if defined(ARDUINO)
    static void reset();
    static inline void setActive(bool active) { digitalWrite(chip_select_pin, active ? LOW : HIGH); }
    static inline void setCommandMode() { digitalWrite(data_command_switch_pin, LOW); }
    static inline void setDataMode() { digitalWrite(data_command_switch_pin, HIGH); }
    static void sendCommand(uint8_t command);
    static void sendDataByte(uint8_t data);
    static void sendDataWord(uint16_t data);
#endif

public:
    static void initialise();
    static void setBacklightBrightness(uint8_t brightness);
    static void setPixel(int16_t x, int16_t y, uint16_t colour);
    static void fillPixels(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t colour);
    static void drawText(int16_t x, int16_t y, const char* t, uint16_t colour, const ORIFont* font);
    static void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour);
    static void drawCircle(int16_t cx, int16_t cy, uint16_t r, uint16_t fill, uint16_t outline);
    static void clear(uint16_t colour);

    static void blit();
    static void setRegionDirty(int16_t x, int16_t y, uint16_t w, uint16_t h);

    static uint16_t getWidth();
    static uint16_t getHeight();
};