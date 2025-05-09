#ifndef PHYSICS_H
#define PHYSICS_H

#include "../core/math.h"

void physicsInit();
void physicsCleanup();
void physicsStep(double dt);

// has a transform and rigid-body
unsigned int createBody(vec2 pos, vec2 size); // with a collider
unsigned int createBodyNoCollider(vec2 pos); // no collider just moves and ignores everything
// has a transform and collider
unsigned int createStaticBody(vec2 pos, vec2 size);

void removeBody(unsigned int id);
void removeStaticbody(unsigned int id);

// only works on not static bodies
void applyForce(unsigned int id, vec2 force);
vec2 getPosition(unsigned int id);
vec2 getVelocity(unsigned int id);
void setVelocity(unsigned int id, vec2 v);

#endif
