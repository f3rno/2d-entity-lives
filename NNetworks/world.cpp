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

uint CWorld::FATIGUE_DELAY_LOOPS = 1000;
uint CWorld::FERTILITY_START = 250 * CWorld::FATIGUE_DELAY_LOOPS;
uint CWorld::FERTILITY_END = 600 * CWorld::FATIGUE_DELAY_LOOPS;
float CWorld::OFFSPRING_CHANCE = 2.0f / (CWorld::FERTILITY_END - CWorld::FERTILITY_START);
float CWorld::MUTATION_RATE = 0.1f;
float CWorld::CROSSOVER_RATE = 2.0f;

CWorld::CWorld(uint _width, uint _height, uint _start_pop, uint start_food) {
  width = _width;
  height = _height;
  start_pop = _start_pop;
  resetFatigueDelay();
  clearOffspringQueue();
  fatigues = 0;
  breeding = true;

  for (uint i = 0; i < start_food; i++) {
    food.push_back(new CFood(this, getRandomPosition()));
  }

  seedPopulation(1);

  if (!arial_font.loadFromFile("./arial.ttf")) {
    std::cout << "Arial font failed to load!\n";
  }

  fatigues_text.setFont(arial_font);
  fatigues_text.setPosition(0, 0);
  fatigues_text.setCharacterSize(16);
  fatigues_text.setColor(sf::Color::Green);
}

void CWorld::seedPopulation(uint gen) {
  for (uint i = 0; i < start_pop; i++) {
    ants.push_back(new CAnt(this, getRandomPosition(), gen));
  }
}

void CWorld::seedPopulation(SNeuralNetworkData* genome, uint gen) {
  seedPopulation(gen);

  for (uint i = 0; i < ants.size(); i++) {
    ants[i]->insertGenome(genome);
  }
}

void CWorld::produceOffspring() {
  CAnt* a = offspringQueue[0];
  CAnt* b = offspringQueue[1];
  assert(a->isFertile() && b->isFertile());

  SNeuralNetworkData* genome_child_1 = new SNeuralNetworkData;
  SNeuralNetworkData* genome_child_2 = new SNeuralNetworkData;

  a->getGenome(genome_child_1);
  b->getGenome(genome_child_2);

  uint genome_size = nn_network_size(genome_child_1);

  double temp;
  bool crossover = false;
  float crossover_chance = CROSSOVER_RATE / genome_size;

  for (uint i = 0; i < genome_size; i++) {
    if (crossover) {

      temp = genome_child_1->data[i];
      genome_child_1->data[i] = genome_child_2->data[i];
      genome_child_2->data[i] = temp;

    } else if (nn_util_genrandom() <= crossover_chance) {

      // Crossover at some point randomly along the genome
      crossover = true;
    }

    // Mutate
    if (nn_util_genrandom() <= MUTATION_RATE) {
      genome_child_1->data[i] = nn_util_genrandom();
    }

    // Mutate
    if (nn_util_genrandom() <= MUTATION_RATE) {
      genome_child_2->data[i] = nn_util_genrandom();
    }
  }

  // Birth!
  uint generation = std::max(a->getGeneration(), b->getGeneration()) + 1;
  CAnt* offspring_1 = new CAnt(this, getRandomPosition(), generation);
  CAnt* offspring_2 = new CAnt(this, getRandomPosition(), generation);

  offspring_1->insertGenome(genome_child_1);
  offspring_2->insertGenome(genome_child_2);

  nn_network_delete_data(genome_child_1);
  nn_network_delete_data(genome_child_2);

  ants.push_back(offspring_1);
  ants.push_back(offspring_2);

  std::cout << "2 new ants [" << generation << "]" << std::endl;

  a->advanceFertility();
  b->advanceFertility();
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

void CWorld::resetFatigueDelay() {
  fatigue_countdown = CWorld::FATIGUE_DELAY_LOOPS;
  fatigues++;
}

bool CWorld::breedingEnabled() {
  return breeding;
}

void CWorld::setBreeding(bool _breeding) {
  breeding = _breeding;
}

void CWorld::clearOffspringQueue() {
  offspringQueue[0] = NULL;
  offspringQueue[1] = NULL;
}

void CWorld::requestOffspring(CAnt* ant) {
  if (!breeding || !ant->isFertile()) { return; }

  uint fitnessRange[2];
  getFitnessRange(fitnessRange);
  
  // Favour fitter ants
  float fitness_score = (float) std::max(1, (int)ant->getFitness()) / (float) std::max(1, (int)fitnessRange[1]);
  bool has_chance = ((float)rand() / RAND_MAX) <= OFFSPRING_CHANCE;
  bool fit = ((float)rand() / RAND_MAX) <= fitness_score;

  if (!has_chance || !fit) {
    return;
  }

  // Free up first queue slot if the any has become infertile
  if (offspringQueue[0] != NULL) {
    if (!offspringQueue[0]->isFertile()) { offspringQueue[0] = NULL; }
   }

  // We have to queue up two parents
  if (offspringQueue[0] == NULL) {
    offspringQueue[0] = ant;
  } else if (offspringQueue[1] == NULL) {
    offspringQueue[1] = ant;
    produceOffspring();
    clearOffspringQueue();
  }
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
  updateFatigueDelay();
  simulateAnts();
}

void CWorld::updateFatigueDelay() {
  fatigue_countdown--;

  if (fatigue_countdown < 0) {
    resetFatigueDelay();
  }
}

void CWorld::simulateAnts() {
  std::vector<CAnt *> marked_for_death;

  for (int i = ants.size() - 1; i >= 0; i--) {

    // Apply hunger before stepping
    if (fatigue_countdown == 0) {
      ants[i]->hungerTick();
    }

    ants[i]->step();

    if (ants[i]->needsToDie()) {
      marked_for_death.push_back(ants[i]);
    }
  }

  if (marked_for_death.size() > 0) {
    runGrimReaper(marked_for_death);
  }
}

void CWorld::runGrimReaper(std::vector<CAnt *> targets) {
  for (uint i = 0; i < targets.size(); i++) {
    grimReaper(targets[i]);
  }
}

void CWorld::grimReaper(CAnt* ant) {
  if (offspringQueue[0] == ant) { offspringQueue[0] = NULL; }

  // If last ant, reseed
  if (ants.size() == 1) {
    uint gen = ants.back()->getGeneration() + 1;
    std::cout << "Re-seeding population, gen [" << gen << "]" << std::endl;

    SNeuralNetworkData* prodigy_genome = new SNeuralNetworkData;
    ants.back()->getGenome(prodigy_genome);

    // Seed with the prodigy
    // seedPopulation(prodigy_genome, gen);
    seedPopulation(1); // Randomize
  }

  ants.erase(std::remove(ants.begin(), ants.end(), ant), ants.end());
  delete ant;
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
  ss << "Fatigues: " << fatigues;
  fatigues_text.setString(ss.str());
  window->draw(fatigues_text);
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