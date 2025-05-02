#include "systems.h"

#include "ecs.h"
#include "render2d.h"
#include "camera.h"
#include "physics.h"
#include "components.h"
#include "chunk.h"

#include <stdio.h>

extern Render2d *renderer;
extern camera_t camera;


void renderSystem(entity_t entity, double dt) {
  (void)dt;

  renderer->view = camera.view;
  renderer->projection = camera.projection;

  transform_t *transform = (transform_t*)ecsGetComponent(entity, TRANSFORM);
  Sprite *sprite = (Sprite*)ecsGetComponent(entity, SPRITE);
  sprite->x = transform->position.x;
  sprite->y = transform->position.y;
  r2dDrawSprite(renderer, *sprite);
}


void physicsSystem(entity_t e, double dt) {
  rigidbody_t *rb = (rigidbody_t*)ecsGetComponent(e, RIGIDBODY);
  transform_t *tf = (transform_t*)ecsGetComponent(e, TRANSFORM);
  collider_t *c = (collider_t*)ecsGetComponent(e, COLLIDER);
  float *g = (float*)ecsGetComponent(e, GRAVITY);

  // apply forces to object in this case just gravity
  rb->force.y += rb->mass * -(*g);

  // i think collision detection goes here
  // then resolve collisions here pass dt
  collision_t collision;
  if (c) {
    // unsigned int count = ecsGetCount();
    unsigned int count = 10;
    for (unsigned int i = 0; i < count; i++) {
      if (i == e) continue;
      // if (!ecsEntityExists(i)) continue;
      collider_t *bc = (collider_t*)ecsGetComponent(i, COLLIDER);
      transform_t *bt = (transform_t*)ecsGetComponent(i, TRANSFORM);
      if (!bc) continue;
      // check for collision between two colliders
      collision = collisionDetection(c, tf, bc, bt);
      if (collision.hasCollision) {
        break;
        printf("we have a collision between %d %d\n", e, i);
        // rb->force = vec3Scaler(vec3Add(rb->force, collision.a), -50);
        printf("collsion: %f %f\n", collision.a.x, collision.a.y);

      }
    }
  }


  // calculate new position
  rb->velocity.y += rb->force.y / rb->mass * dt;
  rb->velocity.x += rb->force.x / rb->mass * dt;
  tf->position.y += rb->velocity.y * dt;
  tf->position.x += rb->velocity.x * dt;

  if (collision.hasCollision) {
    // stop velocity?
    rb->velocity = (vec3){0, 0, 0};
    // move position
    // printf("\npos: %f %f\n", tf->position.x, tf->position.y);
    collision.a.x = 0;
    collision.a.y = 0.5 - collision.a.y;
    // printf("hasCollision %f %f\n", collision.a.x, collision.a.y);
    // tf->position = vec3Add(tf->position, collision.a);
    tf->position = vec3Add(tf->position, collision.a);
    // printf("pos: %f %f\n\n", tf->position.x, tf->position.y);


  }

  // clear force
  rb->force.y = 0.0f;


  // apply gravity | which is just a downwards force
  // resolve collisions(dt)
  // move objects(dt);
}

void chunkLoadSystem(entity_t e, double dt) {
  (void)dt;
  printf("chunkLoadSystem\n");
  chunk_t *chunk = (chunk_t*)ecsGetComponent(e, CHUNK);

  // check distance from player
  // if under x distance load this chunk into memory and load entities
  float dis = distance((vec3){camera.pos.x, 0, 0}, (vec3){chunk->x + (CHUNK_WIDTH / 2.0f), 0, 0});
  if (dis < CHUNK_DISTANCE) {
    printf("Distance passed loading new chunk\n");
  }
}

void chunkUnloadSystem(entity_t e, double dt) {
  (void)dt;
  printf("chunkUnloadSystem\n");
  chunk_t *chunk = (chunk_t*)ecsGetComponent(e, CHUNK);
  // check distance from player
  // if greater x distance unload this chunk from entities, dont need to unload from memory?
  (void)chunk;
}

void chunkUpdateSystem(entity_t e, double dt) {
  (void)dt;
  printf("chunkUpdateSystem\n");
  chunk_t *chunk = (chunk_t*)ecsGetComponent(e, CHUNK);
  // loop over all tiles looking for "dirty" tiles
  (void)chunk;
}

