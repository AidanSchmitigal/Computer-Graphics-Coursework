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

#include "editor/PieMenu.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "imguizmo.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include "editor/assets.h"
#include "editor/console.h"
#include "editor/debug.h"
#include "editor/lightswindow.h"
#include "editor/objects.h"
#include "editor/shader.h"
#include "editor/style.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "utils/arraymath.h"
#include "utils/bmptotexture.h"
#include "utils/colors.h"
#include "utils/glslprogram.h"
#include "utils/keytime.h"
#include "utils/texture.h"
// #include "setmaterial.cpp"
// #include "setlight.cpp"
// #include "osucone.cpp"
// #include "osutorus.cpp"
// #include "bmptotexture.cpp"
// #include "loadobjfile.cpp"

//	This is a sample OpenGL / GLFW program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glfw menu
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

const char *WINDOWTITLE = "OpenGL / GLFW Sample -- Aidan Schmitigal";
const char *GLUITITLE = "User Interface Window";

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

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// line width for the axes:

const GLfloat AXES_WIDTH = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

// non-constant global variables:

float Xmouse, Ymouse;  // mouse values

GLuint AxesList;            // list to hold the axes
GLFWwindow *MainWindow;     // window id for main graphics window
bool WindowVisible = true;  // is the window visible?
float Scale = 0.3;          // scaling factor
float SceneTime = 0.0;      // used for animation, # of seconds elapsed
float DeltaTime = 0.0;      // used for animation, # of seconds since last frame
float Xrot = 20, Yrot = 0;  // rotation angles in degrees
glm::vec3 CameraTranslation = glm::vec3(0.f, 0.f, 0.f);

bool ImGuiSceneRenderHovered = true;
float scene_window_width = INIT_WINDOW_WIDTH;
float scene_window_height = INIT_WINDOW_HEIGHT;

// function prototypes:
void Animate();
void DisplayImGui();
void Display();
void InitGraphics();
void InitImGui();
void InitAnimations();
void InitLists();

// Event Callbacks
void Keyboard(GLFWwindow *window, int key, int scancode, int action, int mods);
void MouseButton(GLFWwindow *window, int button, int action, int mods);
void MouseMotion(GLFWwindow *window, double xpos, double ypos);
void MouseWheel(GLFWwindow *window, double xoffset, double yoffset);
void Resize(GLFWwindow *window, int width, int height);
void Visibility(GLFWwindow *window, int focused);

void Reset();
void ExitProgram();

void Axes(float);
void HsvRgb(float[3], float[3]);

// these are here for when you need them -- just uncomment the ones you need:

