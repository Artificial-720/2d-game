
#include <stdio.h>

#include "window.h"

#include "ecs.h"


void gameInit() {
  unsigned int textureId;
  ecsLoadTexture("assets/image.png", &textureId);

  // create 10 boxes in a row for "ground"
  for (int i = 0; i < 10; i++) {
    Entity box = ecsCreateEntity();
    Sprite sprite = {.x = 0, .y = 0, .width = 50, .height = 50, .texture = textureId};
    Transform transform = {.position = (vec3){50 * i, 50, 0}};
    // Rigidbody rb = {.velocity = (vec3){0, 0, 0}};

    ecsAddComponentSprite(box, sprite);
    ecsAddComponentTransform(box, transform);
    // ecsAddComponentRigidbody(box, rb);
  }

  Entity player = ecsCreateEntity();
  Sprite sprite = {.x = 0, .y = 0, .width = 50, .height = 50, .texture = textureId};
  Transform transform = {.position = (vec3){50, 150, 0}};
  Rigidbody rb = {.velocity = (vec3){0, 0, 0}, .acceleration = (vec3){0, 0, 0}};
  ecsAddComponentSprite(player, sprite);
  ecsAddComponentTransform(player, transform);
  ecsAddComponentRigidbody(player, rb);
  ecsAddComponentController(player);
}


void playerController(Entity player, double deltatime) {
  printf("hi from controller\n");
  (void)deltatime;
  Transform *transform = ecsGetTransform(player);
  Rigidbody *rb = ecsGetRigidbody(player);

  // TODO collisions
  if (transform->position.y < 101) {
    rb->velocity.y = 0;
    rb->acceleration.y = 0;
  }

  int keyA = ecsGetKey(ECS_KEY_A);
  int keyD = ecsGetKey(ECS_KEY_D);
  if (keyA == PRESS) {
    rb->velocity.x = -50.0f;
  } else if (keyD == PRESS) {
    rb->velocity.x = 50.0f;
  } else {
    rb->velocity.x = 0.0f;
  }

  if (ecsGetKey(ECS_KEY_SPACE) == PRESS && transform->position.y < 101) {
    rb->velocity.y = 100.0f;
  }

}


GLFWwindow *window;
int main() {
  int screenWidth = 1028;
  int screenHeight = 720;
  window = initWindow(screenWidth, screenHeight, "2d Render!");
  if(!window) {
    printf("Failed to create a window\n");
    return 0;
  }

  if (ecsInit()) {
    cleanupWindow(window);
    return 0;
  }

  unsigned int sig = ecsGetSignature(TRANSFORM) | ecsGetSignature(RIGIDBODY);
  ecsRegisterSystem(sig, ecsPhysics);
  sig = ecsGetSignature(CONTROLLER);
  ecsRegisterSystem(sig, playerController);

  gameInit();


  double previousTime = glfwGetTime();
  while(!glfwWindowShouldClose(window)) {
    double now = glfwGetTime();
    double deltatime = now - previousTime;
    previousTime = now;
    // printf("fps: %f\n", 1.0f / deltatime); // not perfect but works for now


    // user input
    glfwPollEvents();

    ecsUpdate(deltatime);
    ecsRender();

    glfwSwapBuffers(window);
  }

  ecsTerminate();
  cleanupWindow(window);

  return 0;
}
