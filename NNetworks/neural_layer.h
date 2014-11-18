#ifndef NEURAL_LAYER_H
#define NEURAL_LAYER_H

#include "types.h"

// POD to speed things up
struct SNeuron;
struct SNeuralLayer {
  SNeuron** neurons;
  uint n_inputs;
  uint n_neurons;
};

SNeuralLayer* nn_layer_create(uint inputs, uint neurons);
void nn_layer_delete(SNeuralLayer* layer);
void nn_layer_process(SNeuralLayer* layer, double* inputs, double* outputs);

#endif