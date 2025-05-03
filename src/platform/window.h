#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "input.h"
#include "output.h"

typedef GLFWwindow window_t;

window_t *windowInit(int width, int height, const char *title);
void windowTerminate(window_t *window);
int windowShouldClose(window_t *window);
void windowSwapBuffers(window_t *window);

/*
* return is if width or height changed from value in width or height
*/
int windowUpdateViewport(window_t *window, int *width, int *height);
void pollInput(window_t *window, input_t *input);
void processGameOutput(window_t *window, output_t *output);
double getTime();

#endif
