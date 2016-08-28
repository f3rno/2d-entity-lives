#include "util.h"
#include <random>

int randi(int limit) {
  return ((float)rand() / RAND_MAX) * limit;
}

