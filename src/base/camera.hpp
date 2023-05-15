#ifndef _SKETCHY_CAMERA_
#define _SKETCHY_CAMERA_

#include <utils.hpp>

typedef enum {
  PERSPECTIVE, ORTHOGRAPHIC
} CamProjectionType;

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
  inline void setProjectionType(CamProjectionType projType) { m_projType = projType; }
  inline CamProjectionType getProjType() { return m_projType; }
  inline void setOrthoWidth(float orthoWidth) { m_orthoWidth = orthoWidth; }
  inline float getOrthoWidth() { return m_orthoWidth; }
  inline void setOrthoHeight(float orthoHeight) { m_orthoHeight = orthoHeight; }
  inline float getOrthoHeight() { return m_orthoHeight; }

  inline glm::mat4 computeViewMatrix() const {
    return glm::lookAt(m_pos, m_center, UP);
  }

  // Returns the projection matrix stemming from the camera intrinsic parameter.
  inline glm::mat4 computeProjectionMatrix() const {
    switch(m_projType) {
      case CamProjectionType::PERSPECTIVE:
        return glm::perspective(
          glm::radians(m_fov), m_aspectRatio, m_near, m_far
          );
      case CamProjectionType::ORTHOGRAPHIC:
        return glm::ortho(
          -m_orthoWidth/2, m_orthoWidth/2,
          -m_orthoHeight/2, m_orthoHeight/2
          );
    }
    return glm::mat4(1);
  }

  void fpsMoveLook(glm::vec2 angles);
  void fpsMoveForward(float speed);
  void fpsMoveRight(float speed);
  void fpsMoveUp(float speed);

  void camZoomToCenter(float ratio); // ration must be in [0,1] or [-1,0] intervals
  void camRotateArroundCenter(glm::vec2 angles);
  void camSlide(glm::vec2 displacement);

private:
  glm::vec3 m_pos = glm::vec3(0, 0, 0);
  glm::vec3 m_center = glm::vec3(0, 0, 0);
  float m_fov = 45.f;        // Field of view, in degrees
  float m_aspectRatio = 1.f; // Ratio between the width and the height of the image
  float m_near = 0.1f; // Distance before which geometry is excluded from the rasterization process
  float m_far = 10.f; // Distance after which the geometry is excluded from the rasterization process

  CamProjectionType m_projType = CamProjectionType::PERSPECTIVE;

  float m_orthoWidth = 5.0f;
  float m_orthoHeight = 5.0f;

  glm::vec3 UP = glm::vec3(0, 1, 0);
};

#endif
