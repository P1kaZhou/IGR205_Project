#ifndef _SKETCHY_MESH_GROUP_
#define _SKETCHY_MESH_GROUP_

#include <utils.hpp>

/*
A model that has a transform matrix.
Holds methods for standard transformations (rotation, translation, scale)
*/
class MeshGroup {
public:

    inline MeshGroup & scaleMesh(const glm::vec3 & scale) {
        this->scale *= scale;
        return *this;
    }
    inline MeshGroup & rotateMesh(const glm::vec3 & angles) {
        this->angles += angles;
        return *this;
    }
    inline MeshGroup & translateMesh(const glm::vec3 & trans) {
        this->position += trans;
        return *this;
    }

    inline MeshGroup & setScale(const glm::vec3 & scale) {
        this->scale = scale;
        return *this;
    }
    inline MeshGroup & setOrientation(const glm::vec3 & angles) {
        this->angles = angles;
        return *this;
    }
    inline MeshGroup & setPosition(const glm::vec3 & trans) {
        this->position = trans;
        return *this;
    }

    /**Resets position, orientation angles and scale.*/
    inline MeshGroup & resetSettings() {
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        angles = glm::vec3(0.0f, 0.0f, 0.0f);
        scale = glm::vec3(1.0f, 1.0f, 1.0f);
        return *this;
    }

    inline glm::mat4 getWorldMatrix() const {
        auto worldMat = computeWorldMatrix();
        if(parent) {
            glm::mat4 parentMat = parent->getWorldMatrix();
            return parentMat * worldMat;
        }
        else {
            return worldMat;
        }
    }

    inline MeshGroup * getParent() const { return parent; }
    inline void setParent(MeshGroup * parent) {
        this->parent = parent;
    }

    inline const glm::vec3 & getPosition() const { return position; }
    inline const glm::vec3 & getScale() const { return scale; }
    inline const glm::vec3 & getAngles() const { return angles; }

protected:
    MeshGroup * parent = nullptr;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 angles = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    inline glm::mat4 computeWorldMatrix() const {
        glm::mat4 posMat = glm::translate(glm::mat4(1), position);
        glm::mat4 scaleMat = glm::scale(glm::mat4(1), scale);
        glm::mat4 rotMat = glm::eulerAngleXYZ(angles.x, angles.y, angles.z);
        return posMat * scaleMat * rotMat;
    }

};

#endif
