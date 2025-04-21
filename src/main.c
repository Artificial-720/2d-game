#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "window.h"
#include "render2d.h"
#include "controls.h"
#include "camera.h"

#define WORLD_WIDTH 20
#define WORLD_HEIGHT 10
#define TILE_SIZE 50
#define PLAYER_MOVEMENT_SPEED 150

typedef struct {
  int id;
} Tile;

typedef struct {
  Sprite *sprites;
  int spriteCount;

  // tiles -> block ids in a array
  // entities -> entities in a array

  Tile *tiles;
  int tileCount;

  Sprite player;


} GameState;

GameState gameState;
Render2d *render;
float zoomFactor = 1.0f;
float velocity = 1.0f; // temp test variable REMOVE LATER



void initGame() {
  gameState.sprites = 0;
  gameState.spriteCount = 0;
  gameState.tiles = 0;
  gameState.tileCount = 0;

  gameState.player = (Sprite) {.x = 5, .y = 500, .width = 50, .height = 50, .rotation = 0, .texture = 1};


  // load tiles
  // create array of tiles this is the world
  // populate the world
  int tileSize = WORLD_WIDTH * WORLD_HEIGHT;
  Tile *tiles;
  tiles = (Tile*)malloc(tileSize * sizeof(Tile));
  for (int i = 0; i < tileSize; i++) {
    tiles[i] = (Tile){.id = (i >= tileSize / 2)? 0: 1};
  }
  gameState.tiles = tiles;
  gameState.tileCount = tileSize;

  unsigned int textureDirt;
  r2dCreateTexture(render, "assets/image.png", &textureDirt);
}

float yvelocity = 0.0f;
void update(double deltatime) {
  (void)deltatime;
  gameState.player.x += deltatime * (PLAYER_MOVEMENT_SPEED * velocity);

  // gravity
  yvelocity += -(500) * deltatime;
  gameState.player.y += deltatime * (yvelocity);
  if (gameState.player.y < 255) {
    yvelocity = 0;
  }
  gameState.player.y = clamp(gameState.player.y, 250, 5000);
}


void draw(GLFWwindow *window) {
  r2dClear();
  // draw background
  // draw tiles
  //
  for (int i = 0; i < gameState.tileCount; i++) {
    if (gameState.tiles[i].id) {
      int x = (i % WORLD_WIDTH) * TILE_SIZE;
      int y = (i / WORLD_WIDTH) * TILE_SIZE;
      r2dDrawSprite(render, (Sprite){x, y, TILE_SIZE, TILE_SIZE, 0, 1});
    }
  }
  // draw entities
  // draw ui

  for (int i = 0; i < gameState.spriteCount; i++) {
    r2dDrawSprite(render, gameState.sprites[i]);
  }

  r2dDrawSprite(render, gameState.player);

  glfwSwapBuffers(window);
}

void userInput(GLFWwindow *window) {
  int keyState = 0;

  keyState = glfwGetKey(window, ESC_KEY);
  if (keyState == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  keyState = glfwGetKey(window, GLFW_KEY_SPACE);
  if (keyState == GLFW_PRESS) {
    yvelocity += 75;
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

  int mouseKey = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
  if (mouseKey == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    printf("mouse click at %f %f\n", xpos, ypos);


    // projection * view
    // inverse of pv
    // times that by vec4(xpos, ypos, 0, 1.0)
    // mat4 pv = multiply(render->projection, render->view);
    // mat4 inversePV = inverse(pv);

  }
}


void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  (void)window;
  (void)xoffset;
  zoomFactor += yoffset / 10;
  zoomFactor = clamp(zoomFactor, 0.5f, 3.0f);
  // render->view = mat4Init(1.0f);
  // render->view = scale(render->view, (vec3){zoomFactor, zoomFactor, 0});
  // render->view = translate(render->view, (vec3){1028 / 2.0f, 720 / 2.0f, 0.0f});
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

  Camera camera = initCamera();

  double previousTime = glfwGetTime();
  while(!glfwWindowShouldClose(window)) {
    double now = glfwGetTime();
    double deltatime = now - previousTime;
    previousTime = now;
    printf("fps: %f\n", 1.0f / deltatime); // not perfect but works for now

    glfwPollEvents();
    int viewportChanged = updateWindowViewport(window, &screenWidth, &screenHeight);

    (void)viewportChanged;

    userInput(window);
    update(deltatime);

    // update camera position
    // setPosition(camera, player.position + offset);
    // update render to match, view and projection
    cameraSetPosition(&camera, screenWidth / 2.0f - gameState.player.x, screenHeight / 2.0f - gameState.player.y);
    // printf("camera pos %f %f\n", camera.x, camera.y);
    render->view = mat4Init(1.0f);
    render->view = scale(render->view, (vec3){zoomFactor, zoomFactor, 0});
    render->view = translate(render->view, (vec3){camera.x, camera.y, 0.0f});

    draw(window);
  }

  r2dTerminate(render);
  cleanupWindow(window);

  return 0;
}
