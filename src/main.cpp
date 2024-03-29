
#include <base.hpp>
#include <mesh.hpp>
#include <animation.hpp>
#include <camera-controller.hpp>

#include <drawing.hpp>
#include <modeling/shape.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <modeling/delaunay.hpp>
#include <modeling/medial-axis.hpp>
#include <modeling/medial-axis-generator.hpp>
#include <modeling/cylindrical-douglas-peucker.hpp>
#include <modeling/cylinder-generator.hpp>
#include <modeling/skeleton-generator.hpp>
#include <modeling/rigging.hpp>
#include <modeling/skining-generator.hpp>
#include <modeling/chords-generator.hpp>
#include <modeling/mesh-generator.hpp>
#include <mesh-skeleton.hpp>

#include <modeling/rigging-mesh.hpp>

#include <geometry/draw-2d.hpp>

#include <modeling/smoothing.hpp>

#include <modeling/remeshing.hpp>

#include <modeling/operations.h>


Renderer * renderer = nullptr;
Mesh * selectedMesh = nullptr;
CameraController * camController;
Drawing * drawing;

void errorCallback(int error, const char *desc) {
  std::cout <<  "Error " << error << ": " << desc << std::endl;
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
  renderer->getCamera().setAspectRatio(static_cast<float>(width)/static_cast<float>(height));
  glViewport(0, 0, (GLint)width, (GLint)height);
  getOpenGLError("view port change");
}

void cursor_position_callback(GLFWwindow* window, double xPos, double yPos) {
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  camController->mouseSetMousePos(xPos, yPos);
  xPos = (xPos - width/2);
  yPos = -(yPos - height/2);
  camController->mouseSetScreenPos(xPos, yPos);

  drawing->update(xPos/(width/2), yPos/(height/2));
}
void cursor_enter_callback(GLFWwindow* window, int entered)
{
  if(entered) {
    camController->enableLook(true);
    camController->enableMove(true);
  }
  else {
    camController->enableLook(false);
    camController->enableMove(false);
    
    drawing->stop();
  }
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  camController->onZoom(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    glm::vec3 rayDirection;
    glm::vec3 rayStartPos;
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    camController->mouseMakeWorldRay(rayDirection, rayStartPos, width, height);

    // Mesh * m = new Mesh(
    //   MeshGeometry::meshGetSphereData(0.01f, 10, 10, nullptr),
    //   MeshMaterial::meshGetBasicMaterial(COLOR_ORANGE)
    // );
    // m->setHighLightable(false);
    // m->translateMesh(rayStartPos);
    // renderer->addRenderable(m);
    // m = new Mesh(
    //   MeshGeometry::meshGetSphereData(0.05f, 10, 10, nullptr),
    //   MeshMaterial::meshGetBasicMaterial(COLOR_GREEN)
    // );
    // m->setHighLightable(false);
    // m->translateMesh(rayStartPos+rayDirection);
    // renderer->addRenderable(m);

    Renderable * object  = renderer->getRayIntersection(rayDirection, rayStartPos);
    if(object && object->getIsHighLightable()) {
      if(selectedMesh) {
        selectedMesh->setHighLight(false);
      }
      object->setHighLight(true);
      selectedMesh = (Mesh*) object;
    }
    else {
      if(selectedMesh) {
        selectedMesh->setHighLight(false);
        selectedMesh = nullptr;
      }
    }
  }

  else if(button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    camController->setRotate(true);
  }
  else if(button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
    camController->setRotate(false);
  }

  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    drawing->continu();
  }
  else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    drawing->stop();
  }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS && key == GLFW_KEY_W) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if(action == GLFW_PRESS && key == GLFW_KEY_F) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else if(action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
    glfwSetWindowShouldClose(window, true);
  }
  
  else if(action == GLFW_PRESS && key == GLFW_KEY_I) {
    if(selectedMesh) {
      selectedMesh->translateMesh({0.0f, 0.1f, 0.0f});
    }
    else
      camController->setMoveFront(true);
  }
  else if(action == GLFW_PRESS && key == GLFW_KEY_K) {
    if(selectedMesh) {
      selectedMesh->translateMesh({0.0f, -0.1f, 0.0f});
    }
    else
      camController->setMoveBack(true);
  }
  else if(action == GLFW_PRESS && key == GLFW_KEY_L) {
    if(selectedMesh) {
      selectedMesh->translateMesh({0.1f, 0.0f, 0.0f});
    }
    else
      camController->setMoveRight(true);
  }
  else if(action == GLFW_PRESS && key == GLFW_KEY_J) {
    if(selectedMesh) {
      selectedMesh->translateMesh({-0.1f, 0.0f, 0.0f});
    }
    else
      camController->setMoveLeft(true);
  }
  else if(action == GLFW_PRESS && key == GLFW_KEY_Y) {
    if(selectedMesh) {
      selectedMesh->translateMesh({0.0f, 0.0f, 0.1f});
    }
    else
      camController->setMoveUp(true);
  }
  else if(action == GLFW_PRESS && key == GLFW_KEY_H) {
    if(selectedMesh) {
      selectedMesh->translateMesh({0.0f, 0.0f, -0.1f});
    }
    else
      camController->setMoveDown(true);
  }

  else if(action == GLFW_RELEASE && key == GLFW_KEY_I) {
    camController->setMoveFront(false);
  }
  else if(action == GLFW_RELEASE && key == GLFW_KEY_K) {
    camController->setMoveBack(false);
  }
  else if(action == GLFW_RELEASE && key == GLFW_KEY_L) {
    camController->setMoveRight(false);
  }
  else if(action == GLFW_RELEASE && key == GLFW_KEY_J) {
    camController->setMoveLeft(false);
  }
  else if(action == GLFW_RELEASE && key == GLFW_KEY_Y) {
    camController->setMoveUp(false);
  }
  else if(action == GLFW_RELEASE && key == GLFW_KEY_H) {
    camController->setMoveDown(false);
  }

  else if(action == GLFW_RELEASE && key == GLFW_KEY_N) {
    if(selectedMesh) {
      selectedMesh->setHighLight(false);
      selectedMesh = nullptr;
    }
  }

  else if(action == GLFW_PRESS && key == GLFW_KEY_LEFT_ALT) {
    camController->setSlide(true);
  }
  else if(action == GLFW_RELEASE && key == GLFW_KEY_LEFT_ALT) {
    camController->setSlide(false);
  }

  else if(action == GLFW_PRESS && key == GLFW_KEY_LEFT_CONTROL) {
    camController->setRotate(true);
  }
  else if(action == GLFW_RELEASE && key == GLFW_KEY_LEFT_CONTROL) {
    camController->setRotate(false);
  }

}

