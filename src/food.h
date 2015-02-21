#ifndef FOOD_H
#define FOOD_H

#include <stdint.h>
#include "entity.h"
#include <SFML/Graphics.hpp>

class CWorld;
class CFood : public CEntity {

public:
  CFood(CWorld* world, sf::Vector2f start_pos);
};

#endif
