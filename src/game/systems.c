#include "systems.h"

#include "ecs.h"
#include "physics.h"
#include "components.h"
#include "../platform/renderer2d.h"

#include <stdlib.h>

static void physicsStep(double dt, entity_t entity) {
  rigidbody_t *rb = (rigidbody_t*)ecsGetComponent(entity, RIGIDBODY);
  transform_t *tf = (transform_t*)ecsGetComponent(entity, TRANSFORM);

  // apply forces to object in this case just gravity
  if (ecsHasComponent(entity, GRAVITY)) {
    float *g = (float*)ecsGetComponent(entity, GRAVITY);
    rb->force.y += rb->mass * -(*g);
  }

  // collision detection
  collision_t collision;
  if (ecsHasComponent(entity, COLLIDER)) {
    collider_t *c = (collider_t*)ecsGetComponent(entity, COLLIDER);

    int count = 0;
    unsigned long sig = ecsGetSignature(TRANSFORM);
    sig |= ecsGetSignature(COLLIDER);
    entity_t *others = ecsQuery(sig, &count);
    for (int i = 0; i < count; i++) {
      entity_t other = others[i];
      if (other == entity) continue;
      collider_t *otherC = (collider_t*)ecsGetComponent(other, COLLIDER);
      transform_t *otherT = (transform_t*)ecsGetComponent(other, TRANSFORM);
      // check for collision between two colliders
      collision = collisionDetection(c, tf, otherC, otherT);
      if (collision.hasCollision) {
        break; // TODO handle multiple collisions
      }
    }
    free(others);
  }

  // calculate new position
  rb->velocity.y += rb->force.y / rb->mass * dt;
  rb->velocity.x += rb->force.x / rb->mass * dt;
  tf->position.y += rb->velocity.y * dt;
  tf->position.x += rb->velocity.x * dt;

  // resolve collisions
  if (collision.hasCollision) {
    rb->velocity = (vec3){0, 0, 0};
    // move position
    collision.a.x = 0;
    collision.a.y = 0.5 - collision.a.y;
    tf->position = vec3Add(tf->position, collision.a);
  }

  // clear force
  rb->force.y = 0.0f;
}

void spriteSystem() {
  int count = 0;
  unsigned long sig = ecsGetSignature(TRANSFORM);
  sig |= ecsGetSignature(SPRITE);
  entity_t *entities = ecsQuery(sig, &count);

  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    transform_t *transform = (transform_t*)ecsGetComponent(entity, TRANSFORM);
    sprite_t *sprite = (sprite_t*)ecsGetComponent(entity, SPRITE);
    sprite->x = transform->position.x;
    sprite->y = transform->position.y;
    r2dDrawSprite(*sprite);
  }

  free(entities);
}

void physicsSystem(double dt) {
  int count = 0;
  unsigned long sig = ecsGetSignature(TRANSFORM);
  sig |= ecsGetSignature(RIGIDBODY);
  entity_t *entities = ecsQuery(sig, &count);

  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    physicsStep(dt, entity);
  }

  free(entities);
}

void playerSystem(entity_t player, input_t *input) {
  rigidbody_t *rb = (rigidbody_t*)ecsGetComponent(player, RIGIDBODY);
  if (input->keyStates[KEY_A] == KEY_HELD) {
    rb->velocity.x = -4.0f;
  } else if (input->keyStates[KEY_D] == KEY_HELD) {
    rb->velocity.x = 4.0f;
  }
  if (input->keyStates[KEY_SPACE] == KEY_HELD) {
    if (rb->velocity.y < 0.01) {
      rb->force.y = 500;
    }
  }
}

void cameraSystem(camera_t *camera, entity_t player, input_t *input) {
  camera->width = input->windowWidth;
  camera->height = input->windowHeight;
  float h = 30;
  float w = h * ((float)camera->width / camera->height);
  camera->projection = orthographic(
    -w / (2 * camera->zoomFactor), w / (2 * camera->zoomFactor),
    -h / (2 * camera->zoomFactor), h / (2 * camera->zoomFactor),
    -1, 1
  );
  transform_t *tf = (transform_t*)ecsGetComponent(player, TRANSFORM);
  cameraUpdatePosition(camera, tf->position.x, tf->position.y);
  r2dSetView(camera->view);
  r2dSetProjection(camera->projection);
}
