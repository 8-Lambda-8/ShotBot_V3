#include "pump.h"

void pump_init() {
  pinMode(PUMP_A_A, OUTPUT);
  pinMode(PUMP_A_B, OUTPUT);

  pinMode(PUMP_B_A, OUTPUT);
  pinMode(PUMP_B_B, OUTPUT);

  digitalWrite(PUMP_A_A, HIGH);
  digitalWrite(PUMP_A_B, HIGH);
  digitalWrite(PUMP_B_A, HIGH);
  digitalWrite(PUMP_B_B, HIGH);
}

uint32_t pumpTime = 0;
uint64_t pumpTimer = 0;

bool pumpFinished = true;
bool dualPump = false;

void pump(uint8_t pump_id, uint8_t ml) { pump(pump_id, ml, false); };

void pump(uint8_t pump_id, uint8_t ml, bool reverse) {
  if (!pump_finished()) return;

  pumpTime = ml * milisPerML;
  pumpTimer = millis();
  pumpFinished = false;
  dualPump = false;

  if (pump_id == 0 && reverse) {
    digitalWrite(PUMP_A_A, HIGH);
  } else if (pump_id == 0) {
    digitalWrite(PUMP_A_B, HIGH);
  } else if (pump_id == 1 && reverse) {
    digitalWrite(PUMP_B_A, HIGH);
  } else if (pump_id == 1) {
    digitalWrite(PUMP_B_B, HIGH);
  } else if (pump_id == 2 && reverse) {
    dualPump = true;
    digitalWrite(PUMP_A_A, HIGH);
    digitalWrite(PUMP_B_A, HIGH);
    pumpTime = pumpTime / 2;
  } else if (pump_id == 2) {
    dualPump = true;
    digitalWrite(PUMP_A_B, HIGH);
    digitalWrite(PUMP_B_B, HIGH);
    pumpTime = pumpTime / 2;
  }
}

bool pump_finished() { return pumpFinished; }

uint8_t current_ml() { return (millis() - pumpTimer) / (milisPerML / dualPump ? 2 : 1); }

void pump_stop() {
  pumpFinished = true;
  // Turn off Pump
  digitalWrite(PUMP_A_A, HIGH);
  digitalWrite(PUMP_A_B, HIGH);
  digitalWrite(PUMP_B_A, HIGH);
  digitalWrite(PUMP_B_B, HIGH);
}

void pump_loop() {
  if (millis() - pumpTimer > pumpTime) {
    pump_stop();
  }
}
