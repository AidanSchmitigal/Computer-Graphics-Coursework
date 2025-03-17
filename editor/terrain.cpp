#include "terrain.h"

#include <array>
#include <string>
#include <vector>

#include "imgui.h"
#include "objects.h"
#include "shader.h"

namespace TerrainEditor {

bool viewLayersPanel = true;

const int MAX_LAYERS = 5;
std::array<Layer, 5> layers = {
    Layer(),
    Layer(),
    Layer(),
    Layer(),
    Layer(),
};

std::weak_ptr<Objects::Object> terrainObject;
std::shared_ptr<ShaderEditor::Shader> terrainShader = nullptr;

const int NUM_TEXTURES = 7;
const std::array<std::string, NUM_TEXTURES> textureNames = {
    // Bases
    "Value Noise",
    "Perlin Noise",
    "Simplex Noise",
    "Ridged Noise",
    "Worley Noise",
    "FBm Noise",
    "Turbulence Noise",
};

const int NUM_MODIFIERS = 16;
const std::array<std::string, NUM_MODIFIERS> modifierNames = {
    // Modifiers
    "Thermal",
    "Hydraulic",
    "Wind",
    "Canyon",
    "Terracing",
    "Fault Line",
    "Mountain Ranges",
    "Cratered",
    "Cave",
    "Volcanoes",
    "Swamps",
    "Island Chains",
    "Smooth",
    "Clamp",
    "Sharp Ridge",
    "Flow Map",
};

const int NUM_MASKS = 3;
const std::array<std::string, NUM_MASKS> maskNames = {
    "None",
    "Height",
    "Slope",
};

const int NUM_MATERIALS = 12;
const std::array<std::string, NUM_MATERIALS> materialNames = {
    "Smooth Rock",
    "Rough Rock",
    "Grass",
    "Snow",
    "Ice",
    "Dirt",
    "Sand",
    "Volcanic Rock",
    "Moss",
    "Clay",
    "Slate",
    "Gravel",
};

// Color gradient presets
const int NUM_GRADIENTS = 5;
const std::array<std::string, NUM_GRADIENTS> gradientPresets = {
    "Default Mountain",
    "Desert",
    "Tropical",
    "Tundra",
    "Volcanic",
};

// Shader gradient parameters
const int MAX_GRADIENT_COLORS = 8;
std::array<ImVec4, MAX_GRADIENT_COLORS> colorGradient = {
    ImVec4(0.0f, 0.5f, 0.0f, 1.0f),     // Grass
    ImVec4(0.2f, 0.6f, 0.2f, 1.0f),     // Light grass
    ImVec4(0.6f, 0.5f, 0.3f, 1.0f),     // Dirt
    ImVec4(0.5f, 0.5f, 0.5f, 1.0f),     // Rock
    ImVec4(0.65f, 0.65f, 0.65f, 1.0f),  // Light rock
    ImVec4(0.8f, 0.8f, 0.8f, 1.0f),     // Stone
    ImVec4(0.9f, 0.9f, 0.9f, 1.0f),     // Light snow
    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),     // Snow
};

std::array<float, MAX_GRADIENT_COLORS> gradientPercentages = {
    0.0f, 0.15f, 0.25f, 0.35f, 0.5f, 0.65f, 0.8f, 1.0f};

std::array<int, MAX_GRADIENT_COLORS> materialTypes = {
    2, 8, 5, 0, 1, 10, 3, 4  // Index into materialNames (zero-indexed)
};

int colorCount = 8;
float smoothness = 0.1f;
bool materialPanelOpen = false;
bool layersPanelOpen = false;
float minColorHeight = 0.f;
float maxColorHeight = 1.f;

float heightScale = 1.f;
float scale = 1.f;
int selectedResolution = 0;
bool valuesChanged = true;

