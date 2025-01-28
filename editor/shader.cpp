#include "shader.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_internal.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

#include "utils/simplex.h"

namespace ShaderEditor {

const char* SHADERS_FOLDER = "shaders";

std::vector<std::unique_ptr<Shader>> shaders;
int selectedIndex = -1;
std::filesystem::file_time_type lastReadTime;

bool viewShaderListPanel = true;
bool viewShaderPropertiesPanel = true;

void compileShaderProgram(std::unique_ptr<Shader>& shader);
std::string GetGLTypeString(GLenum type);
void InitializeUniform(Uniform& uniform);

GLuint noise3dTexture;
GLuint noise2dTexture;

GLuint* textures2D = {&noise2dTexture};
const char* textures2DNames[] = {"noise2d"};
GLuint* textures3D = {&noise3dTexture};
const char* textures3DNames[] = {"noise3d"};

// MARK: findShaders
void findShaders() {
  std::unordered_map<std::string, std::vector<std::string>> shaderGroups;

  try {
    for (const auto& entry : std::filesystem::directory_iterator(SHADERS_FOLDER)) {
      if (!entry.is_regular_file()) continue;

      if (entry.path().extension() == ".vert" || entry.path().extension() == ".frag" || entry.path().extension() == ".geom") {
        std::string groupName = entry.path().stem().string();
        shaderGroups[groupName].push_back(entry.path().string());

        if (lastReadTime < entry.last_write_time()) {
          lastReadTime = entry.last_write_time();
        }
      }
    }

    for (const auto& entry : shaderGroups) {
      // Get shader from list if it exists
      auto existingShader = std::find_if(shaders.begin(), shaders.end(), [&entry](const std::unique_ptr<Shader>& shader) {
        return shader->name == entry.first;
      });
      if (existingShader != shaders.end()) {
        // Existing shader was found
        printf("Found shader: %s\n", entry.first.c_str());

        // Update files
        existingShader->get()->files = entry.second;
      } else {
        // New shader to be made

        printf("Making a new shader: %s\n", entry.first.c_str());

        auto shader = std::make_unique<Shader>();
        strncpy(shader->name, entry.first.c_str(), 256);
        shader->files = entry.second;

        existingShader = shaders.insert(shaders.end(), std::move(shader));
      }

      compileShaderProgram(*existingShader);
    }
  } catch (const std::filesystem::filesystem_error& e) {
    printf("Error: %s\n", e.what());
  }
}

// MARK: Compile
void compileShaderProgram(std::unique_ptr<Shader>& shader) {
  printf("Compiling shader: %s\n", shader->name);
  shader->error.clear();

  std::array<const char*, 6> shaderFiles = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
  int shaderFileIndex = 0;
  for (const auto& file : shader->files) {
    shaderFiles[shaderFileIndex++] = file.c_str();
    if (shaderFileIndex == 6) break;
  }

  shader->program.Init();
  if (!shader->program.Create(shaderFiles[0], shaderFiles[1], shaderFiles[2], shaderFiles[3], shaderFiles[4], shaderFiles[5])) {
    printf("Failed to create program\n");

    // Read the error log and put it in the shader
    std::ifstream logFile("glsllog.txt");
    if (logFile.is_open()) {
      std::string line;
      while (std::getline(logFile, line)) {
        shader->error += line;
        shader->error += "\n";
      }
    }

    return;
  }
  shader->program.Use();

  int numactiveuniforms;
  glGetProgramiv(shader->program.GetProgram(), GL_ACTIVE_UNIFORMS, &numactiveuniforms);
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error: %s\n", gluErrorString(error));
  }

