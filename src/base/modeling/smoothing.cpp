//
// Created by daniel on 22/05/23.
//

#include "smoothing.hpp"

void smoothing::setLambda(float lambda) {
    this->lambda = lambda;
}

void smoothing::applyLaplacianToMedial(MedialAxis &medialAxis, int iterations) {
    for (int k = 0; k < iterations; k++) {
        std::vector<MedialAxisPoint *> points = medialAxis.getPoints();
        // Iterate through every medial axis point
        for (int i = 0; i < points.size(); i++) {
            MedialAxisPoint *point = points[i];
            glm::vec2 newPoint = (1 - lambda) * point->getPoint();
            int adjPointsIndex = point->getAdjIndex(point);
            MedialAxisPoint *adjPoints = point->getAdjs()[adjPointsIndex];
            int v = 0;
            glm::vec2 alternativePoint = glm::vec2(0, 0);
            /*
            for (auto point : adjPoints) {
                MedialAxisPoint * adjPoint = adjPoints[j];
                newPoint += lambda*adjPoint->getPoint()/v;
            }
            // Add the corrected point to the medial axis

            */
        }

    }
}

// Not sure whether I should go for this one
void smoothing::chordialAxisTransform(ConstrainedDelaunayTriangulation2D &cdt, MedialAxis &medialAxis) {
    std::vector<glm::uvec3> &triangles = cdt->getTriangles();
    std::vector<Geometry::Edge> &edges = cdt->getEdges();
    std::vector<glm::vec2> boundaryEdges; // placeholder

    // Question


}

void smoothing::insignificantBranchesRemoval(MedialAxis &medialAxis, float threshold,
                                             ConstrainedDelaunayTriangulation2D &cdt) {
    std::vector<glm::uvec3> &triangles = cdt->getTriangles();

}

std::vector<glm::uvec3> smoothing::computeJunctionTriangles(ConstrainedDelaunayTriangulation2D &cdt) {
    std::vector<Geometry::Edge> &edges = cdt->getEdges();

    std::map<unsigned, std::set<unsigned>> edgeCount;
    for (int i = 0; i < edges.size(); i++) {
        Geometry::Edge edge = edges[i];
        unsigned int a = edge.a;
        unsigned int b = edge.b;
        edgeCount[a].insert(b);
        edgeCount[b].insert(a);
    }

    std::vector<glm::uvec3> junctionTriangles;
    for (auto const &[key, val]: edgeCount) {
        std::set<unsigned> visitedInts;
        std::vector<unsigned> neighbors = std::vector<unsigned>(val.size());
        std::copy(val.begin(), val.end(), neighbors.begin());
        if (val.size() == 2 && !visitedInts.count(key) && !visitedInts.count(neighbors[0]) &&
            !visitedInts.count(neighbors[1])) {
            unsigned int a = neighbors[0];
            unsigned int b = neighbors[1];
            visitedInts.insert(a);
            visitedInts.insert(b);
            visitedInts.insert(key);
            glm::uvec3 triangle = glm::uvec3(key, a, b);
            junctionTriangles.push_back(triangle);
        }

    }

    return junctionTriangles;
}

std::vector<glm::uvec3> smoothing::getSignificantTriangles(std::vector<glm::uvec3> & triangles, std::vector<glm::vec2> & points){

}