float NowS0, NowT0, NowD;
GLSLProgram Pattern;

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// main program:
// MARK: Main
int main(int argc, char *argv[]) {
  // ConsoleWindow::CreateConsole();

  glfwSetErrorCallback(glfw_error_callback);

  // turn on the glut package:
  // (do this before checking argc and argv since glutInit might
  // pull some command line arguments out)

  if (!glfwInit())
    return 1;

  // setup all the graphics stuff:
  // this needs to be called once and only once!

  InitGraphics();

  noise_setseed(1);

  InitImGui();

  // create the display lists that **will not change**:

  InitLists();

  while (!glfwWindowShouldClose(MainWindow)) {
    // draw the scene once and wait for some interaction:
    // (this will never return)

    glfwPollEvents();
    // If the window is minimized, we stop rendering the scene
    if (glfwGetWindowAttrib(MainWindow, GLFW_ICONIFIED) == GLFW_TRUE || glfwGetWindowAttrib(MainWindow, GLFW_VISIBLE) == GLFW_FALSE) {
      ImGui_ImplGlfw_Sleep(100);
      continue;
    }
    Animate();
    Display();
    ShaderEditor::checkShaderFiles();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(MainWindow);
  glfwTerminate();

  // ConsoleWindow::DestroyConsole();

  return 0;
}

void ExitProgram() {
  glfwSetWindowShouldClose(MainWindow, GL_TRUE);
}

// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
// this is typically where animation parameters are set

// MARK: Animate
double previousTime = 0;
void Animate() {
  double currentTime = glfwGetTime();
  DeltaTime = currentTime - previousTime;
  previousTime = currentTime;

  if (Debug::freezeTime == false) {
    SceneTime += DeltaTime;
  }
}

// MARK: DisplayImGui
void DisplayImGui() {
  ImGui::NewFrame();

  ImGuizmo::BeginFrame();

  ImGuiIO &io = ImGui::GetIO();

  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  window_flags |= ImGuiWindowFlags_NoBackground;

  ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
  ImGui::PopStyleVar(2);

  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoDockingOverCentralNode | ImGuiDockNodeFlags_PassthruCentralNode;
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
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Asset Browser", nullptr, &Assets::viewAssetBrowser);
      ImGui::MenuItem("Objects", nullptr, &Objects::viewObjectsPanel);
      ImGui::MenuItem("Lights Editor", nullptr, &LightsEditor::viewLightsPanel);
      ImGui::MenuItem("Console", nullptr, &ConsoleWindow::viewConsolePanel);
      ImGui::MenuItem("Debug", nullptr, &Debug::viewDebugPanel);
      ImGui::MenuItem("ImGui Demo", nullptr, &Debug::viewDemoWindow);
      ImGui::MenuItem("Shaders List", nullptr, &ShaderEditor::viewShaderListPanel);
      ImGui::MenuItem("Shader Properties", nullptr, &ShaderEditor::viewShaderPropertiesPanel);
      ImGui::MenuItem("Overlay", nullptr, &Debug::viewOverlay);
      ImGui::EndMenu();
    }
    ImGui::Text("%.1f FPS (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);

    ImGui::SameLine(ImGui::GetWindowWidth() - 60);
    if (ImGui::MenuItem("Quit", "Alt+F4")) {
      printf("Quit\n");
      ExitProgram();
    }

    ImGui::EndMainMenuBar();
  }

  const bool should_open_popup =
      (ImGui::IsKeyDown(ImGuiKey_GraveAccent) ||
       ImGui::IsMouseDown(ImGuiMouseButton_Right));

  {
    if (!ImGui::IsAnyItemHovered() && should_open_popup) {
      ImGui::OpenPopup("PieMenu");
    }

    if (BeginPiePopup("PieMenu", 1)) {
      if (PieMenuItem("Front")) {
        Xrot = 0.f;
        Yrot = 0.f;
      }
      if (PieMenuItem("Back")) {
        Xrot = 0.f;
        Yrot = -180.f;
      }
      if (PieMenuItem("Left")) {
        Xrot = 0.f;
        Yrot = 90.f;
      }
      if (PieMenuItem("Right")) {
        Xrot = 0.f;
        Yrot = -90.f;
      }
      if (PieMenuItem("Top")) {
        Xrot = 90.f;
        Yrot = 0.f;
      }
      if (PieMenuItem("Bottom")) {
        Xrot = -90.f;
        Yrot = 0.f;
      }

      EndPiePopup();
    }
  }

  ImGui::End();

  if (Debug::viewDemoWindow)
    ImGui::ShowDemoWindow(&Debug::viewDemoWindow);

  LightsEditor::ShowLightsPanel();
  ConsoleWindow::ShowConsolePanel();
  Assets::ShowExampleAppAssetsBrowser();
  Objects::ShowObjectsPanel();
  ShaderEditor::ShowShaderListPanel();
  ShaderEditor::ShowShaderEditorPanel();
  ShaderEditor::ShowShaderPropertiesPanel();
  Debug::ShowDebugPanel();
  if (Debug::viewDebugPanel && ImGui::Begin("Debug")) {
    ImGui::DragFloat("Scale", &Scale, 0.01f, 0.f, 10.f);
    ImGui::Text("Scene Time: %f", SceneTime);

    ImGui::End();
  }

  Debug::ShowOverlay();

  if (Debug::viewOverlay && ImGui::Begin("Stats Overlay")) {
    ImGui::Text("Scene Time: %f", SceneTime);
    ImGui::Text("Camera Position: %f, %f, %f", CameraTranslation[0], CameraTranslation[1], CameraTranslation[2]);
    ImGui::Text("Camera Rotation: %f, %f", Xrot, Yrot);
    ImGui::End();
  }
  // View popup
}
// draw the complete scene:
// MARK: Display
void Display() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();

  DisplayImGui();

  // set the viewport to be a square centered in the window:

  int displayWidth, displayHeight;
  glfwGetFramebufferSize(MainWindow, &displayWidth, &displayHeight);
  glViewport(0, 0, displayWidth, displayHeight);

  glEnable(GL_DEPTH_TEST);

  // erase the background:
  glDrawBuffer(GL_BACK);
  glClearColor(LightsEditor::clearColor[0] * LightsEditor::clearColor[3], LightsEditor::clearColor[1] * LightsEditor::clearColor[3], LightsEditor::clearColor[2] * LightsEditor::clearColor[3], LightsEditor::clearColor[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

  // set the viewing volume:
  // remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
  // USE gluOrtho2D( ) IF YOU ARE DOING 2D !

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // if (NowProjection == ORTHO)
  //   glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
  // else
  gluPerspective(70.f, (GLdouble)displayWidth / (GLdouble)displayHeight, 0.1f, 1000.f);

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

  // Translate the camera
  glTranslatef(CameraTranslation[0], CameraTranslation[1], CameraTranslation[2]);

  // possibly draw the axes:

  if (Debug::axes != 0) {
    // glColor3fv(&Colors[NowColor][0]);
    glCallList(AxesList);
  }

  // since we are using glScalef( ), be sure the normals get unitized:
  glEnable(GL_NORMALIZE);

  Objects::DisplayObjects();

  Pattern.UnUse();

  // MARK: ImGui Render
  float cameraView[16];
  float cameraProjection[16];

  glGetFloatv(GL_MODELVIEW_MATRIX, cameraView);
  glGetFloatv(GL_PROJECTION_MATRIX, cameraProjection);
  Objects::DisplayGizmo(cameraView, cameraProjection);
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  GLFWwindow *backup_current_context = glfwGetCurrentContext();
  ImGui::UpdatePlatformWindows();
  ImGui::RenderPlatformWindowsDefault();
  glfwMakeContextCurrent(backup_current_context);

  // swap the double-buffered framebuffers:
  glfwSwapBuffers(MainWindow);

  // be sure the graphics buffer has been sent:
  glFlush();
}

// initialize the glut and OpenGL libraries:
//	also setup callback functions

// MARK: InitGraphics
void InitGraphics() {
  fprintf(stderr, "Starting InitGraphics.\n");

  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);  // 3.2+ only

  // request the display modes:
  // ask for red-green-blue-alpha color, double-buffering, and z-buffering:

  // glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  // set the initial window configuration:

  // open the window and set its title:
  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  MainWindow = glfwCreateWindow(mode->width, mode->height, WINDOWTITLE, nullptr, nullptr);
  if (MainWindow == nullptr) {
    fprintf(stderr, "Could not open window with GLFW3\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(MainWindow);
  glfwSwapInterval(1);  // Enable vsync

  // ImGui will overwrite but save the callbacks and call them for us when an event happens.

  glfwSetKeyCallback(MainWindow, Keyboard);
  glfwSetMouseButtonCallback(MainWindow, MouseButton);
  glfwSetCursorPosCallback(MainWindow, MouseMotion);
  glfwSetWindowSizeCallback(MainWindow, Resize);
  glfwSetScrollCallback(MainWindow, MouseWheel);
  glfwSetWindowFocusCallback(MainWindow, Visibility);

  ShaderEditor::InitTextures();

  // TODO: Find a better spot of Program loading
  // Pattern.Init();
  // bool valid = Pattern.Create((char *)"./shaders/pattern.vert", (char *)"./shaders/pattern.frag");
  // if (!valid)
  //   fprintf(stderr, "Could not create the Pattern shader!\n");
  // else
  //   fprintf(stderr, "Pattern shader created!\n");

  // // set the uniform variables that will not change:

  // Pattern.Use();
  // Pattern.SetUniformVariable((char *)"uKa", 0.1f);
  // Pattern.SetUniformVariable((char *)"uKd", 0.5f);
  // Pattern.SetUniformVariable((char *)"uKs", 0.4f);
  // Pattern.SetUniformVariable((char *)"uColor", 1.f, 0.5f, 0.f, 1.f);
  // Pattern.SetUniformVariable((char *)"uSpecularColor", 1.f, 1.f, 1.f, 1.f);
  // Pattern.SetUniformVariable((char *)"uShininess", 12.f);
  // Pattern.UnUse();
}

// MARK: UserData

static void *UserData_ReadOpen(ImGuiContext *, ImGuiSettingsHandler *, const char *name) {
  return (void *)"Panel Visibility";
}

static void UserData_ReadLine(ImGuiContext *, ImGuiSettingsHandler *, void *entry, const char *line) {
  // printf("UserDat_ReadLine: %s | %s\n", entry, line);
  // Line example: "ViewDebugPanel = 0" or "ViewDemoWindow = 1"

  if (strncmp(line, "ViewDebugPanel =", 16) == 0) {
    Debug::viewDebugPanel = atoi(line + 17);
    // printf("ViewDebugPanel: %d\n", Debug::viewDebugPanel);
  } else if (strncmp(line, "ViewDemoWindow =", 16) == 0) {
    Debug::viewDemoWindow = atoi(line + 17);
    // printf("ViewDemoWindow: %d\n", Debug::viewDemoWindow);
  } else if (strncmp(line, "ViewAssetBrowser =", 18) == 0) {
    Assets::viewAssetBrowser = atoi(line + 19);
    // printf("ViewAssetBrowser: %d\n", Assets::viewAssetBrowser);
  } else if (strncmp(line, "ViewLightsPanel =", 17) == 0) {
    LightsEditor::viewLightsPanel = atoi(line + 18);
    // printf("ViewLightsPanel: %d\n", LightsEditor::viewLightsPanel);
  } else if (strncmp(line, "ViewConsolePanel =", 17) == 0) {
    ConsoleWindow::viewConsolePanel = atoi(line + 18);
    // printf("ViewConsolePanel: %d\n", ConsoleWindow::viewConsolePanel);
  } else if (strncmp(line, "ViewOverlay =", 13) == 0) {
    Debug::viewOverlay = atoi(line + 14);
    // printf("ViewOverlay: %d\n", Debug::viewOverlay);
  } else if (strncmp(line, "ViewOverlayLocation =", 21) == 0) {
    Debug::overlayLocation = atoi(line + 22);
    // printf("ViewOverlayLocation: %d\n", Debug::overlayLocation);
    // } else if (strncmp(line, "ViewShaderEditorPanel =", 23) == 0) {
    //   ShaderEditor::viewShaderEditorPanel = atoi(line + 24);
    // printf("ViewShaderEditorPanel: %d\n", ShaderEditor::viewShaderEditorPanel);
  } else {
    printf("Unknown Line: %s\n", line);
  }
}

static void UserData_WriteAll(ImGuiContext *ctx, ImGuiSettingsHandler *handler, ImGuiTextBuffer *buf) {
  printf("Saving...\n");
  buf->appendf("[%s][%s]\n", "UserData", "Panel Visbility");
  buf->appendf("%s = %d\n", "ViewDebugPanel", Debug::viewDebugPanel);
  buf->appendf("%s = %d\n", "ViewDemoWindow", Debug::viewDemoWindow);
  buf->appendf("%s = %d\n", "ViewAssetBrowser", Assets::viewAssetBrowser);
  buf->appendf("%s = %d\n", "ViewLightsPanel", LightsEditor::viewLightsPanel);
  buf->appendf("%s = %d\n", "ViewConsolePanel", ConsoleWindow::viewConsolePanel);
  buf->appendf("%s = %d\n", "ViewOverlay", Debug::viewOverlay);
  buf->appendf("%s = %d\n", "ViewOverlayLocation", Debug::overlayLocation);
  // buf->appendf("%s = %d\n", "ViewShaderEditorPanel", ShaderEditor::viewShaderEditorPanel);
  buf->append("\n");
}

// MARK: InitImGui
void InitImGui() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiSettingsHandler ini_handler;
  ini_handler.TypeName = "UserData";
  ini_handler.TypeHash = ImHashStr("UserData");
  ini_handler.ReadOpenFn = UserData_ReadOpen;
  ini_handler.ReadLineFn = UserData_ReadLine;
  ini_handler.WriteAllFn = UserData_WriteAll;
  ImGui::AddSettingsHandler(&ini_handler);

  ShaderEditor::RegisterImGuiSettingsHandler();

  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport / Platform Windows

  // Setup Dear ImGui style
  StyleColorsMaterial();

  ImGui::LoadIniSettingsFromDisk(ImGui::GetIO().IniFilename);

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(MainWindow, false);  // False for InstallCallbacks
  ImGui_ImplOpenGL3_Init("#version 120");
  // ImGui_ImplOpenGL3_Init("#version 330");
  ImGui_ImplGlfw_InstallCallbacks(MainWindow);  // just so we can do it here for clarity

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

  // create the object:
  Objects::InitLists();

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
void Keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureKeyboard)
    return;

  // printf("Keyboard: %i, %i, %i, %i\n", key, scancode, action, mods);

  switch (key) {
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
      break;

    case 'q':
    case 'Q':
    case ESCAPE:
      ExitProgram();
      break;  // happy compiler

      // default:
      // fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", key, key);
  }
}

