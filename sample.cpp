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

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif
#include "glut.h"

#define GLM_FORCE_RADIANS
#include "editor/console.h"
#include "editor/debug.h"
#include "editor/lightswindow.h"
#include "editor/style.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "objects/objects.h"
#include "utils/arraymath.h"
#include "utils/bmptotexture.h"
#include "utils/colors.h"
#include "utils/glslprogram.h"
#include "utils/keytime.h"
// #include "setmaterial.cpp"
// #include "setlight.cpp"
// #include "osucone.cpp"
// #include "osutorus.cpp"
// #include "bmptotexture.cpp"
// #include "loadobjfile.cpp"

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
//	Author:			Aidan Schmitigal

// title of these windows:

const char *WINDOWTITLE = "OpenGL / GLUT Sample -- Aidan Schmitigal";
const char *GLUITITLE = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_WIDTH = 1200;
const int INIT_WINDOW_HEIGHT = 1000;

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

// line width for the axes:

const GLfloat AXES_WIDTH = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

// non-constant global variables:

int ActiveButton = 0;       // current button that is down
GLuint AxesList;            // list to hold the axes
int MainWindow;             // window id for main graphics window
float Scale = 0.3;          // scaling factor
float SceneTime = 0.0;      // used for animation, # of seconds elapsed
float DeltaTime = 0.0;      // used for animation, # of seconds elapsed
int Xmouse, Ymouse;         // mouse values
float Xrot = 20, Yrot = 0;  // rotation angles in degrees

int SphereList;
bool ImGuiSceneRenderHovered = true;
float scene_window_width = INIT_WINDOW_WIDTH;
float scene_window_height = INIT_WINDOW_HEIGHT;

// function prototypes:
void Animate();
void DisplayImGui();
void Display();
void DoRasterString(float, float, float, char *);
void DoStrokeString(float, float, float, float, char *);
float ElapsedSeconds();
void InitGraphics();
void InitImGui();
void InitAnimations();
void InitLists();
void Keyboard(unsigned char, int, int);
void KeyboardUp(unsigned char, int, int);
void KeyboardSpecial(int, int, int);
void KeyboardSpecialUp(int, int, int);
void MouseButton(int, int, int, int);
void MouseMotion(int, int);
void MouseMotionPassive(int, int);
void Reset();
void ExitProgram();
void Resize(int, int);
void Visibility(int);

void Axes(float);
void HsvRgb(float[3], float[3]);

// these are here for when you need them -- just uncomment the ones you need:

float NowS0, NowT0, NowD;
GLSLProgram Pattern;

// main program:

int main(int argc, char *argv[]) {
  // turn on the glut package:
  // (do this before checking argc and argv since glutInit might
  // pull some command line arguments out)

  glutInit(&argc, argv);

  // setup all the graphics stuff:
  // this needs to be called once and only once!

  InitGraphics();
  noise_setseed(0);

  InitImGui();

  // create the display lists that **will not change**:

  InitLists();

  // draw the scene once and wait for some interaction:
  // (this will never return)

  glutSetWindow(MainWindow);
  glutMainLoop();
  return 0;
}

