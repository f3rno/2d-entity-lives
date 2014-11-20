#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>

class CWorld;
class CVector2;
class CEntity {

public:
  CEntity(CWorld* _world, sf::Shape* _body, sf::Vector2f start_pos);
  ~CEntity();

  virtual void step();
  virtual void draw(sf::RenderWindow* window);

  sf::Shape* getBody();
  CVector2 getPosition();

protected:
  sf::Shape* body;
  CWorld* world;
};

#endif