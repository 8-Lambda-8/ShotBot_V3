#ifndef POSITION_H /* include guards */
#define POSITION_H

#include "move.h"

const uint8_t Xcnt = 6;
const uint8_t Ycnt = 4;
constexpr uint8_t PosCount = Xcnt * Ycnt;

const uint8_t posGap = 47;      // mm
const uint8_t fillOffset = 20;  // mm

void movePos(uint8_t id);
void movePosFill(uint8_t id);

#endif /* POSITION_H */
