#ifndef _SKETCHY_BASE_
#define _SKETCHY_BASE_

#include <utils.hpp>
#include <camera.hpp>
#include <light.hpp>
#include <frame-buffer.hpp>
#include <geometry/geometry.hpp>

typedef void (*WindowSizeCallback)(GLFWwindow* window, int width, int height);
typedef void (*KeyCallback)(GLFWwindow* window, int key, int scancode, int action, int mods);
typedef void (*ErrorCallback)(int error, const char *desc);
typedef void (*CursorPositionCallback)(GLFWwindow* window, double xpos, double ypos);
typedef void (*CursorEnterCallback)(GLFWwindow* window, int entered);
typedef void (*MouseButtonCallback)(GLFWwindow* window, int button, int action, int mods);
typedef void (*MouseScrollCallback)(GLFWwindow* window, double xoffset, double yoffset);

class Renderer;

/*
Hold any renderable model.
*/
class Renderable {
  friend class Renderer;
public:
  /*Init GPU buffers.*/
  virtual void init() = 0;

  /*Render the model.*/
  virtual void render(Renderer & renderer, GLuint program=0,
    const glm::mat4 & extraTransformFirst = glm::mat4(1),
    const glm::mat4 & extraTransformLast = glm::mat4(1)) = 0;

  virtual bool isMeshhInitialized() = 0;

  virtual const Geometry::BoundingBox::BoundingBox & getBoundingBox() = 0;

  virtual glm::mat4 getWorldTransform() = 0;

  inline void setHighLight(bool v) { isHighlighted = v;}
  inline bool getIsHighLighted() const { return isHighlighted; }

  inline void setHighLightable(bool v) { isHighlightable = v;}
  inline bool getIsHighLightable() const { return isHighlightable; }

  inline bool setDepthTest(bool v) {withDepthTest=v;}
  inline bool depthTest() {return withDepthTest;}

  std::string name;

private:
  bool isHighlighted = false;
  bool isHighlightable = true;
  bool withDepthTest = true;
};

/*
Hold scene data (meshes, lights, camera, etc.)
And methods to initialise window and OpenGL context.
*/
class Renderer {
protected:
  // Window parameters
  GLFWwindow *g_window = nullptr;

  // Basic camera model
  Camera g_camera;

  std::vector<Renderable*> renderables;
  std::vector<Light*> lights;

  long currentTime = getTimeMillis();
  long delta = 0;

  int width = 0;
  int height = 0;

  bool withFaceCull = true;

public:

  inline void faceCulling(bool v) { withFaceCull = v; }

  inline Camera & getCamera() { return g_camera; }
  inline GLFWwindow * getWindow() { return g_window; }
  inline long updateDeltaTime() {
    delta = getTimeMillis() -  currentTime;
    currentTime = getTimeMillis();
    return delta;
  }

  inline Renderable * getRayIntersection(glm::vec3 rayDirection, glm::vec3 rayStartPos) const {
    for(auto object : renderables) {
      if(object->isHighlightable) {
        bool intersect = Geometry::BoundingBox::intersectLine(object->getBoundingBox(), rayDirection, rayStartPos, object->getWorldTransform());
        if(intersect) {
          return object;
        }
      }
    }
    return nullptr;
  }

  inline void addRenderable(Renderable * r) { renderables.push_back(r); }
  inline void removeRenderable(Renderable * r) {
    unsigned index = 0;
    bool found = false;
    for(unsigned i=0; i<renderables.size() && !found; i++) {
      if(renderables[i]==r) {
        index = i;
        found = true;
      }
    }
    if(found) {
      renderables.erase(renderables.begin()+index);
      delete r;
    }
  }
  inline void addLight(Light * l) { lights.push_back(l); }
  inline const std::vector<Renderable*> & getRenderables() { return renderables; }
  inline const std::vector<Light*> & getLights() { return lights; }

  void initGLFW(
    int width, int height,
    WindowSizeCallback windowSizeCallback,
    KeyCallback keyCallback,
    ErrorCallback errorCallback,
    CursorPositionCallback cursorPositionCallback,
    CursorEnterCallback cursorEnterCallback,
    MouseButtonCallback mouseButtonCallback,
    MouseScrollCallback mouseScrollCallback
  );
  void initOpenGL();
  void initCamera();
  void clear();

  void render();

  void saveCurrentImageTGA(const std::string & filepath);
};

#endif
