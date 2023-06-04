#include "menu.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

struct Button {
  uint8_t BtnPin;
  bool BtnState;
  bool BtnStateLast;
  uint8_t id;
};

Button buttons[4] = {{Button1, false, false, 0},
                     {Button2, false, false, 1},
                     {Button3, false, false, 2},
                     {Button4, false, false, 3}};

bool button_keyDown[4] = {false, false, false, false};

void menu_init() {
  lcd.init();
  lcd.backlight();

  for (auto &&btn : buttons) {
    pinMode(btn.BtnPin, INPUT);
  }

  menu_print(0, 0, "Startup...");
}

void menu_print(uint8_t x, uint8_t y, const char str[]) {
  lcd.setCursor(x, y);
  lcd.print(str);
};
void menu_print(uint8_t x, uint8_t y, const String& s) {
  lcd.setCursor(x, y);
  lcd.print(s);
};

void menu_printf(uint8_t x, uint8_t y, const char* format, ...) {
  lcd.setCursor(x, y);
  va_list arg;
  va_list copy;
  va_start(arg, format);
  va_copy(copy, arg);
  lcd.printf(format, copy);
};

void updateDisplay() {
}

bool buttonRead(Button btn) { return !digitalRead(btn.BtnPin); }

void updateButtons() {
  for (auto &&btn : buttons) {
    btn.BtnStateLast = btn.BtnState;
    btn.BtnState = buttonRead(btn);

    button_keyDown[btn.id] = btn.BtnState != btn.BtnStateLast && btn.BtnState;
  } 
}

unsigned long displayUpdateTimer = 0;

void menu_loop() {
  if (millis() - displayUpdateTimer > 1000) {
    displayUpdateTimer = millis();
    updateDisplay();
  }
  updateButtons();

}
