#include <camera.hpp>

void Camera::mouseUpdateLook(long delta) {
    glm::vec3 center3 = m_center;
    glm::vec4 center4 = glm::vec4(center3.x, center3.y, center3.z, 0.0f);
    
    float angleX = lookSpeedX * delta * 0.001f * (xPos);
    float angleY = lookSpeedY * delta * 0.001f * (yPos);

    angleX *= -1.f; // to have a clock wise rotation for positive values of xPos (so angleX)

    auto yAxe = glm::cross(center3-m_pos, {0.0f, 1.0f, 0.0f});
    auto matX = glm::rotate(glm::mat4(1), angleX, {0.0f, 1.0f, 0.0f});
    auto matY = glm::rotate(glm::mat4(1), angleY, yAxe);

    center4 = center4 - glm::vec4(m_pos, 0.0); // Camera to center
    if(glm::abs(xPos) > noLookZoneWidth)
        center4 = matX * center4; // Rotate X
    if(glm::abs(yPos) > noLookZoneHeight)
        center4 = matY * center4; // Rotate Y
    center4 = center4 + glm::vec4(m_pos, 0.0); // Camera to its position

    setCenter({center4.x, center4.y, center4.z});
}

void Camera::updateMove(long delta) {
    float d = moveSpeed  * delta * 0.001f;
    if(isMovingUp) {
        m_pos += glm::vec3(0, d, 0);
        m_center += glm::vec3(0, d, 0);
    }
    if(isMovingDown) {
        m_pos += glm::vec3(0, -d, 0);
        m_center += glm::vec3(0, -d, 0);
    }
    if(isMovingFront) {
        glm::vec3 look = m_center - m_pos;
        glm::vec3 front = glm::normalize( glm::vec3(look.x, 0.0f, look.z) ); // Projection on the orizontal plane
        front = front * d;
        m_pos += front;
        m_center += front;
    }
    if(isMovingBack) {
        glm::vec3 look = m_center - m_pos;
        glm::vec3 front = glm::normalize( glm::vec3(look.x, 0.0f, look.z) ); // Projection on the orizontal plane
        front = -front * d;
        m_pos += front;
        m_center += front;
    }
    if(isMovingRight) {
        glm::vec3 look = m_center - m_pos;
        glm::vec3 front = glm::normalize( glm::vec3(look.x, 0.0f, look.z) ); // Projection on the orizontal plane
        glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
        glm::vec3 v = glm::normalize( glm::cross(front, up) );
        v = v * d;
        m_pos += v;
        m_center += v;
    }
    if(isMovingLeft) {
        glm::vec3 look = m_center - m_pos;
        glm::vec3 front = glm::normalize( glm::vec3(look.x, 0.0f, look.z) ); // Projection on the orizontal plane
        glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
        glm::vec3 v = glm::normalize( glm::cross(front, up) );
        v = -v * d;
        m_pos += v;
        m_center += v;
    }
}
