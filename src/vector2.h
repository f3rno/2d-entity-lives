#ifndef VECTOR_H
#define VECTOR_H

#include <SFML/System.hpp>

// We have to define our own vector class since SFML doesn't provide much...
class CVector2 {

public:
  CVector2();
  CVector2(float _x, float _y);
  CVector2(CVector2 a, CVector2 b);  // Vector between two points

  float x;
  float y;

  CVector2 normal();
  void normalise();
  float length();
  float distance(float sx, float sy);
  float distance(CVector2 vec);
  float distance(sf::Vector2f vec);

  sf::Vector2f toSF();
};

#endif
