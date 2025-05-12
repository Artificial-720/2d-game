#ifndef UI_H
#define UI_H

#include "camera.h"
#include "../platform/input.h"

typedef struct {
  vec2 pos; // screen space
  int enabled;
} ui_t;

typedef enum {
  ITEM_EMPTY = 0,
  ITEM_PICKAXE
} item_e;

typedef struct {
  item_e item;
  int count;
} item_t;

void setupHud();
void drawHud(camera_t *camera, item_t *inventory, int selected, input_t *input);

#endif
