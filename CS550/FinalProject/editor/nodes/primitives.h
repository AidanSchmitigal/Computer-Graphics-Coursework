#pragma once

#include "cereal/types/base_class.hpp"
#include "cereal/types/polymorphic.hpp"
#include "graph.h"
#include "nodes.h"

namespace NodeEditor {

// 3D Primitives

class PrimitiveCubeNode : public Node {
 public:
  PrimitiveCubeNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class PrimitiveSphereNode : public Node {
 public:
  PrimitiveSphereNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class PrimitiveTorusNode : public Node {
 public:
  PrimitiveTorusNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class PrimitiveCylinderNode : public Node {
 public:
  PrimitiveCylinderNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class PrimitiveTorusKnotNode : public Node {
 public:
  PrimitiveTorusKnotNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

// 2D Primitives

class PrimitivePlaneNode : public Node {
 public:
  PrimitivePlaneNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class PrimitiveCircleNode : public Node {
 public:
  PrimitiveCircleNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

}  // namespace NodeEditor
CEREAL_REGISTER_TYPE(NodeEditor::PrimitiveCubeNode)
CEREAL_REGISTER_TYPE(NodeEditor::PrimitiveSphereNode)
CEREAL_REGISTER_TYPE(NodeEditor::PrimitiveTorusNode)
CEREAL_REGISTER_TYPE(NodeEditor::PrimitiveCylinderNode)
CEREAL_REGISTER_TYPE(NodeEditor::PrimitiveTorusKnotNode)
CEREAL_REGISTER_TYPE(NodeEditor::PrimitivePlaneNode)
CEREAL_REGISTER_TYPE(NodeEditor::PrimitiveCircleNode)