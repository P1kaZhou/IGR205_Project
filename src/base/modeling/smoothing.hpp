//
// Created by daniel on 22/05/23.
//

#ifndef SKETCHY_SMOOTHING_HPP
#define SKETCHY_SMOOTHING_HPP


#include <vector>
#include "glm/vec2.hpp"
#include "medial-axis.hpp"

class smoothing {

private:

    float lambda;

public:

    void setLambda(float lambda);

    void applyLaplacianToMedial(MedialAxis & medialAxis, int iterations);

    void insignificantBranchesRemoval(MedialAxis & medialAxis, float threshold, ConstrainedDelaunayTriangulation2D & cdt);

    void chordialAxisTransform(ConstrainedDelaunayTriangulation2D & cdt, MedialAxis & medialAxis); // Not sure

    std::vector<glm::uvec3> computeJunctionTriangles(ConstrainedDelaunayTriangulation2D & cdt);

    std::vector<glm::uvec3> getSignificantTriangles(std::vector<glm::uvec3> & triangles, std::vector<glm::vec2> & points);

    std::vector<glm::uvec3> computeConnectingRegion(std::vector<glm::uvec3> & triangles, std::vector<glm::vec2> & points);
};


#endif //SKETCHY_SMOOTHING_HPP