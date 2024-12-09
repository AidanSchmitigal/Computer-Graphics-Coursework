#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <vector>

#ifndef F_PI
#define F_PI ((float)(M_PI))
#define F_2_PI ((float)(2.f * F_PI))
#define F_PI_2 ((float)(F_PI / 2.f))
#endif

#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include "imgui_internal.h"
#include "imnodes/imnodes.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

#include "glut/glut.h"

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

const char *WINDOWTITLE = "OpenGL Final Project - Aidan Schmitigal";
const char *GLUITITLE = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_WIDTH = 1200;
const int INIT_WINDOW_HEIGHT = 1000;

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

// these are here for when you need them -- just uncomment the ones you need:

// fog parameters:
const GLfloat FOGCOLOR[4] = {.0f, .0f, .0f, 1.f};
const GLenum FOGMODE = GL_LINEAR;
const GLfloat FOGDENSITY = 0.30f;
const GLfloat FOGSTART = 1.5f;
const GLfloat FOGEND = 4.f;

// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
// #define DEMO_Z_FIGHTING
// #define DEMO_DEPTH_BUFFER

// non-constant global variables:
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

// ImGUI Scene Rendering Framebuffer
// GLuint FBO;
// GLuint RBO;
GLuint framebuffer_texture_id;
bool ImGuiSceneRenderHovered = false;
float scene_window_width = INIT_WINDOW_WIDTH;
float scene_window_height = INIT_WINDOW_HEIGHT;

// Scene Time in milliseconds
long scene_ms = 0;
// Change in Scene Time in seconds
float delta_time = 0;

// function prototypes:

void Animate();
void DisplayImGui();
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
void InitImGui();
void InitAnimations();
void InitLists();
void InitMenus();
void Keyboard(unsigned char, int, int);
void KeyboardUp(unsigned char, int, int);
void KeyboardSpecial(int, int, int);
void KeyboardSpecialUp(int, int, int);
void MouseButton(int, int, int, int);
void MouseMotion(int, int);
void MouseMotionPassive(int, int);
void Reset();
void ExitProgram();
void ExitEffects();
void Resize(int, int);
void Visibility(int);

void Axes(float);
// void HsvRgb(float[3], float[3]);
// void Cross(float[3], float[3], float[3]);
// float Dot(float[3], float[3]);
// float Unit(float[3], float[3]);
// float Unit(float[3]);

// MARK: Includes
// #include "utils/bmptotexture.cpp"
// #include "glslprogram.cpp"
#include "editor/lightswindow.h"
#include "editor/nodeeditor.h"
#include "objects/heli.550"
#include "objects/objects.h"
// #include "objects/osucone.cpp"
// #include "objects/osugrid.cpp"
// #include "objects/osusphere.cpp"
// #include "objects/osutorus.cpp"
#include "plants/plants.h"
#include "utils/colors.h"
#include "utils/keytime.h"
// #include "utils/loadobjfile.cpp"
#include "utils/particles.h"
// #include "utils/pillpath.cpp"
// #include "utils/setlight.cpp"
#include "editor/debug.h"
#include "editor/nodes/graph.h"
#include "editor/nodes/nodes.h"
#include "utils/setmaterial.h"
#include "utils/simplex.h"
#include "utils/utils.h"
#include "utils/vectors.h"

// MARK: Main
int main(int argc, char *argv[]) {
  // turn on the glut package:
  // (do this before checking argc and argv since glutInit might
  // pull some command line arguments out)

  glutInit(&argc, argv);

  // setup all the graphics stuff:

  InitGraphics();
  noise_setseed(0);

  InitImGui();

  NodeEditor::load();

  // create the display lists that **will not change**:

  InitLists();

  InitAnimations();

  // init all the global variables used by Display( ):
  // this will also post a redisplay

  Reset();

  // setup all the user interface stuff:

  InitMenus();

  atexit(ExitEffects);

  glutSetWindow(MainWindow);
  glutMainLoop();
  return 0;
}

void ExitEffects() {
  printf("Exiting\n");

  NodeEditor::save();

  glutSetWindow(MainWindow);
  glFinish();
  glutDestroyWindow(MainWindow);

  // Cleanup
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplGLUT_Shutdown();
  ImGui::DestroyContext();
  ImNodes::DestroyContext();

  printf("Exitted\n");
}