// called when the mouse button transitions down or up:
// MARK: Mouse Button
void MouseButton(GLFWwindow *window, int button, int action, int mods) {
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
    return;

  // printf("MouseButton: %i, %i, %i\n", button, action, mods);
}

// called when the mouse moves while a button is down:
// MARK: Mouse Motion
void MouseMotion(GLFWwindow *window, double xpos, double ypos) {
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
    return;

  if (xpos < 0 || ypos < 0 || xpos > io.DisplaySize.x || ypos > io.DisplaySize.y)
    return;

  // printf("MosueMoved: %f, %f\n", xpos, ypos);

  int dx = xpos - Xmouse;  // change in mouse coords
  int dy = ypos - Ymouse;

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      // Translate the camera

      // Current transformation matrix
      glm::mat4 currentMatrix = glm::mat4(1.f);
      currentMatrix = glm::translate(currentMatrix, glm::vec3(CameraTranslation.x, 0.f, 0.f));
      currentMatrix = glm::scale(currentMatrix, glm::vec3(Scale, Scale, Scale));
      currentMatrix = glm::rotate(currentMatrix, glm::radians(Yrot), glm::vec3(0.f, 1.f, 0.f));

      glm::vec3 upDir = glm::vec3(0.f, 1.f, 0.f);
      glm::vec3 lookDir = glm::vec3(cos((Yrot + 90.f) * F_PI / 180.f), 0, sin((Yrot + 90.f) * F_PI / 180.f));
      glm::vec3 xRotDir = glm::cross(upDir, lookDir);

      currentMatrix = glm::rotate(currentMatrix, glm::radians(Xrot), xRotDir);

      glm::vec3 screenspace_translation = glm::vec3(dx, -dy, 0.f);
      glm::vec4 worldspace_translation = glm::inverse(currentMatrix) * glm::vec4(screenspace_translation, 0.f);
      glm::vec3 worldspace_translation_vec3 = glm::vec3(worldspace_translation);
      CameraTranslation += worldspace_translation_vec3 / 1000.f;
    } else {
      // Rotate the camera
      Xrot += (ANGFACT * dy);
      Yrot += (ANGFACT * dx);
    }
  }

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    Scale += SCLFACT * (float)(dx - dy);

    // keep object from turning inside-out or disappearing:

    if (Scale < MINSCALE)
      Scale = MINSCALE;
  }

  Xmouse = xpos;  // new current position
  Ymouse = ypos;
}

void MouseWheel(GLFWwindow *window, double xoffset, double yoffset) {
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
    return;

  Scale += yoffset * SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;

  // keep object from turning inside-out or disappearing:
  if (Scale < MINSCALE)
    Scale = MINSCALE;
}

// called when user resizes the window:
// MARK: Resize
void Resize(GLFWwindow *window, int width, int height) {
  // don't really need to do anything since window size is
  // checked each time in Display( ):
}

// handle a change to the window's visibility:
// MARK: Visibility
void Visibility(GLFWwindow *window, int state) {
  // printf("Visibility: %i\n", state);
  WindowVisible = state == GLFW_TRUE;
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