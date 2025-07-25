#include "../inc/screen.h"

#if defined(ARDUINO)
#include <SPI.h>
#endif

// prototype screen res 320x170
// new prototype screen res 820x320
#define SCREEN_WIDTH 820
#define SCREEN_HEIGHT 320
#if defined(OPENGL)
#include <GLFW/glfw3.h>
// janky fix for windows
#ifndef GL_UNSIGNED_SHORT_5_6_5
#define GL_UNSIGNED_SHORT_5_6_5 0x8363

#endif
#define PREVIEW_SCALE 2
#endif

#include "../inc/compat.h"
#include "../inc/font.h"

static uint16_t* framebuffer = nullptr;
static uint16_t framebuffer_width;
static uint16_t framebuffer_height;
static uint16_t dirty_region_sx;
static uint16_t dirty_region_sy;
static uint16_t dirty_region_ex;
static uint16_t dirty_region_ey;

#if defined(OPENGL)
static GLFWwindow* window;
#endif

void ORIScreen::initialise()
{
    // initialise static variables
    framebuffer_width = SCREEN_WIDTH;
    framebuffer_height = SCREEN_HEIGHT;
    if (framebuffer != nullptr)
        delete[] framebuffer;
    framebuffer = new uint16_t[framebuffer_width * framebuffer_height];
    clear(0xFFFF);

#if defined(ARDUINO)
/*
    // initialise the control pins
    pinMode(backlight_pin, OUTPUT);
    pinMode(chip_select_pin, OUTPUT);
    pinMode(reset_pin, OUTPUT);
    pinMode(data_command_switch_pin, OUTPUT);

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
    */
#endif

#if defined(OPENGL)
	glfwInit();

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	window = glfwCreateWindow(framebuffer_width * PREVIEW_SCALE, framebuffer_height * PREVIEW_SCALE, "origami sunset preview", nullptr, nullptr);
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
#endif

	// set backlight to full brightness
    setBacklightBrightness(255);
    blit();
}

#if defined(ARDUINO)
void ORIScreen::reset()
{
    /*
    setActive(true);
    delay(20);
    digitalWrite(reset_pin, LOW);
    delay(20);
    digitalWrite(reset_pin, HIGH);
    delay(50);
    */
}

void ORIScreen::sendCommand(uint8_t command)
{
    /*
    setActive(true);
    setCommandMode();
    SPI.transfer(command);
    setActive(false);
    */
}

void ORIScreen::sendDataByte(uint8_t data)
{
    /*
    setActive(true);
    setDataMode();
    SPI.transfer(data);
    setActive(false);
    */
}

void ORIScreen::sendDataWord(uint16_t data)
{
    /*
    setActive(true);
    setDataMode();
    SPI.transfer16(data);
    setActive(false);
    */
}
#endif

void ORIScreen::setBacklightBrightness(uint8_t brightness)
{
#if defined(ARDUINO)
/*
    analogWrite(backlight_pin, brightness);
    */
#endif
}

