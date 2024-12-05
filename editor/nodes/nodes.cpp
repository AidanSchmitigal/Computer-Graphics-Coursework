#include "nodes.h"

#include "graph.h"
#include "imgui_stdlib.h"

namespace NodeEditor {

#define IN(t, i) (Graph::graph.backLinks.count(inputs[i].id) == 0 ? std::get<t>(inputs[i].value) : std::get<t>(Graph::graph.socketIdToSocket[Graph::graph.backLinks[inputs[i].id]]->value))
#define OUT(i) outputs[i].value

GroupInputNode::GroupInputNode() : Node("Group Input") {
  addOutput("Group", AttributeType::Int, 0);
}

void GroupInputNode::display() {
  renderSockets();
  ImGui::Text("Group Input");
}

void GroupInputNode::evaluateInternal() {
  // printf("Evaluate GroupInputNode, Output is: %d\n", std::get<int>(outputs[0].value));
}

GroupOutputNode::GroupOutputNode() : Node("Group Output") {
  addInput("Group", AttributeType::Int, 0);
}

void GroupOutputNode::display() {
  renderSockets();
  ImGui::Text("Group Output");
}

void GroupOutputNode::evaluateInternal() {
  // printf("Evaluate GroupOutputNode, Output is: %d\n", std::get<int>(inputs[0].value));
}

ValueNode::ValueNode() : Node("Value") {
  addOutput("Value", AttributeType::Float, 0.0f);
}

void ValueNode::display() {
  renderSockets();
  ImGui::PushItemWidth(NODE_WIDTH);

  float value = std::get<float>(outputs[0].value);
  if (ImGui::SliderFloat("##value", &value, 0.f, 1.f)) {
    dirty = true;
    outputs[0].value = value;
  }
}

void ValueNode::evaluateInternal() {
  // printf("Evaluate ValueNode, Output is: %f\n", std::get<float>(outputs[0].value));
}

AddNode::AddNode() : Node("Add") {
  addInput("A", AttributeType::Float, 0.0f);
  addInput("B", AttributeType::Float, 0.0f);
  addOutput("Result", AttributeType::Float, 0.0f);
}

void AddNode::display() {
  renderSockets();
}

void AddNode::evaluateInternal() {
  float a = IN(float, 0);
  float b = IN(float, 1);
  OUT(0) = a + b;
}

PrintNode::PrintNode() : Node("Print") {
  addInput("Input", AttributeType::Float, 0.0f);
}

void PrintNode::display() {
  ImGui::InputText("Message", &message);
  renderSockets();
  ImGui::Text("Printing: %f", IN(float, 0));
}

void PrintNode::evaluateInternal() {
  printf("Node %d: %f\n", id, IN(float, 0));
}

PrimitveCubeNode::PrimitveCubeNode() : Node("Primitive Cube") {
  addInput("Size", AttributeType::Vector);
  addInput("Subdivisions", AttributeType::Int, 1);
  addOutput("Vertex Mesh", AttributeType::TriangleMesh);
}

void PrimitveCubeNode::display() {
  renderSockets();
  ImGui::Text("Primitive Cube");
}

void PrimitveCubeNode::evaluateInternal() {
}

}  // namespace NodeEditor