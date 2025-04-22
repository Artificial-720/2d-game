#include "controls.h"
#include <GLFW/glfw3.h>

extern GLFWwindow *window;

int ecsGetKey(int key) {
  int keyState = glfwGetKey(window, key);
  if (keyState == GLFW_PRESS) {
    return PRESS;
  } else {
    return RELEASE;
  }
  return keyState;
}
