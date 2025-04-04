#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/simplex.h"

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

void OsuCube(float width, float height, float depth) {
  float halfWidth = width / 2.0f;
  float halfHeight = height / 2.0f;
  float halfDepth = depth / 2.0f;

  glBegin(GL_QUADS);

  // Front face
  glNormal3f(0.0f, 0.0f, 1.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-halfWidth, -halfHeight, halfDepth);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(halfWidth, -halfHeight, halfDepth);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(halfWidth, halfHeight, halfDepth);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-halfWidth, halfHeight, halfDepth);

  // Back face
  glNormal3f(0.0f, 0.0f, -1.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-halfWidth, -halfHeight, -halfDepth);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(-halfWidth, halfHeight, -halfDepth);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(halfWidth, halfHeight, -halfDepth);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(halfWidth, -halfHeight, -halfDepth);

  // Top face
  glNormal3f(0.0f, 1.0f, 0.0f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-halfWidth, halfHeight, -halfDepth);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-halfWidth, halfHeight, halfDepth);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(halfWidth, halfHeight, halfDepth);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(halfWidth, halfHeight, -halfDepth);

  // Bottom face
  glNormal3f(0.0f, -1.0f, 0.0f);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(-halfWidth, -halfHeight, -halfDepth);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(halfWidth, -halfHeight, -halfDepth);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(halfWidth, -halfHeight, halfDepth);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-halfWidth, -halfHeight, halfDepth);

  // Right face
  glNormal3f(1.0f, 0.0f, 0.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(halfWidth, -halfHeight, -halfDepth);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(halfWidth, halfHeight, -halfDepth);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(halfWidth, halfHeight, halfDepth);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(halfWidth, -halfHeight, halfDepth);

  // Left face
  glNormal3f(-1.0f, 0.0f, 0.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-halfWidth, -halfHeight, -halfDepth);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-halfWidth, -halfHeight, halfDepth);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(-halfWidth, halfHeight, halfDepth);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-halfWidth, halfHeight, -halfDepth);

  glEnd();
}