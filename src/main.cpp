
#include <base.hpp>
#include <mesh.hpp>
#include <animation.hpp>
#include <camera-controller.hpp>

#include <drawing.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <modeling/delaunay.hpp>
#include <modeling/medial-axis.hpp>
#include <modeling/medial-axis-generator.hpp>
#include <modeling/cylindrical-douglas-peucker.hpp>
#include <modeling/cylinder-generator.hpp>

#include <geometry/draw-2d.hpp>


Renderer * renderer = nullptr;
Mesh * selectedMesh = nullptr;
CameraController * camController;
Drawing * drawing;

Mesh * generatedMesh = nullptr;

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

ControllerMode controllerMode = ControllerMode::FPS;

bool drawing_render_3d = false;

int im_resolution_w = 400;
int im_resolution_h = 400;

glm::vec3 chordColor = {0, 200, 0};
glm::vec3 axisColor = {255, 255, 255};
glm::vec3 axisPointColor = {0, 200, 200};
glm::vec3 shapeColor = {150, 0, 0};
glm::vec3 shapePointColor = {0, 0, 200};
glm::vec3 skeletonPointColor = {255, 0, 0};
glm::vec3 skeletonColor = {255, 255, 255};

float cdp_threshold = 0.5;
float importanceCylindricalError = 1.0f;
float importanceDistanceError = 1.0f;

int ma_prun_depth = 15;
int ma_prun_count = 3;

