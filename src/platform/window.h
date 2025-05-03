#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef GLFWwindow window_t;

window_t *windowInit(int width, int height, const char *title);
void windowTerminate(window_t *window);
int windowShouldClose(window_t *window);
void windowSwapBuffers(window_t *window);

/*
* return is if width or height changed from value in width or height
*/
int updateWindowViewport(window_t *window, int *width, int *height);

#define KEY_SPACE 32
#define KEY_A 65
#define KEY_D 68
#define KEY_F 70
#define KEY_ESC 256

#define MOUSE_BUTTON_LEFT 0
#define MOUSE_BUTTON_RIGHT 1

enum keyState {PRESS, RELEASE, HELD};

int getKey(window_t *window, int keyCode);
int getMouseButton(window_t *window, int button);
void getCursorPos(window_t *window, double *xpos, double *ypos);


void pollInput();
double getTime();

void toggleFullScreen(window_t *window);

void windowSetScrollCallback(window_t *window, void (*callback)(window_t *, double, double));



  // TODO move this to window
  // for alpha
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // for lines
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

#endif