  int bufsize;
  glGetProgramiv(shader->program.GetProgram(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufsize);
  char* lname = new char[bufsize + 1];

  for (int i = 0; i < numactiveuniforms; i++) {
    GLint lsize;
    GLenum ltype;
    glGetActiveUniform(shader->program.GetProgram(), i, bufsize, NULL, &lsize, &ltype, lname);

    auto existingUniform = std::find_if(shader->uniforms.begin(), shader->uniforms.end(), [&lname](const Uniform& uniform) {
      return uniform.name == lname;
    });
    if (existingUniform == shader->uniforms.end()) {
      printf("Found new uniform: %s\n", lname);

      // New uniform
      shader->uniforms.emplace_back(lsize, ltype, lname);
      InitializeUniform(shader->uniforms.back());
    }
  }
  delete[] lname;
  shader->program.UnUse();
}

std::chrono::time_point<std::chrono::system_clock> lastCheckTime = std::chrono::system_clock::now();
// MARK: Check Files
void checkShaderFiles() {
  if (std::chrono::system_clock::now() - lastCheckTime < std::chrono::seconds(1)) {
    return;
  }
  lastCheckTime = std::chrono::system_clock::now();

  try {
    for (const auto& entry : std::filesystem::directory_iterator(SHADERS_FOLDER)) {
      if (!entry.is_regular_file()) continue;  // Skip non-file entries

      const std::filesystem::file_time_type lastWriteTime = entry.last_write_time();

      if (lastWriteTime > lastReadTime) {
        findShaders();
        return;
      }
    }
  } catch (const std::filesystem::filesystem_error& e) {
    // Handle file system errors, e.g., permission issues or directory not found
    std::cerr << "Error checking shader files: " << e.what() << std::endl;
  }
}

// MARK: Select
void setSelectedShader(int index) {
  if (index < 0 || index >= shaders.size()) return;
  selectedIndex = index;
}

// MARK: Show Error
void showShaderError(int index) {
  if (index < 0 || index >= shaders.size()) return;
  auto& shader = shaders[index];
  if (shader->error.empty()) return;

  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImLerp(ImGui::GetStyle().Colors[ImGuiCol_WindowBg], ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 0.15f));
  ImGui::PushID(index);
  ImGui::Begin("##shader_error", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
  ImGui::PopStyleColor();
  ImGui::SeparatorText("ERROR IN SHADER COMPILATION");
  ImGui::Text("Shader Name: %s", shader->name);
  ImGui::Separator();
  ImGui::Text("Error: %s", shader->error.c_str());

  ImGui::End();
  ImGui::PopID();
}

// MARK: Show List
void ShowShaderListPanel() {
  if (!viewShaderListPanel) return;

  ImGui::Begin("Shader List");
  ImGui::Text("Shader List");
  ImGui::SameLine();
  if (ImGui::Button("Refresh")) {
    findShaders();
  }

  ImGui::Separator();

  {
    if (ImGui::BeginListBox("##draw_list", ImVec2(-1, -1))) {
      for (int i = 0; i < shaders.size(); i++) {
        const bool is_selected = (selectedIndex == i);
        if (ImGui::Selectable(shaders[i]->name, is_selected)) {
          setSelectedShader(i);
          printf("CLICKED!!\n");
        }

        // if (ImGui::IsItemHovered())
        //     item_highlighted_idx = i;

        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }
      if (shaders.size() == 0) {
        ImGui::Text("No shaders found");
      }
      ImGui::EndListBox();
    }
  }

  ImGui::End();
}

// MARK: Editor Panel
void ShowShaderEditorPanel() {
  if (selectedIndex != -1) {
    auto shader = shaders[selectedIndex].get();
    ImGui::Begin("Shader Editor");
    ImGui::Text("Shader Name: %s", shader->name);

    ImGui::Separator();

    showShaderError(selectedIndex);

    ImGui::Text("Uniforms");
    for (int i = 0; i < shader->uniforms.size(); i++) {
      ImGui::Text("%s", shader->uniforms[i].name.c_str());
      ImGui::Text("Type: %s", GetGLTypeString(shader->uniforms[i].type).c_str());
      // ImGui::Text("Value: %s", shader->uniforms[i].value);
      ImGui::Separator();
    }
    ImGui::End();
  }
}

// MARK: Properties Panel
void ShowShaderPropertiesPanel() {
  if (selectedIndex != -1) {
    auto shader = shaders[selectedIndex].get();
    ImGui::Begin("Shader Properties");
    ImGui::Text("Shader Name: %s", shader->name);

    ImGui::Separator();
    ImGui::Text("Shift + Drag = Faster");
    ImGui::Text("Alt + Drag = Slower");
    ImGui::Separator();

    showShaderError(selectedIndex);

    ImGui::Text("Uniforms");
    for (size_t i = 0; i < shader->uniforms.size(); i++) {
      auto& uniform = shader->uniforms[i];
      ImGui::Text("Name: %s", uniform.name.c_str());
      ImGui::Text("Type: %s", GetGLTypeString(uniform.type).c_str());

      ImGui::PushID(i);

      if (uniform.type == GL_FLOAT) {
        auto& v = std::get<float>(uniform.value);
        if (ImGui::DragFloat(uniform.name.c_str(), &v, 0.1f)) {
        }
      } else if (uniform.type == GL_FLOAT_VEC2) {
        auto& v = std::get<glm::vec2>(uniform.value);
        if (ImGui::DragFloat2(uniform.name.c_str(), glm::value_ptr(v), 0.1f)) {
          uniform.value = v;
        }
      } else if (uniform.type == GL_FLOAT_VEC3) {
        auto& v = std::get<glm::vec3>(uniform.value);
        if (ImGui::DragFloat3(uniform.name.c_str(), glm::value_ptr(v), 0.1f)) {
          uniform.value = v;
        }
        if (ImGui::ColorEdit3(uniform.name.c_str(), glm::value_ptr(v))) {
          uniform.value = v;
        }
      } else if (uniform.type == GL_FLOAT_VEC4) {
        auto& v = std::get<glm::vec4>(uniform.value);
        if (ImGui::DragFloat4(uniform.name.c_str(), glm::value_ptr(v), 0.1f)) {
          uniform.value = v;
        }
        if (ImGui::ColorEdit4(uniform.name.c_str(), glm::value_ptr(v))) {
          uniform.value = v;
        }
      } else if (uniform.type == GL_DOUBLE) {
        auto& v = std::get<double>(uniform.value);
        if (ImGui::InputDouble(uniform.name.c_str(), &v)) {
          uniform.value = v;
        }
      } else if (uniform.type == GL_INT) {
        auto& v = std::get<int>(uniform.value);
        if (ImGui::DragInt(uniform.name.c_str(), &v)) {
          uniform.value = v;
        }
      } else if (uniform.type == GL_INT_VEC2) {
        auto& v = std::get<glm::ivec2>(uniform.value);
        if (ImGui::DragInt2(uniform.name.c_str(), glm::value_ptr(v))) {
          uniform.value = v;
        }
      } else if (uniform.type == GL_INT_VEC3) {
        auto& v = std::get<glm::ivec3>(uniform.value);
        if (ImGui::DragInt3(uniform.name.c_str(), glm::value_ptr(v))) {
          uniform.value = v;
        }
        float vi[3] = {static_cast<float>(v.x) / 255.0f, static_cast<float>(v.y) / 255.0f, static_cast<float>(v.z) / 255.0f};
        if (ImGui::ColorEdit3(uniform.name.c_str(), vi)) {
          v = glm::ivec3(static_cast<int>(vi[0] * 255), static_cast<int>(vi[1] * 255), static_cast<int>(vi[2] * 255));
          uniform.value = v;
        }
      } else if (uniform.type == GL_INT_VEC4) {
        auto& v = std::get<glm::ivec4>(uniform.value);
        if (ImGui::DragInt4(uniform.name.c_str(), glm::value_ptr(v))) {
          uniform.value = v;
        }
        float vi[4] = {static_cast<float>(v.x) / 255.0f, static_cast<float>(v.y) / 255.0f, static_cast<float>(v.z) / 255.0f, static_cast<float>(v.w) / 255.0f};
        if (ImGui::ColorEdit4(uniform.name.c_str(), vi)) {
          v = glm::ivec4(static_cast<int>(vi[0]) * 255, static_cast<int>(vi[1]) * 255, static_cast<int>(vi[2]) * 255, static_cast<int>(vi[3]) * 255);
          uniform.value = v;
        }
      } else if (uniform.type == GL_UNSIGNED_INT) {
        auto& v = std::get<uint>(uniform.value);
        if (ImGui::DragInt(uniform.name.c_str(), (int*)&v)) {
          uniform.value = v;
        }
      } else if (uniform.type == GL_BOOL) {
        auto& v = std::get<bool>(uniform.value);
        if (ImGui::Checkbox(uniform.name.c_str(), &v)) {
          uniform.value = v;
        }
      } else if (uniform.type == GL_BOOL_VEC2) {
        auto& v = std::get<glm::bvec2>(uniform.value);
        if (ImGui::Checkbox(uniform.name.c_str(), &v.x)) {
          uniform.value = v;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox(uniform.name.c_str(), &v.y)) {
          uniform.value = v;
        }
      } else if (uniform.type == GL_BOOL_VEC3) {
        auto& v = std::get<glm::bvec3>(uniform.value);
        if (ImGui::Checkbox(uniform.name.c_str(), &v.x)) {
          uniform.value = v;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox(uniform.name.c_str(), &v.y)) {
          uniform.value = v;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox(uniform.name.c_str(), &v.z)) {
          uniform.value = v;
        }
      } else if (uniform.type == GL_BOOL_VEC4) {
        auto& v = std::get<glm::bvec4>(uniform.value);
        if (ImGui::Checkbox(uniform.name.c_str(), &v.x)) {
          uniform.value = v;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox(uniform.name.c_str(), &v.y)) {
          uniform.value = v;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox(uniform.name.c_str(), &v.z)) {
          uniform.value = v;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox(uniform.name.c_str(), &v.w)) {
          uniform.value = v;
        }
      } else if (uniform.type == GL_SAMPLER_1D) {
        // auto& v = std::get<uint>(uniform.value);
        // if (ImGui::BeginCombo(uniform.name.c_str(), textures2DNames[v])) {
        //   for (int i = 0; i < 2; i++) {
        //     if (ImGui::Selectable(textures2DNames[i], v == i)) {
        //       v = i;
        //     }
        //   }

        //   ImGui::EndCombo();
        // }
      } else if (uniform.type == GL_SAMPLER_2D) {
        auto& v = std::get<GLuint>(uniform.value);
        if (ImGui::BeginCombo(uniform.name.c_str(), textures2DNames[v])) {
          for (int i = 0; i < 2; i++) {
            if (ImGui::Selectable(textures2DNames[i], v == i)) {
              v = i;
            }
          }
          ImGui::EndCombo();
        }
      } else if (uniform.type == GL_SAMPLER_3D) {
        auto& v = std::get<GLuint>(uniform.value);
        if (ImGui::BeginCombo(uniform.name.c_str(), textures3DNames[v])) {
          for (int i = 0; i < 2; i++) {
            if (ImGui::Selectable(textures3DNames[i], v == i)) {
              v = i;
            }
          }
          ImGui::EndCombo();
        }
      }
      ImGui::Separator();
      ImGui::PopID();

      ImGui::End();
    }
  }
}

// MARK: Use Shader
void useSelectedShader() {
  if (selectedIndex == -1) return;
  shaders[selectedIndex]->program.Use();
  // printf("Using shader: %d\n", shaders[selectedIndex]->program.GetProgram());

  // Set Uniforms
  for (int i = 0; i < shaders[selectedIndex]->uniforms.size(); i++) {
    auto& uniform = shaders[selectedIndex]->uniforms[i];
    if (uniform.type == GL_FLOAT) {
      auto& v = std::get<float>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), v);
    } else if (uniform.type == GL_FLOAT_VEC2) {
      auto& v = std::get<glm::vec2>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_VEC3) {
      auto& v = std::get<glm::vec3>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_VEC4) {
      auto& v = std::get<glm::vec4>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_DOUBLE) {
      auto& v = std::get<double>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), v);
    } else if (uniform.type == GL_INT) {
      auto& v = std::get<int>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), v);
    } else if (uniform.type == GL_INT_VEC2) {
      auto& v = std::get<glm::ivec2>(uniform.value);
      // shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_INT_VEC3) {
      auto& v = std::get<glm::ivec3>(uniform.value);
      // shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_INT_VEC4) {
      auto& v = std::get<glm::ivec4>(uniform.value);
      // shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_UNSIGNED_INT) {
      auto& v = std::get<uint>(uniform.value);
      // shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), v);
    } else if (uniform.type == GL_BOOL) {
      auto& v = std::get<bool>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), v);
    } else if (uniform.type == GL_BOOL_VEC2) {
      auto& v = std::get<glm::bvec2>(uniform.value);
      // shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_BOOL_VEC3) {
      auto& v = std::get<glm::bvec3>(uniform.value);
      // shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_BOOL_VEC4) {
      auto& v = std::get<glm::bvec4>(uniform.value);
      // shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT2) {
      auto& v = std::get<glm::mat2>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT3) {
      auto& v = std::get<glm::mat3>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT4) {
      auto& v = std::get<glm::mat4>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT2x3) {
      auto& v = std::get<glm::mat2x3>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT2x4) {
      auto& v = std::get<glm::mat2x4>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT3x2) {
      auto& v = std::get<glm::mat3x2>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT3x4) {
      auto& v = std::get<glm::mat3x4>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT4x2) {
      auto& v = std::get<glm::mat4x2>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT4x3) {
      auto& v = std::get<glm::mat4x3>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT2x3) {
      auto& v = std::get<glm::mat2x3>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT2x4) {
      auto& v = std::get<glm::mat2x4>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT3x4) {
      auto& v = std::get<glm::mat3x4>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT4x2) {
      auto& v = std::get<glm::mat4x2>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT4x3) {
      auto& v = std::get<glm::mat4x3>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT3x2) {
      auto& v = std::get<glm::mat3x2>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT3x4) {
      auto& v = std::get<glm::mat3x4>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_FLOAT_MAT4x2) {
      auto& v = std::get<glm::mat4x2>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), glm::value_ptr(v));
    } else if (uniform.type == GL_SAMPLER_1D) {
      auto& v = std::get<GLuint>(uniform.value);
      // shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), v);
    } else if (uniform.type == GL_SAMPLER_2D) {
      auto& v = std::get<GLuint>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), v);
    } else if (uniform.type == GL_SAMPLER_3D) {
      auto& v = std::get<GLuint>(uniform.value);
      shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), v);
    } else if (uniform.type == GL_SAMPLER_CUBE) {
      // auto& v = std::get<GLuint>(uniform.value);
      // shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), v);
    } else if (uniform.type == GL_SAMPLER_1D_SHADOW) {
      // auto& v = std::get<GLuint>(uniform.value);
      // shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), v);
    } else if (uniform.type == GL_SAMPLER_2D_SHADOW) {
      // auto& v = std::get<GLuint>(uniform.value);
      // shaders[selectedIndex]->program.SetUniformVariable((char*)uniform.name.c_str(), v);
    }
  }
}

