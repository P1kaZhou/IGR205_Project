#ifndef _SKETCHY_CYLINDRICAL_SKELETON_GENERATOR_
#define _SKETCHY_CYLINDRICAL_SKELETON_GENERATOR_

#include <utils.hpp>
#include <geometry/geometry.hpp>
#include <cylindrical-douglas-peucker.hpp>
#include <rigging.hpp>

class SkeletonGenerator {
public:
    SkeletonGenerator(
        const std::vector<glm::vec2> & points,
        const std::vector<std::vector<glm::vec2>> & externalAxis,
        const std::vector<std::vector<glm::vec2>> & internalAxis,
        const std::vector<Geometry::Edge> & chords,
        float cdpThreshold,
        float cdpDistanceImp,
        float cdpCylindricalImp
    ):
    points(points),
    externalAxis(externalAxis), internalAxis(internalAxis),
    chords(chords), cdpThreshold(cdpThreshold),
    cdpDistanceImp(cdpDistanceImp),
    cdpCylindricalImp(cdpCylindricalImp)
    {}

    void compute();

private:
    const std::vector<glm::vec2> & points;
    const std::vector<Geometry::Edge> & chords;
    
    float cdpThreshold;
    float cdpDistanceImp;
    float cdpCylindricalImp;

    std::vector<std::vector<glm::vec2>> externalAxis;
    std::vector<std::vector<glm::vec2>> internalAxis;

    Rigging rigging;

};

#endif
