#include "entity.h"
#include "vector2.h"

CEntity::CEntity(CWorld* _world, sf::Shape* _body, sf::Vector2f start_pos) {
  body = _body;
  world = _world;

  if (body != NULL) {
    body->setPosition(start_pos);
  }
}

void CEntity::step() {}

void CEntity::draw(sf::RenderWindow* window) {
  if (body != NULL) {
    window->draw(*body);
  }
}

sf::Shape* CEntity::getBody() {
  return body;
}

CVector2 CEntity::getPosition() {
  return CVector2(body->getPosition().x, body->getPosition().y);
}

CEntity::~CEntity() {
  delete body;
}