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
#include "imgui_stdlib.h"
#include "imnodes/imnodes.h"
#include "nodes/coloring.h"
#include "nodes/geometry.h"
#include "nodes/graph.h"
#include "nodes/math.h"
#include "nodes/nodes.h"
#include "nodes/primitives.h"
#include "nodes/transforms.h"
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

std::unordered_map<std::string, std::function<std::shared_ptr<Node>()>> transformMenu = {
    {"Translate", []() { return std::make_shared<TranslateNode>(); }},
    {"Rotate", []() { return std::make_shared<RotateNode>(); }},
    {"Scale", []() { return std::make_shared<ScaleNode>(); }},
};

std::unordered_map<std::string, std::function<std::shared_ptr<Node>()>> mathMenu = {
    {"Add", []() { return std::make_shared<AddNode>(); }},
    {"Subtract", []() { return std::make_shared<SubtractNode>(); }},
    {"Multiply", []() { return std::make_shared<MultiplyNode>(); }},
    {"Divide", []() { return std::make_shared<DivideNode>(); }},
    {"Power", []() { return std::make_shared<PowerNode>(); }},
    {"Modulo", []() { return std::make_shared<ModuloNode>(); }},
    {"Absolute", []() { return std::make_shared<AbsoluteNode>(); }},
    {"Floor", []() { return std::make_shared<FloorNode>(); }},
    {"Ceil", []() { return std::make_shared<CeilNode>(); }},
    {"Round", []() { return std::make_shared<RoundNode>(); }},
    {"Sqrt", []() { return std::make_shared<SqrtNode>(); }},
    {"Sin", []() { return std::make_shared<SinNode>(); }},
    {"Cos", []() { return std::make_shared<CosNode>(); }},
    {"Tan", []() { return std::make_shared<TanNode>(); }},
    {"Arcsin", []() { return std::make_shared<ArcsinNode>(); }},
    {"Arccos", []() { return std::make_shared<ArccosNode>(); }},
    {"Arctan", []() { return std::make_shared<ArctanNode>(); }},
    {"Arctan2", []() { return std::make_shared<Arctan2Node>(); }},
    {"Log", []() { return std::make_shared<LogNode>(); }},
    {"Exp", []() { return std::make_shared<ExpNode>(); }},
};

std::unordered_map<std::string, std::function<std::shared_ptr<Node>()>> geometryMenu = {
    {"Merge", []() { return std::make_shared<MergeGeometryNode>(); }},
    {"Instance", []() { return std::make_shared<InstanceOnPointsNode>(); }},
};

std::unordered_map<std::string, std::function<std::shared_ptr<Node>()>> coloringMenu = {
    {"Set Color", []() { return std::make_shared<SetColorNode>(); }},
};

std::unordered_map<std::string, std::unordered_map<std::string, std::function<std::shared_ptr<Node>()>>> menuItemToNodeCreator = {
    {"Groups", groupMenu},
    {"Primitives", primitivesMenu},
    {"Vectors", vectorMenu},
    {"Transforms", transformMenu},
    {"Math", mathMenu},
    {"Geometry", geometryMenu},
    {"Coloring", coloringMenu},
};

bool center = true;
std::string search = "";

#include <algorithm>
#include <cctype>
#include <string>

/// Try to find in the Haystack the Needle - ignore case
bool findStringIC(const std::string& strHaystack, const std::string& strNeedle) {
  auto it = std::search(
      strHaystack.begin(), strHaystack.end(),
      strNeedle.begin(), strNeedle.end(),
      [](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); });
  return (it != strHaystack.end());
}

void showAddNodeMenu() {
  const bool should_open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                                 ImNodes::IsEditorHovered() &&
                                 ((ImGui::IsKeyDown(ImGuiKey_A) && ImGui::IsKeyDown(ImGuiMod_Shift)) ||
                                  ImGui::IsMouseDown(ImGuiMouseButton_Right));

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));

  if (!ImGui::IsAnyItemHovered() && should_open_popup) {
    ImGui::OpenPopup("add node");
  }

  if (!ImGui::BeginPopup("add node")) {
    ImGui::PopStyleVar();
    return;
  }

  const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

  if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
      !ImGui::IsAnyItemActive() &&
      !ImGui::IsAnyMouseDown()) {
    ImGui::SetKeyboardFocusHere(0);
  }

  bool pick_first = ImGui::InputText("##search", &search, ImGuiInputTextFlags_EnterReturnsTrue);

  auto add_node = [&](const std::string& menu_item, const std::function<std::shared_ptr<Node>()>& create_node) {
    if (ImGui::MenuItem(menu_item.c_str()) || pick_first) {
      std::shared_ptr<Node> node = create_node();
      Graph::graph.addNode(node);
      ImNodes::SetNodeScreenSpacePos(node->id, click_pos);

      if (pick_first) {
        ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
        ImGui::PopStyleVar();
        search = "";
        return true;
      }
    }
    return false;
  };

  if (!search.empty()) {
    for (const auto& submenu : menuItemToNodeCreator) {
      for (const auto& [menu_item, create_node] : submenu.second) {
        bool found = findStringIC(menu_item, search);
        if (found && add_node(menu_item, create_node)) {
          return;
        }
      }
    }
  } else {
    for (const auto& submenu : menuItemToNodeCreator) {
      if (ImGui::BeginMenu(submenu.first.c_str())) {
        for (const auto& [menu_item, create_node] : submenu.second) {
          add_node(menu_item, create_node);
        }
        ImGui::EndMenu();
      }
    }
  }

  ImGui::EndPopup();
  ImGui::PopStyleVar();
}

void ShowNodesPanel() {
  ImGui::Begin("Node Editor");
  ImNodes::BeginNodeEditor();

  showAddNodeMenu();

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