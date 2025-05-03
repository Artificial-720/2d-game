#ifndef SPRITE_H
#define SPRITE_H

typedef struct {
  float x, y; // position
  float width, height;
  float rotation; // in degrees
  unsigned int texture;
} sprite_t;

sprite_t createSprite(float x, float y, float width, float height, float rotation, unsigned int textureID);

#endif
