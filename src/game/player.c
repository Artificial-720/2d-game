#include "player.h"
#include "ecs.h"
#include "item.h"
#include "world.h"
#include "components.h"
#include "physics.h"

#include <assert.h>
#include <stdio.h>


static tile_e getTileFromItem(item_e item) {
  switch (item) {
    case ITEM_DIRT:
      return TILE_DIRT;
    case ITEM_GRASS:
      return TILE_GRASS;
    case ITEM_SEED:
      return TILE_SEED;
    case ITEM_DOOR:
      return TILE_DOOR;
    default:
      printf("warning: missing get tile from item\n");
      return 0;
  }
}

void placeBlock(world_t *world, player_t *player, vec2 usePos) {
  slot_t *inventory = player->inventory;

  if (inventory[player->selected].count > 0) {
    tile_e tile = getTileFromItem(inventory[player->selected].item);
    if (worldPlaceTile(world, usePos.x, usePos.y, tile)) {
      inventory[player->selected].count--;
      if (inventory[player->selected].count == 0) {
        inventory[player->selected].item = ITEM_EMPTY;
      }
    }
  }
}

void breakBlockForeground(world_t *world, player_t *player, vec2 usePos) {
  tile_e broken = TILE_EMPTY;
  worldBreakTile(world, usePos.x, usePos.y, &broken);
  (void)player;
}

void breakBlockBackground(world_t *world, player_t *player, vec2 usePos) {
  worldBreakTileBackground(world, usePos.x, usePos.y);
  (void)player;
}

void swordHitCallback(entity_t trigger, entity_t other) {
  physics_t *physics = (physics_t*)ecsGetComponent(trigger, PHYSICS);
  if (physics->owner != other) {
    printf("callback called with %d for other\n", other);
    health_t *health = (health_t*)ecsGetComponent(other, HEALTH);
    int temp = health->value;
    health->value -= 25;
    printf("damaged %d health %d -> %d\n", other, temp, health->value);
  }
}

void swordAttack(world_t *world, player_t *player, vec2 usePos) {
  if (player->state != PLAYER_IDLE) return;
  combat_t *combat = (combat_t*)ecsGetComponent(player->entity, COMBAT);

  if (!combat) return;
  if (combat->cooldown > 0) return;

  printf("attacking\n");
  player->state = PLAYER_ATTACKING;
  combat->cooldown += combat->attackTime;


  // spawn sword hit box
  transform_t *tf = (transform_t*)ecsGetComponent(player->entity, TRANSFORM);

  vec2 hitboxOffset = {0};
  hitboxOffset.x = player->facingLeft ? -2.0f : 2.0f;
  vec2 hitboxPos = vec2Add(tf->pos, hitboxOffset);
  vec2 hitboxSize = {2.0f, 2.0f};

  entity_t sword = ecsCreateEntity();

  transform_t transform = {0};
  transform.pos = hitboxPos;
  ecsAddComponent(sword, TRANSFORM, (void*)&transform);

  physics_t p = {.body = 0, .isStatic = 0, .isTrigger = 1, .callback = swordHitCallback, .owner = player->entity};
  p.body = createTrigger(hitboxPos, hitboxSize);
  ecsAddComponent(sword, PHYSICS, (void*)&p);

  lifetime_t lifetime = {.remaining = 0.5f};
  ecsAddComponent(sword, LIFETIME, (void*)&lifetime);


  (void)world;
  (void)usePos;
}



use_cb getUseItem(item_e item) {
  switch (item) {
    case ITEM_DIRT:
    case ITEM_GRASS:
    case ITEM_SEED:
    case ITEM_DOOR:
      return placeBlock;
    case ITEM_PICKAXE:
      return breakBlockForeground;
    case ITEM_AXE:
      return breakBlockBackground;
    case ITEM_SWORD:
      return swordAttack;
    default:
      return 0;
  }
}

int giveItemToPlayer(player_t *player, item_e item, int count) {
  assert(item != ITEM_EMPTY);
  assert(count > 0);
  assert(player);

  // find a slot for the item
  // check if already have the item
  for (int i = 0; i < INVENTORY_SIZE; i++) {
    if (player->inventory[i].item == item) {
      player->inventory[i].count += count;
      return 1;
    }
  }

  // don't have the item find a empty slot
  for (int i = 0; i < INVENTORY_SIZE; i++) {
    if (player->inventory[i].item == ITEM_EMPTY) {
      player->inventory[i].item = item;
      player->inventory[i].count += count;
      return 1;
    }
  }

  return 0;
}
