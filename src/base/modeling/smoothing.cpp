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
            std::vector<MedialAxisPoint *> neighbors = point->getAdjs();
            int v = neighbors.size();
            // Iterate through every neighbor of the point
            for (auto const &neighbor: neighbors) {
                newPoint += lambda / v * neighbor->getPoint();
            }

        }

    }
}

std::vector<glm::uvec2> smoothing::computeNormalChordalAxes(MedialAxis &medialAxis, ConstrainedDelaunayTriangulation2D &cdt,
                                                            std::vector<glm::vec2> sketchPoints) {
    std::vector<MedialAxisPoint*> points = medialAxis.getPoints();
    std::vector<glm::uvec3> triangles = cdt->getTriangles();

    std::vector<glm::uvec2> normalChordalAxes; // The axes are determines by two points from the sketchpoints

    std::set<Geometry::Edge> visitedEdges;

    // The idea:
    // Iterate through the triangles of the CDT, and "perpendicularize" them with the CAT from medial axis
    for (auto triangle: triangles){
        glm::vec2 a = sketchPoints[triangle.x];
        glm::vec2 b = sketchPoints[triangle.y];
        glm::vec2 c = sketchPoints[triangle.z];

        // Get the smallest angle of the triangle
        float abc = glm::acos(glm::dot(glm::normalize(a - b), glm::normalize(c - b)));
        float bca = glm::acos(glm::dot(glm::normalize(b - c), glm::normalize(a - c)));
        float cab = glm::acos(glm::dot(glm::normalize(c - a), glm::normalize(b - a)));

        // Now if abc is the smallest angle, the edges ab and bc are the ones that will be perpendicularized
        // And also added to the visited edges, that is
    }

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

std::vector<glm::uvec3>
smoothing::computeConnectingRegion(std::vector<glm::uvec3> &triangles, std::vector<glm::vec2> &points) {
    // At this point, we only have the significant triangles

    int trianglesNumber = triangles.size(); // To achieve the merging, we will need to compare respective distances
    std::vector<std::vector<float>> distances = std::vector<std::vector<float>>(trianglesNumber,
                                                                                std::vector<float>(trianglesNumber));
    // All the distances are initialized to 0, but they are going to be filled. It's a symmetric matrix but it doesn't matter
    for (int i = 0; i < trianglesNumber; i++) {
        for (int j = 0; j < trianglesNumber; j++) {
            if (i != j) {
                // The metric we are going to use will be the euclidean distance, from the centers of the triangles
                glm::vec2 center1 =
                        (points[triangles[i][0]] + points[triangles[i][1]] + points[triangles[i][2]]) / 3.0f;
                glm::vec2 center2 =
                        (points[triangles[j][0]] + points[triangles[j][1]] + points[triangles[j][2]]) / 3.0f;
                distances[i][j] = glm::distance(center1, center2);
                distances[j][i] = distances[i][j];
            }
        }
    }

    // Now we have the distances, we can start merging the triangles, but for this we need to know which triangle is going to be merged with which
}
