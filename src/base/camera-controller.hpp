#ifndef _SKETCHY_CAMERA_CONTROLLER_
#define _SKETCHY_CAMERA_CONTROLLER_

#include <camera.hpp>

typedef enum {
  NONE, FPS, CAM
} ControllerMode;

class CameraController {
public:
  CameraController(Camera & camera)
  : camera(camera) {}

  inline void setControllerMode(ControllerMode mode) { this->mode = mode; }
  inline ControllerMode getControllerMode() { return this->mode; }

  inline void mouseSetScreenPos(float screenX, float screenY) {
    xPos = screenX;
    yPos = screenY;
  }
  inline void mouseSetLookSpeed(float lookSpeedX, float lookSpeedY) {
    this->lookSpeedX = lookSpeedX;
    this->lookSpeedY = lookSpeedY;
  }
  /*Set mouse position in window space coordinate.*/
  inline void mouseSetMousePos(float mousex, float mousey) {
    mouseX = mousex;
    mouseY = mousey;
  }

  void mouseMakeWorldRay(glm::vec3 & direction, glm::vec3 & startPos, int width, int height);

  void update(double);

  void onZoom(double y);
  
  inline void setMoveUp(bool b) { isMovingUp = b;}
  inline void setMoveDown(bool b) { isMovingDown = b;}
  inline void setMoveRight(bool b) { isMovingRight = b;}
  inline void setMoveLeft(bool b) { isMovingLeft = b;}
  inline void setMoveFront(bool b) { isMovingFront = b;}
  inline void setMoveBack(bool b) { isMovingBack = b;}

  inline void setRotate(bool b) { isRotating = b;}
  inline void setSlide(bool b) { isSliding = b;}

  inline void enableLook(bool b) { isLookEnabled = b; }
  inline void enableMove(bool b) { isMoveEnabled = b; }
  inline void enableCam(bool b) { isCamEnabled = b; }

private:
  Camera & camera;

  ControllerMode mode = ControllerMode::FPS;

  float lookSpeedX = M_PI * 0.0005; // Rad by seconds
  float lookSpeedY = M_PI * 0.0005; // Rad by seconds

  double xPos = 0; // X mouse position in screen coordinate (centered at the screen center)
  double yPos = 0; // Y mouse position in screen coordinate (centered at the screen center)
  double mouseX = 0; // X Mouse position in window coordinate (centered at the top left corner)
  double mouseY = 0; // Y Mouse position in window coordinate (centered at the top left corner)
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

  bool isRotating = false;
  bool isSliding = false;
  bool isCamEnabled = false;

};

#endif
