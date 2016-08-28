#include "util.h"
#include <stdlib.h>

int randi(int limit) {
  return ((float)rand() / RAND_MAX) * limit;
}

double randd(double limit) {
  return ((double)rand() / RAND_MAX) * limit;
}

float randf(float limit) {
  return ((float)rand() / RAND_MAX) * limit;
}
