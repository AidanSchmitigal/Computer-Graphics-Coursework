#include "graph.h"

#include <iostream>

#include "graph.h"
#include "imnodes/imnodes.h"
#include "nodes.h"
#include "pairings.h"

namespace NodeEditor {

Graph Graph::graph = Graph();
Globals Graph::globals = {0.f, 0.f};

Graph::Graph() {
  outputNode = std::make_shared<GroupOutputNode>();
  addNode(outputNode);
}

uint Graph::newId() {
  return currentId++;
}

void Graph::addNode(std::shared_ptr<Node> node) {
  nodes.push_back(node);
  nodeIdToNode[node->id] = node;
  evaluationOrder.clear();
}

void Graph::removeNode(uint nodeId) {
  if (nodeId == outputNode->id) return;

  auto node = nodeIdToNode.at(nodeId);

  for (auto [toSocketId, fromSocketId] : backLinks) {
    if (std::get<0>(reverse_pairing3(fromSocketId)) == node->id || std::get<0>(reverse_pairing3(toSocketId)) == node->id) {
      disconnect(pairing(fromSocketId, toSocketId));
    }
  }

  nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
  nodeIdToNode.erase(node->id);
  evaluationOrder.clear();
}

void Graph::addSocket(std::shared_ptr<Socket> socket) {
  socketIdToSocket[socket->id] = socket;
}

void Graph::connect(uint fromSocketId, uint toSocketId) {
  if (socketIdToSocket[fromSocketId]->type != socketIdToSocket[toSocketId]->type) return;

  if (backLinks.count(toSocketId)) {
    disconnect(pairing(fromSocketId, toSocketId));
  }

  backLinks[toSocketId] = fromSocketId;
  forwardLinks[fromSocketId].insert(toSocketId);

  evaluationOrder.clear();
  auto [nodeId, socketId, io] = reverse_pairing3(fromSocketId);
  propagateDirty(nodeId);
}

void Graph::disconnect(uint linkId) {
  auto [fromSocketId, toSocketId] = reverse_pairing(linkId);

  backLinks.erase(toSocketId);
  forwardLinks[fromSocketId].erase(toSocketId);
  for (auto& [fromId, ids] : forwardLinks) {
    ids.erase(toSocketId);
  }

  auto [nodeId, socketId, io] = reverse_pairing3(fromSocketId);
  propagateDirty(nodeId);

  evaluationOrder.clear();
}

std::shared_ptr<Node> Graph::getNode(uint nodeId) {
  return nodeIdToNode.at(nodeId);
}

void Graph::propagateDirty(uint nodeId) {
  std::shared_ptr<Node> node = getNode(nodeId);
  node->dirty = true;
  for (uint i = 0; i < node->outputCount; i++) {
    if (forwardLinks.count(node->outputs[i]->id) == 0) continue;

    auto inputSockets = forwardLinks[node->outputs[i]->id];
    for (uint inputSocketId : inputSockets) {
      auto [nodeId, SocketIndex, io] = reverse_pairing3(inputSocketId);
      propagateDirty(nodeId);
    }
  }
}

void Graph::evaluate() {
  if (nodes.size() == 0) return;

  if (evaluationOrder.size() == 0) {
    printf("Recalculating graph\n");
    std::unordered_set<uint> visited;
    // std::vector<Node*> evaluationOrder;

    std::function<bool(std::shared_ptr<Node>)> dfs = [&](std::shared_ptr<Node> node) -> bool {
      if (visited.count(node->id)) return false;
      visited.insert(node->id);

      for (int i = 0; i < node->inputCount; i++) {
        uint inputId = pairing3(node->id, i, 0);
        if (backLinks.count(inputId) == 0) continue;

        uint dependentOutputSocketId = backLinks[inputId];
        // NodeId, SocketIndex, Input/Output
        auto [dependentNodeId, dependentSocketIndex, io] = reverse_pairing3(dependentOutputSocketId);
        auto dependentNode = getNode(dependentNodeId);
        bool priorIsDirty = dfs(dependentNode);
        node->dirty = node->dirty || priorIsDirty;
      }

      evaluationOrder.push_back(node);
      return node->dirty;
    };

    for (auto& node : nodes) {
      dfs(node);
    }
  }

  for (auto& node : evaluationOrder) {
    node->evaluate();
  }
}

void Graph::render() {
  for (auto& node : nodes) {
    ImNodes::BeginNode(node->id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("%u", node->id);
    ImGui::SameLine();
    ImGui::TextUnformatted(node->name.c_str());

    if (node->dirty) {
      ImGui::SameLine();
      ImGui::TextUnformatted("*");
    }
    ImNodes::EndNodeTitleBar();

    node->display();

    // ImGui::PushItemWidth(NODE_WIDTH);
    // ImGui::Separator();
    ImGui::Text("%f ms", node->lastEvaluationTime);
    ImNodes::EndNode();

    if (node->dirty) node->markDirty();
  }

  for (auto const& [inputSocketId, outputSocketId] : backLinks) {
    ImNodes::Link(pairing(outputSocketId, inputSocketId), outputSocketId, inputSocketId);
  }
}

Node::~Node() {}
Node::Node(const std::string& name) : id(Graph::graph.newId()), displayWidth(NODE_WIDTH), name(name), dirty(true), lastEvaluationTime(0.0) {}

void Node::addInput(const std::string& name, AttributeType type, const AttributeValue& defaultValue) {
  uint socketId = pairing3(id, inputCount, 0);
  auto socket = std::make_shared<Socket>(name, type, socketId, defaultValue);
  inputs[inputCount] = socket;
  Graph::graph.addSocket(socket);
  inputCount++;
}

void Node::addOutput(const std::string& name, AttributeType type, const AttributeValue& defaultValue) {
  uint socketId = pairing3(id, outputCount, 1);
  auto socket = std::make_shared<Socket>(name, type, socketId, defaultValue);
  outputs[outputCount] = socket;
  Graph::graph.addSocket(socket);
  outputCount++;
}

void Node::markDirty() {
  dirty = true;
  Graph::graph.propagateDirty(id);
}

void Node::evaluate() {
  if (!dirty) return;

  auto startTime = std::chrono::high_resolution_clock::now();

  evaluateInternal();

  auto endTime = std::chrono::high_resolution_clock::now();
  lastEvaluationTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();

  dirty = false;
}

void Node::renderSockets() {
  // Render Outputs
  for (size_t i = 0; i < outputCount; ++i) {
    const auto& output = outputs[i];
    if (output->id == -1) continue;

    ImNodes::BeginOutputAttribute(output->id);
    ImGui::Text("%s", output->name.c_str());
    ImNodes::EndOutputAttribute();
  }

  // Render Inputs
  for (size_t i = 0; i < inputCount; ++i) {
    const auto& input = inputs[i];
    if (input->id == -1) continue;

    uint inputSocketId = input->id;
    ImNodes::BeginInputAttribute(inputSocketId);
    ImGui::Text("%s", input->name.c_str());

    // Render input widgets based on type
    ImGui::SameLine();
    const float label_width = ImGui::CalcTextSize(input->name.c_str()).x;
    ImGui::PushItemWidth(NODE_WIDTH - label_width);

    float dragSpeed = 0.01f * ImGui::IsKeyDown(ImGuiMod_Shift) ? 10.f : 1.f;

    if (Graph::graph.backLinks.count(inputSocketId) == 0) {
      switch (input->type) {
        case AttributeType::Float: {
          float& value = std::get<float>(input->value);

          ImGui::PushItemWidth(NODE_WIDTH);
          if (ImGui::DragFloat("##value1", &value, dragSpeed)) {
            dirty = true;
          }

          break;
        }
        case AttributeType::Int: {
          ImGui::Dummy(ImVec2(0, 0));

          ImGui::PushItemWidth(NODE_WIDTH);

          int value = std::get<int>(input->value);
          if (ImGui::InputInt("##value1", &value)) {
            dirty = true;
            input->value = value;
          }
          break;
        }
        case AttributeType::String: {
          std::string& value = std::get<std::string>(input->value);
          char buffer[256];  // A buffer for editing
          std::snprintf(buffer, sizeof(buffer), "%s", value.c_str());
          if (ImGui::InputText("##value1", buffer, sizeof(buffer))) {
            value = buffer;
            dirty = true;
          }
          break;
        }
        case AttributeType::Vector: {
          ImGui::Dummy(ImVec2(0, 0));

          glm::vec3& vec = std::get<glm::vec3>(input->value);
          float arr[3] = {vec.x, vec.y, vec.z};

          ImGui::PushItemWidth(NODE_WIDTH);
          if (ImGui::DragFloat("##value1", arr, dragSpeed)) {
            vec.x = arr[0];
            dirty = true;
          }
          ImGui::PushItemWidth(NODE_WIDTH);
          if (ImGui::DragFloat("##value2", arr + 1, dragSpeed)) {
            vec.y = arr[1];
            dirty = true;
          }
          ImGui::PushItemWidth(NODE_WIDTH);
          if (ImGui::DragFloat("##value3", arr + 2, dragSpeed)) {
            vec.z = arr[2];
            dirty = true;
          }

          break;
        }
        case AttributeType::Color: {
          auto& color = std::get<glm::vec3>(input->value);
          dirty |= ImGui::ColorEdit3("##value", &color.x);
          break;
        }
        case AttributeType::Mesh: {
          break;
        }
        default:
          ImGui::Text("Unsupported typs");
          break;
      }
    }
    ImNodes::EndInputAttribute();
  }
}

}  // namespace NodeEditor
