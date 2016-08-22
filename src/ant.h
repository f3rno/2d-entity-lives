#ifndef ANT_H
#define ANT_H

#include <stdint.h>
#include <vector>
#include <sstream>
#include "neural/neural_network.h"

class CWorld;

struct SAnt {
  SNeuralNetwork* brain;
  double* brain_outputs;
  double* brain_inputs;

  uint16_t id;
  uint16_t generation;
  uint16_t parent_1_id;
  uint16_t parent_2_id;
  uint16_t x;
  uint16_t y;

  float rotation;

  int fitness;
};

SAnt* ant_create(uint16_t x, uint16_t y, float angle, uint16_t gen, uint16_t id);
void ant_step(SAnt* ant, CWorld* world);
void ant_insert_genome(SAnt* ant, SNeuralNetworkData* input, uint16_t parent_1_id, uint16_t parent_2_id);
void ant_delete(SAnt* ant);

#endif
