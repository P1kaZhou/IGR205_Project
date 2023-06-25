//
// Created by daniel on 16/06/23.
//

#include "operations.h"

/*
// This is a horror complexity-wise
void operations::merge(std::vector<glm::vec3> &positions1, std::vector<glm::uvec3> &faces1,
                       std::vector<glm::vec3> &positions2, std::vector<glm::uvec3> &faces2) {
    // TODO: Implement + use *NOT* CGAL libs
    // The algorithm is the following
    // 1. Get the intersection of the two meshes, which can be assimilated to a closed loop
    // 2. Each surface mesh is accommodated to the intersection of the loop (cf https://www.cs.tau.ac.il/~dcor/articles/2007/A-Sketch-Based.pdf)
    // 3. Discard the triangles inside the other surface
    // 4. Merge the two meshes along the shared boundary (if necessary, create new vertices by splitting boundary edges)
    // 5. Apply localized Laplacian smoothing (include vertex which closest vertex from the intersection loop is vi, if
    // dist(vertex, vi) < 0.6 dist(vi, center)

    std::vector<glm::vec3> intersectionPoints;

    // How to find the points?
    // Current solution: iterate through the *triangles* of the mesh, and check if the triangle intersects the other mesh

    // 1. Get the intersection of the two meshes, which can be assimilated to a closed loop
    for (auto triangle1: faces1) {
        unsigned a1 = triangle1.x;
        glm::vec3 pa1 = positions1[a1];
        unsigned b1 = triangle1.y;
        glm::vec3 pb1 = positions1[b1];
        unsigned c1 = triangle1.z;
        glm::vec3 pc1 = positions1[c1];
        for (auto triangle2: faces2) {
            unsigned a2 = triangle2.x;
            glm::vec3 pa2 = positions2[a2];
            unsigned b2 = triangle2.y;
            glm::vec3 pb2 = positions2[b2];
            unsigned c2 = triangle2.z;
            glm::vec3 pc2 = positions2[c2];

            Geometry::Edge E2ab(a2, b2);
            Geometry::Edge E2bc(b2, c2);
            Geometry::Edge E2ca(c2, a2);

            glm::vec3 pab = Geometry::planeToLineIntersection(pa1,pb1, pc1,
                                                              pa2, pb2);
            glm::vec3 pbc = Geometry::planeToLineIntersection(pa1,pb1, pc1,
                                                                pb2, pc2);
            glm::vec3 pca = Geometry::planeToLineIntersection(pa1,pb1, pc1,
                                                                pc2, pa2);
            // Of course, they are going to intersect, but if the intersection is inside the triangle a1b1c1, then this
            // point is an intersection point

            if (Geometry::triangleArea(pa1, pb1, pab) + Geometry::triangleArea(pa1, pab, pbc) +
                Geometry::triangleArea(pa1, pbc, pc1) == Geometry::triangleArea(pa1, pb1, pc1)) {
                intersectionPoints.push_back(pab);
            }
        }
    }

    // Order the points
    glm::vec3 ogPoint = intersectionPoints[0];
    std::vector<glm::vec3> orderedPoints;
    orderedPoints.push_back(ogPoint);

    // Get the closest point to the ogPoint and keep going
    while (intersectionPoints.size() > 0) {
        glm::vec3 pointToConsider = orderedPoints[orderedPoints.size() - 1];
        float minDist = std::numeric_limits<float>::max();
        glm::vec3 closestPoint;
        for (auto point: intersectionPoints) {
            float dist = glm::distance(pointToConsider, point);
            if (dist < minDist && std::find(orderedPoints.begin(), orderedPoints.end(), point) == orderedPoints.end()) {
                minDist = dist;
                closestPoint = point;
            }
        }
        orderedPoints.push_back(closestPoint);
        ogPoint = closestPoint;
        intersectionPoints.erase(std::remove(intersectionPoints.begin(), intersectionPoints.end(), closestPoint), intersectionPoints.end());
    }

    glm::vec3 center = glm::vec3(0,0,0);
    for (auto point: orderedPoints) {
        center += point;
    }
    center /= orderedPoints.size();

    // Get a plan equation from the points
    // Or do we need one actually?
    // TODO: figure that out


    // 2. Each surface mesh is accommodated to the intersection of the loop (cf https://www.cs.tau.ac.il/~dcor/articles/2007/A-Sketch-Based.pdf)
    // We treat both meshes separately

    // 2.1. Mesh 1
    // Get the edge path
    std::set<Geometry::Edge> edgePath;
    for (unsigned i = 0; i < orderedPoints.size(); i++) {
        glm::vec3 point1 = orderedPoints[i];
        glm::vec3 point2 = orderedPoints[(i + 1) % orderedPoints.size()];
        float minDistSum = std::numeric_limits<float>::max();
        std::set<Geometry::Edge> visitedEdges;

        unsigned closestPoint1;
        unsigned closestPoint2;
        for (auto triangle1: faces1) {
            unsigned a1 = triangle1.x;
            glm::vec3 pa1 = positions1[a1];
            unsigned b1 = triangle1.y;
            glm::vec3 pb1 = positions1[b1];
            unsigned c1 = triangle1.z;
            glm::vec3 pc1 = positions1[c1];

            Geometry::Edge E1ab(a1, b1);
            Geometry::Edge E1bc(b1, c1);
            Geometry::Edge E1ca(c1, a1);

            if (visitedEdges.find(E1ab) == visitedEdges.end()) {
                visitedEdges.insert(E1ab);
                float dist1 = std::min(glm::distance(point1, pa1), glm::distance(point1, pb1));
                float dist2 = std::min(glm::distance(point2, pa1), glm::distance(point2, pb1));
                float distSum = dist1 + dist2;
                if (distSum < minDistSum) {
                    minDistSum = distSum;
                    closestPoint1 = a1;
                    closestPoint2 = b1;
                }
            }
            if (visitedEdges.find(E1bc) == visitedEdges.end()) {
                visitedEdges.insert(E1bc);
                float dist1 = std::min(glm::distance(point1, pb1), glm::distance(point1, pc1));
                float dist2 = std::min(glm::distance(point2, pb1), glm::distance(point2, pc1));
                float distSum = dist1 + dist2;
                if (distSum < minDistSum) {
                    minDistSum = distSum;
                    closestPoint1 = b1;
                    closestPoint2 = c1;
                }
            }
            if (visitedEdges.find(E1ca) == visitedEdges.end()) {
                visitedEdges.insert(E1ca);
                float dist1 = std::min(glm::distance(point1, pc1), glm::distance(point1, pa1));
                float dist2 = std::min(glm::distance(point2, pc1), glm::distance(point2, pa1));
                float distSum = dist1 + dist2;
                if (distSum < minDistSum) {
                    minDistSum = distSum;
                    closestPoint1 = c1;
                    closestPoint2 = a1;
                }
            }
        }
        edgePath.insert(Geometry::Edge(closestPoint1, closestPoint2));
    }

    // Transform edgePath into a vector for ease of access
    std::vector<Geometry::Edge> edgePathVector1;
    for (auto edge: edgePath) {
        edgePathVector1.push_back(edge);
    }

    // Important part: verify that the path is closed
    // If not, we need to add the missing edges
    // We do that by finding the closest point to the last point in the path
    // and adding the edge between the two
    // Hopefully it won't matter for now

    // 2.2. Mesh 2
    edgePath.clear();
    for (unsigned i = 0; i < orderedPoints.size(); i++) {
        glm::vec3 point1 = orderedPoints[i];
        glm::vec3 point2 = orderedPoints[(i + 1) % orderedPoints.size()];
        float minDistSum = std::numeric_limits<float>::max();
        std::set<Geometry::Edge> visitedEdges;

        unsigned closestPoint1;
        unsigned closestPoint2;
        for (auto triangle2: faces2) {
            unsigned a2 = triangle2.x;
            glm::vec3 pa1 = positions2[a2];
            unsigned b2 = triangle2.y;
            glm::vec3 pb1 = positions2[b2];
            unsigned c2 = triangle2.z;
            glm::vec3 pc1 = positions2[c2];

            Geometry::Edge E1ab(a2, b2);
            Geometry::Edge E1bc(b2, c2);
            Geometry::Edge E1ca(c2, a2);

            if (visitedEdges.find(E1ab) == visitedEdges.end()) {
                visitedEdges.insert(E1ab);
                float dist1 = std::min(glm::distance(point1, pa1), glm::distance(point1, pb1));
                float dist2 = std::min(glm::distance(point2, pa1), glm::distance(point2, pb1));
                float distSum = dist1 + dist2;
                if (distSum < minDistSum) {
                    minDistSum = distSum;
                    closestPoint1 = a2;
                    closestPoint2 = b2;
                }
            }
            if (visitedEdges.find(E1bc) == visitedEdges.end()) {
                visitedEdges.insert(E1bc);
                float dist1 = std::min(glm::distance(point1, pb1), glm::distance(point1, pc1));
                float dist2 = std::min(glm::distance(point2, pb1), glm::distance(point2, pc1));
                float distSum = dist1 + dist2;
                if (distSum < minDistSum) {
                    minDistSum = distSum;
                    closestPoint1 = b2;
                    closestPoint2 = c2;
                }
            }
            if (visitedEdges.find(E1ca) == visitedEdges.end()) {
                visitedEdges.insert(E1ca);
                float dist1 = std::min(glm::distance(point1, pc1), glm::distance(point1, pa1));
                float dist2 = std::min(glm::distance(point2, pc1), glm::distance(point2, pa1));
                float distSum = dist1 + dist2;
                if (distSum < minDistSum) {
                    minDistSum = distSum;
                    closestPoint1 = c2;
                    closestPoint2 = a2;
                }
            }
        }
        edgePath.insert(Geometry::Edge(closestPoint1, closestPoint2));
    }

    // Transform edgePath into a vector for ease of access
    std::vector<Geometry::Edge> edgePathVector2;
    for (auto edge: edgePath) {
        edgePathVector2.push_back(edge);
    }


    // 3. Discard the triangles inside the other surface
    // 3.1. Mesh 1

    std::vector<glm::vec3> newpositions1;
    std::vector<glm::vec3> newnormals1;

} */

