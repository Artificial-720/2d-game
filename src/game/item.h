#ifndef ITEM_H
#define ITEM_H

#include "world.h"

typedef enum {
  ITEM_EMPTY = 0,
  ITEM_DIRT,
  ITEM_GRASS,
  ITEM_SEED,
  ITEM_WOOD,
  ITEM_PICKAXE,
  ITEM_AXE,
} item_e;

typedef struct {
  item_e item;
} pickup_t;

unsigned int getItemTextureId(item_e item);

#endif
