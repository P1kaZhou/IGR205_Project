#ifndef _SKETCHY_MESH_GENERATOR_
#define _SKETCHY_MESH_GENERATOR_

#include <utils.hpp>
#include "cylinder-generator.hpp"

#include <list>

class LimbConnexions;

class MeshGenerator {
public:
  MeshGenerator(
    const std::vector<glm::vec2> & points,
    const std::vector<std::vector<glm::vec2>> & externalAxis,
    const std::vector<std::vector<glm::vec2>> & internalAxis,
    const std::vector<Geometry::Edge> & chords,
    const std::map<glm::vec2, Geometry::Edge> & axisPointToChord,
    unsigned cylinderResolution
  ): 
  points(points),
  externalAxis(externalAxis), internalAxis(internalAxis),
  chords(chords),
  axisPointToChord(axisPointToChord),
  cylinderResolution(cylinderResolution) {}

  ~MeshGenerator();

  void compute();

  std::vector<glm::vec3> & getVertices() { return vertices; }
  std::vector<glm::uvec3> & getFaces() { return faces; }

private:
  const std::vector<glm::vec2> & points;
  std::vector<std::vector<glm::vec2>> externalAxis;
  std::vector<std::vector<glm::vec2>> internalAxis;
  const std::vector<Geometry::Edge> & chords;
  const std::map<glm::vec2, Geometry::Edge> & axisPointToChord;
  unsigned cylinderResolution;

  std::vector<glm::vec3> vertices;
  std::vector<glm::uvec3> faces;

  std::list<CylinderGenerator> cylinders;

  std::vector<glm::uvec3> createConnexionGrid(unsigned p, unsigned q);
  std::vector<std::pair<std::list<CylinderGenerator>::iterator, bool>> getCylindersOnPoint(unsigned p);

  LimbConnexions * limbConnexions = nullptr;

  bool cylinderContainsShapeOrigin(const CylinderGenerator & cyl);

};

#endif
