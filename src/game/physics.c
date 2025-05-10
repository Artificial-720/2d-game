#include "physics.h"

#include <assert.h>
#include <stdio.h>

#define MAX_COLLISIONS 10

typedef struct {
  vec2 velocity;
  vec2 force;
  float mass;
} rigidbody_t;

typedef struct {
  unsigned int id;
  vec2 pos;
  vec2 size;
  rigidbody_t rb;
} body_t;

typedef struct {
  unsigned int id;
  vec2 pos;
  vec2 size;
} staticBody_t;

typedef struct {
  // vec3 a; // point of A into B
  // vec3 b; // point of B into A
  // vec3 normal; // b - a normalized
  // float depth; // length of b - a
  // int hasCollision;
  staticBody_t body;
} collision_t;

static body_t bodies[100];
static staticBody_t staticBodies[10000];
static unsigned int bodyCount = 0;
static unsigned int staticCount = 0;
static int nextBodyId = 0;
static int nextStaticId = 0;
static float gravity = 9.81f;

void physicsInit() {
}

void physicsCleanup() {
}

unsigned int createBody(vec2 pos, vec2 size) {
  int id = nextBodyId++;
  body_t b = {
    .pos = pos,
    .size = size,
    .id = id
  };
  bodies[bodyCount++] = b;

  return id;
}

// unsigned int createBodyNoCollider(vec2 pos) {
// }

unsigned int createStaticBody(vec2 pos, vec2 size) {
  int id = nextStaticId++;
  staticBody_t b = {
    .pos = pos,
    .size = size,
    .id = id
  };
  staticBodies[staticCount++] = b;

  return id;
}

// void removeBody(unsigned int id) {
// }

static int indexOf(unsigned int id, unsigned int *index) {
  for (unsigned int i = 0; i < staticCount; i++) {
    if (id == staticBodies[i].id) {
      *index = i;
      return 0;
    }
  }
  return 1;
}

void removeStaticbody(unsigned int id) {
  unsigned int index;
  assert(!indexOf(id, &index));

  staticCount--;
  for (unsigned int i = index; i < staticCount; i++) {
    staticBodies[i] = staticBodies[i + 1];
  }
}

// void applyForce(unsigned int id, vec2 force) {
// }

vec2 getPosition(unsigned int id) {
  return bodies[id].pos;
}

vec2 getVelocity(unsigned int id) {
  return bodies[id].rb.velocity;
}

void setVelocity(unsigned int id, vec2 v) {
  bodies[id].rb.velocity = v;
}

void applyForce(unsigned int id, vec2 force) {
  bodies[id].rb.force = vec2Add(bodies[id].rb.force, force);
}

int pointInsideQuad(vec2 pos, vec2 size, vec2 point) {
  if (point.x >= pos.x && point.x <= pos.x + size.x &&
      point.y <= pos.y && point.y >= pos.y - size.y) {
    return 1;
  }
  return 0;
}

int quadQuadIntersection(vec2 posA, vec2 sizeA, vec2 posB, vec2 sizeB) {
  if (pointInsideQuad(posA, sizeA, (vec2){posB.x          , posB.y          })) return 1;
  if (pointInsideQuad(posA, sizeA, (vec2){posB.x + sizeB.x, posB.y          })) return 1;
  if (pointInsideQuad(posA, sizeA, (vec2){posB.x          , posB.y - sizeB.y})) return 1;
  if (pointInsideQuad(posA, sizeA, (vec2){posB.x + sizeB.x, posB.y - sizeB.y})) return 1;

  if (pointInsideQuad(posB, sizeB, (vec2){posA.x          , posA.y          })) return 1;
  if (pointInsideQuad(posB, sizeB, (vec2){posA.x + sizeA.x, posA.y          })) return 1;
  if (pointInsideQuad(posB, sizeB, (vec2){posA.x          , posA.y - sizeA.y})) return 1;
  if (pointInsideQuad(posB, sizeB, (vec2){posA.x + sizeA.x, posA.y - sizeA.y})) return 1;

  return 0;
}

