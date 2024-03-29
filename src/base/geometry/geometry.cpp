#include "geometry.hpp"

namespace Geometry {

/*
Return the vector of all edges from the faces.
Complexity ~ O(Nlog(N)) for N faces.
*/
std::vector<Edge> extractEdgesFromFaces(
    const std::vector<glm::uvec3> & faces
) {
    std::set<Edge> edges;
    for(unsigned f=0;f<faces.size();f++) {
        const glm::uvec3 & face = faces[f];

        // Edge x-y
        auto e = edges.find(Edge(face.x, face.y)); // O(log#edges)
        if(e==edges.end()) {
            edges.insert(Edge(face.x, face.y, f)); // O(log#edges)
        }
        else {
            auto edge = *e;
            edge.f2 = f;
            edges.erase(e); // O(1)
            edges.insert(edge); // O(log#edges)
        }

        // Edge x-z
        e = edges.find(Edge(face.x, face.z)); // O(log#edges)
        if(e==edges.end()) {
            edges.insert(Edge(face.x, face.z, f)); // O(log#edges)
        }
        else {
            auto edge = *e;
            edge.f2 = f;
            edges.erase(e); // O(1)
            edges.insert(edge); // O(log#edges)
        }

        // Edge y-z
        e = edges.find(Edge(face.y, face.z)); // O(log#edges)
        if(e==edges.end()) {
            edges.insert(Edge(face.y, face.z, f)); // O(log#edges)
        }
        else {
            auto edge = *e;
            edge.f2 = f;
            edges.erase(e); // O(1)
            edges.insert(edge); // O(log#edges)
        }
    }
    std::vector<Edge> res;
    for(auto & e : edges) {
        res.push_back(e);
    }
    return res;
}

/*
Tells if an edge is a boundary (has a single adjacent face).
Complexity ~ O(1).
*/
bool isBoundaryEdge(const Edge & edge) {
    return
        (edge.f1!=UINT_MAX && edge.f2==UINT_MAX) ||
        (edge.f2!=UINT_MAX && edge.f1==UINT_MAX);
}

/*
Return the vector of boundary edges (edge with a single adjacent face).
Complexity ~ O(N) for N edges.
*/
std::vector<Edge> findBoundaryEdges(
    const std::vector<Edge> & edges
) {
    std::vector<Edge> boundaries;
    for(const Edge & edge : edges) {
        if(isBoundaryEdge(edge)) {
            boundaries.push_back(edge);
        }
    }
    return boundaries;
}

/*
Tells if the edge is sharp using the threshold angle between its ajacent faces.
Complexity ~ O(1).
*/
bool isSharpEdge(
    const Edge & edge,
    float thresholdAngle,
    const std::vector<glm::vec3> & vertexPositions,
    const std::vector<glm::uvec3> & faces
) {
    if(edge.f1==UINT_MAX || edge.f2==UINT_MAX) return false;
    glm::uvec3 f1 = faces[edge.f1];
    glm::uvec3 f2 = faces[edge.f2];
    glm::vec3 n1 = glm::cross(
        vertexPositions[f1.y]-vertexPositions[f1.x],
        vertexPositions[f1.z]-vertexPositions[f1.x]);
    glm::vec3 n2 = glm::cross(
        vertexPositions[f2.y]-vertexPositions[f2.x],
        vertexPositions[f2.z]-vertexPositions[f2.x]);
    n1 = glm::normalize(n1);
    n2 = glm::normalize(n2);
    float angle = glm::acos(glm::dot(n1, n2)/(glm::length(n1)*glm::length(n2)));
    return angle<thresholdAngle;
}

/*
Return the vector of sharp edges.
Complexity ~ O(N) for N edges.
*/
std::vector<Edge> findSharpEdges(
    const std::vector<Edge> & edges,
    float thresholdAngle,
    const std::vector<glm::vec3> & vertexPositions,
    const std::vector<glm::uvec3> & faces
) {
    std::vector<Edge> sharpEdges;
    for(const Edge & edge : edges) {
        if(isSharpEdge(edge, thresholdAngle, vertexPositions, faces)) {
            sharpEdges.push_back(edge);
        }
    }
    return sharpEdges;
}

/*
Return the vector of vertices adjacent to the vertex.
Complexity ~ O(N) for N edges.
*/
std::vector<unsigned> findVertexAdjacentVertices(
    unsigned vertexInd,
    const std::vector<Edge> & edges
) {
    std::vector<unsigned> adjs;
    for(const Edge & edge : edges) {
        if(edge.a==vertexInd) {
            adjs.push_back(edge.b);
        }
        else if(edge.b==vertexInd) {
            adjs.push_back(edge.a);
        }
    }
    return adjs;
}

/*
Return the vector of edges adjacent to the vertex.
Complexity ~ O(N) for N edges.
*/
std::vector<Edge> findVertexAdjacentEdges(
    unsigned vertexInd,
    const std::vector<Edge> & edges
) {
    std::vector<Edge> adjs;
    for(const Edge & edge : edges) {
        if(edge.a==vertexInd || edge.b==vertexInd) {
            adjs.push_back(edge);
        }
    }
    return adjs;
}

/*
Return the vector of indice of faces adjacent to the vertex.
Complexity ~ O(N) for N faces.
*/
std::vector<unsigned> findVertexAdjacentFaces(
    unsigned vertexInd,
    const std::vector<glm::uvec3> & faces
) {
    std::vector<unsigned> adjs;
    for(unsigned f=0;f<faces.size();f++) {
        const glm::uvec3 & face = faces[f];
        if(face.x==vertexInd || face.y==vertexInd || face.z==vertexInd) {
            adjs.push_back(f);
        }
    }
    return adjs;
}

/*Compute the baricentric coordinates of point p in triangle abc.*/
void triangleBarycentricCoords(
    const glm::vec2 & p, const glm::vec2 & a, const glm::vec2 & b, const glm::vec2 & c,
    float & u, float & v, float & w)
{
    float totalArea = triangleArea(a, b, c);
    v = triangleArea(c, a, p)/totalArea;
    w = triangleArea(a, b, p)/totalArea;
    u = triangleArea(b, c, p)/totalArea;
}
/*Compute the baricentric coordinates of point p in triangle abc.*/
void triangleBarycentricCoords(
    const glm::vec3 & p, const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c,
    float & u, float & v, float & w)
{
    float totalArea = triangleArea(a, b, c);
    v = triangleArea(c, a, p)/totalArea;
    w = triangleArea(a, b, p)/totalArea;
    u = triangleArea(b, c, p)/totalArea;
}

glm::vec2 triangleCircumcenter(
    const glm::vec2 & a, const glm::vec2 & b, const glm::vec2 & c
) {
    float la = glm::sin(2*glm::angle(glm::normalize(b-a), glm::normalize(c-a)));
    float lb = glm::sin(2*glm::angle(glm::normalize(a-b), glm::normalize(c-b)));
    float lc = glm::sin(2*glm::angle(glm::normalize(a-c), glm::normalize(b-c)));
    return {
        (a.x*la + b.x*lb + c.x*lc)/(la + lb + lc),
        (a.y*la + b.y*lb + c.y*lc)/(la + lb + lc)
    };
}

bool isPointInTriangle(
    const glm::vec2 & p,
    const glm::vec2 & a,
    const glm::vec2 & b,
    const glm::vec2 & c
) {
    float areaSum = triangleArea(c,a,p)+triangleArea(a,b,p)+triangleArea(b,c,p);
    return triangleArea(a,b,c)==areaSum;
}

/*Find the intersection point of a line with a plane.
- The plane is defined by two vectors that we assume being coplanar.
- The line is defined by two points.
We assume that the plane and the line are not parallel.*/
glm::vec3 planeToLineIntersection(
    const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3,
    const glm::vec3 & q1, const glm::vec3 & q2
) {
    glm::vec3 n = glm::cross(p2-p1, p3-p1);
    float t = glm::dot(p1-q1, n) / glm::dot(q2-q1, n);
    return q1 + t*(q2-q1);
}

void lineToLineIntersectionCoef(
    const glm::vec2 & a, const glm::vec2 & u,
    const glm::vec2 & b, const glm::vec2 & v,
    float & t1, float & t2
) {
    auto ab = a-b;
    // t1 = (ab.y - ab.x*v.y/v.x) / (u.x*v.y/v.x - u.y);
    t1 = (v.x*ab.y - ab.x*v.y) / (u.x*v.y - u.y*v.x);
    if(v.x!=0) t2 = (ab.x + t1*u.x) / v.x;
    if(v.y!=0) t2 = (ab.y + t1*u.y) / v.y;
}

glm::vec3 projectPointOnLine(
    const glm::vec3 & l1, const glm::vec3 & l2,
    const glm::vec3 & v
) {
    return (glm::dot(l2-l1, v)/((l2-l1).length()*(l2-l1).length())) * (l2-l1);
}

glm::vec2 projectPointOnLine(
    const glm::vec2 & l1, const glm::vec2 & l2,
    const glm::vec2 & v
) {
    return (glm::dot(l2-l1, v)/((l2-l1).length()*(l2-l1).length())) * (l2-l1);
}

/*
Return the closest chord to a point.
*/
const Geometry::Edge & getChordOnPoint(
    const glm::vec2 & p,
    const std::vector<glm::vec2> & points,
    const std::vector<Geometry::Edge> & chords
) {
    unsigned closestChord;
    float minArea = FLT_MAX;
    for(unsigned i=0; i<chords.size(); i++) {
        const auto & c = chords[i];
        // float area = Geometry::triangleArea(p, points[c.a], points[c.b]);
        float area = glm::distance(p, 0.5f*(points[c.a] + points[c.b]));
        if(minArea > area) {
            minArea = area;
            closestChord = i;
        }
    }
    return chords[closestChord];
}



namespace BoundingBox {

/*Return the axis aligned bounding box of the vertex positions.*/
BoundingBox computeBoundingBox(const std::vector<glm::vec3> & vertexPositions) {
    if(vertexPositions.size() == 0) return {};
    BoundingBox box = {
        vertexPositions.at(0).x, vertexPositions.at(0).x,
        vertexPositions.at(0).y, vertexPositions.at(0).y,
        vertexPositions.at(0).z, vertexPositions.at(0).z
    };
    for(glm::vec3 pos : vertexPositions) {
        if(pos.x > box.maxX) {
            box.maxX = pos.x;
        }
        if(pos.y > box.maxY) {
            box.maxY = pos.y;
        }
        if(pos.z > box.maxZ) {
            box.maxZ = pos.z;
        }

        if(pos.x < box.minX) {
            box.minX = pos.x;
        }
        if(pos.y < box.minY) {
            box.minY = pos.y;
        }
        if(pos.z < box.minZ) {
            box.minZ = pos.z;
        }
    }
    return box;
}

bool intervalIntersect(float a_, float b_, float c_, float d_) {
    float a = glm::min(a_, b_);
    float b = glm::max(a_, b_);
    float c = glm::min(c_, d_);
    float d = glm::max(c_, d_);
    //std::cout << a << " " << b << " " << c << " " << d << " " << std::endl;
    return !(b<c || d<a);
}

/*Tells if the line semi-segment starting at startPos with the provided direction 
intersect the bounding box with the transformation matrix applied to it.*/
bool intersectLine(
    const BoundingBox & b,
    const glm::vec3 & direction, const glm::vec3 & startPos,
    const glm::mat4 & transform
) {
    BoundingBox box = b;
    glm::vec4 min = transform*glm::vec4(box.minX, box.minY, box.minZ, 1.0f);
    glm::vec4 max = transform*glm::vec4(box.maxX, box.maxY, box.maxZ, 1.0f);
    box.minX = min.x;
    box.minY = min.y;
    box.minZ = min.z;
    box.maxX = max.x;
    box.maxY = max.y;
    box.maxZ = max.z;
    float Tx1 = (box.minX-startPos.x)/direction.x;
    float Tx2 = (box.maxX-startPos.x)/direction.x;
    float Ty1 = (box.minY-startPos.y)/direction.y;
    float Ty2 = (box.maxY-startPos.y)/direction.y;
    float Tz1 = (box.minZ-startPos.z)/direction.z;
    float Tz2 = (box.maxZ-startPos.z)/direction.z;
    return 
        intervalIntersect(Tx1, Tx2, Ty1, Ty2) &&
        intervalIntersect(Tx1, Tx2, Tz1, Tz2) &&
        intervalIntersect(Tz1, Tz2, Ty1, Ty2);
}

}

}
