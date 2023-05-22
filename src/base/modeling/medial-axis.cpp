#include "medial-axis.hpp"

// JCV
#define JC_VORONOI_IMPLEMENTATION
#include <jc_voronoi/jc_voronoi.h>
#define JC_VORONOI_CLIP_IMPLEMENTATION
#include <jc_voronoi/jc_voronoi_clip.h>

// CGAL
// includes for defining the Voronoi diagram adaptor
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
// typedefs for defining the adaptor
typedef CGAL::Exact_predicates_inexact_constructions_kernel                  K;
typedef CGAL::Delaunay_triangulation_2<K>                                    DT;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                 AT;
typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
typedef CGAL::Voronoi_diagram_2<DT,AT,AP>                                    VD;
// typedef for the result type of the point location

void MedialAxisGenerator::compute() {
  rawMedialAxis.clear();
  jcv_diagram diagram;
  memset(&diagram, 0, sizeof(jcv_diagram));

  unsigned N = points.size();

  if(N<3) {
    std::cout << "Not enough points for medial axis" << std::endl;
    return;
  }

  // Centroids
  jcv_point centroids[N] = {0};
  auto maxX = points[0].x;
  auto minX = points[0].x;
  auto maxY = points[0].y;
  auto minY = points[0].y;
  for(unsigned v=0;v<N;v++) {
    auto p = points[v];
    centroids[v].x = p.x;
    centroids[v].y = p.y;
    if(p.x>maxX) maxX = p.x;
    if(p.x<minX) minX = p.x;
    if(p.y>maxY) maxY = p.y;
    if(p.y<minY) minY = p.y;
  }

  jcv_rect rect = {};
  rect.max.x = maxX;
  rect.max.y = maxY;
  rect.min.x = minX;
  rect.min.y = minY;

  // Compute voronoi diagrams
  std::cout << "Start voronoi diagram" << std::endl;
  jcv_diagram_generate(N, centroids, &rect, 0, &diagram);

  {
    const jcv_edge * edges  = jcv_diagram_get_edges(&diagram);
    while(edges) {
      if(edges->sites[0] && edges->sites[1]) {
        if(
          edges->sites[0]->index != (edges->sites[1]->index-1)%N &&
          edges->sites[0]->index != (edges->sites[1]->index+1)%N
        ) {
          insertSegment(
            {edges->pos[0].x, edges->pos[0].y},
            {edges->pos[1].x, edges->pos[1].y}
          );
        }
      }

      edges = edges->next;
    }
  }

  jcv_diagram_free(&diagram);
  std::cout << "end voronoi diagram" << std::endl;

  // showMedialAxis();

  std::cout << "Min distance " << getMinDistance() << std::endl;
}

void MedialAxisGenerator::insertSegment(const glm::vec2 & a, const glm::vec2 & b) {
  // if(
  //   // isSame(a, b)
  //   a == b
  // ) {
  //   std::cout << "SAME" << std::endl;
  //   return;
  // }

  if(!contains(rawMedialAxis, a)) {
    // If the first point is not in
    rawMedialAxis.emplace_back(a, std::vector<glm::vec2>());
  }
  if(!contains(rawMedialAxis, b)) {
    // If the first point is not in
    rawMedialAxis.emplace_back(b, std::vector<glm::vec2>());
  }

  auto & aa = rawMedialAxis[get(rawMedialAxis, a)];
  auto & bb = rawMedialAxis[get(rawMedialAxis, b)];
  if(!contains(aa.second, b)) {
    aa.second.push_back(b);
  }
  if(!contains(bb.second, a)) {
    bb.second.push_back(a);
  }
}

