#include <stdlib.h>
#include "qtree.h"
#include "../vec2.h"

SQTree* sqtree_create(
  uint16_t depth,
  uint8_t limit,
  uint16_t width,
  uint16_t height,
  uint16_t x,
  uint16_t y
) {
  SQTree* tree = (SQTree*)malloc(sizeof(SQTree));
  tree->depth = depth;
  tree->limit = limit;
  tree->width = width;
  tree->height = height;
  tree->hwidth = width / 2;
  tree->hheight = height / 2;
  tree->x = x;
  tree->y = y;

  tree->subdivided = false;
  tree->item_count = 0;
  tree->cells = NULL;
  tree->items = (SQTreeItem**)calloc(limit, sizeof(SQTreeItem*));

  return tree;
}

// Frees all associated tree items as well
void sqtree_delete(SQTree* tree) {
  if(tree->items != NULL) {
    for(uint16_t i = 0; i < tree->limit; i++) {
      if(tree->items[i] != NULL) {
        sqtree_item_delete(tree->items[i]);
      }
    }

    free(tree->items);
    tree->items = NULL;
  }

  if(tree->subdivided && tree->cells != NULL) {
    for(uint8_t i = 0; i < 4; i++) {
      if(tree->cells[i] != NULL) {
        sqtree_delete(tree->cells[i]);
      }
    }

    free(tree->cells);
    tree->cells = NULL;
  }

  free(tree);
  tree = NULL;
}

void sqtree_insert(SQTree* tree, SQTreeItem* item) {
  if(tree->item_count == tree->limit && !tree->subdivided) {
    sqtree_subdivide(tree);
  }

  // Insert object into the next free slot
  if(tree->item_count < tree->limit) {
    for(uint16_t i = 0; i < tree->limit; i++) {
      if(tree->items[i] == NULL) {
        tree->items[i] = item;
        break;
      }
    }
  } else {
    sqtree_insert_into_cells(tree, item);
  }

  tree->item_count++;
}

void sqtree_insert_into_cells(SQTree* tree, SQTreeItem* item) {
  if(item->pos[0] < tree->x + tree->hwidth) {
    if(item->pos[1] < tree->y + tree->hheight) {
      sqtree_insert(tree->cells[0], item);
    } else {
      sqtree_insert(tree->cells[1], item);
    }
  } else {
    if(item->pos[1] < tree->y + tree->hheight) {
      sqtree_insert(tree->cells[2], item);
    } else {
      sqtree_insert(tree->cells[3], item);
    }
  }
}

// Removes an item from a QTree; falls back on sqtree_remove_from_cells if
// subdivded. The item is not deleted.
bool sqtree_remove(SQTree* tree, SQTreeItem* item) {
  if(tree->item_count == 0) {
    return false;
  } else if(tree->subdivided) {
    return sqtree_remove_from_cells(tree, item);
  } else {
    for(uint16_t i = 0; i < tree->limit; i++) {
      if(tree->items[i] != NULL && tree->items[i] == item) {
        tree->items[i] = NULL;

        tree->item_count--;
        return true;
      }
    }
  }

  return false;
}

// Removes an item from a QTree's sub-cells (subdivided tree)
bool sqtree_remove_from_cells(SQTree* tree, SQTreeItem* item) {
  bool removed = false;

  if(tree->item_count == 0) {
    return false;
  } else if(item->pos[0] < tree->x + tree->hwidth) {
    if(item->pos[1] < tree->y + tree->hheight) {
      removed = sqtree_remove(tree->cells[0], item);
    } else {
      removed = sqtree_remove(tree->cells[1], item);
    }
  } else {
    if(item->pos[1] < tree->y + tree->hheight) {
      removed = sqtree_remove(tree->cells[2], item);
    } else {
      removed = sqtree_remove(tree->cells[3], item);
    }
  }

  if(removed) {
    tree->item_count--;
  }

  return removed;
}

SQTreeItem* sqtree_find_nearest(SQTree* tree, uint16_t* pos) {
  if(tree->subdivided) {
    return sqtree_find_nearest_in_cells(tree, pos);
  } else {
    if(tree->item_count == 0) {
      return NULL;
    }

    float distance, best_distance = -1;
    SQTreeItem* nearest = NULL;

    for(uint16_t i = 0; i < tree->item_count; i++) {
      if(tree->items[i] != NULL) {
        distance = vec2_distance(pos, tree->items[i]->pos);

        if(best_distance == -1 || distance < best_distance) {
          best_distance = distance;
          nearest = tree->items[i];
        }
      }
    }

    return nearest;
  }

  return NULL;
}

SQTreeItem* sqtree_find_nearest_in_cells(SQTree* tree, uint16_t* pos) {
  uint8_t cell_i;

  if(pos[0] < tree->x + tree->hwidth) {
    if(pos[1] < tree->y + tree->hheight) {
      cell_i = 0;
    } else {
      cell_i = 1;
    }
  } else {
    if(pos[1] < tree->y + tree->hheight) {
      cell_i = 2;
    } else {
      cell_i = 3;
    }
  }

  SQTreeItem* result = sqtree_find_nearest(tree->cells[cell_i], pos);

  // If we still haven't found anything, search remaining cells
  // This is NOT optimal!
  if(result == NULL) {
    for(uint8_t i = 0; i < 4; i++) {
      if(i == cell_i) {
        continue;
      }

      result = sqtree_find_nearest(tree->cells[i], pos);

      if(result != NULL) {
        break;
      }
    }
  }

  return result;
}

void sqtree_subdivide(SQTree* tree) {
  tree->cells = (SQTree**)malloc(sizeof(SQTree*)*4);
  tree->cells[0] = sqtree_create(
    tree->depth + 1,
    tree->limit,
    tree->hwidth,
    tree->hheight,
    tree->x,
    tree->y
  );

  tree->cells[1] = sqtree_create(
    tree->depth + 1,
    tree->limit,
    tree->hwidth,
    tree->hheight,
    tree->x,
    tree->y + tree->hheight
  );

  tree->cells[2] = sqtree_create(
    tree->depth + 1,
    tree->limit,
    tree->hwidth,
    tree->hheight,
    tree->x + tree->hwidth,
    tree->y
  );

  tree->cells[3] = sqtree_create(
    tree->depth + 1,
    tree->limit,
    tree->hwidth,
    tree->hheight,
    tree->x + tree->hwidth,
    tree->y + tree->hheight
  );

  // Move our own items down. We should only be called if our item_count is at
  // our limit! We don't check for performance reasons.
  for(uint16_t i = 0; i < tree->limit; i++) {
    if(tree->items[i] != NULL) {
      sqtree_insert_into_cells(tree, tree->items[i]);
      tree->items[i] = NULL;
    }
  }

  tree->subdivided = true;
}

