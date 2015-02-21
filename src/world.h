#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include <sstream>

struct SNeuralNetworkData;
class CFood;
class CAnt;
class CVector2;
class CWorld {

public:
  static uint16_t GENERATION_DELAY_LOOPS;
  static float MUTATION_RATE;
  static float CROSSOVER_RATE;

  CWorld(uint16_t _width, uint16_t _height, uint16_t _start_pop, uint16_t start_food, bool graphics);
  ~CWorld();

  void step();
  void draw(sf::RenderWindow* window);
  void consumeFood(CFood* food_item);
  void requestOffspring(CAnt* ant);

  uint16_t getWidth();
  uint16_t getHeight();

  uint16_t getGenerationFitness();
  sf::Vector2f getRandomPosition();
  CFood* getNearestFood(sf::Vector2f origin);
  CFood* getNearestFood(CVector2 origin);
  sf::Font* getFont();

  std::vector<CAnt *> ants;
  std::vector<CFood *> food;

private:
  bool has_graphics;

  uint16_t width, height;
  int fatigue_countdown;
  uint16_t start_pop;

  sf::Font arial_font;
  sf::Text generation_text;
  std::ostringstream ss;
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
};

#endif
