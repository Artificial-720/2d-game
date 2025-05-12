#ifndef RENDERER2D_H
#define RENDERER2D_H

#include "sprite.h"
#include "../core/math.h"

int r2dInit();
void r2dTerminate();

// Clears the buffer
void r2dClear();

int r2dCreateTexture(int width, int height, unsigned char *data, unsigned int *textureID);

void r2dDrawSprite(sprite_t sprite);

void r2dSetView(mat4 a);
void r2dSetProjection(mat4 a);
// 256 max
void r2dSetClearColorRGBA(float r, float g, float b, float a);

// the code i have seen is renderer.render(camera, scene);
// something like that so ever time i am drawing stuff its from some camera
//
// i think i need to have another camera that will hold the screen space
// so i can render ui to that camera, then i dont have to deal with world space
// things can be draw to screen directly
#endif
