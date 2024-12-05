#include "vectors.h"

#include <math.h>

void Cross(float v1[3], float v2[3], float vout[3]) {
  float tmp[3];
  tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
  tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
  tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
  vout[0] = tmp[0];
  vout[1] = tmp[1];
  vout[2] = tmp[2];
}

float Dot(float v1[3], float v2[3]) {
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

float Unit(float vin[3], float vout[3]) {
  float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
  if (dist > 0.0) {
    dist = sqrtf(dist);
    vout[0] = vin[0] / dist;
    vout[1] = vin[1] / dist;
    vout[2] = vin[2] / dist;
  } else {
    vout[0] = vin[0];
    vout[1] = vin[1];
    vout[2] = vin[2];
  }
  return dist;
}

float Unit(float v[3]) {
  float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
  if (dist > 0.0) {
    dist = sqrtf(dist);
    v[0] /= dist;
    v[1] /= dist;
    v[2] /= dist;
  }
  return dist;
}

float *
Array3(float a, float b, float c) {
  static float array[4];

  array[0] = a;
  array[1] = b;
  array[2] = c;
  array[3] = 1.;
  return array;
}

// utility to create an array from a multiplier and an array:

float *
MulArray3(float factor, float array0[]) {
  static float array[4];

  array[0] = factor * array0[0];
  array[1] = factor * array0[1];
  array[2] = factor * array0[2];
  array[3] = 1.;
  return array;
}

float *
MulArray3(float factor, float a, float b, float c) {
  static float array[4];

  float *abc = Array3(a, b, c);
  array[0] = factor * abc[0];
  array[1] = factor * abc[1];
  array[2] = factor * abc[2];
  array[3] = 1.;
  return array;
}