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


class MedialAxisPoint {
public:
  MedialAxisPoint(const glm::vec2 & point)
  : point(point) {}

  inline int getAdjIndex(MedialAxisPoint * p) {
    for(int i=0; i<adjs.size(); i++) {
      auto adj = adjs[i];
      if(adj == p) return i;
    }
    return -1;
  }

  inline void removeAdjIfExist(MedialAxisPoint * p) {
    int index = getAdjIndex(p);
    if(index >= 0) adjs.erase(adjs.begin()+index);
  }

  inline void addAdj(MedialAxisPoint * p) {
    auto index = getAdjIndex(p);
    if(index == -1) {
      adjs.push_back(p);
    }
  }

  inline const glm::vec2 & getPoint() { return point; }
  inline const std::vector<MedialAxisPoint *> & getAdjs() { return adjs; }

private:
  glm::vec2 point;
  std::vector<MedialAxisPoint *> adjs;
};

class MedialAxis {
public:
  MedialAxis() {}

  inline void clear() {
    for(MedialAxisPoint * p : points) {
      delete p;
    }
    points.clear();
  }

  inline MedialAxisPoint * insertPoint(const glm::vec2 & point) {
    MedialAxisPoint * p = getAxisPoint(point);
    if(p == nullptr) {
      p = new MedialAxisPoint(point);
      points.push_back(p);
    }
    return p;
  }

  inline void insertEdge(const glm::vec2 & point1, const glm::vec2 & point2) {
    auto p1 = insertPoint(point1);
    auto p2 = insertPoint(point2);
    p1->addAdj(p2);
    p2->addAdj(p1);
  }

  inline void removePoint(const glm::vec2 & point) {
    int index = getAxisPointIndex(point);
    if(index >= 0) {
      for(MedialAxisPoint * p : points) {
        p->removeAdjIfExist(points[index]);
      }
      delete points[index];
      points.erase(points.begin()+index);
    }
  }

  inline MedialAxisPoint * getAxisPoint(const glm::vec2 & point) {
    for(MedialAxisPoint * p : points) {
      if(p->getPoint() == point) {
        return p;
      }
    }
    return nullptr;
  }

  inline int getAxisPointIndex(const glm::vec2 & point) {
    for(int i=0; i<points.size(); i++) {
      MedialAxisPoint * p = points[i];
      if(p->getPoint() == point) {
        return i;
      }
    }
    return -1;
  }

  inline const std::vector<MedialAxisPoint*> & getPoints() { return points; }

  inline void showAsAdjMatrix() {
    unsigned N = points.size();
    int mat[N][N];
    memset(mat, 0, sizeof(int)*N*N);
    for(unsigned i=0; i<N; i++) {
      auto p = points[i];
      for(unsigned k=0; k<p->getAdjs().size(); k++) {
        auto q = p->getAdjs()[k];
        unsigned j = getAxisPointIndex(q->getPoint());
        mat[i][j] = 1;
      }
    }
    for(unsigned i=0; i<N; i++) {
      for(unsigned j=0; j<N; j++) {
        std::cout << mat[i][j];
        if(j<N-1) std::cout << ", ";
      }
      std::cout << std::endl;
    }
  }

  inline void showAsAdjList() {
    for(unsigned i=0; i<points.size(); i++) {
      auto p = points[i];
      std::cout << i;
      showVec(p->getPoint(), " point");
      std::cout << "count " << p->getAdjs().size() << std::endl;
      for(auto q : p->getAdjs()) {
        std::cout << "\t" << getAxisPointIndex(q->getPoint());
        showVec(q->getPoint(), " n");
      }
    }
  }

private:
  std::vector<MedialAxisPoint*> points;
};

class MedialAxisGenerator {
public:
  MedialAxisGenerator(
      std::vector<glm::vec2> & points,
      std::vector<glm::uvec3> & triangulation
  ): points(points), triangulation(triangulation) {}

  std::vector<std::vector<glm::vec2>> extractExternalAxis();

  inline MedialAxis & getMedialAxis() {
    return medialAxis;
  }

  void pruning(unsigned minSize);

  std::vector<glm::vec2> computeMidPoints();

  void compute();

private:
  std::vector<glm::vec2> & points; // Points of the shape
  std::vector<glm::uvec3> & triangulation;

  MedialAxis medialAxis;
  
  unsigned triangleInternalEdgeCount(
    const glm::uvec3 & triangle,
    std::vector<glm::uvec2> & internalEdges,
    std::vector<glm::uvec2> & externalEdges
  );

};

#endif
