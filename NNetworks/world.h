#ifndef WORLD_H
#define WORLD_H

#include "types.h"
#include <vector>
#include <SFML\Graphics.hpp>
#include <sstream>

struct SNeuralNetworkData;
class CFood;
class CAnt;
class CVector2;
class CWorld {

public:
  static uint FATIGUE_DELAY_LOOPS;
  static uint FERTILITY_START;
  static uint FERTILITY_END;
  static float OFFSPRING_CHANCE;
  static float MUTATION_RATE;
  static float CROSSOVER_RATE;

  CWorld(uint _width, uint _height, uint _start_pop, uint start_food);
  ~CWorld();

  void step();
  void draw(sf::RenderWindow* window);
  void consumeFood(CFood* food_item);
  void requestOffspring(CAnt* ant);
  void setBreeding(bool _breeding);

  uint getWidth();
  uint getHeight();

  sf::Vector2f getRandomPosition();
  CFood* getNearestFood(sf::Vector2f origin);
  CFood* getNearestFood(CVector2 origin);
  sf::Font* getFont();
  bool breedingEnabled();

private:
  uint width, height;
  int fatigue_countdown;
  uint start_pop;

  sf::Font arial_font;
  sf::Text fatigues_text;
  std::ostringstream ss;
  uint fatigues;
  bool breeding;

  std::vector<CAnt *> ants;
  std::vector<CFood *> food;

  void resetFatigueDelay();
  void wrapScreenEdges();
  void grimReaper(CAnt* ant);
  void runGrimReaper(std::vector<CAnt *> targets);
  void produceOffspring(); // Uses parents in the queue
  void clearOffspringQueue();
  void seedPopulation(uint gen);
  void seedPopulation(SNeuralNetworkData* genome, uint gen);
  void getFitnessRange(uint* range);
  void simulateAnts();
  void updateFatigueDelay();

  CAnt* offspringQueue[2];
};

#endif