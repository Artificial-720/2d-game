#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include "render2d.h"
#include "controls.h"

#define WORLD_WIDTH 1750
#define WORLD_HEIGHT 900


typedef struct {
  Sprite *sprites;
  int spriteCount;
} GameState;

GameState gameState;
Render2d *render;
float zoomFactor = 1.0f;
float velocity = 1.0f; // temp test variable REMOVE LATER


void initGame() {
  int buffSize = 100;
  gameState.spriteCount = 0;
  gameState.sprites = (Sprite*)malloc(buffSize * sizeof(Sprite));
  if (!gameState.sprites) {
    // um idk what todo if cant load sprites
    return;
  }

  unsigned int textureDirt;
  r2dCreateTexture(render, "assets/image.png", &textureDirt);

  Sprite sprite = createSprite(0, 0, 50, 50, 0, textureDirt);
  gameState.sprites[gameState.spriteCount] = sprite;
  gameState.spriteCount++;

  float xpos = 0;
  for (int i = 0; i < 50; i++) {
    Sprite sprite = createSprite(xpos, -50, 50, 50, 0, textureDirt);
    gameState.sprites[gameState.spriteCount] = sprite;
    gameState.spriteCount++;
    xpos += 50;
  }


}

void update(double deltatime) {
  (void)deltatime;

  gameState.sprites[0].x += velocity;
}


void draw(GLFWwindow *window) {
  r2dClear();

  for (int i = 0; i < gameState.spriteCount; i++) {
    r2dDrawSprite(render, gameState.sprites[i]);
  }

  glfwSwapBuffers(window);
}

void userInput(GLFWwindow *window) {
  int keyState = 0;

  keyState = glfwGetKey(window, ESC_KEY);
  if (keyState == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  // keyState = glfwGetKey(window, GLFW_KEY_W);
  int keyStateA = glfwGetKey(window, GLFW_KEY_A);
  // keyState = glfwGetKey(window, GLFW_KEY_S);
  int keyStateD = glfwGetKey(window, GLFW_KEY_D);

  if (keyStateA == GLFW_PRESS) {
    velocity = -1.0f;
  } else if (keyStateD == GLFW_PRESS) {
    velocity = 1.0f;
  } else {
    velocity = 0.0f;
  }
}


void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  (void)window;
  (void)xoffset;
  zoomFactor += yoffset / 10;
  zoomFactor = clamp(zoomFactor, 0.5f, 3.0f);
  render->view = mat4Init(1.0f);
  render->view = scale(render->view, (vec3){zoomFactor, zoomFactor, 0});
  render->view = translate(render->view, (vec3){1028 / 2.0f, 720 / 2.0f, 0.0f});
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
  glfwSetScrollCallback(window, scrollCallback);

  render = r2dInit();
  if (!render) {
    cleanupWindow(window);
    return 0;
  }
  render->projection = orthographic(0, screenWidth, 0, screenHeight, 0, 1);

  render->view = translate(render->view, (vec3){screenWidth / 2.0f, screenHeight / 2.0f, 0.0f});

  initGame();

  double previousTime = glfwGetTime();
  while(!glfwWindowShouldClose(window)) {
    double now = glfwGetTime();
    double deltatime = now - previousTime;
    previousTime = now;

    glfwPollEvents();
    int viewportChanged = updateWindowViewport(window, &screenWidth, &screenHeight);

    (void)viewportChanged;

    userInput(window);
    update(deltatime);
    draw(window);
  }

  r2dTerminate(render);
  cleanupWindow(window);

  return 0;
}
