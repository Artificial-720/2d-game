#include "window.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

static int initWidth;
static int initHeight;
static int keyLocks[256];



GLFWwindow *windowInit(int width, int height, const char* title) {
  initWidth = width;
  initHeight = height;
  for (int i = 0; i < 256; i++) keyLocks[i] = 0;

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

void windowTerminate(GLFWwindow *window) {
  glfwDestroyWindow(window);
  glfwTerminate();
}

int windowShouldClose(window_t *window) {
  return glfwWindowShouldClose(window);
}

void windowSwapBuffers(window_t *window) {
  glfwSwapBuffers(window);
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

int getKey(window_t *window, int keyCode) {
  int keyState = glfwGetKey(window, keyCode);
  if (keyState == GLFW_PRESS) {
    if (keyLocks[keyCode]) {
      return HELD;
    } else {
      keyLocks[keyCode] = 1;
      return PRESS;
    }
  } else {
    if (keyLocks[keyCode]) {
      keyLocks[keyCode] = 0;
    }
    return RELEASE;
  }
  return keyState;
}

void pollInput() {
  glfwPollEvents();
}

double getTime() {
  return glfwGetTime();
}

void toggleFullScreen(window_t *window) {
  GLFWmonitor *monitor = glfwGetWindowMonitor(window);
  if (monitor) {
    glfwSetWindowMonitor(window, NULL, 50, 50, initWidth, initHeight, 0);
  } else {
    GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
    if (primaryMonitor) {
      const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
      glfwSetWindowMonitor(window, primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
  }
}

void windowSetScrollCallback(window_t *window, void (*callback)()) {
  glfwSetScrollCallback(window, callback);
}

int getMouseButton(window_t *window, int button) {
  if (glfwGetMouseButton(window, button) == GLFW_PRESS) {
    return PRESS;
  }
  return RELEASE;
}

void getCursorPos(window_t *window, double *xpos, double *ypos) {
  glfwGetCursorPos(window, xpos, ypos);
}
