#include "skeleton-generator.hpp"

static bool findIdForKey(
    const std::vector<std::pair<glm::vec2, unsigned>> & pairs,
    const glm::vec2 & key,
    unsigned & id
) {
    for(auto pair : pairs) {
        if(pair.first == key) {
            id = pair.second;
            return true;
        }
    }
    return false;
}

void SkeletonGenerator::compute() {

    // Pairs of junction triangle points and their junction id in
    // the skeleton
    std::vector<std::pair<glm::vec2, unsigned>> junctionPoints;

    // The last joint of each axis is in a junction triangle
    for(auto & skelAxis : externalAxis) {
        if(skelAxis.size() >= 2) {
            // We link the junction triangle point to the reste of the axis
            // in the skeleton
            glm::vec2 & junctionTrianglePoint = skelAxis[skelAxis.size()-1];
            glm::vec2 & junctionTriangleLinkPoint = skelAxis[skelAxis.size()-2];
            unsigned junctionTriangleJointId = rigging.addJoint(
                glm::vec3(junctionTrianglePoint, 0));
            unsigned linkPointJointId = rigging.addJoint(
                glm::vec3(junctionTriangleLinkPoint, 0));

            showVec(junctionTrianglePoint, "junctionTrianglePoint");
            showVec(junctionTriangleLinkPoint, "junctionTriangleLinkPoint");
            rigging.addBone(junctionTriangleJointId, linkPointJointId);

            junctionPoints.push_back({junctionTrianglePoint, junctionTriangleJointId});
        }
    }
    // We remove that last joint
    for(auto & skelAxis : externalAxis) {
        skelAxis.erase(skelAxis.begin()+skelAxis.size()-1);
    }

    // Douglas-Peucker Algorithm for external axis
    CDP cdp(points, externalAxis, chords, cdpThreshold);
    cdp.compute();
    std::vector<std::vector<glm::vec2>> externalAxisSkeleton = cdp.getSkeleton();

    // Add cylinders skeleton to final skeleton
    for(auto skelAxis : externalAxisSkeleton) {
        unsigned prevJointId = 0;
        unsigned jointId = 0;
        for(unsigned jointIndex=0; jointIndex<skelAxis.size(); jointIndex++) {
            auto joint = skelAxis[jointIndex];
            jointId = rigging.addJoint(glm::vec3(joint, 0));

            if(jointIndex>0) {
                rigging.addBone(jointId, prevJointId);
            }

            prevJointId = jointId;
        }
    }

    for(auto axis : internalAxis) {
        const glm::vec2 & junctionTrianglePointStart = axis.front();
        const glm::vec2 & junctionTrianglePointEnd = axis.back();
        unsigned junctionTriangleJointStartID;
        unsigned junctionTriangleJointEndID;
        if(findIdForKey(junctionPoints, junctionTrianglePointStart, junctionTriangleJointStartID)) {
            if(findIdForKey(junctionPoints, junctionTrianglePointEnd, junctionTriangleJointEndID)) {
                rigging.addBone(
                    junctionTriangleJointStartID,
                    junctionTriangleJointEndID);
            }            
        }
    }
    
}


