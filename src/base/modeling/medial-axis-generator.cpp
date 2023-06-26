#include "medial-axis-generator.hpp"

// JCV
#define JC_VORONOI_IMPLEMENTATION
#include <jc_voronoi/jc_voronoi.h>
#define JC_VORONOI_CLIP_IMPLEMENTATION
#include <jc_voronoi/jc_voronoi_clip.h>


/*
Return each external axis from the medial axis.
Each returned axis start from limb extremities and 
end with a junction point (3 or more neighbors. or a limb extremity).
*/
std::vector<std::vector<glm::vec2>> MedialAxisGenerator::extractExternalAxis() {
  std::vector<std::vector<glm::vec2>> axisList;
  std::vector<glm::vec2> externalAxisEndPoints;
  for(auto medialPoint : medialAxis.getPoints()) {
    if(medialPoint->getAdjs().size()==1) {
      std::vector<glm::vec2> axis;
      axis.push_back(medialPoint->getPoint());
      axis.push_back(medialPoint->getAdjs()[0]->getPoint());
      auto prevStart = medialPoint->getPoint();
      auto start = medialPoint->getAdjs()[0]->getPoint();
      bool stop = false;
      while(!stop && medialAxis.getAxisPoint(start)->getAdjs().size()==2) {
        // While we are on the same axis
        auto adjs = medialAxis.getAxisPoint(start)->getAdjs();
        if(adjs[0]->getPoint()==prevStart) {
          prevStart = start;
          start = adjs[1]->getPoint();
        }
        else {
          prevStart = start;
          start = adjs[0]->getPoint();
        }

        // To prevent eventual loops
        if(std::find(axis.begin(), axis.end(), start) != axis.end()) {
          stop = true;
        }
        else {
          axis.push_back(start);
        }
      }

      if(std::find(
        externalAxisEndPoints.begin(), externalAxisEndPoints.end(), axis[0]
      ) == externalAxisEndPoints.end()) {
        // We make sure the the axis doesn't start iwth the end of another external axis
        // Other wise they are the same but in reverse order
        axisList.push_back(axis);
      }
      externalAxisEndPoints.push_back(axis[axis.size()-1]);
    }
  }
  return axisList;
}

/*
Return each internal axis from the medial axis. 
*/
std::vector<std::vector<glm::vec2>> MedialAxisGenerator::extractInternalAxis() {
  std::vector<std::vector<glm::vec2>> axisList;
  // list of points that are ajacent to points with 3 or more adjacent points
  std::vector<glm::vec2> internalAxisEndPoints;
  for(auto medialPoint : medialAxis.getPoints()) {
    if(medialPoint->getAdjs().size()>2) {
      // We start with an internal node
      for(auto adjPoint : medialPoint->getAdjs()) {
        if(std::find(
          internalAxisEndPoints.begin(),
          internalAxisEndPoints.end(),
          adjPoint->getPoint()
        ) == internalAxisEndPoints.end()) {
          // If the adjacent point is not the end of a previous internal axis
          std::vector<glm::vec2> axis;
          axis.push_back(medialPoint->getPoint());
          axis.push_back(adjPoint->getPoint());
          auto prevStart = medialPoint->getPoint();
          auto start = adjPoint->getPoint();
          bool stop = false;
          while(!stop && medialAxis.getAxisPoint(start)->getAdjs().size()==2) {
            // While we are on the same axis
            auto adjs = medialAxis.getAxisPoint(start)->getAdjs();
            if(adjs[0]->getPoint()==prevStart) {
              prevStart = start;
              start = adjs[1]->getPoint();
            }
            else {
              prevStart = start;
              start = adjs[0]->getPoint();
            }

            // To prevent eventual loops
            if(std::find(axis.begin(), axis.end(), start) != axis.end()) {
              stop = true;
            }
            else {
              axis.push_back(start);
            }
          }
          if(medialAxis.getAxisPoint(start)->getAdjs().size()>2) {
            // If it's an internal axis
            internalAxisEndPoints.push_back(axis[axis.size()-2]);
            axisList.push_back(axis);
          }
        }
      }
    }
  }
  return axisList;
}

void MedialAxisGenerator::pruning(unsigned minSize) {
  std::vector<std::vector<glm::vec2>> axiss = extractExternalAxis();

  for(auto axis : axiss) {
    if(axis.size() < minSize) {
      unsigned i = 0;
      for(auto a : axis) {
        if(i<axis.size()-1) {
          medialAxis.removePoint(a);
        }
        i++;
      }
    }
  }
}

std::vector<glm::vec2> MedialAxisGenerator::computeMidPoints() {
  medialAxis.clear();
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

    if(externalEdges.size() == 1) {
      // Termination triangle
      terminationTriangles.push_back(triangle);
    }
    else if(externalEdges.size() == 2) {
      // Sleeve triangle
      medialAxis.insertEdge(
        {
          (points[externalEdges[0].x].x+points[externalEdges[0].y].x)/2,
          (points[externalEdges[0].x].y+points[externalEdges[0].y].y)/2
        },
        {
          (points[externalEdges[1].x].x+points[externalEdges[1].y].x)/2,
          (points[externalEdges[1].x].y+points[externalEdges[1].y].y)/2
        }
      );
      sleeveTriangles.push_back(triangle);
    }
    else if(externalEdges.size() == 3) {
      // Junction triangle
      glm::vec2 center = (points[triangle.x]+points[triangle.y]+points[triangle.z])*(1.f/3.0f);
      medialAxis.insertEdge(
        {
          (points[externalEdges[0].x].x+points[externalEdges[0].y].x)/2,
          (points[externalEdges[0].x].y+points[externalEdges[0].y].y)/2
        },
        center
      );
      medialAxis.insertEdge(
        {
          (points[externalEdges[1].x].x+points[externalEdges[1].y].x)/2,
          (points[externalEdges[1].x].y+points[externalEdges[1].y].y)/2
        },
        center
      );
      medialAxis.insertEdge(
        {
          (points[externalEdges[2].x].x+points[externalEdges[2].y].x)/2,
          (points[externalEdges[2].x].y+points[externalEdges[2].y].y)/2
        },
        center
      );
      junctionTriangles.push_back(triangle);
    }
  }
  return midPoints;
}

/*
For the provided triangle, returns the number of edges (1, 2 or 3)
that are not in the shape (those are called internal edges).
At the end:
- internalEdges contains the internal edges (those that are in the shape)
- externalEdges contains the external edges (those that are not in the shape)
*/
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

void MedialAxisGenerator::smooth(int size) {
  auto externalAxis = extractExternalAxis();
  auto internalAxis = extractInternalAxis();

  for(auto & axis : externalAxis) {
    smoothAxis(axis, size);
  }
  for(auto & axis : internalAxis) {
    smoothAxis(axis, size);
  }
}

void MedialAxisGenerator::smoothAxis(const std::vector<glm::vec2> & axis, int size) {
  for(int i=1; i<axis.size()-1; i++) {
    glm::vec2 newP = axis[i];
    int count = 1;
    for(int j=1; j<=size; j++) {
      if(i-j>=0) {
        auto & m = axis[i-j];
        newP += m;
        count += 1;
      }
      if(i+j<axis.size()) {
        auto & n = axis[i+j];
        newP += n;
        count += 1;
      }
    }
    newP = newP * ( 1.f/count );
    auto point = medialAxis.getAxisPoint(axis[i]);
    if(point) point->setPoint(newP);
  }
}
