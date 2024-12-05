#include "cereal/archives/json.hpp"
#include "cereal/types/base_class.hpp"
#include "cereal/types/polymorphic.hpp"
#include "graph.h"

namespace NodeEditor {

#define IN(t, i) (graph->backLinks.count(inputs[i].attributeId) == 0 ? std::get<t>(inputs[i].value) : std::get<t>(graph->socketIdToSocket[inputs[i].attributeId]->value))
#define OUT(i) outputs[i].value

// extern std::unordered_map<std::string, std::function<Node*()>> menuItemToNodeCreator;

class GroupInputNode : public Node {
 public:
  GroupInputNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class GroupOutputNode : public Node {
 public:
  GroupOutputNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class ValueNode : public Node {
 public:
  ValueNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class AddNode : public Node {
 public:
  AddNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class PrintNode : public Node {
 public:
  PrintNode();
  void display() override;
  void evaluateInternal() override;

  std::string message;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class PrimitveCubeNode : public Node {
 public:
  PrimitveCubeNode();
  void display() override;
  void evaluateInternal() override;

  glm::vec3 size;
  int subdivisions;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

}  // namespace NodeEditor

CEREAL_REGISTER_TYPE(NodeEditor::AddNode)
CEREAL_REGISTER_TYPE(NodeEditor::ValueNode)
CEREAL_REGISTER_TYPE(NodeEditor::PrintNode)
CEREAL_REGISTER_TYPE(NodeEditor::PrimitveCubeNode)