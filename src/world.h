#ifndef WORLD_H
#define WORLD_H

#include "types.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include <sstream>

struct SNeuralNetworkData;
class CFood;
class CAnt;
class CVector2;
class CWorld {

public:
  static uint GENERATION_DELAY_LOOPS;
  static float MUTATION_RATE;
  static float CROSSOVER_RATE;

  CWorld(uint _width, uint _height, uint _start_pop, uint start_food);
  ~CWorld();

  void step();
  void draw(sf::RenderWindow* window);
  void consumeFood(CFood* food_item);
  void requestOffspring(CAnt* ant);

  uint getWidth();
  uint getHeight();

  uint getGenerationFitness();
  sf::Vector2f getRandomPosition();
  CFood* getNearestFood(sf::Vector2f origin);
  CFood* getNearestFood(CVector2 origin);
  sf::Font* getFont();

  std::vector<CAnt *> ants;
  std::vector<CFood *> food;

private:
  uint width, height;
  int fatigue_countdown;
  uint start_pop;

  sf::Font arial_font;
  sf::Text generation_text;
  std::ostringstream ss;
  uint next_generation_delay;
  uint generation;

  CAnt* spawnOffspring(CAnt* parent_a, CAnt* parent_b, uint gen);
  void resetGenerationDelay();
  void wrapScreenEdges();
  void grimReaper(CAnt* ant);
  void runGrimReaper(std::vector<CAnt *> targets);
  void advanceGeneration(uint gen);
  void getFitnessRange(uint* range);
  void simulateAnts();
  void manageGeneration();
};

#endif
