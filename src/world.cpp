#include "world.h"
#include "food.h"
#include "ant.h"
#include "neural/neural_network.h"
#include "neural/utility.h"
#include "vector2.h"
#include "qtree/qtree.h"
#include <random>
#include <iostream>
#include <assert.h>
#include <algorithm>

uint16_t CWorld::GENERATION_DELAY_LOOPS = 10000;
float CWorld::MUTATION_RATE = 0.02f;

CWorld::CWorld(uint16_t _width, uint16_t _height, uint16_t _start_pop, uint16_t start_food) {
  width = _width;
  height = _height;
  start_pop = _start_pop;
  last_ant_n = 0;

  food_tree = new CQTree(0, 8, width, height, 0, 0);
  resetGenerationDelay();

  for (uint16_t i = 0; i < start_food; i++) {
    spawnFood();
  }

  advanceGeneration(1);
}

void CWorld::spawnFood() {
  CVector2 position = getRandomPosition();
  uint16_t x = (uint16_t)position.x;
  uint16_t y = (uint16_t)position.y;

  CFood* new_food = new CFood(this, position);
  food_tree->insertObject(new_food, x, y);
  food.push_back(new_food);
}

void CWorld::advanceGeneration(uint16_t gen) {
  generation = gen;

  // First generation is entirely random
  if (gen == 1) {
    for (uint16_t i = 0; i < start_pop; i++) {
      ants.push_back(new CAnt(this, getRandomPosition(), gen, last_ant_n));
      last_ant_n++;
    }
  } else {

    // Note that we randomly select couples!
    CAnt* parents[2];
    parents[0] = NULL;
    parents[1] = NULL;

    // Generate a new population of the same size
    uint16_t prev_gen_size = ants.size();
    std::vector<CAnt* > new_generation;

    float totalFitness = 0;
    float minFitness = 0; // We don't pay attention to mentally challenged ants
    float maxFitness = 0;

    for (uint16_t i = 0; i < ants.size(); i++) {
      if (ants[i]->fitness > maxFitness) {
        maxFitness = ants[i]->fitness;
      }

      if (ants[i]->fitness > 0 && (ants[i]->fitness < minFitness || minFitness == 0)) {
        minFitness = ants[i]->fitness;
      }

      totalFitness += ants[i]->fitness;
    }

    float fitnessCutoff = (1.0f - (minFitness / maxFitness)) * 0.9f;
    uint16_t loopCount = 0; // We decrease the cutoff as we run out of options

    while (new_generation.size() < prev_gen_size) {
      for (uint16_t i = 0; i < ants.size(); i++) {

        // Eugenics FTW
        if (ants[i]->fitness / maxFitness > fitnessCutoff) {
          if (parents[0] == NULL) {
            parents[0] = ants[i];
          } else if (parents[0] != ants[i] && parents[1] == NULL) {
            parents[1] = ants[i];

            CAnt* child = spawnOffspring(parents[0], parents[1], gen);
            new_generation.push_back(child);

            parents[0] = NULL;
            parents[1] = NULL;
            
            if (new_generation.size() == prev_gen_size) {
              break;
            }
          }
        }
      }

      loopCount++;

      if (loopCount % 10 == 0) {
        fitnessCutoff -= 1;
      }
    }

    // Transfer generation over
    for (uint16_t i = 0; i < ants.size(); i++) {
      delete ants[i];
      ants[i] = new_generation[i];
    }
  }
}

uint16_t CWorld::getGenerationFitness() {
  uint16_t total = 0;

  for (uint16_t i = 0; i < ants.size(); i++) {
    total += ants[i]->fitness;
  }

  return total;
}

CAnt* CWorld::spawnOffspring(CAnt* parent_a, CAnt* parent_b, uint16_t gen) {

  SNeuralNetworkData* child_genome = new SNeuralNetworkData;
  SNeuralNetworkData* parent_a_genome = new SNeuralNetworkData;
  SNeuralNetworkData* parent_b_genome = new SNeuralNetworkData;

  parent_a->getGenome(parent_a_genome);
  parent_b->getGenome(parent_b_genome);

  // For simplicity, use a parent genome as scaffolding
  parent_a->getGenome(child_genome);

  // Not that we start in a random crossover configuration
  uint16_t genome_size = nn_network_size(parent_a_genome);
  bool crossover = nn_util_genrandom() <= 0.5f;
  uint16_t crossover_point = rand() % genome_size;

  for (uint16_t i = 0; i < genome_size; i++) {
    if (i == crossover_point) {
      crossover = !crossover; // Swap parent genome source
    }

    if (crossover) {
      child_genome->data[i] = parent_a_genome->data[i];
    } else {
      child_genome->data[i] = parent_b_genome->data[i];
    }

    // Mutate
    if (nn_util_genrandom() <= MUTATION_RATE) {
      child_genome->data[i] = nn_util_genrandom();
    }
  }

  // Birth!
  CAnt* offspring = new CAnt(this, getRandomPosition(), gen, last_ant_n);
  last_ant_n++;

  offspring->insertGenome(child_genome, parent_a->getID(), parent_b->getID());

  nn_network_delete_data(child_genome);
  nn_network_delete_data(parent_a_genome);
  nn_network_delete_data(parent_b_genome);

  return offspring;
}

