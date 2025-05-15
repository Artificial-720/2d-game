#include "systems.h"

#include "components.h"
#include "ecs.h"
#include "item.h"
#include "physics.h"
#include "player.h"
#include "state.h"
#include "world.h"
#include "../platform/renderer2d.h"

#include <stdlib.h>

state_e inputPlaying(player_t *player, camera_t *camera, world_t *world, input_t *input, output_t *output) {
  // Pause the game
  if (input->keyStates[KEY_ESCAPE] == KEY_PRESS) {
    return STATE_PAUSE;
  }

  // full screen toggle
  if (input->keyStates[KEY_F] == KEY_PRESS) {
    output->toggleFullScreen = 1;
  }

  // apply forces
  physics_t *playerBody = (physics_t*)ecsGetComponent(player->entity, PHYSICS);
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

  // hot bar selection
  if (input->keyStates[KEY_1] == KEY_PRESS) {
    player->selected = 0;
  } else if (input->keyStates[KEY_2] == KEY_PRESS) {
    player->selected = 1;
  } else if (input->keyStates[KEY_3] == KEY_PRESS) {
    player->selected = 2;
  } else if (input->keyStates[KEY_4] == KEY_PRESS) {
    player->selected = 3;
  } else if (input->keyStates[KEY_5] == KEY_PRESS) {
    player->selected = 4;
  } else if (input->keyStates[KEY_6] == KEY_PRESS) {
    player->selected = 5;
  } else if (input->keyStates[KEY_7] == KEY_PRESS) {
    player->selected = 6;
  } else if (input->keyStates[KEY_8] == KEY_PRESS) {
    player->selected = 7;
  } else if (input->keyStates[KEY_9] == KEY_PRESS) {
    player->selected = 8;
  } else if (input->keyStates[KEY_0] == KEY_PRESS) {
    player->selected = 9;
  }

  // click input
  if (input->mouseStates[MOUSE_BUTTON_LEFT] == KEY_PRESS) {
    vec4 worldPos = screenToWorld(camera, input->mouseX, input->mouseY);
    use_cb use = getUseItem(player->inventory[player->selected].item);
    if (use) {
      use(world, player, (vec2){worldPos.x, worldPos.y});
    }
  }

  return STATE_PLAYING;
}

state_e inputPause(input_t *input) {
  if (input->keyStates[KEY_ESCAPE] == KEY_PRESS) {
    return STATE_PLAYING;
  }
  return STATE_PAUSE;
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

void drawEntities(camera_t *camera) {
  int count = 0;
  unsigned long sig = ecsGetSignature(SPRITE) | ecsGetSignature(TRANSFORM);
  entity_t *entities = ecsQuery(sig, &count);

  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    sprite_t *sprite = (sprite_t*)ecsGetComponent(entity, SPRITE);
    transform_t *transform = (transform_t*)ecsGetComponent(entity, TRANSFORM);
    sprite->x = transform->pos.x;
    sprite->y = transform->pos.y;
    r2dDrawSprite(camera, *sprite);
  }

  free(entities);
}

void pickupItems(player_t *player) {
  transform_t *transform = (transform_t*)ecsGetComponent(player->entity, TRANSFORM);
  vec3 a = {transform->pos.x, transform->pos.y, 0};

  int count = 0;
  unsigned long sig = ecsGetSignature(TRANSFORM) | ecsGetSignature(PICKUP);
  entity_t *entities = ecsQuery(sig, &count);

  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    transform_t *t = (transform_t*)ecsGetComponent(entity, TRANSFORM);
    pickup_t *pickup = (pickup_t*)ecsGetComponent(entity, PICKUP);
    vec3 b = {t->pos.x, t->pos.y, 0};

    float dis = distance(a, b);
    if (dis < player->pickupDis) {
      if (giveItemToPlayer(player, pickup->item, 1)) {
        ecsDeleteEntity(entity);
      }
    }
  }

  free(entities);
}

void updateCameras(entity_t player, camera_t *camera, camera_t *cameraUi, input_t *input) {
  // main world camera
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

  // ui camera
  cameraUi->width = input->windowWidth;
  cameraUi->height = input->windowHeight;
  cameraUi->projection = orthographic(0, input->windowWidth, input->windowHeight, 0, 0, 1);
}
