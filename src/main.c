#include <stdio.h>

#include "game/game.h"
#include "platform/window.h"
#include "platform/renderer2d.h"

window_t *window;

int main() {
  int screenWidth = 1028;
  int screenHeight = 720;
  window = windowInit(screenWidth, screenHeight, "2d Game!");
  if(!window) {
    printf("Failed to create a window\n");
    return 1;
  }

  if (r2dInit()) {
    windowTerminate(window);
    return 1;
  }

  if (gameInit()) {
    windowTerminate(window);
    r2dTerminate();
    return 1;
  }

  double previousTime = getTime();
  while(!windowShouldClose(window)) {
    double now = getTime();
    double deltatime = now - previousTime;
    previousTime = now;

    pollInput();
    r2dClear();

    gameFrame(deltatime);

    windowSwapBuffers(window);
  }

  windowTerminate(window);
  r2dTerminate();

  gameTerminate();

  return 0;
}
