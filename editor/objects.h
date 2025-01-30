#pragma once

#include "editor/shader.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "utils/arraymath.h"
#include "utils/colors.h"
#include "utils/glslprogram.h"
#include "utils/keytime.h"
#include "utils/simplex.h"

namespace Objects {

extern bool viewObjectsPanel;

enum class ObjectType { Cube,
                        Sphere,
                        Cylinder,
                        Cone,
                        Torus,
                        Plane,
                        NUM_OBJECT_TYPES };

struct Object {
  std::string name;
  glm::mat4 transform;
  bool hidden = false;
  ObjectType type;
  std::weak_ptr<ShaderEditor::Shader> shader;

  Object(std::string name, glm::vec3 position, ObjectType type)
      : name(name), transform(glm::translate(glm::mat4(1.f), position)), type(type) {}

  Object(std::string name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, ObjectType type)
      : name(name), transform(glm::translate(glm::mat4(1.f), position) * glm::rotate(glm::mat4(1.f), rotation.x, glm::vec3(1.f, 0.f, 0.f)) * glm::rotate(glm::mat4(1.f), rotation.y, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(glm::mat4(1.f), rotation.z, glm::vec3(0.f, 0.f, 1.f)) * glm::scale(glm::mat4(1.f), scale)), type(type) {}

  Object(std::string name, glm::mat4 transform, ObjectType type)
      : name(name), transform(transform), type(type) {}

  Object(std::string name, ObjectType type)
      : name(name), transform(glm::mat4(1.f)), type(type) {}

  void Display();
};

void ShowObjectsPanel();
void TransformStart(float* cameraView, float* cameraProjection, float* matrix);
void TransformEnd();
void EditTransform(float* cameraView, float* cameraProjection, float* matrix);
void DisplayGizmo(float* cameraView, float* cameraProjection);
void DisplayObjects();
void InitLists();
std::weak_ptr<Object> getSelectedObject();

}  // namespace Objects