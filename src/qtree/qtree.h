#ifndef QTREE_H
#define QTREE_H

#include <stdint.h>

class CEntity;
class CQTree {

public:
  CQTree(uint16_t _depth, uint8_t _limit, uint16_t _width, uint16_t _height, uint16_t _x, uint16_t _y);
  ~CQTree();

  void insertObject(CEntity* object, uint16_t o_x, uint16_t o_y);
  void removeObject(CEntity* object, uint16_t o_x, uint16_t o_y);
  CEntity* findNearest(uint16_t o_x, uint16_t o_y);

private:
  CQTree** cells;
  CEntity** items;
  uint8_t item_count;

  uint16_t depth;
  uint8_t limit;

  uint16_t width;
  uint16_t height;
  uint16_t hwidth;
  uint16_t hheight;
  uint16_t x;
  uint16_t y;

  bool subdivided;

  void insertObjectIntoSubCells(CEntity* object, uint16_t o_x, uint16_t o_y);
  void subdivide();
};

#endif
