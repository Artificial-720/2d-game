#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow *initWindow(int width, int height, const char *title);
void cleanupWindow(GLFWwindow *window);

/*
* return is if width or height changed from value in width or height
*/
int updateWindowViewport(GLFWwindow *window, int *width, int *height);

#endif
