#include "camera.h"
#include "math.h"

camera_t cameraInit() {
  camera_t camera = {
    .view = mat4Init(1.0f),
    .zoomFactor = 1.0f,
    .pos = (vec2){0.0f, 0.0f},
    // .offset = (vec2){0.0f, 0.0f}
  };
  return camera;
}

void cameraUpdatePosition(camera_t *camera, float x, float y) {
  camera->pos.x = x;
  camera->pos.y = y;
  camera->view = mat4Init(1.0f);
  camera->view = translate(camera->view, (vec3){-x, -y, 0});
}
