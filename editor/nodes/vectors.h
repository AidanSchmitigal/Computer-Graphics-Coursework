#pragma once

#include "cereal/types/base_class.hpp"
#include "cereal/types/polymorphic.hpp"
#include "graph.h"
#include "nodes.h"

namespace NodeEditor {

class CombineXYZNode : public Node {
 public:
  CombineXYZNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};
class SeparateXYZNode : public Node {
 public:
  SeparateXYZNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};
class VectorAddNode : public Node {
 public:
  VectorAddNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};
class VectorSubtractNode : public Node {
 public:
  VectorSubtractNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};
class VectorMultiplyNode : public Node {
 public:
  VectorMultiplyNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};
class VectorDivideNode : public Node {
 public:
  VectorDivideNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};
class VectorLengthNode : public Node {
 public:
  VectorLengthNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};
class VectorNormalizeNode : public Node {
 public:
  VectorNormalizeNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

}  // namespace NodeEditor

CEREAL_REGISTER_TYPE(NodeEditor::CombineXYZNode)
CEREAL_REGISTER_TYPE(NodeEditor::SeparateXYZNode)
CEREAL_REGISTER_TYPE(NodeEditor::VectorAddNode)
CEREAL_REGISTER_TYPE(NodeEditor::VectorSubtractNode)
CEREAL_REGISTER_TYPE(NodeEditor::VectorMultiplyNode)
CEREAL_REGISTER_TYPE(NodeEditor::VectorDivideNode)
CEREAL_REGISTER_TYPE(NodeEditor::VectorLengthNode)
CEREAL_REGISTER_TYPE(NodeEditor::VectorNormalizeNode)