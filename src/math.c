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

mat4 transpose(mat4 a) {
  mat4 result = {0};

  result.data[0] = a.data[0];
  result.data[1] = a.data[4];
  result.data[2] = a.data[8];
  result.data[3] = a.data[12];

  result.data[4] = a.data[1];
  result.data[5] = a.data[5];
  result.data[6] = a.data[9];
  result.data[7] = a.data[13];

  result.data[8] = a.data[2];
  result.data[9] = a.data[6];
  result.data[10] = a.data[10];
  result.data[11] = a.data[14];

  result.data[12] = a.data[3];
  result.data[13] = a.data[7];
  result.data[14] = a.data[11];
  result.data[15] = a.data[15];

  return result;
}

float determinant(mat4 a) {
  float det = 0.0f;
  det += a.data[0] * (
        a.data[5] * (a.data[10] * a.data[15] - a.data[11] * a.data[14])
      - a.data[6] * (a.data[9] * a.data[15] - a.data[11] * a.data[13])
      + a.data[7] * (a.data[9] * a.data[14] - a.data[10] * a.data[13])
  );
  det -= a.data[1] * (
        a.data[4] * (a.data[10] * a.data[15] - a.data[11] * a.data[14])
      - a.data[6] * (a.data[8] * a.data[15] - a.data[11] * a.data[12])
      + a.data[7] * (a.data[8] * a.data[14] - a.data[10] * a.data[12])
  );
  det += a.data[2] * (
        a.data[4] * (a.data[9] * a.data[15] - a.data[11] * a.data[13])
      - a.data[5] * (a.data[8] * a.data[15] - a.data[11] * a.data[12])
      + a.data[7] * (a.data[8] * a.data[13] - a.data[9] * a.data[12])
  );
  det -= a.data[3] * (
        a.data[4] * (a.data[9] * a.data[14] - a.data[10] * a.data[13])
      - a.data[5] * (a.data[8] * a.data[14] - a.data[10] * a.data[12])
      + a.data[6] * (a.data[8] * a.data[13] - a.data[9] * a.data[12])
  );
  return det;
}

float det3x3(
    float a00, float a01, float a02,
    float a10, float a11, float a12,
    float a20, float a21, float a22
) {
    return a00 * (a11 * a22 - a12 * a21)
         - a01 * (a10 * a22 - a12 * a20)
         + a02 * (a10 * a21 - a11 * a20);
}


mat4 adjoint(mat4 a) {
  mat4 cofactors = mat4Init(0.0f);

  // For readability:
  float* m = a.data;

  // Cofactor matrix filling:
  cofactors.data[0]  =  det3x3(m[5], m[6], m[7],  m[9], m[10], m[11],  m[13], m[14], m[15]);
  cofactors.data[1]  = -det3x3(m[4], m[6], m[7],  m[8], m[10], m[11],  m[12], m[14], m[15]);
  cofactors.data[2]  =  det3x3(m[4], m[5], m[7],  m[8], m[9],  m[11],  m[12], m[13], m[15]);
  cofactors.data[3]  = -det3x3(m[4], m[5], m[6],  m[8], m[9],  m[10],  m[12], m[13], m[14]);

  cofactors.data[4]  = -det3x3(m[1], m[2], m[3],  m[9], m[10], m[11],  m[13], m[14], m[15]);
  cofactors.data[5]  =  det3x3(m[0], m[2], m[3],  m[8], m[10], m[11],  m[12], m[14], m[15]);
  cofactors.data[6]  = -det3x3(m[0], m[1], m[3],  m[8], m[9],  m[11],  m[12], m[13], m[15]);
  cofactors.data[7]  =  det3x3(m[0], m[1], m[2],  m[8], m[9],  m[10],  m[12], m[13], m[14]);

  cofactors.data[8]  =  det3x3(m[1], m[2], m[3],  m[5], m[6],  m[7],   m[13], m[14], m[15]);
  cofactors.data[9]  = -det3x3(m[0], m[2], m[3],  m[4], m[6],  m[7],   m[12], m[14], m[15]);
  cofactors.data[10] =  det3x3(m[0], m[1], m[3],  m[4], m[5],  m[7],   m[12], m[13], m[15]);
  cofactors.data[11] = -det3x3(m[0], m[1], m[2],  m[4], m[5],  m[6],   m[12], m[13], m[14]);

  cofactors.data[12] = -det3x3(m[1], m[2], m[3],  m[5], m[6],  m[7],   m[9],  m[10], m[11]);
  cofactors.data[13] =  det3x3(m[0], m[2], m[3],  m[4], m[6],  m[7],   m[8],  m[10], m[11]);
  cofactors.data[14] = -det3x3(m[0], m[1], m[3],  m[4], m[5],  m[7],   m[8],  m[9],  m[11]);
  cofactors.data[15] =  det3x3(m[0], m[1], m[2],  m[4], m[5],  m[6],   m[8],  m[9],  m[10]);

  mat4 tran = transpose(cofactors);
  return tran;
}

mat4 inverse(mat4 a) {
  mat4 adj = adjoint(a);
  float deter = determinant(a);
  return mat4Scaler(adj, 1.0f / deter);
}

mat4 mat4Scaler(mat4 a, float scaler) {
  mat4 result = {0};

  for (int i = 0; i < 16; i++) {
    result.data[i] = a.data[i] * scaler;
  }

  return result;
}

vec4 mat4vec4multiply(mat4 a, vec4 v) {
  vec4 result = {0};

  float* m = a.data;

  result.x = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.w;
  result.y = m[4] * v.x + m[5] * v.y + m[6] * v.z + m[7] * v.w;
  result.z = m[8] * v.x + m[9] * v.y + m[10] * v.z + m[11] * v.w;
  result.w = m[12] * v.x + m[13] * v.y + m[14] * v.z + m[15] * v.w;

  // result.x = m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12] * v.w;
  // result.y = m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13] * v.w;
  // result.z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w;
  // result.w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w;


  return result;
}
