#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "ecs.h"
#include "texture.h"
#include "animation.h"
#include "../core/math.h"

enum component_id {
  TRANSFORM, SPRITE, PHYSICS, UI, PICKUP, ANIMATION, AI, COOLDOWN
};

typedef struct {
  vec2 pos;
} transform_t;

typedef struct {
  unsigned int body;
  int isStatic;
} physics_t;

// typedef struct {
//   double frameTime;
//   double accumulatedTime;
//   int totalFrames;
//   int frame;
//   texture_t texture;
//   vec4 frames[10];
// } animation_t;

typedef enum {
  AI_IDLE,
  AI_PRE_JUMP,
  AI_JUMPING,
  AI_JUMP_TO_FALL,
  AI_FALLING,
  AI_LANDING,
  AI_HURT,
  AI_DEAD
} aiState_e;

typedef struct {
  aiState_e state;
  entity_t target;
  double agroDis;
  double cooldown;
  int facingLeft;
} ai_t;


#endif
