#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI ((float)(M_PI))
#define F_2_PI ((float)(2.f * F_PI))
#define F_PI_2 ((float)(F_PI / 2.f))
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

#include "glut.h"

#ifndef TOTAL
#define TOTAL 30.f
#endif

//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// title of these windows:

const char *WINDOWTITLE = "OpenGL Project 4 - Aidan Schmitigal";
const char *GLUITITLE = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT = 4;
const int MIDDLE = 2;
const int RIGHT = 1;

// which projection:

enum Projections {
  ORTHO,
  PERSP
};

// which button:

enum ButtonVals {
  RESET,
  QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = {0., 0., 0., 1.};

// line width for the axes:

const GLfloat AXES_WIDTH = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors {
  WHITE,
  RED,
  YELLOW,
  GREEN,
  CYAN,
  BLUE,
  MAGENTA
};

const int Colors_Length = 7;

char *ColorNames[] =
    {
        (char *)"White",
        (char *)"Red",
        (char *)"Yellow",
        (char *)"Green",
        (char *)"Cyan",
        (char *)"Blue",
        (char *)"Magenta"};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
    {
        {1., 1., 1.},  // white
        {1., 0., 0.},  // red
        {1., 1., 0.},  // yellow
        {0., 1., 0.},  // green
        {0., 1., 1.},  // cyan
        {0., 0., 1.},  // blue
        {1., 0., 1.},  // magenta
};

// these are here for when you need them -- just uncomment the ones you need:
// MAKR: Includes

// fog parameters:

const GLfloat FOGCOLOR[4] = {.0f, .0f, .0f, 1.f};
const GLenum FOGMODE = GL_LINEAR;
const GLfloat FOGDENSITY = 0.30f;
const GLfloat FOGSTART = 1.5f;
const GLfloat FOGEND = 4.f;

// for lighting:

const float WHITE_ARRAY[4] = {1., 1., 1., 1.};

// for animation:

const int MS_PER_CYCLE = 1000;  // 1000 milliseconds = 1 seconds

// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
// #define DEMO_Z_FIGHTING
// #define DEMO_DEPTH_BUFFER

// non-constant global variables:

#define BLADE_RADIUS 1.0
#define BLADE_WIDTH 0.4

GLuint PotList;

GLuint WateringCanList;
GLuint WaterDropletList;
GLuint SeedPouchList;
GLuint bouncingFrogList;
GLuint DirtMoundList;
GLuint GroundPlaneList;

int ActiveButton;     // current button that is down
GLuint AxesList;      // list to hold the axes
int AxesOn;           // != 0 means to draw the axes
int DebugOn;          // != 0 means to print debugging info
int DepthCueOn;       // != 0 means to use intensity depth cueing
int DepthBufferOn;    // != 0 means to use the z-buffer
int DepthFightingOn;  // != 0 means to force the creation of z-fighting
int MainWindow;       // window id for main graphics window
int NowColor;         // index into Colors[ ]
int LightMode = 1;    // 0 = point light, 1 = spot light
int NowProjection;    // ORTHO or PERSP
float Scale;          // scaling factor
int ShadowsOn;        // != 0 means to turn shadows on
float Time;           // used for animation, this has a value between 0. and 1.
int Xmouse, Ymouse;   // mouse values
float Xrot, Yrot;     // rotation angles in degrees
float Ytanslate;
int FreezeAnimation = 0;

int selectedColor = GREEN;

// Scene Time in milliseconds
long scene_ms = 0;
// Change in Scene Time in seconds
float delta_time = 0;

// function prototypes:

void Animate();
void Display();
void DoAxesMenu(int);
void DoColorMenu(int);
void DoDepthBufferMenu(int);
void DoDepthFightingMenu(int);
void DoDepthMenu(int);
void DoDebugMenu(int);
void DoMainMenu(int);
void DoProjectMenu(int);
void DoRasterString(float, float, float, char *);
void DoStrokeString(float, float, float, float, char *);
float ElapsedSeconds();
void InitGraphics();
void InitAnimations();
void InitLists();
void InitMenus();
void Keyboard(unsigned char, int, int);
void MouseButton(int, int, int, int);
void MouseMotion(int, int);
void Reset();
void Resize(int, int);
void Visibility(int);

void Axes(float);
void HsvRgb(float[3], float[3]);
void Cross(float[3], float[3], float[3]);
float Dot(float[3], float[3]);
float Unit(float[3], float[3]);
float Unit(float[3]);

// utility to create an array from 3 separate values:

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

#include "bmptotexture.cpp"
// #include "glslprogram.cpp"
#include "heli.550"
#include "keytime.cpp"
#include "loadobjfile.cpp"
#include "osucone.cpp"
#include "osugrid.cpp"
#include "osusphere.cpp"
#include "osutorus.cpp"
#include "particles.cpp"
#include "pillpath.cpp"
#include "plants.cpp"
#include "setlight.cpp"
#include "setmaterial.cpp"

Keytimes cameraXKeyframe;
Keytimes cameraZKeyframe;

Keytimes cameraLookAtXKeyframe;
Keytimes cameraLookAtZKeyframe;

Keytimes lightXKeyframe;
Keytimes lightZKeyframe;
Keytimes lightRedKeyframe;
Keytimes lightGreenKeyframe;
Keytimes lightBlueKeyframe;

Keytimes bouncingFrogYKeyframe;
Keytimes bouncingFrogScaleXZKeyframe;
Keytimes bouncingFrogScaleYKeyframe;

// main program:

// MARK: Main
int main(int argc, char *argv[]) {
  // turn on the glut package:
  // (do this before checking argc and argv since glutInit might
  // pull some command line arguments out)

  glutInit(&argc, argv);

  // setup all the graphics stuff:

  InitGraphics();
  noise_setseed(0);

  // create the display lists that **will not change**:

  InitLists();

  InitAnimations();

  // init all the global variables used by Display( ):
  // this will also post a redisplay

  Reset();

  // setup all the user interface stuff:

  InitMenus();

  // draw the scene once and wait for some interaction:
  // (this will never return)

  glutSetWindow(MainWindow);
  glutMainLoop();

  // glutMainLoop( ) never actually returns
  // the following line is here to make the compiler happy:

  return 0;
}

// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it
// MARK: Animate
int ms = 0;
void Animate() {
  // put animation stuff in here -- change some global variables for Display( ) to find:

  ms = glutGet(GLUT_ELAPSED_TIME);
  if (FreezeAnimation == 0) {
    delta_time = (ms - scene_ms) / 1000.f;
    scene_ms = ms;

    // Yrot += 2 * delta_time;
  }

  // for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

  // force a call to Display( ) next time it is convenient:

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

float min(float a, float b) {
  return a > b ? b : a;
}

float max(float a, float b) {
  return a > b ? a : b;
}

// draw the complete scene:
// MARK: Display
void Display() {
  if (DebugOn != 0)
    fprintf(stderr, "Starting Display.\n");

  // set which window we want to do the graphics into:
  glutSetWindow(MainWindow);

  // erase the background:
  glDrawBuffer(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
#ifdef DEMO_DEPTH_BUFFER
  if (DepthBufferOn == 0)
    glDisable(GL_DEPTH_TEST);
#endif

  // specify shading to be flat:

  glShadeModel(GL_FLAT);

  // set the viewport to be a square centered in the window:

  GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
  GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
  GLsizei v = vx < vy ? vx : vy;  // minimum dimension
  GLint xl = (vx - v) / 2;
  GLint yb = (vy - v) / 2;
  glViewport(xl, yb, v, v);

  // set the viewing volume:
  // remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
  // USE gluOrtho2D( ) IF YOU ARE DOING 2D !

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (NowProjection == ORTHO)
    glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
  else
    gluPerspective(70.f, 1.f, 0.1f, 1000.f);

  // place the objects into the scene:

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_LIGHTING);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Array3(0.3f, 0.3f, 0.3f));

  // set the eye position, look-at position, and up-vector:

  // cameraXKeyframe.GetValue
  // printf("%f\n", scene_ms * 10000.);
  // printf("%f\n", cameraXKeyframe.GetValue(fmod(scene_ms * .0001, 2.)));
  // gluLookAt(cos(cameraXKeyframe.GetValue(fmod(scene_ms * .0001, 2.))) * 3.f, 0.f, sin(cameraXKeyframe.GetValue(fmod(scene_ms * .0001, 2.))) * 3.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
  // printf("%f, %f\n", sin(scene_ms * .0001 / TOTAL * F_PI * 2), cos(scene_ms * .0001 / TOTAL * F_PI * 2));
  // gluLookAt(0.f, 0.f, 3.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
  gluLookAt(cameraXKeyframe.GetValue(fmod(scene_ms * .0001, 1.)) * 2, 1.f, cameraZKeyframe.GetValue(fmod(scene_ms * .0001, 1.)) * 2, -cameraLookAtXKeyframe.GetValue(fmod(scene_ms * .0001, 1.)), 0.f, 0.f, 0.f, 1.f, 0.f);
  // gluLookAt(0.f, 0.f, cameraXKeyframe.GetValue(fmod(scene_ms * .0001, 2.)), 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

  // rotate the scene:

  // glRotatef(Yrot, 0.f, 1.f, 0.f);
  float upDir[] = {0, 1, 0};
  float lookDir[] = {cos((Yrot + 90) * F_PI / 180.f), 0, sin((Yrot + 90) * F_PI / 180.f)};

  float xRotDir[3];                                            // Output direction vector
  xRotDir[0] = upDir[1] * lookDir[2] - upDir[2] * lookDir[1];  // x-component
  xRotDir[1] = upDir[2] * lookDir[0] - upDir[0] * lookDir[2];  // y-component
  xRotDir[2] = upDir[0] * lookDir[1] - upDir[1] * lookDir[0];  // z-component

  // glRotatef((GLfloat)Xrot, xRotDir[0], xRotDir[1], xRotDir[2]);
  glTranslatef(0, Ytanslate, 0);

  // uniformly scale the scene:

  if (Scale < MINSCALE)
    Scale = MINSCALE;
  glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

  // set the fog parameters:

  if (DepthCueOn != 0) {
    glFogi(GL_FOG_MODE, FOGMODE);
    glFogfv(GL_FOG_COLOR, FOGCOLOR);
    glFogf(GL_FOG_DENSITY, FOGDENSITY);
    glFogf(GL_FOG_START, FOGSTART);
    glFogf(GL_FOG_END, FOGEND);
    glEnable(GL_FOG);
  } else {
    glDisable(GL_FOG);
  }

  // possibly draw the axes:

  if (AxesOn != 0) {
    glColor3fv(&Colors[NowColor][0]);
    glCallList(AxesList);
  }

  // since we are using glScalef( ), be sure the normals get unitized:

  glEnable(GL_NORMALIZE);

  // draw the box object by calling up its display list:
  // MARK: Draw Lists

  glShadeModel(GL_FLAT);
  // glShadeModel(GL_SMOOTH);

  update_particles(delta_time);
  update_plants(delta_time);

  render_particles();
  render_plants();

  /***************************************************
   * ====     GARDEN SIGN     ====
   ***************************************************/

  glPushMatrix();
  glTranslatef(-5.f, -1.5f, -1.f);
  glScalef(0.8f, 0.8f, 0.8f);
  glRotatef(45.f, 0.f, 1.f, 0.f);
  glTranslatef(0, bouncingFrogYKeyframe.GetValue(fmod(scene_ms * 0.0005, 1)), 0);
  glScalef(bouncingFrogScaleXZKeyframe.GetValue(fmod(scene_ms * 0.0005, 1)), bouncingFrogScaleYKeyframe.GetValue(fmod(scene_ms * 0.0005, 1)), bouncingFrogScaleXZKeyframe.GetValue(fmod(scene_ms * 0.0005, 1)));
  glCallList(bouncingFrogList);
  glPopMatrix();

  /***************************************************
   * ====     GROUND PLANE     ====
   ***************************************************/
  glPushMatrix();

  glTranslatef(0, -1.1, 0);
  glScalef(20, 1, 10);
  SetMaterial(0.4f, 0.6f, 0.1f, 1.f);
  glCallList(GroundPlaneList);
  // glCallList(WateringCanList);
  glPopMatrix();

  /***************************************************
   * ====     DIRT MOUNDS     ====
   ***************************************************/

  int mound_count = 25;
  SetMaterial(0.7f, 0.5f, 0.1f, 1.f);
  for (int i = 0; i < mound_count; i++) {
    glPushMatrix();

    float coords[2];
    float rotation;
    pill_path(i * 1. / mound_count, 5, 2, coords, &rotation);

    glTranslatef(coords[0], sin(i) * 0.1 - 1, coords[1]);
    glScalef(0.75 + 0.25 * sin(i * F_PI / mound_count), 1 + 0.25 * sin(i), 0.75 + 0.25 * sin(i * F_PI / mound_count));
    glCallList(DirtMoundList);

    glPopMatrix();
  }

  /***************************************************
   * ====     BEGIN ANIMATION     ====
   ***************************************************/

  float total_time_loop = TOTAL;
  // float total_time_loop = 30.f;

  float time = fmod((scene_ms) / (total_time_loop * 1000), 1.);

  /***************************************************
   * ====     WATERING CAN     ====
   ***************************************************/
  glPushMatrix();

  float coords[2];
  float rotation;
  pill_path(time, 5, 2, coords, &rotation);
  glTranslatef(coords[0], 0, coords[1]);

  // Floating movement:
  float float_animation_speed = 100;
  float offset_x = cos(time * float_animation_speed) * .5;
  float offset_y = sin(2 * time * float_animation_speed - F_PI_2 + 1) * .25;

  glRotatef(rotation, 0, 1, 0);
  glTranslatef(offset_x, offset_y, 0);

  glRotatef(cos(time * float_animation_speed) * 30, 0, 1, 0);
  glRotatef(sin(2 * time * float_animation_speed - F_PI_2 + 1) * 10 - 30, 1, 0, 0);

  GLfloat modelViewMatrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);

