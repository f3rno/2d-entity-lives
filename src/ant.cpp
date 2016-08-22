#include "ant.h"
#include "neural/neural_network.h"
#include <vector>
#include <cmath>
#include "vector2.h"
#include "world.h"
#include "qtree/qtree_item.h"
#include <algorithm>
#include <iostream>

#define PI 3.1415926f

SAnt* ant_create(uint16_t x, uint16_t y, float angle, uint16_t gen, uint16_t id) {
  SAnt* ant = (SAnt*)malloc(sizeof(SAnt));
  ant->x = x;
  ant->y = y;
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
void ant_step(SAnt* ant, CWorld* world) {
  SQTreeItem* target_food = world->getNearestFood(ant->x, ant->y);

  // Only act if there is food in the world, otherwise we die anyway
  if (target_food != NULL) {

    // Calculate food vector
    CVector2 position(ant->x, ant->y);
    CVector2 foodV(position, CVector2(target_food->x, target_food->y));

    // If food is close enough, consume it
    if (foodV.length() <= 3.5f) {

      ant->fitness += 1;
      world->consumeFood(target_food);

    } else {
      foodV.normalise();

      float angle = ant->rotation;
      float angle_r = angle * (PI / 180.0f);
      float look_x = -1.0f * sin(angle_r);
      float look_y = cos(angle_r);

      ant->brain_inputs[0] = foodV.x;
      ant->brain_inputs[1] = foodV.y;
      ant->brain_inputs[2] = look_x;
      ant->brain_inputs[3] = look_y;

      nn_network_process(ant->brain, ant->brain_inputs, ant->brain_outputs);

      ant->brain_outputs[0] = (ant->brain_outputs[0] - 0.5f);
      ant->brain_outputs[1] = (ant->brain_outputs[1] - 0.5f) * 4.0f;

      ant->rotation += ant->brain_outputs[0] * (180.0f / PI);
      ant->x += ant->brain_outputs[1] * look_x;
      ant->y += ant->brain_outputs[1] * look_y;
    }
  }
}

/*
void CAnt::getGenome(SNeuralNetworkData* output) {
  nn_network_save(brain, output);
}*/

void ant_insert_genome(SAnt* ant, SNeuralNetworkData* input, uint16_t parent_1_id, uint16_t parent_2_id) {
  nn_network_load(ant->brain, input);

  ant->parent_1_id = parent_1_id;
  ant->parent_2_id = parent_2_id;
}

/*
void CAnt::draw(sf::RenderWindow* window) {

  // Draw food target line
  sf::Vector2f body_position = body->getPosition();

  if (target_food != NULL) {
    sf::Vertex food_line[] = {
      sf::Vertex(body_position, sf::Color(100, 100, 100, 64)),
      sf::Vertex(target_food->getBody()->getPosition(), sf::Color(200, 50, 50, 200))
    };

    window->draw(food_line, 2, sf::Lines);
  }

  window->draw(*body);

  ss.str(std::string());
  ss << "F" << fitness;
  status_str.setString(ss.str());
  status_str.setPosition(sf::Vector2f(body_position.x - 16.0f, body_position.y - 16.0f));
  window->draw(status_str);
}*/

void ant_delete(SAnt* ant) {
  nn_network_delete(ant->brain);

  free(ant->brain_outputs);
  free(ant->brain_inputs);
  free(ant);
}
