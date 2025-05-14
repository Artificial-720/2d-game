#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "../core/math.h"

enum component_id {
  TRANSFORM, SPRITE, PHYSICS, UI, PICKUP
};

typedef struct {
  vec2 pos;
} transform_t;

typedef struct {
  unsigned int body;
  int isStatic;
} physics_t;


#endif
