#include "geometry.h"

namespace NodeEditor {

#define IN(t, i) (Graph::graph.backLinks.count(inputs[i]->id) == 0 ? std::get<t>(inputs[i]->value) : std::get<t>(Graph::graph.socketIdToSocket[Graph::graph.backLinks[inputs[i]->id]]->value))
#define OUT(i) outputs[i]->value

MergeGeometryNode::MergeGeometryNode() : Node("Merge Geometry") {
  addInput("Geometry 1", AttributeType::Mesh, std::vector<Vertex>());
  addInput("Geometry 2", AttributeType::Mesh, std::vector<Vertex>());
  addInput("Geometry 3", AttributeType::Mesh, std::vector<Vertex>());
  addInput("Geometry 4", AttributeType::Mesh, std::vector<Vertex>());
  addInput("Geometry 5", AttributeType::Mesh, std::vector<Vertex>());
  addOutput("Geometry", AttributeType::Mesh, std::vector<Vertex>());
}
void MergeGeometryNode::display() {
  renderSockets();
}
void MergeGeometryNode::evaluateInternal() {
  std::vector<Vertex>& vertices = std::get<std::vector<Vertex>>(OUT(0));
  std::vector<Vertex>& vertices1 = IN(std::vector<Vertex>, 0);
  std::vector<Vertex>& vertices2 = IN(std::vector<Vertex>, 1);
  std::vector<Vertex>& vertices3 = IN(std::vector<Vertex>, 2);
  std::vector<Vertex>& vertices4 = IN(std::vector<Vertex>, 3);
  std::vector<Vertex>& vertices5 = IN(std::vector<Vertex>, 4);

  vertices.clear();
  vertices.reserve(vertices1.size() + vertices2.size() + vertices3.size() + vertices4.size() + vertices5.size());
  vertices.insert(vertices.end(), vertices1.begin(), vertices1.end());
  vertices.insert(vertices.end(), vertices2.begin(), vertices2.end());
  vertices.insert(vertices.end(), vertices3.begin(), vertices3.end());
  vertices.insert(vertices.end(), vertices4.begin(), vertices4.end());
  vertices.insert(vertices.end(), vertices5.begin(), vertices5.end());
}

InstanceOnPointsNode::InstanceOnPointsNode() : Node("Instance On Points") {
  addInput("Geometry", AttributeType::Mesh, std::vector<Vertex>());
  addInput("Points", AttributeType::Mesh, std::vector<Vertex>());
  addOutput("Geometry", AttributeType::Mesh, std::vector<Vertex>());
}

void InstanceOnPointsNode::display() {
  renderSockets();
}

void InstanceOnPointsNode::evaluateInternal() {
  std::vector<Vertex>& vertices = std::get<std::vector<Vertex>>(OUT(0));

  std::vector<Vertex>& geometry = IN(std::vector<Vertex>, 0);
  std::vector<Vertex>& points = IN(std::vector<Vertex>, 1);

  vertices.clear();

  for (Vertex point : points) {
    for (Vertex vertex : geometry) {
      vertex.position += point.position;
      vertices.push_back(vertex);
    }
  }
}

#undef IN
#undef OUT

}  // namespace NodeEditor