#ifndef ANIMATION_H
#define ANIMATION_H

#include "ecs.h"
#include "texture.h"

typedef enum {
  ANIM_IDLE,
  ANIM_JUMP_START,
  ANIM_JUMP_UP,
  ANIM_JUMP_FALL,
  ANIM_JUMP_DOWN,
  ANIM_JUMP_LAND,
  ANIM_HURT,
  ANIM_DEATH,
  ANIM_WALK,
  ANIM_COUNT
} animationState_e;

typedef animationState_e (*ani_cb)(entity_t, int*);


typedef struct {
  int set;
  texture_t texture;
  int current;
  int total;
  int width, height;
  double time;
  int x, y;
  int paddingX, paddingY;
} animation_t;

typedef struct {
  animationState_e state;
  ani_cb callback;
  double cooldown;
  animation_t animations[ANIM_COUNT];
} animationComponent_t;

typedef struct {
  double time;
} cooldown_t;

animationState_e slimeAnimation(entity_t entity, int *facingLeft);


animation_t createAnimation(texture_t texture, int x, int y, int paddingX, int paddingY, int frames, int width, int height, double time);

#endif
