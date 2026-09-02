#pragma once
#include <stdint.h>
#include <stddef.h>

void vga_clear(void);
void vga_putchar(char c);
void vga_print(const char *str);