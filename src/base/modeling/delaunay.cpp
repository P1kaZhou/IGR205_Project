#include "delaunay.hpp"

#include <CDT.h>

void ConstrainedDelaunayTriangulation2D::constrainedDelaunayWithCDT(
  const std::vector<glm::vec2> & points
) {
  std::vector<glm::uvec2> constraint;
  for(unsigned i=0; i<points.size(); i++) {
    constraint.push_back({
      i, (i+1)%points.size()
    });
  }

  CDT::Triangulation<float> cdt;
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

  // cdt.eraseSuperTriangle();
  cdt.eraseOuterTriangles();
  // cdt.eraseOuterTrianglesAndHoles();
  for(auto f : cdt.triangles) {
    triangles.emplace_back(f.vertices[0], f.vertices[1], f.vertices[2]);

    {
      bool isNotFixed = true;
      auto a = f.vertices[0];
      auto b = f.vertices[1];
      for(auto e : cdt.fixedEdges) {
        if( (e.v1() == a || e.v2() == a) && (e.v1() == b || e.v2() == b)) {
          isNotFixed = false;
        }
      }
      if(isNotFixed) {
        edges.emplace_back(a, b);
      }
    }

    {
      bool isNotFixed = true;
      auto a = f.vertices[1];
      auto b = f.vertices[2];
      for(auto e : cdt.fixedEdges) {
        if( (e.v1() == a || e.v2() == a) && (e.v1() == b || e.v2() == b)) {
          isNotFixed = false;
        }
      }
      if(isNotFixed) {
        edges.emplace_back(a, b);
      }
    }

    {
      bool isNotFixed = true;
      auto a = f.vertices[2];
      auto b = f.vertices[0];
      for(auto e : cdt.fixedEdges) {
        if( (e.v1() == a || e.v2() == a) && (e.v1() == b || e.v2() == b)) {
          isNotFixed = false;
        }
      }
      if(isNotFixed) {
        edges.emplace_back(a, b);
      }
    }
  }
}