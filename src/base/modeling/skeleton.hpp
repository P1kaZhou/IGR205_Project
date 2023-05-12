#ifndef _SKETCHY_SKELETON_
#define _SKETCHY_SKELETON_

#include <utils.hpp>

#include <map>

class SkeletonBone {
    friend class SkeletonBone;
    friend class Rigging;

public:
    SkeletonBone(
        SkeletonJoint & a,
        SkeletonJoint & b
    ): a(a), b(b) {}

    inline SkeletonJoint & getOther(const SkeletonJoint & a) {
        if(this->a.id == a.id) return this->b;
        return this->a;
    }

    bool operator==(SkeletonBone const & bone) const {
        return (a.id==bone.a.id && b.id==bone.b.id) || 
            (a.id==bone.b.id && b.id==bone.a.id);
    }

private:
    SkeletonJoint & a;
    SkeletonJoint & b;
};

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

#endif
