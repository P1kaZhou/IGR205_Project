#ifndef _GUM3D_CAMERA_
#define _GUM3D_CAMERA_

#include <utils.hpp>

/*
Holds camera data (fov, near, far, etc.) for perspective projectioin camera.
Has methods to move and rotate the camera through time.
*/
class Camera {
public:
  inline float getFov() const { return m_fov; }
  inline void setFoV(const float f) { m_fov = f; }
  inline float getAspectRatio() const { return m_aspectRatio; }
  inline void setAspectRatio(const float a) { m_aspectRatio = a; }
  inline float getNear() const { return m_near; }
  inline void setNear(const float n) { m_near = n; }
  inline float getFar() const { return m_far; }
  inline void setFar(const float n) { m_far = n; }
  inline void setPosition(const glm::vec3 &p) { m_pos = p; }
  inline glm::vec3 getPosition() { return m_pos; }
  inline void setCenter(const glm::vec3 &c) { m_center = c; }
  inline glm::vec3 getCenter() { return m_center; }

  inline glm::mat4 computeViewMatrix() const {
    return glm::lookAt(m_pos, m_center, glm::vec3(0, 1, 0));
  }

  // Returns the projection matrix stemming from the camera intrinsic parameter.
  inline glm::mat4 computeProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
  }

  inline void mouseSetScreenPos(float screenX, float screenY) {
    xPos = screenX;
    yPos = screenY;
  }
  inline void mouseSetLookSpeed(float lookSpeedX, float lookSpeedY) {
    this->lookSpeedX = lookSpeedX;
    this->lookSpeedY = lookSpeedY;
  }

  inline void mouseSetMousePos(float mousex, float mousey) {
    mouseX = mousex;
    mouseY = mousey;
  }

  // inline void mouseMakeWorldRay(glm::vec3 & direction, glm::vec3 & startPos, int width, int height) {
  //   startPos = {xPos/(width*0.5), yPos/(height*0.5), 1};
  //   glm::mat4 inverseViewMat = glm::inverse(computeProjectionMatrix()*computeViewMatrix());
  //   startPos = inverseViewMat*glm::vec4(startPos, 1.0f);
  //   startPos += m_pos;
  //   direction = glm::normalize(startPos-m_pos);
  // }

  inline void mouseMakeWorldRay(glm::vec3 & direction, glm::vec3 & startPos, int width, int height) {
    float xPos = (2.0f*mouseX)/width - 1.0f;
    float yPos = 1.0f-(2.0f*mouseY)/height;
    startPos = {xPos, yPos, -1.0f};
    glm::mat4 inverseViewMat = glm::inverse(computeViewMatrix());
    glm::mat4 inverseProjMat = glm::inverse(computeProjectionMatrix());
    startPos = inverseProjMat*glm::vec4(startPos, 1.0f);
    startPos.z = -1.0f; // Forward in camera space
    startPos = inverseViewMat*glm::vec4(startPos, 1.0f);
    direction = glm::normalize(startPos-m_pos);
  }

  inline void update(long delta) {
    if(isLookEnabled) mouseUpdateLook(delta);
    if(isMoveEnabled) updateMove(delta);
  }

  inline void setMoveUp(bool b) { isMovingUp = b;}
  inline void setMoveDown(bool b) { isMovingDown = b;}
  inline void setMoveRight(bool b) { isMovingRight = b;}
  inline void setMoveLeft(bool b) { isMovingLeft = b;}
  inline void setMoveFront(bool b) { isMovingFront = b;}
  inline void setMoveBack(bool b) { isMovingBack = b;}

  inline void enableLook(bool b) { isLookEnabled = b; }
  inline void enableMove(bool b) { isMoveEnabled = b; }

private:
  glm::vec3 m_pos = glm::vec3(0, 0, 0);
  glm::vec3 m_center = glm::vec3(0, 0, 0);
  float m_fov = 45.f;        // Field of view, in degrees
  float m_aspectRatio = 1.f; // Ratio between the width and the height of the image
  float m_near = 0.1f; // Distance before which geometry is excluded from the rasterization process
  float m_far = 10.f; // Distance after which the geometry is excluded from the rasterization process

  void mouseUpdateLook(long delta);
  void updateMove(long delta);
  
  double xPos = 0;
  double yPos = 0;
  double mouseX = 0;
  double mouseY = 0;
  float lookSpeedX = M_PI * 0.0005; // Rad by seconds
  float lookSpeedY = M_PI * 0.0005; // Rad by seconds
  float noLookZoneWidth = 100.0f;
  float noLookZoneHeight = 100.0f;
  bool isLookEnabled = false;

  bool isMovingUp = false;
  bool isMovingDown = false;
  bool isMovingRight = false;
  bool isMovingLeft = false;
  bool isMovingFront = false;
  bool isMovingBack = false;
  float moveSpeed = 2.5; // Space units by seconds
  bool isMoveEnabled = false;
};

#endif
