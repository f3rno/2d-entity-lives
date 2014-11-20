#include "food.h"
#include "world.h"

CFood::CFood(CWorld* world, sf::Vector2f start_pos) :
  CEntity(world, new sf::CircleShape(2.0f), start_pos) {
  body->setFillColor(sf::Color(100, 255, 100, 255));
}