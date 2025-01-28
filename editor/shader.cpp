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
#include "utils/texture.h"

namespace ShaderEditor {

const char* SHADERS_FOLDER = "shaders";

std::vector<std::unique_ptr<Shader>> shaders;
int selectedIndex = -1;
std::filesystem::file_time_type lastReadTime;

bool viewShaderListPanel = true;
bool viewShaderPropertiesPanel = true;

void compileShaderProgram(std::unique_ptr<Shader>& shader);

GLuint noise3dTexture;
GLuint noise2dTexture;

// const std::array<GLuint*, 1> textures2D = {&noise2dTexture};
const std::array<const char*, 1> textures2DNames = {"noise2d"};
// const std::array<GLuint*, 1> textures3D = {&noise3dTexture};
const std::array<const char*, 1> textures3DNames = {"noise3d"};

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
      // printf("Found new uniform: %s\n", lname);

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
  return;
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
        auto& v = std::get<uint>(uniform.value);
        if (ImGui::BeginCombo(uniform.name.c_str(), textures2DNames[v])) {
          for (int i = 0; i < textures2DNames.size(); i++) {
            if (ImGui::Selectable(textures2DNames[i], v == i)) {
              v = i;
            }
          }
          ImGui::EndCombo();
        }
      } else if (uniform.type == GL_SAMPLER_3D) {
        auto& v = std::get<uint>(uniform.value);
        if (ImGui::BeginCombo(uniform.name.c_str(), textures3DNames[v])) {
          for (int i = 0; i < textures3DNames.size(); i++) {
            if (ImGui::Selectable(textures3DNames[i], v == i)) {
              v = i;
            }
          }
          ImGui::EndCombo();
        }
      }

      ImGui::Separator();
      ImGui::PopID();
    }

    ImGui::End();
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
    SetShaderUniform(uniform, shaders[selectedIndex]->program);
  }
}

// MARK: Register ImGui Settings Handler
void RegisterImGuiSettingsHandler() {
  // TODO: Remove
}

// MARK: texture_setup
void InitTextures() {
  printf("Initializing noise textures\n");
  int size = 256;
  const char* noise3d_path = "textures/noise3d.064.raw";
  const char* noise2d_path = "textures/noise2d.064.raw";

  bool loaded = loadTexture3D(noise3d_path, GL_TEXTURE0, &noise3dTexture, &size, &size, &size);

  if (!loaded) {
    fprintf(stderr, "Could not load 3D noise texture from file\n");
    return;
  }

  printf("Created 3D noise texture with size %d, and id %d\n", size, noise3dTexture);

  loaded = loadTexture2D(noise2d_path, GL_TEXTURE1, &noise2dTexture, &size, &size);

  if (!loaded) {
    fprintf(stderr, "Could not load 2D noise texture from file\n");
    return;
  }

  printf("Created 2D noise texture with size %d, and id %d\n", size, noise2dTexture);
}

}  // namespace ShaderEditor
