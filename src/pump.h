#ifndef PUMP_H /* include guards */
#define PUMP_H

#include <Arduino.h>

#include "pin_config.h"

#if !defined(PUMP_A_A) || !defined(PUMP_A_B) || !defined(PUMP_B_A) || !defined(PUMP_B_B)
#error "Stepper Pins not Defined!"
#endif

const uint16_t milisPerML = 467;  // ms/ml

void pump_init();

void pump(uint8_t pump_id, uint8_t ml);
void pump(uint8_t pump_id, uint8_t ml, bool reverse);

bool pump_finished();
void pump_stop();

uint8_t current_ml();

void pump_loop();

#endif /* PUMP_H */
