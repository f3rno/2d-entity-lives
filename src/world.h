#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include <vector>
#include <sstream>

struct SNeuralNetworkData;
class CFood;
class CAnt;
class CVector2;
class CQTree;
class CWorld {

public:
  static uint16_t GENERATION_DELAY_LOOPS;
  static float MUTATION_RATE;
  static float CROSSOVER_RATE;

  CWorld(uint16_t _width, uint16_t _height, uint16_t _start_pop, uint16_t start_food);
  ~CWorld();

  void step();
  void consumeFood(CFood* food_item);
  void requestOffspring(CAnt* ant);

  uint16_t getWidth();
  uint16_t getHeight();

  uint16_t getGenerationFitness();
  CVector2 getRandomPosition();
  CFood* getNearestFood(CVector2 origin);

  std::vector<CAnt *> ants;
  std::vector<CFood* > food;
  CQTree* food_tree; // Doesn't take ownership of food objects

private:
  uint16_t width, height;
  int fatigue_countdown;
  uint16_t start_pop;
  uint16_t last_ant_n;

  uint16_t next_generation_delay;
  uint16_t generation;

  CAnt* spawnOffspring(CAnt* parent_a, CAnt* parent_b, uint16_t gen);
  void resetGenerationDelay();
  void wrapScreenEdges();
  void grimReaper(CAnt* ant);
  void runGrimReaper(std::vector<CAnt *> targets);
  void advanceGeneration(uint16_t gen);
  void getFitnessRange(uint16_t* range);
  void simulateAnts();
  void manageGeneration();
  void spawnFood();
};

#endif
