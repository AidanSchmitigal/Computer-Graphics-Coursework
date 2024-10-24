#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    for (int j = 0; j < z; j++) {
      glVertex3f(DX * (float)j, 0, DZ * (float)(i + 0));
      glVertex3f(DX * (float)j, 0, DZ * (float)(i + 1));
    }
    glEnd();
  }
}