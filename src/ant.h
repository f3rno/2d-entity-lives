#ifndef ANT_H
#define ANT_H

#include <stdint.h>
#include <vector>
#include <sstream>
#include "entity.h"

struct SNeuralNetwork;
struct SNeuralNetworkData;
class CWorld;
class CVector2;

class CAnt : public CEntity {

public:
  CAnt(CWorld* world, CVector2 start_pos, uint16_t _generation, uint16_t _id);
  ~CAnt();

  void step();

  uint16_t getGeneration();
  uint16_t fitness;
  void getGenome(SNeuralNetworkData* output);
  void insertGenome(SNeuralNetworkData* input, uint16_t _parent_1_id, uint16_t _parent_2_id);

  uint16_t getID();
  uint16_t getParent1ID();
  uint16_t getParent2ID();

private:
  SNeuralNetwork* brain;

  double* brain_outputs;
  double* brain_inputs;
  uint16_t generation;

  uint16_t id;
  uint16_t parent_1_id;
  uint16_t parent_2_id;
};

#endif
