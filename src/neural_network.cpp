#include "neural_network.h"
#include "neural_layer.h"
#include "neuron.h"
#include <assert.h>

// Creates a layer with the specified layers and layer sizes
SNeuralNetwork* nn_network_create(uint n_layers, uint* l_sizes, uint* l_inputs) {
  SNeuralNetwork* network = new SNeuralNetwork;

  network->n_weights = 0;
  network->n_layers = n_layers;
  network->layers = new SNeuralLayer*[n_layers];
  network->output_buffers = new double*[n_layers - 1];

  for (uint i = 0; i < n_layers; i++) {
    network->layers[i] = nn_layer_create(l_inputs[i], l_sizes[i]);
    network->n_weights += l_sizes[i] * (l_inputs[i] + 1);

    // All but the last layer have buffers
    if (i < n_layers - 1) {
      network->output_buffers[i] = new double[l_sizes[i]];
    }
  }

  return network;
}

SNeuralNetwork* nn_network_create(SNeuralNetworkData* data) {
  SNeuralNetwork* network = nn_network_create(data->n_layers, data->l_sizes, data->l_inputs);
  nn_network_load(network, data);
  return network;
}

void nn_network_delete(SNeuralNetwork* network) {

  for (uint i = 0; i < network->n_layers; i++) {
    nn_layer_delete(network->layers[i]);

    // All but the last layer have buffers
    if (i < network->n_layers - 1) {
      delete[] network->output_buffers[i];
    }
  }

  delete[] network->output_buffers;
  delete[] network->layers;
  delete network;
}

void nn_network_process(SNeuralNetwork* network, double* inputs, double* outputs) {
  
  // If we only have one layer, pass outputs directly
  if (network->n_layers == 1) {
    nn_layer_process(network->layers[0], inputs, outputs);
  } else {
    for (uint i = 0; i < network->n_layers; i++) {

      if (i == 0) {
        // Use inputs and pass result to first buffer
        nn_layer_process(network->layers[0], inputs, network->output_buffers[0]);
      } else if (i > 0 && i < network->n_layers - 1) {
        // Store intermediate result in an output buffer
        nn_layer_process(network->layers[i], network->output_buffers[i - 1], network->output_buffers[i]);
      } else {
        // Output last layer result directly
        nn_layer_process(network->layers[i], network->output_buffers[i - 1], outputs);
      }

    }
  }

}

uint nn_network_size(SNeuralNetwork* network) {
  uint size = 0;

  for (uint i = 0; i < network->n_layers; i++) {

    // We add an input for the hidden bias
    size += (network->layers[i]->n_inputs + 1) * network->layers[i]->n_neurons;
  }

  return size;
}

uint nn_network_size(SNeuralNetworkData* data) {
  uint size = 0;

  for (uint i = 0; i < data->n_layers; i++) {
    size += (data->l_inputs[i] + 1) * data->l_sizes[i];
  }

  return size;
}

void nn_network_save(SNeuralNetwork* network, SNeuralNetworkData* output) {
  output->n_layers = network->n_layers;
  output->l_inputs = new uint[network->n_layers];
  output->l_sizes = new uint[network->n_layers];
  output->data = new double[nn_network_size(network)];

  uint data_index = 0;

  for (uint i = 0; i < network->n_layers; i++) {
    output->l_inputs[i] = network->layers[i]->n_inputs;
    output->l_sizes[i] = network->layers[i]->n_neurons;

    for (uint n = 0; n < network->layers[i]->n_neurons; n++) {
      for (uint w = 0; w < network->layers[i]->neurons[n]->n_weights; w++) {
        output->data[data_index] = network->layers[i]->neurons[n]->weights[w];
        data_index++;
      }
    }
  }
}

void nn_network_load(SNeuralNetwork* network, SNeuralNetworkData* input) {
  uint data_index = 0;

  for (uint i = 0; i < network->n_layers; i++) {
    for (uint n = 0; n < network->layers[i]->n_neurons; n++) {
      for (uint w = 0; w < network->layers[i]->neurons[n]->n_weights; w++) {
        network->layers[i]->neurons[n]->weights[w] = input->data[data_index];
        data_index++;
      }
    }
  }
}

void nn_network_delete_data(SNeuralNetworkData* data) {
  delete[] data->l_inputs;
  delete[] data->l_sizes;
  delete data;
}

SNeuralNetworkData* nn_network_create_data(uint n_layers, uint n_data) {
  SNeuralNetworkData* data = new SNeuralNetworkData;

  data->n_layers = n_layers;
  data->l_inputs = new uint[n_layers];
  data->l_sizes = new uint[n_layers];
  data->data = new double[n_data];

  return data;
}