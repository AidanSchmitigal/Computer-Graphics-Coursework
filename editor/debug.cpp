#include "debug.h"

#include "imgui.h"

namespace Debug {

bool flatShading = true;
bool showWireframe = false;
bool showNormals = false;
bool autoRotate = false;
bool showDemoWindow = false;
bool lightingEnabled = true;
bool freezeTime = false;

void ShowDebugPanel() {
  ImGui::Begin("Debug");

  ImGui::Checkbox("Flat Shading", &flatShading);
  ImGui::Checkbox("Show Wireframe", &showWireframe);
  ImGui::Checkbox("Show Normals", &showNormals);
  ImGui::Checkbox("Auto Rotate", &autoRotate);
  ImGui::Checkbox("Show Demo Window", &showDemoWindow);
  ImGui::Checkbox("Lighting Enabled", &lightingEnabled);
  ImGui::Checkbox("Freeze Time", &freezeTime);

  ImGui::End();
}

}  // namespace Debug