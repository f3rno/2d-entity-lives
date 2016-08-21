#include <cstdlib>
#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include "world.h"
#include "neural/neural_network.h"

using namespace std;

int main(int argc, char *argv[]) {

  CWorld* world = new CWorld(800, 800, 10, 30);

  for(int i = 0; i < 1000000; i++) {
    world->step();
  }

  delete world;

  return 0;
}
