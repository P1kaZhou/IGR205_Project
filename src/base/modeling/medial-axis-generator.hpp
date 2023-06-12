#ifndef _SKETCHY_MEDIAL_AXIS_GENERATOR_
#define _SKETCHY_MEDIAL_AXIS_GENERATOR_

#include "medial-axis.hpp"

class MedialAxisGenerator {
public:
  MedialAxisGenerator(
      const std::vector<glm::vec2> & points,
      const std::vector<glm::uvec3> & triangulation
  ): points(points), triangulation(triangulation) {}

  std::vector<std::vector<glm::vec2>> extractExternalAxis();
  std::vector<std::vector<glm::vec2>> extractInternalAxis();

  inline MedialAxis & getMedialAxis() {
    return medialAxis;
  }

  void pruning(unsigned minSize);

  std::vector<glm::vec2> computeMidPoints();

  void smooth(int size);

private:
  const std::vector<glm::vec2> & points; // Points of the shape
  const std::vector<glm::uvec3> & triangulation;

  MedialAxis medialAxis;
  
  unsigned triangleInternalEdgeCount(
    const glm::uvec3 & triangle,
    std::vector<glm::uvec2> & internalEdges,
    std::vector<glm::uvec2> & externalEdges
  );

  void smoothAxis(const std::vector<glm::vec2> & axis, int size);

};

#endif
