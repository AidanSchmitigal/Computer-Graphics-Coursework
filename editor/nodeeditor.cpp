#include "nodeeditor.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// #include "cereal/archives/json.hpp"  // or binary, XML, etc.
#include "cereal/archives/binary.hpp"
#include "cereal/types/base_class.hpp"
#include "cereal/types/polymorphic.hpp"
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include "imnodes/imnodes.h"
#include "nodes/graph.h"
#include "nodes/nodes.h"
#include "nodes/primitives.h"
#include "nodes/vectors.h"

namespace NodeEditor {

std::unordered_map<std::string, std::function<std::shared_ptr<Node>()>> primitivesMenu = {
    {"Cube", []() { return std::make_shared<PrimitiveCubeNode>(); }},
    {"Sphere", []() { return std::make_shared<PrimitiveSphereNode>(); }},
    {"Torus", []() { return std::make_shared<PrimitiveTorusNode>(); }},
    {"Cylinder", []() { return std::make_shared<PrimitiveCylinderNode>(); }},
    {"Cone", []() {
       auto cone = std::make_shared<PrimitiveCylinderNode>();
       cone->inputs[1]->value = 0.0f;
       return cone;
     }},
    {"TorusKnot", []() { return std::make_shared<PrimitiveTorusKnotNode>(); }},
    {"Plane", []() { return std::make_shared<PrimitivePlaneNode>(); }},
    {"Circle", []() { return std::make_shared<PrimitiveCircleNode>(); }},
};

std::unordered_map<std::string, std::function<std::shared_ptr<Node>()>> groupMenu = {
    {"Input", []() { return std::make_shared<GroupInputNode>(); }},
};

std::unordered_map<std::string, std::function<std::shared_ptr<Node>()>> vectorMenu = {
    {"Combine XYZ", []() { return std::make_shared<CombineXYZNode>(); }},
    {"Separate XYZ", []() { return std::make_shared<SeparateXYZNode>(); }},
    {"Vector", []() { return std::make_shared<CombineXYZNode>(); }},
    {"Vector Add", []() { return std::make_shared<VectorAddNode>(); }},
    {"Vector Subtract", []() { return std::make_shared<VectorSubtractNode>(); }},
    {"Vector Multiply", []() { return std::make_shared<VectorMultiplyNode>(); }},
    {"Vector Divide", []() { return std::make_shared<VectorDivideNode>(); }},
    {"Vector Length", []() { return std::make_shared<VectorLengthNode>(); }},
    {"Vector Normalize", []() { return std::make_shared<VectorNormalizeNode>(); }},
};

std::unordered_map<std::string, std::unordered_map<std::string, std::function<std::shared_ptr<Node>()>>> menuItemToNodeCreator = {
    {"Groups", groupMenu},
    {"Primitives", primitivesMenu},
    {"Vectors", vectorMenu},
};

bool center = true;

void ShowNodesPanel() {
  ImGui::Begin("Node Editor");
  ImNodes::BeginNodeEditor();
  {
    const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                            ImNodes::IsEditorHovered() && ((ImGui::IsKeyDown(ImGuiKey_A) && ImGui::IsKeyDown(ImGuiMod_Shift)) || ImGui::IsMouseDown(ImGuiMouseButton_Right));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
    if (!ImGui::IsAnyItemHovered() && open_popup) {
      ImGui::OpenPopup("add node");
    }

    if (ImGui::BeginPopup("add node")) {
      const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

      for (const auto& submenu : menuItemToNodeCreator) {
        if (ImGui::BeginMenu(submenu.first.c_str())) {
          for (const auto& [menuItem, createNode] : submenu.second) {
            if (ImGui::MenuItem(menuItem.c_str())) {
              std::shared_ptr<Node> node = createNode();
              Graph::graph.addNode(node);

              ImNodes::SetNodeScreenSpacePos(node->id, click_pos);
            }
          }
          ImGui::EndMenu();
        }
      }
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
  }

  Graph::graph.render();

  ImNodes::MiniMap();
  ImNodes::EndNodeEditor();
  {
    int start_attr, end_attr;
    if (ImNodes::IsLinkCreated(&start_attr, &end_attr)) {
      Graph::graph.connect(start_attr, end_attr);
    }
  }
  {
    int link_id;
    if (ImNodes::IsLinkDestroyed(&link_id)) {
      Graph::graph.disconnect(link_id);
    }
  }
  {
    const int num_selected = ImNodes::NumSelectedLinks();
    if (num_selected > 0 && ImGui::IsKeyReleased(ImGuiKey_X)) {
      static std::vector<int> selected_links;
      selected_links.resize(static_cast<size_t>(num_selected));
      ImNodes::GetSelectedLinks(selected_links.data());
      for (const int link_id : selected_links) {
        if (link_id < 0) continue;
        Graph::graph.disconnect(link_id);
      }
    }
  }
  {
    const int num_selected = ImNodes::NumSelectedNodes();
    if (num_selected > 0 && ImGui::IsKeyReleased(ImGuiKey_X)) {
      static std::vector<int> selected_nodes;
      selected_nodes.resize(static_cast<size_t>(num_selected));
      ImNodes::GetSelectedNodes(selected_nodes.data());
      for (const int node_id : selected_nodes) {
        if (node_id < 0) continue;
        Graph::graph.removeNode(node_id);
      }
    }
  }

  Graph::graph.evaluate();

  ImGui::End();
  if (center) {
    ImNodes::SetMiniMapNeedsCentering();
    center = false;
  }
}

void ShowGroupsPanel() {
  ImGui::Begin("Groups Panel");

  ImGui::Text("Groups");

  ImGui::End();
}

void save() {
  ImNodes::SaveCurrentEditorStateToIniFile("imnodes.ini");

  std::ofstream os("graph.bin");
  // cereal::JSONOutputArchive archive(os);
  cereal::BinaryOutputArchive archive(os);
  archive(Graph::graph);
}

void load() {
  ImNodes::LoadCurrentEditorStateFromIniFile("imnodes.ini");

  std::ifstream is("graph.bin");
  if (!is.is_open()) return;

  // cereal::JSONInputArchive archive(is);
  cereal::BinaryInputArchive archive(is);
  archive(Graph::graph);
  center = true;
}

}  // namespace NodeEditor