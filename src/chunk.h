#ifndef CHUNK_H
#define CHUNK_H

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 64
#define CHUNK_DISTANCE 15

typedef struct {
  int id;
} tile_t;

typedef struct {
  float x, y;
  tile_t tiles[CHUNK_WIDTH * CHUNK_HEIGHT];
} chunk_t;


static inline int clickedInChunk(chunk_t *chunk, float x, float y) {
  return (x < CHUNK_WIDTH + chunk->x && x >= 0 + chunk->x && y < CHUNK_HEIGHT + chunk->y && y >= 0 + chunk->y);
};

void placeTile(chunk_t *chunk, float x, float y);
void removeTile(chunk_t *chunk, float x, float y);



#endif
