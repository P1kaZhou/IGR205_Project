#include <camera.hpp>

void Camera::fpsMoveLook(glm::vec2 angles) {
    glm::vec3 center3 = m_center;
    glm::vec4 center4 = glm::vec4(center3, 0.0f);
    
    float angleX = angles.x;
    float angleY = angles.y;

    angleX *= -1.f; // to have a clock wise rotation for positive values of xPos (so angleX)

    auto yAxe = glm::cross(center3-m_pos, UP);
    auto matX = glm::rotate(glm::mat4(1), angleX, UP);
    auto matY = glm::rotate(glm::mat4(1), angleY, yAxe);

    center4 = center4 - glm::vec4(m_pos, 0.0); // Camera to center
    center4 = matX * center4; // Rotate X
    center4 = matY * center4; // Rotate Y
    center4 = center4 + glm::vec4(m_pos, 0.0); // Camera to its position

    setCenter({center4.x, center4.y, center4.z});
}

void Camera::fpsMoveForward(float amplitude) {
    glm::vec3 look = m_center - m_pos;
    glm::vec3 front = glm::normalize( glm::vec3(look.x, 0.0f, look.z) ); // Projection on the orizontal plane
    front = front * amplitude;
    m_pos += front;
    m_center += front;
}
void Camera::fpsMoveRight(float amplitude) {
    glm::vec3 look = m_center - m_pos;
    glm::vec3 front = glm::normalize( glm::vec3(look.x, 0.0f, look.z) ); // Projection on the orizontal plane
    glm::vec3 v = glm::normalize( glm::cross(front, UP) );
    v = v * amplitude;
    m_pos += v;
    m_center += v;
}
void Camera::fpsMoveUp(float amplitude) {
    m_pos += glm::vec3(0, amplitude, 0);
    m_center += glm::vec3(0, amplitude, 0);
}

void Camera::camZoomToCenter(float ratio) {
    if(glm::abs(ratio) < 0 || glm::abs(ratio) > 1) {
        std::cerr << "Invalid ratio for camera move forward";
        std::cerr << " - ration must be in [0,1] or [-1,0] intervals";
        std::cerr << " - ratio="<<ratio << std::endl;
        assert(false);
    }
    glm::vec3 look = m_center - m_pos;
    glm::vec3 front = look * ratio;
    m_pos += front;
}
/*
angles.x => horizontal
angles.y => vertical
*/
void Camera::camRotateArroundCenter(glm::vec2 angles) {
    glm::vec4 posInCenterFrame = glm::vec4(m_pos-m_center, 0.0f);
    glm::vec3 horAxis = glm::normalize(glm::cross(m_center-m_pos, UP));
    glm::mat4 rotMatHor = glm::rotate(glm::mat4(1), angles.x, UP);
    glm::mat4 rotMatVer = glm::rotate(glm::mat4(1), angles.y, horAxis);

    posInCenterFrame = rotMatHor * rotMatVer * posInCenterFrame;
    m_pos = glm::vec3(posInCenterFrame)+m_center;
}
void Camera::camSlide(glm::vec2 displacement) {
    glm::vec3 horAxis = glm::normalize(glm::cross(m_center-m_pos, UP));
    glm::vec3 verAxis = glm::normalize(glm::cross(m_center-m_pos, horAxis));
    glm::vec3 v = displacement.x*horAxis + displacement.y*verAxis;
    m_pos += v;
    m_center += v;
}
