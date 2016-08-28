#ifndef NEURAL_LAYER_H
#define NEURAL_LAYER_H

#include <stdint.h>
#include "neuron.h"

// POD to speed things up
typedef struct SNeuralLayer {
  SNeuron** neurons;
  uint16_t n_inputs;
  uint16_t n_neurons;
} SNeuralLayer;

SNeuralLayer* nn_layer_create(uint16_t inputs, uint16_t neurons);
void nn_layer_delete(SNeuralLayer* layer);
void nn_layer_process(SNeuralLayer* layer, double* inputs, double* outputs);

#endif
