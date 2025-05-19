#include "world.h"

#include "components.h"
#include "item.h"
#include "physics.h"
#include "assets.h"
#include "ecs.h"
#include "../platform/sprite.h"
#include "../platform/renderer2d.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define SEED_GROW_CHANCE 50
#define LEAF_CHANCE 45
#define LEAF_MIN_HEIGHT 4
#define TREE_MAX_HEIGHT 10
#define TREE_BRANCH_CHANCE 10

static void convertToGrid(float x, float y, int *ix, int *iy) {
  *ix = (int)x;
  *iy = (int)(y + 1);
}

static int validGridCoords(world_t *world, int x, int y) {
  if (x >= 0 && x < world->width &&
      y >= 0 && y < world->height) {
    return 1;
  }
  return 0;
}

static void indexToWorldCoords(world_t *world, int index, int *x, int *y) {
  *x = index % world->width;
  *y = index / world->width;
}

static int worldCoordsToIndex(world_t *world, int x, int y) {
  return x + world->width * y;
}

tile_e getTileAt(world_t *world, float x, float y) {
  int ix, iy;
  convertToGrid(x, y, &ix, &iy);
  return world->tiles[worldCoordsToIndex(world, ix, iy)].type;
}

world_t *worldInit(int width, int height) {
  assert(width > 0 && height > 0);
  world_t *world = (world_t*)malloc(sizeof(world_t));
  // should make sure world is not null

  world->width = width;
  world->height = height;
  world->tiles = (tile_t*)malloc((width * height) * sizeof(tile_t));
  world->background = (tile_t*)malloc((width * height) * sizeof(tile_t));

  // fill with empty
  tile_t empty = {.entityId = 0, .type = TILE_EMPTY, .loaded = 0};
  for (int i = 0; i < (width * height); i++) {
    world->tiles[i] = empty;
    world->background[i] = empty;
  }

  return world;
}

void worldTerminate(world_t *world) {
  if (world) {
    free(world->tiles);
    free(world->background);
    free(world);
  }
}


static int canPlaceTile(world_t *world, int x, int y, tile_e type) {
  int index = worldCoordsToIndex(world, x, y);
  if (isBackgroundTile(type)) {
    if (world->background[index].type != TILE_EMPTY) return 0;
  } else {
    if (world->tiles[index].type != TILE_EMPTY) return 0;
  }

  if (type == TILE_SEED) {
    if (validGridCoords(world, x, y - 1)) {
      int other = worldCoordsToIndex(world, x, y - 1);
      return (world->tiles[other].type == TILE_GRASS) ? 1 : 0;
    }
    return 0;
  }

  if (type == TILE_DOOR) {
    if (validGridCoords(world, x, y - 1)) {
      int other = worldCoordsToIndex(world, x, y - 1);
      return (world->tiles[other].type == TILE_GRASS) ? 1 : 0;
    }
    return 0;
  }

  return 1;
}

int worldPlaceTile(world_t *world, float x, float y, tile_e type) {
  printf("placing tile with type: %d\n", type);
  assert(world);
  int ix, iy;
  convertToGrid(x, y, &ix, &iy);
  if (validGridCoords(world, ix, iy)) {
    if (canPlaceTile(world, ix, iy, type)) {
      int index = worldCoordsToIndex(world, ix, iy);
      if (isBackgroundTile(type)) {
        world->background[index].type = type;
        world->background[index].dirty = 1;
      } else {
        printf("placing tile\n");
        world->tiles[index].type = type;
        world->tiles[index].dirty = 1;
      }
      return 1;
    }
  }

  return 0;
}


