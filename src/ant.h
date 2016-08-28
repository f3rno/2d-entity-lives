#ifndef ANT_H
#define ANT_H

#include <stdint.h>
#include "neural/neural_network.h"
#include "vec2.h"

struct SWorld;
typedef struct SAnt {
  SNeuralNetwork* brain;
  double* brain_outputs;
  double* brain_inputs;

  uint16_t id;
  uint16_t generation;
  uint16_t parent_1_id;
  uint16_t parent_2_id;
  uint16_t* pos;

  float rotation;

  int fitness;
} SAnt;

SAnt* ant_create(uint16_t x, uint16_t y, float angle, uint16_t gen, uint16_t id);
void ant_step(SAnt* ant, struct SWorld* world, float* vec2f, uint16_t* vec2i);
void ant_insert_genome(SAnt* ant, SNeuralNetworkData* input, uint16_t parent_1_id, uint16_t parent_2_id);
void ant_delete(SAnt* ant);
SAnt* ant_mate(struct SWorld* w, SAnt* a, SAnt* b, int gen, int id, float mutation_rate);

#endif
