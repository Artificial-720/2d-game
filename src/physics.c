#include "physics.h"

#include <stdio.h>

void step(double deltatime) {
  (void)deltatime;
  // ApplyGravity();
  // ResolveCollisions(dt);
  // MoveObjects(dt);
}

typedef struct {
  vec3 a; // point of A into B
  vec3 b; // point of B into A
  vec3 normal; // b - a normalized
  float depth; // length of b - a
  int hasCollision;
} collision_t2;

collision_t collisionDetection(collider_t *ac, transform_t *at, collider_t *bc, transform_t *bt) {
  collision_t result = {0};

  vec3 aCenter = vec3Add(at->position, ac->offset);
  vec3 bCenter = vec3Add(bt->position, bc->offset);
  float d = distance(aCenter, bCenter);

  if (d < (ac->radius + bc->radius)) {
    result.hasCollision = 1;

    // float midpointdistance = d / 2;
    vec3 ab = vec3Sub(aCenter, bCenter);
    result.a = vec3Scaler(normalize(ab), (d/2));
    vec3 ba = vec3Sub(bCenter, aCenter);
    result.b = vec3Scaler(normalize(ba), (d/2));

    result.normal = normalize(ba);






  } else {
    result.hasCollision = 0;
  }

  return result;
}
