#ifndef _SKETCHY_CHORDS_GENERATOR_
#define _SKETCHY_CHORDS_GENERATOR_

#include <utils.hpp>
#include <geometry/geometry.hpp>
#include "shape.hpp"

class ChordsGenerator {
public:
  ChordsGenerator(
    const std::vector<glm::vec2> & points,
    const std::vector<std::vector<glm::vec2>> & externalAxis,
    const std::vector<std::vector<glm::vec2>> & internalAxis
  ): points(points), externalAxis(externalAxis), internalAxis(internalAxis) {}

  inline void compute() {
    chords.clear();
    for(auto axis : internalAxis) {
      // Avoid the the first and last points which are in a junction triangle
      processAxis(axis.begin()+1, axis.begin()+axis.size()-1);
    }
    for(auto axis : externalAxis) {
      // Avoid the the last point which is in a junction triangle
      processAxis(axis.begin(), axis.begin()+axis.size()-1);
    }
  }

  inline const std::vector<Geometry::Edge> & getChords() { return chords; }

  std::map<glm::vec2, Geometry::Edge> axisPointToChord;

private:
  const std::vector<glm::vec2> & points;
  const std::vector<std::vector<glm::vec2>> & externalAxis;
  const std::vector<std::vector<glm::vec2>> & internalAxis;

  std::vector<Geometry::Edge> chords;

  inline void processAxis(
    std::vector<glm::vec2>::const_iterator begin,
    std::vector<glm::vec2>::const_iterator end
  ) {
    glm::vec2 dir;
    glm::vec2 n;
    for(auto it = begin; it!=end; it++) {
      if(it==begin) {
        dir = *it - *(it+1);
        n = Geometry::getOrthonalVector(dir);
      }
      else if(it+1==end) {
        dir = *it - *(it-1);
        n = Geometry::getOrthonalVector(dir);
      }
      else {
        dir = *(it+1) - *(it-1);
        n = Geometry::getOrthonalVector(dir);
      }
      auto a = findClosestPointToSemiLine(*it, n);
      auto b = findClosestPointToSemiLine(*it, -n);
      Geometry::Edge chord(a, b);
      chords.push_back(chord);

      axisPointToChord.insert({*it, chord});
    }
  }

  inline unsigned findClosestPointToSemiLine(
    const glm::vec2 & p, const glm::vec2 & v
  ) {
    unsigned N = points.size();
    float minDistance = FLT_MAX;
    float maxDot = FLT_MIN;
    float distance = 0;
    float dot = 0;
    unsigned minDistIndex = 0;
    float t1, t2;
    bool foundIndex = false;
    for(unsigned i=0; i<N; i++) {
      auto & m = points[i];

      distance = glm::distance(p, m);
      dot = glm::dot(glm::normalize(v), glm::normalize(m-p));
      if(distance*(1/dot) < minDistance*(1/maxDot) && dot>0) {
        minDistance = distance;
        maxDot = dot;
        minDistIndex = i;
        foundIndex = true;
      }
    }

    if(!foundIndex) {
      std::cout << "Found no intersecting edge in shape for orthogonal chord" << std::endl;
      assert(false);
    }

    return minDistIndex;
  }

  inline bool doesCrossChord(const glm::vec2 & p, const glm::vec2 & q) {
    for(auto & c : chords) {
      float t1, t2;
      Geometry::lineToLineIntersectionCoef(
        p, p-q, points[c.a], points[c.a]-points[c.b], t1, t2
      );
      if(t2>0 && t2<1) {
        return true;
      }
    }
    return false;
  }
};

#endif
