#include "debug.h"

#include "imgui.h"

namespace Debug {

bool flatShading = true;
bool showWireframe = false;
bool showNormals = false;
bool autoRotate = false;
bool lightingEnabled = true;
bool freezeTime = false;
bool axes = true;
int overlayLocation = -1;

bool viewDebugPanel = true;
bool viewDemoWindow = false;
bool viewOverlay = false;

void ShowDebugPanel() {
  if (!viewDebugPanel) return;

  ImGui::Begin("Debug");

  ImGui::Checkbox("Show Overlay", &viewOverlay);
  ImGui::Checkbox("Flat Shading", &flatShading);
  ImGui::Checkbox("Show Wireframe", &showWireframe);
  ImGui::Checkbox("Show Normals", &showNormals);
  ImGui::Checkbox("Auto Rotate", &autoRotate);
  ImGui::Checkbox("Show Demo Window", &viewDemoWindow);
  ImGui::Checkbox("Lighting Enabled", &lightingEnabled);
  ImGui::Checkbox("Freeze Time", &freezeTime);
  ImGui::Checkbox("Axes", &axes);

  ImGui::End();
}

void ShowOverlay() {
  if (!viewOverlay) return;

  ImGuiIO& io = ImGui::GetIO();
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
  if (overlayLocation >= 0) {
    const float PAD = 10.0f;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos;  // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = (overlayLocation & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y = (overlayLocation & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (overlayLocation & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (overlayLocation & 2) ? 1.0f : 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowViewport(viewport->ID);
    window_flags |= ImGuiWindowFlags_NoMove;
  } else if (overlayLocation == -2) {
    // Center window
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    window_flags |= ImGuiWindowFlags_NoMove;
  }
  ImGui::SetNextWindowBgAlpha(0.35f);  // Transparent background
  if (viewOverlay && ImGui::Begin("Stats Overlay", nullptr, window_flags)) {
    if (ImGui::IsMousePosValid())
      ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
    else
      ImGui::Text("Mouse Position: <invalid>");

    ImGui::Text("%.1f FPS (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);

    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Custom", nullptr, overlayLocation == -1)) overlayLocation = -1;
      if (ImGui::MenuItem("Center", nullptr, overlayLocation == -2)) overlayLocation = -2;
      if (ImGui::MenuItem("Top-left", nullptr, overlayLocation == 0)) overlayLocation = 0;
      if (ImGui::MenuItem("Top-right", nullptr, overlayLocation == 1)) overlayLocation = 1;
      if (ImGui::MenuItem("Bottom-left", nullptr, overlayLocation == 2)) overlayLocation = 2;
      if (ImGui::MenuItem("Bottom-right", nullptr, overlayLocation == 3)) overlayLocation = 3;
      if (viewOverlay && ImGui::MenuItem("Close")) viewOverlay = false;
      ImGui::EndPopup();
    }
    ImGui::End();
  }
}

}  // namespace Debug