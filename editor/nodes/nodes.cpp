#include "nodes.h"

#include "../debug.h"
#include "graph.h"
#include "imgui_stdlib.h"

namespace NodeEditor {

#define IN(t, i) (Graph::graph.backLinks.count(inputs[i]->id) == 0 ? std::get<t>(inputs[i]->value) : std::get<t>(Graph::graph.socketIdToSocket[Graph::graph.backLinks[inputs[i]->id]]->value))
#define OUT(i) outputs[i]->value

GroupInputNode::GroupInputNode() : Node("Group Input") {
  addOutput("Scene Time", AttributeType::Float, 0.0f);
  addOutput("Delta Time", AttributeType::Float, 0.0f);
}

void GroupInputNode::display() {
  renderSockets();
  if (!Debug::freezeTime) markDirty();
}

void GroupInputNode::evaluateInternal() {
  OUT(0) = Graph::globals.sceneTime;
  OUT(1) = Graph::globals.deltaTime;
}

GroupOutputNode::GroupOutputNode() : Node("Group Output") {
  addInput("Output", AttributeType::Mesh, std::vector<Vertex>());
}

void GroupOutputNode::display() {
  renderSockets();
}

void GroupOutputNode::evaluateInternal() {}

const std::shared_ptr<Socket> GroupOutputNode::getOutput() {
  return Graph::graph.backLinks.count(inputs[0]->id) == 0 ? inputs[0] : Graph::graph.socketIdToSocket[Graph::graph.backLinks[inputs[0]->id]];
}

ValueNode::ValueNode() : Node("Value") {
  addOutput("Value", AttributeType::Float, 0.0f);
}

void ValueNode::display() {
  renderSockets();
  ImGui::PushItemWidth(NODE_WIDTH);

  float value = std::get<float>(OUT(0));
  if (ImGui::SliderFloat("##value", &value, 0.f, 1.f)) {
    dirty = true;
    OUT(0) = value;
  }
}

void ValueNode::evaluateInternal() {
  // printf("Evaluate ValueNode, Output is: %f\n", std::get<float>(outputs[0].value));
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

#undef IN
#undef OUT

}  // namespace NodeEditor