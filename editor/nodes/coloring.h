#pragma once

#include "cereal/types/base_class.hpp"
#include "cereal/types/polymorphic.hpp"
#include "graph.h"
#include "nodes.h"

namespace NodeEditor {

class SetColorNode : public Node {
 public:
  SetColorNode();
  void display() override;
  void evaluateInternal() override;

  template <class Archive>
  void serialize(Archive& archive) {
    archive(cereal::base_class<Node>(this));
  }
};

}  // namespace NodeEditor

CEREAL_REGISTER_TYPE(NodeEditor::SetColorNode)