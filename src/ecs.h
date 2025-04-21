#ifndef ECS_H
#define ECS_H

#include "render2d.h"
#include "math.h"

typedef struct {
  vec3 velocity;
  vec3 acceleration;
} Rigidbody;

typedef struct {
  vec3 position;
  vec3 rotation;
  vec3 scale;
} Transform;

enum componetId {
  RIGIDBODY, TRANSFORM, SPRITE
};

unsigned int ecsGetSignature(enum componetId id);


typedef unsigned int Entity;

typedef struct {
  int signature;
  void (*systemCallback)(Entity, double);
  Entity *entities;
  int entitiesCount;
  int entitiesMax;
} System;


int ecsInit();
void ecsTerminate();

Entity ecsCreateEntity();
void ecsAddComponentSprite(Entity entity, Sprite sprite);
void ecsAddComponentTransform(Entity entity, Transform transform);
void ecsAddComponentRigidbody(Entity entity, Rigidbody rb);

void ecsLoadTexture(const char *filename, unsigned int *textureId);

void ecsUpdate(double deltatime);
void ecsRender();

void ecsRegisterSystem(int signature, void (*systemCallback)(Entity, double));

void ecsPhysics(Entity entity, double deltatime);

#endif
