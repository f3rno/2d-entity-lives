#ifndef FOOD_H
#define FOOD_H

#include <stdint.h>
#include "entity.h"

class CWorld;
class CVector2;
class CFood : public CEntity {

public:
  CFood(CWorld* world, CVector2 start_pos);
};

#endif