// Set colors for a specific preset
void SetGradientPreset(int presetIndex) {
  valuesChanged = true;

  switch (presetIndex) {
    case 0:  // Default Mountain
      colorGradient = {
          ImVec4(0.0f, 0.5f, 0.0f, 1.0f),     // Grass
          ImVec4(0.2f, 0.6f, 0.2f, 1.0f),     // Light grass
          ImVec4(0.6f, 0.5f, 0.3f, 1.0f),     // Dirt
          ImVec4(0.5f, 0.5f, 0.5f, 1.0f),     // Rock
          ImVec4(0.65f, 0.65f, 0.65f, 1.0f),  // Light rock
          ImVec4(0.8f, 0.8f, 0.8f, 1.0f),     // Stone
          ImVec4(0.9f, 0.9f, 0.9f, 1.0f),     // Light snow
          ImVec4(1.0f, 1.0f, 1.0f, 1.0f),     // Snow
      };
      gradientPercentages = {0.0f, 0.15f, 0.25f, 0.35f, 0.5f, 0.65f, 0.8f, 1.0f};
      materialTypes = {2, 8, 5, 0, 1, 10, 3, 4};
      break;

    case 1:  // Desert
      colorGradient = {
          ImVec4(0.85f, 0.75f, 0.5f, 1.0f),   // Sand
          ImVec4(0.82f, 0.7f, 0.45f, 1.0f),   // Lighter sand
          ImVec4(0.7f, 0.6f, 0.4f, 1.0f),     // Darker sand
          ImVec4(0.65f, 0.55f, 0.35f, 1.0f),  // Rocky sand
          ImVec4(0.6f, 0.5f, 0.3f, 1.0f),     // Sandy rock
          ImVec4(0.55f, 0.45f, 0.25f, 1.0f),  // Rocky terrain
          ImVec4(0.5f, 0.4f, 0.2f, 1.0f),     // Dark rocky terrain
          ImVec4(0.45f, 0.35f, 0.15f, 1.0f),  // Mountain rock
      };
      gradientPercentages = {0.0f, 0.15f, 0.3f, 0.45f, 0.6f, 0.75f, 0.85f, 1.0f};
      materialTypes = {6, 6, 6, 5, 5, 1, 1, 7};
      break;

    case 2:  // Tropical
      colorGradient = {
          ImVec4(0.05f, 0.3f, 0.6f, 1.0f),    // Water
          ImVec4(0.9f, 0.8f, 0.6f, 1.0f),     // Sand
          ImVec4(0.1f, 0.7f, 0.2f, 1.0f),     // Vibrant grass
          ImVec4(0.0f, 0.5f, 0.0f, 1.0f),     // Forest
          ImVec4(0.0f, 0.4f, 0.0f, 1.0f),     // Dense forest
          ImVec4(0.6f, 0.5f, 0.3f, 1.0f),     // Dirt
          ImVec4(0.5f, 0.5f, 0.5f, 1.0f),     // Rock
          ImVec4(0.65f, 0.65f, 0.65f, 1.0f),  // Mountain
      };
      gradientPercentages = {0.0f, 0.05f, 0.1f, 0.3f, 0.5f, 0.7f, 0.85f, 1.0f};
      materialTypes = {4, 6, 2, 8, 8, 5, 0, 1};
      break;

    case 3:  // Tundra
      colorGradient = {
          ImVec4(0.8f, 0.9f, 1.0f, 1.0f),  // Ice
          ImVec4(0.9f, 0.9f, 0.9f, 1.0f),  // Snow
          ImVec4(0.7f, 0.8f, 0.8f, 1.0f),  // Icy snow
          ImVec4(0.6f, 0.7f, 0.7f, 1.0f),  // Frozen ground
          ImVec4(0.5f, 0.5f, 0.5f, 1.0f),  // Rock
          ImVec4(0.4f, 0.4f, 0.4f, 1.0f),  // Dark rock
          ImVec4(0.3f, 0.3f, 0.3f, 1.0f),  // Darker rock
          ImVec4(0.2f, 0.2f, 0.2f, 1.0f),  // Mountain peak
      };
      gradientPercentages = {0.0f, 0.1f, 0.2f, 0.35f, 0.5f, 0.7f, 0.85f, 1.0f};
      materialTypes = {4, 3, 3, 3, 10, 1, 1, 1};
      break;

    case 4:  // Volcanic
      colorGradient = {
          ImVec4(0.1f, 0.1f, 0.1f, 1.0f),  // Dark volcanic rock
          ImVec4(0.2f, 0.2f, 0.2f, 1.0f),  // Volcanic rock
          ImVec4(0.3f, 0.2f, 0.2f, 1.0f),  // Reddish rock
          ImVec4(0.4f, 0.3f, 0.3f, 1.0f),  // Lighter volcanic rock
          ImVec4(0.5f, 0.4f, 0.3f, 1.0f),  // Dirt
          ImVec4(0.6f, 0.5f, 0.4f, 1.0f),  // Dry soil
          ImVec4(0.7f, 0.6f, 0.5f, 1.0f),  // Sandy soil
          ImVec4(0.8f, 0.7f, 0.6f, 1.0f),  // Ash
      };
      gradientPercentages = {0.0f, 0.15f, 0.3f, 0.45f, 0.6f, 0.75f, 0.9f, 1.0f};
      materialTypes = {7, 7, 7, 7, 5, 5, 11, 11};
      break;
  }

  colorCount = 8;  // All presets use 8 colors
}

