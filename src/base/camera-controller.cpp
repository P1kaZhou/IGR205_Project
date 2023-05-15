#include "camera-controller.hpp"

void CameraController::mouseMakeWorldRay(glm::vec3 & direction, glm::vec3 & startPos, int width, int height) {
  float xPos = (2.0f*mouseX)/width - 1.0f;
  float yPos = 1.0f-(2.0f*mouseY)/height;
  startPos = {xPos, yPos, -1.0f};
  glm::mat4 inverseViewMat = glm::inverse(camera.computeViewMatrix());
  glm::mat4 inverseProjMat = glm::inverse(camera.computeProjectionMatrix());
  startPos = inverseProjMat*glm::vec4(startPos, 1.0f);
  startPos.z = -1.0f; // Forward in camera space
  startPos = inverseViewMat*glm::vec4(startPos, 1.0f);
  direction = glm::normalize(startPos-camera.getPosition());
}

void CameraController::update(double delta) {
  if(mode == ControllerMode::FPS) {
    if(isMoveEnabled) {
      if(isMovingUp) {
        camera.fpsMoveUp(delta*0.001*moveSpeed);
      }
      if(isMovingDown) {
        camera.fpsMoveUp(-delta*0.001*moveSpeed);
      }
      if(isMovingFront) {
        camera.fpsMoveForward(delta*0.001*moveSpeed);
      }
      if(isMovingBack) {
        camera.fpsMoveForward(-delta*0.001*moveSpeed);
      }
      if(isMovingRight) {
        camera.fpsMoveRight(delta*0.001*moveSpeed);
      }
      if(isMovingLeft) {
        camera.fpsMoveRight(-delta*0.001*moveSpeed);
      }
    }

    if(isLookEnabled) {
      if(glm::abs(xPos) > noLookZoneWidth || glm::abs(yPos) > noLookZoneHeight) {
        camera.fpsMoveLook({
          xPos*lookSpeedX*0.01,
          yPos*lookSpeedY*0.01
        });
      }
    }
  }
  else if(mode == ControllerMode::CAM) {
    if(isLookEnabled) {
      if(isRotating) {
        if(glm::abs(xPos) > noLookZoneWidth || glm::abs(yPos) > noLookZoneHeight) {
          camera.camRotateArroundCenter({
            xPos*lookSpeedX*0.1,
            -yPos*lookSpeedY*0.1
          });
        }
      }
      if(isSliding) {
        if(glm::abs(xPos) > noLookZoneWidth || glm::abs(yPos) > noLookZoneHeight) {
          camera.camSlide({
            xPos*lookSpeedX*0.1,
            -yPos*lookSpeedY*0.1
          });
        }
      }
    }
  }
}

void CameraController::onZoom(double offset) {
  camera.camZoomToCenter(offset*0.1);
}
