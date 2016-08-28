#include <stdlib.h>
#include "neural_layer.h"
#include "neuron.h"
#include "util.h"

// Creates a new neural layer with n_neurons, each with n_inputs
SNeuralLayer* nn_layer_create(uint16_t n_inputs, uint16_t n_neurons) {
  SNeuralLayer* layer = (SNeuralLayer*)malloc(sizeof(SNeuralLayer));

  layer->n_inputs = n_inputs;
  layer->n_neurons = n_neurons;
  layer->neurons = (SNeuron**)malloc(sizeof(SNeuron*) * n_neurons);

  for (uint16_t i = 0; i < n_neurons; i++) {
    layer->neurons[i] = nn_neuron_create(n_inputs, 1);
  }

  return layer;
}

// Deletes the layer and all of its neurons
void nn_layer_delete(SNeuralLayer* layer) {
  for (uint16_t i = 0; i < layer->n_neurons; i++) {
    nn_neuron_delete(layer->neurons[i]);
  }

  free(layer->neurons);
  free(layer);
}

// Processes inputs and writes outputs to the specified array
void nn_layer_process(SNeuralLayer* layer, double* inputs, double* outputs) {
  for (uint16_t i = 0; i < layer->n_neurons; i++) {
    outputs[i] = nn_util_sigmoid(nn_neuron_process(layer->neurons[i], inputs));
  }
}
