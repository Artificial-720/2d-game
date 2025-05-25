#include "animation.h"
#include "components.h"
#include "ecs.h"
#include "physics.h"
#include "../platform/sprite.h"
#include "texture.h"

#include <stdio.h>

animation_t createAnimation(texture_t texture, int x, int y, int paddingX, int paddingY, int frames, int width, int height, double time) {
  animation_t a = {0};
  a.set = 1;
  a.width = width;
  a.height = height;
  a.texture = texture;
  a.current = 0;
  a.total = frames;
  a.time = time;
  a.x = x;
  a.y = y;
  a.paddingX = paddingX;
  a.paddingY = paddingY;

  return a;
}



animationState_e slimeAnimation(entity_t entity, int *facingLeft) {
  ai_t *ai = (ai_t*)ecsGetComponent(entity, AI);
  *facingLeft = ai->facingLeft;
  switch (ai->state) {
    case AI_IDLE:         return ANIM_IDLE;
    case AI_PRE_JUMP:     return ANIM_JUMP_START;
    case AI_JUMPING:      return ANIM_JUMP_UP;
    case AI_JUMP_TO_FALL: return ANIM_JUMP_FALL;
    case AI_FALLING:      return ANIM_JUMP_DOWN;
    case AI_LANDING:      return ANIM_JUMP_LAND;
    case AI_HURT:         return ANIM_HURT;
    case AI_DEAD:         return ANIM_DEATH;
    default:              return ANIM_IDLE;
  }
}

