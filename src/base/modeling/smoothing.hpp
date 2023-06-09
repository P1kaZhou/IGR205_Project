//
// Created by daniel on 22/05/23.
//

#ifndef SKETCHY_SMOOTHING_HPP
#define SKETCHY_SMOOTHING_HPP


#include <vector>
#include "glm/vec2.hpp"
#include "medial-axis.hpp"
#include "medial-axis-generator.hpp"
#include "delaunay.hpp"

class smoothing {

private:

    float lambda;

public:

    void setLambda(float lambda);

    void applyLaplacianToMedial(MedialAxis &medialAxis, int iterations);

    void insignificantBranchesRemoval(MedialAxisGenerator &medialAxisG, float threshold, std::vector<glm::uvec3> triangles,
                                      std::vector<glm::vec2> sketchPoints);

    std::vector<glm::uvec2> computeNormalChordalAxes(MedialAxis &medialAxis, ConstrainedDelaunayTriangulation2D &cdt,
                                                     std::vector<glm::vec2> &points);

    std::vector<glm::uvec3> computeJunctionTriangles(ConstrainedDelaunayTriangulation2D &cdt);

    std::vector<glm::uvec3> getSignificantTriangles(std::vector<glm::uvec3> &triangles, std::vector<glm::vec2> &points);

    std::vector<glm::uvec2> computeConnectingRegion(std::vector<glm::uvec3> &triangles, std::vector<glm::vec2> &points,
                                                    MedialAxis &medialAxis);

};


#endif //SKETCHY_SMOOTHING_HPP