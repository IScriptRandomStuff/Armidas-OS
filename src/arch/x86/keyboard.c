#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"
#include "io.h"
#include "irq.h"
#include "isr.h"

#define KB_DATA_PORT 0x60

static bool shift_held = false;

static const char kb_scancode_table[128] = {
    0,    0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']', '\n',   0,  'a',  's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';','\'', '`',   0, '\\', 'z',  'x',  'c',  'v',
    'b', 'n', 'm', ',', '.', '/',   0,  '*',   0, ' ',   0,    0,   0,    0,    0,    0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,    0,    0,    0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,    0,    0,    0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,    0,    0,    0,
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,    0,    0,    0,
};

static void (*keypress_callback)(char c) = 0;

static void keyboard_callback(struct registers *r)
{
    (void)r; // silence unused parameter warning
    uint8_t scancode = inb(KB_DATA_PORT);
    
    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7F;
        if (released == 0x2A || released == 0x36)
            shift_held = false;
        return;
    }

    if (scancode == 0x2A || scancode == 0x36) {
    shift_held = true;
    return;
    }

    char c = kb_scancode_table[scancode];
    if (!c) return;

    if (shift_held && c >= 'a' && c <= 'z')
        c -= 32;

    if (keypress_callback)
        keypress_callback(c);
}

void keyboard_on_keypress(void (*callback)(char c))
{
    keypress_callback = callback;
}

void keyboard_install(void)
{
    irq_install_handler(1, keyboard_callback);
}