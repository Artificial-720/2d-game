#ifndef UI_H
#define UI_H

#include "item.h"
#include "player.h"
#include "../platform/camera.h"
#include "../platform/input.h"

typedef struct {
  vec2 pos; // screen space
  int enabled;
} ui_t;

void setupHud();
void drawHud(player_t *player, camera_t *camera, input_t *input);

void drawPauseScreen(camera_t *camera);

#endif
