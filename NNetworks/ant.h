#ifndef ANT_H
#define ANT_H

#include "types.h"
#include "entity.h"
#include <SFML\Graphics.hpp>
#include <vector>
#include <sstream>

struct SNeuralNetwork;
struct SNeuralNetworkData;
class CWorld;
class CFood;

enum CAntFertility {
  NEWBORN,
  FERTILE,
  SENIOR
};

class CAnt : public CEntity {

public:
  CAnt(CWorld* world, sf::Vector2f start_pos, uint _generation);
  ~CAnt();

  void step();
  void draw(sf::RenderWindow* window);
  void hungerTick();
  bool needsToDie();
  void advanceFertility();
  CAntFertility getFertility();
  bool isFertile();
  uint getAge();
  uint getFitness();
  void getGenome(SNeuralNetworkData* output);
  void insertGenome(SNeuralNetworkData* input);
  uint getGeneration();

private:
  void manageFertility();

  CFood* target_food;
  SNeuralNetwork* brain;
  double* brain_outputs;
  double* brain_inputs;

  std::ostringstream ss;
  sf::Text status_str;
  uint generation;
  uint health;
  uint fitness;
  uint age;
  bool death_mark;
  CAntFertility fertility;
};

#endif