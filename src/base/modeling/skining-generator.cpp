#include "skining-generator.hpp"

static bool segmentIntersectMesh(
  glm::vec3 segA, glm::vec3 segB,
  std::vector<glm::vec3> & vertices,
  std::vector<glm::uvec3> & faces
) {
  for(unsigned t=0; t<faces.size(); t++) {
    auto v1 = vertices[faces[t].x];
    auto v2 = vertices[faces[t].y];
    auto v3 = vertices[faces[t].z];
    if(
      v1 != segA && v2 != segA && v3 != segA
    ) {
      glm::vec3 p = Geometry::planeToLineIntersection(
        v1, v2, v3,
        segA, segB
      );
      if(glm::dot(segB-segA, p-segA) > 0 && glm::l2Norm(segB-segA)>glm::l2Norm(p-segA)) {
        // If the intersection point is inside the segment
        return true;
      }
    }
  }
  return false;
}

const int SkiningGenerator::findVertexClosestVisibleBoneIndex(
    const glm::vec3 & vertex
) {
  float minDistance = FLT_MAX;
  int targetBoneIndex = -1;
  for(int i=0; i<rigging.getBones().size(); i++) {
    auto & bone = rigging.getBones().at(i);
    // glm::vec3 projOnBone = Geometry::projectPointOnLine(
    //   bone.getA().getPoint(), bone.getB().getPoint(),
    //   vertex
    // );
    // bool intersectMesh = segmentIntersectMesh(
    //   vertex, projOnBone,
    //   vertices, faces
    // );
    // auto distance = glm::distance(projOnBone,vertex);

    auto distance = 
      glm::distance(bone.getA().getPoint(), vertex)+
      glm::distance(bone.getB().getPoint(), vertex);
    if(minDistance > distance) {
      targetBoneIndex = i;
      minDistance = distance;
    }
  }

  if(targetBoneIndex==-1) {
    std::cerr << "No bone found for vertex " << std::endl;
    assert(false);
  }

  return targetBoneIndex;
}

void SkiningGenerator::compute() {
  for(unsigned i=0; i<vertices.size(); i++) {
    int boneIndex = findVertexClosestVisibleBoneIndex(vertices[i]);
    if(boneIndex>=0) rigging.getBonesSkins().at(boneIndex).setSkinWeight(i, 1.0f);
  }
}
