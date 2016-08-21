#include "entity.h"
#include "vector2.h"

CEntity::CEntity(CWorld* _world, CVector2 start_pos) {
  world = _world;
  position = start_pos;
  rotation = 0;
}

void CEntity::step() {}

CVector2 CEntity::getPosition() {
  return position;
}

void CEntity::move(float x, float y) {
  position.x += x;
  position.y += y;
}

void CEntity::rotate(float a) {
  rotation += a;
}

