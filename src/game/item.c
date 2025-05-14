#include "item.h"

#include "texture.h"
#include <assert.h>


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
      assert(0 && "Missing item texture");
  }
}