// MARK: GetGLTypeString
std::string GetGLTypeString(GLenum type) {
  switch (type) {
    case GL_FLOAT:
      return "float";
    case GL_FLOAT_VEC2:
      return "vec2";
    case GL_FLOAT_VEC3:
      return "vec3";
    case GL_FLOAT_VEC4:
      return "vec4";
    case GL_DOUBLE:
      return "double";
    case GL_INT:
      return "int";
    case GL_INT_VEC2:
      return "ivec2";
    case GL_INT_VEC3:
      return "ivec3";
    case GL_INT_VEC4:
      return "ivec4";
    case GL_UNSIGNED_INT:
      return "unsigned int";
    case GL_BOOL:
      return "bool";
    case GL_BOOL_VEC2:
      return "bvec2";
    case GL_BOOL_VEC3:
      return "bvec3";
    case GL_BOOL_VEC4:
      return "bvec4";
    case GL_FLOAT_MAT2:
      return "mat2";
    case GL_FLOAT_MAT3:
      return "mat3";
    case GL_FLOAT_MAT4:
      return "mat4";
    case GL_FLOAT_MAT2x3:
      return "mat2x3";
    case GL_FLOAT_MAT2x4:
      return "mat2x4";
    case GL_FLOAT_MAT3x2:
      return "mat3x2";
    case GL_FLOAT_MAT3x4:
      return "mat3x4";
    case GL_FLOAT_MAT4x2:
      return "mat4x2";
    case GL_FLOAT_MAT4x3:
      return "mat4x3";
    case GL_SAMPLER_1D:
      return "sampler1D";
    case GL_SAMPLER_2D:
      return "sampler2D";
    case GL_SAMPLER_3D:
      return "sampler3D";
    case GL_SAMPLER_CUBE:
      return "samplerCube";
    case GL_SAMPLER_1D_SHADOW:
      return "sampler1DShadow";
    case GL_SAMPLER_2D_SHADOW:
      return "sampler2DShadow";
    default:
      return "unknown";
  }
}

