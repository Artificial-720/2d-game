#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  unsigned int id;
  int width, height;
} texture_t;

texture_t loadTexture(const char *path);

#endif
