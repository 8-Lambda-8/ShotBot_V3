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

extern uint8_t state;
extern uint8_t stateL;
extern uint8_t currentPos;
uint8_t menuState = 0;

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
  if (state != 0 && move_finished()) {
    menuState = 0;
    switch (state) {
      case 10:
        menu_printf(0, 2, "Pos %d", currentPos);
        menu_print(0, 3, "[Abort][ ] [ ][    ]");
        break;
      case 14: {
        menu_printf(0, 2, "%d ml", current_ml());
        menu_print(0, 3, "[Abort][ ] [ ][Next]");
      }

      default:
        break;
    }

  } else if (state == 0) {
    switch (menuState) {
      case 0:  // Home Menu
        menu_print(0, 0, "#  Shot Bot v3.0   #");
        menu_print(0, 1, "                    ");
        menu_print(0, 2, "                    ");
        menu_printf(0, 3, "[Fill][  ] [  ] [  ]");
        break;

      default:
        break;
    }
  }
}

bool buttonRead(Button btn) { return !digitalRead(btn.BtnPin); }

void updateButtons() {
  for (auto &&btn : buttons) {
    btn.BtnStateLast = btn.BtnState;
    btn.BtnState = buttonRead(btn);

    button_keyDown[btn.id] = btn.BtnState != btn.BtnStateLast && btn.BtnState;
  }

  if (state != 0) {
    if (button_keyDown[0]) {  // Abort
      state = 0;
      pump_stop();
      move_home();
    } else if (state == 14 && button_keyDown[3]) {
      pump_stop();  // next glass
    }
  } else {
    if (button_keyDown[0]) {  // Start Fill
      state = 10;
    }
  }
}

unsigned long displayUpdateTimer = 0;

void menu_loop() {
  if (millis() - displayUpdateTimer > 1000 || state != stateL) {
    displayUpdateTimer = millis();
    updateDisplay();
  }
  updateButtons();

}
