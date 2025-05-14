#include "game.h"

#include "ecs.h"
#include "item.h"
#include "physics.h"
#include "world.h"
#include "components.h"
#include "systems.h"
#include "camera.h"
#include "texture.h"
#include "ui.h"
#include "../platform/renderer2d.h"
#include "../platform/sprite.h"
#include "player.h"

#define PLAYER_START_X 10
#define PLAYER_START_Y 100
#define TICK_RATE (1.0f / 1.0f)

typedef struct {
  camera_t camera;
  world_t *world;
  player_t player;
} gameState_t;

static gameState_t gameState;
static double accumulated;

int gameInit() {
  accumulated = 0.0f;
  // Set sky color
  r2dSetClearColorRGBA(130.0f, 190.0f, 253.0f, 1.0f);

  ecsInit();

  // register our ecs components
  ecsRegisterComponent(SPRITE, sizeof(sprite_t));
  ecsRegisterComponent(TRANSFORM, sizeof(transform_t));
  ecsRegisterComponent(PHYSICS, sizeof(physics_t));
  ecsRegisterComponent(UI, sizeof(ui_t));
  ecsRegisterComponent(PICKUP, sizeof(pickup_t));

  gameState.player.pickupDis = 1.5f;

  // Create our player entity
  unsigned int texture = loadTexture("assets/image.png");
  entity_t player = ecsCreateEntity();
  sprite_t sprite = createSprite(10, 10, 1, 1, 0, texture);
  transform_t transform = {.pos = (vec2){10, 10}};
  physics_t p = {.body = 0, .isStatic = 0};
  p.body = createBody((vec2){5, 15}, (vec2){1.0f, 1.0f});
  ecsAddComponent(player, SPRITE, (void*)&sprite);
  ecsAddComponent(player, TRANSFORM, (void*)&transform);
  ecsAddComponent(player, PHYSICS, (void*)&p);
  gameState.player.entity = player;

  // Setup UI entities
  setupHud();

  // populate default inventory
  giveItemToPlayer(&gameState.player, ITEM_PICKAXE, 1);
  giveItemToPlayer(&gameState.player, ITEM_AXE, 1);
  giveItemToPlayer(&gameState.player, ITEM_DIRT, 9);
  giveItemToPlayer(&gameState.player, ITEM_GRASS, 9);
  giveItemToPlayer(&gameState.player, ITEM_SEED, 9);
  gameState.player.selected = 0;

  // Create our world and load the area around the player
  gameState.world = worldInit(WORLD_WIDTH, WORLD_HEIGHT);
  worldGenerate(gameState.world, 10);
  // load in initial world, load the tiles around the player
  refreshWorld(gameState.world, gameState.camera.pos.x);

  // Setup Camera
  gameState.camera = cameraInit();

  return 0;
}

int gameFrame(double dt, input_t *input, output_t *output) {
  accumulated += dt;

  // full screen toggle
  if (input->keyStates[KEY_F] == KEY_PRESS) {
    output->toggleFullScreen = 1;
  }

  // player input
  inputSystem(&gameState.player, &gameState.camera, gameState.world, input);

  // update physics
  physicsSystem(dt);

  // pick up tiles near player
  pickupItems(&gameState.player);

  // render
  cameraSystem(&gameState.camera, gameState.player.entity, input);
  drawBackground(gameState.world, gameState.camera.pos.x);
  drawEntities();
  drawForeground(gameState.world, gameState.camera.pos.x);
  drawHud(&gameState.player, &gameState.camera, input);

  if (accumulated > TICK_RATE) {
    growVegetation(gameState.world);
    accumulated -= TICK_RATE;
  }
  refreshWorld(gameState.world, gameState.camera.pos.x);

  return 0;
}

void gameTerminate() {
  worldTerminate(gameState.world);
  ecsTerminate();
}
