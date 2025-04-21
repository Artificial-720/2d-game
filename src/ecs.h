#ifndef ECS_H
#define ECS_H


#include "render2d.h"
#include "math.h"

typedef unsigned int Entity;

typedef struct {
  vec3 position;
  vec3 rotation;
  vec3 scale;
} Transform;


int ecsInit();
void ecsTerminate();

Entity ecsCreateEntity();
void ecsAddComponetSprite(Entity entity, Sprite sprite);
void ecsAddComponetTransform(Entity entity, Transform transform);

void ecsLoadTexture(const char *filename, unsigned int *textureId);

void ecsUpdate(double deltatime);
void ecsRender();

#endif
