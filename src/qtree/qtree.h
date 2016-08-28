#ifndef QTREE_H
#define QTREE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "qtree_item.h"

typedef struct SQTree {
  struct SQTree** cells;
  SQTreeItem** items;

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
} SQTree;

SQTree* sqtree_create(
  uint16_t depth,
  uint8_t limit,
  uint16_t width,
  uint16_t height,
  uint16_t x,
  uint16_t y
);

void sqtree_delete(SQTree* tree);
void sqtree_insert(SQTree* tree, SQTreeItem* item);
bool sqtree_remove(SQTree* tree, SQTreeItem* item);
bool sqtree_remove_from_cells(SQTree* tree, SQTreeItem* item);
SQTreeItem* sqtree_find_nearest(SQTree* tree, uint16_t* pos);
SQTreeItem* sqtree_find_nearest_in_cells(SQTree* tree, uint16_t* pos);

void sqtree_subdivide(SQTree* tree);
void sqtree_insert_into_cells(SQTree* tree, SQTreeItem* item);

#endif
