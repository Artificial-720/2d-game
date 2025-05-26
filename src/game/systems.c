#include "systems.h"

#include "animation.h"
#include "components.h"
#include "ecs.h"
#include "item.h"
#include "physics.h"
#include "player.h"
#include "state.h"
#include "world.h"
#include "../platform/renderer2d.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
  int grounded = onGround(playerBody->body);
  if (input->keyStates[KEY_A] == KEY_HELD) {
    velocity.x = -4.0f;
    player->facingLeft = 1;
    if (grounded) {
      player->state = PLAYER_WALKING;
    }
  } else if (input->keyStates[KEY_D] == KEY_HELD) {
    velocity.x = 4.0f;
    player->facingLeft = 0;
    if (grounded) {
      player->state = PLAYER_WALKING;
    }
  } else {
    velocity.x = 0.0f;
    if (player->state != PLAYER_USE && player->state != PLAYER_ATTACKING) {
      player->state = PLAYER_IDLE;
    }
  }
  if (input->keyStates[KEY_W] == KEY_HELD) {
    velocity.y = 4.0f;
  } else if (input->keyStates[KEY_S] == KEY_HELD) {
    velocity.y = -4.0f;
  }
  setVelocity(playerBody->body, velocity);

  if (input->keyStates[KEY_SPACE] == KEY_HELD) {
    if (onGround(playerBody->body)) {
      applyForce(playerBody->body, (vec2){0.0f, 800.0f});
      player->state = PLAYER_JUMPING;
    }
  }

  if (velocity.y < -0.01f) {
    player->state = PLAYER_FALLING;
  } else if (velocity.y > 0.01f) {
    player->state = PLAYER_JUMPING;
  }

  // end attack
  combat_t *combat = (combat_t*)ecsGetComponent(player->entity, COMBAT);
  if (player->state == PLAYER_ATTACKING && combat->cooldown <= 0.0f) {
    player->state = PLAYER_IDLE;
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
      // player->state = PLAYER_USE;
    }
  }
  if (input->mouseStates[MOUSE_BUTTON_RIGHT] == KEY_PRESS) {
    // vec4 worldPos = screenToWorld(camera, input->mouseX, input->mouseY);
    // tile_e underMouse = getTileAt(world, worldPos.x, worldPos.y);
    // if (underMouse == TILE_DOOR) {
    //   printf("door\n");
    // }
  }


  return STATE_PLAYING;
}

state_e inputPause(input_t *input) {
  if (input->keyStates[KEY_ESCAPE] == KEY_PRESS) {
    return STATE_PLAYING;
  }
  return STATE_PAUSE;
}

void lifetimeSystem(double dt) {
  int count = 0;
  unsigned long sig = ecsGetSignature(LIFETIME);
  entity_t *entities = ecsQuery(sig, &count);
  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    lifetime_t *lifetime = (lifetime_t*)ecsGetComponent(entity, LIFETIME);
    lifetime->remaining -= dt;
    if (lifetime->remaining <= 0.0f) {
      printf("lifetime ran out deleting entity %d\n", entity);

      if (ecsHasComponent(entity, PHYSICS)) {
        physics_t *physics = (physics_t*)ecsGetComponent(entity, PHYSICS);
        if (physics->isTrigger) {
          removeTrigger(physics->body);
        }
      }
      ecsDeleteEntity(entity);
    }
  }
  free(entities);
}

void triggerSystem() {
  int eventCount = 0;
  triggerEvent_t *events = getTriggerEvents(&eventCount);

  int count = 0;
  unsigned long sig = ecsGetSignature(PHYSICS);
  entity_t *entities = ecsQuery(sig, &count);
  for (int i = 0; i < eventCount; i++) {
    triggerEvent_t event = events[i];
    if (!event.onEnter) continue;

    // find the trigger that was "triggered"
    trigger_cb_t callback = 0;
    entity_t triggerEntity = 0;
    int found = 0;
    for (int j = 0; j < count; j++) {
      entity_t entity = entities[j];
      physics_t *physics = (physics_t*)ecsGetComponent(entity, PHYSICS);
      if (!physics->isTrigger) continue;
      if (physics->body == event.trigger) {
        found = 1;
        triggerEntity = entity;
        callback = physics->callback;
        break;
      }
    }
    assert(found);

    // find the other entity id
    for (int j = 0; j < count; j++) {
      entity_t entity = entities[j];
      physics_t *physics = (physics_t*)ecsGetComponent(entity, PHYSICS);

      if (physics->body == event.other) {
        if (callback) {
          callback(triggerEntity, entity);
        }
        break;
      }
    }
  }

  free(entities);
}