void ExitProgram() {
  printf("Exiting\n");

  glutSetWindow(MainWindow);
  glFinish();
  glutDestroyWindow(MainWindow);

  // Cleanup
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplGLUT_Shutdown();
  ImGui::DestroyContext();

  printf("Exitted\n");

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
  // put animation stuff in here -- change some global variables for Display( ) to find:

  ms = glutGet(GLUT_ELAPSED_TIME);
  DeltaTime = ms / 1000.f - SceneTime;
  if (Debug::freezeTime == false) {
    SceneTime += DeltaTime;
  }
  // for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

  // force a call to Display( ) next time it is convenient:

  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

// MARK: DisplayImGui

void DisplayImGui() {
  // printf("DisplayImGui\n");
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL2_NewFrame();
  ImGui_ImplGLUT_NewFrame();
  ImGui::NewFrame();
  ImGuiIO &io = ImGui::GetIO();

  // static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoDockingOverCentralNode;
  // ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
  // const ImGuiViewport *viewport = ImGui::GetMainViewport();
  // ImGui::SetNextWindowPos(viewport->WorkPos);
  // ImGui::SetNextWindowSize(viewport->WorkSize);
  // ImGui::SetNextWindowViewport(viewport->ID);
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  // window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  // window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  // window_flags |= ImGuiWindowFlags_NoBackground;

  // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  // ImGui::Begin("DockSpaceWindow", NULL, window_flags);
  // ImGui::PopStyleVar();
  // ImGui::PopStyleVar(2);

  // ImGuiID dockspace_id = ImGui::GetID("FullWindowDockSpace");
  // ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

  // if (ImGui::BeginMainMenuBar()) {
  //   ImGui::TextDisabled("(?)");
  //   if (ImGui::BeginItemTooltip()) {
  //     ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
  //     ImGui::TextUnformatted(
  //         "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!"
  //         "\n"
  //         "- Drag from window title bar or their tab to dock/undock."
  //         "\n"
  //         "- Drag from window menu button (upper-left button) to undock an entire node (all windows)."
  //         "\n"
  //         "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)"
  //         "\n"
  //         "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)"
  //         "\n\n"
  //         "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window."
  //         "\n\n"
  //         "Read comments in ShowExampleAppDockSpace() for more details.");
  //     ImGui::PopTextWrapPos();
  //     ImGui::EndTooltip();
  //   }

  //   ImGui::SameLine(ImGui::GetWindowWidth() - 60);
  //   if (ImGui::MenuItem("Quit", "Alt+F4")) {
  //     printf("Quit\n");
  //     ExitProgram();
  //   }

  //   ImGui::EndMainMenuBar();
  // }

  // ImGui::End();

  if (Debug::showDemoWindow)
    ImGui::ShowDemoWindow(&Debug::showDemoWindow);

  LightsEditor::ShowLightsPanel();
  ConsoleWindow::ShowConsolePanel();
  Debug::ShowDebugPanel();
  if (ImGui::Begin("Debug")) {
    ImGui::DragFloat("Scale", &Scale, 0.01f, 0.f, 10.f);
    ImGui::End();
  }

  // static ImGuiWindowFlags scene_window_flags = ImGuiWindowFlags_NoTitleBar;
  // ImGuiWindowClass scene_window_class;
  // scene_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoUndocking;
  // ImGui::SetNextWindowClass(&scene_window_class);

  // ImGui::Begin("Scene", NULL, scene_window_flags);

  // ImGui::BeginChild("Scene", ImVec2(0, 0), 0, ImGuiWindowFlags_NoScrollbar);

  // we access the ImGui window size
  // const float window_width = ImGui::GetContentRegionAvail().x;
  // const float window_height = ImGui::GetContentRegionAvail().y;
  // scene_window_width = window_width;
  // scene_window_height = window_height;

  // we rescale the framebuffer to the actual window size here and reset the glViewport
  // rescale_framebuffer(window_width, window_height);
  // glViewport(0, 0, window_width, window_height);

  // ImVec2 pos = ImGui::GetCursorScreenPos();

  // and here we can add our created texture as image to ImGui
  // unfortunately we need to use the cast to void* or I didn't find another way tbh
  // ImGui::GetWindowDrawList()->AddImage(
  //     (ImTextureID)framebuffer_texture_id,
  //     ImVec2(pos.x, pos.y),
  //     ImVec2(pos.x + window_width, pos.y + window_height),
  //     ImVec2(0, 1),
  //     ImVec2(1, 0));

  // ImGui::Text("%.1f FPS (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);

  // ImGuiSceneRenderHovered = ImGui::IsWindowHovered();

  // ImGui::EndChild();

  // ImGui::End();

  // Rendering
  ImGui::Render();
}
// draw the complete scene:
// MARK: Display
void Display() {
  DisplayImGui();
  // printf("Display\n");
  // set which window we want to do the graphics into:
  glutSetWindow(MainWindow);

  // erase the background:
  glDrawBuffer(GL_BACK);
  glClearColor(LightsEditor::clearColor[0] * LightsEditor::clearColor[3], LightsEditor::clearColor[1] * LightsEditor::clearColor[3], LightsEditor::clearColor[2] * LightsEditor::clearColor[3], LightsEditor::clearColor[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  // specify shading to be flat:

  glShadeModel(GL_FLAT);

  // set the viewport to be a square centered in the window:

  GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
  GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
  glViewport(0, 0, vx, vy);

  // set the viewing volume:
  // remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
  // USE gluOrtho2D( ) IF YOU ARE DOING 2D !

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // if (NowProjection == ORTHO)
  //   glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
  // else
  gluPerspective(70.f, (GLdouble)vx / (GLdouble)vy, 0.1f, 1000.f);

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

  // uniformly scale the scene:

  if (Scale < MINSCALE)
    Scale = MINSCALE;
  glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

  // possibly draw the axes:

  if (Debug::axes != 0) {
    // glColor3fv(&Colors[NowColor][0]);
    glCallList(AxesList);
  }

  // since we are using glScalef( ), be sure the normals get unitized:

  glEnable(GL_NORMALIZE);

  // draw the box object by calling up its display list:

  Pattern.Use();

  // set the uniform variables that will change over time:

  NowS0 = 0.5f;
  NowT0 = 0.5f;
  NowD = 0.2f + 0.1f * sinf(2.f * F_PI * SceneTime);
  Pattern.SetUniformVariable((char *)"uS0", NowS0);
  Pattern.SetUniformVariable((char *)"uT0", NowT0);
  Pattern.SetUniformVariable((char *)"uD", NowD);

  glCallList(SphereList);

  Pattern.UnUse();  // Pattern.Use(0);  also works

  // Render the ImGUIWindow
  ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

  // swap the double-buffered framebuffers:
  glutSwapBuffers();

  // be sure the graphics buffer has been sent:
  glFlush();
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

  Pattern.Init();
  bool valid = Pattern.Create((char *)"pattern.vert", (char *)"pattern.frag");
  if (!valid)
    fprintf(stderr, "Could not create the Pattern shader!\n");
  else
    fprintf(stderr, "Pattern shader created!\n");

  // set the uniform variables that will not change:

  Pattern.Use();
  Pattern.SetUniformVariable((char *)"uKa", 0.1f);
  Pattern.SetUniformVariable((char *)"uKd", 0.5f);
  Pattern.SetUniformVariable((char *)"uKs", 0.4f);
  Pattern.SetUniformVariable((char *)"uColor", 1.f, 0.5f, 0.f, 1.f);
  Pattern.SetUniformVariable((char *)"uSpecularColor", 1.f, 1.f, 1.f, 1.f);
  Pattern.SetUniformVariable((char *)"uShininess", 12.f);
  Pattern.UnUse();
}

// MARK: InitImGui
void InitImGui() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport / Platform Windows

  // Setup Dear ImGui style
  StyleColorsMaterial();

  // Setup Platform/Renderer backends
  ImGui_ImplGLUT_Init();
  ImGui_ImplOpenGL2_Init();

  // Install GLUT handlers (glutReshapeFunc(), glutMotionFunc(), glutPassiveMotionFunc(), glutMouseFunc(), glutKeyboardFunc() etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
  // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
  ImGui_ImplGLUT_InstallFuncs();

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

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

// MARK: InitLists
void InitLists() {
  fprintf(stderr, "Starting InitLists.\n");

  glutSetWindow(MainWindow);

  // create the object:

  SphereList = glGenLists(1);
  glNewList(SphereList, GL_COMPILE);
  OsuSphere(1., 64, 64);
  glEndList();

  // create the axes:

  AxesList = glGenLists(1);
  glNewList(AxesList, GL_COMPILE);
  glLineWidth(AXES_WIDTH);
  Axes(1.5);
  glLineWidth(1.);
  glEndList();
}

// the keyboard callback:

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

    case 'f':
    case 'F':
      Debug::freezeTime = !Debug::freezeTime;
      if (Debug::freezeTime)
        glutIdleFunc(NULL);
      else
        glutIdleFunc(Animate);
      break;

    case 'q':
    case 'Q':
    case ESCAPE:
      ExitProgram();
      break;  // happy compiler

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

// called when the mouse button transitions down or up:
// MARK: Mouse Button
void MouseButton(int button, int state, int x, int y) {
  printf("MouseButton: %d, %d, %d, %d\n", button, state, x, y);
  if (!ImGuiSceneRenderHovered) {
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
      return;
  }

  printf("Allowed mouse to happen\n");

  int b = 0;  // LEFT, MIDDLE, or RIGHT

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
  glColor3f(1., 0., 0.);
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
  glColor3f(0., 1., 0.);
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
  glColor3f(0., 0., 1.);
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