// MARK: InitializeUniform
void InitializeUniform(Uniform& uniform) {
  switch (uniform.type) {
    case GL_FLOAT:
      if (uniform.name == "uKa")
        uniform.value = (float)0.1f;
      else if (uniform.name == "uKd")
        uniform.value = (float)0.9f;
      else if (uniform.name == "uKs")
        uniform.value = (float)0.9f;
      else if (uniform.name == "uShininess")
        uniform.value = (float)10.0f;
      else
        uniform.value = (float)0.0f;
      break;
    case GL_FLOAT_VEC2:
      uniform.value = glm::vec2();
      break;
    case GL_FLOAT_VEC3:
      if (uniform.name.find("Color") != std::string::npos)
        uniform.value = glm::vec3(static_cast<float>(rand()), static_cast<float>(rand()), static_cast<float>(rand())) / static_cast<float>(RAND_MAX);
      else
        uniform.value = glm::vec3();

      break;
    case GL_FLOAT_VEC4:
      uniform.value = glm::vec4();
      break;
    case GL_DOUBLE:
      uniform.value = (double)0.0;
      break;
    case GL_INT:
      uniform.value = (int)0;
      break;
    case GL_INT_VEC2:
      uniform.value = glm::ivec2();
      break;
    case GL_INT_VEC3:
      uniform.value = glm::ivec3();
      break;
    case GL_INT_VEC4:
      uniform.value = glm::ivec4();
      break;
    case GL_UNSIGNED_INT:
      uniform.value = (uint)0;
      break;
    case GL_BOOL:
      uniform.value = (bool)false;
      break;
    case GL_BOOL_VEC2:
      uniform.value = glm::bvec2();
      break;
    case GL_BOOL_VEC3:
      uniform.value = glm::bvec3();
      break;
    case GL_BOOL_VEC4:
      uniform.value = glm::bvec4();
      break;
    case GL_FLOAT_MAT2:
      uniform.value = glm::mat2();
      break;
    case GL_FLOAT_MAT3:
      uniform.value = glm::mat3();
      break;
    case GL_FLOAT_MAT4:
      uniform.value = glm::mat4();
      break;
    case GL_FLOAT_MAT2x3:
      uniform.value = glm::mat2x3();
      break;
    case GL_FLOAT_MAT2x4:
      uniform.value = glm::mat2x4();
      break;
    case GL_FLOAT_MAT3x2:
      uniform.value = glm::mat3x2();
      break;
    case GL_FLOAT_MAT3x4:
      uniform.value = glm::mat3x4();
      break;
    case GL_FLOAT_MAT4x2:
      uniform.value = glm::mat4x2();
      break;
    case GL_FLOAT_MAT4x3:
      uniform.value = glm::mat4x3();
      break;
    case GL_SAMPLER_1D:
      break;
    case GL_SAMPLER_2D:
      uniform.value = noise2dTexture;
      break;
    case GL_SAMPLER_3D:
      uniform.value = noise3dTexture;
      break;
    case GL_SAMPLER_CUBE:
      break;
    case GL_SAMPLER_1D_SHADOW:
      break;
    case GL_SAMPLER_2D_SHADOW:
      break;
    default:
      printf("Unknown type: %d\n", uniform.type);
      uniform.value = 0;  // Default fallback
      break;
  }
}

