#include <Arduino.h>

#include "move.h"
#include "pin_config.h"

int delayTime = 50;

const int posGap = 47;  // mm

const uint8_t Xcnt = 6;
const uint8_t Ycnt = 4;

int posY_(uint8_t id) { return (int)id / (int)Xcnt; }
int posY(uint8_t id) { return posGap * posY_(id); }
int posX(uint8_t id) {
  return posGap *
         (((posY_(id) % 2) == 0) ? (id - (Xcnt)*posY_(id)) : ((1 + posY_(id)) * Xcnt - 1 - id));
}
void movePos(uint8_t id) {
  X_moveTo(posX(id));
  Y_moveTo(posY(id));
}
void movePosFill(uint8_t id) {
  X_moveTo(posX(id));
  Y_moveTo(posY(id) + 20);
}

void setup() {
  Serial.begin(115200);

  while (!Serial)
    ;
  Serial.println("Start...");

  move_init();

  Serial.println("end Setup");
}

String serialString = "";

int state = 0;
uint8_t currentPos = 0;

unsigned long fillTimer = 0;

unsigned long loopCounter = 0;
unsigned long loopTimer = 0;

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

    } else if (serialString.charAt(0) == 'A') {
      Serial.print("Moving Test 'A' ");
      state = 1;
    } else if (serialString.charAt(0) == 'B') {
      Serial.print("Moving Test 'B' ");
      state = 11;
    /* } else if (serialString.charAt(0) == 'S') {
      stepper_X.setMaxSpeed((int)serialString.substring(1).toInt() * steps_per_mm_X);
      stepper_Y.setMaxSpeed((int)serialString.substring(1).toInt() * steps_per_mm_Y);
      Serial.print("changed Max Speed");
    } else if (serialString.charAt(0) == 's') {
      stepper_X.setSpeed((int)serialString.substring(1).toInt() * steps_per_mm_X);
      stepper_Y.setSpeed((int)serialString.substring(1).toInt() * steps_per_mm_Y);
      Serial.print("changed Speed");
    } else if (serialString.charAt(0) == 'a') {
      stepper_X.setAcceleration((int)serialString.substring(1).toInt() * steps_per_mm_X);
      stepper_Y.setAcceleration((int)serialString.substring(1).toInt() * steps_per_mm_Y);
      Serial.print("Moving Test 'B' "); */
    } else if (serialString.charAt(0) == 'T') {
      Serial.println(analogRead(SENSOR_Pin));
    } else if (serialString.charAt(0) == 'H') {
      move_home();
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
      if (id < (Xcnt * Ycnt - 1)) movePos(id);
    } else if (serialString.charAt(0) == 'p') {
      Serial.println("else");
      uint8_t id = (int)serialString.substring(1).toInt();
      Serial.printf("id=%d\n", id);
      if (id < (Xcnt * Ycnt - 1)) movePosFill(id);
    } else {
      Serial.println("else");
      uint8_t id = (int)serialString.toInt();
      Serial.printf("id=%d\n", id);
      if (id < (Xcnt * Ycnt - 1)) movePos(id);
    }
  }

  if ((stepper_X.distanceToGo() == 0) && (stepper_Y.distanceToGo() == 0)) {
    // Serial.print("To go is 0A");
    switch (state) {
      case 10:  // Move to Glass Pos
        if (currentPos > 17) {
          state = 0;
          currentPos = 0;
          movePos(currentPos);

        } else {
          // currentPos++;

          Serial.print("pos: ");
          Serial.println(currentPos);

          movePos(currentPos);

          state++;
        }
        break;
      case 11:  // sense Glass
        Serial.print("senseGlass:  ");

        if (analogRead(SENSOR_Pin) < 3000) {
          Serial.println("Glass ");
          state++;
        } else {
          Serial.println("no Glass");
          Serial.println("");
          Serial.println("");
          state = 15;
        }

        break;
      case 12:  // move to Fill pos
        Serial.println("move Fill pos");
        movePosFill(currentPos);
        // Serial.println(stepper_Y.distanceToGo());
        // if (stepper_Y.distanceToGo()==0)
        state++;
        // else
        // Serial.println("distance to go not null");
        break;
        // Serial.println("test");
      case 13:  // start Glass Fill
        Serial.println("enable Pump");
        // Todo: Enable Pump

        fillTimer = millis();
        state++;
        break;
      case 14:  // End Glass fill

        if ((millis() - fillTimer) > 3000) {
          // Todo: disable Pump
          state++;
          Serial.println("");
          Serial.println("");
        }
        break;
      case 15:  // next Glass
        currentPos++;
        state = 10;
        break;

      default:
        break;
    }
    delay(100);
  }

  loopCounter++;
  if (millis() - loopTimer > 1000) {
    loopTimer = millis();
    Serial.print("Loops per Seccond: ");
    Serial.println(loopCounter);
    loopCounter = 0;
  }

  // Stepper runner:
  if (stepper_X.distanceToGo() != 0) {
    stepper_X.run();
  }

  if (stepper_Y.distanceToGo() != 0) {
    stepper_Y.run();
  }
}