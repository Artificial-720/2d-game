#include "world.h"

#include "../platform/sprite.h"
#include "components.h"
#include "physics.h"
#include "texture.h"
#include "ecs.h"
#include "asset_map.h"

#include <assert.h>
#include <stdlib.h>

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

static unsigned int getTileTextureId(tile_e type) {
  switch (type) {
    case TILE_DIRT:
      return getTexture(SPRITE_DIRT);
    case TILE_GRASS:
      return getTexture(SPRITE_GRASS);
    case TILE_SEED:
      return getTexture(SPRITE_SEED);
    default:
      assert(0);
      return 0;
  }
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

int worldPlaceTile(world_t *world, float x, float y, tile_e type) {
  assert(world);
  int ix, iy;
  convertToGrid(x, y, &ix, &iy);
  if (validGridCoords(world, ix, iy)) {
    int index = worldCoordsToIndex(world, ix, iy);
    if (world->tiles[index].type == TILE_EMPTY) {
      world->tiles[index].type = type;
      world->tiles[index].dirty = 1;
    }
  }

  return 1;
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
      world->tiles[index].type = TILE_EMPTY;
      world->tiles[index].dirty = 1;
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
    return  (world->tiles[above].type == TILE_EMPTY) ? 1 : 0;
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

void growVegetation(world_t *world) {
  assert(world);
  for (int i = 0; i < (world->width * world->height); i++) {
    int x, y;
    indexToWorldCoords(world, i, &x, &y);
    if (world->tiles[i].dirty) continue;
    switch (world->tiles[i].type) {
      case TILE_GRASS:
        updateGrass(world, x, y);
      default:
        ;
    }
  }
}

//-----------------------------------------------------------------------------
// Loading and unloading tiles
//-----------------------------------------------------------------------------

static void createTileEntity(tile_t *tile, int x, int y) {
  unsigned int texture = getTileTextureId(tile->type);

  entity_t box = ecsCreateEntity();
  sprite_t sprite = {.x = 0, .y = 0, .width = 1, .height = 1, .texture = texture};
  transform_t transform = {.pos = (vec2){x, y}};
  physics_t p = {.body = 0, .isStatic = 1};
  p.body = createStaticBody((vec2){x, y}, (vec2){1.0f, 1.0f});
  ecsAddComponent(box, SPRITE, (void*)&sprite);
  ecsAddComponent(box, TRANSFORM, (void*)&transform);
  ecsAddComponent(box, PHYSICS, (void*)&p);

  tile->entityId = box;
  tile->loaded = 1;
  tile->dirty = 0;
}

static void removeTileEntity(tile_t *tile) {
  physics_t *p = (physics_t*)ecsGetComponent(tile->entityId, PHYSICS);
  removeStaticbody(p->body);
  ecsDeleteEntity(tile->entityId);
  tile->entityId = 0;
  tile->loaded = 0;
  tile->dirty = 0;
}


static void refreshTileEntity(tile_t *tile) {
  if (tile->loaded && tile->type == TILE_EMPTY) {
    removeTileEntity(tile);
    return;
  }

  sprite_t *sprite = (sprite_t*)ecsGetComponent(tile->entityId, SPRITE);
  sprite->texture = getTileTextureId(tile->type);
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
          refreshTileEntity(&world->tiles[i]);
        }
        continue;
      }

      if (world->tiles[i].type == TILE_EMPTY) continue;
      createTileEntity(&world->tiles[i], x, y);
    } else {
      // should be unloaded
      if (!world->tiles[i].loaded) {
        continue;
      }

      removeTileEntity(&world->tiles[i]);
    }
  }
}
