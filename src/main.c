
#include <stdio.h>

#include "window.h"

#include "ecs.h"


void gameInit() {

  unsigned int box = ecsCreateEntity();

  unsigned int textureId;
  ecsLoadTexture("assets/image.png", &textureId);
  // add a sprite
  Sprite sprite = {.x = 0, .y = 0, .width = 50, .height = 50, .texture = textureId};
  ecsAddComponentSprite(box, sprite);
  Transform transform = {.position = (vec3){0, 50, 0}};
  ecsAddComponentTransform(box, transform);
  Rigidbody rb = {.velocity = (vec3){0, 0, 0}};
  ecsAddComponentRigidbody(box, rb);

  sprite.x = 100;
  Entity box2 = ecsCreateEntity();
  ecsAddComponentSprite(box2, sprite);
  transform.position.x = 100;
  ecsAddComponentTransform(box2, transform);
  ecsAddComponentRigidbody(box2, rb);

}


int main() {
  int screenWidth = 1028;
  int screenHeight = 720;
  GLFWwindow *window;
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
