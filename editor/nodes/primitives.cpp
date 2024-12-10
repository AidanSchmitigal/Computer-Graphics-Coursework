#include "primitives.h"

#include <variant>
#include <vector>

#include "glm/common.hpp"
#include "graph.h"
#include "imgui_stdlib.h"

namespace NodeEditor {

#define IN(t, i) (Graph::graph.backLinks.count(inputs[i]->id) == 0 ? std::get<t>(inputs[i]->value) : std::get<t>(Graph::graph.socketIdToSocket[Graph::graph.backLinks[inputs[i]->id]]->value))
#define OUT(i) outputs[i]->value

PrimitiveCubeNode::PrimitiveCubeNode() : Node("Primitive Cube") {
  addInput("Size", AttributeType::Vector, glm::vec3(1.0f));
  addInput("Subdivisions", AttributeType::Int, 1);
  addOutput("Vertex Mesh", AttributeType::Mesh, std::vector<Vertex>());
}

void PrimitiveCubeNode::display() {
  renderSockets();
  ImGui::Text("Primitive Cube");
}

void PrimitiveCubeNode::evaluateInternal() {
  std::vector<Vertex>& vertices = std::get<std::vector<Vertex>>(OUT(0));
  glm::vec3& size = IN(glm::vec3, 0);
  int& subdivisions = IN(int, 1);

  vertices.clear();

  // Ensure subdivisions are at least 1
  subdivisions = std::max(1, subdivisions);

  auto createFace = [&](const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4, const glm::vec3& normal) {
    float stepX = 1.0f / subdivisions;
    float stepY = 1.0f / subdivisions;

    for (int i = 0; i < subdivisions; ++i) {
      for (int j = 0; j < subdivisions; ++j) {
        glm::vec3 p11 = glm::mix(p1, p2, i * stepX);
        glm::vec3 p12 = glm::mix(p1, p2, (i + 1) * stepX);
        glm::vec3 p21 = glm::mix(p3, p4, i * stepX);
        glm::vec3 p22 = glm::mix(p3, p4, (i + 1) * stepX);

        glm::vec3 v1 = glm::mix(p11, p21, j * stepY);
        glm::vec3 v2 = glm::mix(p11, p21, (j + 1) * stepY);
        glm::vec3 v3 = glm::mix(p12, p22, j * stepY);
        glm::vec3 v4 = glm::mix(p12, p22, (j + 1) * stepY);

        // Add two triangles for the quad
        vertices.push_back({v1, normal, glm::vec3(0.0f)});
        vertices.push_back({v3, normal, glm::vec3(0.0f)});
        vertices.push_back({v2, normal, glm::vec3(0.0f)});

        vertices.push_back({v2, normal, glm::vec3(0.0f)});
        vertices.push_back({v3, normal, glm::vec3(0.0f)});
        vertices.push_back({v4, normal, glm::vec3(0.0f)});
      }
    }
  };

  glm::vec3 halfSize = size * 0.5f;

  // Bottom face (-Y)
  createFace(
      {-halfSize.x, -halfSize.y, halfSize.z},
      {halfSize.x, -halfSize.y, halfSize.z},
      {-halfSize.x, -halfSize.y, -halfSize.z},
      {halfSize.x, -halfSize.y, -halfSize.z},
      {0.0f, -1.0f, 0.0f});

  // Top face (+Y)
  createFace(
      {-halfSize.x, halfSize.y, halfSize.z},
      {halfSize.x, halfSize.y, halfSize.z},
      {-halfSize.x, halfSize.y, -halfSize.z},
      {halfSize.x, halfSize.y, -halfSize.z},
      {0.0f, 1.0f, 0.0f});

  // Front face (+Z)
  createFace(
      {-halfSize.x, -halfSize.y, halfSize.z},
      {halfSize.x, -halfSize.y, halfSize.z},
      {-halfSize.x, halfSize.y, halfSize.z},
      {halfSize.x, halfSize.y, halfSize.z},
      {0.0f, 0.0f, 1.0f});

  // Back face (-Z)
  createFace(
      {halfSize.x, -halfSize.y, -halfSize.z},
      {-halfSize.x, -halfSize.y, -halfSize.z},
      {halfSize.x, halfSize.y, -halfSize.z},
      {-halfSize.x, halfSize.y, -halfSize.z},
      {0.0f, 0.0f, -1.0f});

  // Right face (+X)
  createFace(
      {halfSize.x, -halfSize.y, halfSize.z},
      {halfSize.x, -halfSize.y, -halfSize.z},
      {halfSize.x, halfSize.y, halfSize.z},
      {halfSize.x, halfSize.y, -halfSize.z},
      {1.0f, 0.0f, 0.0f});

  // Left face (-X)
  createFace(
      {-halfSize.x, -halfSize.y, -halfSize.z},
      {-halfSize.x, -halfSize.y, halfSize.z},
      {-halfSize.x, halfSize.y, -halfSize.z},
      {-halfSize.x, halfSize.y, halfSize.z},
      {-1.0f, 0.0f, 0.0f});
}

PrimitiveSphereNode::PrimitiveSphereNode() : Node("Primitive Sphere") {
  addInput("Radius", AttributeType::Float, 1.0f);
  addInput("Slices", AttributeType::Int, 16);
  addInput("Stacks", AttributeType::Int, 8);
  addOutput("Vertex Mesh", AttributeType::Mesh, std::vector<Vertex>());
}
void PrimitiveSphereNode::display() {
  renderSockets();
}

#define F_PI ((float)(M_PI))
#define F_2_PI ((float)(2.f * F_PI))
#define F_PI_2 ((float)(F_PI / 2.f))

inline void DrawSphLatLng(std::vector<Vertex>& vertices, float radius, float lat, float lng) {
  // lat is in radians between -F_PI_2 and +F_PI_2
  // lng is in radians between -F_PI and +F_PI
  float xz = cosf(lat);
  float x = xz * sinf(lng);
  float y = sinf(lat);
  float z = xz * cosf(lng);
  float nx = x;  // for a *sphere only*, the normal is the unitized position
  float ny = y;  // for a *sphere only*, the normal is the unitized position
  float nz = z;  // for a *sphere only*, the normal is the unitized position
  float s = (lng + F_PI) / F_2_PI;
  float t = (lat + F_PI_2) / F_PI;

  // glTexCoord2f(s, t);
  // glColor3f( 1.f, 0.5f, 0.f );
  // glNormal3f(nx, ny, nz);
  // glVertex3f(x * radius, y * radius, z * radius);

  vertices.push_back(Vertex(glm::vec3(x * radius, y * radius, z * radius), glm::vec3(nx, ny, nz), glm::vec3(0.f, 0.f, 0.f)));
}

void PrimitiveSphereNode::evaluateInternal() {
  std::vector<Vertex>& vertices = std::get<std::vector<Vertex>>(OUT(0));
  float& radius = IN(float, 0);
  int& slices = IN(int, 1);
  int& stacks = IN(int, 2);

  vertices.clear();

  radius = (float)fabs(radius);
  if (slices < 4) slices = 4;
  if (stacks < 4) stacks = 4;

  // south pole:
  {
    int istack = 0;
    float north = -F_PI_2 + F_PI * (float)(istack + 1) / (float)stacks;
    float south = -F_PI_2 + F_PI * (float)(istack + 0) / (float)stacks;
    for (int islice = 0; islice < slices; islice++) {
      float west = -F_PI + F_2_PI * (float)(islice + 0) / (float)slices;
      float east = -F_PI + F_2_PI * (float)(islice + 1) / (float)slices;

      DrawSphLatLng(vertices, radius, south, .5f * (east + west));
      DrawSphLatLng(vertices, radius, north, east);
      DrawSphLatLng(vertices, radius, north, west);
    }
  }

  // north pole:
  {
    int istack = stacks - 1;
    float north = -F_PI_2 + F_PI * (float)(istack + 1) / (float)stacks;
    float south = -F_PI_2 + F_PI * (float)(istack + 0) / (float)stacks;
    for (int islice = 0; islice < slices; islice++) {
      float west = -F_PI + F_2_PI * (float)(islice + 0) / (float)slices;
      float east = -F_PI + F_2_PI * (float)(islice + 1) / (float)slices;

      DrawSphLatLng(vertices, radius, north, .5f * (east + west));
      DrawSphLatLng(vertices, radius, south, west);
      DrawSphLatLng(vertices, radius, south, east);
    }
  }

  // all the bands in between:
  for (int istack = 1; istack < stacks - 1; istack++) {
    float north = -F_PI_2 + F_PI * (float)(istack + 1) / (float)stacks;
    float south = -F_PI_2 + F_PI * (float)(istack + 0) / (float)stacks;
    for (int islice = 0; islice < slices; islice++) {
      float west = -F_PI + F_2_PI * (float)(islice + 0) / (float)slices;
      float east = -F_PI + F_2_PI * (float)(islice + 1) / (float)slices;

      DrawSphLatLng(vertices, radius, north, west);
      DrawSphLatLng(vertices, radius, south, west);
      DrawSphLatLng(vertices, radius, north, east);

      DrawSphLatLng(vertices, radius, north, east);
      DrawSphLatLng(vertices, radius, south, west);
      DrawSphLatLng(vertices, radius, south, east);
    }
  }
}

PrimitiveTorusNode::PrimitiveTorusNode() : Node("Primitive Torus") {
  addInput("Inner Radius", AttributeType::Float, 0.5f);
  addInput("Outer Radius", AttributeType::Float, 1.0f);
  addInput("Sides", AttributeType::Int, 16);
  addInput("Rings", AttributeType::Int, 8);
  addOutput("Vertex Mesh", AttributeType::Mesh, std::vector<Vertex>());
}
void PrimitiveTorusNode::display() {
  renderSockets();
}
void PrimitiveTorusNode::evaluateInternal() {
  std::vector<Vertex>& vertices = std::get<std::vector<Vertex>>(OUT(0));
  float& innerRadius = IN(float, 0);
  float& outerRadius = IN(float, 1);
  int& nsides = IN(int, 2);
  int& nrings = IN(int, 3);

  vertices.clear();

  float ringDelta = 2.0f * F_PI / (float)nrings;
  float sideDelta = 2.0f * F_PI / (float)nsides;

  float theta = 0.0;
  float cosTheta = 1.0;
  float sinTheta = 0.0;

  for (int i = 0; i < nrings; i++) {
    float theta1 = theta + ringDelta;
    float cosTheta1 = cosf(theta1);
    float sinTheta1 = sinf(theta1);

    float phi = 0.0;
    float s0 = 1.f - (float)(i + 0) / (float)nrings;
    float s1 = 1.f - (float)(i + 1) / (float)nrings;

    for (int j = 0; j <= nsides; j++) {
      float cosPhi = cosf(phi);
      float sinPhi = sinf(phi);
      float dist = outerRadius + innerRadius * cosPhi;

      float t = 1.f - (float)j / (float)nsides;

      // setTex(s0, t);
      // setNorm(cosTheta * cosPhi, sinPhi, -sinTheta * cosPhi);
      // setVert(cosTheta * dist, innerRadius * sinPhi, -sinTheta * dist);
      auto newVert = Vertex(glm::vec3(cosTheta * dist, innerRadius * sinPhi, -sinTheta * dist), glm::vec3(cosTheta * cosPhi, sinPhi, -sinTheta * cosPhi), glm::vec3(0.f, 0.f, 0.f));
      if (vertices.size() >= 3) {
        vertices.push_back(vertices.end()[-2]);
        vertices.push_back(vertices.end()[-2]);
      }
      vertices.push_back(newVert);

      // setTex(s1, t);
      // setNorm(cosTheta1 * cosPhi, sinPhi, -sinTheta1 * cosPhi);
      // setVert(cosTheta1 * dist, innerRadius * sinPhi, -sinTheta1 * dist);
      auto newVert2 = Vertex(glm::vec3(cosTheta1 * dist, innerRadius * sinPhi, -sinTheta1 * dist), glm::vec3(cosTheta1 * cosPhi, sinPhi, -sinTheta1 * cosPhi), glm::vec3(0.f, 0.f, 0.f));
      if (vertices.size() >= 3) {
        vertices.push_back(vertices.end()[-2]);
        vertices.push_back(vertices.end()[-2]);
      }
      vertices.push_back(newVert2);

      phi += sideDelta;
    }

    theta = theta1;
    cosTheta = cosTheta1;
    sinTheta = sinTheta1;
  }
}

inline void DrawConeLatLng(std::vector<Vertex>& vertices, int ilat, int ilng, int numlats, int numlngs, float radbot, float radtop, float height, bool strip = false) {
  float t = (float)ilat / (float)(numlats - 1);
  float y = t * height;
  float rad = t * radtop + (1.f - t) * radbot;
  float lng = -F_PI + 2.f * F_PI * (float)ilng / (float)(numlngs - 1);
  float x = cosf(lng);
  float z = -sinf(lng);
  float s = (float)ilng / (float)(numlngs - 1);
  float n[3] = {height * x, radbot - radtop, height * z};
  float norm = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
  n[0] /= norm;
  n[1] /= norm;
  n[2] /= norm;

  // glTexCoord2f(s, t);
  // Unit(n, n);
  // glNormal3fv(n);
  // glVertex3f(rad * x, y, rad * z);

  if (strip) {
    vertices.push_back(vertices.end()[-2]);
    vertices.push_back(vertices.end()[-2]);
  }
  vertices.push_back(Vertex(glm::vec3(rad * x, y, rad * z), glm::vec3(n[0], n[1], n[2]), glm::vec3(0.f, 0.f, 0.f)));
}

PrimitiveCylinderNode::PrimitiveCylinderNode() : Node("Primitive Cylinder") {
  addInput("Bottom Radius", AttributeType::Float, 0.5f);
  addInput("Top Radius", AttributeType::Float, 0.5f);
  addInput("Height", AttributeType::Float, 1.0f);
  addInput("Slices", AttributeType::Int, 16);
  addInput("Stacks", AttributeType::Int, 8);
  addOutput("Vertex Mesh", AttributeType::Mesh, std::vector<Vertex>());
}
void PrimitiveCylinderNode::display() {
  renderSockets();
}
void PrimitiveCylinderNode::evaluateInternal() {
  std::vector<Vertex>& vertices = std::get<std::vector<Vertex>>(OUT(0));
  float& radBot = IN(float, 0);
  float& radTop = IN(float, 1);
  float& height = IN(float, 2);
  int& slices = IN(int, 3);
  int& stacks = IN(int, 4);

  vertices.clear();

  // sanity check:
  radBot = (float)fabs((double)radBot);
  radTop = (float)fabs((double)radTop);
  slices = abs(slices);
  stacks = abs(stacks);
  if (slices < 4) slices = 3;

  int numLngs = slices;
  int numLats = stacks;

  for (int ilat = 0; ilat < numLats - 1; ilat++) {
    DrawConeLatLng(vertices, ilat + 0, 0, numLats, numLngs, radBot, radTop, height);
    DrawConeLatLng(vertices, ilat + 1, 0, numLats, numLngs, radBot, radTop, height);

    for (int ilng = 1; ilng < numLngs; ilng++) {
      DrawConeLatLng(vertices, ilat + 0, ilng, numLats, numLngs, radBot, radTop, height, ilng != 1);
      DrawConeLatLng(vertices, ilat + 1, ilng, numLats, numLngs, radBot, radTop, height, true);
    }
  }

  // draw the bottom circle:
  if (radBot != 0.) {
    for (int ilng = numLngs - 1; ilng >= 0; ilng--) {
      DrawConeLatLng(vertices, 0, ilng + 1, numLats, numLngs, radBot, radTop, height);
      DrawConeLatLng(vertices, 0, ilng + 0, numLats, numLngs, radBot, radTop, height);

      float s = (float)ilng / (float)(numLngs - 1);
      vertices.push_back(Vertex(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
    }
  }

  // draw the top circle:
  if (radTop != 0.) {
    for (int ilng = 0; ilng < numLngs - 1; ilng++) {
      float s = (float)ilng / (float)(numLngs - 1);
      vertices.push_back(Vertex(glm::vec3(0.f, height, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));

      DrawConeLatLng(vertices, numLats - 1, ilng + 0, numLats, numLngs, radBot, radTop, height);
      DrawConeLatLng(vertices, numLats - 1, ilng + 1, numLats, numLngs, radBot, radTop, height);
    }
  }
}

PrimitiveTorusKnotNode::PrimitiveTorusKnotNode() : Node("Primitive TorusKnot") {
  addInput("P", AttributeType::Int, 2);
  addInput("Q", AttributeType::Int, 3);
  addInput("Resolution", AttributeType::Int, 30);
  addOutput("Vertex Mesh", AttributeType::Mesh, std::vector<Vertex>());
}
void PrimitiveTorusKnotNode::display() {
  renderSockets();
}
void PrimitiveTorusKnotNode::evaluateInternal() {
  std::vector<Vertex>& vertices = std::get<std::vector<Vertex>>(OUT(0));
  int& p = IN(int, 0);
  int& q = IN(int, 1);
  int& resolution = IN(int, 2);

  vertices.clear();

  float radius = 3.f;

  for (int i = 0; i < resolution; i++) {
    float t = (float)i / (float)(resolution - 1);
    float phi = t * F_PI * 2.f;

    float r = cosf(q * phi) + 2;

    float x = r * cosf(p * phi) * radius;
    float y = -r * sinf(q * phi);
    float z = r * sinf(p * phi) * radius;

    vertices.push_back(Vertex(glm::vec3(x, y, z), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
  }
}

PrimitivePlaneNode::PrimitivePlaneNode() : Node("Primitive Plane") {
  addInput("X Size", AttributeType::Float, 1.0f);
  addInput("Y Size", AttributeType::Float, 1.0f);
  addInput("Subdivisions", AttributeType::Int, 1);
  addOutput("Vertex Mesh", AttributeType::Mesh, std::vector<Vertex>());
}
void PrimitivePlaneNode::display() {
  renderSockets();
}
void PrimitivePlaneNode::evaluateInternal() {
  std::vector<Vertex>& vertices = std::get<std::vector<Vertex>>(OUT(0));
  float& xSize = IN(float, 0);
  float& ySize = IN(float, 1);
  int& subdivisions = IN(int, 2);

  vertices.clear();

  for (int i = 0; i < subdivisions; i++) {
    float x = xSize * i / (float)subdivisions;
    float x2 = xSize * (i + 1) / (float)subdivisions;
    float y = ySize * i / (float)subdivisions;
    float y2 = ySize * (i + 1) / (float)subdivisions;

    vertices.push_back(Vertex(glm::vec3(x, 0.0f, y), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    vertices.push_back(Vertex(glm::vec3(x2, 0.0f, y), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    vertices.push_back(Vertex(glm::vec3(x2, 0.0f, y2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

    vertices.push_back(Vertex(glm::vec3(x, 0.0f, y), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    vertices.push_back(Vertex(glm::vec3(x2, 0.0f, y2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    vertices.push_back(Vertex(glm::vec3(x, 0.0f, y2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
  }
}

PrimitiveCircleNode::PrimitiveCircleNode() : Node("Primitive Circle") {
  addInput("Radius", AttributeType::Float, 1.0f);
  addInput("Subdivisions", AttributeType::Int, 16);
  addOutput("Vertex Mesh", AttributeType::Mesh, std::vector<Vertex>());
}
void PrimitiveCircleNode::display() {
  renderSockets();
}
void PrimitiveCircleNode::evaluateInternal() {
  std::vector<Vertex>& vertices = std::get<std::vector<Vertex>>(OUT(0));
  float& radius = IN(float, 0);
  int& subdivisions = IN(int, 1);

  vertices.clear();

  for (int i = 0; i < subdivisions; i++) {
    float x = radius * cosf(2.0f * F_PI * (float)i / (float)subdivisions);
    float x2 = radius * cosf(2.0f * F_PI * (float)(i + 1) / (float)subdivisions);
    float y = radius * sinf(2.0f * F_PI * (float)i / (float)subdivisions);
    float y2 = radius * sinf(2.0f * F_PI * (float)(i + 1) / (float)subdivisions);

    vertices.push_back(Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    vertices.push_back(Vertex(glm::vec3(x, 0.0f, y), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    vertices.push_back(Vertex(glm::vec3(x2, 0.0f, y2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
  }
}

#undef IN
#undef OUT

}  // namespace NodeEditor