#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "../../imnodes/imnodes.h"
#include "cereal/archives/json.hpp"  // or binary, XML, etc.
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
  int fromSocketId;
  int toSocketId;

  template <class Archive>
  void serialize(Archive& archive, Link& link) {
    archive(link.fromSocketId, link.toSocketId);
  }
};

class Graph;

#define NODE_WIDTH 100
#define MAX_INPUTS 10
#define MAX_OUTPUTS 10

enum class AttributeType {
  TriangleMesh,
  Curve,
  Color,
  Vector,  // glm::vec3
  // Rotation,  // glm::quat
  Float,
  Int,
  String,
  Bool,
};

using AttributeValue = std::variant<
    std::monostate,          // Represents an uninitialized state
    std::vector<glm::vec3>,  // VertexMesh or Curve can use std::vector<float> for simplicity
    glm::vec3,               // For vectors
    // glm::quat,           // For rotations
    float,
    int,
    std::string,
    bool>;

struct Socket {
  std::string name;
  AttributeType type;
  int id;
  AttributeValue value;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(name, type, id, value);
  }
};

class Node {
 public:
  int id;

  std::string name;
  int inputCount = 0;
  int outputCount = 0;
  std::array<Socket, MAX_INPUTS> inputs;
  std::array<Socket, MAX_OUTPUTS> outputs;

  bool dirty = true;
  double lastEvaluationTime = 0.0;

  Node(std::string name);
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

class Graph {
 public:
  static Graph graph;

  int currentId;
  // All nodes
  std::vector<Node*> nodes;
  // Map from node ID to node
  std::unordered_map<int, Node*> nodeIdToNode;
  // Map from Socket ID to Socket object
  std::unordered_map<int, Socket*> socketIdToSocket;
  // Map from input Socket ID to output Socket ID
  std::unordered_map<int, int> backLinks;
  // Map from output Socket ID to input Socket IDs
  std::unordered_map<int, std::unordered_set<int>> forwardLinks;

  std::vector<Node*> evaluationOrder;

  Graph();
  ~Graph();

  int newId();
  void addNode(Node* node);
  void removeNode(Node* node);
  void addSocket(Socket* socket);
  void connect(int fromSocketId, int toSocketId);
  void disconnect(int fromSocketId, int toSocketId);

  void propagateDirty(int nodeId);
  Node* getNode(int nodeId);
  int size() { return nodes.size(); }

  void evaluate();

  void render();

  template <class Archive>
  void serialize(Archive& archive) {
    archive(currentId, backLinks, forwardLinks);
  }
};

}  // namespace NodeEditor
