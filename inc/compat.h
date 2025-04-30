#pragma once

#include <stdint.h>

#if defined(OPENGL)
#include <thread>
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