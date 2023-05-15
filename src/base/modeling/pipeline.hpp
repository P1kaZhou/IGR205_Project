#ifndef _SKETCHY_PIPELINE_
#define _SKETCHY_PIPELINE_

#include <utils.hpp>

class Pipeline {
public:
    Pipeline();

    std::vector<Mesh> computeCylinders();

    Mesh computeMeshFromCylinders(std::vector<Mesh>);

    Rigging computeSkeleton(Mesh);

    void pipelineStep();

private:
    std::vector<glm::vec2> sketchPoints;

};

#endif
