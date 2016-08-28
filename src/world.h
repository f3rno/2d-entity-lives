#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include "qtree/qtree.h"
#include "qtree/qtree_item.h"

struct SAnt;
typedef struct SWorld {
  uint16_t width, height;
  uint16_t population;
  uint16_t food_count;
  uint32_t generation;

  uint16_t last_ant_id;
  uint32_t steps_to_next_generation;
  uint32_t generation_steps;

  float mutation_rate;

  struct SAnt** ants;
  SQTreeItem** food;
  SQTree* food_tree; // Doesn't take ownership of food objects
} SWorld;

SWorld* world_create(uint16_t w, uint16_t h, uint16_t pop, uint16_t food);
SQTreeItem* world_create_food();

void world_init(SWorld* world);
void world_init_food(SWorld* world);
void world_init_gen(SWorld* world);
void world_reset_gen_steps(SWorld* world);

float world_calc_fitness_cutoff(SWorld* world);

void world_consume_food(SWorld* world, SQTreeItem* food);
void world_step(SWorld* world);
void world_step_ants(SWorld* world);
void world_step_ant_range(SWorld* world, uint16_t start, uint16_t end);
void world_wrap_screen_edges(SWorld* world);
void world_mate(SWorld* world);

void world_purge_food(SWorld* world);
void world_purge_population(SWorld* world);
void world_delete(SWorld* world);

#endif
