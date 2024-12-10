#include "math.h"

#include <cmath>
#include <variant>
#include <vector>

#include "graph.h"
#include "imgui_stdlib.h"

namespace NodeEditor {

#define IN(t, i) (Graph::graph.backLinks.count(inputs[i]->id) == 0 ? std::get<t>(inputs[i]->value) : std::get<t>(Graph::graph.socketIdToSocket[Graph::graph.backLinks[inputs[i]->id]]->value))
#define OUT(i) outputs[i]->value

AddNode::AddNode() : Node("Add") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
  addInput("B", AttributeType::Float, 0.0f);
}
void AddNode::display() {
  renderSockets();
}
void AddNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);
  float& b = IN(float, 1);

  value = a + b;
}

SubtractNode::SubtractNode() : Node("Subtract") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
  addInput("B", AttributeType::Float, 0.0f);
}
void SubtractNode::display() {
  renderSockets();
}
void SubtractNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);
  float& b = IN(float, 1);

  value = a - b;
}

MultiplyNode::MultiplyNode() : Node("Multiply") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
  addInput("B", AttributeType::Float, 0.0f);
}
void MultiplyNode::display() {
  renderSockets();
}
void MultiplyNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);
  float& b = IN(float, 1);

  value = a * b;
}

DivideNode::DivideNode() : Node("Divide") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
  addInput("B", AttributeType::Float, 0.0f);
}
void DivideNode::display() {
  renderSockets();
}
void DivideNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);
  float& b = IN(float, 1);

  value = a / b;
}

PowerNode::PowerNode() : Node("Power") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
  addInput("B", AttributeType::Float, 0.0f);
}
void PowerNode::display() {
  renderSockets();
}
void PowerNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);
  float& b = IN(float, 1);

  value = pow(a, b);
}

ModuloNode::ModuloNode() : Node("Modulo") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
  addInput("B", AttributeType::Float, 0.0f);
}
void ModuloNode::display() {
  renderSockets();
}
void ModuloNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);
  float& b = IN(float, 1);

  value = fmod(a, b);
}

AbsoluteNode::AbsoluteNode() : Node("Absolute") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void AbsoluteNode::display() {
  renderSockets();
}
void AbsoluteNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = abs(a);
}

FloorNode::FloorNode() : Node("Floor") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void FloorNode::display() {
  renderSockets();
}
void FloorNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = floorf(a);
}

CeilNode::CeilNode() : Node("Ceil") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void CeilNode::display() {
  renderSockets();
}
void CeilNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = ceilf(a);
}

RoundNode::RoundNode() : Node("Round") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void RoundNode::display() {
  renderSockets();
}
void RoundNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = roundf(a);
}

SqrtNode::SqrtNode() : Node("Sqrt") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void SqrtNode::display() {
  renderSockets();
}
void SqrtNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = sqrtf(a);
}

SinNode::SinNode() : Node("Sin") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void SinNode::display() {
  renderSockets();
}
void SinNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = sinf(a);
}

CosNode::CosNode() : Node("Cos") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void CosNode::display() {
  renderSockets();
}
void CosNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = cosf(a);
}

TanNode::TanNode() : Node("Tan") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void TanNode::display() {
  renderSockets();
}
void TanNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = tanf(a);
}

ArcsinNode::ArcsinNode() : Node("Arcsin") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void ArcsinNode::display() {
  renderSockets();
}
void ArcsinNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = asinf(a);
}

ArccosNode::ArccosNode() : Node("Arccos") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void ArccosNode::display() {
  renderSockets();
}
void ArccosNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = acosf(a);
}

ArctanNode::ArctanNode() : Node("Arctan") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void ArctanNode::display() {
  renderSockets();
}
void ArctanNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = atanf(a);
}

Arctan2Node::Arctan2Node() : Node("Arctan2") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
  addInput("B", AttributeType::Float, 0.0f);
}
void Arctan2Node::display() {
  renderSockets();
}
void Arctan2Node::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);
  float& b = IN(float, 1);

  value = atan2f(a, b);
}

LogNode::LogNode() : Node("Log") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void LogNode::display() {
  renderSockets();
}
void LogNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = logf(a);
}

ExpNode::ExpNode() : Node("Exp") {
  addOutput("Value", AttributeType::Float, 0.0f);
  addInput("A", AttributeType::Float, 0.0f);
}
void ExpNode::display() {
  renderSockets();
}
void ExpNode::evaluateInternal() {
  float& value = std::get<float>(OUT(0));
  float& a = IN(float, 0);

  value = expf(a);
}

#undef IN
#undef OUT

}  // namespace NodeEditor