std::vector<std::vector<glm::vec2>> MedialAxisGenerator::extractExternalAxis() {
  std::cout << "Start external axis extraction" << std::endl;
  std::vector<std::vector<glm::vec2>> axisList;
  std::cout << "point count : " << rawMedialAxis.size() << std::endl;
  for(auto ax : rawMedialAxis) {
    if(ax.second.size()==1) {
      std::vector<glm::vec2> axis;
      axis.push_back(ax.first);
      axis.push_back(ax.second[0]);
      auto prevStart = ax.first;
      auto start = ax.second[0];
      bool stop = false;
      while(!stop && rawMedialAxis[get(rawMedialAxis, start)].second.size() == 2) {
        // While we are on the same axis
        auto adjs = rawMedialAxis[get(rawMedialAxis, start)].second;
        if(adjs[0]==prevStart) {
          prevStart = start;
          start = adjs[1];
        }
        else {
          prevStart = start;
          start = adjs[0];
        }
        if(contains(axis, start)) {
          stop = true;
        }
        else {
          axis.push_back(start);
        }
      }
      // if(rawMedialAxis[get(rawMedialAxis, start)].second.size() > 1) {
        axisList.push_back(axis);
      // }
    }
  }
  std::cout << "end external axis extraction" << std::endl;
  return axisList;
}

void MedialAxisGenerator::pruning(unsigned minSize) {
  std::cout << "Start prunning" << std::endl;
  std::cout << "point count : " << rawMedialAxis.size() << std::endl;
  std::vector<std::vector<glm::vec2>> axiss;
  for(auto ax : rawMedialAxis) {
    if(ax.second.size()==1) {
      std::vector<glm::vec2> axis;
      axis.push_back(ax.first);
      axis.push_back(ax.second[0]);
      auto prevStart = ax.first;
      auto start = ax.second[0];
      bool stop = false;
      while(!stop && rawMedialAxis[get(rawMedialAxis, start)].second.size() == 2) {
        // While we are on the same axis
        auto adjs = rawMedialAxis[get(rawMedialAxis, start)].second;
        if(adjs[0]==prevStart) {
          prevStart = start;
          start = adjs[1];
        }
        else {
          prevStart = start;
          start = adjs[0];
        }
        if(contains(axis, start)) {
          stop = true;
        }
        else {
          axis.push_back(start);
        }
      }

      axiss.push_back(axis);
    }
  }

  for(auto axis : axiss) {
    if(axis.size() < minSize) {
      unsigned i = 0;
      for(auto a : axis) {
        if(i<axis.size()-1) removeVertex(a);
        i++;
      }
    }
  }
  std::cout << "prun point count after : " << rawMedialAxis.size() << std::endl;
  std::cout << "end prunning" << std::endl;
}

void MedialAxisGenerator::removeVertex(const glm::vec2 & a) {
  for(unsigned i=0; i<rawMedialAxis.size(); i++) {
    auto & ax = rawMedialAxis[i];
    if(isSame(ax.first, a)) {
      rawMedialAxis.erase(rawMedialAxis.begin()+i);
      i--;
    }
    for(unsigned j=0; j<ax.second.size(); j++) {
      if(isSame(ax.second[j], a)) {
        ax.second.erase(ax.second.begin()+j);
      }
    }
  }
}

std::vector<glm::vec2> MedialAxisGenerator::computeMidPoints() {
  rawMedialAxis.clear();
  std::vector<glm::vec2> midPoints;
  std::vector<glm::uvec2> internalEdges;
  std::vector<glm::uvec2> externalEdges;
  for(auto & triangle : triangulation) {
    internalEdges.clear();
    externalEdges.clear();
    auto internalEdgeCount = triangleInternalEdgeCount(
      triangle, internalEdges, externalEdges);

    for(auto & edge : externalEdges) {
      midPoints.emplace_back(
        (points[edge.x].x+points[edge.y].x)/2,
        (points[edge.x].y+points[edge.y].y)/2
      );
    }

    std::cout << "external edge count : " << externalEdges.size() << std::endl;

    if(externalEdges.size() == 2) {
      // Sleeve triangle
      insertSegment(
        {
          (points[externalEdges[0].x].x+points[externalEdges[0].y].x)/2,
          (points[externalEdges[0].x].y+points[externalEdges[0].y].y)/2
        },
        {
          (points[externalEdges[1].x].x+points[externalEdges[1].y].x)/2,
          (points[externalEdges[1].x].y+points[externalEdges[1].y].y)/2
        }
      );
    }
    else if(externalEdges.size() == 3) {
      // Junction triangle
      // glm::vec2 center = Geometry::triangleCircumcenter(
      //   points[externalEdges[0].y],
      //   points[externalEdges[1].y],
      //   points[externalEdges[2].y]
      // );
      glm::vec2 center = (points[triangle.x]+points[triangle.y]+points[triangle.z])*(1.f/3.0f);
      insertSegment(
        {
          (points[externalEdges[0].x].x+points[externalEdges[0].y].x)/2,
          (points[externalEdges[0].x].y+points[externalEdges[0].y].y)/2
        },
        center
      );
      insertSegment(
        {
          (points[externalEdges[1].x].x+points[externalEdges[1].y].x)/2,
          (points[externalEdges[1].x].y+points[externalEdges[1].y].y)/2
        },
        center
      );
      insertSegment(
        {
          (points[externalEdges[2].x].x+points[externalEdges[2].y].x)/2,
          (points[externalEdges[2].x].y+points[externalEdges[2].y].y)/2
        },
        center
      );
    }
  }
  std::cout << "Mid point count : " << midPoints.size() << std::endl;
  std::cout << "Medial axis point count : " << rawMedialAxis.size() << std::endl;
  return midPoints;
}

