#include "nodeeditor.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../imnodes/imnodes.h"
#include "cereal/archives/json.hpp"  // or binary, XML, etc.
#include "cereal/types/base_class.hpp"
#include "cereal/types/polymorphic.hpp"
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include "nodes/graph.h"
#include "nodes/nodes.h"

namespace NodeEditor {

std::unordered_map<std::string, std::function<Node*()>> menuItemToNodeCreator = {
    {"Value", []() { return new ValueNode(); }},
    {"Add", []() { return new AddNode(); }},
    {"Print", []() { return new PrintNode(); }},
    {"Primitive Cube", []() { return new PrimitveCubeNode(); }},
    {"Group Input", []() { return new GroupInputNode(); }}};

void ShowNodesPanel() {
  ImGui::Begin("Node Editor");
  ImNodes::BeginNodeEditor();
  {
    const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                            ImNodes::IsEditorHovered() && ImGui::IsKeyDown(ImGuiKey_A) && ImGui::IsKeyDown(ImGuiMod_Shift);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
    if (!ImGui::IsAnyItemHovered() && open_popup) {
      ImGui::OpenPopup("add node");
    }

    if (ImGui::BeginPopup("add node")) {
      const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

      for (const auto& [menuItem, createNode] : menuItemToNodeCreator) {
        if (ImGui::MenuItem(menuItem.c_str())) {
          Node* node = createNode();
          ImNodes::SetNodeScreenSpacePos(node->id, click_pos);
        }
      }
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
  }

  Graph::graph.render();

  ImNodes::MiniMap();
  ImNodes::EndNodeEditor();
  int start_attr, end_attr;
  if (ImNodes::IsLinkCreated(&start_attr, &end_attr)) {
    Graph::graph.connect(start_attr, end_attr);
  }

  Graph::graph.evaluate();

  ImGui::End();
}

void save() {
  std::ofstream os("graph.json");
  cereal::JSONOutputArchive archive(os);
  archive(Graph::graph);
}

void load() {
  std::ifstream is("graph.json");
  if (!is.is_open()) {
    return;
  }
  cereal::JSONInputArchive archive(is);
  archive(Graph::graph);
}

}  // namespace NodeEditor