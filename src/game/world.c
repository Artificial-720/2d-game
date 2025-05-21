#include "world.h"
#include "assets.h"
#include "components.h"
#include "ecs.h"
#include "physics.h"
#include "../core/noise.h"
#include "../platform/renderer2d.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define TREE_MAX_HEIGHT 10

static inline int validGridCoords(world_t *world, int x, int y) {
  return (x >= 0 && x < world->width && y >= 0 && y < world->height);
}

static void spawnPickupTile(tile_e type, int x, int y) {
  item_e drop = tileDrop(type);
  if (drop == ITEM_EMPTY) return;

  unsigned int texture = getItemTextureId(drop);
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




void worldInit(world_t *world, int width, int height) {
  assert(width > 0 && height > 0);

  world->width = width;
  world->height = height;
  world->tiles = (tile_t*)malloc((width * height) * sizeof(tile_t));
  world->background = (tile_t*)malloc((width * height) * sizeof(tile_t));

  // fill with empty
  tile_t empty = {0};
  for (int i = 0; i < (width * height); i++) {
    world->tiles[i] = empty;
    world->background[i] = empty;
  }
}

void worldTerminate(world_t *world) {
  free(world->tiles);
  free(world->background);
}

int worldPlaceTile(world_t *world, float x, float y, tile_e type) {
  assert(world);
  int ix = (int)x;
  int iy = (int)(y + 1);
  if (!validGridCoords(world, ix, iy)) return 0;

  int index = world->width * iy + ix;
  if (isBackgroundTile(type)) {
    if (world->background[index].type != TILE_EMPTY) return 0;
  } else {
    if (world->tiles[index].type != TILE_EMPTY) return 0;
  }

  // special case tiles
  if (type == TILE_SEED) {
    if (!validGridCoords(world, x, y - 1)) return 0;
    int other = world->width * y + x;
    if (world->tiles[other].type != TILE_GRASS) return 0;
  }
  // todo handle doors

  // place the tile
  if (isBackgroundTile(type)) {
    world->background[index].type = type;
    world->background[index].dirty = 1;
  } else {
    world->tiles[index].type = type;
    world->tiles[index].dirty = 1;
  }

  return 1;
}

void worldBreakTile(world_t *world, float x, float y, tile_e *broken) {
  assert(world);
  int ix = (int)x;
  int iy = (int)(y + 1);
  if (!validGridCoords(world, ix, iy)) return;

  int index = world->width * iy + ix;
  if (broken) {
    *broken = world->tiles[index].type;
  }
  if (world->tiles[index].type == TILE_EMPTY) return;

  spawnPickupTile(world->tiles[index].type, ix, iy);
  world->tiles[index].type = TILE_EMPTY;
  world->tiles[index].dirty = 1;
}

void worldBreakTileBackground(world_t *world, float x, float y) {
  assert(world);
  int ix = (int)x;
  int iy = (int)(y + 1);
  if (!validGridCoords(world, ix, iy)) return;

  int index = world->width * iy + ix;
  if (world->background[index].type == TILE_EMPTY) return;

  if (world->background[index].type == TILE_WOOD) {
    // remove all the wood above and leaves
    for (int i = 0; i < TREE_MAX_HEIGHT; i++) {
      if (validGridCoords(world, ix, iy + i)) {
        int subIndex = world->width * (iy + i) + ix;
        if (world->background[subIndex].type == TILE_WOOD ||
            world->background[subIndex].type == TILE_LEAVES) {
          spawnPickupTile(world->background[subIndex].type, ix, iy + i);
          // actually remove tile
          world->background[subIndex].type = TILE_EMPTY;
          world->background[subIndex].dirty = 1;
        }
        // check for leaves
        if (validGridCoords(world, ix + 1, iy + i)) {
          int rightIndex = world->width * (iy + i) + (ix + 1);
          if (world->background[rightIndex].type == TILE_LEAVES) {
          world->background[rightIndex].type = TILE_EMPTY;
          world->background[rightIndex].dirty = 1;
          }
        }
        if (validGridCoords(world, ix - 1, iy + i)) {
          int leftIndex = world->width * (iy + i) + (ix - 1);
          if (world->background[leftIndex].type == TILE_LEAVES) {
          world->background[leftIndex].type = TILE_EMPTY;
          world->background[leftIndex].dirty = 1;
          }
        }
      }
    }
  } else {
    spawnPickupTile(world->background[index].type, ix, iy);
    // actually remove tile
    world->background[index].type = TILE_EMPTY;
    world->background[index].dirty = 1;
  }
}

//-----------------------------------------------------------------------------
// World Generation
//-----------------------------------------------------------------------------

void worldGenerate(world_t *world, int seed) {
  assert(world);
  int surfaceLevel = 70;
  int maxHill = 5;
  int dirtDepth = 10;
  float caveChance = 0.5f;
  float oreChance = 0.5f;
  float seedChance = 0.5f;
  float noiseScale = 0.1f;

  // flat ground for testing
  // int stopIndex = world->width * (surfaceLevel + 1);
  // for (int i = 0; i < (world->width * world->height); i++) {
  //   if (stopIndex == i) break;
  //   world->tiles[i] = dirt;
  // }

  // 1. Generating height
  for (int x = 0; x < WORLD_WIDTH; x++) {
    double height = perlin(x * noiseScale, surfaceLevel * noiseScale);
    int offset = maxHill * height;

    for (int y = 0; y < WORLD_HEIGHT; y++) {
      int index = world->width * y + x;
      int surface = surfaceLevel + offset;
      if (y < surface) {
        world->tiles[index].type = TILE_STONE;
      }
      // else {
      //   world->tiles[index].type = TILE_EMPTY;
      // }
    }
  }

  // 2. Generating caves
  for (int x = 0; x < WORLD_WIDTH; x++) {
    for (int y = 0; y < WORLD_HEIGHT; y++) {
      int index = world->width * y + x;
      if (world->tiles[index].type == TILE_STONE) {
        double chance = perlin(x * noiseScale, y * noiseScale);
        if (chance > caveChance) {
          world->tiles[index].type = TILE_EMPTY;
        }
      }
    }
  }

  // 3. Filling with ore
  noiseScale += 0.1f;
  for (int x = 0; x < WORLD_WIDTH; x++) {
    for (int y = 0; y < WORLD_HEIGHT; y++) {
      int index = world->width * y + x;
      if (world->tiles[index].type == TILE_STONE) {
        double chance = perlin(x * noiseScale, y * noiseScale);
        if (chance > oreChance) {
          world->tiles[index].type = TILE_IRON;
        }
      }
    }
  }

  // 4. Surface replacement
  for (int x = 0; x < WORLD_WIDTH; x++) {
    int count = 0;
    for (int y = WORLD_HEIGHT - 1; y > 0; y--) {
      if (count > dirtDepth) break;
      int index = world->width * y + x;

      if (world->tiles[index].type == TILE_EMPTY) continue;

      if (world->tiles[index].type == TILE_STONE) {
        if (count == 0) {
          world->tiles[index].type = TILE_GRASS;
        } else {
          world->tiles[index].type = TILE_DIRT;
        }
        count++;
      }
    }
  }

  // 5. Planting trees
  for (int x = 0; x < WORLD_WIDTH; x++) {
    for (int y = WORLD_HEIGHT - 1; y > 0; y--) {
      int index = world->width * y + x;
      if (world->tiles[index].type != TILE_GRASS) continue;

      double chance = perlin(x * noiseScale, y * noiseScale);
      if (chance > seedChance) {
        if (validGridCoords(world, x, y + 1)) {
          int subIndex = world->width * (y + 1) + x;
          world->background[subIndex].type = TILE_SEED;
        }
      }
    }
  }

  (void)seed;
}

//-----------------------------------------------------------------------------
// Growth
//-----------------------------------------------------------------------------

void growVegetation(world_t *world) {
  (void)world;
}


//-----------------------------------------------------------------------------
// Loading and unloading tiles
//-----------------------------------------------------------------------------


typedef struct {
  int used; // slot used
  int hasId;
  unsigned int id;
  int x, y;
  int index;
} slotWorld_t;

#define MAX_WORLD_SLOTS 2000
static slotWorld_t slots[MAX_WORLD_SLOTS] = {0};

void refreshPhysicsEntities(camera_t *camera, world_t *world) {
  assert(world);

  // some sort of list of physic bodies
  // remove all the bodies
  for (int i = 0; i < MAX_WORLD_SLOTS; i++) {
    if (!slots[i].used) continue;

    if (slots[i].x > camera->pos.x - TILE_LOAD_DISTANCE &&
        slots[i].x < camera->pos.x + TILE_LOAD_DISTANCE &&
        slots[i].y > camera->pos.y - TILE_LOAD_DISTANCE &&
        slots[i].y < camera->pos.y + TILE_LOAD_DISTANCE) {
      // should be loaded
      if (world->tiles[slots[i].index].dirty) {
        if (world->tiles[slots[i].index].type == TILE_EMPTY) {
          slots[i].used = 0;
        }
        world->tiles[slots[i].index].dirty = 0;
      }
    } else {
      // should be unloaded
      slots[i].used = 0;
    }
  }


  // create new bodies
  for (int i = 0; i < (world->height * world->width); i++) {
    if (world->tiles[i].type == TILE_EMPTY) continue;
    int x = i % world->width;
    int y = i / world->width;


    if (x > camera->pos.x - TILE_LOAD_DISTANCE && x < camera->pos.x + TILE_LOAD_DISTANCE &&
        y > camera->pos.y - TILE_LOAD_DISTANCE && y < camera->pos.y + TILE_LOAD_DISTANCE) {

      // check if already have a body
      int alreadyLoaded = 0;
      for (int j = 0; j < MAX_WORLD_SLOTS; j++) {
        if (slots[j].used && slots[j].x == x && slots[j].y == y) {
          alreadyLoaded = 1;
          break;
        }
      }
      if (alreadyLoaded) continue;



      // find a open slot, prefer slots that are not freed
      int found = 0;
      for (int j = 0; j < MAX_WORLD_SLOTS; j++) {
        if (!slots[j].used && slots[j].hasId) {
          found = 1;
          slots[j].used = 1;
          slots[j].x = x;
          slots[j].y = y;
          slots[j].index = i;
          setStaticPosition(slots[j].id, (vec2){x, y});
          break;
        }
      }
      if (!found) {
        // find next open slot that is freed
        for (int j = 0; j < MAX_WORLD_SLOTS; j++) {
          if (!slots[j].used) {
            slotWorld_t s = {
              .used = 1,
              .hasId = 1,
              .id = createStaticBody((vec2){x, y}, (vec2){1.0f, 1.0f}),
              .x = x, .y = y,
              .index = i
            };
            slots[j] = s;
            found = 1;
            break;
          }
        }
      }

      if (!found) {
        // problem ran out of slots
        assert(0);
      }
    }
  }

  // finally remove the unused static bodies
  for (int i = 0; i < MAX_WORLD_SLOTS; i++) {
    if (slots[i].hasId && !slots[i].used) {
      removeStaticbody(slots[i].id);
      slots[i].hasId = 0;
    }
  }

}

//-----------------------------------------------------------------------------
// Draw
//-----------------------------------------------------------------------------

void drawBackground(camera_t * camera, world_t *world) {
  sprite_t sprite = createSprite(0, 0, 1, 1, 0, 0);
  for (int i = 0; i < (world->height * world->width); i++) {
    int x = i % world->width;
    int y = i / world->width;
    if (y <= camera->pos.y - TILE_LOAD_DISTANCE || y >= camera->pos.y + TILE_LOAD_DISTANCE) continue;
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
    int x = i % world->width;
    int y = i / world->width;
    if (y <= camera->pos.y - TILE_LOAD_DISTANCE || y >= camera->pos.y + TILE_LOAD_DISTANCE) continue;
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
