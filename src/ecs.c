#include "ecs.h"
#include <assert.h>
#include <stdlib.h>


Render2d *render;

int *entities;
int entityCount;
int maxEntities;

Sprite *sprites;
Transform *transforms;



int ecsInit() {
  entityCount = 0;
  maxEntities = 100;
  entities = (int*)malloc(maxEntities * sizeof(int));
  sprites = (Sprite*)malloc(maxEntities * sizeof(Sprite));
  transforms = (Transform*)malloc(maxEntities * sizeof(Transform));
  // zero out
  for (int i = 0; i < maxEntities; i++) {
    entities[i] = 0;
    sprites[i] = (Sprite){0};
    transforms[i] = (Transform){0};
  }

  render = r2dInit();
  if (!render) {
    return 1;
  }


  // TODO figure better way of handling this data for the projection
  int screenWidth = 1028;
  int screenHeight = 720;
  render->projection = orthographic(0, screenWidth, 0, screenHeight, 0, 1);
  render->view = translate(render->view, (vec3){screenWidth / 2.0f, screenHeight / 2.0f, 0.0f});


  return 0;
}

void ecsTerminate() {
  r2dTerminate(render);
  free(entities);
}

Entity ecsCreateEntity() {
  Entity e = entityCount;
  entityCount++;

  return e;
}

void ecsAddComponetSprite(Entity entity, Sprite sprite) {
  assert(sprites);

  sprites[entity] = sprite;
}

void ecsLoadTexture(const char *filename, unsigned int *textureId) {
  r2dCreateTexture(render, filename, textureId);
}

void ecsRender() {
  r2dClear();

  for (int i = 0; i < entityCount; i++) {
    sprites[i].x = transforms[i].position.x;
    sprites[i].y = transforms[i].position.y;
    r2dDrawSprite(render, sprites[i]);
  }
}


void ecsUpdate(double deltatime) {
  // the idea here is that i call the system function for each entity that has
  // those components

  for (int i = 0; i < entityCount; i++) {
    transforms[i].position.y -= 100 * deltatime;
  }
}

void ecsAddComponetTransform(Entity entity, Transform transform) {
  assert(transforms);

  transforms[entity] = transform;
}
