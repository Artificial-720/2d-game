#include "ecs.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

Render2d *render;

int *entities;
int entityCount;
int maxEntities;

Sprite *sprites;
Transform *transforms;
Rigidbody *rigidbodies;

System *systems;
int maxSystems;
int systemCount;


int ecsInit() {
  entityCount = 0;
  maxEntities = 100;
  entities = (int*)malloc(maxEntities * sizeof(int));
  sprites = (Sprite*)malloc(maxEntities * sizeof(Sprite));
  transforms = (Transform*)malloc(maxEntities * sizeof(Transform));
  rigidbodies = (Rigidbody*)malloc(maxEntities * sizeof(Rigidbody));
  // zero out
  for (int i = 0; i < maxEntities; i++) {
    entities[i] = 0;
    sprites[i] = (Sprite){0};
    transforms[i] = (Transform){0};
  }

  maxSystems = 32;
  systemCount = 0;
  systems = (System*)malloc(maxSystems * sizeof(System));


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
  free(sprites);
  free(transforms);
  free(rigidbodies);

  for (int i = 0; i < systemCount; i++) {
    System s = systems[i];
    free(s.entities);
  }
  free(systems);
}

Entity ecsCreateEntity() {
  Entity e = entityCount;
  entityCount++;

  return e;
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

  // for (int i = 0; i < entityCount; i++) {
  //   transforms[i].position.y -= 100 * deltatime;
  // }


  for (int i = 0; i < systemCount; i++) {
    System system = systems[i];
    // for (int j = 0; j < system.entitiesCount; j++) {
    //   system.systemCallback(system.entities[j]);
    // }
    for (int j = 0; j < entityCount; j++) {
      if (entities[j] & system.signature) {
        system.systemCallback(j, deltatime);
      }
    }
  }

}

void ecsRegisterSystem(int signature, void (*systemCallback)(Entity, double)) {
  assert(systems);

  System s = {
    .signature = signature,
    .systemCallback = systemCallback,
    .entities = 0,
    .entitiesCount = 0,
    .entitiesMax = 32
  };
  s.entities = (Entity*)malloc(s.entitiesMax * sizeof(Entity));

  systems[systemCount] = s;

  systemCount++;
}

void ecsPhysics(Entity entity, double deltatime) {
  printf("hello from ecsPhysics\n");
  transforms[entity].position.y -= 100 * deltatime;
}

unsigned int ecsGetSignature(enum componetId id) {
  return 0x1 << id;
}




// Component
void ecsAddComponentRigidbody(Entity entity, Rigidbody rb) {
  assert(rigidbodies);
  entities[entity] ^= ecsGetSignature(RIGIDBODY);
  rigidbodies[entity] = rb;
}

void ecsAddComponentTransform(Entity entity, Transform transform) {
  assert(transforms);
  entities[entity] ^= ecsGetSignature(TRANSFORM);
  transforms[entity] = transform;
}

void ecsAddComponentSprite(Entity entity, Sprite sprite) {
  assert(sprites);
  entities[entity] ^= ecsGetSignature(SPRITE);
  sprites[entity] = sprite;
}
