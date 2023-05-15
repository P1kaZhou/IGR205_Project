
#include <base.hpp>
#include <mesh.hpp>
#include <animation.hpp>
#include <camera-controller.hpp>

#include <drawing.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

Mesh * meshLight1 = nullptr;
Mesh * meshLight2 = nullptr;
Mesh * meshPlane1 = nullptr;

Mesh * meshModel1 = nullptr;
Mesh * meshModel2 = nullptr;
Mesh * meshModel3 = nullptr;
Mesh * meshModel4 = nullptr;
Mesh * meshModel5 = nullptr;
Mesh * meshModel6 = nullptr;
Mesh * meshModel7 = nullptr;
Mesh * meshModel8 = nullptr;
Mesh * meshModel9 = nullptr;

Mesh * meshBasketball = nullptr;
void createTest() {
  // Lights
  {
    renderer->addLight(Light::lightGetConstantCaster(
      {0.0f, 3.5f, 0.0f}, {0.1f, 0.1f, 0.1f}, {0.8f, 0.8f, 0.8f}, {0.8f, 0.8f, 0.8f}
    ));
    renderer->addLight(Light::lightGetConstantCaster(
      {1.0f, 3.5f, 0.0f}, {0.1f, 0.1f, 0.1f}, {0.8f, 0.8f, 0.8f}, {0.8f, 0.8f, 0.8f}
    ));
  }
  
  // Lights position Spheres
  {
    meshLight1 = new Mesh(
      MeshGeometry::meshGetSphereData(0.1f, 70, 80, nullptr),
      MeshMaterial::meshGetBasicMaterial(COLOR_WHITE)
    );
    meshLight1->translateMesh({0.0f, 3.5f, 0.0f});
    renderer->addRenderable(meshLight1);
    meshLight2 = new Mesh(
      MeshGeometry::meshGetSphereData(0.1f, 70, 80, nullptr),
      MeshMaterial::meshGetBasicMaterial(COLOR_WHITE)
    );
    meshLight2->translateMesh({1.0f, 3.5f, 0.0f});
    renderer->addRenderable(meshLight2);
  }
  
  // world center
  {
    Mesh * meshCenter = new Mesh(
      MeshGeometry::meshGetSphereData(0.1f, 70, 80, nullptr),
      MeshMaterial::meshGetBasicMaterial(COLOR_BLACK)
    );
    renderer->addRenderable(meshCenter);

    meshPlane1 = new Mesh(
      MeshGeometry::meshGetPlaneData(1.0f, 1.0f, nullptr),
      MeshMaterial::meshGetSimplePhongMaterial(COLOR_PINK, COLOR_PINK, 1)
    );
    renderer->addRenderable(meshPlane1);
  }

  {
    // Model 1
    meshModel1 = new Mesh(
      MeshGeometry::meshGetFromOfffile("media/models/geometry_triangle/Apple.off"),
      MeshMaterial::meshGetSimplePhongMaterial(COLOR_RED, COLOR_RED, 2)
    );
    meshModel1->name = "Apple";
    renderer->addRenderable(meshModel1);
    // Model 2
    meshModel2 = new Mesh(
      MeshGeometry::meshGetFromOfffile("media/models/geometry_triangle/cone.off"),
      MeshMaterial::meshGetSimplePhongMaterial(COLOR_ORANGE, COLOR_ORANGE, 2)
    );
    meshModel2->name = "Cone";
    renderer->addRenderable(meshModel2);
    // Model 3
    meshModel3 = new Mesh(
      MeshGeometry::meshGetFromOfffile("media/models/geometry_triangle/epcot.off"),
      MeshMaterial::meshGetSimplePhongMaterial(COLOR_GREEN, COLOR_GREEN*0.1f, 2)
    );
    meshModel3->name = "Epcot";
    renderer->addRenderable(meshModel3);
    // Model 4
    meshModel4 = new Mesh(
      MeshGeometry::meshGetFromOfffile("media/models/geometry_triangle/icosahedron.off"),
      MeshMaterial::meshGetSimplePhongMaterial(COLOR_WHITE, COLOR_WHITE*0.1f, 2)
    );
    meshModel4->name = "Icosahedron";
    renderer->addRenderable(meshModel4);
    // Model 5
    meshModel5 = new Mesh(
      MeshGeometry::meshGetFromOfffile("media/models/geometry_triangle/octahedron.off"),
      MeshMaterial::meshGetSimplePhongMaterial(COLOR_BLUE, COLOR_BLUE*0.1f, 2)
    );
    meshModel5->name = "Octahedron";
    renderer->addRenderable(meshModel5);
    // Model 6
    meshModel6 = new Mesh(
      MeshGeometry::meshGetFromOfffile("media/models/geometry_triangle/seashell.off"),
      MeshMaterial::meshGetSimplePhongMaterial(COLOR_YELLOW, COLOR_YELLOW, 2)
    );
    meshModel6->name = "Seashell";
    renderer->addRenderable(meshModel6);
    // Model 7
    meshModel7 = new Mesh(
      MeshGeometry::meshGetFromOfffile("media/models/geometry_triangle/Sword01.off"),
      MeshMaterial::meshGetSimplePhongMaterial(COLOR_GRAY, COLOR_GRAY, 2)
    );
    meshModel7->name = "Sword01";
    renderer->addRenderable(meshModel7);
    // Model 8
    meshModel8 = new Mesh(
      MeshGeometry::meshGetFromOfffile("media/models/geometry_triangle/socket.off"),
      MeshMaterial::meshGetSimplePhongMaterial(COLOR_WHITE, COLOR_WHITE, 2)
    );
    meshModel8->name = "Socket";
    renderer->addRenderable(meshModel8);
    // Model 9
    meshModel9 = new Mesh(
      MeshGeometry::meshGetFromOfffile("media/models/geometry_triangle/tetra.off"),
      MeshMaterial::meshGetSimplePhongMaterial(COLOR_PURPLE, COLOR_PURPLE, 2)
    );
    meshModel9->name = "Tetra";
    renderer->addRenderable(meshModel9);
  }

  // Mesh with diffuse, specular and normal maps.
  {
    meshBasketball = new Mesh(
      MeshGeometry::meshGetSphereData(1.0f, 70, 70, nullptr),
      MeshMaterial::meshGetTextureMaterial("media/earth.jpg", "media/earth_specular.jpg", "media/earth_normal.jpg", COLOR_BLACK)
    );
    meshBasketball->name = "Basketball";
    renderer->addRenderable(meshBasketball);
  }

  // Child-Parent relationships
  meshModel1->setParent(meshModel3);
  meshModel2->setParent(meshModel3);
  // Positions
  meshPlane1->translateMesh({0.0f, -2.0f, 0.0f});
  meshModel1->translateMesh({-2.0f, 0.0f, 2.0f});
  meshModel2->translateMesh({2.0f, 0.0f, 2.0f});
  meshModel3->translateMesh({0.0f, 2.0f, 2.0f});
  meshModel4->translateMesh({0.0f, 2.0f, -2.0f});
  meshModel5->translateMesh({4.0f, 0.0f, 0.0f});
  meshModel6->translateMesh({-2.0f, 0.0f, 0.0f});
  meshModel7->translateMesh({2.0f, 1.0f, 1.0f});
  meshModel8->translateMesh({0.0f, 1.0f, 0.0f});
  meshModel9->translateMesh({-1.0f, 1.0f, -2.0f});
  meshBasketball->translateMesh({-4.0f, 0.0f, 0.0f});
  // Scales
  meshModel5->scaleMesh({2.0f, 2.0f, 2.0f});
  meshPlane1->scaleMesh({16.0f, 1.0f, 16.0f});
}

