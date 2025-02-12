#include "objects.h"

#include <unordered_map>

#include "imguizmo.h"
#include "objects/osucone.cpp"
#include "objects/osucube.cpp"
#include "objects/osugrid.cpp"
#include "objects/osusphere.cpp"
#include "objects/osutorus.cpp"
#include "utils/loadobjfile.h"

namespace Objects {

std::vector<std::shared_ptr<Object>> objects;
int selectedObject = -1;

const char* objectTypesNames[] = {"Cube", "Sphere", "Cylinder", "Cone", "Torus", "Plane", "Vertical Plane", "Europa Clipper", "Dirt Mound", "Frog", "Garden Sign", "ISS", "Seed Pouch", "Watering Can"};
const ObjectType objectTypes[] = {
    ObjectType::Cube,
    ObjectType::Sphere,
    ObjectType::Cylinder,
    ObjectType::Cone,
    ObjectType::Torus,
    ObjectType::Plane,
    ObjectType::Plane2,
    ObjectType::EuropaClipper,
    ObjectType::DirtMound,
    ObjectType::Frog,
    ObjectType::GardenSign,
    ObjectType::ISS,
    // ObjectType::Helicopter,
    ObjectType::SeedPouch,
    ObjectType::WateringCan,
};

std::unordered_map<ObjectType, GLuint> objectDisplayLists = {};

bool viewObjectsPanel = true;

bool useWindow = true;
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
static bool useSnap(false);
static float snap[3] = {1.f, 1.f, 1.f};

std::weak_ptr<Object> getSelectedObject() {
  if (selectedObject < 0 || selectedObject >= objects.size()) return std::weak_ptr<Object>();
  return objects[selectedObject];
}

void InitLists() {
  fprintf(stderr, "Starting InitLists.\n");

  objects.push_back(std::make_unique<Object>("Sphere", ObjectType::Sphere));

  // create the object:

  GLuint newList = objectDisplayLists[ObjectType::Cube] = glGenLists(1);
  printf("Cube\n");
  glNewList(newList, GL_COMPILE);
  OsuCube(1., 1., 1.);
  glEndList();

  newList = objectDisplayLists[ObjectType::Sphere] = glGenLists(1);
  printf("Sphere\n");
  glNewList(newList, GL_COMPILE);
  OsuSphere(1., 64, 64);
  glEndList();

  newList = objectDisplayLists[ObjectType::Cone] = glGenLists(1);
  printf("Cone\n");
  glNewList(newList, GL_COMPILE);
  OsuCone(1.f, 0.f, 1.f, 10, 10);
  glEndList();

  newList = objectDisplayLists[ObjectType::Cylinder] = glGenLists(1);
  printf("Cylinder\n");
  glNewList(newList, GL_COMPILE);
  OsuCone(1.f, 1.f, 1.f, 10, 10);
  glEndList();

  newList = objectDisplayLists[ObjectType::Torus] = glGenLists(1);
  printf("Torus\n");
  glNewList(newList, GL_COMPILE);
  OsuTorus(1.f, 0.2f, 10, 10);
  glEndList();

  newList = objectDisplayLists[ObjectType::Plane] = glGenLists(1);
  printf("Plane\n");
  glNewList(newList, GL_COMPILE);
  OsuGrid(10.0f, 10.0f);
  glEndList();

  newList = objectDisplayLists[ObjectType::Plane2] = glGenLists(1);
  printf("Plane2\n");
  glNewList(newList, GL_COMPILE);
  OsuGrid2();
  glEndList();

  newList = objectDisplayLists[ObjectType::EuropaClipper] = glGenLists(1);
  printf("EuropaClipper\n");
  glNewList(newList, GL_COMPILE);
  LoadObjFile("objects/clipper.obj");
  glEndList();

  newList = objectDisplayLists[ObjectType::DirtMound] = glGenLists(1);
  printf("DirtMound\n");
  glNewList(newList, GL_COMPILE);
  LoadObjFile("objects/dirt_mound.obj");
  glEndList();

  newList = objectDisplayLists[ObjectType::Frog] = glGenLists(1);
  printf("Frog\n");
  glNewList(newList, GL_COMPILE);
  LoadObjFile("objects/dumbfrog.obj");
  glEndList();

  newList = objectDisplayLists[ObjectType::GardenSign] = glGenLists(1);
  printf("GardenSign\n");
  glNewList(newList, GL_COMPILE);
  LoadObjFile("objects/gardensign.obj");
  glEndList();

  newList = objectDisplayLists[ObjectType::ISS] = glGenLists(1);
  printf("ISS\n");
  glNewList(newList, GL_COMPILE);
  // LoadObjFile("objects/iss.obj");
  glEndList();

  // newList = objectDisplayLists[ObjectType::Helicopter] = glGenLists(1);
  // glNewList(newList, GL_COMPILE);
  // LoadObjFile("objects/helicopter.obj");
  // glEndList();

  newList = objectDisplayLists[ObjectType::SeedPouch] = glGenLists(1);
  printf("SeedPouch\n");
  glNewList(newList, GL_COMPILE);
  LoadObjFile("objects/seed_pouch.obj");
  glEndList();

  newList = objectDisplayLists[ObjectType::WateringCan] = glGenLists(1);
  printf("WateringCan\n");
  glNewList(newList, GL_COMPILE);
  LoadObjFile("objects/watering_can.obj");
  glEndList();
}

void ShowObjectsPanel() {
  if (!viewObjectsPanel) return;

  ImGui::Begin("Objects");

  ImGuiIO& io = ImGui::GetIO();

  // ImGuizmo::IsUsing();
  // ImGuizmo::IsOver();
  // ImGuizmo::IsOver(ImGuizmo::TRANSLATE);
  // ImGuizmo::IsOver(ImGuizmo::ROTATE);
  // ImGuizmo::IsOver(ImGuizmo::SCALE);

  if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE) || ImGui::IsKeyPressed(ImGuiKey_T) || ImGui::IsKeyPressed(ImGuiKey_G))
    mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
  ImGui::SameLine();
  if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE) || ImGui::IsKeyPressed(ImGuiKey_R))
    mCurrentGizmoOperation = ImGuizmo::ROTATE;
  ImGui::SameLine();
  if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE) || ImGui::IsKeyPressed(ImGuiKey_S))
    mCurrentGizmoOperation = ImGuizmo::SCALE;

  // float matrixTranslation[3], matrixRotation[3], matrixScale[3];
  // ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(objects[selectedObject].transform), matrixTranslation, matrixRotation, matrixScale);
  // ImGui::InputFloat3("Tr", matrixTranslation);
  // ImGui::InputFloat3("Rt", matrixRotation);
  // ImGui::InputFloat3("Sc", matrixScale);
  // ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(objects[selectedObject].transform));

  if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
    if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
      mCurrentGizmoMode = ImGuizmo::LOCAL;
    ImGui::SameLine();
    if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
      mCurrentGizmoMode = ImGuizmo::WORLD;
  }

  ImGui::Separator();

  if (ImGui::BeginCombo("##ObjectAdder", "Add Object")) {
    for (int i = 0; i < (int)ObjectType::NUM_OBJECT_TYPES; i++) {
      if (ImGui::Selectable(objectTypesNames[i], selectedObject == objects.size())) {
        objects.push_back(std::make_unique<Object>(Object(std::string(objectTypesNames[i]), objectTypes[i])));
        selectedObject = objects.size() - 1;
      }
    }
    ImGui::EndCombo();
  }

  if (ImGui::Button("Remove Object")) {
    if (selectedObject != -1) {
      objects.erase(objects.begin() + selectedObject);
      selectedObject = -1;
    }
  }

  for (int i = 0; i < objects.size(); i++) {
    ImGui::PushID(i);
    ImGui::Checkbox("##box", &objects[i]->hidden);
    ImGui::SameLine();
    if (ImGui::Selectable(objects[i]->name.c_str(), selectedObject == i)) {
      selectedObject = i;
    }
    ImGui::PopID();
  }

  ImGui::End();
}

