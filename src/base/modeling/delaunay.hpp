#ifndef _SKETCHY_DELAUNAY_
#define _SKETCHY_DELAUNAY_

#include <utils.hpp>
#include <geometry/geometry.hpp>

#include <CDT.h>

class ConstrainedDelaunayTriangulation2D {
public:
  ConstrainedDelaunayTriangulation2D(
    const std::vector<glm::vec2> & points
  ) {
    std::vector<glm::uvec2> constraint;
    for(unsigned i=0; i<points.size(); i++) {
      constraint.push_back({
        i, (i+1)%points.size()
      });
    }
    cdt.insertVertices(
      points.begin(),
      points.end(),
      [](const glm::vec2 & p){ return p.x; },
      [](const glm::vec2 & p){ return p.y; }
    );
    cdt.insertEdges(
        constraint.begin(),
        constraint.end(),
        [](const glm::uvec2 & p){ return p.x; },
        [](const glm::uvec2 & p){ return p.y; }
    );

    cdt.eraseSuperTriangle();
    // cdt.eraseOuterTrianglesAndHoles();
    for(auto f : cdt.triangles) {
        triangles.emplace_back(f.vertices[0], f.vertices[1], f.vertices[2]);
    }
    for(auto e : cdt.fixedEdges) {
        edges.emplace_back(e.v1(), e.v2());
    }

    for(auto f : triangles) {
        showVec(f);
    }
  }

  std::vector<glm::uvec3> & getTriangles() {
    return triangles;
  }

  std::vector<Geometry::Edge> & getEdges() {
    return edges;
  }

private:
  CDT::Triangulation<float> cdt;
  std::vector<glm::uvec3> triangles;
  std::vector<Geometry::Edge> edges;
};

#endif
