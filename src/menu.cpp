#include "menu.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

uint8_t downArrowChar[] = {
    0b00000, 0b00000, 0b10001, 0b01010, 0b10101, 0b01010, 0b00100, 0b00000,
};
uint8_t upArrowChar[] = {
    0b00000, 0b00100, 0b01010, 0b10101, 0b01010, 0b10001, 0b00000, 0b00000,
};

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
extern uint8_t filledCount;
extern uint8_t selectedML;
extern uint8_t selectedCount;
extern uint8_t selectedDrink;
const char *drinkNames[] = {"Ramazotti     ", "Luft          ", "Halb / Halb   ", "Links | Rechts"};
uint8_t menuState = 0;
uint8_t menuCursor = 0;
const char *moveOptions[] = {"X Axis", "Y Axis", "Pump 1", "Pump 2"};

void menu_init() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, downArrowChar);
  lcd.createChar(2, upArrowChar);

  for (auto &&btn : buttons) {
    pinMode(btn.BtnPin, INPUT);
  }

  menu_print(0, 0, "Startup...");
}

void menu_print(uint8_t x, uint8_t y, const char str[]) {
  lcd.setCursor(x, y);
  lcd.print(str);
};
void menu_print(uint8_t x, uint8_t y, const String &s) {
  lcd.setCursor(x, y);
  lcd.print(s);
};

template <class... A>
void menu_printf(uint8_t x, uint8_t y, const char *format, A... args) {
  lcd.setCursor(x, y);
  lcd.printf(format, args...);
};

void updateDisplay() {
  if (!move_finished()) return;
  if (state != 0) {
    menuState = 0;
    switch (state) {
      case 10:
        menu_printf(0, 1, " Pos %02d  Count %02d   ", currentPos, filledCount);
        menu_print(0, 2, "                    ");
        menu_print(0, 3, "[Abort][ ] [ ][    ]");
        break;
      case 14: {
        menu_printf(0, 2, " %02d / %02d ml         ", current_ml(), selectedML);
        menu_print(0, 3, "[Abort][ ] [ ][Next]");
        break;
      }

      default:
        break;
    }
    return;
  }
  switch (menuState) {
    case 0:  // Home Menu
      menu_print(0, 0, "#  Shot Bot v3.0   #");
      menu_printf(0, 1, "  %s   ", drinkNames[selectedDrink]);
      menu_printf(0, 2, "  %02d ml      %02d     ", selectedML, selectedCount);
      menu_printf(0, 3, "[Fill][ml][sel] [%c]", 0x01);
      break;
    case 1:  // Home Menu 2
      menu_print(0, 0, "#  Shot Bot v3.0   #");
      menu_print(0, 1, "                    ");
      menu_print(0, 2, "                    ");
      menu_printf(0, 3, "[Fill][cnt][mov] [%c]", 0x02);
      break;
    case 10:  // ml Menu
      menu_print(0, 1, " Change fill amount ");
      menu_printf(0, 2, "        %02d ml       ", selectedML);
      menu_print(0, 3, "[Fill] [-] [+] [esc]");
      break;
    case 20:  // Drink Menu
      menu_print(0, 1, "   Drink Selection  ");
      menu_printf(0, 2, "  %s   ", drinkNames[selectedDrink]);
      menu_print(0, 3, "[Fill] [<] [>] [esc]");
      break;
    case 30:  // count Menu
      menu_print(0, 1, "    Change count    ");
      menu_printf(0, 2, "         %02d         ", selectedCount);
      menu_print(0, 3, "[Fill] [-] [+] [esc]");
      break;
    case 40:  // Move Menu
      menu_print(0, 1, "    Move    ");
      if (menuCursor < 2) {
        menu_printf(0, 2, " %s     %03d mm ", moveOptions[menuCursor],
                    (uint8_t)(menuCursor == 0 ? X_currentPosition() : Y_currentPosition()));
        menu_print(0, 3, "[sel] [-5][+5] [esc]");
      } else {
        menu_printf(0, 2, " %s            ", moveOptions[menuCursor]);
        menu_print(0, 3, "[sel] [-]  [+] [esc]");
      }
      break;

    default:
      break;
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
    // when Filling
    if (button_keyDown[0]) {  // Abort
      state = 0;
      currentPos = 0;
      pump_stop();
      move_home();
      Serial.println("Abort");
    } else if (state == 14 && button_keyDown[3]) {
      pump_stop();  // next glass
    }
    return;
  }

  // in Menu:
  if (button_keyDown[0] && menuState < 40) {  // Start Fill
    menu_print(0, 1, "                    ");
    menu_print(0, 2, "                    ");
    state = 10;
    return;
  }

  if (menuState > 9 && button_keyDown[3]) {
    menuState = 0;
    return;
  }
  switch (menuState) {
    case 0:  // Home Menu
      if (button_keyDown[1]) {
        menuState = 10;
      } else if (button_keyDown[2]) {
        menuState = 20;
      } else if (button_keyDown[3]) {
        menuState = 1;
      }
      break;
    case 1:  // Home Menu
      if (button_keyDown[1]) {
        menuState = 30;
      } else if (button_keyDown[2]) {
        menuState = 40;
      } else if (button_keyDown[3]) {
        menuState = 0;
      }
      break;
    case 10:  // ml Menu
      if (button_keyDown[1] && selectedML > 10) {
        selectedML -= 2;
      } else if (button_keyDown[2] && selectedML < 40) {
        selectedML += 2;
      }
      break;
    case 20:  // Drink Menu
      if (button_keyDown[1]) {
        selectedDrink--;
        if (selectedDrink > 200) selectedDrink = 3;
      } else if (button_keyDown[2]) {
        selectedDrink++;
        if (selectedDrink > 3) selectedDrink = 0;
      }
      break;
    case 30:  // count Menu
      if (button_keyDown[1] && selectedCount > 2) {
        selectedCount--;
      } else if (button_keyDown[2] && selectedCount < PosCount) {
        selectedCount++;
      }
      break;
    case 40:  // move Menu
      if (button_keyDown[0]) {
        menuCursor++;
        if (menuCursor > 3) menuCursor = 0;
      } else if (button_keyDown[1]) {
        if (menuCursor == 0 && X_currentPosition() > 4) {
          X_moveTo(X_currentPosition() - 5);
        } else if (menuCursor == 1 && Y_currentPosition() > 4) {
          Y_moveTo(Y_currentPosition() - 5);
        } else {
          if (pump_finished())
            pump(menuCursor - 2, 5, true);
          else
            pump_stop();
        }
      } else if (button_keyDown[2]) {
        if (menuCursor == 0 && X_currentPosition() < 246) {
          X_moveTo(X_currentPosition() + 5);
        } else if (menuCursor == 1 && Y_currentPosition() < 166) {
          Y_moveTo(Y_currentPosition() + 5);
        } else {
          if (pump_finished())
            pump(menuCursor - 2, 5, false);
          else
            pump_stop();
        }
      }
      break;
    default:
      menuState = 0;
      break;
  }
}

unsigned long displayUpdateTimer = 0;

void menu_loop() {
  if (millis() - displayUpdateTimer > 150 || state != stateL) {
    displayUpdateTimer = millis();
    updateDisplay();
  }
  updateButtons();
}
