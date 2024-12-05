#include "graph.h"

#include <iostream>

#include "graph.h"
#include "pairings.h"

namespace NodeEditor {

Graph Graph::graph;

Graph::Graph() {}

Graph::~Graph() {
  for (Node* node : nodes) {
    delete node;
  }
}

int Graph::newId() {
  return currentId++;
}

void Graph::addNode(Node* node) {
  nodes.push_back(node);
  nodeIdToNode[node->id] = node;
  evaluationOrder.clear();
}

void Graph::removeNode(Node* node) {
  // nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
  // nodeIdToNode.erase(node->id);
  // delete node;
  // TODO: implement
  evaluationOrder.clear();
}

void Graph::addSocket(Socket* socket) {
  socketIdToSocket[socket->id] = socket;
}

void Graph::connect(int fromSocketId, int toSocketId) {
  if (backLinks.count(toSocketId)) return;
  // links.push_back({fromSocketId, toSocketId});
  backLinks[toSocketId] = fromSocketId;
  forwardLinks[fromSocketId].insert(toSocketId);

  evaluationOrder.clear();
  auto [nodeId, socketId, io] = reverse_pairing3(fromSocketId);
  propagateDirty(nodeId);
}

void Graph::disconnect(int fromSocketId, int toSocketId) {
  // auto [nodeId, SocketId, io] = reverse_pairing3(fromSocketId);
  // TODO: implement
  evaluationOrder.clear();
}

Node* Graph::getNode(int nodeId) {
  return nodeIdToNode.at(nodeId);
}

void Graph::propagateDirty(int nodeId) {
  Node* node = getNode(nodeId);
  node->dirty = true;
  for (int i = 0; i < node->outputs.size(); i++) {
    if (forwardLinks.count(node->outputs[i].id) == 0) continue;

    auto inputSockets = forwardLinks[node->outputs[i].id];
    for (int inputSocketId : inputSockets) {
      auto [nodeId, SocketIndex, io] = reverse_pairing3(inputSocketId);
      propagateDirty(nodeId);
    }
  }
}

void Graph::evaluate() {
  if (nodes.size() == 0) return;

  if (evaluationOrder.size() == 0) {
    printf("Recalculating graph\n");
    std::unordered_set<int> visited;
    // std::vector<Node*> evaluationOrder;

    std::function<bool(Node*)> dfs = [&](Node* node) -> bool {
      if (visited.count(node->id)) return false;
      visited.insert(node->id);

      for (int i = 0; i < node->inputs.size(); i++) {
        int inputId = pairing3(node->id, i, 0);
        if (backLinks.count(inputId) == 0) continue;

        int dependentOutputSocketId = backLinks[inputId];
        // NodeId, SocketIndex, Input/Output
        auto [dependentNodeId, dependentSocketIndex, io] = reverse_pairing3(dependentOutputSocketId);
        Node* dependentNode = nodes[dependentNodeId];
        bool priorIsDirty = dfs(dependentNode);
        node->dirty = node->dirty || priorIsDirty;
      }

      evaluationOrder.push_back(node);
      return node->dirty;
    };

    for (Node* node : nodes) {
      dfs(node);
    }
  }

  for (Node* node : evaluationOrder) {
    node->evaluate();
  }
}

void Graph::render() {
  for (Node* node : nodes) {
    ImNodes::BeginNode(node->id);
    ImNodes::BeginNodeTitleBar();
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

  int i = 0;
  for (auto const& [inputSocketId, outputSocketId] : backLinks) {
    ImNodes::Link(i, outputSocketId, inputSocketId);
    i++;
  }
}

Node::~Node() {}
Node::Node(std::string name) : id(Graph::graph.newId()), name(name) {
  Graph::graph.addNode(this);
  // inputs.fill(0);
  // outputs.fill(0);
}

void Node::addInput(const std::string& name, AttributeType type, const AttributeValue& defaultValue) {
  int socketId = pairing3(id, inputCount, 0);
  inputs[inputCount] = Socket{name, type, socketId, defaultValue};
  Graph::graph.addSocket(&inputs[inputCount]);
  inputCount++;
}

void Node::addOutput(const std::string& name, AttributeType type, const AttributeValue& defaultValue) {
  int socketId = pairing3(id, outputCount, 1);
  outputs[outputCount] = Socket{name, type, socketId, defaultValue};
  Graph::graph.addSocket(&outputs[outputCount]);
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
    if (outputs[i].id == -1) continue;
    ImNodes::BeginOutputAttribute(outputs[i].id);
    ImGui::Text("%s", outputs[i].name.c_str());
    ImNodes::EndOutputAttribute();
  }

  // Render Inputs
  for (size_t i = 0; i < inputCount; ++i) {
    if (inputs[i].id == -1) continue;
    int inputSocketId = inputs[i].id;
    ImNodes::BeginInputAttribute(inputSocketId);
    ImGui::Text("%s", inputs[i].name.c_str());

    // Render input widgets based on type
    ImGui::SameLine();
    const float label_width = ImGui::CalcTextSize(inputs[i].name.c_str()).x;
    ImGui::PushItemWidth(NODE_WIDTH - label_width);

    if (Graph::graph.backLinks.count(inputSocketId) == 0) {
      switch (inputs[i].type) {
        case AttributeType::Float: {
          float value = std::get<float>(inputs[i].value);
          if (ImGui::SliderFloat("##value", &value, 0.f, 1.f)) {
            dirty = true;
            inputs[i].value = value;
          }
          break;
        }
        case AttributeType::Int: {
          int value = std::get<int>(inputs[i].value);
          if (ImGui::InputInt("##value", &value)) {
            dirty = true;
            inputs[i].value = value;
          }
          break;
        }
        case AttributeType::String: {
          std::string& value = std::get<std::string>(inputs[i].value);
          char buffer[256];  // A buffer for editing
          std::snprintf(buffer, sizeof(buffer), "%s", value.c_str());
          if (ImGui::InputText("##value", buffer, sizeof(buffer))) {
            value = buffer;
            dirty = true;
          }
          break;
        }
        case AttributeType::Vector: {
          glm::vec3& vec = std::get<glm::vec3>(inputs[i].value);
          float arr[3] = {vec.x, vec.y, vec.z};
          if (ImGui::InputFloat3("##value", arr)) {
            vec.x = arr[0];
            vec.y = arr[1];
            vec.z = arr[2];
            dirty = true;
          }
          break;
        }
        case AttributeType::Color: {
          auto& color = std::get<glm::vec3>(inputs[i].value);
          dirty |= ImGui::ColorEdit3("##value", &color.x);
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

void printSocketValue(const NodeEditor::Socket& socket) {
  std::visit([](auto&& arg) {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, float>) {
      std::cout << "Float value: " << arg << std::endl;
    } else if constexpr (std::is_same_v<T, int>) {
      std::cout << "Int value: " << arg << std::endl;
    } else if constexpr (std::is_same_v<T, std::string>) {
      std::cout << "String value: " << arg << std::endl;
      // } else if constexpr (std::is_same_v<T, glm::vec3>) {
      //   std::cout << "Vector value: (" << arg.x << ", " << arg.y << ", " << arg.z << ")" << std::endl;
    } else {
      std::cout << "Unknown or uninitialized value." << std::endl;
    }
  },
             socket.value);
}

}  // namespace NodeEditor
