#include "move.h"

AccelStepper stepper_X(AccelStepper::DRIVER, STEP_PIN_X, DIR_PIN_X);
AccelStepper stepper_Y(AccelStepper::DRIVER, STEP_PIN_Y, DIR_PIN_Y);


void X_moveTo(float mm) { stepper_X.moveTo((long)mm * steps_per_mm_X); }
float X_currentPosition() { return (float)stepper_X.currentPosition() / steps_per_mm_X; }

void Y_moveTo(float mm) { stepper_Y.moveTo((long)mm * steps_per_mm_Y); }
float Y_currentPosition() { return (float)stepper_Y.currentPosition() / steps_per_mm_Y; }

void move_home() {
  Serial.println("Move Home");
  Serial.println(X_currentPosition());
  Serial.println(Y_currentPosition());

  stepper_X.moveTo(0);
  stepper_Y.moveTo(0);
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

  stepper_Y.move(2000 * steps_per_mm_Y);
  while (digitalRead(SW_PIN_Y)) {
    stepper_Y.runSpeed();
  }
  stepper_Y.setCurrentPosition(homeOffset_Y * steps_per_mm_Y);

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
