#ifndef _SKETCHY_MESH_SKELETON_
#define _SKETCHY_MESH_SKELETON_

#include <utils.hpp>
#include <modeling/skeleton.hpp>
#include <modeling/rigging.hpp>
#include <texture.hpp>
#include <base.hpp>

#define MAP_TEXTURE_VERTEX_TRANSFORM_COEF 4

struct MeshBone {
  glm::vec3 A;
  glm::vec3 B;
  glm::mat4 rotationMat; // Rotation arround A

  std::vector<float> verticesWeights;
};

class MeshSkeleton {
public:
  MeshSkeleton(
    const Rigging & rigging
  );

  void setVerticesCoef(unsigned transformIndex, unsigned vertexIndex, float value);
  void initVerticesTranformsCoef();
  void updateUniform(GLuint program);

  inline std::vector<MeshBone> & getBones() { return bones; }

  Renderable * getSkeletonMesh(
    const glm::vec3 & color, unsigned highlightBoneIndex, const glm::vec3 & colorHighlight
  );

private:
  std::vector<MeshBone> bones;

  unsigned verticesCount = 0;

  /*Each row is a transform. each column is a vertex.*/
  float * vertexTransformCoefs = nullptr;
  FloatTexture vertexTransformCoefsTexture;

  GLuint uboCoefs;
};

#endif
