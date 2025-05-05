#include "texture.h"
#include "../platform/renderer2d.h"

#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../core/stb_image.h"

#define MAX_TEXTURES 100
#define MAX_PATH 100

typedef struct {
  int filled;
  unsigned int id;
  char path[MAX_PATH];
} texture_t;

static texture_t textures[MAX_TEXTURES];

static unsigned int findTexture (const char *path) {
  for (int i = 0; i < MAX_TEXTURES; i++) {
    if (!textures[i].filled) continue;

    int equal = 1;
    for (int j = 0; j < MAX_PATH; j++) {
      if (textures[i].path[j] != path[j]) {
        equal = 0;
        break;
      }
      if (textures[i].path[j] == '\0' || path[j] == '\0') {
        break;
      }
    }

    if (equal) {
      return textures[i].id;
    }
  }
  return 0;
}

unsigned int loadTexture(const char *path) {
  unsigned int id = findTexture(path);
  if (id) {
    // already loaded
    return id;
  }

  // load file
  stbi_set_flip_vertically_on_load(1);
  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 4);
  if (data) {
    // create renderer texture
    if (r2dCreateTexture(width, height, data, &id)) {
      assert(0);
    }
  } else {
    return 0;
  }
  stbi_image_free(data);

  // add to array
  for (int i = 0; i < MAX_TEXTURES; i++) {
    if (textures[i].filled) continue;

    textures[i].filled = 1;
    textures[i].id = id;

    for (int j = 0; j < MAX_PATH; j++) {
      textures[i].path[j] = path[j];
      if (path[j] == '\0') break;
    }

    break;
  }

  return id;
}

unsigned int getTexture(const char *path) {
  unsigned int id = findTexture(path);
  if (id) {
    return id;
  }
  return loadTexture(path);
}
