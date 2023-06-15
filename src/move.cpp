#include "move.h"

AccelStepper stepper_X(AccelStepper::DRIVER, STEP_PIN_X, DIR_PIN_X);
AccelStepper stepper_Y(AccelStepper::DRIVER, STEP_PIN_Y, DIR_PIN_Y);

void move_init() {
  pinMode(SW_PIN_X, INPUT);
  pinMode(SW_PIN_Y, INPUT);

  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH);

  stepper_off();

  stepper_X.setMaxSpeed(speed * steps_per_mm_X);
  stepper_X.setAcceleration(accel * steps_per_mm_X);
  stepper_X.setPinsInverted(false, false, false);

  stepper_Y.setMaxSpeed(speed * steps_per_mm_Y);
  stepper_Y.setAcceleration(accel * steps_per_mm_Y);
  stepper_Y.setPinsInverted(true, false, false);

  stepper_on();

  stepper_X.setSpeed(speed * steps_per_mm_X);
  stepper_Y.setSpeed(speed * steps_per_mm_Y);

  sense_home();
}

void X_moveTo(float mm) { stepper_X.moveTo((long)mm * steps_per_mm_X); }
float X_currentPosition() { return (float)stepper_X.currentPosition() / steps_per_mm_X; }

void Y_moveTo(float mm) { stepper_Y.moveTo((long)mm * steps_per_mm_Y); }
float Y_currentPosition() { return (float)stepper_Y.currentPosition() / steps_per_mm_Y; }

void move_home() {
  Serial.println("Move Home");

  stepper_X.moveTo(0);
  stepper_Y.moveTo(161 * steps_per_mm_Y);
  while ((stepper_X.distanceToGo() != 0) || (stepper_Y.distanceToGo() != 0)) {
    stepper_X.run();
    stepper_Y.run();
  }
  Serial.println("On Home");
}

void sense_home() {
  Serial.println("Sense Home");

  stepper_X.setSpeed(speed_sense * steps_per_mm_X);
  stepper_Y.setSpeed(speed_sense * steps_per_mm_Y);

  menu_print(0, 1, "Sense Home Y        ");
  stepper_Y.move(2000 * steps_per_mm_Y);
  while (digitalRead(SW_PIN_Y)) {
    stepper_Y.runSpeed();
  }
  stepper_Y.setCurrentPosition(homeOffset_Y * steps_per_mm_Y);

  menu_print(0, 1, "Sense Home X        ");
  stepper_X.move(-2000 * steps_per_mm_X);
  while (digitalRead(SW_PIN_X)) {
    stepper_X.runSpeed();
  }
  stepper_X.setCurrentPosition(homeOffset_X * steps_per_mm_X);

  stepper_X.setSpeed(speed * steps_per_mm_X);
  stepper_Y.setSpeed(speed * steps_per_mm_Y);

  move_home();
}

void stepper_on() { digitalWrite(EN_PIN, LOW); }
void stepper_off() { digitalWrite(EN_PIN, HIGH); }

bool move_finished() { return (stepper_X.distanceToGo() == 0) && (stepper_Y.distanceToGo() == 0); }

void move_loop() {
  if (stepper_X.distanceToGo() != 0) {
    stepper_X.run();
  }

  if (stepper_Y.distanceToGo() != 0) {
    stepper_Y.run();
  }
}
