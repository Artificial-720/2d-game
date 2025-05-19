#include "assets.h"

#define SPRITE_DIRT "assets/tiles/dirt.png"
#define SPRITE_GRASS "assets/tiles/dirt_grass.png"
#define SPRITE_SEED "assets/tiles/grass1.png"
#define SPRITE_WOOD "assets/tiles/trunk_side.png"
#define SPRITE_LEAVES "assets/tiles/leaves.png"

unsigned int getTileTextureId(tile_e type) {
  switch (type) {
    case TILE_DIRT:
      return getTexture(SPRITE_DIRT);
    case TILE_GRASS:
      return getTexture(SPRITE_GRASS);
    case TILE_SEED:
      return getTexture(SPRITE_SEED);
    case TILE_WOOD:
      return getTexture(SPRITE_WOOD);
    case TILE_LEAVES:
      return getTexture(SPRITE_LEAVES);
    case TILE_DOOR:
      return getTexture("assets/greysand.png");
    default:
      return getTexture(MISSING_TEXTURE);
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
      return ITEM_DIRT;
    case TILE_GRASS:
      return ITEM_DIRT;
    case TILE_WOOD:
      return ITEM_WOOD;
    default:
      return ITEM_EMPTY;
  }
}

unsigned int getItemTextureId(item_e item) {
  switch (item) {
    case ITEM_PICKAXE:
      return getTexture("assets/Items/pick_iron.png");
    case ITEM_AXE:
      return getTexture("assets/Items/axe_iron.png");
    case ITEM_DIRT:
      return getTexture("assets/tiles/dirt.png");
    case ITEM_GRASS:
      return getTexture("assets/tiles/dirt_grass.png");
    case ITEM_SEED:
      return getTexture("assets/Items/seed.png");
    default:
      return getTexture(MISSING_TEXTURE);
  }
}

unsigned int getNumberTextureId(int n) {
  switch (n) {
    case 0:
      return getTexture("assets/numbers/Number0 7x10.png");
    case 1:
      return getTexture("assets/numbers/Number1 7x10.png");
    case 2:
      return getTexture("assets/numbers/Number2 7x10.png");
    case 3:
      return getTexture("assets/numbers/Number3 7x10.png");
    case 4:
      return getTexture("assets/numbers/Number4 7x10.png");
    case 5:
      return getTexture("assets/numbers/Number5 7x10.png");
    case 6:
      return getTexture("assets/numbers/Number6 7x10.png");
    case 7:
      return getTexture("assets/numbers/Number7 7x10.png");
    case 8:
      return getTexture("assets/numbers/Number8 7x10.png");
    case 9:
      return getTexture("assets/numbers/Number9 7x10.png");
  }
  return 0;
}
