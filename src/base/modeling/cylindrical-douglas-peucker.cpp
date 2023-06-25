#include "cylindrical-douglas-peucker.hpp"

void CDP::compute() {
    skeleton.clear();
    for(auto ax : axis) {
        auto skel = std::vector<glm::vec2>();

        skel.push_back(*ax.begin());
        computeSingleAxisSkeleton(
            ax, ax.begin(), ax.begin()+ax.size()-1, skel
        );
        skel.push_back(*(ax.begin()+ax.size()-1));

        skeleton.push_back(skel);
    }
}

void CDP::computeSingleAxisSkeleton(
    const std::vector<glm::vec2> & axi,
    std::vector<glm::vec2>::iterator start,
    std::vector<glm::vec2>::iterator end,
    std::vector<glm::vec2> & skeleton
){
    if(start == end) return;

    auto cur = start+1;
    auto split = start+1;
    bool haveSplit = false;
    float error = 0;
    float maxError = FLT_MIN;
    for(;cur!=end;cur++) {
        error = computeError(*start, *end, *cur);
        // std::cout << "error : " << error << std::endl;
        if(error > threshold && maxError <= error) {
            split = cur;
            maxError = error;
            haveSplit = true;
            // std::cout << "split" << std::endl;
        }
    }

    if(haveSplit) {
        computeSingleAxisSkeleton(axi, start, split, skeleton);
        skeleton.push_back(*split);
        computeSingleAxisSkeleton(axi, split, end, skeleton);
    } 
}

float CDP::computeError(
    const glm::vec2 & start,
    const glm::vec2 & end,
    const glm::vec2 & v
) {
    // showVec(start, "start");
    // showVec(end, "end");
    // showVec(v, "v");
    auto startChord = Geometry::getChordOnPoint(start, points, chords);
    auto endChord = getChordOnPoint(end, points, chords);
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

    const auto & vChord = getChordOnPoint(v, points, chords);
    // showVec(points[vChord.a], "points[vChord.a],");
    // showVec(points[vChord.b], "points[vChord.b],");
    auto e1_dTop = Geometry::pointToSegmentDistance(points[vChord.a], baseA, baseD);
    auto e1_dBottom = Geometry::pointToSegmentDistance(points[vChord.a], baseB, baseC);
    auto e2_dTop = Geometry::pointToSegmentDistance(points[vChord.b], baseA, baseD);
    auto e2_dBottom = Geometry::pointToSegmentDistance(points[vChord.b], baseB, baseC);
    float e1 = glm::min(e1_dTop, e1_dBottom);
    float e2 = glm::min(e2_dTop, e2_dBottom);

    float distanceError = Geometry::pointToSegmentDistance(v, start, end);
    distanceError = distanceError*distanceError;
    float cylindricalError = e1*e1 + e2*e2;

    // std::cout << "cylindricalError = " << cylindricalError << std::endl;
    // std::cout << "distanceError = " << distanceError << std::endl;

    return importanceCylindricalError*cylindricalError + importanceDistanceError*distanceError;
}

glm::vec2 CDP::getPointChordProjectionOnAxiOrthogonalTo(
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
