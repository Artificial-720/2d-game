#include "noise.h"

/*
      ┌────────────┐(1,1)
      │            │
      │            │
      │            │
      │            │
      │            │
      │            │
(0, 0)└────────────┘
*/

typedef struct {
  float x,y;
} vector2;

// Perlin noise
// three steps
// 1. grid of random gradient vectors
// 2. dot product between the gradient vectors and the offsets
// 3. interpolation between the values

static vector2 gradients[8] = {
  {1, 0}, {-1, 0}, {0, 1}, {0, -1},
  {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
};

static vector2 gradientVec(int x, int y) {
  int seed = 11;
  int hash = (x + y * 57 + seed) % 256;
  return gradients[hash % 8];
}

static float dotProduct(float x0, float y0, float x1, float y1) {
  return (x0 * x1) + (y0 * y1);
}

static float interpolate(float a0, float a1, float weight) {
  return (a1 - a0) * (2.0f - weight * 2.0f) * weight * weight + a0;
}

double perlin(float x, float y) {
  // convert to grid coordinates
  int x0 = (int)x;
  int y0 = (int)y;
  int x1 = x0 + 1;
  int y1 = y0 + 1;

  // Interpolation weights
  // based on distance to cell origin
  // close the point is to a corner the more influence
  // that corner has on the result
  float sx = x - (float)x0;
  float sy = y - (float)y0;

  // Get gradient vectors for each corner
  vector2 grad00 = gradientVec(x0, y0);
  vector2 grad10 = gradientVec(x1, y0);
  vector2 grad01 = gradientVec(x0, y1);
  vector2 grad11 = gradientVec(x1, y1);

  // dot product between gradient vector and offset(ie. distance vector)
  // offset happens to be the same as our interpolation weights
  float dot00 = dotProduct(grad00.x, grad00.y, sx    , sy    );
  float dot10 = dotProduct(grad10.x, grad10.y, sx - 1, sy    );
  float dot01 = dotProduct(grad01.x, grad01.y, sx    , sy - 1);
  float dot11 = dotProduct(grad11.x, grad11.y, sx - 1, sy - 1);

  // interpolate between the points, "smooth" between all the values
  // interpolate in x direction
  float iTop = interpolate(dot00, dot10, sx);
  float iBottom = interpolate(dot01, dot11, sx);
  // interpolate in y direction
  float result = interpolate(iTop, iBottom, sy);

  return result;
}