void ORIScreen::setPixel(int16_t x, int16_t y, uint16_t colour)
{
    if (x >= framebuffer_width || y >= framebuffer_height)
        return;
    if (x < 0 || y < 0)
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

void ORIScreen::fillPixels(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t colour)
{
    if (w == 0 || h == 0)
        return;

    if (x < 0)
    {
        w -= -x;
        x = 0;
    }
    if (y < 0)
    {
        h -= -y;
        y = 0;
    }

    if (x + w > framebuffer_width)
        w = framebuffer_width - x;
    if (y + h > framebuffer_height)
        h = framebuffer_height - y;

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

void ORIScreen::drawText(int16_t x, int16_t y, const char* t, uint16_t colour, const ORIFont* font)
{
    int16_t glyph_width = font->getGlyphWidth();
    int16_t glyph_height = font->getGlyphHeight();
    int32_t glyph_pitch = font->getGlyphDataSize() / glyph_height;

    int16_t top_clip = cmax(0, (int16_t)(y + glyph_height) - (int16_t)framebuffer_height);

    int16_t pixel_x = x;
    int16_t pixel_y_top = (y + glyph_height - 1) - top_clip;
    int16_t pixel_y = pixel_y_top;
    uint32_t cnum = 0;
    for (size_t ci = 0; t[ci] != 0x0; ci++)
    {
        char c = t[ci];
        cnum++;
        uint8_t* glyph = font->getGlyphData(c);

        int16_t pixel_x_left = pixel_x;
        int32_t glyph_off = top_clip * glyph_pitch;
        for (int16_t j = top_clip; j < glyph_height; j++, pixel_x = pixel_x_left, pixel_y--)
        {
            if (pixel_y < 0)
                break;
            
            int32_t counter = pixel_x + (pixel_y * (int16_t)framebuffer_width);
            for (int32_t i = 0; i < glyph_pitch; i++)
            {
                uint8_t value = glyph[glyph_off];
                uint8_t comparator = 0b10000000;
                for (uint8_t _ = 0; _ < 8; _++)
                {
                    if (pixel_x >= framebuffer_width)
                        break;
                    if ((value & comparator) && (pixel_x >= 0))
                        framebuffer[counter] = colour;
                    pixel_x++;
                    counter++;
                    comparator = comparator >> 1;
                }

                glyph_off++;
            }
        }
        pixel_y = pixel_y_top;
        pixel_x += glyph_width + 1;
    }
    setRegionDirty(x, y, (glyph_width + 1) * cnum, glyph_height);
}

void ORIScreen::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour)
{
    bool x_flipped = x0 > x1;
    int16_t xmin = x_flipped ? x1 : x0; int16_t ymin = x_flipped ? y1 : y0;
    int16_t xmax = x_flipped ? x0 : x1; int16_t ymax = x_flipped ? y0 : y1;

    int16_t rise = ymax - ymin;
    int16_t run = xmax - xmin;
    
    float gradient = (float)rise / (float)run;
    float x = xmin;
    float y = ymin;

    // TODO: line width

    if (cabs(gradient) < 1.0f)
    {
        for (int16_t i = 0; i < run; i++, x+= 1.0f, y += gradient)
        {
            if (x < 0) continue;
            else if ((uint16_t)x >= ORIScreen::getWidth())
                break;
            if (y < 0) continue;
            else if ((uint16_t)y >= ORIScreen::getHeight())
            {
                if (gradient > 0)
                    break;
                else
                    continue;
            }

            framebuffer[(uint16_t)x + ((uint16_t)y * framebuffer_width)] = colour;
        }
    }
    else
    {
        float inv_grad = cabs(1.0f / gradient);
        float y_inc = rise > 0 ? 1.0f : -1.0f;
        for (int16_t i = 0; i < cabs(rise); i++, x += inv_grad, y += y_inc)
        {
            if (x < 0) continue;
            else if ((uint16_t)x >= ORIScreen::getWidth())
                break;
            if (y < 0) continue;
            else if ((uint16_t)y >= ORIScreen::getHeight())
            {
                if (y_inc > 0)
                    break;
                else
                    continue;
            }

            framebuffer[(uint16_t)x + ((uint16_t)y * framebuffer_width)] = colour;
        }
    }
    setRegionDirty(xmin, cmin(y0, y1), run, cabs(rise));
}

void ORIScreen::drawCircle(int16_t cx, int16_t cy, uint16_t r, uint16_t fill, uint16_t outline)
{
    if (r == 0)
    {
        setPixel(cx, cy, outline);
        return;
    }

    int16_t x_min = cx - (int16_t)r;
    int16_t y_min = cy - (int16_t)r;
    int16_t x_max = x_min + r + r;
    int16_t y_max = y_min + r + r;

    int16_t left_clip = cmax(-x_min, 0);
    int16_t bottom_clip = cmax(-y_min, 0);
    int16_t right_clip = cmax(x_max - (int16_t)ORIScreen::getWidth() + 1, 0);
    int16_t top_clip = cmax(y_max - (int16_t)ORIScreen::getHeight() + 1, 0);

    int16_t actual_width = (r * 2) - (left_clip + right_clip);
    int16_t actual_height = (r * 2) - (bottom_clip + top_clip);

    int32_t rr = r * r;

    uint32_t row_start = ((y_min + bottom_clip) * ORIScreen::getWidth()) + (x_min + left_clip);
    uint32_t counter = row_start;
    for (int16_t y = y_min + bottom_clip; y <= y_max - top_clip; y++)
    {
        int16_t dy = y - cy;
        for (int16_t x = x_min + left_clip; x <= x_max - right_clip; x++)
        {
            int16_t dx = x - cx;
            int32_t d = (dx * dx) + (dy * dy);
            if (cabs(rr - d) < r)
                framebuffer[counter] = outline;
            else if (d < rr)
                framebuffer[counter] = fill;
            counter++;
        }
        row_start += ORIScreen::getWidth();
        counter = row_start;
    }

    setRegionDirty(cx + left_clip, cy + bottom_clip, actual_width, actual_height);
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
#if defined(ARDUINO)
/*
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
    */
#endif

#if defined(OPENGL)
	glfwMakeContextCurrent(window);
	glViewport(0, 0, framebuffer_width, framebuffer_height);

    // under OpenGL things work a little differently and we 
    // have to just draw the whole buffer (ignoring the dirty region).
    // i think this is just as performant as doing individual rows actually...
    glRasterPos2f(-1.0f, -1.0f);
    glPixelZoom(PREVIEW_SCALE, PREVIEW_SCALE);
    glDrawPixels(framebuffer_width, framebuffer_height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, framebuffer);
	
    glFlush();
	//glfwSwapBuffers(window);
    //glfwSwapInterval(1);
#endif

    // reset dirty region
    dirty_region_sx = UINT16_MAX;
    dirty_region_ex = UINT16_MAX;
    dirty_region_sy = UINT16_MAX;
    dirty_region_ey = UINT16_MAX;
}

void ORIScreen::setRegionDirty(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
    // expand the dirty region
    if (x < dirty_region_sx)
        dirty_region_sx = cmax(x, 0);
    if ((x + w - 1 > dirty_region_ex) || (dirty_region_ex == UINT16_MAX))
        dirty_region_ex = cmin(x + w - 1, framebuffer_width - 1);
    if (y < dirty_region_sy)
        dirty_region_sy = cmax(y, 0);
    if ((y + h - 1 > dirty_region_ey) || (dirty_region_ey == UINT16_MAX))
        dirty_region_ey = cmin(y + h - 1, framebuffer_height - 1);
}

uint16_t ORIScreen::getWidth() { return framebuffer_width; }

uint16_t ORIScreen::getHeight() { return framebuffer_height; }

#if defined(OPENGL)
GLFWwindow* ORIScreen::getWindow()
{
    return window;
}
#endif