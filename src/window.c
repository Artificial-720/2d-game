#include "window.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

GLFWwindow *initWindow(int width, int height, const char* title) {
  GLFWwindow *window;

  if(!glfwInit()) {
    printf("Failed to init glfw\n");
    return 0;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, title, 0, 0);
  if (!window) {
    printf("Failed to create a window\n");
    glfwTerminate();
    return 0;
  }

  glfwMakeContextCurrent(window);

  if(glewInit()) {
    printf("Failed to init glew\n");
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
  }

  return window;
}

void cleanupWindow(GLFWwindow *window) {
  glfwDestroyWindow(window);
  glfwTerminate();
}

int updateWindowViewport(GLFWwindow *window, int *width, int *height) {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  if (*width != w || *height != h) {
    *width = w;
    *height = h;
    glViewport(0, 0, w, h);
    return 1;
  }
  return 0;
}

