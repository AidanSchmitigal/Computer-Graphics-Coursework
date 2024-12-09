#pragma once

#include "cereal/types/base_class.hpp"
#include "cereal/types/polymorphic.hpp"
#include "graph.h"

namespace NodeEditor {

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

  const std::shared_ptr<Socket> getOutput();

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

}  // namespace NodeEditor

CEREAL_REGISTER_TYPE(NodeEditor::AddNode)
CEREAL_REGISTER_TYPE(NodeEditor::ValueNode)
CEREAL_REGISTER_TYPE(NodeEditor::PrintNode)
CEREAL_REGISTER_TYPE(NodeEditor::GroupInputNode)
CEREAL_REGISTER_TYPE(NodeEditor::GroupOutputNode)