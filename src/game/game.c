#include "game.h"

#include "ecs.h"
#include "physics.h"
#include "world.h"
#include "components.h"
#include "systems.h"
#include "camera.h"
#include "texture.h"
#include "../platform/sprite.h"

#define PLAYER_START_X 10
#define PLAYER_START_Y 100
#define TICK_RATE (1.0f / 1.0f)

typedef struct {
  camera_t camera;
  entity_t player;
  world_t *world;
} gameState_t;

static gameState_t gameState;
static double accumulated;


int gameInit() {
  accumulated = 0.0f;

  ecsInit();

  // register our ecs components
  ecsRegisterComponent(SPRITE, sizeof(sprite_t));
  ecsRegisterComponent(TRANSFORM, sizeof(transform_t));
  ecsRegisterComponent(PHYSICS, sizeof(physics_t));
  ecsRegisterComponent(UI, sizeof(ui_t));

  // load our assets we need
  unsigned int texture = loadTexture("assets/image.png");

  // Create our player entity
  entity_t player = ecsCreateEntity();
  sprite_t sprite = createSprite(10, 10, 1, 1, 0, texture);
  transform_t transform = {.pos = (vec2){10, 10}};
  physics_t p = {.body = 0, .isStatic = 0};
  p.body = createBody((vec2){5, 15}, (vec2){1.0f, 1.0f});
  ecsAddComponent(player, SPRITE, (void*)&sprite);
  ecsAddComponent(player, TRANSFORM, (void*)&transform);
  ecsAddComponent(player, PHYSICS, (void*)&p);
  gameState.player = player;

  // Setup UI entities
  unsigned int textureBar = loadTexture("assets/tile_0003.png");
  int size = 35;
  sprite_t barSprite = createSprite(0, 0, size, -size, 0, textureBar);
  for (int i = 0; i < 10; i++) {
    entity_t bar = ecsCreateEntity();
    ui_t barUi = {.pos = {10 + ((size + 2) * i), 10.0f}};
    ecsAddComponent(bar, SPRITE, (void*)&barSprite);
    ecsAddComponent(bar, UI, (void*)&barUi);
  }
  unsigned int heartTexture = loadTexture("assets/heart.png");
  sprite_t heartSprite = createSprite(0, 0, size, -(size / (375.0f / 325.0f)), 0, heartTexture);
  for (int i = 0; i < 10; i++) {
    entity_t heart = ecsCreateEntity();
    ui_t heartUi = {.pos = {500 + ((size + 2) * i), 10.f}};
    ecsAddComponent(heart, SPRITE, (void*)&heartSprite);
    ecsAddComponent(heart, UI, (void*)&heartUi);
  }


  // Create our world and load the area around the player
  gameState.world = worldInit(WORLD_WIDTH, WORLD_HEIGHT);
  worldGenerate(gameState.world, 10);
  // load in initial world, load the tiles around the player
  refreshWorld(gameState.world, gameState.camera.pos.x);


  // create a test world
  // unsigned int floorTexture = loadTexture("assets/image.png");
  // entity_t floor = ecsCreateEntity();
  // sprite_t floorSprite = {.x = 1, .y = 1, .width = 20, .height = 1, .texture = floorTexture};
  // transform_t floorTransform = {.pos = (vec2){1, 1}};
  // physics_t floorPhysics = {.body = 0, .isStatic = 1};
  // p.body = createStaticBody((vec2){1, 1}, (vec2){20.0f, 1.0f});
  // ecsAddComponent(floor, SPRITE, (void*)&floorSprite);
  // ecsAddComponent(floor, TRANSFORM, (void*)&floorTransform);
  // ecsAddComponent(floor, PHYSICS, (void*)&floorPhysics);

  // entity_t wall = ecsCreateEntity();
  // sprite_t wallSprite = {.x = 21, .y = 11, .width = 1, .height = 10, .texture = floorTexture};
  // transform_t wallTransform = {.pos = (vec2){21, 11}};
  // physics_t wallPhysics = {.body = 0, .isStatic = 1};
  // p.body = createStaticBody((vec2){21, 11}, (vec2){1.0f, 10.0f});
  // ecsAddComponent(wall, SPRITE, (void*)&wallSprite);
  // ecsAddComponent(wall, PHYSICS, (void*)&wallPhysics);
  // ecsAddComponent(wall, TRANSFORM, (void*)&wallTransform);

  // entity_t box = ecsCreateEntity();
  // sprite_t boxSprite = {.x = 5, .y = 4, .width = 2, .height = 2, .texture = floorTexture};
  // transform_t boxTransform = {.pos = (vec2){5, 4}};
  // physics_t boxPhysics = {.body = 0, .isStatic = 1};
  // p.body = createStaticBody((vec2){5, 4}, (vec2){2.0f, 2.0f});
  // ecsAddComponent(box, SPRITE, (void*)&boxSprite);
  // ecsAddComponent(box, PHYSICS, (void*)&boxPhysics);
  // ecsAddComponent(box, TRANSFORM, (void*)&boxTransform);

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

  // player movement
  inputSystem(gameState.player, input, &gameState.camera, gameState.world);

  // update physics
  physicsSystem(dt);

  // render
  cameraSystem(&gameState.camera, gameState.player, input);
  uiSystem(&gameState.camera, input);
  spriteSystem();

  // load world
  // worldLoadTiles(&gameState.world, tf->pos.x);
  // worldUnloadTiles(&gameState.world, tf->pos.x);

  // random tick
  // randomTick(gameState.world, dt);
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
