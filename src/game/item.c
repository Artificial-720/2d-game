#include "item.h"

#include "texture.h"
#include "world.h"
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


void placeBlock(world_t *world, item_t *inventory, int selected, vec2 worldPos) {
  if (inventory[selected].count > 0) {
    tile_e tile = getTileFromItem(inventory[selected].item);
    if (worldPlaceTile(world, worldPos.x, worldPos.y, tile)) {
      inventory[selected].count--;
      if (inventory[selected].count == 0) {
        inventory[selected].item = ITEM_EMPTY;
      }
    }
  }
}

void breakBlockForeground(world_t *world, item_t *inventory, int selected, vec2 worldPos) {
  (void)inventory;
  (void)selected;
  tile_e broken = TILE_EMPTY;
  worldBreakTile(world, worldPos.x, worldPos.y, &broken);
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


unsigned int getItemTextureId(item_e item) {
  switch (item) {
    case ITEM_PICKAXE:
      return getTexture("assets/Items/pick_iron.png");
    case ITEM_DIRT:
      return getTexture("assets/tiles/dirt.png");
    case ITEM_GRASS:
      return getTexture("assets/tiles/dirt_grass.png");
    case ITEM_SEED:
      return getTexture("assets/Items/seed.png");
    default:
      assert(0 && "Missing item texture");
  }
}
