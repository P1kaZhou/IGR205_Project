#ifndef _SKETCHY_RIGGING_
#define _SKETCHY_RIGGING_

#include <utils.hpp>
#include "skeleton.hpp"

/*
The skinning of vertices for one bone.
*/
class SkinningGroup {
public:
    SkinningGroup(unsigned verticesCount) {
        for(unsigned i=0; i<verticesCount; i++) {
            vertexSkinWeights.insert({i, 0.0f});
        }
    }

    inline void setSkinWeight(unsigned vertexIndice, float weight) {
        vertexSkinWeights[vertexIndice] = weight;
    }

private:
    std::map<unsigned, float> vertexSkinWeights;
};

class Rigging {
public:
    Rigging() {}

    // SKELETON

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
        bones.emplace_back(
            getJointById(jointAId),
            getJointById(jointBId));
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

    // SKINNING

    inline void initSkinning(
        unsigned verticesCount
    ) {
        bonesSkins.resize(verticesCount, SkinningGroup(verticesCount));
        this->verticesCount = verticesCount;
    }

    inline void setBoneSkinning(
        unsigned jointAId, unsigned jointBId,
        const std::vector<float> & vertexSkinningWeights
    ) {
        unsigned index = getBoneIndexByJointsId(jointAId, jointBId);
        for(unsigned i=0; i<verticesCount; i++) {
            bonesSkins[index].setSkinWeight(i, vertexSkinningWeights[i]);
        }
    }

    inline const std::vector<SkeletonBone> & getBones() const { return bones; }
    inline const std::vector<SkeletonJoint> & getJoints() const { return joints; }

private:
    std::vector<SkeletonJoint> joints;
    std::vector<SkeletonBone> bones;
    std::vector<SkinningGroup> bonesSkins;

    unsigned verticesCount;

    unsigned lastID = 0;

    inline unsigned getBoneIndexByJointsId(unsigned jointAId, unsigned jointBId) {
        for(unsigned i=0; i<bones.size(); i++) {
            const SkeletonBone & it = bones[i];
            if(it.a.id == jointAId && it.b.id == jointBId) return i;
            else if(it.a.id == jointAId && it.b.id == jointBId) return i;
        }
        std::cerr << "Unknown skeleton bone for joint ids " << jointAId << " and " << jointBId << std::endl;
        assert(false);
    }
};

#endif
