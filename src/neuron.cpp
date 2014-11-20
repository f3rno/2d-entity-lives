#include "neuron.h"
#include "utility.h"
#include <cmath>

// Creat a neuron with n_inputs
SNeuron* nn_neuron_create(uint n_inputs, double bias) {
  SNeuron* neuron = new SNeuron();

  neuron->n_weights = n_inputs + 1;
  neuron->weights = new double[n_inputs + 1];
  nn_neuron_randomizeWeights(neuron);
  neuron->weights[n_inputs] = bias;

  return neuron;
}

// Delete the neuron and all of its weights
void nn_neuron_delete(SNeuron* neuron) {
  delete[] neuron->weights;
  delete neuron;
}

void nn_neuron_randomizeWeights(SNeuron* neuron) {
  for (uint i = 0; i < neuron->n_weights; i++) {
    neuron->weights[i] = nn_util_genrandom();
  }
}

// Process the provided inputs for the neuron, returns raw output
double nn_neuron_process(SNeuron* neuron, double* inputs) {
  double output = 0;

  for (uint i = 0; i < neuron->n_weights - 2; i++) {
    output += neuron->weights[i] * inputs[i];
  }
  
  // Subtract bias seperately at the end
  output -= neuron->weights[neuron->n_weights - 1];

  return output;
}