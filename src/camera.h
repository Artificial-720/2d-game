#ifndef CAMERA_H
#define CAMERA_H

#include "math.h"

typedef struct {
  vec2 pos;
  mat4 view;
  mat4 projection;
  float zoomFactor;
  int width, height;
} camera_t;


camera_t cameraInit();

void cameraUpdatePosition(camera_t *camera, float x, float y);

vec4 screenToWorld(camera_t *camera, float xpos, float ypos);

#endif
