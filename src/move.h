#ifndef MOVE_H /* include guards */
#define MOVE_H

#include <AccelStepper.h>
#include <Arduino.h>

#include "pin_config.h"
#include "menu.h"

#if !defined(EN_PIN) || !defined(STEP_PIN_X) || !defined(DIR_PIN_X) || !defined(STEP_PIN_Y) || \
    !defined(DIR_PIN_Y)
#error "Stepper Pins not Defined!"
#endif

// (steps per rotation*micro stepping)/(toothCount*toothSeperation)
constexpr uint32_t steps_per_mm_X = (200 * 16) / (20 * 2);  // = 160
constexpr uint32_t steps_per_mm_Y = (200 * 16) / (30 * 2);  // = 64

constexpr uint32_t speed = 300;       // mm/s
constexpr uint32_t speed_sense = 100;  // mm/s
constexpr uint32_t speed_slow = 200;  // mm/s

constexpr uint32_t accel = 1000;  // mm/s^2

const int16_t homeOffset_X = -10;  // mm
const int16_t homeOffset_Y = 175;  // mm

void move_init();

void X_moveTo(float mm);
float X_currentPosition();

void Y_moveTo(float mm);
float Y_currentPosition();

void move_home();

void sense_home();

void stepper_on();
void stepper_off();

bool move_finished();

void move_loop();

#endif /* MOVEH_H */
