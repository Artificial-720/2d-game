#include "world.h"

#include <stdlib.h>

#include "texture.h"
#include "components.h"
#include "ecs.h"
#include "physics.h"
#include "../platform/sprite.h"

static int withinLoadDistance(int x, int cameraX) {
    return (x  > cameraX - TILE_LOAD_DISTANCE && x < cameraX + TILE_LOAD_DISTANCE);
}

static void indexToWorldCoords(int index, int *x, int *y) {
  *x = index % WORLD_WIDTH;
  *y = index / WORLD_WIDTH;
}
static int worldCorrdsToIndex(int x, int y) {
  return x + WORLD_WIDTH * y;
}

static void createTileEntity(tile_t *tile, int x, int y) {
  unsigned int texture = getTexture("assets/tiles/dirt.png");

  entity_t box = ecsCreateEntity();
  sprite_t sprite = {.x = 0, .y = 0, .width = 1, .height = 1, .texture = texture};
  transform_t transform = {.position = (vec3){x, y, 0}, .scale = (vec3){1.0f, 1.0f, 1.0f}};
  collider_t collider = {.offset = (vec3){0, 0, 0}, .radius = 0.5};
  ecsAddComponent(box, SPRITE, (void*)&sprite);
  ecsAddComponent(box, TRANSFORM, (void*)&transform);
  ecsAddComponent(box, COLLIDER, (void*)&collider);

  tile->entityId = box;
  tile->loaded = 1;
}

static void removeTileEntity(tile_t *tile) {
  ecsDeleteEntity(tile->entityId);
  tile->entityId = 0;
  tile->loaded = 0;
}

world_t worldInit() {
  world_t world = {
    .tiles = 0
  };

  world.tiles = (tile_t*)malloc(WORLD_TILE_COUNT * sizeof(tile_t));

  // fill with empty to prevent crash
  tile_t empty = {.entityId = 0, .type = TILE_EMPTY, .loaded = 0};
  for (int i = 0; i < WORLD_TILE_COUNT; i++) {
    int x, y;
    indexToWorldCoords(i, &x, &y);
    world.tiles[i] = empty;
  }

  return world;
}

void worldTerminate(world_t *world) {
  free(world->tiles);
}


void worldLoadTiles(world_t *world, int cameraX) {
  for (int i = 0; i < WORLD_TILE_COUNT; i++) {
    if (world->tiles[i].type == TILE_EMPTY) continue;
    if (world->tiles[i].loaded) continue;

    int x, y;
    indexToWorldCoords(i, &x, &y);
    if (withinLoadDistance(x, cameraX)) {
      createTileEntity(&world->tiles[i], x, y);
    }
  }
}

void worldUnloadTiles(world_t *world, int cameraX) {
  for (int i = 0; i < WORLD_TILE_COUNT; i++) {
    if (world->tiles[i].type == TILE_EMPTY) continue;
    if (!world->tiles[i].loaded) continue;

    int x, y;
    indexToWorldCoords(i, &x, &y);
    if (!withinLoadDistance(x, cameraX)) {
      removeTileEntity(&world->tiles[i]);
    }
  }
}

void worldTranslateToGrid(float x, float y, int *tileX, int *tileY) {
  *tileX = (int)x;
  *tileY = (int)(y + 1);
}

void worldPlaceTile(world_t *world, int x, int y, enum tile_type type) {
  int index = worldCorrdsToIndex(x, y);

  if (world->tiles[index].type == TILE_EMPTY) {
    world->tiles[index].type = type;
  }
}

#include <stdio.h>
#include "../core/noise.h"
void worldGenerate(world_t *world) {
  printf("Generating world\n");
  int surfaceLevel = 50;

  // 1. flat ground
  printf("Empty world\n");
  tile_t empty = {.entityId = 0, .type = TILE_EMPTY, .loaded = 0};
  tile_t dirt = {.entityId = 0, .type = TILE_GRASS, .loaded = 0};
  for (int i = 0; i < WORLD_TILE_COUNT; i++) {
    int x, y;
    indexToWorldCoords(i, &x, &y);
    world->tiles[i] = (y < surfaceLevel) ? dirt : empty;
  }

  float noiseScale = 0.1f;
  int maxHill = 5; // offset from surface level
  printf("Adding hills\n");

  for (int x = 0; x < WORLD_WIDTH; x++) {
    double height = perlin(x * noiseScale, surfaceLevel * noiseScale);
    int offset = maxHill * height;

    for (int y = 0; y < WORLD_HEIGHT; y++) {
      int index = worldCorrdsToIndex(x, y);
      world->tiles[index] = (y < (surfaceLevel + offset)) ? dirt : empty;
    }
  }

}
