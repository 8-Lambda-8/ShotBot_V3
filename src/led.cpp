#include "led.h"

CRGB leds[NUM_LEDS];

uint8_t animationStep = 0;
extern uint8_t state;
extern uint8_t menuState;
extern uint8_t animationHue = 0;
extern uint8_t animation = 10;
/**
 * 00 Off
 * 10 Rainbow
 * 20 Breathing
 */

const CRGB drinkColors[]{CRGB::DarkRed, CRGB::Cyan};

void led_init() {
  FastLED.addLeds<CHIPSET, WS2812_Pin, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(128);

  for (uint8_t i = 0; i < 4; i++) {
    leds[i] = btnStartupColors[i];
  }

  for (uint8_t i = 4; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Cyan;
  }

  FastLED.show();

  delay(200);

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void updateButtonLeds(uint8_t id, CRGB col) { leds[id] = col; }

void clearLedStrip() {
  for (uint8_t i = 4; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
}

void setBrightness(uint8_t b) { FastLED.setBrightness(b); }

void showMl(uint8_t ml, uint8_t selectedDrink, uint8_t max) {
  clearLedStrip();
  uint8_t mlLedCount = map(ml, 0, max, 0, 24);

  for (uint8_t i = NUM_LEDS - 1; i > NUM_LEDS - mlLedCount - 1; i--) {
    if (selectedDrink < 2)
      leds[i] = drinkColors[selectedDrink];
    else
      leds[i] = drinkColors[i % 2];
  }
}

void showDrinkColor(uint8_t selectedDrink) {
  if (selectedDrink < 2) {
    for (uint8_t i = 4; i < NUM_LEDS; i++) {
      leds[i] = drinkColors[selectedDrink];
    }
  } else if (selectedDrink == 2) {
    for (uint8_t i = 4; i < NUM_LEDS; i++) {
      leds[i] = drinkColors[i % 2];
    }
  } else {
    for (uint8_t i = 4; i < 11; i++) {
      leds[i] = CRGB::Black;
    }
    for (uint8_t i = 11; i < 20; i++) {
      leds[i] = drinkColors[1];
    }
    for (uint8_t i = 20; i < NUM_LEDS; i++) {
      leds[i] = drinkColors[0];
    }
  }
}

void showCount(uint8_t selectedCount, uint8_t selectedDrink) {
  clearLedStrip();
  for (uint8_t i = NUM_LEDS - 1; i > NUM_LEDS - selectedCount - 1; i--) {
    if (selectedDrink < 2)
      leds[i] = drinkColors[selectedDrink];
    else
      leds[i] = drinkColors[i % 2];
  }
}

void updateAnimation() {
  if (state != 0 && menuState < 10) return;
  if (animation == 10) {  // Rainbow
    for (uint8_t i = 4; i < NUM_LEDS; i++) {
      leds[i] = CHSV(animationStep + i * 11, 255, 255);
    }
  } else if (animation == 20) {  // Breathing
    for (uint8_t i = 4; i < NUM_LEDS; i++) {
      leds[i] = CHSV(animationHue, 255,
                     animationStep > 128 ? 255 - (animationStep - 128) * 2 : animationStep * 2);
    }
  }
}

uint32_t animationStepTimer = 0;

void led_loop() {
  if (millis() - animationStepTimer > 10) {
    animationStepTimer = millis();
    animationStep++;

    updateAnimation();
  }
}
