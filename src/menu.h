#ifndef MENU_H /* include guards */
#define MENU_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "global_vars.h"
#include "pin_config.h"

void menu_init();
void menu_loop();

void menu_print(uint8_t x, uint8_t y, const char str[]);
void menu_print(uint8_t x, uint8_t y, const String& s);

void menu_printf(uint8_t x, uint8_t y, const char* format, ...);


#endif /* MENU_H */