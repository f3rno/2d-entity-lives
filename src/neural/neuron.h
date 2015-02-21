#ifndef NEURON_H
#define NEURON_H

#include <stdint.h>

// POD to speed things up
struct SNeuron {

  double* weights;
  uint16_t n_weights;

};

SNeuron* nn_neuron_create(uint16_t n_inputs, double bias);
void nn_neuron_delete(SNeuron* neuron);
void nn_neuron_randomizeWeights(SNeuron* neuron);
double nn_neuron_process(SNeuron* neuron, double* inputs);

#endif
