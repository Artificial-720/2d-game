#ifndef RENDERER2D_H
#define RENDERER2D_H

#include "camera.h"
#include "sprite.h"
#include "../core/math.h"

int r2dInit();
void r2dTerminate();

// Clears the buffer
void r2dClear();

int r2dCreateTexture(int width, int height, unsigned char *data, unsigned int *textureID);

void r2dDrawSprite(camera_t *camera, sprite_t sprite);

void r2dSetView(mat4 a);
void r2dSetProjection(mat4 a);
// 256 max
void r2dSetClearColorRGBA(float r, float g, float b, float a);

#endif
