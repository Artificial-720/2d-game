#include "camera.h"
#include "math.h"

#include <stdio.h>

camera_t cameraInit() {
  camera_t camera = {
    .view = mat4Init(1.0f),
    .zoomFactor = 1.0f,
    .pos = (vec2){0.0f, 0.0f},
    .offset = (vec2){0.0f, 0.0f}
  };
  return camera;
}

void cameraUpdatePosition(camera_t *camera, float x, float y) {
  // renderer->view = translate(renderer->view, (vec3){camera.pos.x, camera.pos.y, 0});

  float newX = camera->offset.x - x;
  float newY = camera->offset.y - y;

  printf("camera pos: %f %f\n", newX, newY);

  camera->view = mat4Init(1.0f);
  camera->view = translate(camera->view, (vec3){newX, newY, 0});
  // camera->view = translate(camera->view, (vec3){camera->offset.x - x, y, 0});
  // camera->view = translate(camera->view, (vec3){camera->offset.x, camera->offset.y, 0.0f});


  // camera->pos.x = x;
  // camera->pos.y = y;


}