void ExitProgram() {
  ExitEffects();
  exit(0);
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
  // printf("Animate\n");
  // put animation stuff in here -- change some global variables for Display( ) to find:

  ms = glutGet(GLUT_ELAPSED_TIME);
  if (FreezeAnimation == 0) {
    delta_time = (ms - scene_ms) / 1000.f;
    scene_ms = ms;

    if (Debug::autoRotate) {
      Yrot += 2 * delta_time;
    }
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

void create_framebuffer() {
  glGenTextures(1, &framebuffer_texture_id);
  glBindTexture(GL_TEXTURE_2D, framebuffer_texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scene_window_width, scene_window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void rescale_framebuffer(float width, float height) {
  glBindTexture(GL_TEXTURE_2D, framebuffer_texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
}

// MARK: DisplayImGui

void DisplayImGui() {
  // printf("DisplayImGui\n");
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL2_NewFrame();
  ImGui_ImplGLUT_NewFrame();
  ImGui::NewFrame();
  ImGuiIO &io = ImGui::GetIO();

  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoDockingOverCentralNode;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  // window_flags |= ImGuiWindowFlags_NoBackground;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpaceWindow", NULL, window_flags);
  ImGui::PopStyleVar();
  ImGui::PopStyleVar(2);

  ImGuiID dockspace_id = ImGui::GetID("FullWindowDockSpace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

  if (ImGui::BeginMainMenuBar()) {
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip()) {
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(
          "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!"
          "\n"
          "- Drag from window title bar or their tab to dock/undock."
          "\n"
          "- Drag from window menu button (upper-left button) to undock an entire node (all windows)."
          "\n"
          "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)"
          "\n"
          "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)"
          "\n\n"
          "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window."
          "\n\n"
          "Read comments in ShowExampleAppDockSpace() for more details.");
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }

    ImGui::SameLine(ImGui::GetWindowWidth() - 60);
    if (ImGui::MenuItem("Quit", "Alt+F4")) {
      printf("Quit\n");
      ExitProgram();
    }

    ImGui::EndMainMenuBar();
  }

  ImGui::End();

  // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
  if (Debug::showDemoWindow)
    ImGui::ShowDemoWindow(&Debug::showDemoWindow);

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.

  LightsEditor::ShowLightsPanel();
  Debug::ShowDebugPanel();
  if (ImGui::Begin("Debug")) {
    ImGui::DragFloat("Scale", &Scale, 0.01f, 0.f, 10.f);
    ImGui::End();
  }
  NodeEditor::ShowNodesPanel();
  NodeEditor::ShowGroupsPanel();

  static ImGuiWindowFlags scene_window_flags = ImGuiWindowFlags_NoTitleBar;
  ImGuiWindowClass scene_window_class;
  scene_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoUndocking;
  ImGui::SetNextWindowClass(&scene_window_class);

  ImGui::Begin("Scene", NULL, scene_window_flags);

  ImGui::BeginChild("Scene", ImVec2(0, 0), 0, ImGuiWindowFlags_NoScrollbar);

  // we access the ImGui window size
  const float window_width = ImGui::GetContentRegionAvail().x;
  const float window_height = ImGui::GetContentRegionAvail().y;
  scene_window_width = window_width;
  scene_window_height = window_height;

  // we rescale the framebuffer to the actual window size here and reset the glViewport
  rescale_framebuffer(window_width, window_height);
  glViewport(0, 0, window_width, window_height);

  ImVec2 pos = ImGui::GetCursorScreenPos();

  // and here we can add our created texture as image to ImGui
  // unfortunately we need to use the cast to void* or I didn't find another way tbh
  ImGui::GetWindowDrawList()->AddImage(
      (ImTextureID)framebuffer_texture_id,
      ImVec2(pos.x, pos.y),
      ImVec2(pos.x + window_width, pos.y + window_height),
      ImVec2(0, 1),
      ImVec2(1, 0));

  ImGui::Text("%.1f FPS (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);

  ImGuiSceneRenderHovered = ImGui::IsWindowHovered();

  ImGui::EndChild();

  ImGui::End();

  // Rendering
  ImGui::Render();
}

// draw the complete scene:
// MARK: Display
void Display() {
  DisplayImGui();
  // printf("Display\n");

  if (DebugOn != 0)
    fprintf(stderr, "Starting Display.\n");

  // set which window we want to do the graphics into:
  glutSetWindow(MainWindow);

  // Bind the framebuffer
  // glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  // erase the background:
  glDrawBuffer(GL_BACK);
  glClearColor(LightsEditor::clearColor[0] * LightsEditor::clearColor[3], LightsEditor::clearColor[1] * LightsEditor::clearColor[3], LightsEditor::clearColor[2] * LightsEditor::clearColor[3], LightsEditor::clearColor[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  // specify shading to be flat:

  if (Debug::flatShading) {
    glShadeModel(GL_FLAT);
  } else {
    glShadeModel(GL_SMOOTH);
  }

  if (Debug::showWireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  // set the viewport to be a square centered in the window:
  // TODO: Change to be the size of a ImGUI window

  // GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
  // GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
  // GLsizei v = vx < vy ? vx : vy;  // minimum dimension
  // GLint xl = (vx - v) / 2;
  // GLint yb = (vy - v) / 2;
  // glViewport(xl, yb, v, v);
  // glViewport(0, 0, 600, 600);

  // set the viewing volume:
  // remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
  // USE gluOrtho2D( ) IF YOU ARE DOING 2D !

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (NowProjection == ORTHO)
    glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
  else
    gluPerspective(70.f, scene_window_width / scene_window_height, 0.1f, 1000.f);

  // place the objects into the scene:

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // set the eye position, look-at position, and up-vector:

  gluLookAt(0.f, 0.f, 3.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

  // rotate the scene:

  glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
  float upDir[] = {0, 1, 0};
  float lookDir[] = {cos((Yrot + 90.f) * F_PI / 180.f), 0, sin((Yrot + 90.f) * F_PI / 180.f)};

  float xRotDir[3];                                            // Output direction vector
  xRotDir[0] = upDir[1] * lookDir[2] - upDir[2] * lookDir[1];  // x-component
  xRotDir[1] = upDir[2] * lookDir[0] - upDir[0] * lookDir[2];  // y-component
  xRotDir[2] = upDir[0] * lookDir[1] - upDir[1] * lookDir[0];  // z-component

  glRotatef((GLfloat)Xrot, xRotDir[0], xRotDir[1], xRotDir[2]);
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
    glCallList(AxesList);
  }

  // since we are using glScalef( ), be sure the normals get unitized:

  glEnable(GL_NORMALIZE);

  glShadeModel(GL_FLAT);
  glEnable(GL_TEXTURE_2D);
  if (Debug::lightingEnabled) {
    glEnable(GL_LIGHTING);
  }
  //***************************************************************************

  // // glTranslatef(0.f, -0.5f, -1.f);
  // OsuTorus(1.f, 2.f, 100.f, 15.f);
  // glPopMatrix();

  glPushMatrix();

  glBegin(GL_TRIANGLES);

  const std::shared_ptr<NodeEditor::Socket> output = NodeEditor::Graph::graph.outputNode->getOutput();
  auto &vertices = std::get<std::vector<NodeEditor::Vertex>>(output->value);
  for (NodeEditor::Vertex vertex : vertices) {
    glNormal3f(vertex.normal.x, vertex.normal.y, vertex.normal.z);
    glColor3f(vertex.color.r, vertex.color.g, vertex.color.b);
    glVertex3f(vertex.position.x, vertex.position.y, vertex.position.z);
  }

  glEnd();

  glPopMatrix();

  LightsEditor::DisplayLights();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  if (Debug::showNormals) {
    glColor3f(1.f, 1.f, 1.f);
    for (NodeEditor::Vertex vertex : vertices) {
      glBegin(GL_LINES);
      glVertex3f(vertex.position.x, vertex.position.y, vertex.position.z);
      glVertex3f(vertex.position.x + vertex.normal.x, vertex.position.y + vertex.normal.y, vertex.position.z + vertex.normal.z);
      glEnd();
    }
  }

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

  // Unbind the framebuffer
  glBindTexture(GL_TEXTURE_2D, framebuffer_texture_id);
  // glClearColor(0.f, 0.f, 0.f, 1.f);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, scene_window_width, scene_window_height, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  // glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // glDrawBuffer(GL_BACK);
  // glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render the ImGUIWindow
  ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

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
  // NowColor = id - RED;

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

  // int numColors = sizeof(Colors) / (3 * sizeof(float));
  int colormenu = glutCreateMenu(DoColorMenu);
  glutAddMenuEntry("No colors", 0);
  // for (int i = 0; i < numColors; i++) {
  // glutAddMenuEntry(ColorNames[i], i);
  // }

  // int camerapositionmenu = glutCreateMenu(DoInsideMenu);
  // glutAddMenuEntry("Outside", 0);
  // glutAddMenuEntry("Inside", 1);

  int axesmenu = glutCreateMenu(DoAxesMenu);
  glutAddMenuEntry("Off", 0);
  glutAddMenuEntry("On", 1);

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
  glutAddSubMenu("Axes", axesmenu);
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
}

// initialize the glut and OpenGL libraries:
//	also setup callback functions

void loadTexture(char *file, GLuint *texture, int *width, int *height) {
  unsigned char *textureData = BmpToTexture(file, width, height);
  if (textureData == NULL) {
    fprintf(stderr, "Cannot open texture '%s'\n", file);
  } else {
    fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", file, *width, *height);
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, *width, *height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
  }
}

// MARK: InitGraphics
void InitGraphics() {
  if (DebugOn != 0)
    fprintf(stderr, "Starting InitGraphics.\n");

  // request the display modes:
  // ask for red-green-blue-alpha color, double-buffering, and z-buffering:

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  // set the initial window configuration:

  glutInitWindowPosition(0, 0);
  int width = glutGet(GLUT_SCREEN_WIDTH);
  int height = glutGet(GLUT_SCREEN_HEIGHT);
  glutInitWindowSize(width, height);

  // open the window and set its title:

  MainWindow = glutCreateWindow(WINDOWTITLE);
  glutSetWindowTitle(WINDOWTITLE);

  // set the framebuffer clear values:

  glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

  // loadTexture("planets/saturn_ring.bmp", &SaturnRingsTex, &width, &height);

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
  glutReshapeFunc(Resize);             // -> ImGui_ImplGLUT_ReshapeFunc
  glutKeyboardFunc(Keyboard);          // -> ImGui_ImplGLUT_KeyboardFunc
  glutKeyboardUpFunc(KeyboardUp);      // -> ImGui_ImplGLUT_KeyboardUpFunc
  glutSpecialFunc(KeyboardSpecial);    // -> ImGui_ImplGLUT_SpecialFunc
  glutSpecialFunc(KeyboardSpecialUp);  // -> ImGui_ImplGLUT_SpecialUpFunc
  glutMouseFunc(MouseButton);          // -> ImGui_ImplGLUT_MouseFunc
  glutMotionFunc(MouseMotion);         // -> ImGui_ImplGLUT_MotionFunc
  glutPassiveMotionFunc(MouseMotionPassive);
  // glutPassiveMotionFunc( NULL );
  glutVisibilityFunc(Visibility);
  // glutEntryFunc(NULL);
  // glutSpaceballMotionFunc(NULL);
  // glutSpaceballRotateFunc(NULL);
  // glutSpaceballButtonFunc(NULL);
  // glutButtonBoxFunc(NULL);
  // glutDialsFunc(NULL);
  // glutTabletMotionFunc(NULL);
  // glutTabletButtonFunc(NULL);
  // glutMenuStateFunc(NULL);
  // glutTimerFunc(-1, NULL, 0);

  // setup glut to call Animate( ) every time it has
  // 	nothing it needs to respond to (which is most of the time)
  // we don't need to do this for this program, and really should set the argument to NULL
  // but, this sets us up nicely for doing animation

  glutIdleFunc(Animate);

  // Create the framebuffer
  create_framebuffer();

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

// MARK: InitImGui
void InitImGui() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImNodes::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport / Platform Windows

  // Add .ini handle for UserData type
  ImGuiSettingsHandler ini_handler;
  ini_handler.TypeName = "UserData";
  ini_handler.TypeHash = ImHashStr("UserData");
  ini_handler.ReadOpenFn = MyUserData_ReadOpen;
  ini_handler.ReadLineFn = MyUserData_ReadLine;
  ini_handler.WriteAllFn = MyUserData_WriteAll;
  ImGui::AddSettingsHandler(&ini_handler);

  ImNodes::GetIO().LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
  // ImNodes::GetIO().MultipleSelectModifier.Modifier = &ImGui::GetIO().KeyShift;
  ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();
  ImNodes::StyleColorsDark();
  // ImNodes::StyleColorsClassic();

  // Setup Platform/Renderer backends
  // FIXME: Consider reworking this example to install our own GLUT funcs + forward calls ImGui_ImplGLUT_XXX ones, instead of using ImGui_ImplGLUT_InstallFuncs().
  ImGui_ImplGLUT_Init();
  ImGui_ImplOpenGL2_Init();

  // Install GLUT handlers (glutReshapeFunc(), glutMotionFunc(), glutPassiveMotionFunc(), glutMouseFunc(), glutKeyboardFunc() etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
  // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
  // ImGui_ImplGLUT_InstallFuncs();

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  // io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
  // IM_ASSERT(font != nullptr);
}

// MARK: InitAnimations
void InitAnimations() {
}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )
// MARK: InitLists

void InitLists() {
  if (DebugOn != 0)
    fprintf(stderr, "Starting InitLists.\n");

  glutSetWindow(MainWindow);

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
  printf("Keyboard: %d, %d, %d\n", c, x, y);

  if (!ImGuiSceneRenderHovered) {
    ImGui_ImplGLUT_KeyboardFunc(c, x, y);

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
      return;
  }

  printf("Allowed keyboard to happen\n");

  switch (c) {
    case '=':
    case '+':
      Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
      // keep object from turning inside-out or disappearing:
      if (Scale < MINSCALE)
        Scale = MINSCALE;
      break;

    case '-':
    case '_':
      Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
      // keep object from turning inside-out or disappearing:
      if (Scale < MINSCALE)
        Scale = MINSCALE;
      break;

    case 'p':
    case 'P':
      // NowProjection = PERSP;
      LightMode = 0;
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

// MARK: Keyboard Up
void KeyboardUp(unsigned char c, int x, int y) {
  if (!ImGuiSceneRenderHovered) {
    ImGui_ImplGLUT_KeyboardUpFunc(c, x, y);
  }

  // ImGuiIO &io = ImGui::GetIO();
  // if (io.WantCaptureKeyboard)
  //   return;
}

// MARK: Keyboard Special
void KeyboardSpecial(int key, int x, int y) {
  if (!ImGuiSceneRenderHovered) {
    ImGui_ImplGLUT_SpecialFunc(key, x, y);
  }
}

// MARK: Keyboard Special Up
void KeyboardSpecialUp(int key, int x, int y) {
  if (!ImGuiSceneRenderHovered) {
    ImGui_ImplGLUT_SpecialUpFunc(key, x, y);
  }
}

// MARK: Mouse Button
// called when the mouse button transitions down or up:
void MouseButton(int button, int state, int x, int y) {
  // printf("MouseButton: %d, %d, %d, %d\n", button, state, x, y);
  if (!ImGuiSceneRenderHovered) {
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
      return;
  }

  printf("Allowed mouse to happen\n");

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

      printf("Right button pressed\n");

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

void MouseMotionPassive(int x, int y) {
  ImGui_ImplGLUT_MotionFunc(x, y);
}

void MouseMotion(int x, int y) {
  if (!ImGuiSceneRenderHovered) {
    ImGui_ImplGLUT_MotionFunc(x, y);

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
      return;
  }

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
  ImGui_ImplGLUT_ReshapeFunc(width, height);

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
  glColor3f(1., 0., 0.);
  glVertex3f(length, 0., 0.);
  glVertex3f(0., 0., 0.);
  glEnd();
  glBegin(GL_LINE_STRIP);
  glColor3f(0., 1., 0.);
  glVertex3f(0., length, 0.);
  glVertex3f(0., 0., 0.);
  glEnd();
  glBegin(GL_LINE_STRIP);
  glColor3f(0., 0., 1.);
  glVertex3f(0., 0., length);
  glVertex3f(0., 0., 0.);
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