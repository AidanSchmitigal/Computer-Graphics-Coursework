#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

#ifndef TOTAL
#define TOTAL 30.f
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include "../utils/keytime.h"
#include "../utils/setmaterial.h"

#ifndef F_PI
#define F_PI ((float)(M_PI))
#define F_2_PI ((float)(2.f * F_PI))
#define F_PI_2 ((float)(F_PI / 2.f))
#endif

#include <vector>

#include "plants.h"

struct Plant {
  int plant_id;
  float age;
  float x;
  float y;
  float z;
};

GLuint Plant1List;
GLuint Plant2List;
GLuint Plant3List;
GLuint Plant4List;

Keytimes scaleXZ;
Keytimes scaleY;
Keytimes rotation;

std::vector<Plant> plants;

void add_random_plant(float x, float y, float z) {
  Plant p;
  p.plant_id = rand() % 4;
  p.age = 0.;
  p.x = x + rand() % 10 / 10.;
  p.y = y;
  // p.y = y + rand() % 10 / 10.;
  p.z = z + rand() % 10 / 10.;
  plants.push_back(p);
}

float total_time = TOTAL * .9 * 2;
// float total_time = 50.;

void update_plants(float deltaTime) {
  for (auto it = plants.begin(); it != plants.end();) {
    it->age += deltaTime;
    if (it->age > total_time) {
      it = plants.erase(it);
    } else {
      it++;
    }
  }
}

const GLuint *PLANT_MODELS[4] = {&Plant1List, &Plant2List, &Plant3List, &Plant4List};

void render_plants() {
  for (auto it = plants.begin(); it != plants.end(); it++) {
    glPushMatrix();

    glTranslatef(it->x, it->y, it->z);
    // glRotatef(it->age * 10., 0., 1., 0.);
    // glScalef(sin((it->age - 10) / total_time * F_PI), sin((it->age - 10) / total_time * F_PI) + it->plant_id / 20., sin((it->age - 10) / total_time * F_PI));

    glRotatef(rotation.GetValue(it->age), 0., 1., 0.);
    glScalef(scaleXZ.GetValue(it->age), scaleY.GetValue(it->age), scaleXZ.GetValue(it->age));
    glCallList(*PLANT_MODELS[it->plant_id]);

    glPopMatrix();
  }
}

// Helper function to compute and set normal
void setNormal(float ax, float ay, float az,
               float bx, float by, float bz,
               float cx, float cy, float cz) {
  // Compute vectors from point A to points B and C
  float bax = bx - ax;
  float bay = by - ay;
  float baz = bz - az;

  float cax = cx - ax;
  float cay = cy - ay;
  float caz = cz - az;

  // Compute cross product (B-A)×(C-A)
  float nx = bay * caz - baz * cay;
  float ny = baz * cax - bax * caz;
  float nz = bax * cay - bay * cax;

  // Normalize
  float len = sqrt(nx * nx + ny * ny + nz * nz);
  glNormal3f(nx / len, ny / len, nz / len);
  // printf("Normal: %f, %f, %f\n", nx / len, ny / len, nz / len);
}

