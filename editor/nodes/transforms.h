#pragma once

#include "cereal/types/base_class.hpp"
#include "cereal/types/polymorphic.hpp"
#include "graph.h"
#include "nodes.h"

namespace NodeEditor {

class TranslateNode : public Node {
 public:
  TranslateNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class RotateNode : public Node {
 public:
  RotateNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class ScaleNode : public Node {
 public:
  ScaleNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

}  // namespace NodeEditor

CEREAL_REGISTER_TYPE(NodeEditor::TranslateNode)
CEREAL_REGISTER_TYPE(NodeEditor::RotateNode)
CEREAL_REGISTER_TYPE(NodeEditor::ScaleNode)