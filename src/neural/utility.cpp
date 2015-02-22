#include "utility.h"
#include <random>

double nn_util_genrandom() {
  return (double)rand() / RAND_MAX;
}

double nn_util_sigmoid(double t) {
  // return 1.0 / (1.0 + exp(-t));
  return (t / ((1.0 + abs(t)) * 2.0)) + 0.5; // Faster
}