void plant2() {
  glBegin(GL_TRIANGLES);

  // STEM
  SetMaterial(0.f, .6f, 0.2f, 1.f);

  glNormal3f(0.f, 0.f, 1.f);
  glVertex3f(.1f, 0.f, 0.f);
  glVertex3f(-.1f, 0.f, 0.f);
  glVertex3f(0.f, 2.f, 0.f);
  glVertex3f(0.f, 0.f, 0.1f);
  glVertex3f(0.f, 0.f, -0.1f);
  glVertex3f(0.f, 2.f, 0.f);

  // PETALS
  SetMaterial(0.9f, 0.9f, .8f, 1.f);

  setNormal(0.f, 1.8f, 0.f, -0.2f, 2.2f, 0.2f, -0.3f, 2.5f, -0.3f);
  glVertex3f(0.f, 1.8f, 0.f);
  glVertex3f(-0.2f, 2.2f, 0.2f);
  glVertex3f(-0.3f, 2.5f, -0.3f);

  setNormal(0.f, 1.8f, 0.f, 0.3f, 2.5f, -0.3f, 0.2f, 2.2f, 0.2f);
  glVertex3f(0.f, 1.8f, 0.f);
  glVertex3f(0.3f, 2.5f, -0.3f);
  glVertex3f(0.2f, 2.2f, 0.2f);

  setNormal(-0.3f, 2.5f, -0.3f, 0.3f, 2.5f, -0.3f, 0.2f, 2.5f, -0.5f);
  glVertex3f(-0.3f, 2.5f, -0.3f);
  glVertex3f(0.3f, 2.5f, -0.3f);
  glVertex3f(0.2f, 2.5f, -0.5f);

  setNormal(-0.3f, 2.5f, -0.3f, 0.2f, 2.5f, -0.5f, -0.2f, 2.5f, -0.5f);
  glVertex3f(-0.3f, 2.5f, -0.3f);
  glVertex3f(0.2f, 2.5f, -0.5f);
  glVertex3f(-0.2f, 2.5f, -0.5f);

  SetMaterial(0.8f, 0.8f, .8f, 1.f);
  setNormal(0.f, 1.8f, 0.f, -0.3f, 2.5f, -0.3f, 0.3f, 2.5f, -0.3f);
  glVertex3f(0.f, 1.8f, 0.f);
  glVertex3f(-0.3f, 2.5f, -0.3f);
  glVertex3f(0.3f, 2.5f, -0.3f);

  setNormal(0.f, 1.8f, 0.f, 0.2f, 2.2f, 0.2f, -0.2f, 2.2f, 0.2f);
  glVertex3f(0.f, 1.8f, 0.f);
  glVertex3f(0.2f, 2.2f, 0.2f);
  glVertex3f(-0.2f, 2.2f, 0.2f);

  setNormal(-0.2f, 2.5f, -0.5f, 0.2f, 2.5f, -0.5f, 0.f, 2.3f, -0.6f);
  glVertex3f(-0.2f, 2.5f, -0.5f);
  glVertex3f(0.2f, 2.5f, -0.5f);
  glVertex3f(0.f, 2.3f, -0.6f);

  // STOMATA
  setNormal(0.f, 2.8f, 0.f, 0.05f, 1.9f, 0.f, -0.05f, 1.9f, 0.f);
  glVertex3f(0.f, 2.8f, 0.f);
  glVertex3f(0.05f, 1.9f, 0.f);
  glVertex3f(-0.05f, 1.9f, 0.f);

  setNormal(0.f, 2.8f, 0.f, 0.f, 1.9f, 0.05f, 0.f, 1.9f, -0.05f);
  glVertex3f(0.f, 2.8f, 0.f);
  glVertex3f(0.f, 1.9f, 0.05f);
  glVertex3f(0.f, 1.9f, -0.05f);

  glEnd();

  glBegin(GL_QUADS);

  // CUBES
  SetMaterial(0.9f, 0.7f, 0.1f, 1.f);
  for (int i = 0; i < 6; i++) {
    glVertex3f(0.f, 2.5f + i / 20., 0.f);
    glVertex3f(sin(i * F_PI_2) * .1f, 2.6f + i / 20., 0.f);
    glVertex3f(sin(i * F_PI_2) * .1f, 2.7f + i / 20., cos(i * F_PI_2) * .1f);
    glVertex3f(0.f, 2.6f + i / 20., cos(i * F_PI_2) * .1f);
  }

  glEnd();
}