  SetMaterial(0.7f, 0.9f, 0.9f, 100.f);
  glCallList(WateringCanList);

  glPopMatrix();

  /***************************************************
   * ====     WATER PARTICLES     ====
   ***************************************************/

  glPushMatrix();
  glLoadIdentity();

  glTranslatef(coords[0], 0, coords[1]);
  glRotatef(rotation, 0, 1, 0);
  glTranslatef(offset_x, offset_y, 0);
  glRotatef(cos(time * float_animation_speed) * 30, 0, 1, 0);
  glRotatef(sin(2 * time * float_animation_speed - F_PI_2 + 1) * 10 - 30, 1, 0, 0);

  glTranslatef(0.f, 1.3f, -1.2f);

  GLfloat mvm[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, mvm);
  // printf("\n\n[ %f, %f, %f, %f\n  %f, %f, %f, %f\n  %f, %f, %f, %f\n  %f, %f, %f, %f]", mvm[0], mvm[1], mvm[2], mvm[3], mvm[4], mvm[5], mvm[6], mvm[7], mvm[8], mvm[9], mvm[10], mvm[11], mvm[12], mvm[13], mvm[14], mvm[15]);
  // float final_position[] = {mvm[12], mvm[13], mvm[14]};

  glPopMatrix();
  glPushMatrix();

