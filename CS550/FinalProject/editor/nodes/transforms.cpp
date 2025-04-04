#include "transforms.h"

#include <variant>
#include <vector>

#include "glm/gtx/transform.hpp"
#include "glm/mat4x4.hpp"
#include "glm/matrix.hpp"
#include "graph.h"
#include "imgui_stdlib.h"

namespace NodeEditor {

#define IN(t, i) (Graph::graph.backLinks.count(inputs[i]->id) == 0 ? std::get<t>(inputs[i]->value) : std::get<t>(Graph::graph.socketIdToSocket[Graph::graph.backLinks[inputs[i]->id]]->value))
#define OUT(i) outputs[i]->value

TranslateNode::TranslateNode() : Node("Translate") {
  addInput("Mesh", AttributeType::Mesh, std::vector<Vertex>());
  addInput("Vector", AttributeType::Vector, glm::vec3(0.0f));
  addOutput("Mesh", AttributeType::Mesh, std::vector<Vertex>());
}
void TranslateNode::display() {
  renderSockets();
}
void TranslateNode::evaluateInternal() {
  std::vector<Vertex>& vertices = IN(std::vector<Vertex>, 0);
  glm::vec3& vector = IN(glm::vec3, 1);

  std::vector<Vertex>& newVertices = std::get<std::vector<Vertex>>(OUT(0));
  newVertices.clear();
  newVertices.reserve(vertices.size());

  glm::mat4 matrix = glm::translate(glm::mat4(1.0f), vector);

  for (Vertex vertex : vertices) {
    vertex.position = glm::vec3(matrix * glm::vec4(vertex.position, 1.0f));
    newVertices.push_back(vertex);
  }
}

RotateNode::RotateNode() : Node("Rotate") {
  addInput("Mesh", AttributeType::Mesh, std::vector<Vertex>());
  addInput("Angles", AttributeType::Vector, glm::vec3(0.0f));
  addOutput("Mesh", AttributeType::Mesh, std::vector<Vertex>());
}
void RotateNode::display() {
  renderSockets();
}
void RotateNode::evaluateInternal() {
  std::vector<Vertex>& vertices = IN(std::vector<Vertex>, 0);
  glm::vec3& angles = IN(glm::vec3, 1);

  std::vector<Vertex>& newVertices = std::get<std::vector<Vertex>>(OUT(0));
  newVertices.clear();
  newVertices.reserve(vertices.size());

  glm::mat4 matrix = glm::rotate(glm::mat4(1.0f), angles.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), angles.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), angles.z, glm::vec3(0.0f, 0.0f, 1.0f));

  for (Vertex vertex : vertices) {
    vertex.position = glm::vec3(matrix * glm::vec4(vertex.position, 1.0f));
    newVertices.push_back(vertex);
  }
}

ScaleNode::ScaleNode() : Node("Scale") {
  addInput("Mesh", AttributeType::Mesh, std::vector<Vertex>());
  addInput("Scales", AttributeType::Vector, glm::vec3(0.0f));
  addOutput("Mesh", AttributeType::Mesh, std::vector<Vertex>());
}
void ScaleNode::display() {
  renderSockets();
}
void ScaleNode::evaluateInternal() {
  std::vector<Vertex>& vertices = IN(std::vector<Vertex>, 0);
  glm::vec3& vector = IN(glm::vec3, 1);

  std::vector<Vertex>& newVertices = std::get<std::vector<Vertex>>(OUT(0));
  newVertices.clear();
  newVertices.reserve(vertices.size());

  glm::mat4 matrix = glm::scale(glm::mat4(1.0f), vector);

  for (Vertex vertex : vertices) {
    vertex.position = glm::vec3(matrix * glm::vec4(vertex.position, 1.0f));
    newVertices.push_back(vertex);
  }
}

#undef IN
#undef OUT

}  // namespace NodeEditor