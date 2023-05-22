#ifndef _SKETCHY_MEDIAL_AXIS_
#define _SKETCHY_MEDIAL_AXIS_

#include <utils.hpp>

#include <map>

#include <geometry/geometry.hpp>

template <>
struct std::less<glm::vec2>  {
    bool operator()(const glm::vec2 & a, const glm::vec2 & b) const {
        return a.x<b.x || (a.x==b.x && a.y<b.y);
    }
};

class MedialAxisGenerator {
public:
  MedialAxisGenerator(
      std::vector<glm::vec2> & points,
      std::vector<glm::uvec3> & triangulation
  ): points(points), triangulation(triangulation) {}

  std::vector<std::vector<glm::vec2>> extractExternalAxis();

  inline std::vector< std::pair<glm::vec2, std::vector<glm::vec2>>> getMedialAxis() {
    return rawMedialAxis;
  }

  void pruning(unsigned minSize);

  std::vector<glm::vec2> computeMidPoints();

  void compute();

private:
  std::vector<glm::vec2> & points; // Points of the shape
  std::vector<glm::uvec3> & triangulation;

  std::vector< std::pair<glm::vec2, std::vector<glm::vec2>>> rawMedialAxis;
  
  unsigned triangleInternalEdgeCount(
    const glm::uvec3 & triangle,
    std::vector<glm::uvec2> & internalEdges,
    std::vector<glm::uvec2> & externalEdges
  );

  void insertSegment(const glm::vec2 & a, const glm::vec2 & b);
  void removeVertex(const glm::vec2 & a);

  inline bool isSame(const glm::vec2 & a, const glm::vec2 & b) {
    return glm::distance(a,b) < minDistance;
    // return a==b;
  }

  double minDistance = 0.00001;

  inline void showMedialAxis() {
    // for(unsigned i=0; i<rawMedialAxis.size(); i++) {
    //   auto p = rawMedialAxis[i];
    //   std::cout << i;
    //   showVec(p.first, " point");
    //   std::cout << "count " << p.second.size() << std::endl;
    //   for(auto q : p.second) {
    //     std::cout << "\t" << get(rawMedialAxis, q);
    //     showVec(q, " n");
    //   }
    // }

    // for(unsigned i=0; i<rawMedialAxis.size(); i++) {
    //   std::cout << i << std::endl;
    // }
    // for(unsigned i=0; i<rawMedialAxis.size(); i++) {
    //   auto p = rawMedialAxis[i];
    //   for(unsigned j=0; j<p.second.size(); j++) {
    //     auto q = p.second[j];
    //     std::cout << i << " " << get(rawMedialAxis, q) << std::endl;
    //   }
    // }

    int mat[rawMedialAxis.size()][rawMedialAxis.size()];
    memset(mat, 0, sizeof(int)*rawMedialAxis.size()*rawMedialAxis.size());
    for(unsigned i=0; i<rawMedialAxis.size(); i++) {
      auto p = rawMedialAxis[i];
      for(unsigned k=0; k<p.second.size(); k++) {
        auto q = p.second[k];
        unsigned j = get(rawMedialAxis, q);
        mat[i][j] = 1;
      }
    }
    for(unsigned i=0; i<rawMedialAxis.size(); i++) {
      for(unsigned j=0; j<rawMedialAxis.size(); j++) {
        std::cout << mat[i][j];
        if(j<rawMedialAxis.size()-1) std::cout << ", ";
      }
      std::cout << std::endl;
    }
  }

  inline float getMinDistance() {
    float min = 10000;
    for(unsigned i=0; i<rawMedialAxis.size(); i++) {
      auto & m = rawMedialAxis[i];
      for(unsigned j=0; j<rawMedialAxis.size(); j++) {
        auto & n = rawMedialAxis[j];
        float d = glm::distance(m.first, n.first);
        if(d < min && i!=j) {
          min = d;
        }
      }
    }
    minDistance = min;
    return min;
  }

  inline bool contains(
    const std::vector< std::pair<glm::vec2, std::vector<glm::vec2>>> & rawMedialAxis,
    const glm::vec2 & v
  ) {
    for(auto & m : rawMedialAxis) {
      if(glm::distance(m.first,v) < minDistance) return true;
      // if(m.first==v) return true;
    }
    return false;
  }

  inline unsigned get(
    const std::vector< std::pair<glm::vec2, std::vector<glm::vec2>>> & rawMedialAxis,
    const glm::vec2 & v
  ) {
    for(unsigned i=0; i<rawMedialAxis.size(); i++) {
      auto & m = rawMedialAxis[i];
      if(glm::distance(m.first,v) < minDistance) return i;
      // if(m.first==v) return i;
    }
    return 0;
  }

  inline bool contains(const std::vector<glm::vec2> & points, const glm::vec2 & p) {
    for(auto point : points) {
      if(glm::distance(point,p) < minDistance) return true;
      // if(point==p) return true;
    }
    return false;
  }

};

#endif
