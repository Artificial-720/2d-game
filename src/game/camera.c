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

vec4 screenToWorld(camera_t *camera, float xpos, float ypos) {
  // mat4 vpInverse = multiply(renderer->projection, renderer->view);
  // vpInverse = inverse(vpInverse);
  // double x = 2 * (xpos / width) - 1;
  // double y = 1 - 2 * (ypos / height);
  // vec4 screenPos = {x, -y, 0.0f, 1.0f};
  // vec4 result = mat4vec4multiply(vpInverse, screenPos);
  // result.x += camera.pos.x;
  // result.y -= camera.pos.y;

  mat4 vp = multiply(camera->projection, camera->view);
  mat4 inverseVP = inverse(vp);
  double x = 2 * (xpos / camera->width) - 1;
  double y = 1 - 2 * (ypos / camera->height);
  vec4 screenPos = {x, -y, 0.0f, 1.0f};
  vec4 result = mat4vec4multiply(inverseVP, screenPos);
  result.x += camera->pos.x;
  result.y -= camera->pos.y;
  result.y *= -1.0f;
  return result;
}