static void spawnPickupTile(tile_e type, int x, int y) {
  item_e drop = tileDrop(type);
  if (drop == ITEM_EMPTY) return;

  unsigned int texture = getTileTextureId(type);
  entity_t item = ecsCreateEntity();
  sprite_t sprite = createSprite(x, y, 0.5f, 0.5f, 0, texture);
  transform_t transform = {.pos = (vec2){x, y}};
  pickup_t pickup = {.item = drop};
  physics_t p = {.body = 0, .isStatic = 0};
  p.body = createBody((vec2){x, y}, (vec2){0.5f, 0.5f});
  ecsAddComponent(item, SPRITE, (void*)&sprite);
  ecsAddComponent(item, TRANSFORM, (void*)&transform);
  ecsAddComponent(item, PHYSICS, (void*)&p);
  ecsAddComponent(item, PICKUP, (void*)&pickup);
}

void worldBreakTile(world_t *world, float x, float y, tile_e *broken) {
  assert(world);
  int ix, iy;
  convertToGrid(x, y, &ix, &iy);
  if (validGridCoords(world, ix, iy)) {
    int index = worldCoordsToIndex(world, ix, iy);
    if (broken) {
      *broken = world->tiles[index].type;
    }

    if (world->tiles[index].type != TILE_EMPTY) {
      spawnPickupTile(world->tiles[index].type, ix, iy);

      // actually remove tile
      world->tiles[index].type = TILE_EMPTY;
      world->tiles[index].dirty = 1;
    }
  }
}

void worldBreakTileBackground(world_t *world, float x, float y) {
  assert(world);
  int ix, iy;
  convertToGrid(x, y, &ix, &iy);
  if (validGridCoords(world, ix, iy)) {
    int index = worldCoordsToIndex(world, ix, iy);
    if (world->background[index].type != TILE_EMPTY) {
      spawnPickupTile(world->background[index].type, ix, iy);

      // actually remove tile
      world->background[index].type = TILE_EMPTY;
      world->background[index].dirty = 1;
    }
  }
}

void worldGenerate(world_t *world, int seed) {
  assert(world);
  int surfaceLevel = 5;

  // 1. flat ground
  tile_t empty = {.entityId = 0, .type = TILE_EMPTY, .loaded = 0};
  tile_t dirt = {.entityId = 0, .type = TILE_DIRT, .loaded = 0};
  for (int i = 0; i < (world->width * world->height); i++) {
    int x, y;
    indexToWorldCoords(world, i, &x, &y);
    world->tiles[i] = (y < surfaceLevel) ? dirt : empty;
  }

  (void)seed;
}

//-----------------------------------------------------------------------------
// Growth
//-----------------------------------------------------------------------------

static void replaceTile(tile_t *tile, tile_e type) {
  assert(tile->type != type);
  tile->type = type;
  tile->dirty = 1;
}

static int validGrassLocation(world_t *world, int x, int y) {
  if (x < 0 || x >= world->width) return 0;
  if (y < 0 || y >= world->height) return 0;

  int index = worldCoordsToIndex(world, x, y);
  if (world->tiles[index].type == TILE_GRASS ||
      world->tiles[index].type == TILE_DIRT) {
    int above = worldCoordsToIndex(world, x, y + 1);
    if (world->tiles[above].type == TILE_EMPTY ||
        world->tiles[above].type == TILE_SEED) {
      return 1;
    }
  }
  return 0;
}

static void updateGrass(world_t *world, int x, int y) {
  int index = worldCoordsToIndex(world, x, y);

  // kill grass
  if (!validGrassLocation(world, x, y)) {
    replaceTile(&world->tiles[index], TILE_DIRT);
    return;
  }

  // spread
  if (rand() % 2) {
    if (validGrassLocation(world, x + 1, y)) {
      int other = worldCoordsToIndex(world, x + 1, y);
      if (world->tiles[other].type != TILE_GRASS) {
        replaceTile(&world->tiles[other], TILE_GRASS);
      }
    }
  } else {
    if (validGrassLocation(world, x - 1, y)) {
      int other = worldCoordsToIndex(world, x - 1, y);
      if (world->tiles[other].type != TILE_GRASS) {
        replaceTile(&world->tiles[other], TILE_GRASS);
      }
    }
  }
}