void ShowColorGradientPanel() {
  ImGui::SeparatorText("Color Gradient");

  // Gradient preset selector
  if (ImGui::BeginCombo("Gradient Preset", "Select Preset")) {
    for (int i = 0; i < NUM_GRADIENTS; i++) {
      if (ImGui::Selectable(gradientPresets[i].c_str())) {
        SetGradientPreset(i);
      }
    }
    ImGui::EndCombo();
  }

  // Color count slider
  valuesChanged |= ImGui::SliderInt("Color Count", &colorCount, 2, MAX_GRADIENT_COLORS);

  // Smoothness slider
  valuesChanged |= ImGui::SliderFloat("Smoothness", &smoothness, 0.0f, 1.0f);

  // Min/Max Height sliders
  valuesChanged |= ImGui::DragFloat("Min Color Height", &minColorHeight);
  valuesChanged |= ImGui::DragFloat("Max Color Height", &maxColorHeight);

  // Color gradient editor
  for (int i = 0; i < colorCount; i++) {
    ImGui::PushID(i);

    char label[32];
    sprintf(label, "Color %d", i + 1);

    // Color picker
    valuesChanged |= ImGui::ColorEdit3(label, (float*)&colorGradient[i]);

    // Height percentage slider
    if (i > 0 && i < colorCount - 1) {
      char heightLabel[32];
      sprintf(heightLabel, "Height %d", i + 1);
      valuesChanged |= ImGui::SliderFloat(heightLabel, &gradientPercentages[i],
                                          gradientPercentages[i - 1],
                                          gradientPercentages[i + 1]);
    }

    // Material type selector
    char materialLabel[32];
    sprintf(materialLabel, "Material %d", i + 1);
    if (ImGui::BeginCombo(materialLabel, materialNames[materialTypes[i]].c_str())) {
      for (int j = 0; j < NUM_MATERIALS; j++) {
        if (ImGui::Selectable(materialNames[j].c_str(), materialTypes[i] == j)) {
          materialTypes[i] = j;
          valuesChanged = true;
        }
      }
      ImGui::EndCombo();
    }

    ImGui::PopID();

    if (i < colorCount - 1) {
      ImGui::Separator();
    }
  }
}