void animationSystem(double dt) {
  int count = 0;
  unsigned long sig = ecsGetSignature(SPRITE) | ecsGetSignature(ANIMATION);
  entity_t *entities = ecsQuery(sig, &count);
  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    animationComponent_t *ani = (animationComponent_t*)ecsGetComponent(entity, ANIMATION);
    sprite_t *sprite = (sprite_t*)ecsGetComponent(entity, SPRITE);


    int facingLeft = 0;

    ani_cb cb = ani->callback;
    if (cb) {
      animationState_e cache = ani->state;
      ani->state = cb(entity, &facingLeft);
      if (cache != ani->state) {
        ani->animations[ani->state].current = 0;
        ani->cooldown = 0;
      }
    }




    if (ani->cooldown <= 0.0f) {
      animation_t *a = &ani->animations[ani->state];

      // printf("calling state for entity %d, state: %d\n", entity, ani->state);
      // printf("current frame: %d\n", a->current);

      sprite->texture = a->texture.id;
      if (facingLeft) {
        // Flip horizontally by using negative subWidth and adjusting subX
        sprite->subWidth = -((float)(a->width - 2 * a->paddingX) / a->texture.width);
        sprite->subX = (a->x + ((float)a->width * (a->current + 1)) - a->paddingX) / a->texture.width;

        sprite->subY = (float)a->y / a->texture.height;
        sprite->subHeight = (float)a->height / a->texture.height;
      } else {
        sprite->subX = ((a->x + ((float)a->width * a->current)) + a->paddingX) / a->texture.width;
        sprite->subY = (float)a->y / a->texture.height;
        // sprite->subWidth = (float)a->width / a->texture.width;
        sprite->subWidth = (float)(a->width - 2 * a->paddingX) / a->texture.width;
        sprite->subHeight = (float)a->height / a->texture.height;
      }



      // printf("sprite x: %f y: %f width %f height %f\n", sprite->subX, sprite->subY, sprite->subWidth, sprite->subHeight);

      a->current += 1;
      if (a->current >= a->total) {
        a->current = 0;
      }

      ani->cooldown = a->time;
    }
    ani->cooldown -= dt;

  }
  free(entities);
}

void cooldownSystem(double dt) {
  int count = 0;
  unsigned long sig = ecsGetSignature(COOLDOWN);
  entity_t *entities = ecsQuery(sig, &count);
  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    cooldown_t *cooldown = (cooldown_t*)ecsGetComponent(entity, COOLDOWN);
    if (cooldown->time > 0.0f) {
      cooldown->time -= dt;
    }
  }
  free(entities);

  count = 0;
  sig = ecsGetSignature(COMBAT);
  entities = ecsQuery(sig, &count);
  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    combat_t *combat = (combat_t*)ecsGetComponent(entity, COMBAT);
    if (combat->cooldown > 0.0f) {
      combat->cooldown -= dt;
    }
  }
  free(entities);
}

void aiSystem(double dt) {
  int count = 0;
  unsigned long sig = ecsGetSignature(TRANSFORM) | ecsGetSignature(PHYSICS) | ecsGetSignature(AI);
  entity_t *entities = ecsQuery(sig, &count);
  printf("ai system count: %d\n", count);
  for (int i = 0; i < count; i++) {
    entity_t entity = entities[i];
    transform_t *transform = (transform_t*)ecsGetComponent(entity, TRANSFORM);
    physics_t *physics = (physics_t*)ecsGetComponent(entity, PHYSICS);
    ai_t *ai = (ai_t*)ecsGetComponent(entity, AI);
    health_t *health = (health_t*)ecsGetComponent(entity, HEALTH);

    int grounded = onGround(physics->body);
    vec2 vel = getVelocity(physics->body);
    if (grounded) {
      vel.x = 0.0f;
      setVelocity(physics->body, vel);
    }

    if (ai->cooldown > 0) {
      ai->cooldown -= dt;
      continue;
    }

    if (health->value <= 0) {
      if (ai->state == AI_DEAD) {
        removeBody(physics->body);
        ecsDeleteEntity(entity);
      } else {
        ai->state = AI_DEAD;
        ai->cooldown = 2.8f;
      }
      continue;
    }

    if (ai->state == AI_IDLE) {
      transform_t *target = (transform_t*)ecsGetComponent(ai->target, TRANSFORM);
      double dis = distance((vec3){transform->pos.x, transform->pos.y, 0.0f}, (vec3){target->pos.x, target->pos.y, 0.0f});
      if (dis < ai->agroDis) {
        ai->state = AI_PRE_JUMP;
        ai->cooldown = 1.5f;
      }
      if (transform->pos.x > target->pos.x) {
        ai->facingLeft = 1;
      } else {
        ai->facingLeft = 0;
      }

    } else if (ai->state == AI_PRE_JUMP) {
      transform_t *target = (transform_t*)ecsGetComponent(ai->target, TRANSFORM);
      // apply force
      float jumpForce = 340.0f; // 340
      vec2 force = {0.0f, 0.0f};
      if (onGround(physics->body)) {
        force.x = jumpForce;
        force.y = jumpForce;
      }
      if (transform->pos.x > target->pos.x) {
        force.x *= -1.0f;
      }
      applyForce(physics->body, force);

      ai->state = AI_JUMPING;
    } else if (ai->state == AI_JUMPING) {
      if (vel.y < 1.0f) {
        ai->state = AI_JUMP_TO_FALL;
      }
    } else if (ai->state == AI_JUMP_TO_FALL) {
      if (vel.y < -1.0f) {
        ai->state = AI_FALLING;
      }
    } else if (ai->state == AI_FALLING) {
      if (grounded) {
        ai->state = AI_LANDING;
        ai->cooldown = 1.5f;
      }
    } else if (ai->state == AI_LANDING) {
      ai->state = AI_IDLE;
      ai->cooldown = 1.0f;
    }
  } /* end for */
  free(entities);
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
    if (physics->isTrigger) continue;
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
