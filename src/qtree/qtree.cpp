#include "qtree.h"
#include "../vector2.h"
#include "../entity.h"
#include <cmath>
#include <iostream>

CQTree::CQTree(uint16_t _depth, uint8_t _limit, uint16_t _width, uint16_t _height, uint16_t _x, uint16_t _y) {

  depth = _depth;
  limit = _limit;
  width = _width;
  height = _height;
  hwidth = width / 2;
  hheight = height / 2;
  x = _x;
  y = _y;
  subdivided = false;

  items = new CEntity*[limit];
  item_count = 0;
}

void CQTree::insertObject(CEntity* object, uint16_t o_x, uint16_t o_y) {

  if(item_count == limit && !subdivided) {
    subdivide();
  }

  // Insert object into the next free slot
  if(item_count < limit) {
    items[item_count] = object;
  } else {
    insertObjectIntoSubCells(object, o_x, o_y);
  }

  item_count++;
}

void CQTree::subdivide() {
  if(subdivided) {
    return;
  }

  cells = new CQTree*[4];
  cells[0] = new CQTree(depth + 1, limit, hwidth, hheight, x, y);
  cells[1] = new CQTree(depth + 1, limit, hwidth, hheight, x, y + hheight);
  cells[2] = new CQTree(depth + 1, limit, hwidth, hheight, x + hwidth, y);
  cells[3] = new CQTree(depth + 1, limit, hwidth, hheight, x + hwidth, y + hheight);

  CVector2 position;

  // Move our own items down. We should only be called if our item_count is at
  // our limit! We don't check for performance reasons.
  for(int i = 0; i < item_count; i++) {
    position = items[i]->getPosition();
    insertObjectIntoSubCells(items[i], (uint16_t) position.x, (uint16_t) position.y);

    items[i] = 0;
  }

  subdivided = true;
}

void CQTree::insertObjectIntoSubCells(CEntity* object, uint16_t o_x, uint16_t o_y) {
  if(o_x < x + hwidth) {
    if(o_y < y + hheight) {
      cells[0]->insertObject(object, o_x, o_y);
    } else {
      cells[1]->insertObject(object, o_x, o_y);
    }
  } else {
    if(o_y < y + hheight) {
      cells[2]->insertObject(object, o_x, o_y);
    } else {
      cells[3]->insertObject(object, o_x, o_y);
    }
  }
}

void CQTree::removeObject(CEntity* object, uint16_t o_x, uint16_t o_y) {

  if(subdivided) {
    if(o_x < x + hwidth) {
      if(o_y < y + hheight) {
        cells[0]->removeObject(object, o_x, o_y);
      } else {
        cells[1]->removeObject(object, o_x, o_y);
      }
    } else {
      if(o_y < y + hheight) {
        cells[2]->removeObject(object, o_x, o_y);
      } else {
        cells[3]->removeObject(object, o_x, o_y);
      }
    }

  // Search within our own container
  } else {

    for(int i = 0; i < item_count; i++) {
      if(items[i] == object) {

        // If we have multiple objects, we overwrite the object pointer with the
        // last object in our list.
        if(item_count == 0) {
          items[0] = NULL;
        } else {
          items[i] = items[item_count - 1];
          items[item_count - 1] = NULL;
        }

        item_count--;
        break;
      }
    }

  }
}

CEntity* CQTree::findNearest(uint16_t o_x, uint16_t o_y) {

  if(subdivided) {

    CEntity* result = NULL;
    uint8_t searched;

    if(o_x < x + hwidth) {
      if(o_y < y + hheight) {
        searched = 0;
      } else {
        searched = 1;
      }
    } else {
      if(o_y < y + hheight) {
        searched = 2;
      } else {
        searched = 3;
      }
    }

    result = cells[searched]->findNearest(o_x, o_y);

    // If we still haven't found anything, search remaining cells
    // This is NOT optimal!
    if(result == NULL) {
      for(uint8_t i = 0; i < 4; i++) {
        if(i == searched) {
          continue;
        }

        result = cells[i]->findNearest(o_x, o_y);

        if(result != NULL) {
          break;
        }
      }
    }

    return result;

  } else {

    if (item_count == 0) {
      return NULL;

    // Search within our own container
    } else if (item_count < limit) {

      float best_distance = -1;
      CEntity* nearest = 0;

      for(int i = 0; i < item_count; i++) {
        CVector2 position = items[i]->getPosition();
        float distance = sqrtf(powf(position.x - o_x, 2) + powf(position.y - o_y, 2));

        if(best_distance == -1 || distance < best_distance) {
          best_distance = distance;
          nearest = items[i];
        }
      }

      return nearest;
    }
  }

  return NULL;
}

CQTree::~CQTree() {

  delete[] items;

  if(item_count > limit) {

    delete cells[0];
    delete cells[1];
    delete cells[2];
    delete cells[3];

    delete[] cells;
  }
}
