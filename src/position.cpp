#include "position.h"

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
  Y_moveTo(posY(id) + fillOffset);
}

bool isRightSide(uint8_t id) {
  return (posY_(id) % 2) == 0 ? ((id - (Xcnt)*posY_(id)) > 2)
                              : (((1 + posY_(id)) * Xcnt - 1 - id) > 2);
}