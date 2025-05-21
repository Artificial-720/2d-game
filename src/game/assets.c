#include "assets.h"

#include <stdio.h>

#define SPRITE_DIRT "assets/tiles/dirt.png"
#define SPRITE_GRASS "assets/tiles/dirt_grass.png"
#define SPRITE_SEED "assets/tiles/grass1.png"
#define SPRITE_WOOD "assets/tiles/trunk_side.png"
#define SPRITE_LEAVES "assets/tiles/leaves.png"

unsigned int getTileTextureId(tile_e type) {
  switch (type) {
    case TILE_DIRT:
      return loadTexture(SPRITE_DIRT).id;
    case TILE_GRASS:
      return loadTexture(SPRITE_GRASS).id;
    case TILE_SEED:
      return loadTexture(SPRITE_SEED).id;
    case TILE_WOOD:
      return loadTexture(SPRITE_WOOD).id;
    case TILE_LEAVES:
      return loadTexture(SPRITE_LEAVES).id;
    case TILE_DOOR:
      return loadTexture("assets/greysand.png").id;
    case TILE_STONE:
      return loadTexture("assets/tiles/stone.png").id;
    case TILE_IRON:
      return loadTexture("assets/tiles/stone_browniron.png").id;
    default:
      printf("warning: missing tile texture for %d\n", type);
      return loadTexture(MISSING_TEXTURE).id;
  }
}

int isBackgroundTile(tile_e type) {
  switch (type) {
    case TILE_SEED:
    case TILE_WOOD:
    case TILE_LEAVES:
      return 1;
    default:
      return 0;
  }
  return 0;
}

item_e tileDrop(tile_e tile) {
  switch (tile) {
    case TILE_DIRT:
    case TILE_GRASS:
      return ITEM_DIRT;
    case TILE_WOOD:
      return ITEM_WOOD;
    default:
      printf("warning: missing tile drop for tile: %d\n", tile);
      return ITEM_EMPTY;
  }
}

unsigned int getItemTextureId(item_e item) {
  switch (item) {
    case ITEM_PICKAXE:
      return loadTexture("assets/Items/pick_iron.png").id;
    case ITEM_AXE:
      return loadTexture("assets/Items/axe_iron.png").id;
    case ITEM_DIRT:
      return loadTexture("assets/tiles/dirt.png").id;
    case ITEM_GRASS:
      return loadTexture("assets/tiles/dirt_grass.png").id;
    case ITEM_SEED:
      return loadTexture("assets/Items/seed.png").id;
    case ITEM_WOOD:
      return loadTexture("assets/tiles/wood.png").id;
    default:
      // printf("warning: missing item texture for item: %d\n", item);
      return loadTexture(MISSING_TEXTURE).id;
  }
}

unsigned int getNumberTextureId(int n) {
  switch (n) {
    case 0:
      return loadTexture("assets/numbers/Number0 7x10.png").id;
    case 1:
      return loadTexture("assets/numbers/Number1 7x10.png").id;
    case 2:
      return loadTexture("assets/numbers/Number2 7x10.png").id;
    case 3:
      return loadTexture("assets/numbers/Number3 7x10.png").id;
    case 4:
      return loadTexture("assets/numbers/Number4 7x10.png").id;
    case 5:
      return loadTexture("assets/numbers/Number5 7x10.png").id;
    case 6:
      return loadTexture("assets/numbers/Number6 7x10.png").id;
    case 7:
      return loadTexture("assets/numbers/Number7 7x10.png").id;
    case 8:
      return loadTexture("assets/numbers/Number8 7x10.png").id;
    case 9:
      return loadTexture("assets/numbers/Number9 7x10.png").id;
  }
  return 0;
}