CVector2 CWorld::getRandomPosition() {
  CVector2 pos;

  pos.x = ((float)rand() / RAND_MAX) * width;
  pos.y = ((float)rand() / RAND_MAX) * height;

  return pos;
}

CFood* CWorld::getNearestFood(CVector2 origin) {
  return (CFood *)food_tree->findNearest((uint16_t) origin.x, (uint16_t) origin.y);
}

void CWorld::consumeFood(CFood* food_item) {

  CVector2 position = food_item->getPosition();
  food_tree->removeObject(food_item, (uint16_t) position.x, (uint16_t) position.y);

  spawnFood();
}

void CWorld::getFitnessRange(uint16_t* range) {
  bool first_run = true;
  range[1] = 0;

  for (uint16_t i = 0; i < ants.size(); i++) {
    uint16_t fitness = ants[i]->fitness;

    if (first_run || fitness < range[0]) {
      range[0] = fitness;
    } else if (fitness > range[1]) {
      range[1] = fitness;
    }
  }
}

void CWorld::step() {
  wrapScreenEdges();
  simulateAnts();
  manageGeneration();
}

void stepAntRange(CWorld* world, uint16_t start, uint16_t end) {
  for (uint16_t i = start; i < end; i++) {
    world->ants[i]->step();
  }
}

// Splits simulation up among multiple threads
void CWorld::simulateAnts() {
  // TODO: Add threads
  stepAntRange(this, 0, ants.size());
}

void CWorld::runGrimReaper(std::vector<CAnt *> targets) {
  for (uint16_t i = 0; i < targets.size(); i++) {
    grimReaper(targets[i]);
  }
}

void CWorld::grimReaper(CAnt* ant) {
  ants.erase(std::remove(ants.begin(), ants.end(), ant), ants.end());
  delete ant;
}

void CWorld::manageGeneration() {
  next_generation_delay--;

  if (next_generation_delay == 0) {
    bool fit_ants_exist = false;
    
    // Prevent new generation before ants eat
    for (uint16_t i = 0; i < ants.size(); i++) {
      if (ants[i]->fitness > 0) {
        fit_ants_exist = true;
        break;
      }
    }

    if (fit_ants_exist) {
      generation += 1;
      advanceGeneration(generation);
    }

    resetGenerationDelay();
  }
}

void CWorld::resetGenerationDelay() {
  next_generation_delay = GENERATION_DELAY_LOOPS;
}

void CWorld::wrapScreenEdges() {
  for (uint16_t i = 0; i < ants.size(); i++) {
    CAnt* ant = ants[i];

    while (ant->getPosition().x > width) {
      ant->move((int)width * -1, 0);
    }

    while (ant->getPosition().x < 0) {
      ant->move(width, 0);
    }

    while (ant->getPosition().y > height) {
      ant->move(0, (int)height * -1);
    }

    while (ant->getPosition().y < 0) {
      ant->move(0, height);
    }
  }
}

/*
void CWorld::draw(sf::RenderWindow* window) {
  if(!has_graphics) {
    return;
  }

  for (uint16_t i = 0; i < food.size(); i++) {
    food[i]->draw(window);
  }

  for (uint16_t i = 0; i < ants.size(); i++) {
    ants[i]->draw(window);
  }

  ss.str(std::string());
  ss << "Generation: " << generation;
  generation_text.setString(ss.str());
  window->draw(generation_text);
}*/

uint16_t CWorld::getWidth() {
  return width;
}

uint16_t CWorld::getHeight() {
  return height;
}

CWorld::~CWorld() {
  for (uint16_t i = 0; i < food.size(); i++) {
    delete food[i];
  }

  for (uint16_t i = 0; i < ants.size(); i++) {
    delete ants[i];
  }

  food.clear();
  ants.clear();

  delete food_tree;
}