void Object::Display() {
  if (hidden) return;

  glPushMatrix();
  glMultMatrixf(glm::value_ptr(transform));

  auto use_shader = shader.lock();
  if (use_shader) {
    use_shader->use();
  }

  glCallList(objectDisplayLists[type]);

  use_shader->unUse();
  glPopMatrix();
}

void DisplayObjects() {
  for (int i = 0; i < objects.size(); i++) {
    objects[i]->Display();
  }
}

void DisplayGizmo(float* cameraView, float* cameraProjection) {
  if (objects.size() == 0) return;
  if (selectedObject < 0 || selectedObject >= objects.size()) return;

  ImGui::Begin("DockSpaceWindow");

  ImGuiIO& io = ImGui::GetIO();

  // Perspective(fov, io.DisplaySize.x / io.DisplaySize.y, 0.1f, 100.f, cameraProjection);

  // float eye[] = {cosf(camYAngle) * cosf(camXAngle) * camDistance, sinf(camXAngle) * camDistance, sinf(camYAngle) * cosf(camXAngle) * camDistance};
  // float at[] = {0.f, 0.f, 0.f};
  // float up[] = {0.f, 1.f, 0.f};
  // LookAt(eye, at, up, cameraView);

  float windowWidth = (float)ImGui::GetWindowWidth();
  float windowHeight = (float)ImGui::GetWindowHeight();
  ImVec2 windowPos = ImGui::GetWindowPos();

  ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
  ImGuizmo::SetRect(windowPos.x, windowPos.y - 8, windowWidth, windowHeight);  // ! IDK why 75 is needed

  // ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
  // ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);

  ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(objects[selectedObject]->transform), NULL, useSnap ? &snap[0] : NULL, useSnap ? &snap[1] : NULL, useSnap ? &snap[2] : NULL);

  ImGui::End();
  // for (int matId = 0; matId < gizmoCount; matId++) {
  //   ImGuizmo::PushID(matId);

  //   EditTransform(cameraView, cameraProjection, objectMatrix[matId]);
  //   if (ImGuizmo::IsUsing()) {
  //     lastUsing = matId;
  //   }
  //   ImGuizmo::PopID();
  // }
}

// void EditTransform(float* cameraView, float* cameraProjection, float* matrix) {
//   // ImGuiIO& io = ImGui::GetIO();
//   // float windowWidth = (float)ImGui::GetWindowWidth();
//   // float windowHeight = (float)ImGui::GetWindowHeight();
//   // if (!useWindow) {
//   //   ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
//   // } else {
//   //   ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
//   // }
//   // ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL);
// }

}  // namespace Objects