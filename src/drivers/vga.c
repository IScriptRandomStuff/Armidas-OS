#include "vga.h"

// VGA text buffer always lives at this memory address on x86 PCs
static uint16_t *vga_buffer = (uint16_t *)0xB8000;

// Screen dimensions — standard VGA text mode
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_COLOR  0x0F   // white text on black background

// Track where the cursor currently is
static size_t vga_row = 0;
static size_t vga_col = 0;

// Packs a character + color into one 16-bit value the VGA buffer understands
static uint16_t vga_entry(char c, uint8_t color)
{
    return (uint16_t)c | ((uint16_t)color << 8);
}

void vga_clear(void)
{
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = vga_entry(' ', VGA_COLOR);
        }
    }
    vga_row = 0;
    vga_col = 0;
}

void vga_putchar(char c)
{
    if (c == '\n') {
        vga_col = 0;
        vga_row++;
    } else {
        vga_buffer[vga_row * VGA_WIDTH + vga_col] = vga_entry(c, VGA_COLOR);
        vga_col++;

        if (vga_col >= VGA_WIDTH) {
            vga_col = 0;
            vga_row++;
        }
    }

    if (vga_row >= VGA_HEIGHT) {
        vga_row = 0;
    }
}

void vga_print(const char *str)
{
    while (*str) {
        vga_putchar(*str);
        str++;
    }
}