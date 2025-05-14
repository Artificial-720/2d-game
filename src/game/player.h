#ifndef PLAYER_H
#define PLAYER_H

#include "ecs.h"
#include "item.h"

#define INVENTORY_SIZE 40

typedef struct {
  entity_t entity;
  float pickupDis;
  item_t inventory[INVENTORY_SIZE];
  int selected;
} player_t;

#endif
