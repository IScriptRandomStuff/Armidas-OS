#pragma once
#include <stdbool.h>

void keyboard_install(void);
void keyboard_on_keypress(void (*callback)(char c));