unsigned MedialAxisGenerator::triangleInternalEdgeCount(
  const glm::uvec3 & triangle,
  std::vector<glm::uvec2> & internalEdges,
  std::vector<glm::uvec2> & externalEdges
) {
  unsigned N = points.size();
  unsigned internalEdgeCount = 0;
  bool foundXY = 0;
  bool foundXZ = 0;
  bool foundZY = 0;
  for(unsigned i=0; i<N; i++) {
    if((triangle.x == i && triangle.y == (i+1)%N) || (triangle.y == i && triangle.x == (i+1)%N)) {
      internalEdgeCount += 1;
      internalEdges.push_back({triangle.x, triangle.y});
      foundXY = true;
    }

    if((triangle.x == i && triangle.z == (i+1)%N) || (triangle.z == i && triangle.x == (i+1)%N)) {
      internalEdgeCount += 1;
      internalEdges.push_back({triangle.x, triangle.z});
      foundXZ = true;
    }
    
    if((triangle.z == i && triangle.y == (i+1)%N) || (triangle.y == i && triangle.z == (i+1)%N)) {
      internalEdgeCount += 1;
      internalEdges.push_back({triangle.z, triangle.y});
      foundZY = true;
    }
  }

  if(!foundXY) {
    externalEdges.push_back({triangle.x, triangle.y});
  }
  if(!foundXZ) {
    externalEdges.push_back({triangle.x, triangle.z});
  }
  if(!foundZY) {
    externalEdges.push_back({triangle.z, triangle.y});
  }
  return internalEdgeCount;
}






// std::vector<std::pair<glm::vec2, glm::vec2>> MedialAxisGenerator::computeCGAL() {
//   VD vd;

//   for(auto pt : points) {
//     vd.insert(VD::Site_2(pt.x, pt.y));
//   }

//   if(vd.is_valid()) std::cout << "VALID" << std::endl;
//   else std::cout << "NOT VALID" << std::endl;

//   std::vector<std::pair<glm::vec2, glm::vec2>> edges;

//   VD::Edge_iterator eit = vd.edges_begin();
//   for(; eit != vd.edges_end(); eit++) {
//     std::cout << "edge : " << std::endl;
    
//     if(eit->has_source() && eit->has_target()) {
//       edges.push_back({
//         {eit->source()->point().x(), eit->source()->point().y()},
//         {eit->target()->point().x(), eit->target()->point().y()}
//       });

//       // if(
//       //   eit->right()->point() != (edges->sites[1]->index-1)%N &&
//       //   edges->sites[0]->index != (edges->sites[1]->index+1)%N
//       // ) {
//       //   insertSegment(
//       //     {edges->pos[0].x, edges->pos[0].y},
//       //     {edges->pos[1].x, edges->pos[1].y}
//       //   );
//       // }
//     }
//   }

//   VD::Halfedge_iterator heit = vd.halfedges_begin();
//   for(; heit != vd.halfedges_end(); heit++) {
//     vd.sites_begin()
//   }

//   return edges;
// }
