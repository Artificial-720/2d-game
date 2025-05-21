#include <stdio.h>

#include "platform/input.h"
#include "platform/output.h"
#include "platform/window.h"
#include "platform/renderer2d.h"
#include "game/game.h"


void intToStr(int n, char *buf) {
  int places[4] = {0};
  int place = 1000;
  int temp = n;

  for (int i = 3; i >= 0; i--) {
    places[i] = temp / place;
    temp = temp - (places[i] * place);
    place /= 10;
  }

  buf[0] = '0' + places[3];
  buf[1] = '0' + places[2];
  buf[2] = '0' + places[1];
  buf[3] = '0' + places[0];
  buf[4] = '\0';
}


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

  char buf[15] = "FPS: ";
  int frameCount = 0;
  double startTime = getTime();

  while(!windowShouldClose(window)) {
    double now = getTime();
    double deltatime = now - previousTime;
    previousTime = now;

    frameCount++;
    int fps = (int)(frameCount / (now - startTime));
    intToStr(fps, buf + 5);
    windowSetTitle(window, buf);

    pollInput(window, &platformInput);
    windowUpdateViewport(window, &screenWidth, &screenHeight);
    platformInput.windowHeight = screenHeight;
    platformInput.windowWidth = screenWidth;

    r2dClear();

    gameFrame(deltatime, &platformInput, &gameOutput);

    processGameOutput(window, &gameOutput);
    gameOutput.toggleFullScreen = 0;

    windowSwapBuffers(window);
    // printf("draw calls %d\n", drawCalls());
  }

  windowTerminate(window);
  r2dTerminate();
  gameTerminate();

  return 0;
}
