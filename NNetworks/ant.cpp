#include "ant.h"
#include "neural_network.h"
#include <vector>
#include <cmath>
#include "vector2.h"
#include "world.h"
#include "food.h"
#include <algorithm>

#define PI 3.1415926f

CAnt::CAnt(CWorld* world, sf::Vector2f start_pos, uint _generation) :
CEntity(world, new sf::RectangleShape(sf::Vector2f(5, 5)), start_pos) {

  generation = _generation;
  death_mark = false;
  health = 100;
  target_food = NULL;
  age = 0;
  fitness = 0;
  fertility = CAntFertility::NEWBORN;

  body->setFillColor(sf::Color(230, 126, 34));
  body->setOrigin(2.5, 2.5);

  status_str.setFont(*world->getFont());
  status_str.setColor(sf::Color::Green);
  status_str.setCharacterSize(11);

  uint* brain_layer_sizes = new uint[3];
  uint* brain_layer_inputs = new uint[3];

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

bool CAnt::needsToDie() {
  return death_mark;
}

uint CAnt::getGeneration() {
  return generation;
}

uint CAnt::getFitness() {
  return fitness;
}

void CAnt::hungerTick() {
  health -= 1;
}

CAntFertility CAnt::getFertility() {
  return fertility;
}

bool CAnt::isFertile() {
  return fertility == CAntFertility::FERTILE;
}

void CAnt::advanceFertility() {
  if (fertility == CAntFertility::SENIOR) {
    return;
  }
  else if (fertility == CAntFertility::NEWBORN) {
    fertility = CAntFertility::FERTILE;
  }
  else if (fertility == CAntFertility::FERTILE) {
    fertility = CAntFertility::SENIOR;
    death_mark = true;
  }
}

// Give the ant a chance to update its state
void CAnt::step() {

  // If out of health, death
  if (health == 0) {
    death_mark = true;
    return;
  }

  manageFertility();
  target_food = world->getNearestFood(body->getPosition());

  // Only act if there is food in the world, otherwise we die anyway
  if (target_food != NULL) {

    // Calculate food vector
    CVector2 position = getPosition();
    CVector2 foodV(position, target_food->getPosition());

    // If food is close enough, consume it
    if (foodV.length() <= 3.5f) {

      fitness += 1;
      health += 20; // Give health for eating
      world->consumeFood(target_food);
      target_food = NULL;

    } else {
      CVector2 normalV(position, CVector2(position.x, position.y + 10.0f));
      foodV.normalise();
      normalV.normalise();

      float angle = body->getRotation() * (PI / 180.0f);
      float cos_a = cos(angle);
      float sin_a = sin(angle);

      brain_inputs[0] = foodV.x;
      brain_inputs[1] = foodV.y;
      brain_inputs[2] = (double) cos_a;
      brain_inputs[3] = (double) sin_a;

      nn_network_process(brain, brain_inputs, brain_outputs);

      body->rotate(brain_outputs[0] - 0.5f);
      body->move(brain_outputs[1] * cos_a * 0.1f, brain_outputs[1] * sin_a * 0.1f);
    }
  }

  age++;
}

void CAnt::manageFertility() {
  if (fertility == CAntFertility::NEWBORN && age >= CWorld::FERTILITY_START) {
    advanceFertility();
  }

  if (fertility == CAntFertility::FERTILE) {
    if (age >= CWorld::FERTILITY_START && age <= CWorld::FERTILITY_END) {
      world->requestOffspring(this);
    }
    else {
      advanceFertility();
    }
  }
}

void CAnt::getGenome(SNeuralNetworkData* output) {
  nn_network_save(brain, output);
}

void CAnt::insertGenome(SNeuralNetworkData* input) {
  nn_network_load(brain, input);
}

uint CAnt::getAge() {
  return age;
}

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
  ss << "G" << generation << " F" << fitness;
  status_str.setString(ss.str());
  status_str.setPosition(sf::Vector2f(body_position.x - 16.0f, body_position.y - 16.0f));
  window->draw(status_str);
}

CAnt::~CAnt() {
  nn_network_delete(brain);

  delete[] brain_outputs;
  delete[] brain_inputs;
}