#pragma once

#include <stdint.h>

class ORISerial
{
public:
    static void initialise();

    static void printLn(const char* text);
    static void print(const char* text);
    static void print(const uint8_t i, const uint8_t b = 10);
    static void print(const uint16_t i, const uint8_t b = 10);
    static void print(const uint32_t i, const uint8_t b = 10);
    static void print(const float f);
    static void print(const char c);
};