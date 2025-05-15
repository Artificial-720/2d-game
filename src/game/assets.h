#ifndef ASSETS_H
#define ASSETS_H

#include "world.h"
#include "texture.h"
#include "item.h"

#define MISSING_TEXTURE "assets/image.png"
#define BLACK_TEXTURE "assets/black.png"
#define PAUSE_SCREEN "assets/pause-icon.png"

unsigned int getTileTextureId(tile_e type);
int isBackgroundTile(tile_e type);
item_e tileDrop(tile_e tile);
unsigned int getItemTextureId(item_e item);
unsigned int getNumberTextureId(int n);


#endif
