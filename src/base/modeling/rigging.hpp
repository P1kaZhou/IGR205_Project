#ifndef _SKETCHY_RIGGING_
#define _SKETCHY_RIGGING_

#include <utils.hpp>
#include <skeleton.hpp>

class SkinningGroup {
public:
    SkinningGroup() {}

    inline void setSkinWeight(unsigned vertexIndice, float weight) {
        vertexSkinWeights[vertexIndice] = weight;
    }

private:
    std::map<unsigned, float> vertexSkinWeights;
};

class Rigging {
public:
    Rigging();

    /*
    Insert a joint at the point position.
    Returns the id of the new joint.
    */
    inline unsigned addJoint(const glm::vec3 & point) {
        joints.emplace_back(point);
        lastID += 1;
        joints.back().id = lastID;
        return lastID;
    }

    inline void addBone(unsigned jointAId, unsigned jointBId) {
        bones.emplace_back(jointAId, jointBId);
    }

    inline SkeletonJoint & getJointOnRay(const glm::vec3 & direction, const glm::vec3 & startPos) {
        
    }

    inline SkeletonBone & getBoneOnRay(const glm::vec3 & direction, const glm::vec3 & startPos) {
        
    }

    inline SkeletonJoint & getJointById(unsigned id) {
        for(auto it=joints.begin(); it!=joints.end(); it++) {
            if(it->id == id) return *it;
        }
        std::cerr << "Unknown skeleton joint ID " << id << std::endl;
        assert(false);
    }

    inline SkeletonBone & getBoneByJointsId(unsigned jointAId, unsigned jointBId) {
        for(auto it=bones.begin(); it!=bones.end(); it++) {
            if(it->a.id == jointAId && it->b.id == jointBId) return *it;
            else if(it->a.id == jointAId && it->b.id == jointBId) return *it;
        }
        std::cerr << "Unknown skeleton bone for joint ids " << jointAId << " and " << jointBId << std::endl;
        assert(false);
    }

private:
    std::vector<SkeletonJoint> joints;
    std::vector<SkeletonBone> bones;
    std::vector<SkinningGroup> bonesSkins;

    unsigned lastID = 0;
};

#endif
