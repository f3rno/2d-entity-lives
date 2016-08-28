#include <stdlib.h>
#include <assert.h>
#include "world.h"
#include "ant.h"
#include "neural/neural_network.h"
#include "neural/util.h"
#include "qtree/qtree.h"
#include "util.h"
#include "vec2.h"

SWorld* world_create(uint16_t w, uint16_t h, uint16_t pop, uint16_t food) {
  SWorld* world = (SWorld*)malloc(sizeof(SWorld));

  world->width = w;
  world->height = h;
  world->population = pop;
  world->food_count = food;

  // Sane defaults
  world->generation = 0;
  world->last_ant_id = 0;
  world->steps_to_next_generation = 0;
  world->generation_steps = 10000;
  world->mutation_rate = 0.02f;

  world->ants = (SAnt**)malloc(sizeof(SAnt*) * pop);
  world->food = (SQTreeItem**)malloc(sizeof(SQTreeItem*) * food);
  world->food_tree = sqtree_create(0, 8, w, h, 0, 0);

  return world;
}

/**
 * Create a new food set to a random position. The food item is not tracked
 *
 * @param world
 * @return item
 */
SQTreeItem* world_create_food(SWorld* world) {
  uint16_t x = (uint16_t)randi(world->width);
  uint16_t y = (uint16_t)randi(world->height);

  return sqtree_item_create(NULL, x, y);
}

/**
 * Spawns the initial population and food store
 *
 * @param world
 */
void world_init(SWorld* world) {
  world_reset_gen_steps(world);
  world_init_food(world);
  world_init_gen(world);
}

/**
 * Spawns a randomized food store in the target world
 *
 * @param world
 */
void world_init_food(SWorld* world) {
  for(uint16_t i = 0; i < world->food_count; i++) {
    world->food[i] = world_create_food(world);
  }
}

/**
 * Spawns a randomized generation in the target world
 *
 * @param world
 */
void world_init_gen(SWorld* world) {
  world->generation = 1;

  for (uint16_t i = 0; i < world->population; i++) {
    uint16_t x = (uint16_t)randi(world->width);
    uint16_t y = (uint16_t)randi(world->height);

    world->ants[i] = ant_create(x, y, 0, 1, world->last_ant_id);
    world->last_ant_id++;
  }
}

/**
 * Resets the generation lifecycle counter
 *
 * @param world
 */
void world_reset_gen_steps(SWorld* world) {
  world->steps_to_next_generation = world->generation_steps;
}

float world_calc_fitness_cutoff(SWorld* world) {
  float max = 0;

  for (uint16_t i = 0; i < world->population; i++) {
    if (world->ants[i]->fitness > max) {
      max = world->ants[i]->fitness;
    }
  }

  return max * 0.75;
}

void world_mate(SWorld* world) {
  world->generation += 1;

  // Note that we randomly select couples!
  SAnt* parents[2];
  parents[0] = NULL;
  parents[1] = NULL;

  SAnt** new_generation = (SAnt**)malloc(sizeof(SAnt*) * world->population);
  uint16_t ants_spawned = 0;

  float fitness_cutoff = world_calc_fitness_cutoff(world);
  uint16_t loop_count = 0; // We decrease the cutoff as we run out of options

  while (ants_spawned < world->population) {
    for (uint16_t i = 0; i < world->population; i++) {

      // Eugenics FTW
      if (world->ants[i]->fitness > fitness_cutoff) {
        if (parents[0] == NULL) {
          parents[0] = world->ants[i];
        } else if (parents[0] != world->ants[i] && parents[1] == NULL) {
          parents[1] = world->ants[i];

          new_generation[ants_spawned] = ant_mate(
            world,
            parents[0],
            parents[1],
            world->generation,
            world->last_ant_id,
            world->mutation_rate
          );

          ants_spawned++;
          world->last_ant_id += 1;

          parents[0] = NULL;
          parents[1] = NULL;

          if (ants_spawned  == world->population) {
            break;
          }
        }
      }
    }

    loop_count++;

    if (loop_count % 10 == 0) {
      fitness_cutoff -= 1;
    }
  }

  // Transfer generation over
  world_purge_population(world);
  free(world->ants);

  world->ants = new_generation;
}

/**
 * Fails with an assert if the food is not found
 *
 * @param food
 */
void world_consume_food(SWorld* world, SQTreeItem* food) {
  for (uint16_t i = 0; i < world->food_count; i++) {
    if (world->food[i] == food) {
      sqtree_remove(world->food_tree, food);
      sqtree_item_delete(food);

      // Replace it
      world->food[i] = world_create_food(world);

      return;
    }
  }

  assert(false);
}

/**
 * Simulates the world for one tick
 *
 * @param world
 */
void world_step(SWorld* world) {
  world_wrap_screen_edges(world);
  world_step_ants(world);

  world->steps_to_next_generation--;

  if (world->steps_to_next_generation == 0) {
    world_mate(world);
    world_reset_gen_steps(world);
  }

}

/**
 * Steps a range of ants in the world. This is meant to be used in a
 * multithreaded configuration
 *
 * @param world
 * @param start_index
 * @param end_index
 */
void world_step_ant_range(SWorld* world, uint16_t start, uint16_t end) {
  float* vec2f = (float*)malloc(sizeof(float) * 2);
  uint16_t* vec2i = vec2_zero();

  for (uint16_t i = start; i < end; i++) {
    ant_step(world->ants[i], world, vec2f, vec2i);
  }

  free(vec2f);
  free(vec2i);
}

/**
 * Steps all ants
 *
 * @param world
 */
void world_step_ants(SWorld* world) {
  world_step_ant_range(world, 0, world->population);
}

// TODO: Kill this, give ants a vector to the nearest edge
void world_wrap_screen_edges(SWorld* world) {
  for (uint16_t i = 0; i < world->population; i++) {
    SAnt* ant = world->ants[i];

    while (ant->pos[0] > world->width) {
      ant->pos[0] -= world->width;
    }

    while (ant->pos[0] < 0) {
      ant->pos[0] += world->width;
    }

    while (ant->pos[1] > world->height) {
      ant->pos[1] -= world->height;
    }

    while (ant->pos[1] < 0) {
      ant->pos[1] += world->height;
    }
  }
}

void world_purge_population(SWorld* world) {
  for (uint16_t i = 0; i < world->population; i++) {
    ant_delete(world->ants[i]);
    world->ants[i] = NULL;
  }
}

void world_purge_food(SWorld* world) {
  for (uint16_t i = 0; i < world->food_count; i++) {
    sqtree_item_delete(world->food[i]);
    world->food[i] = NULL;
  }
}

void world_delete(SWorld* world) {
  world_purge_population(world);
  world_purge_food(world);

  free(world->food);
  free(world->ants);

  sqtree_delete(world->food_tree);

  free(world);
}
