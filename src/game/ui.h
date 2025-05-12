#ifndef UI_H
#define UI_H

#include "item.h"
#include "camera.h"
#include "../platform/input.h"

typedef struct {
  vec2 pos; // screen space
  int enabled;
} ui_t;

void setupHud();
void drawHud(camera_t *camera, item_t *inventory, int selected, input_t *input);

#endif
