#ifndef ITEM_H
#define ITEM_H

#include "world.h"
#include "../core/math.h"

typedef enum {
  ITEM_EMPTY = 0,
  ITEM_DIRT,
  ITEM_GRASS,
  ITEM_SEED,
  ITEM_PICKAXE,
} item_e;

typedef struct {
  item_e item;
  int count;
} item_t;

typedef struct {
  item_e item;
} pickup_t;

typedef void (*use_cb)(world_t *world, item_t *inventory, int selected, vec2 worldPos);

use_cb getUseItem(item_e item);
unsigned int getItemTextureId(item_e item);

#endif
