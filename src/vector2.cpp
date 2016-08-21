#include "vector2.h"
#include <cmath>
#include <iostream>

CVector2::CVector2() {
  CVector2(0, 0);
}

CVector2::CVector2(float _x, float _y) {
  x = _x;
  y = _y;
}

// Vector between two points
CVector2::CVector2(CVector2 a, CVector2 b) {
  x = a.x - b.x;
  y = a.y - b.y;
}

CVector2 CVector2::normal() {
  float l = length();
  float nx = x / l;
  float ny = y / l;

  return CVector2(nx, ny);
}

void CVector2::normalise() {
  float l = length();

  x /= l;
  y /= l;
}

float CVector2::length() {
  return distance(0, 0);
}

float CVector2::distance(float target_x, float target_y) {
  return sqrtf(powf(target_x - x, 2) + powf(target_y - y, 2));
}

float CVector2::distance(CVector2 vec) {
  return distance(vec.x, vec.y);
}