Animation * animation1 = nullptr;
Animation * animation2 = nullptr;
Animation * animation3 = nullptr;
Animation * animation4 = nullptr;
void initAnimations() {
  std::vector<AnimationFrame> frames;
  auto frame = AnimationFrame();
  
  // Animation 1
  animation1 = new Animation();
  frame = AnimationFrame();
  frame.position = {0.0f, 0.0f, 0.0f};
  frame.time = 0;
  frames.push_back(frame);
  frame.position = {3.0f, 0.0f, 0.0f};
  frame.time = 2000;
  frames.push_back(frame);
  frame.position = {0.0f, 0.0f, 0.0f};
  frame.time = 4000;
  frames.push_back(frame);
  animation1->setFrames(frames);
  animation1->setMesh(meshModel5);
  animation1->setIsLoop(true);
  frames.clear();

  // Animation 2
  animation2 = new Animation();
  frame = AnimationFrame();
  auto pos6 = meshModel6->getPosition();
  frame.position = pos6;
  frame.scale = {0.5f, 0.5f, 0.5f};
  frame.time = 0;
  frames.push_back(frame);
  frame.scale = {1.0f, 1.0f, 1.0f};
  frame.time = 2000;
  frames.push_back(frame);
  animation2->setFrames(frames);
  animation2->setMesh(meshModel6);
  animation2->setIsLoop(true);
  frames.clear();

  // Animation 3
  animation3 = new Animation();
  frame = AnimationFrame();
  auto pos3 = meshModel3->getPosition();
  frame.position = pos3;
  frame.orientation = {0.0f, 0.0f, 0.0f};
  frame.time = 0;
  frames.push_back(frame);
  frame.orientation = {0.0f, 2*M_PI, 0.0f};
  frame.time = 10000;
  frames.push_back(frame);
  animation3->setFrames(frames);
  animation3->setMesh(meshModel3);
  animation3->setIsLoop(true);
  frames.clear();

  long time = getTimeMillis();
  animation1->start(time);
  animation2->start(time);
  animation3->start(time);
}
void animateAnimations() {
  long time = getTimeMillis();
  animation1->animate(time);
  animation2->animate(time);
  animation3->animate(time);
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

    // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
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

  createTest();
  initAnimations();
  
  long delta = 0;
  while(!glfwWindowShouldClose(renderer->getWindow())) {
    glfwSwapBuffers(renderer->getWindow());

    delta = renderer->updateDeltaTime();
    animateAnimations();
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
