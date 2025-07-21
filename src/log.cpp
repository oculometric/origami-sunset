#include "log.h"

#if defined(ARDUINO)
#include <Arduino.h>
#endif
#if defined(OPENGL)
#include <iostream>
#include <bitset>
#endif

void ORISerial::initialise()
{
#if defined(ARDUINO)
    Serial.begin(115200 /*921600*/);
#endif
}

void ORISerial::printLn(const char* text)
{
#if defined(ARDUINO)
    Serial.println(text);
#endif
#if defined(OPENGL)
    std::cout << text << std::endl;
#endif
}

void ORISerial::print(const char* text)
{
#if defined(ARDUINO)
    Serial.println(text);
#endif
#if defined(OPENGL)
    std::cout << text;
#endif
}

void ORISerial::print(const uint8_t i, const uint8_t b)
{
#if defined(ARDUINO)
    Serial.print(i, b);
#endif
#if defined(OPENGL)
    switch (b)
    {
        case 2: std::cout << std::bitset<8>{i} << std::dec; return;
        case 8: std::cout << std::oct; break;
        case 10: std::cout << std::dec; break;
        case 16: std::cout << std::hex; break;
    }
    std::cout << i;
    std::cout << std::dec;
#endif
}

void ORISerial::print(const uint16_t i, const uint8_t b)
{
#if defined(ARDUINO)
    Serial.print(i, b);
#endif
#if defined(OPENGL)
    switch (b)
    {
        case 2: std::cout << std::bitset<8>{i} << std::dec; return;
        case 8: std::cout << std::oct; break;
        case 10: std::cout << std::dec; break;
        case 16: std::cout << std::hex; break;
    }
    std::cout << i;
    std::cout << std::dec;
#endif
}

void ORISerial::print(const uint32_t i, const uint8_t b)
{
#if defined(ARDUINO)
    Serial.print(i, b);
#endif
#if defined(OPENGL)
    switch (b)
    {
        case 2: std::cout << std::bitset<8>{i} << std::dec; return;
        case 8: std::cout << std::oct; break;
        case 10: std::cout << std::dec; break;
        case 16: std::cout << std::hex; break;
    }
    std::cout << i;
    std::cout << std::dec;
#endif
}

void ORISerial::print(const float f)
{
#if defined(ARDUINO)
    Serial.print(f);
#endif
#if defined(OPENGL)
    std::cout << f;
#endif
}

void ORISerial::print(const char c)
{
#if defined(ARDUINO)
    Serial.print(c);
#endif
#if defined(OPENGL)
    std::cout << c;
#endif
}
