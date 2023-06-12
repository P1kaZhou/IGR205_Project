#include "base.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define _USE_MATH_DEFINES

void Renderer::initGLFW(
  int width, int height,
  WindowSizeCallback windowSizeCallback,
  KeyCallback keyCallback,
  ErrorCallback errorCallback,
  CursorPositionCallback cursorPositionCallback,
  CursorEnterCallback cursorEnterCallback,
  MouseButtonCallback mouseButtonCallback,
  MouseScrollCallback mouseScrollCallback
) {
  glfwSetErrorCallback(errorCallback);

  // Initialize GLFW, the library responsible for window management
  if(!glfwInit()) {
    std::cerr << "ERROR: Failed to init GLFW" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Before creating the window, set some option flags
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  // Create the window
  this->width = width;
  this->height = height;
  g_window = glfwCreateWindow(
    width, height,
    "Interactive 3D Applications (OpenGL) - Simple Solar System",
    nullptr, nullptr);
  if(!g_window) {
    std::cerr << "ERROR: Failed to open window" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  // Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
  glfwMakeContextCurrent(g_window);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetCursorPosCallback(g_window, cursorPositionCallback);
  glfwSetKeyCallback(g_window, keyCallback);
  glfwSetCursorEnterCallback(g_window, cursorEnterCallback);
  glfwSetMouseButtonCallback(g_window, mouseButtonCallback);
  glfwSetScrollCallback(g_window, mouseScrollCallback);
}

void Renderer::initOpenGL() {
  // Load extensions for modern OpenGL
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void Renderer::initCamera() {
  glfwGetWindowSize(g_window, &width, &height);
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));

  g_camera.setPosition(glm::vec3(0.0, 0.0, 3.0));
  g_camera.setNear(0.1);
  g_camera.setFar(80.1);
}

void Renderer::clear() {
  glfwDestroyWindow(g_window);
  glfwTerminate();
  for (unsigned i = 0; i < renderables.size(); i++){
    if(renderables[i]) {
      delete renderables[i];
    }
  }
  for (unsigned i = 0; i < lights.size(); i++){
    if(lights[i]) {
      delete lights[i];
    }
  }
}

// static int photo = 0;
// static int photoId = 0;

void Renderer::render() {
  GLuint singleColorProgram = Program::getInstanceSingleColor()->getProgram();
  GLuint shadowProgram = Program::getInstanceShadowMap()->getProgram();
  GLuint phongProgram = Program::getInstancePhong()->getProgram();

  glViewport(0, 0, width, height);
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Some inits for lights
  for (unsigned i = 0; i < lights.size(); i++){
    if(!lights[i]->uniformNamePos) {
      lights[i]->init(width, height);
      lights[i]->buildUniformNames(i);
    }
  }

  // Some inits for renderables
  for (unsigned i = 0; i < renderables.size(); i++) {
      auto m = renderables[i];
      if(!m->isMeshhInitialized()) {
        std::cout << "Init \"" << m->name << "\"" << std::endl;
        m->init();
      }
  }

  // Shadow maps rendering
  for (unsigned i = 0; i < lights.size(); i++){
    if(lights[i]->isShadowCaster) {
      lights[i]->bindDepthFrameBuffer();
      glUseProgram(shadowProgram);
      glCullFace(GL_BACK);
      glEnable(GL_CULL_FACE);
      lights[i]->updateFromLightDepthUniforms();

      for (unsigned j = 0; j < renderables.size(); j++) {
        auto m = renderables[j];
        m->render(*this, shadowProgram);
      }

      // if(photo == 200) {
      //   lights[i]->depthFrameBuffer.saveDepthTexturePPM(std::string("lightDepthImage")+std::to_string(photoId)+std::string(".ppm"));
      //   // lights[i]->depthFrameBuffer.saveColorTexturePPM(std::string("lightColorImage")+std::to_string(i)+std::string(".ppm"));
      //   photoId += 1;
      // }
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // photo += 1;

  // Render other objects
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_STENCIL_TEST);
  glCullFace(GL_BACK);
  if(withFaceCull) {
    glEnable(GL_CULL_FACE);
  }
  else {
    glDisable(GL_CULL_FACE);
  }
  glUseProgram(phongProgram);
  for (unsigned i = 0; i < renderables.size(); i++) {
      auto m = renderables[i];
      if(!m->isHighlightable || !m->isHighlighted) m->render(*this, phongProgram);
  }
  
  // Fill stencil buffer with ones for highlighted objects
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Replace with ref value when depth and stencil test pass
  glStencilFunc(GL_ALWAYS, 1 /*Ref value*/, 0xFF);
  glUseProgram(phongProgram);
  for (unsigned i = 0; i < renderables.size(); i++) {
      auto m = renderables[i];
      if(m->isHighlightable && m->isHighlighted) m->render(*this, phongProgram);
  }

  // Hightlight objects to highlight
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glDisable(GL_DEPTH_TEST);
  glUseProgram(singleColorProgram);
  glm::vec4 heighLightColor = {0.1f, 0.1f, 0.8f, 1.0f};
  glUniform4f(glGetUniformLocation(singleColorProgram, "singleColor"),
    heighLightColor.r, heighLightColor.g, heighLightColor.b, heighLightColor.a);
  for (unsigned i = 0; i < renderables.size(); i++) {
      auto m = renderables[i];
      if(m->isHighlightable && m->isHighlighted) m->render(*this, singleColorProgram, glm::scale(glm::vec3(1.1f, 1.1f, 1.1f)));
  }
}

void Renderer::saveCurrentImageTGA(const std::string & filepath) {
  const short int w = width;
  const short int h = height;
  std::vector<int> buffer(w*h*3, 0);

  glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, &(buffer[0]));
  
  FILE * out = fopen(filepath.c_str(), "wb");
  short TGAhear[] = {0, 2, 0, 0, 0, 0, w, h, 24};
  fwrite(&TGAhear, sizeof(TGAhear), 1, out);
  fwrite(&(buffer[0]), 3*w*h, 1, out);
  fclose(out);
}
