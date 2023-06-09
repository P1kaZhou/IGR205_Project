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

void smoothing::insignificantBranchesRemoval(MedialAxisGenerator &medialAxisG, float threshold,
                                             std::vector<glm::uvec3> triangles,
                                             std::vector<glm::vec2> sketchPoints) {
    // TODO: apply Prasad criteria
    // The threshold represents the ratio of morphological significance, p/AB

    //The main steps are:
    // 1. Get the potential candidates for removal. An axis is candidate if only one end of
    // the axe is connected to a junction
    // 2. For each candidate, compute the ratio of morphological significance, which is the ratio of the distance
    // to the junction and the length of the "opposition" side
    // 3. Should the ratio be below the threshold, remove the candidate

    // At this point it'd probably be better to remove the triangles immediately, followed by the chordal axis extension



    /* Realized it was not useful
    // STEP 1: get the candidates
    std::vector<MedialAxisPoint *> points = medialAxis.getPoints();
    std::map<int, int> candidates; // indexes of beginning and end of candidate axis


    int n = points.size();
    for (int i = 0; i < n ; i++){
        MedialAxisPoint * axisPoint = points[i];
        std::vector<MedialAxisPoint *> neighbors = axisPoint->getAdjs();
        if (neighbors.size() == 1){
            MedialAxisPoint * myFirstPoint = axisPoint;
            MedialAxisPoint * mySecondPoint = neighbors[0];
            while (mySecondPoint->getAdjs().size() == 2){
                std::vector<MedialAxisPoint *> mySecondPointNeighbors = mySecondPoint->getAdjs();
                if (mySecondPointNeighbors[0] == myFirstPoint){
                    mySecondPoint = mySecondPointNeighbors[1];
                } else {
                    mySecondPoint = mySecondPointNeighbors[0];
                }
            }
        }
    }
    */

}

std::vector<glm::uvec3> smoothing::computeJunctionTriangles(ConstrainedDelaunayTriangulation2D &cdt) {
    std::vector<Geometry::Edge> &edges = cdt->getEdges();
    std::vector<glm::uvec3> &triangles = cdt->getTriangles();

    std::vector<glm::uvec3> junctionTriangles;

    for (auto triangle: triangles) {
        // A triangle is a junction triangle if no edges are in the vector edges
        unsigned a = triangle[0];
        unsigned b = triangle[1];
        unsigned c = triangle[2];
        Geometry::Edge Eab = Geometry::Edge(a, b);
        Geometry::Edge Ebc = Geometry::Edge(b, c);
        Geometry::Edge Eca = Geometry::Edge(c, a);
        if (std::find(edges.begin(), edges.end(), Eab) == edges.end() &&
            std::find(edges.begin(), edges.end(), Ebc) == edges.end() &&
            std::find(edges.begin(), edges.end(), Eca) == edges.end()) {
            junctionTriangles.push_back(triangle);
        }
    }

    // Next step is merging the triangles that share one side
    // We will use a map to store the edges and the triangles that share them
    /*
    std::map<Geometry::Edge, std::vector<glm::uvec3>> edgeMap;
    for (auto triangle: junctionTriangles) {
        unsigned a = triangle[0];
        unsigned b = triangle[1];
        unsigned c = triangle[2];
        Geometry::Edge Eab = Geometry::Edge(a, b);
        Geometry::Edge Ebc = Geometry::Edge(b, c);
        Geometry::Edge Eca = Geometry::Edge(c, a);
        edgeMap[Eab].push_back(triangle);
        edgeMap[Ebc].push_back(triangle);
        edgeMap[Eca].push_back(triangle);
    }
    // Now we iterate through edgeMap and merge the triangles that share an edge
    std::vector<glm::uvec3> mergedTriangles;
    for (auto const &[key, val]: edgeMap) {
        int neighborsCount = val.size();
        if (neighborsCount == 2) {
            // Triangles needs to be merged
            // Get the two triangles
            glm::uvec3 triangleA = val[0];
            glm::uvec3 triangleB = val[1];
            unsigned a = key.a;
            unsigned b = key.b;
        }
    } Because I haven't thought of a viable solution yet
     */

    return junctionTriangles;
}

//Not needed
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

// Returns: a vector of connecting indexes
std::vector<glm::uvec2>
smoothing::computeConnectingRegion(std::vector<glm::uvec3> &triangles, std::vector<glm::vec2> &points,
                                   MedialAxis &medialAxis) {
    // At this point, we only have the significant triangles

    int trianglesNumber = triangles.size(); // To achieve the merging, we will need to compare respective distances

    std::vector<MedialAxisPoint *> medialAxisPoints = medialAxis.getPoints();

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

    for (auto edge: trianglesCouples) {
        glm::vec2 a = points[triangles[edge.a][0]];
        glm::vec2 b = points[triangles[edge.a][1]];
        glm::vec2 c = points[triangles[edge.a][2]];
        glm::vec2 d = points[triangles[edge.b][0]];
        glm::vec2 e = points[triangles[edge.b][1]];
        glm::vec2 f = points[triangles[edge.b][2]];

        glm::vec2 center = (a + b + c + d + e + f) / 6.0f;

        // Check if there is a close enough point
        const float MIN_DISTANCE = 0.1f;
        for (auto point: medialAxisPoints) {
            if (glm::distance(center, point->getPoint()) < MIN_DISTANCE) {
                keptCouples.insert(edge);
                break;
            }
        }
    }

    // Now we have the triangle couples that are going to be merged
    std::vector<glm::uvec2> mergedPairs; // Since we merge one triangle vertex with another one
    for (auto edge: keptCouples) {
        glm::uvec3 triangleA = triangles[edge.a];
        glm::uvec3 triangleB = triangles[edge.b];

        // Get the two couples of points that are going to be merged

        std::vector<float> distances = std::vector<float>(9);
        for (int i = 0; i < 3; i++) {
            glm::vec2 a = points[triangleA[i]];
            for (int j = 0; j < 3; j++) {
                glm::vec2 b = points[triangleB[j]];
                distances[i * 3 + j] = glm::distance(a, b);
            }
        }

        // Now we have the distances, we need to find the two smallest ones
        std::vector<float> distancesCopy = distances;
        std::sort(distancesCopy.begin(), distancesCopy.end());
        float smallestDistance = distancesCopy[0];
        float secondSmallestDistance = distancesCopy[1];

        // Now we need to find the indices of the two smallest distances
        int smallestIndex = -1;
        int secondSmallestIndex = -1;
        for (int i = 0; i < 9; i++) {
            if (distances[i] == smallestDistance) {
                smallestIndex = i;
            }
            if (distances[i] == secondSmallestDistance) {
                secondSmallestIndex = i;
            }
        }

        // Now we have the indices, we need to find the two points
        // We have the formula index = 3*indexA + indexB
        int indexA1 = (int) smallestIndex / 3;
        int indexB1 = smallestIndex % 3;
        int indexA2 = (int) secondSmallestIndex / 3;
        int indexB2 = secondSmallestIndex % 3;

        mergedPairs.push_back(glm::uvec2(triangleA[indexA1], triangleB[indexA2]));
        mergedPairs.push_back(glm::uvec2(triangleA[indexB1], triangleB[indexB2]));

    }

    return mergedPairs;
}