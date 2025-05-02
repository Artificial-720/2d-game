#ifndef WORLD_H
#define WORLD_H

// #define WORLD_WIDTH 8400
// #define WORLD_HEIGHT 2400
#define WORLD_WIDTH 100
#define WORLD_HEIGHT 100
#define WORLD_TILE_COUNT WORLD_HEIGHT * WORLD_WIDTH
#define TILE_LOAD_DISTANCE 20

enum tile_type {
  TILE_EMPTY, TILE_GRASS
};

typedef struct {
  int entityId;
  int loaded;
  enum tile_type type;
} tile_t;

typedef struct {
  tile_t *tiles;
} world_t;

world_t worldGenerate();
void worldTerminate(world_t *world);

void worldLoadTiles(world_t *world);
void worldUnloadTiles(world_t *world);

#endif