void plant3() {
  // Plant 3:

  glBegin(GL_TRIANGLE_STRIP);

  float plantSegments = 25;
  float plantWidth = .05;

  for (int i = 0; i <= plantSegments; i++) {
    glColor3f(0.2, 0.2 + i / plantSegments / 2, 0.4);
    SetMaterial(0.4f, 0.4 + i / plantSegments / 2, 0.2f, 1.f);
    float ii = i * i * .002;
    glVertex3f(-sin(i / 10.0) * plantWidth + ii, i / 10.0, -cos(i / 10.0) * plantWidth + ii);
    glVertex3f(sin(i / 10.0) * plantWidth + ii, i / 10.0, cos(i / 10.0) * plantWidth + ii);
  }

  glEnd();

  glBegin(GL_TRIANGLES);

  float leavesPerSegment = 1.;
  float leafDistantce = 20.;
  float leafOffset = 0.;
  float leafWidth = 0.09;
  float leafBottomNudge = 0.09;

  // Leaves
  int tot = plantSegments * leavesPerSegment;
  for (int i = 0; i <= plantSegments * leavesPerSegment; i++) {
    glColor3f(0.2, 0.4 + i / (plantSegments * leavesPerSegment) / 2, 0.4 + i % (int)leavesPerSegment / leavesPerSegment / 2);
    SetMaterial(0.4f, 0.7 + i / (plantSegments * leavesPerSegment) / 2, 0.1 + i % (int)leavesPerSegment / leavesPerSegment / 2, 1.f);

    float ii = i * i * .002 / leavesPerSegment / leavesPerSegment;
    float stemY = i / 10. / leavesPerSegment;

    float normalAngle = i * F_PI * leavesPerSegment + F_PI_2;
    float normalX = sin(normalAngle);
    float normalZ = cos(normalAngle);

    float perpendicularAngle = normalAngle + F_PI / 2;
    float perpendicularX = sin(perpendicularAngle);
    float perpendicularZ = cos(perpendicularAngle);

    float x1 = ii + perpendicularX * leafWidth + normalX * leafOffset;
    float y1 = stemY + (tot - i) / 200.;
    float z1 = ii + perpendicularZ * leafWidth + normalZ * leafOffset;
    float x2 = ii - perpendicularX * leafWidth + normalX * leafOffset;
    float y2 = stemY;
    float z2 = ii - perpendicularZ * leafWidth + normalZ * leafOffset;
    float x3 = ii + normalX * (leafOffset + leafBottomNudge + (tot - i) / leafDistantce);
    float y3 = stemY + (tot - i) / 100.;
    float z3 = ii + normalZ * (leafOffset + leafBottomNudge + (tot - i) / leafDistantce);

    setNormal(x1, y1, z1, x2, y2, z2, x3, y3, z3);

    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
  }

  glEnd();
}

