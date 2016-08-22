#include <stdlib.h>
#include "qtree_item.h"

SQTreeItem* sqtree_item_create(void* data, uint16_t x, uint16_t y) {
  SQTreeItem* item = (SQTreeItem*)malloc(sizeof(SQTreeItem));

  item->data = data;
  item->x = x;
  item->y = y;

  return item;
}

void sqtree_item_delete(SQTreeItem* item) {
  free(item);
}