  glTranslatef(mvm[12], mvm[13], mvm[14]);

  // float rotated_motion[2] = {x cos(rotation) - y sin(rotation), x sin(rotation) + y cos(rotation)};
  float rad_rot = rotation * F_PI / 180.f + F_PI_2;
  switch (rand() % 9) {
    case 0:
      spawn_particle(mvm[12], mvm[13] + 0.05, mvm[14], cos(rad_rot - .4) * 5, 0.f, -sin(rad_rot - .4) * 5, 0.1f, 0.6f, 0.8f);
      break;
    case 1:
      spawn_particle(mvm[12], mvm[13] + 0.1, mvm[14], cos(rad_rot) * 5, 0.f, -sin(rad_rot) * 5, 0.1f, 0.6f, 0.8f);
      break;
    case 2:
      spawn_particle(mvm[12], mvm[13] + 0.05, mvm[14], cos(rad_rot + .4) * 5, 0.f, -sin(rad_rot + .4) * 5, 0.1f, 0.6f, 0.8f);
      break;
    case 3:
      spawn_particle(mvm[12], mvm[13] + 0.1, mvm[14], cos(rad_rot - .4) * 3, 1.f, -sin(rad_rot - .4) * 3, 0.1f, 0.6f, 0.8f);
      break;
    case 4:
      spawn_particle(mvm[12], mvm[13], mvm[14], cos(rad_rot) * 5, 1.f, -sin(rad_rot) * 5, 0.1f, 0.6f, 0.8f);
      break;
    case 5:
      spawn_particle(mvm[12], mvm[13] - 0.05, mvm[14], cos(rad_rot + .4) * 3, 1.f, -sin(rad_rot + .4) * 3, 0.1f, 0.6f, 0.8f);
      break;
    case 6:
      spawn_particle(mvm[12], mvm[13] - 0.1, mvm[14], cos(rad_rot - .4) * 3, -.8f, -sin(rad_rot - .4) * 3, 0.1f, 0.6f, 0.8f);
      break;
    case 7:
      spawn_particle(mvm[12], mvm[13] - 0.05, mvm[14], cos(rad_rot) * 3, -.8f, -sin(rad_rot) * 3, 0.1f, 0.6f, 0.8f);
      break;
    case 8:
      spawn_particle(mvm[12], mvm[13] - 0.1, mvm[14], cos(rad_rot + .4) * 3, -.8f, -sin(rad_rot + .4) * 3, 0.1f, 0.6f, 0.8f);
      break;
  }

  glPopMatrix();

  /***************************************************
   * ====     SEED POUCH     ====
   ***************************************************/
  glPushMatrix();
  pill_path(fmod(time + 0.2, 1), 5, 2, coords, &rotation);
  glTranslatef(coords[0], 0, coords[1]);
  glRotatef(rotation - 90, 0, 1, 0);

  // 6x^{5}-15x^{4}+10x^{3}
  float scaled_time = fmod(time * 50, 1);
  float smoothed_time = 6 * scaled_time * scaled_time * scaled_time * scaled_time * scaled_time - 15 * scaled_time * scaled_time * scaled_time * scaled_time + 10 * scaled_time * scaled_time * scaled_time;
  float shake_x = cos(time * float_animation_speed) * .5;
  float shake_y = sin(smoothed_time * F_PI);
  float shake_z = sin(time * float_animation_speed) * .5;
  glTranslatef(shake_x, shake_y, shake_z);

  glScalef(.3f, .3f, .3f);

  SetMaterial(0.1f, 0.8f, 0.7f, 10.f);
  glCallList(SeedPouchList);
  glPopMatrix();

  /***************************************************
   * ====     SEED PARTICLES     ====
   ***************************************************/

  glPushMatrix();

  SetMaterial(1.f, 0.f, 1.f, 0.f);
  rad_rot = -rotation * F_PI / 180.f + F_PI_2;
  float rotatex = (shake_x + .2f) * cos(rad_rot) - shake_z * sin(rad_rot);
  float rotatez = (shake_x + .2f) * sin(rad_rot) + shake_z * cos(rad_rot);
  // glTranslatef(rotatex + coords[0], shake_y, rotatez + coords[1]);

  float seed_red = rand() % 255 / 255.;
  float seed_green = rand() % 255 / 255.;
  float seed_blue = rand() % 255 / 255.;
  if (int(time * 1000) % 10 < 2) {
    switch (rand() % 3) {
      case 0:
        spawn_particle(rotatex + coords[0], shake_y, rotatez + coords[1], cos(rad_rot - .4) * 2, 1.5f, sin(rad_rot - .4) * 2, seed_red, seed_blue, seed_green);
        break;
      case 1:
        spawn_particle(rotatex + coords[0], shake_y, rotatez + coords[1], cos(rad_rot), 0.f, sin(rad_rot), seed_red, seed_blue, seed_green);
        break;
      case 2:
        spawn_particle(rotatex + coords[0], shake_y, rotatez + coords[1], cos(rad_rot + .4), 1.f, sin(rad_rot + .4), seed_red, seed_blue, seed_green);
        break;
    }
    add_random_plant(coords[0], -1, rotatez + coords[1]);
  }
  glPopMatrix();

  /***************************************************
   * ====     LIGHTING     ====
   ***************************************************/

