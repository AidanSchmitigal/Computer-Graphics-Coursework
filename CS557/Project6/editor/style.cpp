#include "style.h"

#include "imgui.h"
// #include "imnodes/imnodes.h"

void StyleColorsMaterial() {
  ImGuiStyle* style = &ImGui::GetStyle();
  ImVec4* colors = style->Colors;

  colors[ImGuiCol_Text] = ImVec4(212 / 255.f, 216 / 255.f, 224 / 255.f, 1.0f);
  colors[ImGuiCol_TextDisabled] = ImVec4(212 / 255.f, 216 / 255.f, 224 / 255.f, 0.501960813999176f);
  colors[ImGuiCol_WindowBg] = ImVec4(44 / 255.f, 49 / 255.f, 60 / 255.f, 1.0f);
  colors[ImGuiCol_ChildBg] = ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1587982773780823f);
  colors[ImGuiCol_PopupBg] = ImVec4(44 / 255.f, 49 / 255.f, 60 / 255.f, 1.0f);
  colors[ImGuiCol_Border] = ImVec4(52 / 255.f, 59 / 255.f, 72 / 255.f, 1.0f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.0f);
  colors[ImGuiCol_FrameBg] = ImVec4(27 / 255.f, 29 / 255.f, 35 / 255.f, 0.501960813999176f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(83 / 255.f, 124 / 255.f, 166 / 255.f, 0.250980406999588f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(83 / 255.f, 124 / 255.f, 166 / 255.f, 1.0f);
  colors[ImGuiCol_TitleBg] = ImVec4(27 / 255.f, 29 / 255.f, 35 / 255.f, 1.0f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(27 / 255.f, 29 / 255.f, 35 / 255.f, 1.0f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(27 / 255.f, 29 / 255.f, 35 / 255.f, 1.0f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(27 / 255.f, 29 / 255.f, 35 / 255.f, 1.0f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(5 / 255.f, 5 / 255.f, 5 / 255.f, 0.0f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(136 / 255.f, 136 / 255.f, 136 / 255.f, 1.0f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(85 / 255.f, 85 / 255.f, 85 / 255.f, 1.0f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(153 / 255.f, 153 / 255.f, 153 / 255.f, 1.0f);
  colors[ImGuiCol_CheckMark] = ImVec4(83 / 255.f, 124 / 255.f, 166 / 255.f, 1.0f);
  colors[ImGuiCol_SliderGrab] = ImVec4(61 / 255.f, 133 / 255.f, 224 / 255.f, 1.0f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(66 / 255.f, 150 / 255.f, 250 / 255.f, 1.0f);
  colors[ImGuiCol_Button] = ImVec4(39 / 255.f, 44 / 255.f, 54 / 255.f, 0.501960813999176f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(39 / 255.f, 44 / 255.f, 54 / 255.f, 1.0f);
  colors[ImGuiCol_ButtonActive] = ImVec4(83 / 255.f, 124 / 255.f, 166 / 255.f, 1.0f);
  colors[ImGuiCol_Header] = ImVec4(39 / 255.f, 44 / 255.f, 54 / 255.f, 1.0f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(83 / 255.f, 124 / 255.f, 166 / 255.f, 0.250980406999588f);
  colors[ImGuiCol_HeaderActive] = ImVec4(83 / 255.f, 124 / 255.f, 166 / 255.f, 1.0f);
  colors[ImGuiCol_Separator] = ImVec4(109 / 255.f, 109 / 255.f, 127 / 255.f, 0.5f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(25 / 255.f, 102 / 255.f, 191 / 255.f, 0.7799999713897705f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(25 / 255.f, 102 / 255.f, 191 / 255.f, 1.0f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(27 / 255.f, 29 / 255.f, 35 / 255.f, 1.0f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(83 / 255.f, 124 / 255.f, 166 / 255.f, 0.250980406999588f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(83 / 255.f, 124 / 255.f, 166 / 255.f, 1.0f);
  colors[ImGuiCol_Tab] = ImVec4(39 / 255.f, 44 / 255.f, 54 / 255.f, 1.0f);
  colors[ImGuiCol_TabHovered] = ImVec4(83 / 255.f, 124 / 255.f, 166 / 255.f, 0.250980406999588f);
  colors[ImGuiCol_TabActive] = ImVec4(83 / 255.f, 124 / 255.f, 166 / 255.f, 1.0f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(39 / 255.f, 44 / 255.f, 54 / 255.f, 1.0f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(83 / 255.f, 124 / 255.f, 166 / 255.f, 1.0f);
  colors[ImGuiCol_PlotLines] = ImVec4(155 / 255.f, 155 / 255.f, 155 / 255.f, 1.0f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(255 / 255.f, 109 / 255.f, 89 / 255.f, 1.0f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(229 / 255.f, 178 / 255.f, 0 / 255.f, 1.0f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(255 / 255.f, 153 / 255.f, 0 / 255.f, 1.0f);
  colors[ImGuiCol_TableHeaderBg] = ImVec4(27 / 255.f, 29 / 255.f, 35 / 255.f, 1.0f);
  colors[ImGuiCol_TableBorderStrong] = ImVec4(52 / 255.f, 59 / 255.f, 72 / 255.f, 1.0f);
  colors[ImGuiCol_TableBorderLight] = ImVec4(52 / 255.f, 59 / 255.f, 72 / 255.f, 0.5021458864212036f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.0f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.03862661123275757f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(52 / 255.f, 59 / 255.f, 72 / 255.f, 1.0f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(255 / 255.f, 255 / 255.f, 0 / 255.f, 0.8999999761581421f);
  colors[ImGuiCol_NavHighlight] = ImVec4(66 / 255.f, 150 / 255.f, 249 / 255.f, 1.0f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(52 / 255.f, 59 / 255.f, 72 / 255.f, 0.7529411911964417f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(27 / 255.f, 29 / 255.f, 35 / 255.f, 0.7529411911964417f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(27 / 255.f, 29 / 255.f, 35 / 255.f, 0.7529411911964417f);

  style->Alpha = 1.0;
  style->DisabledAlpha = 0.5;
  style->WindowPadding = ImVec2(8.0, 8.0);
  style->WindowRounding = 0.0;
  style->WindowBorderSize = 1.0;
  style->WindowMinSize = ImVec2(32.0, 32.0);
  style->WindowTitleAlign = ImVec2(0.0, 0.5);
  style->WindowMenuButtonPosition = ImGuiDir_Left;
  style->ChildRounding = 0.0;
  style->ChildBorderSize = 1.0;
  style->PopupRounding = 0.0;
  style->PopupBorderSize = 1.0;
  style->FramePadding = ImVec2(4.0, 3.0);
  style->FrameRounding = 0.0;
  style->FrameBorderSize = 0.0;
  style->ItemSpacing = ImVec2(8.0, 4.0);
  style->ItemInnerSpacing = ImVec2(4.0, 4.0);
  style->CellPadding = ImVec2(4.0, 2.0);
  style->IndentSpacing = 21.0;
  style->ColumnsMinSpacing = 6.0;
  style->ScrollbarSize = 14.0;
  style->ScrollbarRounding = 0.0;
  style->GrabMinSize = 10.0;
  style->GrabRounding = 0.0;
  style->TabRounding = 0.0;
  style->TabBorderSize = 0.0;
  style->TabMinWidthForCloseButton = 0.0;
  style->ColorButtonPosition = ImGuiDir_Left;
  style->ButtonTextAlign = ImVec2(0.5, 0.5);
  style->SelectableTextAlign = ImVec2(0.0, 0.0);

  /*
  ImNodesStyle& dest = ImNodes::GetStyle();

  // Node background colors using ImGui's WindowBg and FrameBg colors
  dest.Colors[ImNodesCol_NodeBackground] = IM_COL32(44, 49, 60, 255);
  dest.Colors[ImNodesCol_NodeBackgroundHovered] = IM_COL32(89, 169, 248, 64);
  dest.Colors[ImNodesCol_NodeBackgroundSelected] = IM_COL32(61, 104, 149, 255);

  // Node outline using ImGui's Border color
  dest.Colors[ImNodesCol_NodeOutline] = IM_COL32(52, 59, 72, 255);

  // Title bar colors using ImGui's TitleBg colors
  dest.Colors[ImNodesCol_TitleBar] = IM_COL32(27, 29, 35, 255);
  dest.Colors[ImNodesCol_TitleBarHovered] = IM_COL32(79, 159, 238, 64);
  dest.Colors[ImNodesCol_TitleBarSelected] = IM_COL32(83, 124, 166, 255);

  // Link colors using ImGui's SliderGrab and ButtonActive colors
  dest.Colors[ImNodesCol_Link] = IM_COL32(61, 133, 224, 200);
  dest.Colors[ImNodesCol_LinkHovered] = IM_COL32(79, 159, 238, 255);
  dest.Colors[ImNodesCol_LinkSelected] = IM_COL32(79, 159, 238, 255);

  // Pin colors using ImGui's Button and ButtonActive colors
  dest.Colors[ImNodesCol_Pin] = IM_COL32(79, 159, 238, 128);
  dest.Colors[ImNodesCol_PinHovered] = IM_COL32(79, 159, 238, 255);

  // Box selector colors
  dest.Colors[ImNodesCol_BoxSelector] = IM_COL32(79, 159, 238, 30);
  dest.Colors[ImNodesCol_BoxSelectorOutline] = IM_COL32(79, 159, 238, 150);

  // Grid colors using similar dark tones from ImGui style
  dest.Colors[ImNodesCol_GridBackground] = IM_COL32(27, 29, 35, 200);
  dest.Colors[ImNodesCol_GridLine] = IM_COL32(109, 109, 127, 50);
  dest.Colors[ImNodesCol_GridLinePrimary] = IM_COL32(109, 109, 127, 100);

  // Minimap colors
  dest.Colors[ImNodesCol_MiniMapBackground] = IM_COL32(27, 29, 35, 150);
  dest.Colors[ImNodesCol_MiniMapBackgroundHovered] = IM_COL32(27, 29, 35, 200);
  dest.Colors[ImNodesCol_MiniMapOutline] = IM_COL32(52, 59, 72, 100);
  dest.Colors[ImNodesCol_MiniMapOutlineHovered] = IM_COL32(52, 59, 72, 200);
  dest.Colors[ImNodesCol_MiniMapNodeBackground] = IM_COL32(44, 49, 60, 100);
  dest.Colors[ImNodesCol_MiniMapNodeBackgroundHovered] = IM_COL32(44, 49, 60, 255);
  dest.Colors[ImNodesCol_MiniMapNodeBackgroundSelected] = IM_COL32(44, 49, 60, 255);
  dest.Colors[ImNodesCol_MiniMapNodeOutline] = IM_COL32(52, 59, 72, 100);
  dest.Colors[ImNodesCol_MiniMapLink] = dest.Colors[ImNodesCol_Link];
  dest.Colors[ImNodesCol_MiniMapLinkSelected] = dest.Colors[ImNodesCol_LinkSelected];
  dest.Colors[ImNodesCol_MiniMapCanvas] = IM_COL32(52, 59, 72, 25);
  dest.Colors[ImNodesCol_MiniMapCanvasOutline] = IM_COL32(52, 59, 72, 200);
  */
}