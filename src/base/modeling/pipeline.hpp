#ifndef _SKETCHY_PIPELINE_
#define _SKETCHY_PIPELINE_

#include <utils.hpp>
#include <mesh-geometry.hpp>
#include "rigging.hpp"

class Pipeline {
public:
    Pipeline(
        std::vector<glm::vec2> & sketchPoints
    ): sketchPoints(sketchPoints) {}

    std::vector<MeshGeometry> computeCylinders();

    MeshGeometry computeMeshFromCylinders(std::vector<MeshGeometry> & cylinders);

    Rigging computeSkeleton(MeshGeometry & mesh);

private:
    std::vector<glm::vec2> & sketchPoints;

};

#endif
