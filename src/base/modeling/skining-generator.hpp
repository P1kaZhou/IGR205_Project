#ifndef _SKETCHY_SKINING_GENERATOR_
#define _SKETCHY_SKINING_GENERATOR_

#include <utils.hpp>
#include <geometry/geometry.hpp>
#include "rigging.hpp"

class SkiningGenerator {
public:
    SkiningGenerator(
        Rigging & rigging,
        const std::vector<glm::vec3> & vertices,
        const std::vector<glm::uvec3> & faces
    ): rigging(rigging), vertices(vertices), faces(faces) {
        rigging.initSkinning(vertices.size());
    }

    void compute();

private:
    Rigging & rigging;

    const std::vector<glm::vec3> & vertices;
    const std::vector<glm::uvec3> & faces;

    const int findVertexClosestVisibleBoneIndex(
        const glm::vec3 & vertex
    );

};

#endif
