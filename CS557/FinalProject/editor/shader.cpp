#include "shader.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
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

#include "editor/objects.h"
#include "editor/scene.h"
#include "utils/simplex.h"
#include "utils/texture.h"

namespace ShaderEditor {

const char* SHADERS_FOLDER = "shaders";

// ! Do not remove shaders from list
std::vector<std::shared_ptr<Shader>> shaders;
std::filesystem::file_time_type lastReadTime;

bool viewShaderListPanel = true;
bool viewShaderPropertiesPanel = true;

void compileShaderProgram(std::shared_ptr<Shader>& shader);

const std::array<int, 4> textures2DBindLocations = {0, 3, 4, 5};
const std::array<const char*, 4> textures2DNames = {"noise2d", "cornell_box", "color_bars", "lena"};
const std::array<int, 1> textures3DBindLocations = {1};
const std::array<const char*, 1> textures3DNames = {"noise3d"};

// template <typename TP>
// std::time_t to_time_t(TP tp) {
//   using namespace std::chrono;
//   auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
//   return system_clock::to_time_t(sctp);
// }

// MARK: findShaders
void findShaders() {
  std::unordered_map<std::string, std::vector<std::string>> shaderGroups;

  try {
    for (const auto& entry : std::filesystem::directory_iterator(SHADERS_FOLDER)) {
      if (!entry.is_regular_file()) continue;

      if (entry.path().extension() == ".vert" || entry.path().extension() == ".frag" || entry.path().extension() == ".geom") {
        std::string groupName = entry.path().stem().string();
        shaderGroups[groupName].push_back(entry.path().string());

        // std::time_t tt = to_time_t(lastReadTime);
        // std::tm* gmt = std::gmtime(&tt);
        // std::stringstream buffer;
        // buffer << std::put_time(gmt, "%A, %d %B %Y %H:%M");
        // std::string formattedFileTime = buffer.str();
        // std::cout << formattedFileTime << '\n';

        if (lastReadTime < entry.last_write_time()) {
          lastReadTime = entry.last_write_time();
        }
      }
    }

    for (const auto& entry : shaderGroups) {
      // Get shader from list if it exists
      auto existingShader = std::find_if(shaders.begin(), shaders.end(), [&entry](const std::shared_ptr<Shader>& shader) {
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

        auto shader = std::make_shared<Shader>();
        shader->id = shaders.size();
        strncpy(shader->name, entry.first.c_str(), 256);
        shader->files = entry.second;
        shader->uniformsChanged = true;

        existingShader = shaders.insert(shaders.end(), std::move(shader));
      }

      compileShaderProgram(*existingShader);
    }
  } catch (const std::filesystem::filesystem_error& e) {
    printf("Error: %s\n", e.what());
  }

  shaders.back()->program.Use();
}

// MARK: Compile
void compileShaderProgram(std::shared_ptr<Shader>& shader) {
  printf("Compiling shader: %s\n", shader->name);
  shader->error.clear();
  shader->uniformsChanged = true;

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

    // return;
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

  printf("Found %i uniforms\n", numactiveuniforms);

  for (auto uniform : shader->uniforms) {
    uniform.missing = true;
  }

  for (int i = 0; i < numactiveuniforms; i++) {
    GLint lsize;
    GLenum ltype;
    glGetActiveUniform(shader->program.GetProgram(), i, bufsize, NULL, &lsize, &ltype, lname);

    printf("Found uniform: %s, %i, %i", lname, lsize, ltype);

    if (strncmp(lname, "gl_", 3) == 0) continue;

    auto existingUniform = std::find_if(shader->uniforms.begin(), shader->uniforms.end(), [&lname, &bufsize](const Uniform& uniform) {
      // return uniform.name == lname;
      return strncmp(uniform.name.c_str(), lname, bufsize) == 0;
    });
    if (existingUniform == shader->uniforms.end()) {
      // printf("Found new uniform: %s\n", lname);

      // New uniform
      // shader->uniforms.emplace_back(lsize, ltype, lname);

      // Place in front if starts with "gl_"  or place back if not
      shader->uniforms.emplace_back(lsize, ltype, lname, false);
      InitializeUniform(shader->uniforms.back());
      SetShaderUniform(shader->uniforms.back(), shader->program);
    } else {
      existingUniform->missing = false;
    }
  }
  delete[] lname;
  shader->program.UnUse();
  shader->program.Use();
  shader->uniformsChanged = true;
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

      // printf("Checking %s\n", entry.path().c_str());

      const std::filesystem::file_time_type lastWriteTime = entry.last_write_time();

      // std::time_t tt = to_time_t(lastWriteTime);
      // std::tm* gmt = std::gmtime(&tt);
      // std::stringstream buffer;
      // buffer << std::put_time(gmt, "%A, %d %B %Y %H:%M");
      // std::string formattedFileTime = buffer.str();
      // std::cout << formattedFileTime << '\n';

      // std::time_t tt2 = to_time_t(lastReadTime);
      // std::tm* gmt2 = std::gmtime(&tt2);
      // std::stringstream buffer2;
      // buffer2 << std::put_time(gmt2, "%A, %d %B %Y %H:%M");
      // std::string formattedFileTime2 = buffer2.str();
      // std::cout << formattedFileTime2 << '\n';

      if (lastWriteTime > lastReadTime) {
        printf("Finding\n");

        for (auto& shader : shaders) {
          if (shader->name == entry.path().stem()) {
            compileShaderProgram(shader);
            break;
          }
        }

        lastReadTime = lastWriteTime;
        return;
      }
    }
  } catch (const std::filesystem::filesystem_error& e) {
    // Handle file system errors, e.g., permission issues or directory not found
    std::cerr << "Error checking shader files: " << e.what() << std::endl;
  }
}

