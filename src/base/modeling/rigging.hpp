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

    inline std::map<unsigned, float> & getVertexSkinWeights() { return vertexSkinWeights; }

private:
    std::map<unsigned, float> vertexSkinWeights;
};

class Rigging {
public:
    Rigging(unsigned expectedJointCount=100, unsigned expectedBonesCount=100) {
        /*
        Because of reallocation of the vectors,
        the references to the joints holded by the bones can become invalid.
        That is why I reserve a significant amount of memory to avoir realloc.
        Proper solutions : 
        - Use a std::list to avoir reallocation
        - Use a std::vector of pointers and dynamically allocate joints at creation.
        */
        joints.reserve(expectedJointCount);
        bones.reserve(expectedBonesCount);
    }

    // SKELETON

    /*
    Insert a joint at the point position.
    Returns the id of the new joint.
    */
    inline unsigned addJoint(const glm::vec3 & point) {
        joints.push_back(SkeletonJoint(point));
        lastID += 1;
        joints.back().id = lastID;
        return lastID;
    }

    inline void addBone(unsigned jointAId, unsigned jointBId) {
        bones.push_back(SkeletonBone(
            getJointById(jointAId),
            getJointById(jointBId)
        ));
    }

    inline SkeletonJoint & getJointOnRay(const glm::vec3 & direction, const glm::vec3 & startPos) {
        
    }

    inline SkeletonBone & getBoneOnRay(const glm::vec3 & direction, const glm::vec3 & startPos) {
        
    }

    inline SkeletonJoint & getJointById(unsigned id) {
        for(auto & joint : joints) {
            if(joint.id == id) return joint;
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
        auto bonesCount = bones.size();
        bonesSkins.resize(bonesCount, SkinningGroup(verticesCount));
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
    inline std::vector<SkinningGroup> & getBonesSkins() { return bonesSkins; }

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
