#include "systems.h"

#include "components.h"
#include "../platform/sprite.h"
#include "../platform/renderer2d.h"
#include "ecs.h"
#include "physics.h"

#include <stdlib.h>

void spriteSystem() {
  int count = 0;
  unsigned long sig = ecsGetSignature(SPRITE) | ecsGetSignature(TRANSFORM);
  entity_t *entities = ecsQuery(sig, &count);

  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    sprite_t *sprite = (sprite_t*)ecsGetComponent(entity, SPRITE);
    transform_t *transform = (transform_t*)ecsGetComponent(entity, TRANSFORM);
    sprite->x = transform->pos.x;
    sprite->y = transform->pos.y;
    r2dDrawSprite(*sprite);
  }

  free(entities);
}

void inputSystem(entity_t player, input_t *input, camera_t *camera, world_t *world) {
  physics_t *playerBody = (physics_t*)ecsGetComponent(player, PHYSICS);
  vec2 velocity = getVelocity(playerBody->body);
  if (input->keyStates[KEY_A] == KEY_HELD) {
    velocity.x = -4.0f;
  } else if (input->keyStates[KEY_D] == KEY_HELD) {
    velocity.x = 4.0f;
  } else {
    velocity.x = 0.0f;
  }
  if (input->keyStates[KEY_W] == KEY_HELD) {
    velocity.y = 4.0f;
  } else if (input->keyStates[KEY_S] == KEY_HELD) {
    velocity.y = -4.0f;
  } else {
    velocity.y = 0.0f;
  }
  setVelocity(playerBody->body, velocity);

  // if (input->keyStates[KEY_SPACE] == KEY_HELD) {
  //   if (velocity.y < 0.1f) {
  //     applyForce(playerBody->body, (vec2){0.0f, 1200.0f});
  //   }
  // }





  (void)player;
  (void)input;
  (void)camera;
  (void)world;
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
  cameraUpdatePosition(camera, tf->pos.x, tf->pos.y);
  r2dSetView(camera->view);
  r2dSetProjection(camera->projection);
}


void physicsSystem(double dt) {
  physicsStep(dt);
  // need to copy transforms out of physics
  int count = 0;
  unsigned long sig = ecsGetSignature(TRANSFORM) | ecsGetSignature(PHYSICS);
  entity_t *entities = ecsQuery(sig, &count);
  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    transform_t *transform = (transform_t*)ecsGetComponent(entity, TRANSFORM);
    physics_t *physics = (physics_t*)ecsGetComponent(entity, PHYSICS);
    if (physics->isStatic) continue;
    transform->pos = getPosition(physics->body);
  }
  free(entities);
}
