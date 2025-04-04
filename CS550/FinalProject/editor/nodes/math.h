#pragma once

#include "cereal/types/base_class.hpp"
#include "cereal/types/polymorphic.hpp"
#include "graph.h"
#include "nodes.h"

namespace NodeEditor {

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

class SubtractNode : public Node {
 public:
  SubtractNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class MultiplyNode : public Node {
 public:
  MultiplyNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class DivideNode : public Node {
 public:
  DivideNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class PowerNode : public Node {
 public:
  PowerNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class ModuloNode : public Node {
 public:
  ModuloNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class AbsoluteNode : public Node {
 public:
  AbsoluteNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class FloorNode : public Node {
 public:
  FloorNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class CeilNode : public Node {
 public:
  CeilNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class RoundNode : public Node {
 public:
  RoundNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class SqrtNode : public Node {
 public:
  SqrtNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class SinNode : public Node {
 public:
  SinNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class CosNode : public Node {
 public:
  CosNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class TanNode : public Node {
 public:
  TanNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class ArcsinNode : public Node {
 public:
  ArcsinNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class ArccosNode : public Node {
 public:
  ArccosNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class ArctanNode : public Node {
 public:
  ArctanNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class Arctan2Node : public Node {
 public:
  Arctan2Node();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class LogNode : public Node {
 public:
  LogNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

class ExpNode : public Node {
 public:
  ExpNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

}  // namespace NodeEditor

CEREAL_REGISTER_TYPE(NodeEditor::AddNode)
CEREAL_REGISTER_TYPE(NodeEditor::SubtractNode)
CEREAL_REGISTER_TYPE(NodeEditor::MultiplyNode)
CEREAL_REGISTER_TYPE(NodeEditor::DivideNode)
CEREAL_REGISTER_TYPE(NodeEditor::PowerNode)
CEREAL_REGISTER_TYPE(NodeEditor::ModuloNode)
CEREAL_REGISTER_TYPE(NodeEditor::AbsoluteNode)
CEREAL_REGISTER_TYPE(NodeEditor::FloorNode)
CEREAL_REGISTER_TYPE(NodeEditor::CeilNode)
CEREAL_REGISTER_TYPE(NodeEditor::RoundNode)
CEREAL_REGISTER_TYPE(NodeEditor::SqrtNode)
CEREAL_REGISTER_TYPE(NodeEditor::SinNode)
CEREAL_REGISTER_TYPE(NodeEditor::CosNode)
CEREAL_REGISTER_TYPE(NodeEditor::TanNode)
CEREAL_REGISTER_TYPE(NodeEditor::ArcsinNode)
CEREAL_REGISTER_TYPE(NodeEditor::ArccosNode)
CEREAL_REGISTER_TYPE(NodeEditor::ArctanNode)
CEREAL_REGISTER_TYPE(NodeEditor::Arctan2Node)
CEREAL_REGISTER_TYPE(NodeEditor::LogNode)
CEREAL_REGISTER_TYPE(NodeEditor::ExpNode)