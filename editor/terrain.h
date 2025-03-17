#pragma once

#include <string>

namespace TerrainEditor {

extern bool viewLayersPanel;

void ShowLayersPanel();
void ModifyShaders();

struct Layer {
  bool enabled = false;
  float scale = 1.f;
  float shift = 0.f;
  bool textureNotModifier = true;
  int textureOrModifierId = 0;
  float positionScale = 1.f;
  float positionShift = 0.f;
  float var1 = 1.f;
  float var2 = 0.f;
  float var3 = 1.f;
  int maskId = 0;
  float maskVar1 = 0.f;
  float maskVar2 = 1.f;

  // Layer(bool textureNotModifier) : textureNotModifier(textureNotModifier) {}
};

}  // namespace TerrainEditor