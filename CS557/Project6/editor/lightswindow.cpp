#include "lightswindow.h"

#include <vector>

#include "imgui.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

namespace LightsEditor {

bool viewLightsPanel = true;

// Define a struct for lights
struct Light {
  float position[3] = {0.0f, 0.0f, 0.0f};
  float color[3] = {1.0f, 1.0f, 1.0f};
  float direction[3] = {0.0f, -1.0f, 0.0f};  // For directional/spotlight
  float intensity = 1.0f;
  float cutoff = 45.0f;  // Spotlight cutoff angle
  bool enabled = true;
  int type = 0;  // 0 = Point, 1 = Directional, 2 = Spotlight
};

std::vector<Light> lights;
float ambientLight[4] = {0.3f, 0.3f, 0.3f, 1.f};
float clearColor[4] = {0.45f, 0.55f, 0.60f, 1.00f};

void ShowLightsPanel() {
  if (!viewLightsPanel) return;

  ImGui::Begin("Lights Panel");

  ImGui::ColorEdit3("Clear Color", clearColor);
  ImGui::ColorEdit3("Ambient Light Color", ambientLight);

  if (ImGui::Button("Add Light")) {
    if (lights.size() < 8) {
      lights.push_back(Light());
    }
  }

  ImGui::SameLine();
  if (ImGui::Button("Clear Lights")) {
    lights.clear();
  }

  for (size_t i = 0; i < lights.size(); ++i) {
    ImGui::PushID(static_cast<int>(i));  // Ensure unique ID for each light

    // Header for each light
    if (ImGui::CollapsingHeader("Light##lightHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Checkbox("Enabled", &lights[i].enabled);

      // Light type selection
      const char* lightTypes[] = {"Point", "Directional", "Spotlight"};
      ImGui::Combo("Type", &lights[i].type, lightTypes, IM_ARRAYSIZE(lightTypes));

      // Position (for all light types)
      ImGui::DragFloat3("Position", lights[i].position, 0.1f);

      // Direction (for directional/spotlight)
      if (lights[i].type == 1 || lights[i].type == 2) {
        ImGui::DragFloat3("Direction", lights[i].direction, 0.1f);
      }

      // Spotlight cutoff (only for spotlight)
      if (lights[i].type == 2) {
        ImGui::SliderFloat("Cutoff", &lights[i].cutoff, 0.0f, 90.0f);
      }

      // Color and intensity
      ImGui::ColorEdit3("Color", lights[i].color);
      ImGui::SliderFloat("Intensity", &lights[i].intensity, 0.0f, 10.0f);
    }

    ImGui::PopID();
  }

  ImGui::End();
}

void DisplayLights() {
  glEnable(GL_LIGHTING);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

  for (size_t i = 0; i < lights.size(); ++i) {
    GLenum lightID = GL_LIGHT0 + i;  // OpenGL supports GL_LIGHT0 to GL_LIGHT7

    const Light& light = lights[i];

    if (light.enabled) {
      glEnable(lightID);

      // Position
      GLfloat position[4] = {light.position[0], light.position[1], light.position[2], light.type == 1 ? 0.0f : 1.0f};
      glLightfv(lightID, GL_POSITION, position);

      // Direction (for directional or spotlight)
      if (light.type == 1 || light.type == 2) {
        GLfloat direction[3] = {light.direction[0], light.direction[1], light.direction[2]};
        glLightfv(lightID, GL_SPOT_DIRECTION, direction);
      }

      // Spotlight cutoff (only for spotlight)
      if (light.type == 2) {
        glLightf(lightID, GL_SPOT_CUTOFF, light.cutoff);
      } else {
        glLightf(lightID, GL_SPOT_CUTOFF, 180.0f);  // Disable spotlight cutoff
      }

      // Color and intensity
      GLfloat ambient[4] = {0.0f, 0.0f, 0.0f, 1.0f};  // Ambient set to zero
      GLfloat diffuse[4] = {light.color[0] * light.intensity, light.color[1] * light.intensity, light.color[2] * light.intensity, 1.0f};
      GLfloat specular[4] = {light.color[0] * light.intensity, light.color[1] * light.intensity, light.color[2] * light.intensity, 1.0f};

      glLightfv(lightID, GL_AMBIENT, ambient);
      glLightfv(lightID, GL_DIFFUSE, diffuse);
      glLightfv(lightID, GL_SPECULAR, specular);

      // Attenuation (default values here, can be exposed in the UI)
      glLightf(lightID, GL_CONSTANT_ATTENUATION, 1.0f);
      glLightf(lightID, GL_LINEAR_ATTENUATION, 0.0f);
      glLightf(lightID, GL_QUADRATIC_ATTENUATION, 0.0f);
    } else {
      glDisable(lightID);
    }
  }

  glDisable(GL_LIGHTING);
}

}  // namespace LightsEditor