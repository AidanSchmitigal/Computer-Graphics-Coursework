#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simplex.cpp"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifndef F_PI
#define F_PI ((float)(M_PI))
#define F_2_PI ((float)(2.f * F_PI))
#define F_PI_2 ((float)(F_PI / 2.f))
#endif

void OsuGrid(float x, float z) {
  float DX = 1. / x;
  float DZ = 1. / z;

  glNormal3f(0., 1., 0.);
  for (int i = 0; i < x; i++) {
    glBegin(GL_QUAD_STRIP);
    for (int j = 0; j <= z; j++) {
      glVertex3f(DZ * (float)j - .5, 0, DX * (float)(i + 0) - .5);
      glVertex3f(DZ * (float)j - .5, 0, DX * (float)(i + 1) - .5);
    }
    glEnd();
  }
}

void OsuNoiseGrid(float xv, float zv, float x_scale = 1, float y_scale = 1, float z_scale = 1) {
  float DX = 1. / xv;
  float DZ = 1. / zv;

  for (int i = 0; i < xv; i++) {
    glBegin(GL_QUAD_STRIP);
    for (int j = 0; j <= zv; j++) {
      float x = (DZ * j);
      float z = (DX * i);
      float y = noise_eval2d(x * x_scale, z * z_scale) * y_scale;

      float y_nudge_x = noise_eval2d((x + 0.0001) * x_scale, z * z_scale) * y_scale;
      float y_nudge_z = noise_eval2d(x * x_scale, (z + 0.0001) * z_scale) * y_scale;

      float vec1[] = {0.0001, y_nudge_x - y, 0};
      float vec2[] = {0, y_nudge_z - y, 0.0001};

      float vec3[] = {0, 0, 0};

      vec3[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
      vec3[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
      vec3[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];

      float vec3_length = -sqrt(vec3[0] * vec3[0] + vec3[1] * vec3[1] + vec3[2] * vec3[2]);

      vec3[0] /= vec3_length;
      vec3[1] /= vec3_length;
      vec3[2] /= vec3_length;

      glNormal3f(vec3[0], vec3[1], vec3[2]);
      glColor3f(vec3[0] * .5 + .5, vec3[1] * .5 + .5, vec3[2] * .5 + .5);
      glVertex3f(x - .5, y, z - .5);

      z = (DX * (i + 1));
      y = noise_eval2d(x * x_scale, z * z_scale) * y_scale;

      y_nudge_x = noise_eval2d((x + 0.0001) * x_scale, z * z_scale) * y_scale;
      y_nudge_z = noise_eval2d(x * x_scale, (z + 0.0001) * z_scale) * y_scale;

      vec1[0] = 0.0001;
      vec1[1] = y_nudge_x - y;
      vec1[2] = 0;

      vec2[0] = 0;
      vec2[1] = y_nudge_z - y;
      vec2[2] = 0.0001;

      vec3[0] = vec3[1] = vec3[2] = 0;

      vec3[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
      vec3[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
      vec3[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];

      vec3_length = -sqrt(vec3[0] * vec3[0] + vec3[1] * vec3[1] + vec3[2] * vec3[2]);

      vec3[0] /= vec3_length;
      vec3[1] /= vec3_length;
      vec3[2] /= vec3_length;

      glNormal3f(vec3[0], vec3[1], vec3[2]);
      glColor3f(vec3[0] * .5 + .5, vec3[1] * .5 + .5, vec3[2] * .5 + .5);
      glVertex3f(x - .5, y, z - .5);
    }
    glEnd();
  }
}