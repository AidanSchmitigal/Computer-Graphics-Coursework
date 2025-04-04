#pragma once

float* Array3(float a, float b, float c);
float* MulArray3(float factor, float array0[]);
float* MulArray3(float factor, float a, float b, float c);
void Cross(float[3], float[3], float[3]);
float Dot(float[3], float[3]);
float Unit(float[3], float[3]);
float Unit(float[3]);