static int seedCanGrow(world_t *world, int x, int y) {
  // tree is 3 tiles wide and x tiles high
  // with a chance of a branch of the side of the tiles

  int check[3 * TREE_MAX_HEIGHT] = {0};
  int next = 0;

  for (int i = 0; i < TREE_MAX_HEIGHT; i++) {
    int localY = y + i;
    if (!validGridCoords(world, x - 1, localY) ||
      !validGridCoords(world, x, localY) ||
      !validGridCoords(world, x + 1, localY)) {
      return 0;
    }
    check[next++] = worldCoordsToIndex(world, x - 1, localY);
    check[next++] = worldCoordsToIndex(world, x, localY);
    check[next++] = worldCoordsToIndex(world, x + 1, localY);
  }

  for (int i = 0; i < (3 * TREE_MAX_HEIGHT); i++) {
    int index = check[i];
    // check foreground
    if (world->tiles[index].type != TILE_EMPTY) {
        return 0;
    }
    // check background
    if (world->background[index].type != TILE_EMPTY &&
        world->background[index].type != TILE_SEED) {
        return 0;
    }
  }

  return 1;
}

static void updateSeed(world_t *world, int x, int y) {
  if (!seedCanGrow(world, x, y)) return;
  if (!(rand() % 100 < SEED_GROW_CHANCE)) return;

  // grow the tree
  for (int i = 0; i < TREE_MAX_HEIGHT; i++) {
    int localY = y + i;
    int center = worldCoordsToIndex(world, x, localY);
    // place top on tree
    if (i > (TREE_MAX_HEIGHT - 3)) {
      int left = worldCoordsToIndex(world, x - 1, localY);
      int right = worldCoordsToIndex(world, x + 1, localY);
      replaceTile(&world->background[left], TILE_LEAVES);
      replaceTile(&world->background[center], TILE_LEAVES);
      replaceTile(&world->background[right], TILE_LEAVES);
      continue;
    }
    replaceTile(&world->background[center], TILE_WOOD);

    // grow branches
    if (i < LEAF_MIN_HEIGHT) continue;
    if (rand() % 100 < LEAF_CHANCE) {
      int left = worldCoordsToIndex(world, x - 1, localY);
      replaceTile(&world->background[left], TILE_LEAVES);
    }
    if (rand() % 100 < LEAF_CHANCE) {
      int right = worldCoordsToIndex(world, x + 1, localY);
      replaceTile(&world->background[right], TILE_LEAVES);
    }
  }
}

void growVegetation(world_t *world) {
  assert(world);
  for (int i = 0; i < (world->width * world->height); i++) {
    int x, y;
    indexToWorldCoords(world, i, &x, &y);
    if (world->tiles[i].dirty) continue;
    switch (world->tiles[i].type) {
      case TILE_GRASS:
        updateGrass(world, x, y);
        break;
      default:
        ;
    }
  }

  for (int i = 0; i < (world->width * world->height); i++) {
    int x, y;
    indexToWorldCoords(world, i, &x, &y);
    if (world->background[i].dirty) continue;
    switch (world->background[i].type) {
      case TILE_SEED:
        updateSeed(world, x, y);
        break;
      default:
        ;
    }
  }
}

//-----------------------------------------------------------------------------
// Loading and unloading tiles
//-----------------------------------------------------------------------------

// create tile entity for the background
static void createTileEntity(tile_t *tile, int x, int y) {
  entity_t box = ecsCreateEntity();
  transform_t transform = {.pos = (vec2){x, y}};
  ecsAddComponent(box, TRANSFORM, (void*)&transform);

  tile->entityId = box;
  tile->loaded = 1;
  tile->dirty = 0;
}

// create tile entity for the foreground
static void createTileEntityForeground(tile_t *tile, int x, int y) {
  createTileEntity(tile, x, y);

  physics_t p = {.body = 0, .isStatic = 1};
  p.body = createStaticBody((vec2){x, y}, (vec2){1.0f, 1.0f});
  ecsAddComponent(tile->entityId, PHYSICS, (void*)&p);
}

