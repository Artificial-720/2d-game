#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "../core/math.h"

enum component_id {
  RIGIDBODY, TRANSFORM, SPRITE, GRAVITY, ANIMATION, COLLIDER, CHUNK, PHYSICS
};
typedef struct {
  vec2 pos;
} transform_t;
typedef struct {
  unsigned int body;
  int isStatic;
} physics_t;


#endif
