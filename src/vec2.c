#include <stdlib.h>
#include <math.h>
#include "vec2.h"

uint16_t* vec2_zero() {
  return vec2_create(0, 0);
}

uint16_t* vec2_create(uint16_t x, uint16_t y) {
  uint16_t* v = (uint16_t*)malloc(sizeof(uint16_t) * 2);

  v[0] = x;
  v[1] = y;

  return v;
}

uint16_t* vec2_origin_to(uint16_t* origin, uint16_t* to) {
  uint16_t* v = vec2_zero();

  v[0] = to[0] - origin[0];
  v[1] = to[1] - origin[1];

  return v;
}

void vec2_normal(uint16_t* v, float* n) {
  float l = vec2_length(v);

  n[0] = (float)v[0] / l;
  n[1] = (float)v[1] / l;
}

float vec2_length(uint16_t* v) {
  uint16_t z[2] = {0, 0};

  return vec2_distance(v, z);
}

float vec2_distance(uint16_t* v, uint16_t* target) {
  return sqrtf(powf(target[0] - v[0], 2) + powf(target[1] - v[1], 2));
}

void vec2_delete(uint16_t* v) {
  free(v);
}
