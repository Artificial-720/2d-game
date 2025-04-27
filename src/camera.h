#ifndef CAMERA_H
#define CAMERA_H

#include "math.h"

typedef struct {
  vec2 pos;
  mat4 view;
  float zoomFactor;
  vec2 offset;
} camera_t;


camera_t cameraInit();

void cameraUpdatePosition(camera_t *camera, float x, float y);


#endif
