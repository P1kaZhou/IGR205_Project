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
  glm::mat4 mat; // Rotation arround A

  std::vector<float> verticesWeights;

  unsigned parentIndexPlusOne;
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

  std::vector<Renderable*> & getSkeletonMesh(
    const glm::vec3 & color
  );

  void rotateBoneArroundA(unsigned boneIndex, const glm::vec3 & angles);

private:
  std::vector<MeshBone> bones;

  unsigned verticesCount = 0;

  std::vector<Renderable*> boneMeshes;

  /*Each row is a transform. each column is a vertex.*/
  float * vertexTransformCoefs = nullptr;
  FloatTexture vertexTransformCoefsTexture;
  unsigned textureElemSize = 3;
};

#endif