void plant4() {
  glBegin(GL_TRIANGLES);

  // STEM
  SetMaterial(0.f, .6f, 0.2f, 1.f);

  // setNormal(.1f, 0.f, 0.f, -.1f, 0.f, 0.f, 0.f, 2.f, 0.f);
  glNormal3f(0.000000, 1.000000, 0.000000);
  glVertex3f(.1f, 0.f, 0.f);
  glVertex3f(-.1f, 0.f, 0.f);
  glVertex3f(0.f, 2.f, 0.f);

  // setNormal(0.f, 0.f, 0.1f, 0.f, 0.f, -0.1f, 0.f, 2.f, 0.f);
  glNormal3f(0.000000, 0.000000, -1.000000);
  glVertex3f(0.f, 0.f, 0.1f);
  glVertex3f(0.f, 0.f, -0.1f);
  glVertex3f(0.f, 2.f, 0.f);

  SetMaterial(0.f, .3f, 0.2f, 1.f);

  setNormal(0.1f, 0.f, 0.f, -0.1f, 0.f, 0.f, 0., 1.f, 0.3f);
  glVertex3f(0.1f, 0.f, 0.f);
  glVertex3f(-0.1f, 0.f, 0.f);
  glVertex3f(0.f, 1.f, 0.3f);

  setNormal(0.f, 0.f, 0.1f, 0.f, 0.f, -0.1f, 0.4f, 1.3f, 0.f);
  glVertex3f(0.f, 0.f, 0.1f);
  glVertex3f(0.f, 0.f, -0.1f);
  glVertex3f(0.4f, 1.3f, 0.f);

  float plantSegments = 25;
  float leavesPerSegment = 15;
  float leafOffset = 0.4;
  float leafWidth = 0.09;
  float leafHeight = 0.08;
  float leafBottomNudge = 0.09;

  for (int i = 0; i <= plantSegments * leavesPerSegment; i++) {
    glColor3f(0.2, 0.4 + i / (plantSegments * leavesPerSegment) / 2, 0.4 + i % (int)leavesPerSegment / leavesPerSegment / 2);
    SetMaterial(0.8f, 0.5 + i / (plantSegments * leavesPerSegment) / 2, 0.4 + i % (int)leavesPerSegment / leavesPerSegment / 2, 1.f);
    // glColor3f(1, 0, 0);

    float ii = 0.5 / leavesPerSegment / leavesPerSegment;
    float stemY = i / 20. / leavesPerSegment;
    // glVertex3f(stemX, stemY, stemZ

    float normalAngle = i * 1.618 * leavesPerSegment + F_PI / 2;
    float normalX = sin(normalAngle);
    float normalZ = cos(normalAngle);
    // glVertex3f(ii + normalX * leafOffset, stemY, ii + normalZ * leafOffset

    // glColor3f(0, 1, 0);

    float perpendicularAngle = normalAngle + F_PI / 2;
    float perpendicularX = sin(perpendicularAngle);
    float perpendicularZ = cos(perpendicularAngle);

    float newleafoffset = (leafOffset - (i / (leavesPerSegment * plantSegments * 3)));

    float x1 = ii + perpendicularX * leafWidth + normalX * newleafoffset;
    float y1 = stemY + 1.3;
    float z1 = ii + perpendicularZ * leafWidth + normalZ * newleafoffset;

    float x2 = ii - perpendicularX * leafWidth + normalX * newleafoffset;
    float y2 = stemY + 1.3;
    float z2 = ii - perpendicularZ * leafWidth + normalZ * newleafoffset;

    float x3 = ii + normalX * (newleafoffset + leafBottomNudge);
    float y3 = stemY + 1.3 - leafHeight;
    float z3 = ii + normalZ * (newleafoffset + leafBottomNudge);

    setNormal(x1, y1, z1, x2, y2, z2, x3, y3, z3);

    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);

    // glVertex3f(ii + perpendicularX, stemY - 0.1, ii + perpendicularZ

    // glVertex3f(ii + sin(i / 10.0 + F_PI / 2 + (F_PI * 2 / leavesPerSegment)) * plantWidth, i / 10. / leavesPerSegment, ii + cos(i / 10.0 + F_PI / 2 + (F_PI * 2 / leavesPerSegment)) * plantWidth
  }

  glEnd();
}

