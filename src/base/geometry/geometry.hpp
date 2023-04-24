#ifndef _GUM3D_GEOMETRY_
#define _GUM3D_GEOMETRY_

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string>

namespace Geometry {

struct Edge {
    unsigned a, b;
    unsigned f1=UINT_MAX, f2=UINT_MAX;
    Edge(unsigned a, unsigned b, unsigned f1=UINT_MAX, unsigned f2=UINT_MAX)
    : a(glm::min(a,b)), b(glm::max(a,b)), f1(f1), f2(f2) {}

    bool operator<(Edge const & o) const {
        return a<o.a  ||  (a==o.a && b<o.b);
    }
    bool operator==(Edge const & o) const {
        return a==o.a && b==o.b;
    }

    unsigned getOtherVertex(unsigned v) {
        if(v==a) return b;
        else return a;
    }

    unsigned getVertexFace1(const std::vector<glm::uvec3> & faces) {
        if(f1==UINT_MAX) return 0;
        auto & face = faces[f1];
        for(unsigned v=0;v<3;v++) {
            if(v!=a && v!=b) return v;
        }
        return 0;
    }
    unsigned getVertexFace2(const std::vector<glm::uvec3> & faces) {
        if(f2==UINT_MAX) return 0;
        auto & face = faces[f2];
        for(unsigned v=0;v<3;v++) {
            if(v!=a && v!=b) return v;
        }
        return 0;
    }
};

inline std::ostream& operator<<(std::ostream & out, const Edge & e) {
    out << "Edge(" <<
    "a:" << e.a <<
    ", b:" << e.b <<
    ", f1:" << e.f1 <<
    ", f2:" << e.f2 <<
    ")";
    return out;
}

std::vector<Edge> extractEdgesFromFaces(const std::vector<glm::uvec3> & faces);

bool isBoundaryEdge(const Edge & edge);
std::vector<Edge> findBoundaryEdges(const std::vector<Edge> & edges);

bool isSharpEdge(
    const Edge & edge,
    float thresholdAngle,
    const std::vector<glm::vec3> & vertexPositions,
    const std::vector<glm::uvec3> & faces
);
std::vector<Edge> findSharpEdges(
    const std::vector<Edge> & edges,
    float thresholdAngle,
    const std::vector<glm::vec3> & vertexPositions,
    const std::vector<glm::uvec3> & faces
);

std::vector<unsigned> findVertexAdjacentVertices(
    unsigned vertexInd,
    const std::vector<Edge> & edges
);
std::vector<Edge> findVertexAdjacentEdges(
    unsigned vertexInd,
    const std::vector<Edge> & edges
);
std::vector<unsigned> findVertexAdjacentFaces(
    unsigned vertexInd,
    const std::vector<glm::uvec3> & faces
);

inline float computeAngle(const glm::vec3 & a, const glm::vec3 & b) {
    return glm::acos(glm::dot(a, b)/(glm::length(a)*glm::length(b)));
}
inline float triangleArea(const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c) {
    return glm::length(glm::cross(b-a,c-a))/2;
}
inline float triangleArea(const glm::vec2 & a, const glm::vec2 & b, const glm::vec2 & c) {
    return triangleArea(glm::vec3(a,0), glm::vec3(b,0), glm::vec3(c,0));
}
void triangleBarycentricCoords(
    const glm::vec2 & p, const glm::vec2 & a, const glm::vec2 & b, const glm::vec2 & c,
    float & u, float & v, float & w
);
void triangleBarycentricCoords(
    const glm::vec3 & p, const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c,
    float & u, float & v, float & w
);
bool isPointInTriangle(
    const glm::vec2 & p,
    const glm::vec2 & a,
    const glm::vec2 & b,
    const glm::vec2 & c
);


namespace BoundingBox {

/*Axis aligned bounding box.*/
typedef struct {
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;
} BoundingBox;

inline void showBoundingBox(const BoundingBox & b, const char * name="") {
    std::cout << name << " : " << std::endl;
    std::cout << b.minX << " " << b.maxX << std::endl;
    std::cout << b.minY << " " << b.maxY << std::endl;
    std::cout << b.minZ << " " << b.maxZ << std::endl;
}

BoundingBox computeBoundingBox(const std::vector<glm::vec3> & vertexPositions);
bool intervalIntersect(float a_, float b_, float c_, float d_);
bool intersectLine(
    const BoundingBox & b,
    const glm::vec3 & direction, const glm::vec3 & startPos,
    const glm::mat4 & transform = glm::mat4(1.0)
);

}

};

#endif
