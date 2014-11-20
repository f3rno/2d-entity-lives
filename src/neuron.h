#ifndef NEURON_H
#define NEURON_H

#include "types.h"

// POD to speed things up
struct SNeuron {

  double* weights;
  uint n_weights;

};

SNeuron* nn_neuron_create(uint n_inputs, double bias);
void nn_neuron_delete(SNeuron* neuron);
void nn_neuron_randomizeWeights(SNeuron* neuron);
double nn_neuron_process(SNeuron* neuron, double* inputs);

#endif