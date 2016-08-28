#ifndef VEC2_H
#define VEC2_H

#include <stdint.h>

uint16_t* vec2_zero();
uint16_t* vec2_create(uint16_t x, uint16_t y);
uint16_t* vec2_origin_to(uint16_t* origin, uint16_t* to);
void vec2_normal(uint16_t* v, float* n);
float vec2_length(uint16_t* v);
float vec2_distance(uint16_t* v, uint16_t* target);
void vec2_delete(uint16_t* v);

#endif
