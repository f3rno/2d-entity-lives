#include "ant.h"
#include "neural/neural_network.h"
#include <vector>
#include <cmath>
#include "vector2.h"
#include "world.h"
#include "food.h"
#include <algorithm>
#include <iostream>

#define PI 3.1415926f

CAnt::CAnt(CWorld* world, CVector2 start_pos, uint16_t _generation, uint16_t _id) :
CEntity(world, start_pos) {

  generation = _generation;
  target_food = NULL;
  fitness = 0;
  id = _id;

  /*body->setFillColor(sf::Color(230, 126, 34));
  body->setOrigin(2.5, 2.5);

  status_str.setFont(*world->getFont());
  status_str.setColor(sf::Color::Green);
  status_str.setCharacterSize(11);*/

  uint16_t* brain_layer_sizes = new uint16_t[3];
  uint16_t* brain_layer_inputs = new uint16_t[3];

  brain_layer_sizes[0] = 4;
  brain_layer_inputs[0] = 4; // 4 inputs (fx, fy, x, y)

  brain_layer_sizes[1] = 6;
  brain_layer_inputs[1] = 4;

  brain_layer_sizes[2] = 2; // 2 outputs, rotation and impulse
  brain_layer_inputs[2] = 6;

  brain = nn_network_create(3, brain_layer_sizes, brain_layer_inputs);
  brain_outputs = new double[2];
  brain_inputs = new double[4];

  delete[] brain_layer_sizes;
  delete[] brain_layer_inputs;
}

uint16_t CAnt::getGeneration() {
  return generation;
}

// Give the ant a chance to update its state
void CAnt::step() {
  target_food = world->getNearestFood(position);

  // Only act if there is food in the world, otherwise we die anyway
  if (target_food != NULL) {

    // Calculate food vector
    CVector2 foodV(position, target_food->getPosition());

    // If food is close enough, consume it
    if (foodV.length() <= 3.5f) {

      fitness += 1;
      world->consumeFood(target_food);
      target_food = NULL;

    } else {
      foodV.normalise();

      float angle = rotation;
      float angle_r = angle * (PI / 180.0f);
      float look_x = -1.0f * sin(angle_r);
      float look_y = cos(angle_r);

      brain_inputs[0] = foodV.x;
      brain_inputs[1] = foodV.y;
      brain_inputs[2] = look_x;
      brain_inputs[3] = look_y;

      nn_network_process(brain, brain_inputs, brain_outputs);

      // Give ants a way to 
      brain_outputs[0] = (brain_outputs[0] - 0.5f);
      brain_outputs[1] = (brain_outputs[1] - 0.5f) * 4.0f;

      rotate(brain_outputs[0] * (180.0f / PI));
      move(brain_outputs[1] * look_x, brain_outputs[1] * look_y);
    }
  }
}

void CAnt::getGenome(SNeuralNetworkData* output) {
  nn_network_save(brain, output);
}

void CAnt::insertGenome(SNeuralNetworkData* input, uint16_t _parent_1_id, uint16_t _parent_2_id) {
  nn_network_load(brain, input);

  parent_1_id = _parent_1_id;
  parent_2_id = _parent_2_id;
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

uint16_t CAnt::getID() {
  return id;
}

uint16_t CAnt::getParent1ID() {
  return parent_1_id;
}

uint16_t CAnt::getParent2ID() {
  return parent_2_id;
}

CAnt::~CAnt() {
  nn_network_delete(brain);

  delete[] brain_outputs;
  delete[] brain_inputs;
}
