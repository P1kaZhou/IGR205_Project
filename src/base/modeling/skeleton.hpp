#ifndef _SKETCHY_SKELETON_
#define _SKETCHY_SKELETON_

#include <utils.hpp>

#include <map>

class SkeletonJoint {
    friend class SkeletonBone;
    friend class Rigging;

public:
    SkeletonJoint(const glm::vec3 & point)
    : point(point) {}

    inline void rotateAroundAdjacentJoint(unsigned jointId, glm::vec3 & rotation) {
        auto center = getJointById(jointId).point;
        rotateAroundPoint(center, rotation);
        for(auto it=adjacentJoints.begin(); it!=adjacentJoints.end(); it++) {
            if(it->id != id) {
                it->rotateAroundPoint(center, rotation);
            }
        }
    }

    inline void addAdjacentJoint(SkeletonJoint & joint) {
        adjacentJoints.push_back(joint);
    }

    inline const glm::vec3 & getPoint() const { return point; }
    inline unsigned getId() const { return id; }
    inline std::vector<SkeletonJoint> & getAdjacentJoints() { return adjacentJoints; }

private:
    unsigned id;
    glm::vec3 point;
    std::vector<SkeletonJoint> adjacentJoints;

    inline void rotateAroundPoint(glm::vec3 & center, glm::vec3 & rotation) {
        glm::mat4 posMat = glm::translate(glm::mat4(1), center);
        glm::mat4 rotMat = glm::eulerAngleXYZ(
            rotation.x, rotation.y, rotation.z);
        glm::mat4 transform = rotMat * posMat;
        point = transform * glm::vec4(point, 1);
    }

    inline SkeletonJoint & getJointById(unsigned id) {
        for(auto it=adjacentJoints.begin(); it!=adjacentJoints.end(); it++) {
            if(it->id == id) return *it;
        }
        assert(false);
    }

};

class SkeletonBone {
    friend class Rigging;

public:
    SkeletonBone(
        const SkeletonJoint & a,
        const SkeletonJoint & b
    ): a(a), b(b) {}

    inline const SkeletonJoint & getOther(const SkeletonJoint & a) const {
        if(this->a.id == a.id) return this->b;
        return this->a;
    }

    bool operator==(SkeletonBone const & bone) const {
        return (a.id==bone.a.id && b.id==bone.b.id) || 
            (a.id==bone.b.id && b.id==bone.a.id);
    }

    inline const SkeletonJoint & getA() const { return a; }
    inline const SkeletonJoint & getB() const { return b; }

private:
    const SkeletonJoint & a;
    const SkeletonJoint & b;
};

#endif