void plant1() {
  glColor3f(0.2, 0.8, 0.4);
  SetMaterial(0.2f, 0.8f, 0.4f, 1.f);

  glBegin(GL_TRIANGLE_STRIP);

  float plantSegments = 25;
  float plantWidth = .05;

  for (int i = 0; i <= plantSegments; i++) {
    glColor3f(0.2, 0.2 + i / plantSegments / 2, 0.4);
    SetMaterial(0.2f, 0.2 + i / plantSegments / 2, 0.4f, 1.f);
    float ii = i * i * .001;
    glVertex3f(-sin(i / 10.0) * plantWidth + ii, i / 10.0, -cos(i / 10.0) * plantWidth + ii);
    glVertex3f(sin(i / 10.0) * plantWidth + ii, i / 10.0, cos(i / 10.0) * plantWidth + ii);
  }

  glEnd();

  glBegin(GL_TRIANGLES);
  // glBegin(GL_LINES);

  float leavesPerSegment = 5;
  float leafOffset = 0.1;
  float leafWidth = 0.09;
  float leafHeight = 0.08;
  float leafBottomNudge = 0.09;
  // glColor3f(1, 0, 0);

  // Leaves
  for (int i = 0; i <= plantSegments * leavesPerSegment; i++) {
    glColor3f(0.2, 0.4 + i / (plantSegments * leavesPerSegment) / 2, 0.4 + i % (int)leavesPerSegment / leavesPerSegment / 2);
    SetMaterial(0.2f, 0.4 + i / (plantSegments * leavesPerSegment) / 2, 0.4 + i % (int)leavesPerSegment / leavesPerSegment / 2, 1.f);
    // glColor3f(1, 0, 0);

    float ii = i * i * .001 / leavesPerSegment / leavesPerSegment;
    float stemY = i / 10. / leavesPerSegment;
    // glVertex3f(stemX, stemY, stemZ

    float normalAngle = i * 1.618 * leavesPerSegment + F_PI / 2;
    float normalX = sin(normalAngle);
    float normalZ = cos(normalAngle);
    // glVertex3f(ii + normalX * leafOffset, stemY, ii + normalZ * leafOffset

    // glColor3f(0, 1, 0);

    float perpendicularAngle = normalAngle + F_PI / 2;
    float perpendicularX = sin(perpendicularAngle);
    float perpendicularZ = cos(perpendicularAngle);

    glVertex3f(ii + perpendicularX * leafWidth + normalX * leafOffset, stemY, ii + perpendicularZ * leafWidth + normalZ * leafOffset);
    glVertex3f(ii - perpendicularX * leafWidth + normalX * leafOffset, stemY, ii - perpendicularZ * leafWidth + normalZ * leafOffset);

    glVertex3f(ii + normalX * (leafOffset + leafBottomNudge), stemY - leafHeight, ii + normalZ * (leafOffset + leafBottomNudge));

    // glVertex3f(ii + perpendicularX, stemY - 0.1, ii + perpendicularZ

    // glVertex3f(ii + sin(i / 10.0 + F_PI / 2 + (F_PI * 2 / leavesPerSegment)) * plantWidth, i / 10. / leavesPerSegment, ii + cos(i / 10.0 + F_PI / 2 + (F_PI * 2 / leavesPerSegment)) * plantWidth
  }
  // MARK: Flowers

  glEnd();
  glBegin(GL_TRIANGLES);
  // glBegin(GL_LINES);

  float flowersPerSegment = 0.5;
  float flowerOffset = 0.2;
  float flowerTilt = 0.02;
  float flowerPetals = 5;
  float flowerPetalLength = 0.1;
  float flowerPetalWidth = 0.05;

  for (int i = 0; i <= plantSegments * flowersPerSegment; i++) {
    // glColor3f(1, 0, 0);
    float ii = i * i * .001 / flowersPerSegment / flowersPerSegment;
    float stemY = i / 10. / flowersPerSegment;
    // glVertex3f(stemX, stemY, stemZ

    float normalAngle = i * 1.618 * flowersPerSegment + F_PI / 2;
    float normalX = sin(normalAngle);
    float normalZ = cos(normalAngle);

    float baseX = ii + normalX * flowerOffset;
    float baseY = stemY;
    float baseZ = ii + normalZ * flowerOffset;

    // glVertex3f(baseX, baseY, baseZ

    for (int j = 0; j < flowerPetals; j++) {
      // glColor3f(0, 1, 0);
      float petalAngle = j * flowerPetals + F_PI / 2;
      float petalX = sin(petalAngle);
      float petalY = cos(petalAngle);
      float petalZ = 0;

      float rotPetalX = petalX * sin(normalAngle + F_PI / 2) - petalZ * cos(normalAngle + F_PI / 2);
      float rotPetalZ = petalX * cos(normalAngle + F_PI / 2) + petalZ * sin(normalAngle + F_PI / 2);

      float tiltX = normalX * flowerTilt;
      float tiltZ = normalZ * flowerTilt;

      float peakX = baseX + rotPetalX * flowerPetalLength + tiltX;
      float peakY = stemY + petalY * flowerPetalLength;
      float peakZ = baseZ + rotPetalZ * flowerPetalLength + tiltZ;

      float tipX = baseX + rotPetalX * flowerPetalLength * 2;
      float tipY = stemY + petalY * flowerPetalLength * 2;
      float tipZ = baseZ + rotPetalZ * flowerPetalLength * 2;

      float crossX = 0 * (tipZ - baseZ) - normalZ * (tipY - baseY);
      float crossY = normalZ * (tipX - baseX) - normalX * (tipZ - baseZ);
      float crossZ = normalX * (tipY - baseY) - 0 * (tipX - baseX);

      float crossLen = sqrt(crossX * crossX + crossY * crossY + crossZ * crossZ);
      crossX = crossX / crossLen * flowerPetalWidth;
      crossY = crossY / crossLen * flowerPetalWidth;
      crossZ = crossZ / crossLen * flowerPetalWidth;

      float leftSideX = peakX + crossX;
      float leftSideY = peakY + crossY;
      float leftSideZ = peakZ + crossZ;

      float rightSideX = peakX - crossX;
      float rightSideY = peakY - crossY;
      float rightSideZ = peakZ - crossZ;

      // glVertex3f(baseX, stemY, baseZ
      // glVertex3f(peakX, peakY, peakZ

      // glColor3f(0, 1, 1);
      // glVertex3f(peakX, peakY, peakZ
      // glVertex3f(tipX, tipY, tipZ

      // glColor3f(0, 0, 1);
      // glVertex3f(tipX + tiltX, tipY, tipZ + tiltZ
      // glVertex3f(tipX, tipY, tipZ

      // glColor3f(1, 0, 1);
      // glVertex3f(leftSideX, leftSideY, leftSideZ
      // glVertex3f(rightSideX, rightSideY, rightSideZ

      glColor3f(0.9, 0.3, 0.1);
      SetMaterial(0.9f, 0.3f, 0.1f, 1.f);

      glVertex3f(baseX, stemY, baseZ);

      glVertex3f(leftSideX, leftSideY, leftSideZ);

      glVertex3f(rightSideX, rightSideY, rightSideZ);

      glColor3f(1, 0.7, 0.7);
      SetMaterial(1.f, 0.7f, 0.7f, 1.f);
      glVertex3f(leftSideX, leftSideY, leftSideZ);

      glVertex3f(rightSideX, rightSideY, rightSideZ);

      glVertex3f(tipX, tipY, tipZ);
    }
  }

  glEnd();
}

