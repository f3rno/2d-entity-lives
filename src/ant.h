#ifndef ANT_H
#define ANT_H

#include <stdint.h>
#include "entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <sstream>

struct SNeuralNetwork;
struct SNeuralNetworkData;
class CWorld;
class CFood;

class CAnt : public CEntity {

public:
  CAnt(CWorld* world, sf::Vector2f start_pos, uint16_t _generation);
  ~CAnt();

  void step();
  void draw(sf::RenderWindow* window);

  uint16_t getGeneration();
  uint16_t fitness;
  void getGenome(SNeuralNetworkData* output);
  void insertGenome(SNeuralNetworkData* input);

private:
  CFood* target_food;
  SNeuralNetwork* brain;
  double* brain_outputs;
  double* brain_inputs;

  std::ostringstream ss;
  sf::Text status_str;
  uint16_t generation;
};

#endif
