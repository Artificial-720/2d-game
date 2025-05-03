#include <stdio.h>

#include "platform/input.h"
#include "platform/output.h"
#include "platform/window.h"
#include "platform/renderer2d.h"
#include "game/game.h"


int main() {
  window_t *window;
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
  input_t platformInput = {0};
  output_t gameOutput = {0};
  while(!windowShouldClose(window)) {
    double now = getTime();
    double deltatime = now - previousTime;
    previousTime = now;

    pollInput(window, &platformInput);
    windowUpdateViewport(window, &screenWidth, &screenHeight);
    platformInput.windowHeight = screenHeight;
    platformInput.windowWidth = screenWidth;

    r2dClear();

    gameFrame(deltatime, &platformInput, &gameOutput);

    processGameOutput(window, &gameOutput);
    gameOutput.toggleFullScreen = 0;

    windowSwapBuffers(window);
  }

  windowTerminate(window);
  r2dTerminate();
  gameTerminate();

  return 0;
}
