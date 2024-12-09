#pragma once

#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/vector.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "../../imnodes/imnodes.h"
#include "cereal/types/array.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/unordered_set.hpp"
#include "cereal/types/variant.hpp"
#include "cereal/types/vector.hpp"
#include "glm/vec3.hpp"
#include "imgui.h"

namespace glm {
template <class Archive>
void serialize(Archive& archive, vec3& v) {
  archive(v.x, v.y, v.z);
}
}  // namespace glm

namespace NodeEditor {

struct Link {
  uint fromSocketId;
  uint toSocketId;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(fromSocketId, toSocketId);
  }
};

class Graph;

#define NODE_WIDTH 150
#define MAX_SOCKETS 10

enum class AttributeType {
  Mesh,
  Curve,
  Color,
  Vector,  // glm::vec3
  // Rotation,  // glm::quat
  Float,
  Int,
  String,
  Bool,
};

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;

  Vertex() : position(glm::vec3(0.f)), normal(glm::vec3(0.f)), color(glm::vec3(0.f)) {}
  Vertex(glm::vec3 position, glm::vec3 normal, glm::vec3 color) : position(position), normal(normal), color(color) {}

  template <class Archive>
  void serialize(Archive& archive) {
    archive(position, color, normal);
  }
};

using AttributeValue = std::variant<
    std::monostate,          // Represents an uninitialized state
    std::vector<glm::vec3>,  // Mesh or Curve can use std::vector<float> for simplicity
    std::vector<Vertex>,     // Mesh
    glm::vec3,               // For vectors
    // glm::quat,           // For rotations
    float,
    int,
    std::string,
    bool>;

struct Socket {
  std::string name;
  AttributeType type;
  uint id;
  AttributeValue value;

  Socket(const std::string& name, AttributeType type, uint id, const AttributeValue& value)
      : name(name), type(type), id(id), value(value) {}

  template <class Archive>
  void serialize(Archive& archive) {
    archive(name, type, id, value);
  }

  template <class Archive>
  static void load_and_construct(Archive& ar, cereal::construct<Socket>& construct) {
    std::string name;
    AttributeType type;
    uint id;
    AttributeValue value;

    ar(name, type, id, value);
    construct(name, type, id, value);
  }
};

class Node : public std::enable_shared_from_this<Node> {
 public:
  uint id;

  float displayWidth = 0.f;
  std::string name;
  int inputCount = 0;
  int outputCount = 0;
  std::array<std::shared_ptr<Socket>, MAX_SOCKETS> inputs;
  std::array<std::shared_ptr<Socket>, MAX_SOCKETS> outputs;

  bool dirty = true;
  double lastEvaluationTime = 0.0;

  explicit Node(const std::string& name);
  virtual ~Node();

  void addInput(const std::string& name, AttributeType type, const AttributeValue& defaultValue = {});
  void addOutput(const std::string& name, AttributeType type, const AttributeValue& defaultValue = {});

  void markDirty();  // Marks the node as dirty and propagates downstream
  virtual void display() = 0;

  void evaluate();
  virtual void evaluateInternal() = 0;  // Evaluation logic for derived classes

  template <class Archive>
  void serialize(Archive& archive) {
    archive(id, name, inputCount, outputCount, inputs, outputs, dirty, lastEvaluationTime);
  }

 protected:
  void renderSockets();
};

class GroupOutputNode;
class Graph {
 public:
  static Graph graph;

  uint currentId = 0;
  // All nodes
  std::vector<std::shared_ptr<Node>> nodes;
  // Map from node ID to node
  std::unordered_map<uint, std::shared_ptr<Node>> nodeIdToNode;
  // Map from Socket ID to Socket object
  std::unordered_map<uint, std::shared_ptr<Socket>> socketIdToSocket;
  // Map from input Socket ID to output Socket ID
  std::unordered_map<uint, uint> backLinks;
  // Map from output Socket ID to input Socket IDs
  std::unordered_map<uint, std::unordered_set<uint>> forwardLinks;

  std::vector<std::shared_ptr<Node>> evaluationOrder;

  std::shared_ptr<GroupOutputNode> outputNode;

  Graph();
  ~Graph() = default;  // Shared pointers handle memory automatically

  uint newId();
  void addNode(std::shared_ptr<Node> node);
  void removeNode(uint nodeId);
  void addSocket(std::shared_ptr<Socket> socket);
  void connect(uint fromSocketId, uint toSocketId);
  void disconnect(uint linkId);

  void propagateDirty(uint nodeId);
  std::shared_ptr<Node> getNode(uint nodeId);
  size_t size() const { return nodes.size(); }

  void evaluate();
  void render();

  template <class Archive>
  void serialize(Archive& archive) {
    archive(nodes, nodeIdToNode, socketIdToSocket, backLinks, forwardLinks, currentId);
  }
};

}  // namespace NodeEditor