  // SetPointLight(GL_LIGHT0, sin(scene_ms / 1000.) * 10, 7, cos(scene_ms / 1000.) * 10, 0.4f, 0.4f, 0.4f);
  // printf("%f, %f, %f\n", fmod(scene_ms * .001, TOTAL), lightXKeyframe.GetValue(fmod(scene_ms * .001, TOTAL)), TOTAL);
  SetPointLight(GL_LIGHT0, lightXKeyframe.GetValue(fmod(scene_ms * 0.001, TOTAL)), 2, lightZKeyframe.GetValue(fmod(scene_ms * 0.001, TOTAL)), lightRedKeyframe.GetValue(fmod(scene_ms * 0.001, TOTAL)), lightGreenKeyframe.GetValue(fmod(scene_ms * 0.001, TOTAL)), lightBlueKeyframe.GetValue(fmod(scene_ms * 0.001, TOTAL)));

  glDisable(GL_LIGHTING);

#ifdef DEMO_Z_FIGHTING
  if (DepthFightingOn != 0) {
    glPushMatrix();
    glRotatef(90.f, 0.f, 1.f, 0.f);
    glCallList(BoxList);
    glPopMatrix();
  }
#endif

  // draw some gratuitous text that just rotates on top of the scene:
  // i commented out the actual text-drawing calls -- put them back in if you have a use for them
  // a good use for thefirst one might be to have your name on the screen
  // a good use for the second one might be to have vertex numbers on the screen alongside each vertex

  glDisable(GL_DEPTH_TEST);
  glColor3f(0.f, 1.f, 1.f);
  // DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );

  // draw some gratuitous text that is fixed on the screen:
  //
  // the projection matrix is reset to define a scene whose
  // world coordinate system goes from 0-100 in each axis
  //
  // this is called "percent units", and is just a convenience
  //
  // the modelview matrix is reset to identity as we don't
  // want to transform these coordinates

  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.f, 100.f, 0.f, 100.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(1.f, 1.f, 1.f);
  // DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

  // swap the double-buffered framebuffers:

  glutSwapBuffers();

  // be sure the graphics buffer has been sent:
  // note: be sure to use glFlush( ) here, not glFinish( ) !

  glFlush();
}