// MARK: Show Error
void showShaderError(const std::shared_ptr<Shader>& shader) {
  if (shader->error.empty()) return;

  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImLerp(ImGui::GetStyle().Colors[ImGuiCol_WindowBg], ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 0.15f));
  ImGui::PushID(shader->id);
  ImGui::BeginChild(ImGui::GetID("##shadererror"), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings);
  ImGui::PopStyleColor();
  ImGui::SeparatorText("ERROR IN SHADER COMPILATION");
  ImGui::Text("Shader Name: %s", shader->name);
  ImGui::Separator();
  ImGui::Text("Error: %s", shader->error.c_str());

  ImGui::EndChild();
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

  std::weak_ptr<Objects::Object> selectedObject = Objects::getSelectedObject();

  auto object = !selectedObject.expired() ? selectedObject.lock() : nullptr;
  auto objectShader = object != nullptr ? object->shader.lock() : nullptr;

  {
    if (ImGui::BeginListBox("##draw_list", ImVec2(-1, -1))) {
      for (int i = 0; i < shaders.size(); i++) {
        const bool is_selected = (object && objectShader && objectShader->id == shaders[i]->id);
        if (ImGui::Selectable(shaders[i]->name, is_selected)) {
          if (object) {
            object->shader = shaders[i];
          }
        }

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
  auto selectedObject = Objects::getSelectedObject();
  auto object = selectedObject.lock();
  auto objectShader = object ? object->shader.lock() : nullptr;

  ImGui::Begin("Shader Editor");
  ImGui::Text("Shader Name: %s", objectShader->name);

  ImGui::Separator();

  showShaderError(objectShader);

  ImGui::Text("Uniforms");
  for (int i = 0; i < objectShader->uniforms.size(); i++) {
    ImGui::Text("%s", objectShader->uniforms[i].name.c_str());
    ImGui::Text("Type: %s", GetGLTypeString(objectShader->uniforms[i].type).c_str());
    // ImGui::Text("Value: %s", shader->uniforms[i].value);
    ImGui::Separator();
  }
  ImGui::End();
}

// MARK: Properties Panel
void ShowShaderPropertiesPanel() {
  if (!viewShaderPropertiesPanel) return;

  auto selectedObject = Objects::getSelectedObject();
  auto object = selectedObject.lock();
  auto shader = object ? object->shader.lock() : nullptr;

  ImGui::Begin("Shader Properties");

  if (shader == nullptr) {
    ImGui::Text("No shader selected for this object");
    ImGui::End();
    return;
  }

  ImGui::Text("Shader Name: %s", shader->name);

  ImGui::Separator();
  ImGui::Text("Shift + Drag = Faster");
  ImGui::Text("Alt + Drag = Slower");
  ImGui::Separator();

  showShaderError(shader);

  ImGui::Text("Uniforms");
  for (size_t i = 0; i < shader->uniforms.size(); i++) {
    auto& uniform = shader->uniforms[i];

    if (uniform.missing) {
      ImGui::Text("Missing uniform: %s", uniform.name.c_str());
      // continue;
    }

    bool dirty = shader->uniformsChanged;

    ImGui::Text("Name: %s", uniform.name.c_str());
    ImGui::Text("Type: %s", GetGLTypeString(uniform.type).c_str());

    if (strncmp(uniform.name.c_str(), "gl_", 3) == 0) continue;

    ImGui::PushID(i);

    if (uniform.type == GL_FLOAT) {
      auto& v = std::get<float>(uniform.value);
      if (ImGui::DragFloat(uniform.name.c_str(), &v, 0.1f)) {
        dirty |= true;
      }
    } else if (uniform.type == GL_FLOAT_VEC2) {
      auto& v = std::get<glm::vec2>(uniform.value);
      if (ImGui::DragFloat2(uniform.name.c_str(), glm::value_ptr(v), 0.1f)) {
        dirty |= true;
        uniform.value = v;
      }
    } else if (uniform.type == GL_FLOAT_VEC3) {
      auto& v = std::get<glm::vec3>(uniform.value);
      if (ImGui::DragFloat3(uniform.name.c_str(), glm::value_ptr(v), 0.1f)) {
        dirty |= true;
        uniform.value = v;
      }
      if (ImGui::ColorEdit3(uniform.name.c_str(), glm::value_ptr(v))) {
        uniform.value = v;
      }
    } else if (uniform.type == GL_FLOAT_VEC4) {
      auto& v = std::get<glm::vec4>(uniform.value);
      if (ImGui::DragFloat4(uniform.name.c_str(), glm::value_ptr(v), 0.1f)) {
        dirty |= true;
        uniform.value = v;
      }
      if (ImGui::ColorEdit4(uniform.name.c_str(), glm::value_ptr(v))) {
        dirty |= true;
        uniform.value = v;
      }
    } else if (uniform.type == GL_DOUBLE) {
      auto& v = std::get<double>(uniform.value);
      if (ImGui::InputDouble(uniform.name.c_str(), &v)) {
        dirty |= true;
        uniform.value = v;
      }
    } else if (uniform.type == GL_INT) {
      auto& v = std::get<int>(uniform.value);
      if (ImGui::DragInt(uniform.name.c_str(), &v)) {
        dirty |= true;
        uniform.value = v;
      }
    } else if (uniform.type == GL_INT_VEC2) {
      auto& v = std::get<glm::ivec2>(uniform.value);
      if (ImGui::DragInt2(uniform.name.c_str(), glm::value_ptr(v))) {
        dirty |= true;
        uniform.value = v;
      }
    } else if (uniform.type == GL_INT_VEC3) {
      auto& v = std::get<glm::ivec3>(uniform.value);
      if (ImGui::DragInt3(uniform.name.c_str(), glm::value_ptr(v))) {
        dirty |= true;
        uniform.value = v;
      }
      float vi[3] = {static_cast<float>(v.x) / 255.0f, static_cast<float>(v.y) / 255.0f, static_cast<float>(v.z) / 255.0f};
      if (ImGui::ColorEdit3(uniform.name.c_str(), vi)) {
        dirty |= true;
        v = glm::ivec3(static_cast<int>(vi[0] * 255), static_cast<int>(vi[1] * 255), static_cast<int>(vi[2] * 255));
        uniform.value = v;
      }
    } else if (uniform.type == GL_INT_VEC4) {
      auto& v = std::get<glm::ivec4>(uniform.value);
      if (ImGui::DragInt4(uniform.name.c_str(), glm::value_ptr(v))) {
        dirty |= true;
        uniform.value = v;
      }
      float vi[4] = {static_cast<float>(v.x) / 255.0f, static_cast<float>(v.y) / 255.0f, static_cast<float>(v.z) / 255.0f, static_cast<float>(v.w) / 255.0f};
      if (ImGui::ColorEdit4(uniform.name.c_str(), vi)) {
        dirty |= true;
        v = glm::ivec4(static_cast<int>(vi[0]) * 255, static_cast<int>(vi[1]) * 255, static_cast<int>(vi[2]) * 255, static_cast<int>(vi[3]) * 255);
        uniform.value = v;
      }
    } else if (uniform.type == GL_UNSIGNED_INT) {
      auto& v = std::get<uint>(uniform.value);
      if (ImGui::DragInt(uniform.name.c_str(), (int*)&v)) {
        dirty |= true;
        uniform.value = v;
      }
    } else if (uniform.type == GL_BOOL) {
      auto& v = std::get<bool>(uniform.value);
      if (ImGui::Checkbox(uniform.name.c_str(), &v)) {
        dirty |= true;
        uniform.value = v;
      }
    } else if (uniform.type == GL_BOOL_VEC2) {
      auto& v = std::get<glm::bvec2>(uniform.value);
      if (ImGui::Checkbox(uniform.name.c_str(), &v.x)) {
        dirty |= true;
        uniform.value = v;
      }
      ImGui::SameLine();
      if (ImGui::Checkbox(uniform.name.c_str(), &v.y)) {
        dirty |= true;
        uniform.value = v;
      }
    } else if (uniform.type == GL_BOOL_VEC3) {
      auto& v = std::get<glm::bvec3>(uniform.value);
      if (ImGui::Checkbox(uniform.name.c_str(), &v.x)) {
        dirty |= true;
        uniform.value = v;
      }
      ImGui::SameLine();
      if (ImGui::Checkbox(uniform.name.c_str(), &v.y)) {
        dirty |= true;
        uniform.value = v;
      }
      ImGui::SameLine();
      if (ImGui::Checkbox(uniform.name.c_str(), &v.z)) {
        dirty |= true;
        uniform.value = v;
      }
    } else if (uniform.type == GL_BOOL_VEC4) {
      auto& v = std::get<glm::bvec4>(uniform.value);
      if (ImGui::Checkbox(uniform.name.c_str(), &v.x)) {
        dirty |= true;
        uniform.value = v;
      }
      ImGui::SameLine();
      if (ImGui::Checkbox(uniform.name.c_str(), &v.y)) {
        dirty |= true;
        uniform.value = v;
      }
      ImGui::SameLine();
      if (ImGui::Checkbox(uniform.name.c_str(), &v.z)) {
        dirty |= true;
        uniform.value = v;
      }
      ImGui::SameLine();
      if (ImGui::Checkbox(uniform.name.c_str(), &v.w)) {
        dirty |= true;
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
      if (ImGui::BeginCombo(uniform.name.c_str(), "name")) {
        for (int i = 0; i < textures2DNames.size(); i++) {
          if (ImGui::Selectable(textures2DNames[i], v == textures2DBindLocations[i])) {
            dirty |= true;
            v = textures2DBindLocations[i];
          }
        }
        ImGui::EndCombo();
      }
    } else if (uniform.type == GL_SAMPLER_3D) {
      auto& v = std::get<uint>(uniform.value);
      if (ImGui::BeginCombo(uniform.name.c_str(), textures3DNames[v])) {
        for (int i = 0; i < textures3DNames.size(); i++) {
          if (ImGui::Selectable(textures3DNames[i], v == textures3DBindLocations[i])) {
            dirty |= true;
            v = textures3DBindLocations[i];
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

// MARK: Use Shader
void Shader::use() {
  // printf("Using shader: %d\n", program.GetProgram());

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Textures::noise2dTexture);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_3D, Textures::noise3dTexture);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_CUBE_MAP, Textures::nvidiaCubeTexture);

  program.Use();

  if (uniformsChanged) {
    // Set Uniforms
    for (int i = 0; i < uniforms.size(); i++) {
      auto& uniform = uniforms[i];
      SetShaderUniform(uniform, program);
    }
  }
}

void Shader::unUse() {
  program.UnUse();
}

// void useSelectedShader() {
//   if (selectedIndex == -1) return;
//   shaders[selectedIndex]->use();
// }

// MARK: Register ImGui Settings Handler
void InitShaders() {
  findShaders();
}

std::shared_ptr<Shader> getShader(std::string name) {
  auto index = std::find_if(shaders.begin(), shaders.end(), [&name](const std::shared_ptr<Shader>& shader) {
    return shader->name == name;
  });
  if (index == shaders.end()) return nullptr;
  return *index;
}

}  // namespace ShaderEditor