void ShowLayersPanel() {
  if (!viewLayersPanel) return;
  valuesChanged = false;

  ImGui::Begin("Terrain");

  if (terrainShader != nullptr) {
    ShaderEditor::showShaderError(terrainShader);
  }
  if (terrainObject.expired()) {
    terrainObject = Objects::getFirstObject();
  }

  // Properties
  ImGui::SeparatorText("Properties");

  if (ImGui::RadioButton("100", selectedResolution == 0)) {
    selectedResolution = 0;
    terrainObject.lock()->type = Objects::ObjectType::Terrain;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("200", selectedResolution == 1)) {
    selectedResolution = 1;
    terrainObject.lock()->type = Objects::ObjectType::Terrain2;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("400", selectedResolution == 2)) {
    selectedResolution = 2;
    terrainObject.lock()->type = Objects::ObjectType::Terrain4;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("800", selectedResolution == 3)) {
    selectedResolution = 3;
    terrainObject.lock()->type = Objects::ObjectType::Terrain8;
  }

  ImGui::Text("Height Scale: %f", heightScale);
  valuesChanged |= ImGui::SliderFloat("Height Scale", &heightScale, 0.f, 5.f);

  // ImGui::Text("Scale: %f", scale);
  // valuesChanged |= ImGui::SliderFloat("Scale", &scale, 0.f, 10.f);

  // Color Gradient & Material Panel
  materialPanelOpen = ImGui::CollapsingHeader("Color & Material Settings");
  if (materialPanelOpen) {
    ShowColorGradientPanel();
  }

  layersPanelOpen = ImGui::CollapsingHeader("Layers");
  if (layersPanelOpen) {
    // Layers
    ImGui::SeparatorText("Layers");

    for (size_t i = 0; i < layers.size(); ++i) {
      auto& layer = layers[i];
      ImGui::PushID(static_cast<int>(i));  // Ensure unique ID for each layer

      // Header for each layer
      std::string name = layer.textureNotModifier ? (textureNames[layer.textureOrModifierId]) : (modifierNames[layer.textureOrModifierId]);
      bool open = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

      ImGui::SameLine();
      if (ImGui::Button("^")) {
        valuesChanged = true;
        if (i > 0) {
          std::swap(layer, layers[i - 1]);
        }
      }
      ImGui::SameLine();
      if (ImGui::Button("v")) {
        valuesChanged = true;
        if (i < layers.size() - 1) {
          std::swap(layer, layers[i + 1]);
        }
      }

      if (open) {
        valuesChanged |= ImGui::Checkbox("Enabled", &layer.enabled);

        if (ImGui::BeginCombo("Mask", maskNames[layer.maskId].c_str())) {
          for (int j = 0; j < NUM_MASKS; j++) {
            if (ImGui::Selectable(maskNames[j].c_str(), layer.maskId == j)) {
              layer.maskId = j;
              valuesChanged = true;
            }
          }
          ImGui::EndCombo();
        }
        if (layer.maskId != 0) {
          valuesChanged |= ImGui::DragFloat2("Min/Max", &layer.maskVar2, 0.001f, 0.f);
        }

        if (layer.textureNotModifier) {
          if (ImGui::BeginCombo("Base", textureNames[layer.textureOrModifierId].c_str())) {
            for (int j = 0; j < NUM_TEXTURES; j++) {
              if (ImGui::Selectable(textureNames[j].c_str(), layer.textureOrModifierId == j)) {
                layer.textureOrModifierId = j;
                valuesChanged = true;
              }
            }
            ImGui::EndCombo();
          }

          valuesChanged |= ImGui::SliderFloat("Scale", &layer.scale, -5.f, 5.f);

          valuesChanged |= ImGui::SliderFloat("Texture Scale", &layer.positionScale, 0.f, 10.f);

        } else {
          if (ImGui::BeginCombo("Modifier", modifierNames[layer.textureOrModifierId].c_str())) {
            for (int j = 0; j < NUM_MODIFIERS; j++) {
              if (ImGui::Selectable(modifierNames[j].c_str(), layer.textureOrModifierId == j)) {
                layer.textureOrModifierId = j;
                valuesChanged = true;
              }
            }
            ImGui::EndCombo();
          }

          valuesChanged |= ImGui::SliderFloat("Strength", &layer.scale, 0.f, 10.f);
        }

        valuesChanged |= ImGui::SliderFloat("Texture Var1", &layer.var1, -5.f, 5.f);
        valuesChanged |= ImGui::SliderFloat("Texture Var2", &layer.var2, -5.f, 5.f);
        valuesChanged |= ImGui::SliderFloat("Texture Var3", &layer.var3, -5.f, 5.f);
        valuesChanged |= ImGui::DragFloat("Position Shift", &layer.positionShift, 0.001f);
        valuesChanged |= ImGui::SliderFloat("Height Shift", &layer.shift, -5.f, 5.f);
      }

      ImGui::PopID();
    }
  }
  ImGui::End();
}

