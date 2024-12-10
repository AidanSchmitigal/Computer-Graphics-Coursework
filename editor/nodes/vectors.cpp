
#include "vectors.h"

#include <variant>
#include <vector>

#include "graph.h"
#include "imgui_stdlib.h"

namespace NodeEditor {

#define IN(t, i) (Graph::graph.backLinks.count(inputs[i]->id) == 0 ? std::get<t>(inputs[i]->value) : std::get<t>(Graph::graph.socketIdToSocket[Graph::graph.backLinks[inputs[i]->id]]->value))
#define OUT(i) outputs[i]->value

CombineXYZNode::CombineXYZNode() : Node("Combine XYZ") {
  addInput("X", AttributeType::Float, 0.0f);
  addInput("Y", AttributeType::Float, 0.0f);
  addInput("Z", AttributeType::Float, 0.0f);
  addOutput("Vector", AttributeType::Vector, glm::vec3(0.0f));
}
void CombineXYZNode::display() {
  renderSockets();
}
void CombineXYZNode::evaluateInternal() {
  float& x = IN(float, 0);
  float& y = IN(float, 1);
  float& z = IN(float, 2);

  OUT(0) = glm::vec3(x, y, z);
}

SeparateXYZNode::SeparateXYZNode() : Node("Separate XYZ") {
  addInput("Vector", AttributeType::Vector, glm::vec3(0.0f));
  addOutput("X", AttributeType::Float, 0.0f);
  addOutput("Y", AttributeType::Float, 0.0f);
  addOutput("Z", AttributeType::Float, 0.0f);
}
void SeparateXYZNode::display() {
  renderSockets();
}
void SeparateXYZNode::evaluateInternal() {
  glm::vec3& v = IN(glm::vec3, 0);

  OUT(0) = v.x;
  OUT(1) = v.y;
  OUT(2) = v.z;
}

VectorAddNode::VectorAddNode() : Node("Vector Add") {
  addInput("A", AttributeType::Vector, glm::vec3(0.0f));
  addInput("B", AttributeType::Vector, glm::vec3(0.0f));
  addOutput("Vector", AttributeType::Vector, glm::vec3(0.0f));
}
void VectorAddNode::display() {
  renderSockets();
}
void VectorAddNode::evaluateInternal() {
  glm::vec3& a = IN(glm::vec3, 0);
  glm::vec3& b = IN(glm::vec3, 1);

  OUT(0) = a + b;
}

VectorSubtractNode::VectorSubtractNode() : Node("Vector Subtract") {
  addInput("A", AttributeType::Vector, glm::vec3(0.0f));
  addInput("B", AttributeType::Vector, glm::vec3(0.0f));
  addOutput("Vector", AttributeType::Vector, glm::vec3(0.0f));
}
void VectorSubtractNode::display() {
  renderSockets();
}
void VectorSubtractNode::evaluateInternal() {
  glm::vec3& a = IN(glm::vec3, 0);
  glm::vec3& b = IN(glm::vec3, 1);

  OUT(0) = a - b;
}

VectorMultiplyNode::VectorMultiplyNode() : Node("Vector Multiply") {
  addInput("A", AttributeType::Vector, glm::vec3(0.0f));
  addInput("B", AttributeType::Vector, glm::vec3(0.0f));
  addOutput("Vector", AttributeType::Vector, glm::vec3(0.0f));
}
void VectorMultiplyNode::display() {
  renderSockets();
}
void VectorMultiplyNode::evaluateInternal() {
  glm::vec3& a = IN(glm::vec3, 0);
  glm::vec3& b = IN(glm::vec3, 1);

  OUT(0) = a * b;
}

VectorDivideNode::VectorDivideNode() : Node("Vector Divide") {
  addInput("A", AttributeType::Vector, glm::vec3(0.0f));
  addInput("B", AttributeType::Vector, glm::vec3(0.0f));
  addOutput("Vector", AttributeType::Vector, glm::vec3(0.0f));
}
void VectorDivideNode::display() {
  renderSockets();
}
void VectorDivideNode::evaluateInternal() {
  glm::vec3& a = IN(glm::vec3, 0);
  glm::vec3& b = IN(glm::vec3, 1);

  OUT(0) = a / b;
}

VectorLengthNode::VectorLengthNode() : Node("Vector Length") {
  addInput("Vector", AttributeType::Vector, glm::vec3(0.0f));
  addOutput("Length", AttributeType::Float, 0.0f);
}
void VectorLengthNode::display() {
  renderSockets();
}
void VectorLengthNode::evaluateInternal() {
  glm::vec3& v = IN(glm::vec3, 0);

  OUT(0) = v.length();
}

VectorNormalizeNode::VectorNormalizeNode() : Node("Vector Normalize") {
  addInput("Vector", AttributeType::Vector, glm::vec3(0.0f));
  addOutput("Vector", AttributeType::Vector, glm::vec3(0.0f));
}
void VectorNormalizeNode::display() {
  renderSockets();
}
void VectorNormalizeNode::evaluateInternal() {
  glm::vec3& v = IN(glm::vec3, 0);

  OUT(0) = v / glm::vec3(v.length());
}

#undef IN
#undef OUT

}  // namespace NodeEditor