void DoInsideMenu(int id) {
  // InsideView = id;

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

void DoAxesMenu(int id) {
  AxesOn = id;

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

void DoColorMenu(int id) {
  NowColor = id - RED;

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

void DoDebugMenu(int id) {
  DebugOn = id;

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

void DoDepthBufferMenu(int id) {
  DepthBufferOn = id;

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

void DoDepthFightingMenu(int id) {
  DepthFightingOn = id;

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

void DoDepthMenu(int id) {
  DepthCueOn = id;

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

// main menu callback:
// MARK: DoMainMenu
void DoMainMenu(int id) {
  switch (id) {
    case RESET:
      Reset();
      break;

    case QUIT:
      // gracefully close out the graphics:
      // gracefully close the graphics window:
      // gracefully exit the program:
      glutSetWindow(MainWindow);
      glFinish();
      glutDestroyWindow(MainWindow);
      exit(0);
      break;

    default:
      fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
  }

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

void DoProjectMenu(int id) {
  NowProjection = id;

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

// use glut to display a string of characters using a raster font:

void DoRasterString(float x, float y, float z, char *s) {
  glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

  char c;  // one character to print
  for (; (c = *s) != '\0'; s++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
  }
}

// use glut to display a string of characters using a stroke font:

void DoStrokeString(float x, float y, float z, float ht, char *s) {
  glPushMatrix();
  glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
  float sf = ht / (119.05f + 33.33f);
  glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
  char c;  // one character to print
  for (; (c = *s) != '\0'; s++) {
    glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
  }
  glPopMatrix();
}

// return the number of seconds since the start of the program:

float ElapsedSeconds() {
  // get # of milliseconds since the start of the program:

  int ms = glutGet(GLUT_ELAPSED_TIME);

  // convert it to seconds:

  return (float)ms / 1000.f;
}

// initialize the glui window:
// MARK: InitMenus
void InitMenus() {
  if (DebugOn != 0)
    fprintf(stderr, "Starting InitMenus.\n");

  glutSetWindow(MainWindow);

  int numColors = sizeof(Colors) / (3 * sizeof(float));
  int colormenu = glutCreateMenu(DoColorMenu);
  for (int i = 0; i < numColors; i++) {
    glutAddMenuEntry(ColorNames[i], i);
  }

  int camerapositionmenu = glutCreateMenu(DoInsideMenu);
  glutAddMenuEntry("Outside", 0);
  glutAddMenuEntry("Inside", 1);

  // int axesmenu = glutCreateMenu(DoAxesMenu);
  // glutAddMenuEntry("Off", 0);
  // glutAddMenuEntry("On", 1);

  int depthcuemenu = glutCreateMenu(DoDepthMenu);
  glutAddMenuEntry("Off", 0);
  glutAddMenuEntry("On", 1);

  int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
  glutAddMenuEntry("Off", 0);
  glutAddMenuEntry("On", 1);

  int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
  glutAddMenuEntry("Off", 0);
  glutAddMenuEntry("On", 1);

  int debugmenu = glutCreateMenu(DoDebugMenu);
  glutAddMenuEntry("Off", 0);
  glutAddMenuEntry("On", 1);

  int projmenu = glutCreateMenu(DoProjectMenu);
  glutAddMenuEntry("Orthographic", ORTHO);
  glutAddMenuEntry("Perspective", PERSP);

  int mainmenu = glutCreateMenu(DoMainMenu);
  // glutAddSubMenu("Inside View", camerapositionmenu);
  // glutAddSubMenu("Axes", axesmenu);
  glutAddSubMenu("Light Colors", colormenu);

#ifdef DEMO_DEPTH_BUFFER
  glutAddSubMenu("Depth Buffer", depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
  glutAddSubMenu("Depth Fighting", depthfightingmenu);
#endif

  glutAddSubMenu("Depth Cue", depthcuemenu);
  glutAddSubMenu("Projection", projmenu);
  glutAddMenuEntry("Reset", RESET);
  glutAddSubMenu("Debug", debugmenu);
  glutAddMenuEntry("Quit", QUIT);

  // attach the pop-up menu to the right mouse button:

  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// initialize the glut and OpenGL libraries:
//	also setup callback functions

// MARK: InitGraphics
void InitGraphics() {
  if (DebugOn != 0)
    fprintf(stderr, "Starting InitGraphics.\n");

  // request the display modes:
  // ask for red-green-blue-alpha color, double-buffering, and z-buffering:

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  // set the initial window configuration:

  glutInitWindowPosition(0, 0);
  glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

  // open the window and set its title:

  MainWindow = glutCreateWindow(WINDOWTITLE);
  glutSetWindowTitle(WINDOWTITLE);

  // set the framebuffer clear values:

  glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

  // setup the callback functions:
  // DisplayFunc -- redraw the window
  // ReshapeFunc -- handle the user resizing the window
  // KeyboardFunc -- handle a keyboard input
  // MouseFunc -- handle the mouse button going down or up
  // MotionFunc -- handle the mouse moving with a button down
  // PassiveMotionFunc -- handle the mouse moving with a button up
  // VisibilityFunc -- handle a change in window visibility
  // EntryFunc	-- handle the cursor entering or leaving the window
  // SpecialFunc -- handle special keys on the keyboard
  // SpaceballMotionFunc -- handle spaceball translation
  // SpaceballRotateFunc -- handle spaceball rotation
  // SpaceballButtonFunc -- handle spaceball button hits
  // ButtonBoxFunc -- handle button box hits
  // DialsFunc -- handle dial rotations
  // TabletMotionFunc -- handle digitizing tablet motion
  // TabletButtonFunc -- handle digitizing tablet button hits
  // MenuStateFunc -- declare when a pop-up menu is in use
  // TimerFunc -- trigger something to happen a certain time from now
  // IdleFunc -- what to do when nothing else is going on

  glutSetWindow(MainWindow);
  glutDisplayFunc(Display);
  glutReshapeFunc(Resize);
  glutKeyboardFunc(Keyboard);
  glutMouseFunc(MouseButton);
  glutMotionFunc(MouseMotion);
  glutPassiveMotionFunc(MouseMotion);
  // glutPassiveMotionFunc( NULL );
  glutVisibilityFunc(Visibility);
  glutEntryFunc(NULL);
  glutSpecialFunc(NULL);
  glutSpaceballMotionFunc(NULL);
  glutSpaceballRotateFunc(NULL);
  glutSpaceballButtonFunc(NULL);
  glutButtonBoxFunc(NULL);
  glutDialsFunc(NULL);
  glutTabletMotionFunc(NULL);
  glutTabletButtonFunc(NULL);
  glutMenuStateFunc(NULL);
  glutTimerFunc(-1, NULL, 0);

  // setup glut to call Animate( ) every time it has
  // 	nothing it needs to respond to (which is most of the time)
  // we don't need to do this for this program, and really should set the argument to NULL
  // but, this sets us up nicely for doing animation

  glutIdleFunc(Animate);

  // init the glew package (a window must be open to do this):

#ifdef WIN32
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "glewInit Error\n");
  } else
    fprintf(stderr, "GLEW initialized OK\n");
  fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

  // all other setups go here, such as GLSLProgram and KeyTime setups:
}

// MARK: InitAnimations
void InitAnimations() {
  cameraXKeyframe.Init();

  cameraXKeyframe.AddTimeValue(0., 0.000000);
  cameraXKeyframe.AddTimeValue(0.1, 0.587785);
  cameraXKeyframe.AddTimeValue(0.2, 0.951057);
  cameraXKeyframe.AddTimeValue(0.3, 0.951057);
  cameraXKeyframe.AddTimeValue(0.4, 0.587785);
  cameraXKeyframe.AddTimeValue(0.5, -0.000000);
  cameraXKeyframe.AddTimeValue(0.6, -0.587785);
  cameraXKeyframe.AddTimeValue(0.7, -0.951057);
  cameraXKeyframe.AddTimeValue(0.8, -0.951056);
  cameraXKeyframe.AddTimeValue(0.9, -0.587785);
  cameraXKeyframe.AddTimeValue(1., -0.000000);

  cameraZKeyframe.Init();

  cameraZKeyframe.AddTimeValue(0., 1.000000);
  cameraZKeyframe.AddTimeValue(0.1, 0.809017);
  cameraZKeyframe.AddTimeValue(0.2, 0.309017);
  cameraZKeyframe.AddTimeValue(0.3, -0.309017);
  cameraZKeyframe.AddTimeValue(0.4, -0.809017);
  cameraZKeyframe.AddTimeValue(0.5, -1.000000);
  cameraZKeyframe.AddTimeValue(0.6, -0.809017);
  cameraZKeyframe.AddTimeValue(0.7, -0.309017);
  cameraZKeyframe.AddTimeValue(0.8, 0.309017);
  cameraZKeyframe.AddTimeValue(0.9, 0.809017);
  cameraZKeyframe.AddTimeValue(1., 1.0);

  cameraLookAtXKeyframe.Init();

  cameraLookAtXKeyframe.AddTimeValue(0., 0.000000);
  cameraLookAtXKeyframe.AddTimeValue(0.1, 0.587785);
  cameraLookAtXKeyframe.AddTimeValue(0.2, 0.951057);
  cameraLookAtXKeyframe.AddTimeValue(0.3, 0.951057);
  cameraLookAtXKeyframe.AddTimeValue(0.4, 0.587785);
  cameraLookAtXKeyframe.AddTimeValue(0.5, -0.000000);
  cameraLookAtXKeyframe.AddTimeValue(0.6, -0.587785);
  cameraLookAtXKeyframe.AddTimeValue(0.7, -0.951057);
  cameraLookAtXKeyframe.AddTimeValue(0.8, -0.951056);
  cameraLookAtXKeyframe.AddTimeValue(0.9, -0.587785);
  cameraLookAtXKeyframe.AddTimeValue(1., -0.000000);

  lightXKeyframe.Init();
  lightXKeyframe.AddTimeValue(0.f * TOTAL, 0.f);    // (0, 0)
  lightXKeyframe.AddTimeValue(.1f * TOTAL, -2.f);   // (-2, 1)
  lightXKeyframe.AddTimeValue(.35f * TOTAL, -2.f);  // (-2, -1)
  lightXKeyframe.AddTimeValue(.6f * TOTAL, 0.f);    // (0, 0)
  lightXKeyframe.AddTimeValue(.7f * TOTAL, 2.f);    // (2, -1)
  lightXKeyframe.AddTimeValue(.85f * TOTAL, 2.f);   // (2, 1)
  lightXKeyframe.AddTimeValue(1.f * TOTAL, 0.f);    // (0, 0)

  lightZKeyframe.Init();
  lightZKeyframe.AddTimeValue(0.f * TOTAL, 0.f);    // (0, 0)
  lightZKeyframe.AddTimeValue(.1f * TOTAL, 1.f);    // (-2, 1)
  lightZKeyframe.AddTimeValue(.35f * TOTAL, -1.f);  // (-2, -1)
  lightZKeyframe.AddTimeValue(.6f * TOTAL, 0.f);    // (0, 0)
  lightZKeyframe.AddTimeValue(.7f * TOTAL, -1.f);   // (2, -1)
  lightZKeyframe.AddTimeValue(.85f * TOTAL, 1.f);   // (2, 1)
  lightZKeyframe.AddTimeValue(1.f * TOTAL, 0.f);    // (0, 0)

  lightRedKeyframe.Init();
  lightRedKeyframe.AddTimeValue(0.f * TOTAL, 1.f);
  lightRedKeyframe.AddTimeValue(.1f * TOTAL, .8f);
  lightRedKeyframe.AddTimeValue(.35f * TOTAL, .8f);
  lightRedKeyframe.AddTimeValue(.6f * TOTAL, .8f);
  lightRedKeyframe.AddTimeValue(.7f * TOTAL, .5f);
  lightRedKeyframe.AddTimeValue(.85f * TOTAL, .4f);
  lightRedKeyframe.AddTimeValue(1.f * TOTAL, 1.f);

  lightGreenKeyframe.Init();
  lightGreenKeyframe.AddTimeValue(0.f * TOTAL, 1.f);
  lightGreenKeyframe.AddTimeValue(.1f * TOTAL, 1.f);
  lightGreenKeyframe.AddTimeValue(.35f * TOTAL, .6f);
  lightGreenKeyframe.AddTimeValue(.6f * TOTAL, .6f);
  lightGreenKeyframe.AddTimeValue(.7f * TOTAL, .8f);
  lightGreenKeyframe.AddTimeValue(.85f * TOTAL, .8f);
  lightGreenKeyframe.AddTimeValue(1.f * TOTAL, 1.f);

  lightBlueKeyframe.Init();
  lightBlueKeyframe.AddTimeValue(0.f * TOTAL, .2f);
  lightBlueKeyframe.AddTimeValue(.1f * TOTAL, .2f);
  lightBlueKeyframe.AddTimeValue(.35f * TOTAL, .4f);
  lightBlueKeyframe.AddTimeValue(.6f * TOTAL, .4f);
  lightBlueKeyframe.AddTimeValue(.7f * TOTAL, .6f);
  lightBlueKeyframe.AddTimeValue(.85f * TOTAL, .6f);
  lightBlueKeyframe.AddTimeValue(1.f * TOTAL, .2f);

  bouncingFrogYKeyframe.Init();
  bouncingFrogYKeyframe.AddTimeValue(0.f, 0.f);
  bouncingFrogYKeyframe.AddTimeValue(.1f, 0.f);
  bouncingFrogYKeyframe.AddTimeValue(.2f, 0.f);
  bouncingFrogYKeyframe.AddTimeValue(.3f, 1.5f);
  bouncingFrogYKeyframe.AddTimeValue(.4f, 2.f);
  bouncingFrogYKeyframe.AddTimeValue(.5f, 1.5f);
  bouncingFrogYKeyframe.AddTimeValue(.6f, 0.f);
  bouncingFrogYKeyframe.AddTimeValue(.7f, 0.f);
  bouncingFrogYKeyframe.AddTimeValue(.8f, 0.f);
  bouncingFrogYKeyframe.AddTimeValue(.9f, 0.f);
  bouncingFrogYKeyframe.AddTimeValue(1.f, 0.f);

  bouncingFrogScaleXZKeyframe.Init();
  // bouncingFrogScaleXZKeyframe.AddTimeValue(0.f, 1.f / 2);
  bouncingFrogScaleXZKeyframe.AddTimeValue(0.f, 1.f / 2);
  bouncingFrogScaleXZKeyframe.AddTimeValue(.1f, 1.f / 2);
  bouncingFrogScaleXZKeyframe.AddTimeValue(.2f, 1.6f / 2);
  // bouncingFrogScaleXZKeyframe.AddTimeValue(.3f, 1.f / 2);
  bouncingFrogScaleXZKeyframe.AddTimeValue(.4f, .8f / 2);
  bouncingFrogScaleXZKeyframe.AddTimeValue(.5f, .6f / 2);
  bouncingFrogScaleXZKeyframe.AddTimeValue(.6f, .8f / 2);
  bouncingFrogScaleXZKeyframe.AddTimeValue(.65f, 1.6f / 2);
  // bouncingFrogScaleXZKeyframe.AddTimeValue(.8f, 1.f / 2);
  bouncingFrogScaleXZKeyframe.AddTimeValue(.9f, 1.f / 2);
  bouncingFrogScaleXZKeyframe.AddTimeValue(1.f, 1.f / 2);

  bouncingFrogScaleYKeyframe.Init();
  // bouncingFrogScaleYKeyframe.AddTimeValue(0.f, 1.f / 2);
  bouncingFrogScaleYKeyframe.AddTimeValue(0.f, 1.f / 2);
  bouncingFrogScaleYKeyframe.AddTimeValue(.1f, 1.f / 2);
  bouncingFrogScaleYKeyframe.AddTimeValue(.2f, .6f / 2);
  // bouncingFrogScaleYKeyframe.AddTimeValue(.3f, 1.f / 2);
  bouncingFrogScaleYKeyframe.AddTimeValue(.4f, 1.6f / 2);
  // bouncingFrogScaleYKeyframe.AddTimeValue(.5f, 1.f / 2);
  bouncingFrogScaleYKeyframe.AddTimeValue(.6f, .6f / 2);
  bouncingFrogScaleYKeyframe.AddTimeValue(.7f, 1.f / 2);
  bouncingFrogScaleYKeyframe.AddTimeValue(.8f, 1.f / 2);
  bouncingFrogScaleYKeyframe.AddTimeValue(.9f, 1.f / 2);
  bouncingFrogScaleYKeyframe.AddTimeValue(1.f, 1.f / 2);
}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )
// # MARK: InitLists
void InitLists() {
  if (DebugOn != 0)
    fprintf(stderr, "Starting InitLists.\n");

  float dx = BOXSIZE / 2.f;
  float dy = BOXSIZE / 2.f;
  float dz = BOXSIZE / 2.f;
  glutSetWindow(MainWindow);

  // create the object:

  // create the axes:

  AxesList = glGenLists(1);
  glNewList(AxesList, GL_COMPILE);
  glLineWidth(AXES_WIDTH);
  Axes(1.5);
  glLineWidth(1.);
  glEndList();

  // MARK: Create Ground Plane
  GroundPlaneList = glGenLists(1);
  glNewList(GroundPlaneList, GL_COMPILE);

  OsuNoiseGrid(200, 300, 20., .5, 10.);

  glEndList();

  // MARK: Create Watering Can

  WateringCanList = glGenLists(1);
  glNewList(WateringCanList, GL_COMPILE);
  // OsuCone(.5, .5, 1, 10, 4);
  LoadObjFile((char *)"watering_can.obj");
  glEndList();

  // MARK: Create Water Droplet

  WaterDropletList = glGenLists(1);
  glNewList(WaterDropletList, GL_COMPILE);
  glBegin(GL_TRIANGLES);
  glColor3f(1., 1., 1.);
  glNormal3f(1., 0., 0.);

  glVertex3f(0., 0., 0.);
  glVertex3f(0., -1., .5);
  glVertex3f(0., -1., -.5);

  glVertex3f(0., 0., 0.);
  glVertex3f(0., 1., -.5);
  glVertex3f(0., 1., .5);

  glEnd();

  glEndList();

  // MARK: Create Seed Pouch

  SeedPouchList = glGenLists(1);
  glNewList(SeedPouchList, GL_COMPILE);
  LoadObjFile((char *)"seed_pouch.obj");
  glEndList();

  // MARK: Create Dirt Mound

  DirtMoundList = glGenLists(1);
  glNewList(DirtMoundList, GL_COMPILE);
  glScalef(0.75, 0.75, 0.75);
  LoadObjFile((char *)"dirt_mound.obj");

  glEndList();

  // MARK: Create Garden Sign
  bouncingFrogList = glGenLists(1);
  glNewList(bouncingFrogList, GL_COMPILE);
  LoadObjFile((char *)"dumbfrog.obj");
  glEndList();

  // MARK: Create Pot

  PotList = glGenLists(1);
  glNewList(PotList, GL_COMPILE);

  glBegin(GL_TRIANGLES);

  glColor3f(1., 0., 0.);

  // glNormal3f(sin(i), cos(i), 0.);
  //   glVertex3f(i / 2, i / 6.0, 0);
  int cylinderSegments = 25;

  for (int i = 0; i <= cylinderSegments; i++) {
    float x = cos(i * 2 * M_PI / cylinderSegments);
    float y = sin(i * 2 * M_PI / cylinderSegments);

    float x2 = cos((i + 1) * 2 * M_PI / cylinderSegments);
    float y2 = sin((i + 1) * 2 * M_PI / cylinderSegments);

    float baseRadius = .6;
    float topRadius = .8;
    float baseHeight = -.7;
    float rimHeight = .2;
    float rimRadius = .9;
    float rimInnerRadius = .6;
    float innerBaseHeight = -.6;

    glColor3f(x + 0.5, 0, y + 0.5);
    glNormal3f(x, 0, y);

    // Wall BottomTri
    glVertex3f(x * baseRadius, baseHeight, y * baseRadius);
    glVertex3f(x * topRadius, 0, y * topRadius);
    glVertex3f(x2 * baseRadius, baseHeight, y2 * baseRadius);
    // Wall TopTri
    glVertex3f(x2 * baseRadius, baseHeight, y2 * baseRadius);
    glVertex3f(x2 * topRadius, 0, y2 * topRadius);
    glVertex3f(x * topRadius, 0, y * topRadius);

    // Inner Wall Bottom Tri
    glVertex3f(x * rimInnerRadius, innerBaseHeight, y * rimInnerRadius);
    glVertex3f(x2 * rimInnerRadius, innerBaseHeight, y2 * rimInnerRadius);
    glVertex3f(x * rimInnerRadius, rimHeight, y * rimInnerRadius);
    // Inner Wall Top Tri
    glVertex3f(x2 * rimInnerRadius, innerBaseHeight, y2 * rimInnerRadius);
    glVertex3f(x * rimInnerRadius, rimHeight, y * rimInnerRadius);
    glVertex3f(x2 * rimInnerRadius, rimHeight, y2 * rimInnerRadius);

    // TopCyl Bottom Tri
    glVertex3f(x * rimRadius, 0, y * rimRadius);
    glVertex3f(x2 * rimRadius, 0, y2 * rimRadius);
    glVertex3f(x2 * rimRadius, rimHeight, y2 * rimRadius);
    // TopCyl Top Tri
    glVertex3f(x * rimRadius, 0, y * rimRadius);
    glVertex3f(x * rimRadius, rimHeight, y * rimRadius);
    glVertex3f(x2 * rimRadius, rimHeight, y2 * rimRadius);

    // Darker Color
    glColor3f((x + 0.5) * 0.6, 0, (y + 0.5) * 0.6);

    // Base Circle
    glVertex3f(0, baseHeight, 0);
    glVertex3f(x * baseRadius, baseHeight, y * baseRadius);
    glVertex3f(x2 * baseRadius, baseHeight, y2 * baseRadius);

    // Rim Base Inner Tri
    glVertex3f(x2 * topRadius, 0, y2 * topRadius);
    glVertex3f(x * topRadius, 0, y * topRadius);
    glVertex3f(x * rimRadius, 0, y * rimRadius);
    // Rim Base Outer Tri
    glVertex3f(x * rimRadius, 0, y * rimRadius);
    glVertex3f(x2 * rimRadius, 0, y2 * rimRadius);
    glVertex3f(x2 * topRadius, 0, y2 * topRadius);

    // Inner Base Circle
    glVertex3f(0, innerBaseHeight, 0);
    glVertex3f(x * rimInnerRadius, innerBaseHeight, y * rimInnerRadius);
    glVertex3f(x2 * rimInnerRadius, innerBaseHeight, y2 * rimInnerRadius);

    // Lighter Color
    glColor3f((x + 0.5) * 1.2, 0, (y + 0.5) * 1.2);

    // Rim Top Outer Tri
    glVertex3f(x * rimRadius, rimHeight, y * rimRadius);
    glVertex3f(x2 * rimRadius, rimHeight, y2 * rimRadius);
    glVertex3f(x2 * rimInnerRadius, rimHeight, y2 * rimInnerRadius);
    // Rim Top Inner Tri
    glVertex3f(x * rimInnerRadius, rimHeight, y * rimInnerRadius);
    glVertex3f(x2 * rimInnerRadius, rimHeight, y2 * rimInnerRadius);
    glVertex3f(x * rimRadius, rimHeight, y * rimRadius);

    // Dirt
    glColor3f(0.65 + (x / 10.0), 0.4 + (y / 10.0), 0.05);
    // Dirt base
    glVertex3f(0, 0, 0);
    glVertex3f(x * rimInnerRadius, 0, y * rimInnerRadius);
    glVertex3f(x2 * rimInnerRadius, 0, y2 * rimInnerRadius);

    glColor3f(0.7 + (x / 10.0), 0.4 + (y / 10.0), 0.1);
    // Dirt Bump
    glVertex3f(0, 0, 0);
    glVertex3f(x * rimInnerRadius, (sin(i) + 1) / 10.0, y * rimInnerRadius);
    glVertex3f(x2 * rimInnerRadius, -0.01, y2 * rimInnerRadius);
  }

  glEnd();
  glEndList();

  // MARK: Create Plants
  create_plants();

  // create the axes:

  AxesList = glGenLists(1);
  glNewList(AxesList, GL_COMPILE);
  glLineWidth(AXES_WIDTH);
  Axes(1.5);
  glLineWidth(1.);
  glEndList();
}

// the keyboard callback:
// MARK: Keyboard
void Keyboard(unsigned char c, int x, int y) {
  if (DebugOn != 0)
    fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

  switch (c) {
      // case 'o':
      // case 'O':
      //   NowProjection = ORTHO;
      //   break;

    case 'p':
    case 'P':
      // NowProjection = PERSP;
      LightMode = 0;
      break;

    case '=':
    case '+':
      Scale *= 1.1;
      break;

    case '-':
    case '_':
      Scale /= 1.1;
      break;

    case 's':
    case 'S':
      LightMode = 1;
      break;

      // w', 'r', 'g', 'b', 'c'

    case 'f':
    case 'F':
      if (FreezeAnimation == 0) {
        FreezeAnimation = 1;
      } else {
        FreezeAnimation = 0;
      }
      break;

    case 'w':
    case 'W':
      NowColor = WHITE;
      break;
    case 'r':
    case 'R':
      NowColor = RED;
      break;
    case 'g':
    case 'G':
      NowColor = GREEN;
      break;
    case 'b':
    case 'B':
      NowColor = BLUE;
      break;
    case 'c':
    case 'C':
      NowColor = CYAN;
      break;
    case 'm':
    case 'M':
      NowColor = MAGENTA;
      break;

    case 'q':
    case 'Q':
    case ESCAPE:
      DoMainMenu(QUIT);  // will not return here
      break;             // happy compiler

    default:
      fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
  }

  // force a call to Display( ):

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

// called when the mouse button transitions down or up:

void MouseButton(int button, int state, int x, int y) {
  int b = 0;  // LEFT, MIDDLE, or RIGHT

  if (DebugOn != 0)
    fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);

  // get the proper button bit mask:

  switch (button) {
    case GLUT_LEFT_BUTTON:
      b = LEFT;
      break;

    case GLUT_MIDDLE_BUTTON:
      b = MIDDLE;
      break;

    case GLUT_RIGHT_BUTTON:
      b = RIGHT;
      break;

    case SCROLL_WHEEL_UP:
      Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
      // keep object from turning inside-out or disappearing:
      if (Scale < MINSCALE)
        Scale = MINSCALE;
      break;

    case SCROLL_WHEEL_DOWN:
      Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
      // keep object from turning inside-out or disappearing:
      if (Scale < MINSCALE)
        Scale = MINSCALE;
      break;

    default:
      b = 0;
      fprintf(stderr, "Unknown mouse button: %d\n", button);
  }

  // button down sets the bit, up clears the bit:

  if (state == GLUT_DOWN) {
    Xmouse = x;
    Ymouse = y;
    ActiveButton |= b;  // set the proper bit
  } else {
    ActiveButton &= ~b;  // clear the proper bit
  }

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

// called when the mouse moves while a button is down:

void MouseMotion(int x, int y) {
  int dx = x - Xmouse;  // change in mouse coords
  int dy = y - Ymouse;

  if ((ActiveButton & LEFT) != 0) {
    Xrot += (ANGFACT * dy);
    Yrot += (ANGFACT * dx);
  }

  if ((ActiveButton & MIDDLE) != 0) {
    Scale += SCLFACT * (float)(dx - dy);

    // keep object from turning inside-out or disappearing:

    if (Scale < MINSCALE)
      Scale = MINSCALE;
  }

  Xmouse = x;  // new current position
  Ymouse = y;

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

// MARK: Reset

void Reset() {
  ActiveButton = 0;
  AxesOn = 0;
  DebugOn = 0;
  DepthBufferOn = 1;
  DepthFightingOn = 0;
  DepthCueOn = 0;
  Scale = .3;
  // Scale = 0.2;
  ShadowsOn = 0;
  NowColor = YELLOW;
  NowProjection = PERSP;
  Xrot = 20;
  Yrot = 0;
  // Xrot = 20;
  // Yrot = -45;
}

// called when user resizes the window:

void Resize(int width, int height) {
  // don't really need to do anything since window size is
  // checked each time in Display( ):

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

// handle a change to the window's visibility:

void Visibility(int state) {
  if (DebugOn != 0)
    fprintf(stderr, "Visibility: %d\n", state);

  if (state == GLUT_VISIBLE) {
    glutSetWindow(MainWindow);
    glutPostRedisplay();
  } else {
    // could optimize by keeping track of the fact
    // that the window is not visible and avoid
    // animating or redrawing it ...
  }
}

///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////

// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = {0.f, 1.f, 0.f, 1.f};

static float xy[] = {-.5f, .5f, .5f, -.5f};

static int xorder[] = {1, 2, -3, 4};

static float yx[] = {0.f, 0.f, -.5f, .5f};

static float yy[] = {0.f, .6f, 1.f, 1.f};

static int yorder[] = {1, 2, 3, -2, 4};

static float zx[] = {1.f, 0.f, 1.f, 0.f, .25f, .75f};

static float zy[] = {.5f, .5f, -.5f, -.5f, 0.f, 0.f};

static int zorder[] = {1, 2, 3, 4, -5, 6};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void Axes(float length) {
  glBegin(GL_LINE_STRIP);
  glVertex3f(length, 0., 0.);
  glVertex3f(0., 0., 0.);
  glVertex3f(0., length, 0.);
  glEnd();
  glBegin(GL_LINE_STRIP);
  glVertex3f(0., 0., 0.);
  glVertex3f(0., 0., length);
  glEnd();

  float fact = LENFRAC * length;
  float base = BASEFRAC * length;

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < 4; i++) {
    int j = xorder[i];
    if (j < 0) {
      glEnd();
      glBegin(GL_LINE_STRIP);
      j = -j;
    }
    j--;
    glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
  }
  glEnd();

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < 5; i++) {
    int j = yorder[i];
    if (j < 0) {
      glEnd();
      glBegin(GL_LINE_STRIP);
      j = -j;
    }
    j--;
    glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
  }
  glEnd();

  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < 6; i++) {
    int j = zorder[i];
    if (j < 0) {
      glEnd();
      glBegin(GL_LINE_STRIP);
      j = -j;
    }
    j--;
    glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
  }
  glEnd();
}

// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void HsvRgb(float hsv[3], float rgb[3]) {
  // guarantee valid input:

  float h = hsv[0] / 60.f;
  while (h >= 6.) h -= 6.;
  while (h < 0.) h += 6.;

  float s = hsv[1];
  if (s < 0.)
    s = 0.;
  if (s > 1.)
    s = 1.;

  float v = hsv[2];
  if (v < 0.)
    v = 0.;
  if (v > 1.)
    v = 1.;

  // if sat==0, then is a gray:

  if (s == 0.0) {
    rgb[0] = rgb[1] = rgb[2] = v;
    return;
  }

  // get an rgb from the hue itself:

  float i = (float)floor(h);
  float f = h - i;
  float p = v * (1.f - s);
  float q = v * (1.f - s * f);
  float t = v * (1.f - (s * (1.f - f)));

  float r = 0., g = 0., b = 0.;  // red, green, blue
  switch ((int)i) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;

    case 1:
      r = q;
      g = v;
      b = p;
      break;

    case 2:
      r = p;
      g = v;
      b = t;
      break;

    case 3:
      r = p;
      g = q;
      b = v;
      break;

    case 4:
      r = t;
      g = p;
      b = v;
      break;

    case 5:
      r = v;
      g = p;
      b = q;
      break;
  }

  rgb[0] = r;
  rgb[1] = g;
  rgb[2] = b;
}

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
