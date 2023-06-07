#include <Arduino.h>

#include "menu.h"
#include "move.h"
#include "pin_config.h"
#include "position.h"
#include "pump.h"

void setup() {
  menu_init();
  Serial.begin(115200);

  while (!Serial)
    ;
  Serial.println("Start...");

  move_init();
  pump_init();

  Serial.println("end Setup");
  menu_print(0, 0, "Finished Setup      ");
  menu_print(0, 1, "                    ");
  menu_print(0, 2, "                    ");
  menu_print(0, 3, "                    ");
  delay(1000);
}

String serialString = "";

unsigned long loopCounter = 0;
unsigned long loopShowTimer = 0;

extern uint8_t state = 0;
extern uint8_t stateL = 0;
extern uint8_t currentPos = 0;
extern uint8_t filledCount = 0;
extern uint8_t selectedML = 20;

void loop() {
  if (Serial.available()) {
    while (Serial.available()) {
      serialString = Serial.readString();  // read the incoming data as string
      Serial.println(serialString);
    }
    if (serialString.charAt(0) == 'Y') {
      Serial.print("Moving Y to ");
      Serial.println(serialString.substring(1));

      Y_moveTo(serialString.substring(1).toInt());

    } else if (serialString.charAt(0) == 'X') {
      Serial.print("Moving X to ");
      Serial.println(serialString.substring(1));

      X_moveTo(serialString.substring(1).toInt());

    } else if (serialString.charAt(0) == 'T') {
      Serial.println(analogRead(SENSOR_Pin));
    } else if (serialString.charAt(0) == 'H') {
      move_home();
    } else if (serialString.charAt(0) == 'S') {
      sense_home();
    } else if (serialString.charAt(0) == 'R') {
      Serial.println("Run Fill all\n");
      state = 10;
    } else if (serialString.charAt(0) == 'N') {
      state = 0;
      move_home();
    } else if (serialString.charAt(0) == 'P') {
      Serial.println("else");
      uint8_t id = (int)serialString.substring(1).toInt();
      Serial.printf("id=%d", id);
      if (id < PosCount) movePos(id);
    } else if (serialString.charAt(0) == 'p') {
      Serial.println("else");
      uint8_t id = (int)serialString.substring(1).toInt();
      Serial.printf("id=%d\n", id);
      if (id < PosCount) movePosFill(id);
    } else {
      Serial.println("else");
      uint8_t id = (int)serialString.toInt();
      Serial.printf("id=%d\n", id);
      if (id < PosCount) movePos(id);
    }
  }

  stateL = state;
  if (move_finished() && pump_finished()) {
    switch (state) {
      case 10:  // Move to Glass Pos
        Serial.println("10 Move To Pos");
        if (currentPos >= PosCount) {
          state = 0;
          currentPos = 0;
          filledCount = 0;
        } else {
          Serial.print("pos: ");
          Serial.println(currentPos);
          state++;
        }
        movePos(currentPos);
        break;
      case 11:  // sense Glass
        Serial.print("11 senseGlass:  ");

        if (analogRead(SENSOR_Pin) < 3000) {
          Serial.println("Glass ");
          state++;
          filledCount++;
        } else {
          Serial.println("no Glass");
          Serial.println("");
          Serial.println("");
          state = 15;
        }
        break;
      case 12:  // move to Fill pos
        Serial.println("12 move Fill pos");
        movePosFill(currentPos);
        state++;
        break;
      case 13:  // start Glass Fill
        Serial.println("13 enable Pump");
        pump(0, selectedML);
        state++;
        break;
      case 14:  // Pump Retraction
        Serial.println("14 enable Pump");
        pump(0, 1, true);
        state++;
        break;
      case 15:  // next Glass
        Serial.println("15 next Glass");
        Serial.println("");
        Serial.println("");
        currentPos++;
        state = 10;
        break;

      default:
        break;
    }
  }

  loopCounter++;
  if (millis() - loopShowTimer > 1000) {
    loopShowTimer = millis();
    Serial.print("Loops per Seccond: ");
    Serial.println(loopCounter);
    loopCounter = 0;
  }

  move_loop();
  pump_loop();
  menu_loop();
}