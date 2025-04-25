#include "math.h"

#include <stdio.h>

mat4 mat4Init(float i) {
  mat4 m = {.data = {0}};

  m.data[0] = i;
  m.data[5] = i;
  m.data[10] = i;
  m.data[15] = i;

  return m;
}

float* matValue(mat4 *mat) {
  return mat->data;
}

mat4 orthographic(float left, float right, float bottom, float top, float near, float far) {
  mat4 m = mat4Init(1.0f);

  // scale
  m.data[0] = 2.0f / (right - left);
  m.data[5] = 2.0f / (top - bottom);
  m.data[10] = -2.0f / (far - near);

  // translation
  m.data[12] = -(left + right) / (right - left);
  m.data[13] = -(top + bottom) / (top - bottom);
  m.data[14] = -(far + near) / (far - near);

  return m;
}

mat4 translate(mat4 mat, vec3 translation) {
  mat4 temp = mat4Init(1.0f);

  temp.data[12] = translation.x;
  temp.data[13] = translation.y;
  temp.data[14] = translation.z;

  return multiply(mat, temp);
}

mat4 scale(mat4 mat, vec3 scale) {
  mat4 temp = mat4Init(1.0f);

  temp.data[0] = scale.x;
  temp.data[5] = scale.y;
  temp.data[10] = scale.z;

  return multiply(mat, temp);
}

mat4 rotate(mat4 mat, float radians, vec3 axis) {
  mat4 temp = mat4Init(1.0f);

  if (axis.x) {
    temp.data[5] = cos(radians);
    temp.data[6] = -sin(radians);
    temp.data[9] = sin(radians);
    temp.data[10] = cos(radians);
  }
  if (axis.y) {
    temp.data[0] = cos(radians);
    temp.data[2] = sin(radians);
    temp.data[8] = -sin(radians);
    temp.data[10] = cos(radians);
  }
  if (axis.z) {
    temp.data[0] = cos(radians);
    temp.data[1] = -sin(radians);
    temp.data[4] = sin(radians);
    temp.data[5] = cos(radians);
  }

  return multiply(mat, temp);
}

float radians(float degrees) {
  return degrees * (3.14159265f / 180.0f);
}


void printMat4(mat4 mat) {
  printf("%f %f %f %f\n", mat.data[0], mat.data[1], mat.data[2], mat.data[3]);
  printf("%f %f %f %f\n", mat.data[4], mat.data[5], mat.data[6], mat.data[7]);
  printf("%f %f %f %f\n", mat.data[8], mat.data[9], mat.data[10], mat.data[11]);
  printf("%f %f %f %f\n", mat.data[12], mat.data[13], mat.data[14], mat.data[15]);
}

mat4 multiply(mat4 a, mat4 b) {
  mat4 result = mat4Init(1.0f);

  int index = 0;
  int aoffset = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int offset = aoffset + i;
      result.data[index] =
        a.data[offset]     * b.data[j + 0] +
        a.data[offset + 1] * b.data[j + 4] +
        a.data[offset + 2] * b.data[j + 8] +
        a.data[offset + 3] * b.data[j + 12];

      index++;
    }
    aoffset += 3;
  }

  return result;
}



float clamp(float value, float min, float max) {
  if (value > max) {
    return max;
  }
  if (value < min) {
    return min;
  }
  return value;
}

vec3 normalize(vec3 v) {
  float distance = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  return (vec3){v.x / distance, v.y / distance, v.z / distance};
}

vec3 vec3Add(vec3 a, vec3 b) {
  return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

float distance(vec3 a, vec3 b) {
  float x = b.x - a.x;
  float y = b.y - a.y;
  return sqrt((x * x) + (y * y));
}

vec3 vec3Sub(vec3 a, vec3 b) {
  return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3 vec3Scaler(vec3 v, float scaler) {
  return (vec3){v.x * scaler, v.y * scaler, v.z * scaler};
}
