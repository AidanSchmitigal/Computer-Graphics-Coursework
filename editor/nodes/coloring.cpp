#include "coloring.h"

namespace NodeEditor {

#define IN(t, i) (Graph::graph.backLinks.count(inputs[i]->id) == 0 ? std::get<t>(inputs[i]->value) : std::get<t>(Graph::graph.socketIdToSocket[Graph::graph.backLinks[inputs[i]->id]]->value))
#define OUT(i) outputs[i]->value

SetColorNode::SetColorNode() : Node("Set Color") {
  addInput("Mesh", AttributeType::Mesh, std::vector<Vertex>());
  addInput("Color", AttributeType::Color, glm::vec3(0.f));
  addOutput("Mesh", AttributeType::Mesh, std::vector<Vertex>());
}
void SetColorNode::display() {
  renderSockets();
}
void SetColorNode::evaluateInternal() {
  std::vector<Vertex>& vertices = std::get<std::vector<Vertex>>(OUT(0));

  std::vector<Vertex>& mesh = IN(std::vector<Vertex>, 0);
  glm::vec3& color = IN(glm::vec3, 1);

  vertices.clear();

  for (Vertex vertex : mesh) {
    vertex.color = color;
    vertices.push_back(vertex);
  }
}

#undef IN
#undef OUT

}  // namespace NodeEditor