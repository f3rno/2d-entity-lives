#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "types.h"

struct SNeuralLayer;
struct SNeuralNetwork {
  SNeuralLayer** layers;
  uint n_layers;
  uint n_weights; // Saved for easier serializing/unserializing

  // Output buffers for faster layer processing/feed-forward
  double** output_buffers;
};

struct SNeuralNetworkData {
  uint n_layers;
  uint* l_inputs;
  uint* l_sizes;

  double* data;
};

SNeuralNetwork* nn_network_create(uint n_layers, uint* l_sizes, uint* l_inputs);
SNeuralNetwork* nn_network_create(SNeuralNetworkData* data);

void nn_network_delete_data(SNeuralNetworkData* data);
SNeuralNetworkData* nn_network_create_data(uint n_layers, uint n_data);

void nn_network_delete(SNeuralNetwork* network);
void nn_network_process(SNeuralNetwork* network, double* inputs, double* outputs);
uint nn_network_size(SNeuralNetwork* network);
uint nn_network_size(SNeuralNetworkData* data);
void nn_network_save(SNeuralNetwork* network, SNeuralNetworkData* output);
void nn_network_load(SNeuralNetwork* network, SNeuralNetworkData* input);

#endif