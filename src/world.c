#include "world.h"

#include <stdio.h>
#include <stdlib.h>

#include "components.h"
#include "ecs.h"
#include "render2d.h"
#include "physics.h"
#include "camera.h"
extern camera_t camera;

static void indexToWorldCoords(int index, int *x, int *y) {
  *x = index % WORLD_WIDTH;
  *y = index / WORLD_WIDTH;
}
// static int worldCorrdsToIndex(int x, int y) {
//   return x + WORLD_WIDTH * y;
// }

static void createTileEntity(tile_t *tile, int x, int y) {
  entity_t box = ecsCreateEntity();
  Sprite sprite = {.x = 0, .y = 0, .width = 1, .height = 1, .texture = 1}; // fix this texture id stuff
  transform_t transform = {.position = (vec3){x, y, 0}, .scale = (vec3){1.0f, 1.0f, 1.0f}};
  rigidbody_t rb = {.velocity = (vec3){0, 0, 0}};
  collider_t collider = {.offset = (vec3){0, 0, 0}, .radius = 0.5};
  ecsAddComponent(box, SPRITE, (void*)&sprite);
  ecsAddComponent(box, RIGIDBODY, (void*)&rb);
  ecsAddComponent(box, TRANSFORM, (void*)&transform);
  ecsAddComponent(box, COLLIDER, (void*)&collider);

  tile->entityId = box;
  tile->loaded = 1;
}

static void remoteTileEntity(tile_t *tile) {
  ecsDeleteEntity(tile->entityId);
  tile->entityId = 0;
  tile->loaded = 0;
}

world_t worldGenerate() {
  world_t world = {
    .tiles = 0
  };

  world.tiles = (tile_t*)malloc(WORLD_TILE_COUNT * sizeof(tile_t));

  tile_t empty = {.entityId = 0, .type = TILE_EMPTY, .loaded = 0};
  tile_t full = {.entityId = 0, .type = TILE_GRASS, .loaded = 0};
  for (int i = 0; i < WORLD_TILE_COUNT; i++) {
    int x, y;
    indexToWorldCoords(i, &x, &y);
    world.tiles[i] = (y < 2) ? full : empty;
  }


  return world;
}

void worldTerminate(world_t *world) {
  free(world->tiles);
}

void worldLoadTiles(world_t *world) {
  for (int i = 0; i < WORLD_TILE_COUNT; i++) {
    if (world->tiles[i].type == TILE_EMPTY) continue;
    if (world->tiles[i].loaded) continue;

    int x, y;
    indexToWorldCoords(i, &x, &y);
    if (x  > camera.pos.x - TILE_LOAD_DISTANCE && x < camera.pos.x + TILE_LOAD_DISTANCE) {
      createTileEntity(&world->tiles[i], x, y);
    }
  }
}

void worldUnloadTiles(world_t *world) {
  for (int i = 0; i < WORLD_TILE_COUNT; i++) {
    if (world->tiles[i].type == TILE_EMPTY) continue;
    if (!world->tiles[i].loaded) continue;

    int x, y;
    indexToWorldCoords(i, &x, &y);
    if (x  < camera.pos.x - TILE_LOAD_DISTANCE || x > camera.pos.x + TILE_LOAD_DISTANCE) {
      remoteTileEntity(&world->tiles[i]);
    }
  }
}

