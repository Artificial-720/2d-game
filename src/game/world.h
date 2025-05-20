#ifndef WORLD_H
#define WORLD_H

#include "../platform/camera.h"

#define WORLD_WIDTH 840
#define WORLD_HEIGHT 240
#define TILE_LOAD_DISTANCE 30

typedef enum {
  TILE_EMPTY, TILE_DIRT, TILE_GRASS, TILE_SEED, TILE_WOOD, TILE_LEAVES,
  TILE_STONE, TILE_IRON, TILE_COAL,
  TILE_DOOR, TILE_DOOR_CLOSE
} tile_e;

typedef struct {
  int entityId;
  int loaded;
  int dirty;
  tile_e type;
} tile_t;

typedef struct {
  tile_t *tiles;
  tile_t *background;
  int width, height;
} world_t;

world_t *worldInit(int width, int height);
void worldTerminate(world_t *world);

int worldPlaceTile(world_t *world, float x, float y, tile_e type);
void worldBreakTile(world_t *world, float x, float y, tile_e *broken);
void worldBreakTileBackground(world_t *world, float x, float y);

void worldGenerate(world_t *world, int seed);
void growVegetation(world_t *world);

void refreshWorld(world_t *world, float cameraX);

void drawBackground(camera_t * camera, world_t *world);
void drawForeground(camera_t *camera, world_t *world);

tile_e getTileAt(world_t *world, float x, float y);

#endif
