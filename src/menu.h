#ifndef MENU_H /* include guards */
#define MENU_H

#include <Arduino.h>
#include <ESPAsyncWiFiManager.h>
#include <LiquidCrystal_I2C.h>

#include "led.h"
#include "move.h"
#include "pin_config.h"
#include "position.h"
#include "pump.h"

void menu_init();
void menu_loop();

void menu_print(uint8_t x, uint8_t y, const char str[]);
void menu_print(uint8_t x, uint8_t y, const String& s);

void menu_printf(uint8_t x, uint8_t y, const char* format, ...);

void configModeCallback(AsyncWiFiManager* myWiFiManager);
void menu_OTA_Start();
void menu_OTA_Progress(size_t progress, size_t size);

#endif /* MENU_H */