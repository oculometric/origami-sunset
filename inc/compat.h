#pragma once

#include <stdint.h>

#if defined(OPENGL)
#include <thread>
#endif

#ifndef cmin
#define cmin(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef cmax
#define cmax(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef cabs
#define cabs(a) (((a) < 0) ? -(a) : (a))
#endif
#ifndef csign
#define csign(a) (((a) >= 0) ? 1 : -1)
#endif

#if !defined(ARDUINO)
inline void delay(uint32_t millis) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
inline void digitalWrite(uint8_t pin, uint8_t val) { }
inline void analogWrite(uint8_t pin, uint8_t val) { }
inline void pinMode(uint8_t pin, uint8_t mode) { }

#define OUTPUT 0x03
#define INPUT 0x01

#define HIGH 0x1
#define LOW 0x0
#endif

#if defined(ARDUINO)
#include <Arduino.h>
#endif

/* by Jim Ulery */
static unsigned long sqrt(unsigned long val)
{
    unsigned long temp, g = 0, b = 0x8000, bshft = 15;
    do
    {
        if (val >= (temp = (((g << 1) + b) << bshft--)))
        {
            g += b;
            val -= temp;
        }
    } while (b >>= 1);
    return g;
}