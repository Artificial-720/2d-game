#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "ecs.h"
#include "texture.h"
#include "../core/math.h"

enum component_id {
  TRANSFORM, SPRITE, PHYSICS, UI, PICKUP, ANIMATION, AI
};

typedef struct {
  vec2 pos;
} transform_t;

typedef struct {
  unsigned int body;
  int isStatic;
} physics_t;

typedef struct {
  double frameTime;
  double accumulatedTime;
  int totalFrames;
  int frame;
  texture_t texture;
  vec4 frames[10];
} animation_t;

typedef struct {
  entity_t target;
  double agroDis;
  double cooldown;
} ai_t;


#endif
