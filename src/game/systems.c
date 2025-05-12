#include "systems.h"

#include "camera.h"
#include "components.h"
#include "../platform/sprite.h"
#include "../platform/renderer2d.h"
#include "ecs.h"
#include "physics.h"
#include "world.h"

#include <stdio.h>
#include <stdlib.h>

tile_e held = TILE_DIRT;
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
  }
  setVelocity(playerBody->body, velocity);

  if (input->keyStates[KEY_SPACE] == KEY_HELD) {
    if (velocity.y < 0.1f) {
      applyForce(playerBody->body, (vec2){0.0f, 800.0f});
    }
  }

  if (input->keyStates[KEY_1] == KEY_PRESS) {
    held = TILE_DIRT;
  } else if (input->keyStates[KEY_2] == KEY_PRESS) {
    held = TILE_GRASS;
  } else if (input->keyStates[KEY_3] == KEY_PRESS) {
    held = TILE_SEED;
  }


  // click input
  if (input->mouseStates[MOUSE_BUTTON_LEFT] == KEY_PRESS) {
    vec4 worldPos = screenToWorld(camera, input->mouseX, input->mouseY);
    if (worldPlaceTile(world, worldPos.x, worldPos.y, held)) {
      printf("placed tile\n");
    }
  }
  if (input->mouseStates[MOUSE_BUTTON_RIGHT] == KEY_PRESS) {
    vec4 worldPos = screenToWorld(camera, input->mouseX, input->mouseY);
    tile_e broken = TILE_EMPTY;
    worldBreakTile(world, worldPos.x, worldPos.y, &broken);
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

void drawEntities() {
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

void drawHud(camera_t *camera, input_t *input) {
  float h = input->windowHeight;
  float w = input->windowWidth;
  mat4 proj = orthographic(0, w, h, 0, 0, 1);
  mat4 view = mat4Init(1.0f);
  r2dSetView(view);
  r2dSetProjection(proj);


  int count = 0;
  unsigned long sig = ecsGetSignature(UI) | ecsGetSignature(SPRITE);
  entity_t *entities = ecsQuery(sig, &count);

  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    ui_t *ui = (ui_t*)ecsGetComponent(entity, UI);
    sprite_t *sprite = (sprite_t*)ecsGetComponent(entity, SPRITE);

    sprite->x = ui->pos.x;
    sprite->y = ui->pos.y;
    r2dDrawSprite(*sprite);
  }

  free(entities);


  // set back to camera for world
  r2dSetView(camera->view);
  r2dSetProjection(camera->projection);
}