void initImGui() {
  const char* glsl_version = "#version 420";

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
  //io.ConfigViewportsNoAutoMerge = true;
  //io.ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsLight();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(renderer->getWindow(), true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

// Our state
bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

ControllerMode controllerMode = ControllerMode::CAM;

bool drawing_render = true;

int im_resolution_w = 300;
int im_resolution_h = 300;

glm::vec3 chordColor = {0, 200, 0};
glm::vec3 axisColor = {0, 200, 200};
glm::vec3 axisPointColor = {0, 20, 200};
glm::vec3 shapeColor = {150, 0, 0};
glm::vec3 shapePointColor = {0, 0, 220};
glm::vec3 skeletonPointColor = {200, 200, 0};
glm::vec3 skeletonColor0 = {200, 0, 0};
glm::vec3 skeletonColor1 = {0, 200, 0};
glm::vec3 skeletonColor2 = {0, 0, 200};

glm::vec3 cylinderMeshColor = {0, 200, 200};
glm::vec3 cylinderMeshColorUnselected = {0, 100, 100};
glm::vec3 mergedMeshColor = {200, 0, 0};
glm::vec3 skeletonMeshColor = {0, 250, 0};
glm::vec3 skeletonMeshColorHighLight = {255, 255, 255};

std::vector<Renderable *> skeletonMesh;
Mesh * generatedMesh = nullptr;
MeshSkeleton * generatedMeshSkeleton = nullptr;

std::vector<Renderable *> skeletonFirstMesh;
Mesh * firstMesh = nullptr;
MeshSkeleton * generatedFirstMeshSkeleton = nullptr;

Mesh * mergedMesh = nullptr;

bool show_skeleton = true;
bool show_mesh = true;
bool show_merged_mesh = true;

float cdp_threshold = 0.3;
float importanceCylindricalError = 1.0f;
float importanceDistanceError = 1.0f;

float pruning__threshold = 0.5f;

int ma_prun_depth = 3;
int ma_prun_count = 3;

int sub_sampling = 20;

int cylinder_sampling = 20;

int smooth_mask_size = 2;

int bones_count = 0;
int focus_bone_index = 0;

char textureFilename[2048];
bool useTexture = false;

void computeSkeletonAndMeshes(
  const std::vector<glm::vec3> & meshVertices,
  const std::vector<glm::uvec3> & meshFaces,
  Shape & shape,
  std::vector<std::vector<glm::vec2>> & externalAxis,
  std::vector<std::vector<glm::vec2>> & internalAxis,
  std::vector<Geometry::Edge> & chords
) {
  std::cout << "External axis = " << externalAxis.size() << std::endl;
  std::cout << "Internal axis = " << internalAxis.size() << std::endl;
  std::cout << "Chords = " << chords.size() << std::endl;


  // Full skeleton
  SkeletonGenerator skelGen(
    shape.getFullPoints(), externalAxis, internalAxis, chords,
    cdp_threshold, importanceDistanceError, importanceCylindricalError);
  skelGen.compute();

  Rigging & rigging = skelGen.getRigging();
  rigging.initSkinning(meshVertices.size());
  SkiningGenerator skinGen(rigging, meshVertices, meshFaces);
  skinGen.compute();

  bones_count = rigging.getBonesSkins().size();
  std::cout << "Bones : " << bones_count << std::endl;

  {
    std::vector<std::pair<glm::vec2, glm::vec2>> bones2D;
    std::vector<glm::vec2> bonesPoints;
    for(auto & bone : skelGen.getRigging().getBones()) {
      bones2D.push_back({
        glm::vec2(bone.getA().getPoint()),
        glm::vec2(bone.getB().getPoint())
      });
      bonesPoints.push_back(glm::vec2(bone.getA().getPoint()));
      bonesPoints.push_back(glm::vec2(bone.getB().getPoint()));
    }
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.setExtraPoints(shape.getFullPoints());
    builder.addExtraPoints(bonesPoints);
    builder.drawSegments(bones2D, skeletonColor0);
    builder.drawShape(true, shape.getFullPoints(), shapeColor);
    builder.drawPoints(shape.getFullPoints(), shapePointColor);
    builder.save("im-080-skeleton.png");
  }
  {
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.setExtraPoints(shape.getFullPoints());
    for(auto ax : skelGen.getExternalAxisSkeleton()) {
      builder.drawShape(false, ax, skeletonColor0);
      builder.drawPoints(ax, axisPointColor);
    }
    for(auto ax : skelGen.getInternalAxisSkeleton()) {
      builder.drawShape(false, ax, skeletonColor1);
      builder.drawPoints(ax, axisPointColor);
    }
    for(auto ax : skelGen.getJunctionAxisSkeleton()) {
      builder.drawShape(false, ax, skeletonColor2);
      builder.drawPoints(ax, axisPointColor);
    }
    builder.drawShape(true, shape.getFullPoints(), shapeColor);
    builder.drawPoints(shape.getFullPoints(), shapePointColor);
    builder.save("im-090-skeleton-details.png");
  }
  
  // std::vector<glm::vec3> meshColors;
  // meshColors.reserve(meshVertices.size());
  // auto & skinGroup = rigging.getBonesSkins().at(focus_bone_index);
  // for(unsigned v=0; v<meshVertices.size(); v++) {
  //   auto weight_ = skinGroup.getVertexSkinWeights().find(v);
  //   if(weight_ != skinGroup.getVertexSkinWeights().end()) {
  //     float weight = weight_->second;
  //     meshColors.push_back({0.f, weight, 0.f});
  //   }
  // }

  // Meshes rendering

  // Mesh
  if(generatedMesh!=nullptr) {
    renderer->removeRenderable(generatedMesh);
  }
  if(useTexture) {
    auto geo = new MeshGeometry(meshVertices, meshFaces);
    geo->recomputeVertexTexCoord_flat_mapping(1);
    generatedMesh = new Mesh(
      geo,
      MeshMaterial::meshGetBasicTextureMaterial(textureFilename, cylinderMeshColor*0.001f)
    );
  }
  else {
    auto geo = new MeshGeometry(meshVertices, meshFaces);
    generatedMesh = new Mesh(
      geo,
      MeshMaterial::meshGetSimplePhongMaterial(cylinderMeshColor*0.01f, cylinderMeshColor*0.001f, 1)
    );
  }
  renderer->addRenderable(generatedMesh);
  // Skeleton of te mesh
  generatedMeshSkeleton = new MeshSkeleton(rigging);
  for(unsigned b=0; b<generatedMeshSkeleton->getBones().size(); b++) {
    for(unsigned v=0; v<generatedMesh->getGeometry()->getVertexPositions().size(); v++) {
      auto coef = rigging.getBonesSkins().at(b).getVertexSkinWeights().at(v);
      generatedMeshSkeleton->setVerticesCoef(
        b, v, coef
      );
    }
  }
  generatedMeshSkeleton->initVerticesTranformsCoef();
  generatedMesh->setSkeleton(generatedMeshSkeleton);
  // Skeleton mesh
  if(skeletonMesh.size()>0) {
    for(auto m : skeletonMesh)
      renderer->removeRenderable(m);
  }
  skeletonMesh = generatedMeshSkeleton->getSkeletonMesh(skeletonMeshColor);
  for(auto m : skeletonMesh) {
    m->setDepthTest(false);
    renderer->addRenderable(m);
  }
  ((Mesh*)skeletonMesh.at(focus_bone_index))->getMaterial()->setBasicColor(skeletonMeshColorHighLight);

  drawing_render = false;
}

void testPipeline(
  Shape shape
) {

  {
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.drawShape(true, shape.getFullPoints(), shapeColor);
    builder.drawPoints(shape.getFullPoints(), shapePointColor);
    builder.save("im-000-shape.png");
  }

  // Delaunay constrained triangulation
  ConstrainedDelaunayTriangulation2D d(shape.getSubSampledPoints());
  auto trianglesSub = d.getTriangles();
  auto chordsSub = d.getEdges();
  auto triangles = shape.convertToFullTriangleSet(trianglesSub);
  auto chords = shape.convertToFullEdgeSet(chordsSub);
  {
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.drawTriangles(shape.getFullPoints(), triangles, chordColor);
    builder.drawShape(true, shape.getFullPoints(), shapeColor);
    builder.drawPoints(shape.getFullPoints(), shapePointColor);
    builder.save("im-010-delaunay-triangulation.png");
  }

  // The raw medial axis
  MedialAxisGenerator medial(shape.getSubSampledPoints(), trianglesSub);
  // medial.compute();
  auto midPoints = medial.computeMidPoints();
  for(unsigned i=0; i<3; i++){
    if(i==1) {
      // for(int ii=1; ii<=ma_prun_count; ii++) {
      //   // medial.pruning(ma_prun_depth*(ii/ma_prun_count));
      //   medial.pruning(ma_prun_depth);
      // }

      smoothing s;
      s.insignificantBranchesRemoval(
        medial, pruning__threshold, shape.getSubSampledPoints()
      );

    }
    if(i==2) {
      medial.smooth(smooth_mask_size);
    }
    auto axis = medial.getMedialAxis();
    std::vector<std::pair<glm::vec2, glm::vec2>> segments;
    for(auto ax : axis.getPoints()) {
      for(auto s : ax->getAdjs()) {
        segments.emplace_back(ax->getPoint(), s->getPoint());
      }
    }
    std::vector<glm::vec2> pointsSeg;
    for(auto seg : segments) {
        pointsSeg.push_back(seg.first);
        pointsSeg.push_back(seg.second);
    }
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.addExtraPoints(shape.getFullPoints());
    builder.addExtraPoints(pointsSeg);
    builder.drawSegments(segments, axisColor);
    builder.drawPoints(pointsSeg, axisPointColor);
    builder.drawShape(true, shape.getFullPoints(), shapeColor);
    builder.drawPoints(shape.getFullPoints(), shapePointColor);
    if(i==0) {
      builder.save("im-020-raw-medial-axis.png");
      builder.drawTriangles(shape.getFullPoints(), triangles, chordColor);
      builder.save("im-020-raw-medial-axis-with-triangles.png");
    }
    if(i==1) builder.save("im-030-pruned-medial-axis.png");
    if(i==2) builder.save("im-040-pruned-smoothed-medial-axis.png");
  }

  // External axis of the medial axis tree
  auto externalAxis = medial.extractExternalAxis();
  {
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.setExtraPoints(shape.getFullPoints());
    for(auto ax : externalAxis) {
      builder.drawShape(false, ax, axisColor);
      builder.drawPoints(ax, axisPointColor);
      std::cout << "External axis size : " << ax.size() << std::endl;
    }
    builder.drawShape(true, shape.getFullPoints(), shapeColor);
    builder.drawPoints(shape.getFullPoints(), shapePointColor);
    builder.save("im-050-external-medial-axis.png");
  }
  // Internal axis of the medial axis tree
  auto internalAxis = medial.extractInternalAxis();
  {
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.setExtraPoints(shape.getFullPoints());
    for(auto ax : internalAxis) {
      builder.drawShape(false, ax, axisColor);
      builder.drawPoints(ax, axisPointColor);
    }
    builder.drawShape(true, shape.getFullPoints(), shapeColor);
    builder.drawPoints(shape.getFullPoints(), shapePointColor);
    builder.save("im-060-internal-medial-axis.png");
  }

  if(externalAxis.size()==0 && internalAxis.size()==0) {
    std::cout << "No axis !!!!! Hint : Reduce the prunning threshold" << std::endl;
    return;
  }

  ChordsGenerator chordsGen(
    shape.getFullPoints(),
    externalAxis, internalAxis
  );
  chordsGen.compute();
  std::vector<Geometry::Edge> optiChords = chordsGen.getChords();
  auto axisPointToChord = chordsGen.axisPointToChord;
  {
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.setExtraPoints(shape.getFullPoints());
    builder.drawEdges(shape.getFullPoints(), optiChords, chordColor);

    for(auto ax : externalAxis) {
      builder.drawShape(false, ax, axisColor);
      builder.drawPoints(ax, axisPointColor);
    }
    for(auto ax : internalAxis) {
      builder.drawShape(false, ax, axisColor);
      builder.drawPoints(ax, axisPointColor);
    }

    builder.drawShape(true, shape.getFullPoints(), shapeColor);
    builder.drawPoints(shape.getFullPoints(), shapePointColor);
    builder.save("im-070-chords.png");
  }

  // Mesh vertices and faces

  MeshGenerator meshGen(
    shape.getFullPoints(),
    externalAxis, internalAxis,
    optiChords, axisPointToChord, cylinder_sampling
  );
  meshGen.compute();
  std::vector<glm::vec3> meshVertices = meshGen.getVertices();
  std::vector<glm::uvec3> meshFaces = meshGen.getFaces();
  std::cout << "Vertices : " << meshVertices.size() << std::endl;
  std::cout << "Faces : " << meshFaces.size() << std::endl;

  // performSmoothing(meshVertices, meshFaces, 10);

  computeSkeletonAndMeshes(
    meshVertices, meshFaces,
    shape,
    externalAxis, internalAxis, chords
  );
}

float cameraNear = 0.1f;
float cameraFar = 10.0f;

bool withFaceCull = false;

glm::vec3 bone_rotation = {0.f,0.f,0.f};

int index_photo = 0;

char shapeFilename[2048];

void renderImGui() {
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Sketchy !");

    ImGui::Text("Camera & rendering");
    ImGui::SliderFloat("Near", &cameraNear, 0.1f, 10.0f);
    ImGui::SliderFloat("Far", &cameraFar, 1.0f, 100.0f);
    ImGui::Checkbox("Face Cull", &withFaceCull);
    ImGui::Separator();

    ImGui::Text("Camera type");
    if (ImGui::RadioButton("FIXED", controllerMode == NONE)) { controllerMode = NONE; } ImGui::SameLine();
    if (ImGui::RadioButton("FPS", controllerMode == FPS)) { controllerMode = FPS; } ImGui::SameLine();
    if (ImGui::RadioButton("CAM", controllerMode == CAM)) { controllerMode = CAM; }
    ImGui::Checkbox("Display drawing", &drawing_render);
    ImGui::Separator();

    // Cylindrical Douglas-Peucker threshold
    ImGui::Text("Douglas-Peucker");
    ImGui::SliderFloat("global threshold", &cdp_threshold, 0.0f, 1.0f);
    ImGui::SliderFloat("Cyl error weight", &importanceCylindricalError, 0.0f, 5.0f);
    ImGui::SliderFloat("Dist error weight", &importanceDistanceError, 0.0f, 5.0f);
    ImGui::Separator();

    ImGui::Text("Medial axis");
    // ImGui::SliderInt("prunning thresh", &ma_prun_depth, 3, 60);
    // ImGui::SliderInt("nbr of prunning", &ma_prun_count, 1, 10);
    ImGui::SliderFloat("Pruning threshold", &pruning__threshold, 0.0f, 10.0f);
    ImGui::SliderInt("Smoothing size", &smooth_mask_size, 1, 10);
    ImGui::SliderInt("Shape sampling", &sub_sampling, 1, 50);
    ImGui::Separator();

    ImGui::Text("3D Generation");
    ImGui::SliderInt("Cylinder sampling", &cylinder_sampling, 1, 100);

    ImGui::Text("Debug Images");
    ImGui::SliderInt("resolution w", &im_resolution_w, 100, 2000);
    ImGui::SliderInt("resolution h", &im_resolution_h, 100, 2000);
    ImGui::Separator();

    ImGui::Text("Texture");
    ImGui::InputText("Texture file", textureFilename, IM_ARRAYSIZE(textureFilename));
    ImGui::Checkbox("Use texture", &useTexture);
    ImGui::Separator();

    ImGui::Text("Pipeline");
    ImGui::InputText("shape file", shapeFilename, IM_ARRAYSIZE(shapeFilename));
    if (ImGui::Button("Load shape file")) {
      drawing->clearDrawing();
      drawing->loadShape(shapeFilename);
    }
    if (ImGui::Button("Save shape file")) {
      drawing->saveShape(shapeFilename);
    }
    if (ImGui::Button("START")) {
      std::vector<glm::vec2> points;
      for(auto p : drawing->getDrawing(drawing->drawingCount()-1)) {
        points.emplace_back(p.x, p.y);
      }
      Shape shape(sub_sampling, points);
      testPipeline(shape);
    } ImGui::SameLine(); ImGui::Text("<---------------------------------------------");
    // Should show skeleton
    bool prev_show_skeleton = show_skeleton;
    ImGui::Checkbox("Show skeleton", &show_skeleton);
    if(prev_show_skeleton!=show_skeleton) {
      for(auto sk : skeletonMesh) {
        ((Mesh*)sk)->shouldRender = show_skeleton;
      }
    }
    // Should show mesh
    bool prev_show_mesh = show_mesh;
    ImGui::Checkbox("Show mesh", &show_mesh);
    if(prev_show_mesh!=show_mesh && generatedMesh) {
      ((Mesh*)generatedMesh)->shouldRender = show_mesh;
    }
    // Should show merged mesh
    bool prev_show_merged_mesh = show_merged_mesh;
    ImGui::Checkbox("Show merge", &show_merged_mesh);
    if(prev_show_merged_mesh!=show_merged_mesh && mergedMesh) {
      ((Mesh*)mergedMesh)->shouldRender = show_merged_mesh;
    }
    // Drawing clearing
    if (ImGui::Button("CLEAR DRAWING")) {
      drawing->clearDrawing();
    }
    // Add mesh
    if (ImGui::Button("Other mesh") && generatedMesh) {
      if(skeletonFirstMesh.size()>0) {
        for(auto m : skeletonFirstMesh)
          renderer->removeRenderable(m);
      }
      if(firstMesh) {
        renderer->removeRenderable(firstMesh);
      }
      if(generatedFirstMeshSkeleton) {
        delete generatedFirstMeshSkeleton;
        generatedFirstMeshSkeleton = nullptr;
      }
      firstMesh = generatedMesh;
      skeletonFirstMesh = skeletonMesh;
      generatedFirstMeshSkeleton = generatedMeshSkeleton;
      generatedMesh = nullptr;
      skeletonMesh.clear();
      generatedMeshSkeleton = nullptr;
      firstMesh->getMaterial()->setDiffuseColor(cylinderMeshColorUnselected*0.01f);
      firstMesh->getMaterial()->setSpecularColor(cylinderMeshColorUnselected*0.001f);
    }
    if (ImGui::Button("Merge meshes") && generatedMesh && firstMesh) {
      operations op;

      std::vector<glm::vec3> positions1 = firstMesh->getGeometry()->getVertexPositions();
      std::vector<glm::uvec3> faces1 = firstMesh->getGeometry()->getFaces();
      std::vector<glm::vec3> positions2 = generatedMesh->getGeometry()->getVertexPositions();
      std::vector<glm::uvec3> faces2 = generatedMesh->getGeometry()->getFaces();
      op.mergeCustom(positions1, faces1, positions2, faces2);

      if(mergedMesh!=nullptr) {
        renderer->removeRenderable(mergedMesh);
      }
      mergedMesh = new Mesh(
        new MeshGeometry(positions1, faces1),
        MeshMaterial::meshGetSimplePhongMaterial(cylinderMeshColor*0.01f, cylinderMeshColor*0.001f, 1)
      );
      renderer->addRenderable(mergedMesh);
    }
    ImGui::Separator();

    ImGui::Text("Meshes");
    if(ImGui::Button("Mesh STL file") && generatedMesh) {
      writeSTL(
        "mesh.stl",
        generatedMesh->getGeometry()->getFaces(),
        generatedMesh->getGeometry()->getVertexPositions());
    }
    // if(ImGui::Button("Skeleton STL file") && skeletonMesh) {
    //   writeSTL(
    //     "skeleton.stl",
    //     skeletonMesh->getGeometry()->getFaces(),
    //     skeletonMesh->getGeometry()->getVertexPositions());
    // }
    if (ImGui::Button("Clear mesh")) {
      if(generatedMesh) {
        renderer->removeRenderable(generatedMesh);
      }
      if(skeletonMesh.size()>0) {
        for(auto m : skeletonMesh)
          renderer->removeRenderable(m);
      }
    }
    ImGui::Separator();

    ImGui::Text("Skeleton");
    ImGui::Text("Bones count = %d", bones_count);
    int prev_focus_bone_index = focus_bone_index;
    ImGui::SliderInt("Select bone", &focus_bone_index, 0, bones_count-1);
    if(focus_bone_index != prev_focus_bone_index) {
      for(unsigned i=0; i<bones_count; i++) {
        ((Mesh*)skeletonMesh.at(i))->getMaterial()->setBasicColor(skeletonMeshColor);
      }
      ((Mesh*)skeletonMesh.at(focus_bone_index))->getMaterial()->setBasicColor(skeletonMeshColorHighLight);
      bone_rotation = glm::vec3(0.f);
    }
    // X axis
    if(ImGui::Button("RotateX+")) {
      if(generatedMesh && bones_count>0) {
        bone_rotation.x += M_PI*0.1;
        generatedMeshSkeleton->rotateBoneArroundA(focus_bone_index, bone_rotation);
      }
    }
    ImGui::SameLine();
    if(ImGui::Button("RotateX-")) {
      if(generatedMesh && bones_count>0) {
        bone_rotation.x -= M_PI*0.1;
        generatedMeshSkeleton->rotateBoneArroundA(focus_bone_index, bone_rotation);
      }
    }
    // Y axis
    if(ImGui::Button("RotateY+")) {
      if(generatedMesh && bones_count>0) {
        bone_rotation.y += M_PI*0.1;
        generatedMeshSkeleton->rotateBoneArroundA(focus_bone_index, bone_rotation);
      }
    }
    ImGui::SameLine();
    if(ImGui::Button("RotateY-")) {
      if(generatedMesh && bones_count>0) {
        bone_rotation.y -= M_PI*0.1;
        generatedMeshSkeleton->rotateBoneArroundA(focus_bone_index, bone_rotation);
      }
    }
    // Z axis
    if(ImGui::Button("RotateZ+")) {
      if(generatedMesh && bones_count>0) {
        bone_rotation.z += M_PI*0.1;
        generatedMeshSkeleton->rotateBoneArroundA(focus_bone_index, bone_rotation);
      }
    }
    ImGui::SameLine();
    if(ImGui::Button("RotateZ-")) {
      if(generatedMesh && bones_count>0) {
        bone_rotation.z -= M_PI*0.1;
        generatedMeshSkeleton->rotateBoneArroundA(focus_bone_index, bone_rotation);
      }
    }
    ImGui::Separator();

    if(ImGui::Button("Photo")) {
      if(renderer) {
        renderer->saveCurrentImageTGA(buildIndexedString("photos/photo",index_photo,".tga"));
        index_photo += 1;
      }
    }

    ImGui::End();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

void interactImGui() {
  camController->setControllerMode(controllerMode);
}

glm::vec3 planeColor = {0.8f, 0.8f, 0.8f};
glm::vec3 planePos = {0.f, -1.f, 0.f};

glm::vec3 lightColor = {1.f, 1.f, 1.f};
glm::vec3 lightMeshColor = {1.f, 1.f, 1.f};
glm::vec3 lightPos1 = {0, 1, 1};
glm::vec3 lightPos2 = {0, 1, -1};

int main(int argc, char ** argv) {

  renderer = new Renderer();
  renderer->initGLFW(1024, 768,
    windowSizeCallback, keyCallback, errorCallback,
    cursor_position_callback, cursor_enter_callback, mouse_button_callback, mouse_scroll_callback);
  renderer->initOpenGL();
  renderer->initCamera();
  glm::vec3 cameraPos = {0.0f, 0.0f, -5.0};
  renderer->getCamera().setPosition(cameraPos);

  camController = new CameraController(renderer->getCamera());
  drawing = new Drawing();

  // renderer->addLight(Light::lightGetConstantCaster(
  //   lightPos1, lightColor*0.001f, lightColor, lightColor*0.f
  // ));
  renderer->addLight(Light::lightGetConstantCaster(
    lightPos2, lightColor*0.01f, lightColor, lightColor*0.f
  ));

  // Mesh * plane = new Mesh(
  //   MeshGeometry::meshGetPlaneData(50, 50, nullptr),
  //   MeshMaterial::meshGetSimplePhongMaterial(planeColor, planeColor, 10)
  // );
  // plane->setPosition(planePos);
  // renderer->addRenderable(plane);

  // Mesh * cube = new Mesh(
  //   MeshGeometry::meshGetSphereData(1, 20, 20, nullptr),
  //   MeshMaterial::meshGetBasicTextureMaterial("media/earth.jpg", planeColor*0.f)
  // );
  // cube->setPosition(planePos);
  // renderer->addRenderable(cube);

  initImGui();

  long delta = 0;
  while(!glfwWindowShouldClose(renderer->getWindow())) {
    glfwSwapBuffers(renderer->getWindow());

    delta = renderer->updateDeltaTime();
    renderer->render();

    renderImGui();
    interactImGui();
    renderer->getCamera().setNear(cameraNear);
    renderer->getCamera().setFar(cameraFar);
    renderer->faceCulling(withFaceCull);
    camController->update(delta);

    if(drawing_render) {
      drawing->render2D();
    }

    if(useTexture) {
      if(!drawing->isTextureInit) {
        drawing->setupTexture(textureFilename);
      }
      drawing->renderTexture();
    }
    
    glfwPollEvents();
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  renderer->clear();
  return EXIT_SUCCESS;
}
