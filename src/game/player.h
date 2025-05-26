#ifndef PLAYER_H
#define PLAYER_H

#include "ecs.h"
#include "item.h"
#include "../core/math.h"

#define INVENTORY_SIZE 40

typedef struct {
  item_e item;
  int count;
} slot_t;

typedef enum {
  PLAYER_IDLE,
  PLAYER_WALKING,
  PLAYER_JUMPING,
  PLAYER_FALLING,
  PLAYER_HURT,
  PLAYER_USE,
  PLAYER_ATTACKING,
  PLAYER_DEAD
} playerState_e;

typedef struct {
  entity_t entity;
  float pickupDis;
  slot_t inventory[INVENTORY_SIZE];
  int selected;
  int facingLeft;
  playerState_e state;
  double useTimer;
} player_t;

typedef void (*use_cb)(world_t *world, player_t *player, vec2 usePos);
use_cb getUseItem(item_e item);

int giveItemToPlayer(player_t *player, item_e item, int count);

#endif
