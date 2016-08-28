#include <stdlib.h>
#include <math.h>
#include "ant.h"
#include "neural/neural_network.h"
#include "world.h"
#include "qtree/qtree_item.h"
#include "qtree/qtree.h"
#include "util.h"

#define PI 3.1415926f

SAnt* ant_create(uint16_t x, uint16_t y, float angle, uint16_t gen, uint16_t id) {
  SAnt* ant = (SAnt*)malloc(sizeof(SAnt));
  ant->pos = vec2_create(x, y);
  ant->rotation = angle;
  ant->generation = gen;
  ant->id = id;

  ant->fitness = 0;

  /*body->setFillColor(sf::Color(230, 126, 34));
  body->setOrigin(2.5, 2.5);

  status_str.setFont(*world->getFont());
  status_str.setColor(sf::Color::Green);
  status_str.setCharacterSize(11);*/

  uint16_t* brain_layer_sizes = (uint16_t*)malloc(sizeof(uint16_t) * 3);
  uint16_t* brain_layer_inputs = (uint16_t*)malloc(sizeof(uint16_t) * 3);

  brain_layer_sizes[0] = 4;
  brain_layer_inputs[0] = 4; // 4 inputs (fx, fy, x, y)

  brain_layer_sizes[1] = 6;
  brain_layer_inputs[1] = 4;

  brain_layer_sizes[2] = 2; // 2 outputs, rotation and impulse
  brain_layer_inputs[2] = 6;

  ant->brain = nn_network_create(3, brain_layer_sizes, brain_layer_inputs);
  ant->brain_outputs = (double*)malloc(sizeof(double) * 2);
  ant->brain_inputs = (double*)malloc(sizeof(double) * 4);

  free(brain_layer_sizes);
  free(brain_layer_inputs);

  return ant;
}

// Update an ant's state for one tick
void ant_step(SAnt* ant, SWorld* world, float* vec2f, uint16_t* vec2i) {
  SQTreeItem* target_food = sqtree_find_nearest(world->food_tree, ant->pos);

  // Only act if there is food in the world, otherwise we die anyway
  if (target_food != NULL) {

    // Calculate food vector
    vec2i[0] = target_food->pos[0] - ant->pos[0];
    vec2i[1] = target_food->pos[1] - ant->pos[1];

    // If food is close enough, consume it
    if (vec2_length(vec2i) <= 3.5f) {

      ant->fitness += 1;
      world_consume_food(world, target_food);

    } else {

      // Grab normal
      vec2_normal(vec2i, vec2f);

      float look_x = -1.0f * sin(ant->rotation);
      float look_y = cos(ant->rotation);

      ant->brain_inputs[0] = vec2f[0];
      ant->brain_inputs[1] = vec2f[1];
      ant->brain_inputs[2] = look_x;
      ant->brain_inputs[3] = look_y;

      nn_network_process(ant->brain, ant->brain_inputs, ant->brain_outputs);

      ant->brain_outputs[0] = (ant->brain_outputs[0] - 0.5f);
      ant->brain_outputs[1] = (ant->brain_outputs[1] - 0.5f) * 4.0f;

      ant->rotation += ant->brain_outputs[0];
      ant->pos[0] += ant->brain_outputs[1] * look_x;
      ant->pos[1] += ant->brain_outputs[1] * look_y;
    }
  }
}

void ant_insert_genome(SAnt* ant, SNeuralNetworkData* input, uint16_t parent_1_id, uint16_t parent_2_id) {
  nn_network_load(ant->brain, input);

  ant->parent_1_id = parent_1_id;
  ant->parent_2_id = parent_2_id;
}

// TODO: Allow mating only when ants are next to each other; this removes the
// SWorld* ref dependency
SAnt* ant_mate(SWorld* w, SAnt* a, SAnt* b, int gen, int id, float mutation_rate) {
  SNeuralNetworkData* child_genome = (SNeuralNetworkData*)malloc(sizeof(SNeuralNetworkData));
  SNeuralNetworkData* a_genome = (SNeuralNetworkData*)malloc(sizeof(SNeuralNetworkData));;
  SNeuralNetworkData* b_genome = (SNeuralNetworkData*)malloc(sizeof(SNeuralNetworkData));;

  nn_network_save(a->brain, a_genome);
  nn_network_save(b->brain, b_genome);

  // For simplicity, use a parent genome as scaffolding
  nn_network_save(a->brain, child_genome);

  // Not that we start in a random crossover configuration
  uint16_t genome_size = nn_network_size_from_data(a_genome);
  bool crossover = randf(1) <= 0.5f;
  uint16_t crossover_point = rand() % genome_size;

  for (uint16_t i = 0; i < genome_size; i++) {
    if (i == crossover_point) {
      crossover = !crossover; // Swap parent genome source
    }

    if (crossover) {
      child_genome->data[i] = a_genome->data[i];
    } else {
      child_genome->data[i] = b_genome->data[i];
    }

    // Mutate
    if (randf(1) <= mutation_rate) {
      child_genome->data[i] = randf(1);
    }
  }

  // Birth!
  uint16_t offspring_x = (uint16_t)randi(w->width);
  uint16_t offspring_y = (uint16_t)randi(w->height);

  SAnt* offspring = ant_create(offspring_x, offspring_y, 0, gen, id);

  ant_insert_genome(offspring, child_genome, a->id, b->id);

  nn_network_delete_data(child_genome);
  nn_network_delete_data(a_genome);
  nn_network_delete_data(b_genome);

  return offspring;
}

void ant_delete(SAnt* ant) {
  nn_network_delete(ant->brain);
  vec2_delete(ant->pos);

  free(ant->brain_outputs);
  free(ant->brain_inputs);
  free(ant);
}
