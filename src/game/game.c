#include "game.h"

#include "ecs.h"
#include "physics.h"
#include "world.h"
#include "components.h"
#include "systems.h"
#include "camera.h"
#include "../platform/renderer2d.h"

#include <stdio.h>

#define PLAYER_START_X 10
#define PLAYER_START_Y 10

typedef struct {
  camera_t camera;
  entity_t player;
  world_t world;
} gameState_t;

static gameState_t gameState;

int gameInit() {
  ecsInit();

  // register our ecs components
  ecsRegisterComponent(SPRITE, sizeof(sprite_t));
  ecsRegisterComponent(RIGIDBODY, sizeof(rigidbody_t));
  ecsRegisterComponent(TRANSFORM, sizeof(transform_t));
  ecsRegisterComponent(COLLIDER, sizeof(collider_t));
  ecsRegisterComponent(GRAVITY, sizeof(float));

  unsigned int textureId = 1;
  r2dCreateTexture("assets/image.png", &textureId);

  // Create our player entity
  entity_t player = ecsCreateEntity();
  sprite_t sprite = {.x = 0, .y = 0, .width = 1, .height = 1, .texture = textureId};
  transform_t transform = {.position = (vec3){PLAYER_START_X, PLAYER_START_Y, 0}, .scale = (vec3){1.0f, 1.0f, 1.0f}};
  rigidbody_t rb = {.velocity = (vec3){0, 0, 0}, .force = (vec3){0, 0, 0}, .mass = 1.0f};
  collider_t collider = {.offset = (vec3){0, 0, 0}, .radius = 0.5};
  float gravity = 9.81f;
  ecsAddComponent(player, SPRITE, (void*)&sprite);
  ecsAddComponent(player, RIGIDBODY, (void*)&rb);
  ecsAddComponent(player, TRANSFORM, (void*)&transform);
  ecsAddComponent(player, GRAVITY, (void*)&gravity);
  ecsAddComponent(player, COLLIDER, (void*)&collider);
  gameState.player = player;

  // Create our world and load the area around the player
  gameState.world = worldGenerate();
  worldLoadTiles(&gameState.world, transform.position.x);

  // Setup Camera
  gameState.camera = cameraInit();

  return 0;
}

int gameFrame(double dt) {
  transform_t *tf = (transform_t*)ecsGetComponent(gameState.player, TRANSFORM);

  // update physics
  physicsSystem(dt);

  // render
  float h = 30;
  float w = h * ((float)1028 / 720);
  gameState.camera.width = 1028;
  gameState.camera.height = 720;
  gameState.camera.projection = orthographic(
    -w / (2 * gameState.camera.zoomFactor), w / (2 * gameState.camera.zoomFactor),
    -h / (2 * gameState.camera.zoomFactor), h / (2 * gameState.camera.zoomFactor),
    -1, 1
  );
  cameraUpdatePosition(&gameState.camera, tf->position.x, tf->position.y);
  r2dSetView(gameState.camera.view);
  r2dSetProjection(gameState.camera.projection);
  spriteSystem();


  // load world
  worldLoadTiles(&gameState.world, tf->position.x);
  worldUnloadTiles(&gameState.world, tf->position.x);

  return 0;
}

void gameTerminate() {
  worldTerminate(&gameState.world);
  ecsTerminate();
}
