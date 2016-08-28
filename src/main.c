#include <time.h>
#include <stdlib.h>
#include "world.h"

int main(int argc, char *argv[]) {
  srand(time(NULL));

  SWorld* world = world_create(800, 800, 10, 30);
  world_init(world);

  for(int i = 0; i < 1000000; i++) {
    world_step(world);
  }

  world_delete(world);
  return 0;
}
