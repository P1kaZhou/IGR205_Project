#ifndef _SKETCHY_CYLINDER_GENERATOR_
#define _SKETCHY_CYLINDER_GENERATOR_

#include <utils.hpp>
#include <map>

#include <geometry/geometry.hpp>

class CylinderGenerator {
public:
    CylinderGenerator(
        const std::vector<glm::vec2> & axis,
        const std::vector<glm::vec2> & shape,
        const std::vector<Geometry::Edge> & chords,
        const std::map<glm::vec2, Geometry::Edge> & axisPointToChord
    ): axis(axis), shape(shape), chords(chords), axisPointToChord(axisPointToChord) {}

    void compute(unsigned circleSampleCount);

    inline const std::vector<glm::vec3> & getVertexPos() const { return genVertexPos; }
    inline const std::vector<glm::uvec3> & getFaces() const { return genFaces; }

    inline const Geometry::Edge & getLastChord() const { return lastChord; }
    inline const Geometry::Edge & getBeforeLastChord() const { return beforeLastChord; }
    inline std::vector<unsigned> getLastSectionSideAVertices() const { return lastSectionSideA; }
    inline std::vector<unsigned> getLastSectionSideBVertices() const { return lastSectionSideB; }

    inline void showVertices() {
        std::cout << "Vertices : " << genVertexPos.size() << std::endl;
        for(auto v : genVertexPos) {
            showVec(v, "\tv : ");
        }
    }

    inline void showFaces() {
        std::cout << "Faces :" << genFaces.size() << std::endl;
        for(auto f : genFaces) {
            showVec(f, "\tf : ");
        }
    }

    unsigned offset = 0;

private:
    // Input
    const std::vector<glm::vec2> & axis;
    const std::vector<glm::vec2> & shape;
    const std::vector<Geometry::Edge> & chords;
    const std::map<glm::vec2, Geometry::Edge> & axisPointToChord;

    // Output
    std::vector<glm::vec3> genVertexPos;
    std::vector<glm::uvec3> genFaces;

    std::vector<unsigned> lastSectionSideA;
    std::vector<unsigned> lastSectionSideB;
    Geometry::Edge lastChord = Geometry::Edge(0,0);
    Geometry::Edge beforeLastChord = Geometry::Edge(0,0);

};

#endif
