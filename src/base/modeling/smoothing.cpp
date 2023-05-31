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
        std::vector<MedialAxisPoint *> newPoints;
        // Iterate through every medial axis point
        for (int i = 0; i < points.size(); i++) {
            MedialAxisPoint *point = points[i];
            glm::vec2 newPoint = (1 - lambda) * point->getPoint();
            std::vector<MedialAxisPoint *> neighbors = point->getAdjs();
            int v = neighbors.size();
            // Iterate through every neighbor of the point
            for (auto const &neighbor: neighbors) {
                newPoint += lambda / v * neighbor->getPoint();
            }
            newPoints.push_back(new MedialAxisPoint(newPoint));

        }
    }
}

std::vector<glm::uvec2> smoothing::computeNormalChordalAxes(MedialAxis &medialAxis,
                                                            ConstrainedDelaunayTriangulation2D &cdt,
                                                            std::vector<glm::vec2> sketchPoints) {
    // There may actually be no need for the triangles, I can just go through the chordal axis and create a normal segment
    std::vector<MedialAxisPoint *> points = medialAxis.getPoints();
    std::vector<glm::uvec3> triangles = cdt->getTriangles();

    std::vector<glm::uvec2> normalChordalAxes; // The axes are determines by two points from the sketchpoints

    std::set<unsigned> visitedPoints;

    // Iterate through every medial axis point
    for (auto maPoint: points) {
        std::vector<MedialAxisPoint *> neighbors = maPoint->getAdjs();
        if (neighbors.size() == 2) { // local gradient computable
            glm::vec2 middlePoint = maPoint->getPoint();
            glm::vec2 p1 = neighbors[0]->getPoint();
            glm::vec2 p2 = neighbors[1]->getPoint();
            glm::vec2 gradient = glm::normalize(p2 - p1);
            glm::vec2 normal = glm::vec2(-gradient.y, gradient.x);

            // Find the two closest points
            // One point from the direction proj1 = middlePoint + t * normal
            // The other point from the direction proj2 = middlePoint - t * normal
            // Requirements: distance small enough, and scalar product of normal with (middlepoint, point) close to 1 (normalized)
            // The other one will actually be close to -1 (normalized)

            float minDistance = std::numeric_limits<float>::max();
            glm::vec2 closestPoint1;
            glm::vec2 closestPoint2;
            std::vector<float> scalarProducts;

            for (int i = 0; i < sketchPoints.size(); i++) {
                glm::vec2 currentPoint = sketchPoints[i];
                scalarProducts.push_back(glm::dot(glm::normalize(normal), glm::normalize(currentPoint - middlePoint)));
                if (glm::distance(currentPoint, middlePoint) < minDistance) {
                    minDistance = glm::distance(currentPoint, middlePoint);
                }
            }

            // Gather the points which are close enough
            std::vector<unsigned> closePoints;
            std::vector<float> closeScalarProducts;
            const float MARGIN = 0.1;
            for (int i = 0; i < sketchPoints.size(); i++) {
                if (glm::distance(sketchPoints[i], middlePoint) <= minDistance + MARGIN) {
                    closePoints.push_back(i);
                    closeScalarProducts.push_back(scalarProducts[i]);
                }
            }

            int candidatesNumber = closePoints.size();
            const float MIN_POSITIVE_SCALAR_PRODUCT = 0.99;
            const float MAX_NEGATIVE_SCALAR_PRODUCT = -0.99;

            // We check the most perpendicular points
            // If they are already found, too bad, I mean there are tons of sketch points anyway

            int maxIndex = std::distance(closeScalarProducts.begin(),
                                         std::max_element(closeScalarProducts.begin(), closeScalarProducts.end()));
            int minIndex = std::distance(closeScalarProducts.begin(),
                                         std::min_element(closeScalarProducts.begin(), closeScalarProducts.end()));
            int trueMaxIndex = closePoints[maxIndex];
            int trueMinIndex = closePoints[minIndex];

            if (closeScalarProducts[maxIndex] >= MIN_POSITIVE_SCALAR_PRODUCT &&
                (visitedPoints.find(trueMaxIndex) != visitedPoints.end()) &&
                closeScalarProducts[minIndex] <= MAX_NEGATIVE_SCALAR_PRODUCT &&
                (visitedPoints.find(trueMinIndex) != visitedPoints.end())) {

                visitedPoints.insert(trueMaxIndex);
                visitedPoints.insert(trueMinIndex);

                normalChordalAxes.push_back(glm::uvec2(trueMaxIndex, trueMinIndex));
            }
        }
    }
    return normalChordalAxes;
}

