#include "chunk.h"

#include "ecs.h"
#include "render2d.h"
#include "physics.h"

#include "components.h"


void placeTile(chunk_t *chunk, float x, float y) {
  if (clickedInChunk(chunk, x, y)) {
    int gridX = (int)x;
    int gridY = (int)((y - 0.1f)); // minus 0.1 to make 12.0 go into 12

    int index = gridY * CHUNK_WIDTH + gridX;
    // if nothing in location place a new tile
    if (chunk->tiles[index].id == 0) {
      entity_t box = ecsCreateEntity();
      Sprite sprite = {.x = 0, .y = 0, .width = 1, .height = 1, .texture = 1}; // fix this texture id stuff
      // make sure to add 1 to the y for the world position
      transform_t transform = {.position = (vec3){gridX, gridY + 1, 0}, .scale = (vec3){1.0f, 1.0f, 1.0f}};
      rigidbody_t rb = {.velocity = (vec3){0, 0, 0}};
      collider_t collider = {.offset = (vec3){0, 0, 0}, .radius = 0.5};
      ecsAddComponent(box, SPRITE, (void*)&sprite);
      ecsAddComponent(box, RIGIDBODY, (void*)&rb);
      ecsAddComponent(box, TRANSFORM, (void*)&transform);
      ecsAddComponent(box, COLLIDER, (void*)&collider);
      chunk->tiles[index] = (tile_t){.id = box};
    }
  }
}

void removeTile(chunk_t *chunk, float x, float y) {
  if (clickedInChunk(chunk, x, y)) {
    int gridX = (int)x;
    int gridY = (int)((y - 0.1f)); // minus 0.1 to make 12.0 go into 12
    int index = gridY * CHUNK_WIDTH + gridX;
    if (chunk->tiles[index].id != 0) {
      ecsDeleteEntity(chunk->tiles[index].id);
      chunk->tiles[index].id = 0;
    }
  }
}
