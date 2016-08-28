#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include <vector>
#include <sstream>
#include "qtree/qtree.h"
#include "qtree/qtree_item.h"
#include "ant.h"

struct SNeuralNetworkData;
class CVector2;
class CWorld {

public:
  static uint16_t GENERATION_DELAY_LOOPS;
  static float MUTATION_RATE;
  static float CROSSOVER_RATE;

  CWorld(uint16_t _width, uint16_t _height, uint16_t _start_pop, uint16_t start_food);
  ~CWorld();

  void step();
  void consumeFood(SQTreeItem* food_item);
  void requestOffspring(SAnt* ant);

  uint16_t getWidth();
  uint16_t getHeight();

  uint16_t getGenerationFitness();
  SQTreeItem* getNearestFood(uint16_t x, uint16_t y);

  std::vector<SAnt *> ants;
  std::vector<SQTreeItem* > food;
  SQTree* food_tree; // Doesn't take ownership of food objects

private:
  uint16_t width, height;
  uint16_t start_pop;
  uint16_t last_ant_n;

  uint16_t next_generation_delay;
  uint16_t generation;

  SAnt* spawnOffspring(SAnt* parent_a, SAnt* parent_b, uint16_t gen);
  void resetGenerationDelay();
  void wrapScreenEdges();
  void grimReaper(SAnt* ant);
  void runGrimReaper(std::vector<SAnt *> targets);
  void advanceGeneration(uint16_t gen);
  void getFitnessRange(uint16_t* range);
  void simulateAnts();
  void manageGeneration();
  void spawnFood();
};

#endif
