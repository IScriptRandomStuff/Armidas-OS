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