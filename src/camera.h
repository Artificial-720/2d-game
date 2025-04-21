#ifndef CAMERA_H
#define CAMERA_H

#include "math.h"

typedef struct {
  float x, y;
  mat4 view;
} Camera;


Camera initCamera();

void cameraSetPosition(Camera *camera, float x, float y);

#endif
