#ifndef MATH_H
#define MATH_H

#include <math.h>

typedef struct {
  float x, y;
} vec2;

typedef struct {
  float x,y,z;
} vec3;

typedef struct {
  float x,y,z,w;
} vec4;

/*
 *  0  1  2  3
 *  4  5  6  7
 *  8  9 10 11
 * 12 13 14 15
 */
typedef struct {
  float data[16];
} mat4;

mat4 mat4Init(float i);
float* matValue(mat4 *mat);
mat4 translate(mat4 mat, vec3 translation);
mat4 scale(mat4 mat, vec3 scale);
mat4 rotate(mat4 mat, float radians, vec3 axis);
mat4 multiply(mat4 a, mat4 b);
mat4 inverse(mat4 a);

/*
* Creates a matrix for orthographic projection
*/
mat4 orthographic(float left, float right, float bottom, float top, float near, float far);

void printMat4(mat4 mat);

float radians(float degrees);
float clamp(float value, float min, float max);

float distance(vec3 a, vec3 b);
mat4 mat4Scaler(mat4 a, float scaler);

vec4 mat4vec4multiply(mat4 a, vec4 b);

// Vector 2 math
vec2 vec2Add(vec2 a, vec2 b);
vec2 vec2Sub(vec2 a, vec2 b);

// Vector 3 math
vec3 normalize(vec3 v);
vec3 vec3Add(vec3 a, vec3 b);
vec3 vec3Sub(vec3 a, vec3 b);
vec3 vec3Scaler(vec3 v, float scaler);

#endif
