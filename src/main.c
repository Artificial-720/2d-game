#include <stdio.h>

#include "math.h"
#include "window.h"   // window and input
#include "ecs.h"      // Entity Component System
#include "render2d.h" // OpenGL
#include "physics.h"  // handles 2d physics and collisions resolution
#include "camera.h"   // handles camera position and movement

#include "chunk.h"    // world tiles

#include "components.h" // ECS - the components
#include "systems.h"    // ECS - the systems definitions

window_t *window;
Render2d *renderer;
camera_t camera;
entity_t player;

int initialize() {
  int screenWidth = 1028;
  int screenHeight = 720;
  window = windowInit(screenWidth, screenHeight, "2d Game!");
  if(!window) {
    printf("Failed to create a window\n");
    return 1;
  }

  if (ecsInit()) {
    windowTerminate(window);
    return 1;
  }

  renderer = r2dInit();
  if (!renderer) {
    return 1;
  }

  float h = 30;
  float w = h * ((float)screenWidth / screenHeight);
  renderer->projection = orthographic(0, w, 0, h, 0, 1);
  camera = cameraInit();

  return 0;
}

void terminate() {
  windowTerminate(window);
  r2dTerminate(renderer);
  ecsTerminate();
}



void update(double deltatime) {
  // player input
  rigidbody_t *rb = (rigidbody_t*)ecsGetComponent(player, RIGIDBODY);
  if (getKey(window, KEY_A) == HELD) {
    rb->velocity.x = -2.0f;
  } else if (getKey(window, KEY_D) == HELD) {
    rb->velocity.x = 2.0f;
  }
  if (getKey(window, KEY_SPACE) == HELD) {
    if (rb->velocity.y < 0.01)
      rb->force.y = 500;
    // rb->force.y = 500;
    // rb->velocity.y = 50;
  }

  // game input toggle fullscreen
  if (getKey(window, KEY_F) == PRESS) {
    toggleFullScreen(window);
  }
  int width, height;
  if (updateWindowViewport(window, &width, &height)) {
    float h = 30.0f;
    float w = h * ((float)width / height);
    camera.width = width;
    camera.height = height;
    camera.projection = orthographic(
      -w / (2 * camera.zoomFactor), w / (2 * camera.zoomFactor),
      -h / (2 * camera.zoomFactor), h / (2 * camera.zoomFactor),
      -1, 1
    );
  }

  // click input
  // if (getMouseButton(window, MOUSE_BUTTON_LEFT) == PRESS) {
  //   double xpos, ypos;
  //   getCursorPos(window, &xpos, &ypos);
  //   vec4 worldPos = screenToWorld(&camera, xpos, ypos);
  //   placeTile(&chunk, worldPos.x, worldPos.y);
  // }
  // if (getMouseButton(window, MOUSE_BUTTON_RIGHT) == PRESS) {
  //   double xpos, ypos;
  //   getCursorPos(window, &xpos, &ypos);
  //   vec4 worldPos = screenToWorld(&camera, xpos, ypos);
  //   removeTile(&chunk, worldPos.x, worldPos.y);
  // }

  // physics(deltatime);

  ecsUpdate(deltatime);

  // Update camera position
  transform_t *tf = (transform_t*)ecsGetComponent(player, TRANSFORM);
  cameraUpdatePosition(&camera, tf->position.x, tf->position.y);
  // printf("player pos: %f %f\n", tf->position.x, tf->position.y);
}


void gameInit() {
  unsigned int textureId;
  r2dCreateTexture(renderer, "assets/image.png", &textureId);

  // Create our player entity
  player = ecsCreateEntity();
  Sprite sprite = {.x = 0, .y = 0, .width = 1, .height = 1, .texture = textureId};
  transform_t transform = {.position = (vec3){10.0f, 23.0f, 0}, .scale = (vec3){1.0f, 1.0f, 1.0f}};
  rigidbody_t rb = {.velocity = (vec3){0, 0, 0}, .force = (vec3){0, 0, 0}, .mass = 1.0f};
  collider_t collider = {.offset = (vec3){0, 0, 0}, .radius = 0.5};
  float gravity = 9.81f;
  ecsAddComponent(player, SPRITE, (void*)&sprite);
  ecsAddComponent(player, RIGIDBODY, (void*)&rb);
  ecsAddComponent(player, TRANSFORM, (void*)&transform);
  ecsAddComponent(player, GRAVITY, (void*)&gravity);
  ecsAddComponent(player, COLLIDER, (void*)&collider);

  // load first chunk
  entity_t chunk = ecsCreateEntity();
  ecsAddComponent(chunk, CHUNK, (void*)0);
  chunk_t *chunkData = ecsGetComponent(chunk, CHUNK);
  chunkData->x = 0;
  chunkData->y = 0;
  for (int i = 0; i < CHUNK_WIDTH * CHUNK_HEIGHT; i++) chunkData->tiles[i] = (tile_t){.id = 0};
  for (int i = 0; i < CHUNK_WIDTH; i++) {
    placeTile(chunkData, i, 20);
  }

}


void scrollCallback(window_t *window, double xoffset, double yoffset) {
  (void)window;
  (void)xoffset;
  float newZoom = camera.zoomFactor + yoffset / 10;
  camera.zoomFactor = clamp(newZoom, 1.0f, 3.0f);
}

int main() {
  if (initialize()) {
    terminate();
    return 0;
  }

  windowSetScrollCallback(window, scrollCallback);

  ecsRegisterComponent(SPRITE, sizeof(Sprite));
  ecsRegisterComponent(RIGIDBODY, sizeof(rigidbody_t));
  ecsRegisterComponent(TRANSFORM, sizeof(transform_t));
  ecsRegisterComponent(COLLIDER, sizeof(collider_t));
  ecsRegisterComponent(GRAVITY, sizeof(float));
  ecsRegisterComponent(CHUNK, sizeof(chunk_t));
  ecsRegisterSystem(ecsGetSignature(TRANSFORM) | ecsGetSignature(RIGIDBODY) | ecsGetSignature(GRAVITY), physicsSystem);
  ecsRegisterSystem(ecsGetSignature(TRANSFORM) | ecsGetSignature(SPRITE), renderSystem);
  ecsRegisterSystem(ecsGetSignature(CHUNK), chunkLoadSystem);
  ecsRegisterSystem(ecsGetSignature(CHUNK), chunkUpdateSystem);

  gameInit();

  double previousTime = getTime();
  while(!windowShouldClose(window)) {
    double now = getTime();
    double deltatime = now - previousTime;
    previousTime = now;

    pollInput();
    r2dClear();
    update(deltatime);
    windowSwapBuffers(window);
  }

  terminate();

  return 0;
}
