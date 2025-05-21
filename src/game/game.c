#include "game.h"

#include "ecs.h"
#include "item.h"
#include "physics.h"
#include "world.h"
#include "components.h"
#include "systems.h"
#include "texture.h"
#include "ui.h"
#include "player.h"
#include "state.h"
#include "../platform/renderer2d.h"
#include <stdio.h>

#define PLAYER_START_X 10
#define PLAYER_START_Y 15
#define TICK_RATE (1.0f / 1.0f)

typedef struct {
  camera_t camera;
  camera_t cameraUi;
  world_t world;
  player_t player;
} gameState_t;

static gameState_t gameState;
static double accumulated;
static state_e state;

int gameInit() {
  accumulated = 0.0f;
  state = STATE_PLAYING;
  // Set sky color
  r2dSetClearColorRGBA(130.0f, 190.0f, 253.0f, 1.0f);

  ecsInit();

  // register our ecs components
  ecsRegisterComponent(SPRITE, sizeof(sprite_t));
  ecsRegisterComponent(TRANSFORM, sizeof(transform_t));
  ecsRegisterComponent(PHYSICS, sizeof(physics_t));
  ecsRegisterComponent(UI, sizeof(ui_t));
  ecsRegisterComponent(PICKUP, sizeof(pickup_t));
  ecsRegisterComponent(ANIMATION, sizeof(animation_t));

  gameState.player.pickupDis = 3.0f;

  // Create our player entity
  unsigned int texture = loadTexture("assets/image.png").id;
  entity_t player = ecsCreateEntity();
  sprite_t sprite = createSprite(10, 10, 1.5, 3, 0, texture);
  transform_t transform = {.pos = (vec2){10, 10}};
  physics_t p = {.body = 0, .isStatic = 0};
  p.body = createBody((vec2){PLAYER_START_X, PLAYER_START_Y}, (vec2){1.5f, 3.0f});
  animation_t animation = {0};
  animation.frameTime = 0.5f;
  animation.texture = loadTexture("assets/player.png");
  for (int i = 0; i < 7; i++) {
    animation.frames[i] = (vec4){(32 * i), 576, 32, 48};
  }
  animation.totalFrames = 7;
  ecsAddComponent(player, SPRITE, (void*)&sprite);
  ecsAddComponent(player, TRANSFORM, (void*)&transform);
  ecsAddComponent(player, PHYSICS, (void*)&p);
  ecsAddComponent(player, ANIMATION, (void*)&animation);
  gameState.player.entity = player;

  // Setup UI entities
  setupHud();

  // populate default inventory
  giveItemToPlayer(&gameState.player, ITEM_PICKAXE, 1);
  giveItemToPlayer(&gameState.player, ITEM_AXE, 1);
  giveItemToPlayer(&gameState.player, ITEM_DIRT, 9);
  giveItemToPlayer(&gameState.player, ITEM_GRASS, 9);
  giveItemToPlayer(&gameState.player, ITEM_SEED, 9);
  giveItemToPlayer(&gameState.player, ITEM_DOOR, 5);
  gameState.player.selected = 0;

  // Create our world and load the area around the player
  worldInit(&gameState.world, WORLD_WIDTH, WORLD_HEIGHT);
  worldGenerate(&gameState.world, 10);
  // load in initial world, load the tiles around the player
  refreshPhysicsEntities(&gameState.camera, &gameState.world);

  // Setup Camera
  gameState.camera = cameraInit();
  gameState.cameraUi = cameraInit();

  return 0;
}

int gameFrame(double dt, input_t *input, output_t *output) {
  // player input
  switch (state) {
    case STATE_PLAYING:
      state = inputPlaying(&gameState.player, &gameState.camera, &gameState.world, input, output);
      break;
    case STATE_PAUSE:
      state = inputPause(input);
      break;
  }

  if (state == STATE_PLAYING) {
    refreshPhysicsEntities(&gameState.camera, &gameState.world);

    // update physics
    physicsSystem(dt);
    // pick up tiles near player
    pickupItems(&gameState.player);

    accumulated += dt;
    if (accumulated > TICK_RATE) {
      growVegetation(&gameState.world);
      accumulated -= TICK_RATE;
    }

    animationSystem(dt);
  }

  // render
  updateCameras(gameState.player.entity, &gameState.camera, &gameState.cameraUi, input);
  drawBackground(&gameState.camera, &gameState.world);
  drawEntities(&gameState.camera);
  drawForeground(&gameState.camera, &gameState.world);
  drawHud(&gameState.player, &gameState.cameraUi);

  if (state == STATE_PAUSE) {
    drawPauseScreen(&gameState.cameraUi);
  }

  return 0;
}

void gameTerminate() {
  worldTerminate(&gameState.world);
  ecsTerminate();
}
