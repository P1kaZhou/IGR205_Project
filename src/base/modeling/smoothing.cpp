//
// Created by daniel on 22/05/23.
//

#include "smoothing.hpp"

void smoothing::setLambda(float lambda) {
    this->lambda = lambda;
}

void smoothing::applyLaplacianToMedial(MedialAxisGenerator &medialAxisG, int iterations) {
    MedialAxis &medialAxis = medialAxisG.getMedialAxis();
    for (int k = 0; k < iterations; k++) {
        const std::vector<MedialAxisPoint *> points = medialAxis.getPoints();
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

std::vector<glm::uvec2> smoothing::computeNormalChordalAxes(MedialAxisGenerator &medialAxisG,
                                                            ConstrainedDelaunayTriangulation2D &cdt,
                                                            std::vector<glm::vec2> &sketchPoints) {
    // There may actually be no need for the triangles, I can just go through the chordal axis and create a normal segment
    const MedialAxis &medialAxis = medialAxisG.getMedialAxis();
    const std::vector<MedialAxisPoint *> points = medialAxis.getPoints();
    std::vector<glm::uvec3> triangles = cdt.getTriangles();

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
                //TODO: verify that the new chordal axis doesn't intersect with all the previous ones
            }
        }
    }
    return normalChordalAxes;
}

//TODO: make sure the operations are done in the right order, signature something as well

// Crashes for weird shapes, fix it later
void smoothing::insignificantBranchesRemoval(MedialAxisGenerator &medialAxisG, float threshold,
                                             std::vector<glm::vec2> sketchPoints) {
    MedialAxis &medialAxis = medialAxisG.getMedialAxis();
    // The threshold represents the ratio of morphological significance, p/AB

    // The main steps are:
    // 1. Get the potential candidates for removal. An axis is candidate if only one end of
    // the axe is connected to a junction
    // 2. For each candidate, compute the ratio of morphological significance, which is the ratio of the distance
    // to the junction and the length of the "opposition" side
    // 3. Should the ratio be below the threshold, remove the candidate

    // At this point it'd probably be better to remove the triangles immediately, followed by the chordal axis extension
    std::vector<std::vector<glm::vec2>> externalAxis = medialAxisG.extractExternalAxis();
    // From this point onward, we iterate through the axis and compute their ratio of morphological significance
    std::vector<glm::uvec3> trianglesToRemove;

    std::vector<glm::uvec3> junctionTriangles = medialAxisG.getJunctionTriangles();
    for (auto axis: externalAxis) {
        std::cout << "axis size: " << axis.size() << std::endl;
        // Get the first and the last element
        glm::vec2 firstPoint = axis[0];
        glm::vec2 lastPoint = axis[axis.size() - 2];
        // TODO: check if -2 is better than -1 because -1 will usually be the center
        // Get the closest junction point
        float minDistance = std::numeric_limits<float>::max();
        glm::uvec2 triangleEdge;
        glm::uvec3 triangleToRemove;

        for (auto triangle: junctionTriangles) {
            // as the chordal points are based from the middle of the triangle edges, we proceed this way
            unsigned a = triangle[0];
            unsigned b = triangle[1];
            unsigned c = triangle[2];

            glm::vec2 middlePointAB = (sketchPoints[a] + sketchPoints[b]) / 2.0f;
            glm::vec2 middlePointBC = (sketchPoints[b] + sketchPoints[c]) / 2.0f;
            glm::vec2 middlePointCA = (sketchPoints[c] + sketchPoints[a]) / 2.0f;

            float distanceAB1 = glm::distance(firstPoint, middlePointAB);
            float distanceBC1 = glm::distance(firstPoint, middlePointBC);
            float distanceCA1 = glm::distance(firstPoint, middlePointCA);
            float distanceAB2 = glm::distance(lastPoint, middlePointAB);
            float distanceBC2 = glm::distance(lastPoint, middlePointBC);
            float distanceCA2 = glm::distance(lastPoint, middlePointCA);
            float minDistances = std::min(std::min(std::min(distanceAB1, distanceBC1), distanceCA1),
                                          std::min(std::min(distanceAB2, distanceBC2), distanceCA2));

            if (distanceAB1 < minDistance && distanceAB1 == minDistances) {
                minDistance = distanceAB1;
                triangleEdge = glm::uvec2(a, b);
                triangleToRemove = triangle;
            } else if (distanceBC1 < minDistance && distanceBC1 == minDistances) {
                minDistance = distanceBC1;
                triangleEdge = glm::uvec2(b, c);
                triangleToRemove = triangle;
            } else if (distanceCA1 < minDistance && distanceCA1 == minDistances) {
                minDistance = distanceCA1;
                triangleEdge = glm::uvec2(c, a);
                triangleToRemove = triangle;
            } else if (distanceAB2 < minDistance && distanceAB2 == minDistances) {
                minDistance = distanceAB2;
                triangleEdge = glm::uvec2(a, b);
                triangleToRemove = triangle;
            } else if (distanceBC2 < minDistance && distanceBC2 == minDistances) {
                minDistance = distanceBC2;
                triangleEdge = glm::uvec2(b, c);
                triangleToRemove = triangle;
            } else if (distanceCA2 < minDistance && distanceCA2 == minDistances) {
                minDistance = distanceCA2;
                triangleEdge = glm::uvec2(c, a);
                triangleToRemove = triangle;
            }
        } // Iteration over the triangles ends here

        trianglesToRemove.push_back(triangleToRemove);
        bool found = false;
        const float CLOSE_ENOUGH = 1.f; // Something small is relevant considering they should "perfectly match<"
        if (minDistance < CLOSE_ENOUGH) {
            found = true;
            // Compute the ratio of morphological significance
            float morpho = glm::distance(firstPoint, lastPoint) /
                           glm::distance(sketchPoints[triangleEdge[0]], sketchPoints[triangleEdge[1]]);
            if (morpho < threshold) {
                // Courtesy of Ghislain
                unsigned i = 0;
                for (auto a: axis) {
                    if (i < axis.size() - 1) {
                        medialAxis.removePoint(a);
                    }
                    i++;
                }
            }
        }

        if (!found) {
            unsigned i = 0;
            for (auto a: axis) {
                if (i < axis.size() - 1) {
                    medialAxis.removePoint(a);
                }
                i++;
            }
        }
    }

    // Once all axis are deleted, we shall proceed to the triangle deletion as well as the chordal axis extension
    // The triangle deletion is straightforward considering we have the list of triangles to remove, and the triangles
    // are passed by reference
    // For the chordal axis extension, for the deleted axes, we first check if they have a common "kept" neighbor
    // (hence the `isLastPointClosest` boolean*), this is to avoid duplicates
    // Then we get the general direction of the "internal" axis that stops at the kept neighbor, and we extend it until
    // it reaches one sketchpoint. The step size should be the same as what separates the points in the medial axis

    // We can estimate the number of points to add by computing the distance between the kept neighbor and the
    // farthest sketchpoint following the general direction of the internal axis, and dividing it by the step size

    // Remove the triangles
    // Later

    std::cout << "bye" << std::endl;

    medialAxisG.smooth(1);

    extendAxis(medialAxisG, sketchPoints);

    std::cout << "bye 2" << std::endl;
}

