#pragma once

#include <stddef.h>
#include <stdint.h>

enum fb_color
{
    FB_COLOR_BLACK  = 0xFF000000,
    FB_COLOR_WHITE  = 0xFFFFFFFF,
    FB_COLOR_RED    = 0xFFFF0000,
    FB_COLOR_BLUE   = 0XFF0000FF,
    FB_COLOR_YELLOW = 0xFFFFFF00,
    FB_COLOR_GRAY   = 0xFF808080,
    FB_COLOR_GREEN  = 0xFF00FF00,
    FB_COLOR_CYAN   = 0xFF00FFFF,
};

struct fb_info
{
    uint64_t base;          //base address of the framebuffer
    uint16_t height;        //height, in pixels
    uint16_t width;         //width, in pixels
    uint16_t pitch;         //bytes per line
    uint16_t bpp;           //bits per pixel
    uint32_t max_pixels;    //total number of pixels
};

void fb_init();

void fb_putpixel(uint32_t x, uint32_t y, enum fb_color col);
void fb_drawline(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, enum fb_color col);

void fb_clearscreen(enum fb_color col);
