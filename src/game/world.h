#ifndef WORLD_H
#define WORLD_H

// #define WORLD_WIDTH 8400
// #define WORLD_HEIGHT 2400
#define WORLD_WIDTH 15
#define WORLD_HEIGHT 30
#define TILE_LOAD_DISTANCE 20

typedef enum {
  TILE_EMPTY, TILE_DIRT, TILE_GRASS, TILE_SEED, TILE_WOOD
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

void worldGenerate(world_t *world, int seed);
void growVegetation(world_t *world);

void refreshWorld(world_t *world, float cameraX);

#endif
