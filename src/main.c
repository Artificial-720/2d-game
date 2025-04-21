
#include <stdio.h>

#include "window.h"

#include "ecs.h"


void gameInit() {

  unsigned int box = ecsCreateEntity();

  unsigned int textureId;
  ecsLoadTexture("assets/image.png", &textureId);
  // add a sprite
  Sprite sprite = {.x = 0, .y = 0, .width = 50, .height = 50, .texture = textureId};
  ecsAddComponetSprite(box, sprite);
  Transform transform = {.position = (vec3){0, 50, 0}};
  ecsAddComponetTransform(box, transform);

  sprite.x = 100;
  ecsAddComponetSprite(ecsCreateEntity(), sprite);
  transform.position.x = 100;
  ecsAddComponetTransform(box, transform);

}


int main() {
  // init ecs
  // while running
  // update
  // terminate ecs

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

  gameInit();


  double previousTime = glfwGetTime();
  while(!glfwWindowShouldClose(window)) {
    double now = glfwGetTime();
    double deltatime = now - previousTime;
    previousTime = now;
    printf("fps: %f\n", 1.0f / deltatime); // not perfect but works for now

    glfwPollEvents();


    // update for each system

    // user input
    // update game
    // render

    ecsUpdate(deltatime);

    ecsRender();

    glfwSwapBuffers(window);
  }

  ecsTerminate();
  cleanupWindow(window);

  return 0;
}
