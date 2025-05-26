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
static const double fixedDelta = 1.0f / 60.0f;
static double accumulatedPhysics;

animationState_e playerAnimation(entity_t entity, int *facingLeft) {
  (void)entity;
  *facingLeft = gameState.player.facingLeft;
  switch (gameState.player.state) {
    case PLAYER_IDLE:         return ANIM_IDLE;
    case PLAYER_JUMPING:      return ANIM_JUMP_UP;
    case PLAYER_FALLING:      return ANIM_JUMP_DOWN;
    case PLAYER_HURT:         return ANIM_HURT;
    case PLAYER_DEAD:         return ANIM_DEATH;
    case PLAYER_WALKING:      return ANIM_WALK;
    case PLAYER_USE:          return ANIM_USE;
    case PLAYER_ATTACKING:    return ANIM_ATTACK;
    default:              return ANIM_IDLE;
  }
}

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
    0, 0, 0, 0, 7, 96, 32, 0.3f);
  ani.animations[ANIM_JUMP_START] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Start-up.png"),
    0, 0, 0, 0, 7, 96, 32, 0.214f);
  ani.animations[ANIM_JUMP_UP] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Up.png"),
    0, 0, 0, 0, 1, 96, 32, 0.3f);
  ani.animations[ANIM_JUMP_FALL] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump to Fall.png"),
    0, 0, 0, 0, 5, 96, 32, 0.1f);
  ani.animations[ANIM_JUMP_DOWN] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Down.png"),
    0, 0, 0, 0, 1, 96, 32, 0.3f);
  ani.animations[ANIM_JUMP_LAND] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Jump/Sprite Sheet - Green Jump Land.png"),
    0, 0, 0, 0, 6, 96, 32, 0.3f);
  ani.animations[ANIM_DEATH] = createAnimation(
    loadTexture("assets/slime/Slime Enemy/Death/Sprite Sheet - Green Death.png"),
    0, 0, 0, 0, 14, 96, 32, 0.2f);

  // ANIM_HURT,
  // ANIM_DEATH,
  // ANIM_COUNT

  ecsAddComponent(slime, ANIMATION, (void*)&ani);
  ecsAddComponent(slime, COOLDOWN, (void*)&c);

  health_t health = {.max = 100, .value = 100};
  ecsAddComponent(slime, HEALTH, (void*)&health);
}



int gameInit() {
  accumulated = 0.0f;
  accumulatedPhysics = 0.0f;
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
  ecsRegisterComponent(COMBAT, sizeof(combat_t));
  ecsRegisterComponent(LIFETIME, sizeof(lifetime_t));
  ecsRegisterComponent(HEALTH, sizeof(health_t));

  gameState.player.pickupDis = 3.0f;

  // Create our player entity
  int width = 2;
  int height = 2;
  texture_t texture = loadTexture("assets/player/spritesheet.png");
  entity_t player = ecsCreateEntity();
  sprite_t sprite = createSprite(10, 10, width, height, 0, texture.id);
  transform_t transform = {.pos = (vec2){10, 10}};
  physics_t p = {.body = 0, .isStatic = 0};
  p.body = createBody((vec2){PLAYER_START_X, PLAYER_START_Y}, (vec2){width, height});

  animationComponent_t ani = {0};
  ani.callback = playerAnimation;
  ani.animations[ANIM_IDLE] = createAnimation(texture, 0, 1152, 37, 0, 6, 128, 40, 0.3f);
  ani.animations[ANIM_JUMP_UP] = createAnimation(texture, 0, 896, 37, 0, 3, 128, 40, 0.3f);
  ani.animations[ANIM_JUMP_DOWN] = createAnimation(texture, 0, 768, 37, 0, 5, 128, 40, 0.3f);
  ani.animations[ANIM_WALK] = createAnimation(texture, 0, 1024, 37, 0, 8, 128, 40, 0.1f);
  ani.animations[ANIM_USE] = createAnimation(texture, 0, 640, 37, 0, 10, 128, 40, 0.1f);
  ani.animations[ANIM_ATTACK] = createAnimation(texture, 0, 512, 37, 0, 5, 128, 40, 0.1f);
  ecsAddComponent(player, ANIMATION, (void*)&ani);

  ecsAddComponent(player, SPRITE, (void*)&sprite);
  ecsAddComponent(player, TRANSFORM, (void*)&transform);
  ecsAddComponent(player, PHYSICS, (void*)&p);

  combat_t combat = {0};
  combat.attackTime = 0.5f;
  ecsAddComponent(player, COMBAT, (void*)&combat);

  gameState.player.entity = player;

  // Setup UI entities
  setupHud();

  // populate default inventory
  giveItemToPlayer(&gameState.player, ITEM_PICKAXE, 1);
  giveItemToPlayer(&gameState.player, ITEM_AXE, 1);
  giveItemToPlayer(&gameState.player, ITEM_SWORD, 1);
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
    refreshPhysicsEntities(&gameState.camera, &gameState.world);

    accumulatedPhysics += dt;
    while (accumulatedPhysics > fixedDelta) {
      // update physics
      physicsSystem(fixedDelta);
      triggerSystem();
      accumulatedPhysics -= fixedDelta;
    }
    aiSystem(dt);
    lifetimeSystem(dt);
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
