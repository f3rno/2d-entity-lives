#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <stdint.h>
#include "neural_layer.h"

typedef struct SNeuralNetwork {
  SNeuralLayer** layers;
  uint16_t n_layers;
  uint16_t n_weights; // Saved for easier serializing/unserializing

  // Output buffers for faster layer processing/feed-forward
  double** output_buffers;
} SNeuralNetwork;

typedef struct SNeuralNetworkData {
  uint16_t n_layers;
  uint16_t* l_inputs;
  uint16_t* l_sizes;

  double* data;
} SNeuralNetworkData;

SNeuralNetwork* nn_network_create(uint16_t n_layers, uint16_t* l_sizes, uint16_t* l_inputs);
SNeuralNetwork* nn_network_create_from_data(SNeuralNetworkData* data);

void nn_network_delete_data(SNeuralNetworkData* data);
SNeuralNetworkData* nn_network_create_data(uint16_t n_layers, uint16_t n_data);

void nn_network_delete(SNeuralNetwork* network);
void nn_network_process(SNeuralNetwork* network, double* inputs, double* outputs);
uint16_t nn_network_size(SNeuralNetwork* network);
uint16_t nn_network_size_from_data(SNeuralNetworkData* data);
void nn_network_save(SNeuralNetwork* network, SNeuralNetworkData* output);
void nn_network_load(SNeuralNetwork* network, SNeuralNetworkData* input);

#endif
