#ifndef RENDER2D_H
#define RENDER2D_H

#include <GL/glew.h>
#include "math.h"

typedef struct {
  float x, y; // position
  float width, height;
  float rotation; // in degrees
  unsigned int texture;
} Sprite;

typedef struct {
  unsigned int program;
  unsigned int vao;
  unsigned int vbo;
  unsigned int ibo;
  unsigned int *textures;
  unsigned int boundTexture;
  int texturesCount;
  int indicesCount;
  int maxTextures;
  mat4 view;
  mat4 projection;
} Render2d;

Render2d* r2dInit();
void r2dTerminate(Render2d *render);

// Clears the buffer
void r2dClear();

int r2dCreateTexture(Render2d *render, const char *filename, unsigned int *textureID);

void r2dDrawSprite(Render2d *render, Sprite sprite);

Sprite createSprite(float x, float y, float width, float height, float rotation, unsigned int textureID);

// void r2dDrawQuad(Render2d *render, float x, float y, float width, float height, vec3 color, float angle);

#endif
