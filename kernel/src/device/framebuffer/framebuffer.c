#include <framebuffer.h>
#include <boot.h>



struct fb_info fb_info;

void fb_init()
{
    fb_info.base = boot_info.tag_framebuffer->framebuffer_addr;
    fb_info.height = boot_info.tag_framebuffer->framebuffer_height;
    fb_info.width = boot_info.tag_framebuffer->framebuffer_width;
    fb_info.pitch = boot_info.tag_framebuffer->framebuffer_pitch;
    fb_info.bpp = boot_info.tag_framebuffer->framebuffer_bpp;
    fb_info.max_pixels = fb_info.height * fb_info.width;

    //printf("FB Base: %p", fb_info.base);
    //printf("FB Height: %d  FB Width: %d\n", fb_info.height, fb_info.width);
    //printf("FB BPP: %d   FB Pitch: %d\n", fb_info.bpp, fb_info.pitch);

    //fb_putpixel(100,100,FB_COLOR_WHITE);
}

void fb_putpixel(uint32_t x, uint32_t y, enum fb_color col)
{
    uint64_t index = y * (fb_info.pitch / (fb_info.bpp / 8)) + x;
    //if(index > fb_info.max_pixels) return;
    *((uint32_t*)fb_info.base + index) = col;
}

void fb_clearscreen(enum fb_color col)
{
    for (uint64_t i = 0; i < (fb_info.height*fb_info.width); i++)
    {
        *((uint32_t*)fb_info.base + i) = col;
    }
}

void fb_drawline(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, enum fb_color col)
{

}