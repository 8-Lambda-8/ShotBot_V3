#include "menu.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

void menu_init() {
  lcd.init();
  lcd.backlight();

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

void menu_loop() {}
