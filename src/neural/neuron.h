#ifndef NEURON_H
#define NEURON_H

#include <stdint.h>

typedef struct SNeuron {
  double* weights;
  uint16_t n_weights;
} SNeuron;

SNeuron* nn_neuron_create(uint16_t n_inputs, double bias);
void nn_neuron_delete(SNeuron* neuron);
void nn_neuron_randomize_weights(SNeuron* neuron);
double nn_neuron_process(SNeuron* neuron, double* inputs);

#endif