void physicsStep(double dt) {
  // printf("running physics step\n");
  for (unsigned int i = 0; i < bodyCount; i++) {
    // printf("pos: %f %f dt: %f\n", bodies[i].pos.x, bodies[i].pos.y, dt);
    // printf("vel: %f %f\n", bodies[i].rb.velocity.x, bodies[i].rb.velocity.y);
    // printf("applying forces\n");
    bodies[i].rb.force.y += -(gravity);
    // printf("updating positions\n");
    bodies[i].rb.velocity.y += bodies[i].rb.force.y * dt;
    bodies[i].rb.velocity.x += bodies[i].rb.force.x * dt;
    bodies[i].pos.y += bodies[i].rb.velocity.y * dt;
    bodies[i].pos.x += bodies[i].rb.velocity.x * dt;

    // printf("checking for collisions with dynamic bodies\n");
    for (unsigned int j = 0; j < bodyCount; j++) {
      if (i == j) continue;
      // TODO
    }
    // printf("checking for collisions with static bodies\n");


    int collisionCount = 0;
    collision_t collisions[MAX_COLLISIONS] = {0};
    // printf("player pos: %f %f\n", bodies[i].pos.x, bodies[i].pos.y);
    for (unsigned int j = 0; j < staticCount; j++) {
      // printf("static pos: %f %f, size: %f %f\n", staticBodies[j].pos.x,staticBodies[j].pos.y,staticBodies[j].size.x,staticBodies[j].size.y);
      if (quadQuadIntersection(bodies[i].pos, bodies[i].size, staticBodies[j].pos, staticBodies[j].size)) {
        collision_t c = {
          .body = staticBodies[j]
        };
        collisions[collisionCount++] = c;
        if (collisionCount >= MAX_COLLISIONS) break;
      }
    }
    // printf("resolving collisions\n");
    printf("collision Count %d\n", collisionCount);
    for (int j = 0; j < collisionCount; j++) {
      staticBody_t b = collisions[j].body;
      // find best axis to resolve collision
      // -x, +x, -y, +y

      vec2 maxA = {bodies[i].pos.x + bodies[i].size.x, bodies[i].pos.y};
      vec2 minA = {bodies[i].pos.x, bodies[i].pos.y - bodies[i].size.y};

      vec2 maxB = {b.pos.x + b.size.x, b.pos.y};
      vec2 minB = {b.pos.x, b.pos.y - b.size.y};

      // calculate distances
      float distances[4] = {
        (maxB.y - minA.y), // dis of A bottom to B top
        (maxA.y - minB.y), // dis of A top to B bottom
        (maxB.x - minA.x), // dis of A left to B right
        (maxA.x - minB.x)  // dis of A right to B left
      };
      vec2 normals[4] = {
        { 0.0f, 1.0f},
        { 0.0f,-1.0f},
        { 1.0f, 0.0f},
        {-1.0f, 0.0f}
      };

      float pen = distances[0];
      vec2 dir = normals[0];
      for (int w = 1; w < 4; w++) {
        if (distances[w] < pen) {
          pen = distances[w];
          dir = normals[w];
        }
      }

      // update position
      bodies[i].pos.y += dir.y * pen;
      bodies[i].pos.x += dir.x * pen;
      // printf("dir: %f %f\n", dir.x, dir.y);
      // zero out velocity that has dir
      if (dir.x) {
        bodies[i].rb.velocity.x = 0;
      }
      if (dir.y) {
        bodies[i].rb.velocity.y = 0;
      }




      // OLD ATTEMPT

      // vec2 change = {0, 0};

      // // resolve Y axis
      // vec3 normY = normalize((vec3){0, bodies[i].pos.y - b.pos.y, 0});
      // float d = distance((vec3){0,bodies[i].pos.y,0}, (vec3){0,b.pos.y,0});
      // float expectedD = bodies[i].size.y / 2 + b.size.y / 2;
      // change.y = normY.y * fabs(expectedD - d);

      // // resolve X axis
      // vec3 normX = normalize((vec3){bodies[i].pos.x - b.pos.x, 0, 0});
      // float actualDisX = distance((vec3){bodies[i].pos.x, 0, 0}, (vec3){b.pos.x, 0, 0});
      // float expectedDisX = bodies[i].size.x / 2 + b.size.x / 2;
      // change.x = normX.x * fabs(expectedDisX - actualDisX);

      // printf("change: %f %f\n", change.x, change.y);
      // // only do one axis to resolve collision
      // if (change.x > change.y) {
      //   printf("apply change to Y %f<---------------------------\n", change.y);
      //   bodies[i].pos.y += change.y;
      //   bodies[i].rb.velocity.y = 0;
      // } else {
      //   printf("apply change to X %f<---------------------------\n", change.x);
      //   bodies[i].pos.x += change.x;
      //   bodies[i].rb.velocity.x = 0;
      // }
    }
    // printf("clear force\n");
    bodies[i].rb.force.y = 0;
    bodies[i].rb.force.x = 0;
  }
}