void smoothing::insignificantBranchesRemoval(MedialAxis &medialAxis, float threshold,
                                             ConstrainedDelaunayTriangulation2D &cdt) {
    std::vector<glm::uvec3> &triangles = cdt->getTriangles();
    // TODO: apply Prasad criteria
    // The threshold represents the ratio of morphological significance, p/AB




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

std::vector<glm::uvec3>
smoothing::getSignificantTriangles(std::vector<glm::uvec3> &triangles, std::vector<glm::vec2> &points) {
    // Most likely, there will be triangles from various area.
    // The easiest way will be to ditch the triangles that are too small

    std::vector<glm::uvec3> significantTriangles;
    std::vector<float> areas = std::vector<float>(triangles.size());
    for (int i = 0; i < triangles.size(); i++) {
        glm::vec2 a = points[triangles[i][0]];
        glm::vec2 b = points[triangles[i][1]];
        glm::vec2 c = points[triangles[i][2]];
        float area = 0.5f * glm::abs((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y));
        areas[i] = area;
    }
    // Now it is up to us to decide. For now let's keep the ones above average
    float average = std::accumulate(areas.begin(), areas.end(), 0.0f) / areas.size();
    for (int i = 0; i < triangles.size(); i++) {
        if (areas[i] > average) {
            significantTriangles.push_back(triangles[i]);
        }
    }
    return significantTriangles;
}

std::vector<glm::uvec2>
smoothing::computeConnectingRegion(std::vector<glm::uvec3> &triangles, std::vector<glm::vec2> &points,
                                   MedialAxis &medialAxis) {
    // At this point, we only have the significant triangles

    int trianglesNumber = triangles.size(); // To achieve the merging, we will need to compare respective distances

    std::vector<MedialAxisPoint*> medialAxisPoints = medialAxis.getPoints();

    // There is a simple way to pick the merged triangles. If there is a chordal axis linking two triangles
    // then they are merged.
    // To see this, let's draw a line between two triangles. If the center of the line is close enough to the chordal axis,
    // then the triangles will be merged
    // Let's use the struct Edge with the two ints being the indices of the triangles
    std::vector<Geometry::Edge> trianglesCouples;
    std::set<Geometry::Edge> keptCouples;
    for (int i = 0; i < trianglesNumber; i++) {
        for (int j = i + 1; j < trianglesNumber; j++) {
            trianglesCouples.push_back(Geometry::Edge(i, j));
        }
    }

    for (auto edge: trianglesCouples){
        glm::vec2 a = points[triangles[edge.a][0]];
        glm::vec2 b = points[triangles[edge.a][1]];
        glm::vec2 c = points[triangles[edge.a][2]];
        glm::vec2 d = points[triangles[edge.b][0]];
        glm::vec2 e = points[triangles[edge.b][1]];
        glm::vec2 f = points[triangles[edge.b][2]];

        glm::vec2 center = (a + b + c + d + e + f) / 6.0f;

        // Check if there is a close enough point
        const float MIN_DISTANCE = 0.1f;
        for (auto point: medialAxisPoints){
            if (glm::distance(center, point->getPoint()) < MIN_DISTANCE){
                keptCouples.insert(edge);
                break;
            }
        }
    }

    // Now we have the triangle couples that are going to be merged
    std::vector<glm::uvec2> mergedPairs; // Since we merge one triangle vertex with another one
    for (auto edge: keptCouples){
        glm::uvec3 triangleA = triangles[edge.a];
        glm::uvec3 triangleB = triangles[edge.b];


    }
    // TODO: return a vector of uvec2 that is a POLYGON, every uvec2 is an edge of the polygon.
}
