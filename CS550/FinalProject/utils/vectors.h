#pragma once

void Cross(float v1[3], float v2[3], float vout[3]);
float Dot(float v1[3], float v2[3]);
float Unit(float vin[3], float vout[3]);
float Unit(float v[3]);
float* Array3(float a, float b, float c);
float* MulArray3(float factor, float array0[]);
float* MulArray3(float factor, float a, float b, float c);