#include <stdlib.h>
#include <math.h>
#include "neuron.h"
#include "util.h"
#include "../util.h"

// Creat a neuron with n_inputs
SNeuron* nn_neuron_create(uint16_t n_inputs, double bias) {
  SNeuron* neuron = (SNeuron*)malloc(sizeof(SNeuron));

  neuron->n_weights = n_inputs + 1;
  neuron->weights = (double*)malloc(sizeof(double) * (n_inputs + 1));
  nn_neuron_randomize_weights(neuron);
  neuron->weights[n_inputs] = bias;

  return neuron;
}

// Delete the neuron and all of its weights
void nn_neuron_delete(SNeuron* neuron) {
  free(neuron->weights);
  free(neuron);
}

void nn_neuron_randomize_weights(SNeuron* neuron) {
  for (uint16_t i = 0; i < neuron->n_weights; i++) {
    neuron->weights[i] = randd(1);
  }
}

// Process the provided inputs for the neuron, returns raw output
double nn_neuron_process(SNeuron* neuron, double* inputs) {
  double output = 0;

  for (uint16_t i = 0; i < neuron->n_weights - 2; i++) {
    output += neuron->weights[i] * inputs[i];
  }

  // Subtract bias seperately at the end
  output -= neuron->weights[neuron->n_weights - 1];

  return output;
}
