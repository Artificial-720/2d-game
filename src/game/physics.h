#ifndef PHYSICS_H
#define PHYSICS_H

#include "../core/math.h"

typedef struct {
  vec3 velocity;
  vec3 force;
  float mass;
} rigidbody_t;

typedef struct {
  vec3 position;
  vec3 rotation;
  vec3 scale;
} transform_t;

typedef struct {
  vec3 offset; // offset from transform position
  float radius;
} collider_t;


typedef struct {
  vec3 a; // point of A into B
  vec3 b; // point of B into A
  vec3 normal; // b - a normalized
  float depth; // length of b - a
  int hasCollision;
} collision_t;


// does a physics step
void step(double deltatime);

collision_t collisionDetection(collider_t *ac, transform_t *at, collider_t *bc, transform_t *bt);


#endif
