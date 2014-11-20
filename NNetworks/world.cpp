#include "world.h"
#include "food.h"
#include "ant.h"
#include "neural_network.h"
#include "utility.h"
#include "vector2.h"
#include <random>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <boost\thread.hpp>

uint CWorld::GENERATION_DELAY_LOOPS = 10000;
float CWorld::MUTATION_RATE = 0.02f;

CWorld::CWorld(uint _width, uint _height, uint _start_pop, uint start_food) {
  width = _width;
  height = _height;
  start_pop = _start_pop;
  resetGenerationDelay();

  for (uint i = 0; i < start_food; i++) {
    food.push_back(new CFood(this, getRandomPosition()));
  }

  advanceGeneration(1);

  if (!arial_font.loadFromFile("./arial.ttf")) {
    std::cout << "Arial font failed to load!\n";
  }

  generation_text.setFont(arial_font);
  generation_text.setPosition(0, 0);
  generation_text.setCharacterSize(16);
  generation_text.setColor(sf::Color::Green);
}

void CWorld::advanceGeneration(uint gen) {
  generation = gen;

  // First generation is entirely random
  if (gen == 1) {
    for (uint i = 0; i < start_pop; i++) {
      ants.push_back(new CAnt(this, getRandomPosition(), gen));
    }
  } else {

    // Note that we randomly select couples!
    CAnt* parents[2];
    parents[0] = NULL;
    parents[1] = NULL;

    // Generate a new population of the same size
    uint prev_gen_size = ants.size();
    std::vector<CAnt* > new_generation;

    float totalFitness = 0;
    float minFitness = 0; // We don't pay attention to mentally challenged ants
    float maxFitness = 0;

    for (uint i = 0; i < ants.size(); i++) {
      if (ants[i]->getFitness() > maxFitness) {
        maxFitness = ants[i]->getFitness();
      }

      if (ants[i]->getFitness() > 0 && (ants[i]->getFitness() < minFitness || minFitness == 0)) {
        minFitness = ants[i]->getFitness();
      }

      totalFitness += ants[i]->getFitness();
    }

    float fitnessCutoff = (1.0f - (minFitness / maxFitness)) * 0.9f;
    float fitnessCutoffReduction = 0.9f;
    uint loopCount = 0; // We decrease the cutoff as we run out of options

    while (new_generation.size() < prev_gen_size) {
      for (uint i = 0; i < ants.size(); i++) {
        float ant_fitness_score = ants[i]->getFitness() / maxFitness;

        // Eugenics FTW
        if (ant_fitness_score > fitnessCutoff) {
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

      if (loopCount % 60 == 0) {
        fitnessCutoff *= fitnessCutoffReduction;
      }
    }

    // Transfer generation over
    for (uint i = 0; i < ants.size(); i++) {
      delete ants[i];
      ants[i] = new_generation[i];
    }

    std::cout << "Spawned generation " << gen << " [prev F" << totalFitness << "]" << std::endl;
  }
}

uint CWorld::getGenerationFitness() {
  uint total = 0;

  for (uint i = 0; i < ants.size(); i++) {
    total += ants[i]->getFitness();
  }

  return total;
}

CAnt* CWorld::spawnOffspring(CAnt* parent_a, CAnt* parent_b, uint gen) {

  SNeuralNetworkData* child_genome = new SNeuralNetworkData;
  SNeuralNetworkData* parent_a_genome = new SNeuralNetworkData;
  SNeuralNetworkData* parent_b_genome = new SNeuralNetworkData;

  parent_a->getGenome(parent_a_genome);
  parent_b->getGenome(parent_b_genome);

  // For simplicity, use a parent genome as scaffolding
  parent_a->getGenome(child_genome);

  // Not that we start in a random crossover configuration
  uint genome_size = nn_network_size(parent_a_genome);
  bool crossover = nn_util_genrandom() <= 0.5f;
  uint crossover_point = rand() % genome_size;

  for (uint i = 0; i < genome_size; i++) {
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
  CAnt* offspring = new CAnt(this, getRandomPosition(), gen);
  offspring->insertGenome(child_genome);

  nn_network_delete_data(child_genome);
  nn_network_delete_data(parent_a_genome);
  nn_network_delete_data(parent_b_genome);

  return offspring;
}

sf::Vector2f CWorld::getRandomPosition() {
  sf::Vector2f pos;

  pos.x = ((float)rand() / RAND_MAX) * width;
  pos.y = ((float)rand() / RAND_MAX) * height;

  return pos;
}

CFood* CWorld::getNearestFood(sf::Vector2f origin) {
  return getNearestFood(CVector2(origin.x, origin.y));
}

CFood* CWorld::getNearestFood(CVector2 origin) {
  if (food.size() == 0) { return NULL; }

  CFood* nearest = NULL;
  float best_distance = -1;

  for (uint i = 0; i < food.size(); i++) {
    float food_distance = origin.distance(food[i]->getPosition());

    if (best_distance == -1 || food_distance < best_distance) {
      best_distance = food_distance;
      nearest = food[i];
    }
  }

  return nearest;
}

void CWorld::consumeFood(CFood* food_item) {
  food.erase(std::remove(food.begin(), food.end(), food_item), food.end());
  food.push_back(new CFood(this, getRandomPosition()));
}

sf::Font* CWorld::getFont() {
  return &arial_font;
}

void CWorld::getFitnessRange(uint* range) {
  bool first_run = true;
  range[1] = 0;

  for (uint i = 0; i < ants.size(); i++) {
    uint fitness = ants[i]->getFitness();

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

void stepAntRange(CWorld* world, uint start, uint end) {
  for (uint i = start; i < end; i++) {
    world->ants[i]->step();
  }
}

// Splits simulation up among multiple threads
void CWorld::simulateAnts() {
  // TODO: Add threads
  stepAntRange(this, 0, ants.size());
}

void CWorld::runGrimReaper(std::vector<CAnt *> targets) {
  for (uint i = 0; i < targets.size(); i++) {
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
    for (uint i = 0; i < ants.size(); i++) {
      if (ants[i]->getFitness() > 0) {
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
  for (uint i = 0; i < ants.size(); i++) {
    sf::Shape* body = ants[i]->getBody();

    while (body->getPosition().x > width) {
      body->move((int)width * -1, 0);
    }

    while (body->getPosition().x < 0) {
      body->move(width, 0);
    }

    while (body->getPosition().y > height) {
      body->move(0, (int)height * -1);
    }

    while (body->getPosition().y < 0) {
      body->move(0, height);
    }
  }
}

void CWorld::draw(sf::RenderWindow* window) {
  for (uint i = 0; i < food.size(); i++) {
    food[i]->draw(window);
  }

  for (uint i = 0; i < ants.size(); i++) {
    ants[i]->draw(window);
  }

  ss.str(std::string());
  ss << "Generation: " << generation;
  generation_text.setString(ss.str());
  window->draw(generation_text);
}

uint CWorld::getWidth() {
  return width;
}

uint CWorld::getHeight() {
  return height;
}

CWorld::~CWorld() {
  for (uint i = 0; i < food.size(); i++) {
    delete food[i];
  }

  for (uint i = 0; i < ants.size(); i++) {
    delete ants[i];
  }

  food.clear();
  ants.clear();
}