// MARK: texture_setup

void InitTextures() {
  printf("Initializing noise textures\n");
  int size = 256;
  const char* noise3d_path = "textures/noise3d.raw";
  const char* noise2d_path = "textures/noise2d.raw";

  // 3D Noise Texture
  float* noise3d = new float[size * size * size];  // Changed to float for OpenGL compatibility

  // Try to load from file first
  bool loaded = false;
  if (FILE* f = fopen(noise3d_path, "rb")) {
    size_t read = fread(noise3d, sizeof(float), size * size * size, f);
    fclose(f);
    loaded = (read == size * size * size);
    printf("Loaded 3D noise from file: %s\n", loaded ? "success" : "failed");
  }

  // Generate if not loaded
  if (!loaded) {
    for (int i = 0; i < size * size * size; i++) {
      noise3d[i] = noise_eval3d(i % size, i / size, i / (size * size));
    }

    // Save to file for next time
    if (FILE* f = fopen(noise3d_path, "wb")) {
      fwrite(noise3d, sizeof(float), size * size * size, f);
      fclose(f);
      printf("Saved 3D noise to file\n");
    }
  }

  glGenTextures(1, &noise3dTexture);
  glBindTexture(GL_TEXTURE_3D, noise3dTexture);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

  // Changed format to single channel float
  glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, size, size, size, 0, GL_RED, GL_FLOAT, noise3d);

  printf("Created 3D noise texture with size %d, and id %d\n", size, noise3dTexture);
  delete[] noise3d;

  // 2D Noise Texture
  float* noise2d = new float[size * size];

  // Try to load from file first
  loaded = false;
  if (FILE* f = fopen(noise2d_path, "rb")) {
    size_t read = fread(noise2d, sizeof(float), size * size, f);
    fclose(f);
    loaded = (read == size * size);
    printf("Loaded 2D noise from file: %s\n", loaded ? "success" : "failed");
  }

  // Generate if not loaded
  if (!loaded) {
    for (int i = 0; i < size * size; i++) {
      noise2d[i] = noise_eval2d(i % size, i / size);
    }

    // Save to file for next time
    if (FILE* f = fopen(noise2d_path, "wb")) {
      fwrite(noise2d, sizeof(float), size * size, f);
      fclose(f);
      printf("Saved 2D noise to file\n");
    }
  }

  glGenTextures(1, &noise2dTexture);
  glBindTexture(GL_TEXTURE_2D, noise2dTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Changed format to single channel float
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size, size, 0, GL_RED, GL_FLOAT, noise2d);

  printf("Created 2D noise texture with size %d, and id %d\n", size, noise2dTexture);
  delete[] noise2d;
}

}  // namespace ShaderEditor
