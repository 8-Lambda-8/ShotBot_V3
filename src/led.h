#ifndef LED_H /* include guards */
#define LED_H

#include <FastLED.h>

#include "pin_config.h"

#define COLOR_ORDER GRB
#define CHIPSET WS2812

#define BRIGHTNESS 20

#define NUM_LEDS 28

const CRGB btnStartupColors[] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow};

void led_init();
void led_loop();

void updateButtonLeds(uint8_t id, CRGB col);

void clearLedStrip();
void setBrightness(uint8_t b);
void showMl(uint8_t ml, uint8_t selectedDrink, uint8_t max);
void showDrinkColor(uint8_t selectedDrink);
void showCount(uint8_t selectedCount, uint8_t selectedDrink);

#endif /* LED_H */