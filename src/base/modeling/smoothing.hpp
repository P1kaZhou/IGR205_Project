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
#include <numeric>

class smoothing {

private:

    float lambda;

public:

    void setLambda(float lambda);

    void applyLaplacianToMedial(MedialAxisGenerator &medialAxisG, int iterations);

    void insignificantBranchesRemoval(MedialAxisGenerator &medialAxisG, float threshold,
                                      std::vector<glm::vec2> sketchPoints);

    std::vector<glm::uvec2> computeNormalChordalAxes(MedialAxisGenerator &medialAxisG, ConstrainedDelaunayTriangulation2D &cdt,
                                                     std::vector<glm::vec2> &points);

    std::vector<glm::uvec3> computeJunctionTriangles(ConstrainedDelaunayTriangulation2D &cdt);

    std::vector<glm::uvec2> computeConnectingRegion(std::vector<glm::uvec3> &triangles, std::vector<glm::vec2> &points,
                                                    MedialAxisGenerator &medialAxisG);

    void extendAxis(MedialAxisGenerator &medialAxisG, std::vector<glm::vec2> &sketchPoints);

    void middleMedialAxis(std::vector<glm::uvec2> chordalAxes, MedialAxisGenerator &medialAxisG,
                          std::vector<glm::vec2> &sketchPoints);

};


#endif //SKETCHY_SMOOTHING_HPP