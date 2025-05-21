#include "window.h"
#include "input.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

static int initWidth;
static int initHeight;
static int keyLocks[256];

static void toggleFullScreen(window_t *window) {
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

static int getKey(window_t *window, int keyCode) {
  int keyState = glfwGetKey(window, keyCode);
  if (keyState == GLFW_PRESS) {
    if (keyLocks[keyCode]) {
      return KEY_HELD;
    } else {
      keyLocks[keyCode] = 1;
      return KEY_PRESS;
    }
  } else {
    if (keyLocks[keyCode]) {
      keyLocks[keyCode] = 0;
    }
    return KEY_RELEASE;
  }
  return keyState;
}

static int getMouseButton(window_t *window, int button) {
  if (glfwGetMouseButton(window, button) == GLFW_PRESS) {
    return KEY_PRESS;
  }
  return KEY_RELEASE;
}

// void windowSetScrollCallback(window_t *window, void (*callback)()) {
//   glfwSetScrollCallback(window, callback);
// }

// void getCursorPos(window_t *window, double *xpos, double *ypos) {
//   glfwGetCursorPos(window, xpos, ypos);
// }


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

  // disable v-sync
  // glfwSwapInterval(0);
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

int windowUpdateViewport(window_t *window, int *width, int *height) {
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

double getTime() {
  return glfwGetTime();
}

void windowSetTitle(window_t *window, const char *title) {
  glfwSetWindowTitle(window, title);
}

void processGameOutput(window_t *window, output_t *output) {
  if (output->toggleFullScreen) {
    toggleFullScreen(window);
  }
}

void pollInput(window_t *window, input_t *input) {
  glfwPollEvents();

  for (int i = GLFW_KEY_A; i < GLFW_KEY_Z + 1; i++) {
    input->keyStates[i - GLFW_KEY_A] = getKey(window, i);
  }
  for (int i = GLFW_KEY_0; i < GLFW_KEY_9 + 1; i++) {
    input->keyStates[i - GLFW_KEY_0 + KEY_0] = getKey(window, i);
  }

  input->keyStates[KEY_SPACE] = getKey(window, GLFW_KEY_SPACE);
  input->keyStates[KEY_ESCAPE] = getKey(window, GLFW_KEY_ESCAPE);

  // mouse
  double mouseX, mouseY;
  glfwGetCursorPos(window, &mouseX, &mouseY);
  input->mouseX = mouseX;
  input->mouseY = mouseY;
  if ((mouseX >= 0 && mouseX <= input->windowWidth) &&
    (mouseY >= 0 && mouseY <= input->windowHeight)) {
    input->mouseStates[MOUSE_BUTTON_LEFT] = getMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    input->mouseStates[MOUSE_BUTTON_RIGHT] = getMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
  } else {
    input->mouseStates[MOUSE_BUTTON_LEFT] = KEY_RELEASE;
    input->mouseStates[MOUSE_BUTTON_RIGHT] = KEY_RELEASE;
  }
}