void operations::mergeCustom(std::vector<glm::vec3> &positions1, std::vector<glm::uvec3> &faces1,
                             std::vector<glm::vec3> &positions2, std::vector<glm::uvec3> &faces2) {

    // Custom algorithm

    // 1. discard all the points from mesh 1 that are inside mesh 2 and vice versa
    // 2. do the same for triangles
    // 3. ???

    // 1. discard all the points from mesh 1 that are inside mesh 2 and vice versa
    // 1.1. Mesh 1
    std::vector<glm::vec3> newpositions1;
    std::set<unsigned> removedIndexes1;
    for (int i = 0; i < positions1.size(); i++) {
        glm::vec3 position1 = positions1[i];
        glm::vec3 rayDirection = glm::vec3(0.f, 0.f, 1.f);
        int nbIntersections = 0;
        for (auto triangle2: faces2) {
            glm::vec3 a2 = positions2[triangle2.x];
            glm::vec3 b2 = positions2[triangle2.y];
            glm::vec3 c2 = positions2[triangle2.z];

            glm::vec3 intersection = Geometry::planeToLineIntersection(a2, b2, c2,
                                                                       position1, position1 + rayDirection);

            if (Geometry::triangleArea(a2, b2, c2) - Geometry::triangleArea(a2, b2, intersection) -
                Geometry::triangleArea(b2, c2, intersection) - Geometry::triangleArea(c2, a2, intersection) < 0.0001f) {
                nbIntersections++;
            }

            if (nbIntersections % 2 == 0) {
                newpositions1.push_back(position1);
            } else {
                removedIndexes1.insert(i);
            }
        }
    }
    // 1.2. Mesh 2
    std::vector<glm::vec3> newpositions2;
    std::set<unsigned> removedIndexes2;
    for (int i = 0; i < positions2.size(); i++) {
        glm::vec3 position2 = positions2[i];
        glm::vec3 rayDirection = glm::vec3(0.f, 0.f, 1.f);
        int nbIntersections = 0;
        for (auto triangle1: faces1) {
            glm::vec3 a1 = positions1[triangle1.x];
            glm::vec3 b1 = positions1[triangle1.y];
            glm::vec3 c1 = positions1[triangle1.z];

            glm::vec3 intersection = Geometry::planeToLineIntersection(a1, b1, c1,
                                                                       position2, position2 + rayDirection);

            if (Geometry::triangleArea(a1, b1, c1) - Geometry::triangleArea(a1, b1, intersection) -
                Geometry::triangleArea(b1, c1, intersection) - Geometry::triangleArea(c1, a1, intersection) < 0.0001f) {
                nbIntersections++;
            }

            if (nbIntersections % 2 == 0) {
                newpositions2.push_back(position2);
            } else {
                removedIndexes2.insert(i);
            }
        }
    }

    // 2. do the same for triangles

    // 2.1. Mesh 1
    std::vector<glm::uvec3> newfaces1;
    for (auto triangle: faces1) {
        unsigned a = triangle.x;
        unsigned b = triangle.y;
        unsigned c = triangle.z;
        if (removedIndexes1.find(a) == removedIndexes1.end() &&
            removedIndexes1.find(b) == removedIndexes1.end() &&
            removedIndexes1.find(c) == removedIndexes1.end()) {
            newfaces1.push_back(triangle);
        }
    }

    // 2.2. Mesh 2
    std::vector<glm::uvec3> newfaces2;
    for (auto triangle: faces2) {
        unsigned a = triangle.x;
        unsigned b = triangle.y;
        unsigned c = triangle.z;
        if (removedIndexes2.find(a) == removedIndexes2.end() &&
            removedIndexes2.find(b) == removedIndexes2.end() &&
            removedIndexes2.find(c) == removedIndexes2.end()) {
            newfaces2.push_back(triangle);
        }
    }

    // 3. Get the list of edges we need to connect
    // 3.1. Mesh 1
    std::map<Geometry::Edge, int> edgeMap1;
    for (auto triangle: faces1) {
        Geometry::Edge E1ab = Geometry::Edge(triangle.x, triangle.y);
        Geometry::Edge E1bc = Geometry::Edge(triangle.y, triangle.z);
        Geometry::Edge E1ca = Geometry::Edge(triangle.z, triangle.x);
        if (edgeMap1.find(E1ab) == edgeMap1.end()) {
            edgeMap1.insert(std::pair<Geometry::Edge, int>(E1ab, 1));
        } else {
            edgeMap1[E1ab]++;
        }
        if (edgeMap1.find(E1bc) == edgeMap1.end()) {
            edgeMap1.insert(std::pair<Geometry::Edge, int>(E1bc, 1));
        } else {
            edgeMap1[E1bc]++;
        }
        if (edgeMap1.find(E1ca) == edgeMap1.end()) {
            edgeMap1.insert(std::pair<Geometry::Edge, int>(E1ca, 1));
        } else {
            edgeMap1[E1ca]++;
        }
    }
    std::set<Geometry::Edge> edgesNeedingExtension1;
    for (auto edge: edgeMap1) {
        if (edge.second == 1) {
            edgesNeedingExtension1.insert(edge.first);
        }
    }

    // 3.2. Mesh 2
    std::map<Geometry::Edge, int> edgeMap2;

    for (auto triangle: faces2) {
        Geometry::Edge E2ab = Geometry::Edge(triangle.x, triangle.y);
        Geometry::Edge E2bc = Geometry::Edge(triangle.y, triangle.z);
        Geometry::Edge E2ca = Geometry::Edge(triangle.z, triangle.x);
        if (edgeMap2.find(E2ab) == edgeMap2.end()) {
            edgeMap2.insert(std::pair<Geometry::Edge, int>(E2ab, 1));
        } else {
            edgeMap2[E2ab]++;
        }
        if (edgeMap2.find(E2bc) == edgeMap2.end()) {
            edgeMap2.insert(std::pair<Geometry::Edge, int>(E2bc, 1));
        } else {
            edgeMap2[E2bc]++;
        }
        if (edgeMap2.find(E2ca) == edgeMap2.end()) {
            edgeMap2.insert(std::pair<Geometry::Edge, int>(E2ca, 1));
        } else {
            edgeMap2[E2ca]++;
        }
    }

    std::set<Geometry::Edge> edgesNeedingExtension2;
    for (auto edge: edgeMap2) {
        if (edge.second == 1) {
            edgesNeedingExtension2.insert(edge.first);
        }
    }

    // 4. Extend the edges

    const int POINTS_OFFSET = newpositions1.size();

    for (auto edge: edgesNeedingExtension1) {
        glm::vec3 p1 = newpositions1[edge.a];
        glm::vec3 p2 = newpositions1[edge.b];

        float minDistance1 = std::numeric_limits<float>::max();
        float minDistance2 = std::numeric_limits<float>::max();
        unsigned closestPoint1;
        unsigned closestPoint2;

        for (auto candidate: edgesNeedingExtension2) {
            glm::vec3 q1 = newpositions2[candidate.a];
            glm::vec3 q2 = newpositions2[candidate.b];

            unsigned index1 = candidate.a;
            unsigned index2 = candidate.b;

            float dfrom1to1 = glm::distance(p1, q1);
            float dfrom1to2 = glm::distance(p1, q2);
            float dfrom2to1 = glm::distance(p2, q1);
            float dfrom2to2 = glm::distance(p2, q2);
            if (dfrom1to1 < minDistance1 && dfrom1to1 < dfrom1to2) {
                minDistance1 = glm::distance(p1, q1);
                closestPoint1 = index1;
            }
            if (dfrom1to2 < minDistance1 && dfrom1to2 < dfrom1to1) {
                minDistance1 = glm::distance(p1, q2);
                closestPoint1 = index2;
            }
            if (dfrom2to1 < minDistance2 && dfrom2to1 < dfrom2to2) {
                minDistance2 = glm::distance(p2, q1);
                closestPoint2 = index1;
            }
            if (dfrom2to2 < minDistance2 && dfrom2to2 < dfrom2to1) {
                minDistance2 = glm::distance(p2, q2);
                closestPoint2 = index2;
            }
        } // CANDIDATE FOUND

        // Add the new points
        glm::uvec3 newTriangle1 = glm::uvec3(edge.a, edge.b, closestPoint1 + POINTS_OFFSET);
        glm::uvec3 newTriangle2 = glm::uvec3(edge.b, closestPoint2 + POINTS_OFFSET, closestPoint1 + POINTS_OFFSET);

        newfaces1.push_back(newTriangle1);
        newfaces1.push_back(newTriangle2);
    }

    newpositions1.insert(newpositions1.end(), newpositions2.begin(), newpositions2.end());
    newfaces1.insert(newfaces1.end(), newfaces2.begin(), newfaces2.end());

    // 5. Apply either a localized smoothing or a global smoothing
}
