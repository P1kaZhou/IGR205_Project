#ifndef _SKETCHY_CYLINDRICAL_DOUGLAS_PEUCKER_
#define _SKETCHY_CYLINDRICAL_DOUGLAS_PEUCKER_

#include <utils.hpp>
#include <map>

#include <geometry/geometry.hpp>

class CDP {
public:
    CDP(
        const std::vector<glm::vec2> & points,
        const std::vector<std::vector<glm::vec2>> & axis,
        const std::vector<Geometry::Edge> & chords,
        float threshold
    ): points(points), axis(axis), chords(chords), threshold(threshold) {}

    void compute();

    inline std::vector<std::vector<glm::vec2>> & getSkeleton() {return skeleton;}

    inline void showSkeleton() {
        std::cout << "skeleton :" << std::endl;
        for(auto skel : skeleton) {
            std::cout << "\tskeleton axis :" << std::endl;
            for(auto joint : skel) {
                showVec(joint, "joint");
            }
        }
    }

private:
    const std::vector<glm::vec2> & points;
    const std::vector<std::vector<glm::vec2>> & axis;
    const std::vector<Geometry::Edge> & chords;
    float threshold;

    std::vector<std::vector<glm::vec2>> skeleton;

    void computeSingleAxisSkeleton(
        const std::vector<glm::vec2> & axi,
        std::vector<glm::vec2>::iterator start,
        std::vector<glm::vec2>::iterator end,
        std::vector<glm::vec2> & skeleton
    );

    float computeError(
        const glm::vec2 & start,
        const glm::vec2 & end,
        const glm::vec2 & v
    );

    glm::vec2 getPointChordProjectionOnAxiOrthogonalTo(
        const glm::vec2 & v0, const glm::vec2 & v1,
        const glm::vec2 & p
    );

};

#endif
