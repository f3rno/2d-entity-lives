#ifndef QTREE_ITEM_H
#define QTREE_ITEM_H

#include <stdint.h>

typedef struct SQTreeItem {
  void* data;
  uint16_t* pos;
} SQTreeItem;

SQTreeItem* sqtree_item_create(void* data, uint16_t x, uint16_t y);
void sqtree_item_delete(SQTreeItem* item);

#endif
