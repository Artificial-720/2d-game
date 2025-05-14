#include "player.h"

#include <assert.h>


static tile_e getTileFromItem(item_e item) {
  switch (item) {
    case ITEM_DIRT:
      return TILE_DIRT;
    case ITEM_GRASS:
      return TILE_GRASS;
    case ITEM_SEED:
      return TILE_SEED;
    default:
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



use_cb getUseItem(item_e item) {
  switch (item) {
    case ITEM_DIRT:
    case ITEM_GRASS:
    case ITEM_SEED:
      return placeBlock;
    case ITEM_PICKAXE:
      return breakBlockForeground;
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
