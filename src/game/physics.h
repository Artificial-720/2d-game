#ifndef PHYSICS_H
#define PHYSICS_H

#include "../core/math.h"

typedef struct {
  unsigned int trigger;
  unsigned int other;
  int onEnter;
} triggerEvent_t;

triggerEvent_t *getTriggerEvents(int *count);

typedef struct {
  unsigned int a;
  unsigned int b;
} contactEvent_t;

contactEvent_t *getContactEvents(int *count);


void physicsInit();
void physicsCleanup();
void physicsStep(double dt);

// has a transform and rigid-body
unsigned int createBody(vec2 pos, vec2 size); // with a collider
unsigned int createBodyNoCollider(vec2 pos); // no collider just moves and ignores everything
// has a transform and collider
unsigned int createStaticBody(vec2 pos, vec2 size);
unsigned int createTrigger(vec2 pos, vec2 size);

void removeTrigger(unsigned int id);
void removeBody(unsigned int id);
void removeStaticbody(unsigned int id);

// only works on not static bodies
void applyForce(unsigned int id, vec2 force);
vec2 getPosition(unsigned int id);
vec2 getVelocity(unsigned int id);
void setVelocity(unsigned int id, vec2 v);
int onGround(unsigned int id);

// static bodies
void setStaticPosition(unsigned int id, vec2 pos);

#endif
