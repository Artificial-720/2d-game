#include "game.h"

#include "animation.h"
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
#define PLAYER_START_Y 75
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

void spawnSlime(entity_t target) {
  float width = 6.0f;
  float height = 2.0f;



  texture_t texture = loadTexture("assets/image.png");
  entity_t slime = ecsCreateEntity();
  sprite_t sprite = createSprite(0, 0, width, height, 0, texture.id);
  transform_t transform = {.pos = (vec2){10.0f, 100.0f}};
  physics_t p = {.body = 0, .isStatic = 0};
  p.body = createBody((vec2){5, 80}, (vec2){width, height});
  ai_t ai = {.target = target, .agroDis = 50.0f};
  ecsAddComponent(slime, SPRITE, (void*)&sprite);
  ecsAddComponent(slime, TRANSFORM, (void*)&transform);
  ecsAddComponent(slime, PHYSICS, (void*)&p);
  ecsAddComponent(slime, AI, (void*)&ai);


  cooldown_t c = {0};
  animationComponent_t ani = {0};
  ani.callback = slimeAnimation;
  ani.animations[ANIM_IDLE] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Idle/Sprite Sheet - Green Idle.png"),
    0, 0, 7, 96, 32, 0.3f);
  ani.animations[ANIM_JUMP_START] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Start-up.png"),
    0, 0, 7, 96, 32, 0.3f);
  ani.animations[ANIM_JUMP_UP] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Up.png"),
    0, 0, 1, 96, 32, 0.3f);
  ani.animations[ANIM_JUMP_FALL] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump to Fall.png"),
    0, 0, 5, 96, 32, 0.1f);
  ani.animations[ANIM_JUMP_DOWN] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Down.png"),
    0, 0, 1, 96, 32, 0.3f);
  ani.animations[ANIM_JUMP_LAND] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Land.png"),
    0, 0, 6, 96, 32, 0.3f);

  // ANIM_HURT,
  // ANIM_DEATH,
  // ANIM_COUNT

  ecsAddComponent(slime, ANIMATION, (void*)&ani);
  ecsAddComponent(slime, COOLDOWN, (void*)&c);



  (void)target;
}



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
  ecsRegisterComponent(ANIMATION, sizeof(animationComponent_t));
  ecsRegisterComponent(AI, sizeof(ai_t));
  ecsRegisterComponent(COOLDOWN, sizeof(cooldown_t));

  gameState.player.pickupDis = 3.0f;

  // Create our player entity
  unsigned int texture = loadTexture("assets/image.png").id;
  entity_t player = ecsCreateEntity();
  sprite_t sprite = createSprite(10, 10, 1.5, 3, 0, texture);
  transform_t transform = {.pos = (vec2){10, 10}};
  physics_t p = {.body = 0, .isStatic = 0};
  p.body = createBody((vec2){PLAYER_START_X, PLAYER_START_Y}, (vec2){1.5f, 3.0f});
  // animation_t animation = {0};
  // animation.frameTime = 0.5f;
  // animation.texture = loadTexture("assets/player.png");
  // for (int i = 0; i < 7; i++) {
  //   animation.frames[i] = (vec4){(32 * i), 576, 32, 48};
  // }
  // animation.totalFrames = 7;

  // animationComponent_t ani = {0};
  // ani.animations[ANIM_IDLE] = createAnimation(
  //   loadTexture("assets/player.png"),
  //   0, 576, 7, 32, 48, 0.3f);
  // ecsAddComponent(player, ANIMATION, (void*)&ani);



  ecsAddComponent(player, SPRITE, (void*)&sprite);
  ecsAddComponent(player, TRANSFORM, (void*)&transform);
  ecsAddComponent(player, PHYSICS, (void*)&p);
  // ecsAddComponent(player, ANIMATION, (void*)&animation);
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

  spawnSlime(player);

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
    cooldownSystem(dt);
    aiSystem(dt);
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