// TODO: either modify this or use the resources provided by Ghislain
std::vector<glm::uvec3> smoothing::computeJunctionTriangles(ConstrainedDelaunayTriangulation2D &cdt) {
    std::vector<Geometry::Edge> &edges = cdt.getEdges();
    std::vector<glm::uvec3> &triangles = cdt.getTriangles();

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

// Returns: a vector of connecting indexes
std::vector<glm::uvec2>
smoothing::computeConnectingRegion(std::vector<glm::uvec3> &triangles, std::vector<glm::vec2> &points,
                                   MedialAxisGenerator &medialAxisG) {
    // At this point, we only have the significant triangles
    MedialAxis &medialAxis = medialAxisG.getMedialAxis();
    int trianglesNumber = triangles.size(); // To achieve the merging, we will need to compare respective distances

    const std::vector<MedialAxisPoint *> medialAxisPoints = medialAxis.getPoints();

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

// A separate function for convenience
// extends the axis with respect to the points to add
void smoothing::extendAxis(MedialAxisGenerator &medialAxisG,
                           std::vector<glm::vec2> &sketchPoints) {
    MedialAxis &medialAxis = medialAxisG.getMedialAxis();

    // First step: get the external axis to extend (they are smoothened out first)
    std::vector<std::vector<glm::vec2>> externalAxisPruned = medialAxisG.extractExternalAxis();

    int numberOfAxes = externalAxisPruned.size();

    if (numberOfAxes > 1) {

        for (auto pointAxis: externalAxisPruned) {
            glm::vec2 pointToAdd = pointAxis[0];

            glm::vec2 gradient = glm::vec2(0.0f);

            int pointAxisSize = pointAxis.size();
            // Compute the cumulative gradient
            int gradientAccuracy = 2;
            if (pointAxisSize < 2) {
                gradientAccuracy = 1;
            }
            for (int i = 1; i < gradientAccuracy + 1; i++) {
                glm::vec2 gradientToAdd = (pointAxis[0] - pointAxis[i]) / float(gradientAccuracy);
                gradient += gradientToAdd;
            }

            // At this point we have the gradient that comes from the axis where the point belongs to

            // Check if the new point is inside the polygon
            // We are going to use lineToLineIntersectionCoef from the Geometry package
            float t1, t2;
            glm::vec2 intersectionPoint;

            std::map<float, float> tmap;
            for (int i = 0; i < sketchPoints.size(); i++) {
                glm::vec2 a = sketchPoints[i];
                glm::vec2 b = sketchPoints[(i + 1) % sketchPoints.size()];
                Geometry::lineToLineIntersectionCoef(pointToAdd, gradient, a, b - a, t1, t2);
                if (t2 > 0.0f && t2 < 1.0f && t1 > 0) {
                    tmap.insert({t1, t2});
                }
            }

            // get the smallest key
            float smallestKey = tmap.begin()->first;
            for (auto it = tmap.begin(); it != tmap.end(); it++) {
                if (it->first < smallestKey) {
                    smallestKey = it->first;
                }
            }
            t1 = smallestKey;
            intersectionPoint = pointToAdd + gradient * t1;
            // as for now
            medialAxis.insertEdge(pointToAdd, intersectionPoint);
        }
    } else {

        std::vector<glm::vec2> singleAxis = externalAxisPruned[0];
        glm::vec2 startingPoint = singleAxis[0];
        glm::vec2 endingPoint = singleAxis[singleAxis.size() - 1];

        glm::vec2 gradientStart;
        glm::vec2 gradientEnd;
        if (singleAxis.size() == 2) { // ?? Ripbozo
            gradientStart = startingPoint - endingPoint;
            gradientEnd = -gradientStart;
        } else if (singleAxis.size() == 1){
            std::cout << "axis is alone bruv" << std::endl;
        } else {
            // Cumulative sum
            gradientStart = singleAxis[0] - singleAxis[1]/float(2) - singleAxis[2]/float(2);
            gradientEnd = singleAxis[singleAxis.size() - 1] - singleAxis[singleAxis.size() - 2]/float(2)
                    - singleAxis[singleAxis.size() - 3]/float(2);

            float t1, t2;

            std::map<float, float> tmapStart;
            std::map<float, float> tmapEnd;

            for (int i = 0; i < sketchPoints.size(); i++) {
                glm::vec2 a = sketchPoints[i];
                glm::vec2 b = sketchPoints[(i + 1) % sketchPoints.size()];
                Geometry::lineToLineIntersectionCoef(startingPoint, gradientStart, a, b - a, t1, t2);
                if (t2 > 0.0f && t2 < 1.0f && t1 > 0) {
                    tmapStart.insert({t1, t2});
                }
                Geometry::lineToLineIntersectionCoef(endingPoint, gradientEnd, a, b - a, t1, t2);
                if (t2 > 0.0f && t2 < 1.0f && t1 > 0) {
                    tmapEnd.insert({t1, t2});
                }
            }

            float smallestKeyStart = tmapStart.begin()->first;
            float smallestKeyEnd = tmapEnd.begin()->first;

            for (auto it = tmapStart.begin(); it!=tmapStart.end(); it++){
                if (it->first < smallestKeyStart){
                    smallestKeyStart = it->first;
                }
            }
            for (auto it = tmapEnd.begin(); it!=tmapEnd.end(); it++){
                if (it->first < smallestKeyEnd){
                    smallestKeyEnd = it->first;
                }
            }

            float t1Start = smallestKeyStart;
            float t1End = smallestKeyEnd;

            glm::vec2 intersectionStart = startingPoint + gradientStart*t1Start;
            glm::vec2 intersectionEnd = endingPoint + gradientEnd*t1End;

            medialAxis.insertEdge(startingPoint, intersectionStart);
            medialAxis.insertEdge(endingPoint, intersectionEnd);
        }
    }
}

void smoothing::middleMedialAxis(std::vector<glm::uvec2> chordalAxes, MedialAxisGenerator &medialAxisG,
                                 std::vector<glm::vec2> &sketchPoints) {

    // The goal is to modify the medial axis by changing each of their points closer to the center

    MedialAxis &medialAxis = medialAxisG.getMedialAxis();
    std::vector<MedialAxisPoint *> points = medialAxis.getPoints();

    for (auto axe: chordalAxes){
        glm::vec2 a = sketchPoints[axe.x];
        glm::vec2 b = sketchPoints[axe.y];
        glm::vec2 center = (a + b) / 2.0f;
        // Get assigned point to the center
        float minDistance = std::numeric_limits<float>::max();
        MedialAxisPoint *closestPoint = nullptr;
        for (auto potentialPoint: points){
            if (glm::distance(potentialPoint->getPoint(), center) < minDistance){
                minDistance = glm::distance(potentialPoint->getPoint(), center);
                closestPoint = potentialPoint;
            }
        }

        // Now we have the closest point, we need to move it closer to the center

    }

}
