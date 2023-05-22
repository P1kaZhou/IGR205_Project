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

    inline float computeError(
        const glm::vec2 & start,
        const glm::vec2 & end,
        const glm::vec2 & v
    ) {
        // showVec(start, "start");
        // showVec(end, "end");
        // showVec(v, "v");
        auto startChord = getChordOnPoint(start);
        auto endChord = getChordOnPoint(end);
        // showVec(points[startChord.a], "points[startChord.a]");
        // showVec(points[startChord.b], "points[startChord.b]");
        // showVec(points[endChord.a], "points[endChord.a]");
        // showVec(points[endChord.b], "points[endChord.b]");
        glm::vec2 baseA = getPointChordProjectionOnAxiOrthogonalTo(start, end, points[startChord.a]);
        glm::vec2 baseB = getPointChordProjectionOnAxiOrthogonalTo(start, end, points[startChord.b]);
        glm::vec2 baseC = getPointChordProjectionOnAxiOrthogonalTo(end, start, points[endChord.a]);
        glm::vec2 baseD = getPointChordProjectionOnAxiOrthogonalTo(end, start, points[endChord.b]);

        // showVec(baseA, "baseA");
        // showVec(baseB, "baseB");
        // showVec(baseC, "baseC");
        // showVec(baseD, "baseD");

        const auto & vChord = getChordOnPoint(v);
        // showVec(points[vChord.a], "points[vChord.a],");
        // showVec(points[vChord.b], "points[vChord.b],");
        auto e1_dTop = Geometry::pointToSegmentDistance(points[vChord.a], baseA, baseD);
        auto e1_dBottom = Geometry::pointToSegmentDistance(points[vChord.a], baseB, baseC);
        auto e2_dTop = Geometry::pointToSegmentDistance(points[vChord.b], baseA, baseD);
        auto e2_dBottom = Geometry::pointToSegmentDistance(points[vChord.b], baseB, baseC);
        float e1 = glm::min(e1_dTop, e1_dBottom);
        float e2 = glm::min(e2_dTop, e2_dBottom);

        return e1*e1 + e2*e2;
    }

    inline glm::vec2 getPointChordProjectionOnAxiOrthogonalTo(
        const glm::vec2 & v0, const glm::vec2 & v1,
        const glm::vec2 & p
    ) {
        float y = glm::sqrt(
            ((v1.x-v0.x)*(v1.x-v0.x)) /
            ((v1.x-v0.x)*(v1.x-v0.x) + (v1.y-v0.y)*(v1.y-v0.y)) );
        float x = glm::sqrt(1-y*y);
        // std::cout << "num : " << ((v1.x-v0.x)*(v1.x-v0.x)) << std::endl;
        // std::cout << "denom : " << ((v1.x-v0.x)*(v1.x-v0.x) + (v1.y-v0.y)) << std::endl;
        glm::vec2 u = {x,y};
        // showVec(u, "u");
        if(glm::dot(u,p-v0)<0) u = -u;
        // std::cout << "angle : " << glm::angle(glm::normalize(p-v0), glm::normalize(v1-v0)) << std::endl;
        float A = Geometry::norm(p, v0) * glm::cos(-M_PI/2 + glm::angle(glm::normalize(p-v0), glm::normalize(v1-v0)));
        // std::cout << "A : " << A << std::endl;
        return v0+A*u;
    }

    /*
    Return the closest chord to a point.
    */
    inline const Geometry::Edge & getChordOnPoint(const glm::vec2 & p) {
        unsigned closestChord;
        float minArea = FLT_MAX;
        for(unsigned i=0; i<chords.size(); i++) {
            const auto & c = chords[i];
            float area = Geometry::triangleArea(points[c.a], points[c.b], p);
            if(minArea > area) {
                minArea = area;
                closestChord = i;
            }
        }
        return chords[closestChord];
    }
};

#endif
