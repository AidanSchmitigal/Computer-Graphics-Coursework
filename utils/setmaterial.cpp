#include "setmaterial.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

#include "colors.h"
#include "glut/glut.h"
#include "vectors.h"

void SetMaterial(float r, float g, float b, float shininess) {
  glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
  glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, (float *)WHITE));
  glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., (float *)WHITE));
  glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
  glMaterialf(GL_BACK, GL_SHININESS, 2.f);

  glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
  glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
  glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
  glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, (float *)WHITE));
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void SetMaterialI(float r, float g, float b, float shininess) {
  SetMaterial(r / 255., g / 255., b / 255., shininess);
}