void setup_animations() {
  scaleXZ.Init();
  scaleXZ.AddTimeValue(0.f * TOTAL, 0.f);
  scaleXZ.AddTimeValue(0.8f * TOTAL, 0.2f);
  scaleXZ.AddTimeValue(1.f * TOTAL, 0.5f);
  scaleXZ.AddTimeValue(1.4f * TOTAL, 0.8f);
  scaleXZ.AddTimeValue(1.6f * TOTAL, 0.9f);
  scaleXZ.AddTimeValue(1.8f * TOTAL, 0.0f);

  scaleY.Init();
  scaleY.AddTimeValue(0.f * TOTAL, 0.f);
  scaleY.AddTimeValue(0.3f * TOTAL, 0.2f);
  scaleY.AddTimeValue(0.5f * TOTAL, 0.2f);
  scaleY.AddTimeValue(0.8f * TOTAL, 0.6f);
  scaleY.AddTimeValue(1.4f * TOTAL, 0.8f);
  scaleY.AddTimeValue(1.6f * TOTAL, 1.f);
  scaleY.AddTimeValue(1.8f * TOTAL, 0.0f);

  rotation.Init();
  rotation.AddTimeValue(0.f * TOTAL, 0.f);
  rotation.AddTimeValue(0.5f * TOTAL, 500.f);
  rotation.AddTimeValue(1.0f * TOTAL, 700.f);
  rotation.AddTimeValue(1.6f * TOTAL, 700.f);
  rotation.AddTimeValue(1.75f * TOTAL, 500.f);
  rotation.AddTimeValue(1.8f * TOTAL, 1000.0f);
}

void create_plants() {
  Plant1List = glGenLists(2);
  glNewList(Plant1List, GL_COMPILE);
  plant1();
  glEndList();

  Plant2List = glGenLists(1);
  glNewList(Plant2List, GL_COMPILE);
  plant2();
  glEndList();

  Plant3List = glGenLists(1);
  glNewList(Plant3List, GL_COMPILE);
  plant3();
  glEndList();

  Plant4List = glGenLists(1);
  glNewList(Plant4List, GL_COMPILE);
  plant4();
  glEndList();

  setup_animations();
}