#include "utility.h"
#include <random>

double nn_util_genrandom() {
  return (double)rand() / RAND_MAX;
}

double nn_util_sigmoid(double t) {
  return 1.0 / (1.0 + exp(-t));
}