#ifndef _SKETCHY_DELAUNAY_
#define _SKETCHY_DELAUNAY_

#include <utils.hpp>
#include <geometry/geometry.hpp>

class ConstrainedDelaunayTriangulation2D {
public:
  ConstrainedDelaunayTriangulation2D(
    const std::vector<glm::vec2> & points,
    bool constraints = true
  ) {
    constrainedDelaunayWithCDT(points, constraints);
  }

  std::vector<glm::uvec3> & getTriangles() {
    return triangles;
  }

  std::vector<Geometry::Edge> & getEdges() {
    return edges;
  }

private:
  std::vector<glm::uvec3> triangles;
  std::vector<Geometry::Edge> edges;

  void constrainedDelaunayWithCDT(
    const std::vector<glm::vec2> & points,
    bool constraints = true
  );
};

#endif
