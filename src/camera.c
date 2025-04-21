#include "camera.h"

Camera initCamera() {
  Camera camera = {
    .view = mat4Init(1.0f)
  };
  return camera;
}

void cameraSetPosition(Camera *camera, float x, float y) {
  camera->x = x;
  camera->y = y;
}