static void removeTileEntity(tile_t *tile) {
  if (!isBackgroundTile(tile->type)) {
    physics_t *p = (physics_t*)ecsGetComponent(tile->entityId, PHYSICS);
    removeStaticbody(p->body);
  }
  ecsDeleteEntity(tile->entityId);
  tile->entityId = 0;
  tile->loaded = 0;
  tile->dirty = 0;
}


void refreshWorld(world_t *world, float cameraX) {
  for (int i = 0; i < (world->height * world->width); i++) {
    int x, y;
    indexToWorldCoords(world, i, &x, &y);
    if (x > cameraX - TILE_LOAD_DISTANCE && x < cameraX + TILE_LOAD_DISTANCE) {
      // should be loaded
      if (world->tiles[i].loaded) {
        if (world->tiles[i].dirty) {
          if (world->tiles[i].type == TILE_EMPTY) {
            physics_t *p = (physics_t*)ecsGetComponent(world->tiles[i].entityId, PHYSICS);
            removeStaticbody(p->body);
            ecsDeleteEntity(world->tiles[i].entityId);
            world->tiles[i].entityId = 0;
            world->tiles[i].loaded = 0;
            world->tiles[i].dirty = 0;
          }
        }
        continue;
      }

      if (world->tiles[i].type == TILE_EMPTY) continue;
      createTileEntityForeground(&world->tiles[i], x, y);
    } else {
      // should be unloaded
      if (!world->tiles[i].loaded) {
        continue;
      }

      removeTileEntity(&world->tiles[i]);
    }
  }
  // Handle background tiles
  for (int i = 0; i < (world->height * world->width); i++) {
    int x, y;
    indexToWorldCoords(world, i, &x, &y);
    if (x > cameraX - TILE_LOAD_DISTANCE && x < cameraX + TILE_LOAD_DISTANCE) {
      // should be loaded
      if (world->background[i].loaded) {
        if (world->background[i].dirty) {
          if (world->tiles[i].type == TILE_EMPTY) {
            ecsDeleteEntity(world->background[i].entityId);
            world->background[i].entityId = 0;
            world->background[i].loaded = 0;
            world->background[i].dirty = 0;
          }
        }
        continue;
      }

      if (world->background[i].type == TILE_EMPTY) continue;
      createTileEntity(&world->background[i], x, y);
    } else {
      // should be unloaded
      if (!world->background[i].loaded) {
        continue;
      }

      removeTileEntity(&world->background[i]);
    }
  }
}


//-----------------------------------------------------------------------------
// Draw
//-----------------------------------------------------------------------------
void drawBackground(camera_t * camera, world_t *world) {
  sprite_t sprite = createSprite(0, 0, 1, 1, 0, 0);
  for (int i = 0; i < (world->height * world->width); i++) {
    int x, y;
    indexToWorldCoords(world, i, &x, &y);
    if (x > camera->pos.x - TILE_LOAD_DISTANCE && x < camera->pos.x + TILE_LOAD_DISTANCE) {
      if (world->background[i].type == TILE_EMPTY) continue;

      sprite.x = x;
      sprite.y = y;
      sprite.texture = getTileTextureId(world->background[i].type);

      r2dDrawSprite(camera, sprite);
    }
  }
}

void drawForeground(camera_t *camera, world_t *world) {
  sprite_t sprite = createSprite(0, 0, 1, 1, 0, 0);
  for (int i = 0; i < (world->height * world->width); i++) {
    int x, y;
    indexToWorldCoords(world, i, &x, &y);
    if (x > camera->pos.x - TILE_LOAD_DISTANCE && x < camera->pos.x + TILE_LOAD_DISTANCE) {
      if (world->tiles[i].type == TILE_EMPTY) continue;

      sprite.height = 1;
      sprite.x = x;
      sprite.y = y;
      sprite.texture = getTileTextureId(world->tiles[i].type);

      if (world->tiles[i].type == TILE_DOOR) {
        sprite.height = 2;
        sprite.y += 1;
      }

      r2dDrawSprite(camera, sprite);
    }
  }
}
