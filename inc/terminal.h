#pragma once

#include <stdint.h>

class ORITerminal
{
public:
    struct Character
    {
        char c;
        uint16_t fg;
        uint16_t bg;
    };
private:

};