void ModifyShaders() {
  if (terrainShader == nullptr) {
    terrainShader = ShaderEditor::getShader("terrain");
  } else if (!valuesChanged)
    return;

  auto& uniforms = terrainShader->uniforms;
  // printf("update\n");

  for (int i = 0; i < uniforms.size(); i++) {
    auto& uniform = uniforms[i];

    // printf("uniform: %s\n", uniform.name.c_str());

    if (uniform.name == "uHeightScale") {
      uniform.value = heightScale;
    } else if (uniform.name == "uScale") {
      uniform.value = scale;
    } else if (strncmp(uniform.name.c_str(), "layers[", 7) == 0) {
      int layerIndex = atoi(uniform.name.c_str() + 7);
      std::string layerValue = uniform.name.c_str() + 7 + 3;

      if (layerIndex >= layers.size())
        continue;

      auto layer = layers[layerIndex];
      if (layerValue == "textureOrModifierId") {
        uniform.value = layer.textureOrModifierId;
      } else if (layerValue == "textureNotModifier") {
        uniform.value = layer.textureNotModifier;
      } else if (layerValue == "scale") {
        uniform.value = layer.scale;
      } else if (layerValue == "var1") {
        uniform.value = layer.var1;
      } else if (layerValue == "var2") {
        uniform.value = layer.var2;
      } else if (layerValue == "var3") {
        uniform.value = layer.var3;
      } else if (layerValue == "maskId") {
        uniform.value = layer.maskId;
      } else if (layerValue == "maskVar1") {
        uniform.value = layer.maskVar1;
      } else if (layerValue == "maskVar2") {
        uniform.value = layer.maskVar2;
      } else if (layerValue == "shift") {
        uniform.value = layer.shift;
      } else if (layerValue == "enabled") {
        uniform.value = layer.enabled;
      } else if (layerValue == "positionScale") {
        uniform.value = layer.positionScale;
      } else if (layerValue == "positionShift") {
        uniform.value = layer.positionShift;
      }
    } else if (uniform.name == "uTex1") {
      uniform.value = 0u;  // Noise texture
    } else if (uniform.name == "uTex2") {
      uniform.value = 5u;  // Lena texture
    }
    // Color gradient parameters
    else if (uniform.name == "uColorCount") {
      uniform.value = colorCount;
    } else if (uniform.name == "uSmoothness") {
      uniform.value = smoothness;
    } else if (uniform.name == "uMinColorHeight") {
      uniform.value = minColorHeight;
    } else if (uniform.name == "uMaxColorHeight") {
      uniform.value = maxColorHeight;
    } else if (strncmp(uniform.name.c_str(), "colorLayers[", 12) == 0) {
      int colorIndex = atoi(uniform.name.c_str() + 12);
      if (colorIndex < colorCount) {
        std::string layerName = uniform.name.c_str() + 12 + 3;
        if (strncmp(layerName.c_str(), "color", 5) == 0) {
          uniform.value = glm::vec3(colorGradient[colorIndex].x, colorGradient[colorIndex].y, colorGradient[colorIndex].z);
        } else if (strncmp(layerName.c_str(), "height", 6) == 0) {
          uniform.value = gradientPercentages[colorIndex];
        } else if (strncmp(layerName.c_str(), "materialType", 12) == 0) {
          uniform.value = materialTypes[colorIndex] + 1;
        }
      }
    }
  }
}

}  // namespace TerrainEditor