void testPipeline(
  std::vector<glm::vec2> & points
) {


  // Delaunay constrained triangulation
  ConstrainedDelaunayTriangulation2D d(points);
  auto triangles = d.getTriangles();
  auto chords = d.getEdges();
  {
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.drawTriangles(points, triangles, chordColor);
    builder.drawShape(true, points, shapeColor);
    builder.drawPoints(points, shapePointColor);
    builder.save("01-delaunay.ppm");
  }

  // The raw medial axis
  MedialAxisGenerator medial(points, triangles);
  // medial.compute();
  auto midPoints = medial.computeMidPoints();
  for(unsigned i=0; i<2; i++){
    if(i==1) {
      for(int ii=1; ii<=ma_prun_count; ii++) {
        // medial.pruning(ma_prun_depth*(ii/ma_prun_count));
        medial.pruning(ma_prun_depth);
      }
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
    builder.addExtraPoints(points);
    builder.addExtraPoints(pointsSeg);
    builder.drawSegments(segments, axisColor);
    builder.drawPoints(pointsSeg, axisPointColor);
    builder.drawShape(true, points, shapeColor);
    builder.drawPoints(points, shapePointColor);
    if(i==0) builder.save("02-medial-axis.ppm");
    if(i==1) builder.save("02-medial-axis-pruning.ppm");
  }
  // External axis of the medial axis tree
  auto externalAxis = medial.extractExternalAxis();
  {
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.setExtraPoints(points);
    for(auto ax : externalAxis) {
      builder.drawShape(false, ax, axisColor);
      builder.drawPoints(ax, axisPointColor);
    }
    builder.drawShape(true, points, shapeColor);
    builder.drawPoints(points, shapePointColor);
    builder.save("03-external-medial-axis.ppm");
  }
  // Internal axis of the medial axis tree
  auto internalAxis = medial.extractInternalAxis();
  {
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.setExtraPoints(points);
    for(auto ax : internalAxis) {
      builder.drawShape(false, ax, axisColor);
      builder.drawPoints(ax, axisPointColor);
    }
    builder.drawShape(true, points, shapeColor);
    builder.drawPoints(points, shapePointColor);
    builder.save("03-internal-medial-axis.ppm");
  }

  // Skeleton
  for(auto & skelAxis : externalAxis) {
    skelAxis.erase(skelAxis.begin()+skelAxis.size()-1);
  }
  CDP cdp(points, externalAxis, chords,
    cdp_threshold, importanceCylindricalError, importanceDistanceError);
  cdp.compute();
  auto skeleton = cdp.getSkeleton();
  {
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.addExtraPoints(points);
    builder.drawEdges(points, chords, chordColor);
    for(auto ax : externalAxis) {
      builder.drawShape(false, ax, axisColor);
      builder.drawPoints(ax, axisPointColor);
    }
    for(auto skel : skeleton) {
      builder.drawPoints(skel, skeletonPointColor);
    }
    builder.drawShape(true, points, shapeColor);
    builder.drawPoints(points, shapePointColor);
    builder.save("04-skeleton.ppm");
  }
  {
    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.addExtraPoints(points);
    for(auto skel : skeleton) {
      builder.drawShape(false, skel, skeletonColor);
      builder.drawPoints(skel, skeletonPointColor);
    }
    builder.drawShape(true, points, shapeColor);
    builder.drawPoints(points, shapePointColor);
    builder.save("04-skeleton-final.ppm");
  }
}

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

    // ImGui::Text("This is some useful text.");
    // ImGui::Checkbox("Demo Window", &show_demo_window);
    // ImGui::Checkbox("Another Window", &show_another_window);

    // ImGui::SliderFloat("C Douglas-Peucker threshold", &cdp_threshold, 0.0f, 1.0f);
    // ImGui::ColorEdit3("clear color", (float*)&clear_color);

    // if (ImGui::Button("Button"))
    //     counter++;
    // ImGui::SameLine();
    // ImGui::Text("counter = %d", counter);

    // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    if (ImGui::RadioButton("FIXED", controllerMode == NONE)) { controllerMode = NONE; } ImGui::SameLine();
    if (ImGui::RadioButton("FPS", controllerMode == FPS)) { controllerMode = FPS; } ImGui::SameLine();
    if (ImGui::RadioButton("CAM", controllerMode == CAM)) { controllerMode = CAM; }

    ImGui::Checkbox("3D drawing", &drawing_render_3d);

    // Cylindrical Douglas-Peucker threshold
    ImGui::SliderFloat("CDP threshold", &cdp_threshold, 0.0f, 2.0f);
    ImGui::SliderFloat("CDP c error", &importanceCylindricalError, 0.0f, 1.0f);
    ImGui::SliderFloat("CDP d error", &importanceDistanceError, 0.0f, 1.0f);
    ImGui::SliderInt("prunning thresh", &ma_prun_depth, 3, 60);
    ImGui::SliderInt("nbr of prunning", &ma_prun_count, 1, 10);

    ImGui::SliderInt("resolution w", &im_resolution_w, 100, 2000);
    ImGui::SliderInt("resolution h", &im_resolution_h, 100, 2000);

    // Perform skeleton computation on shape
    if (ImGui::Button("Medial axis")) {
      std::vector<glm::vec2> points;
      for(auto p : drawing->getDrawing(drawing->drawingCount()-1)) {
        points.emplace_back(p.x, p.y);
      }
      testPipeline(points);
    }

    // if (controllerMode == FPS) { ImGui::Text("FPS"); }
    // if (controllerMode == CAM) { ImGui::Text("CAM"); }

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

  initImGui();

  // { // Test of Constrained Delaunay Triangulation 
  //   std::vector<glm::vec2> points;
  //   points.push_back({1, 1});
  //   points.push_back({1, 0});
  //   points.push_back({0, 0});
  //   points.push_back({0, 1});
  //   points.push_back({0.5, 0.5});
  //   ConstrainedDelaunayTriangulation2D d(points);
  //   auto triangles = d.getTriangles();
  //   auto edges = d.getEdges();

  //   Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
  //   builder.drawTriangles(points, triangles, chordColor);
  //   builder.drawShape(true, points, shapeColor);
  //   builder.drawPoints(points, shapePointColor);
  //   builder.save("01-delaunay.ppm");
  // }

  { // Test medial axis
    std::vector<glm::vec2> points;
    // 0 100 200 300 400 400 400 400 300 200 100 0
    // 0 0 0 0 0 100 200 300 300 300 300 300
    // points.push_back({0, 0});
    // points.push_back({1, 0});
    // points.push_back({2, 0});
    // points.push_back({3, 0});
    // points.push_back({4, 0});
    // points.push_back({4, 1});
    // points.push_back({4, 2});
    // points.push_back({4, 3});
    // points.push_back({3, 3});
    // points.push_back({2, 3});
    // points.push_back({1, 3});
    // points.push_back({0, 3});

    // 200 300 300 300 400 500 500 400 300 300 300 200 200 200 100 0 0 100 200 200
    // 0 0 100 200 200 200 300 300 300 400 500 500 400 300 300 300 200 200 200 100
    points.push_back({2, 0});
    points.push_back({3, 0});
    points.push_back({3, 1});
    points.push_back({3, 2});
    points.push_back({4, 2});
    points.push_back({5, 2});
    points.push_back({5, 3});
    points.push_back({4, 3});
    points.push_back({3, 3});
    points.push_back({3, 4});
    points.push_back({3, 5});
    points.push_back({2, 5});
    points.push_back({2, 4});
    points.push_back({2, 3});
    points.push_back({1, 3});
    points.push_back({0, 3});
    points.push_back({0, 2});
    points.push_back({1, 2});
    points.push_back({2, 2});
    points.push_back({2, 1});

    ConstrainedDelaunayTriangulation2D d(points);
    auto triangles = d.getTriangles();
    auto edges = d.getEdges();

    {
      Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
      builder.drawTriangles(points, triangles, chordColor);
      // builder.drawEdges(points, edges,  chordColor);
      builder.drawShape(true, points, shapeColor);
      builder.drawPoints(points, shapePointColor);
      builder.save("01-delaunay.ppm");
    }

    MedialAxisGenerator medial(points, triangles);
    // medial.compute();
    auto midPoints = medial.computeMidPoints();
    auto axis = medial.getMedialAxis();
    std::vector<std::pair<glm::vec2, glm::vec2>> segments;
    for(auto ax : axis.getPoints()) {
      for(auto s : ax->getAdjs()) {
        segments.emplace_back(ax->getPoint(), s->getPoint());
      }
    }

    {
      Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
      builder.setExtraPoints(points);
      builder.drawSegments(segments, axisColor);
      builder.drawShape(true, points, shapeColor);
      builder.drawPoints(points, shapePointColor);
      builder.save("02-medial-axis.ppm");
    }

    auto external = medial.extractExternalAxis();
    {
      Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
      builder.setExtraPoints(points);
      for(auto ax : external) {
        builder.drawShape(false, ax, axisColor);
      }
      builder.drawTriangles(points, triangles, chordColor);
      builder.drawShape(true, points, shapeColor);
      builder.drawPoints(points, shapePointColor);
      builder.save("03-external-medial-axis.ppm");
    }

    std::cout <<  "edge count : " << edges.size() << std::endl;
    CylinderGenerator cylGen(external[1], points, edges);
    cylGen.compute(50);
    // cylGen.showVertices();
    // cylGen.showFaces();
    MeshGeometry * geometry = new MeshGeometry(cylGen.getVertexPos(), cylGen.getFaces());
    generatedMesh = new Mesh(
      geometry,
      MeshMaterial::meshGetBasicMaterial({0.5, 0.5, 0})
    );
    renderer->addRenderable(generatedMesh);
  }

  { // Test Cylindrical Douglas Peucker
    std::vector<glm::vec2> points;
    points.push_back({0, 0});
    points.push_back({1, 0});
    points.push_back({2, 0});
    points.push_back({3, 1});
    points.push_back({4, 1});
    points.push_back({5, 0});
    points.push_back({6, 0});
    points.push_back({6, 4});
    points.push_back({5, 4});
    points.push_back({4, 3});
    points.push_back({3, 3});
    points.push_back({2, 4});
    points.push_back({1, 4});
    points.push_back({0, 4});
    std::vector<std::vector<glm::vec2>> axis;
    std::vector<glm::vec2> ax;
    ax.push_back({0.5, 2});
    ax.push_back({1.5, 2});
    ax.push_back({2.5, 2});
    ax.push_back({3.5, 2});
    ax.push_back({4.5, 2});
    ax.push_back({5.5, 2});
    axis.push_back(ax);
    std::vector<Geometry::Edge> chords;
    chords.push_back(Geometry::Edge(1,12));
    chords.push_back(Geometry::Edge(2,11));
    chords.push_back(Geometry::Edge(3,10));
    chords.push_back(Geometry::Edge(4,9));
    chords.push_back(Geometry::Edge(5,8));

    CDP cdp(points, axis, chords, 1);
    cdp.compute();
    auto skeleton = cdp.getSkeleton();

    Geometry::DrawBuilder builder(im_resolution_w, im_resolution_h);
    builder.addExtraPoints(points);
    builder.drawEdges(points, chords, chordColor);
    builder.drawShape(false, axis[0], axisColor);
    builder.drawPoints(axis[0], axisPointColor);
    builder.drawShape(true, points, shapeColor);
    builder.drawPoints(points, shapePointColor);
    builder.drawPoints(skeleton[0], skeletonPointColor);
    builder.save("04-skeleton.ppm");


    // CylinderGenerator cylGen(axis[0], points, chords);
    // cylGen.compute(50);
    // cylGen.showVertices();
    // cylGen.showFaces();
    // MeshGeometry * geometry = new MeshGeometry(cylGen.getVertexPos(), cylGen.getFaces());
    // generatedMesh = new Mesh(
    //   geometry,
    //   MeshMaterial::meshGetBasicMaterial({0.5, 0.5, 0})
    // );
    // renderer->addRenderable(generatedMesh);
  }

  long delta = 0;
  while(!glfwWindowShouldClose(renderer->getWindow())) {
    glfwSwapBuffers(renderer->getWindow());

    delta = renderer->updateDeltaTime();
    renderer->render();

    renderImGui();
    interactImGui();
    camController->update(delta);

    if(drawing_render_3d) {
      drawing->render3D(
        renderer->getCamera().computeProjectionMatrix(),
        renderer->getCamera().computeViewMatrix()
      );
    }
    else {
      drawing->render2D();
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
