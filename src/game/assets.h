#ifndef ASSETS_H
#define ASSETS_H

#include "world.h"
#include "texture.h"
#include "item.h"

#define MISSING_TEXTURE "assets/image.png"
#define BLACK_TEXTURE "assets/black.png"
#define PAUSE_SCREEN "assets/pause-icon.png"

int isBackgroundTile(tile_e type);
item_e tileDrop(tile_e tile);
unsigned int getTileTextureId(tile_e type);
unsigned int getItemTextureId(item_e item);
unsigned int getNumberTextureId(int n);


#endif
