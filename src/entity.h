#ifndef ENTITY_H
#define ENTITY_H

#include "vector2.h"

class CWorld;
class CEntity {

public:
  CEntity(CWorld* _world, CVector2 start_pos);

  virtual void step();

  CVector2 getPosition();
  void move(float x, float y);
  void rotate(float a);

protected:
  CWorld* world;
  CVector2 position;
  float rotation;
};

#endif
