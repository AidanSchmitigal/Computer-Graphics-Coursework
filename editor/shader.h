#pragma once

#include <string>
#include <variant>
#include <vector>

#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "uniform.h"
#include "utils/glslprogram.h"

namespace ShaderEditor {

extern bool viewShaderListPanel;
extern bool viewShaderPropertiesPanel;

void ShowShaderListPanel();
void ShowShaderEditorPanel();
void ShowShaderPropertiesPanel();
void RegisterImGuiSettingsHandler();

struct Shader {
  int id;
  char name[256];
  std::vector<std::string> files;
  std::vector<Uniform> uniforms;
  GLSLProgram program;
  long readTime;
  std::string error;

  ~Shader() {
    program.Delete();
    // files.clear();
    // uniforms.clear();
  }

  void use();
  void unUse();
};

void checkShaderFiles();
void InitShaders();

}  // namespace ShaderEditor