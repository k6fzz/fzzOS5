#include "console.h"
#include "../framebuffer/framebuffer.h"

// Console Font
#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04
#define PSF1_FONT_WIDTH 8
#define PSF1_FONT_HEIGHT 16

struct PSF1_HEADER{
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
};

struct PSF1_FONT{
	struct PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
};

extern uint8_t _binary_zapl16_psf_start;
extern uint8_t _binary_zapl16_psf_end;

uint8_t console_buffer[4096];
struct console_buffer con_output_buffer;

uint8_t cmd_buffer[256];
struct console_buffer con_input_buffer;


struct console_info
{
    uint32_t start_pos;
    uint32_t max_lines;
    enum fb_color foreground_color;
    enum fb_color background_color;
    uint16_t cursor_x;
    uint16_t cursor_y;
    struct PSF1_FONT* font;
}console_info;

static inline void console_draw_hline(uint64_t y, enum fb_color color)
{
    uint64_t index = y * fb_info.width;

    for(int i = 0; i < fb_info.width; i++)
    {
        *((uint32_t*)fb_info.base + index + i) = color;
    }
}

static inline void console_scroll()
{
    console_info.cursor_x = 0;

    //TODO: Implement Scrolling
}

static inline void console_next()
{
    console_info.cursor_x = 0;
    console_info.cursor_y += PSF1_FONT_HEIGHT;

    if (console_info.cursor_y > fb_info.height - PSF1_FONT_HEIGHT)
    {
        console_scroll();
    }
}

static void console_drawchar(uint8_t c, uint32_t x, uint32_t y, enum fb_color foreground, enum fb_color background)
{
    uint8_t row;
    uint8_t* glyph = (uint8_t*)&_binary_zapl16_psf_start + 4 + (c * PSF1_FONT_HEIGHT);

    for (uint32_t i = 0; i < 16; i++)
    {
        row = glyph[i];
        for (uint32_t j = 0; j < 8; j++)
        {
            if(row & 0x80)
            {
                fb_putpixel(x+j, y+i, foreground);
            }
            else
            {
                fb_putpixel(x+j, y+i, background);
            }
            row = row << 1;
        }
    }
}

void putchar(uint8_t c)
{
    switch(c)
    {
        case ASCII_LF:
            console_next();
            return;
        case ASCII_CR:
            console_next();
            return;
        case ASCII_HT:
            if(console_info.cursor_x += (PSF1_FONT_HEIGHT * 4) < fb_info.width)
            {
                console_info.cursor_x += (8*4);
                return;
            }
            else
            {
                console_next();
                console_info.cursor_x += (8*4);
                return;
            }
    }

    console_drawchar(c, console_info.cursor_x, console_info.cursor_y, console_info.foreground_color, console_info.background_color);
    console_info.cursor_x += 8;
}



void console_init()
{
    console_info.font = (struct PSF1_FONT*)&_binary_zapl16_psf_start;
    console_info.max_lines = fb_info.height / PSF1_FONT_HEIGHT;
    console_info.foreground_color = FB_COLOR_WHITE;
    console_info.background_color = FB_COLOR_BLACK;
    console_info.cursor_x = 0;
    console_info.cursor_y = 0;

    console_info.start_pos = (fb_info.height / 4) * 3;

    con_output_buffer.buffer = console_buffer;
    con_output_buffer.buffer_size = sizeof(console_buffer);
    con_output_buffer.index = 0;

    con_input_buffer.buffer = cmd_buffer;
    con_input_buffer.buffer_size = sizeof(cmd_buffer);
    con_output_buffer.index = 0;

    console_draw_hline(console_info.start_pos, FB_COLOR_YELLOW);
    console_draw_hline(console_info.start_pos + 1, FB_COLOR_YELLOW);

    kbd_set_target(console_input);

    //fb_clearscreen(console_info.background_color);
}

void console_exec()
{
    //read command buffer and do something

    //clear the terminal screen
    for(int i = console_info.start_pos + (fb_info.width * 2); i < (fb_info.height * fb_info.width); i++)
    {
        *((uint32_t*)fb_info.base + i) = console_info.background_color;
    }

    //write the output buffer to screen
}

void console_input(KEY_INFO_t key)
{
    con_input_buffer.index++;
    con_input_buffer.buffer[con_input_buffer.index] = key.ascii;
    

    con_output_buffer.buffer[con_output_buffer.index] = key.ascii;
    con_output_buffer.index++;
    con_output_buffer.buffer[con_output_buffer.index] = 0x00;
    return;
}