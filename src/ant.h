#ifndef ANT_H
#define ANT_H

#include <stdint.h>
#include "entity.h"
#include <vector>
#include <sstream>

struct SNeuralNetwork;
struct SNeuralNetworkData;
class CWorld;
class CFood;
class CVector2;

class CAnt : public CEntity {

public:
  CAnt(CWorld* world, CVector2 start_pos, uint16_t _generation);
  ~CAnt();

  void step();

  uint16_t getGeneration();
  uint16_t fitness;
  void getGenome(SNeuralNetworkData* output);
  void insertGenome(SNeuralNetworkData* input);

private:
  CFood* target_food;
  SNeuralNetwork* brain;

  double* brain_outputs;
  double* brain_inputs;
  uint16_t generation;
};

#endif
