#include "mesh-skeleton.hpp"

#include <mesh.hpp>

MeshSkeleton::MeshSkeleton(
  const Rigging & rigging
) {
  bones.clear();
  for(unsigned i=0; i<rigging.getBones().size(); i++) {
    const auto & bone = rigging.getBones()[i];
    const auto & boneSkin = ((Rigging&)rigging).getBonesSkins()[i];

    MeshBone meshBone;
    meshBone.A = bone.getA().getPoint();
    meshBone.B = bone.getB().getPoint();
    meshBone.rotationMat = glm::mat4(1);
    meshBone.verticesWeights = std::vector<float>();
    meshBone.verticesWeights.resize(
      boneSkin.getVertexSkinWeights().size(),
      0.0f
    );
    for(const auto & w : boneSkin.getVertexSkinWeights()) {
      meshBone.verticesWeights[w.first] = w.second;
    }
    bones.push_back(meshBone);

    verticesCount = boneSkin.getVertexSkinWeights().size();
  }

  vertexTransformCoefs = new float[bones.size()*verticesCount];
  memset(
      vertexTransformCoefs,
      0,
      sizeof(float)*bones.size()*verticesCount
  );
}

void MeshSkeleton::updateUniform(GLuint program) {
  glUniform1ui(glGetUniformLocation(program, "bonesCount"), bones.size());
  getOpenGLError("bones count uniform");

  glUniform1ui(glGetUniformLocation(program, "verticesCount"), verticesCount);
  getOpenGLError("vertices count uniform");

  for(unsigned b=0; b<bones.size(); b++) {
    auto name = buildIndexedString("bones[", b, "].A");
    glUniform3f(
      glGetUniformLocation(
        program, name
      ),
      bones[b].A.x, bones[b].A.y, bones[b].A.z
    );
    delete name;
    name = buildIndexedString("bones[", b, "].rotationMat");
    glUniformMatrix4fv(
      glGetUniformLocation(
        program, name
      ),
      1, GL_FALSE,
      glm::value_ptr(bones[b].rotationMat)
    );
    delete name;

    // for(unsigned v=0; v<verticesCount; v++) {
    //   name = buildIndexedString("bones[", b, "].weights[");
    //   auto name_ = buildIndexedString(name, v, "]");
    //   glUniform1f(
    //     glGetUniformLocation(
    //       program, name_
    //     ),
    //     bones[b].verticesWeights[v]
    //   );
    //   delete name;
    //   delete name_;
    // }
  }

  vertexTransformCoefsTexture.bind(MAP_TEXTURE_VERTEX_TRANSFORM_COEF);
  glUniform1i(glGetUniformLocation(program, "vertexTransformCoef"), MAP_TEXTURE_VERTEX_TRANSFORM_COEF);
}

void MeshSkeleton::initVerticesTranformsCoef() {
  if(vertexTransformCoefs && verticesCount >  0) {
    vertexTransformCoefsTexture.interpolateToNearest = true;
    vertexTransformCoefsTexture.initFromData(
      vertexTransformCoefs,
      verticesCount,
      bones.size(), 1, false
    );
  }
}

void MeshSkeleton::setVerticesCoef(unsigned transformIndex, unsigned vertexIndex, float value) {
  vertexTransformCoefs[
      transformIndex*verticesCount + vertexIndex
  ] = value;
}

Renderable * MeshSkeleton::getSkeletonMesh(const glm::vec3 & color, unsigned highlightBoneIndex, const glm::vec3 & colorHighlight) {

  std::vector<glm::vec3> triangles;
  std::vector<glm::vec3> colors;
  for(int i=0; i<bones.size(); i++) {
    auto & bone = bones.at(i);

    auto v = glm::cross(bone.A-bone.B, glm::vec3(0,0,1));
    v = glm::normalize(v);
    
    triangles.push_back(bone.A);
    triangles.push_back(bone.B+glm::vec3(0, 0, 0.02));
    triangles.push_back(bone.B+glm::vec3(0, 0, -0.02));

    triangles.push_back(bone.A);
    triangles.push_back(bone.B+v*0.02f);
    triangles.push_back(bone.B+glm::vec3(0, 0, 0.02));

    triangles.push_back(bone.A);
    triangles.push_back(bone.B+v*0.02f);
    triangles.push_back(bone.B+glm::vec3(0, 0, -0.02));
    
    if(i==highlightBoneIndex) {
      colors.push_back(colorHighlight);
      colors.push_back(colorHighlight);
      colors.push_back(colorHighlight);
      colors.push_back(colorHighlight);
      colors.push_back(colorHighlight);
      colors.push_back(colorHighlight);
      colors.push_back(colorHighlight);
      colors.push_back(colorHighlight);
      colors.push_back(colorHighlight);
    }
    else {
      colors.push_back(color);
      colors.push_back(color);
      colors.push_back(color);
      colors.push_back(color);
      colors.push_back(color);
      colors.push_back(color);
      colors.push_back(color);
      colors.push_back(color);
      colors.push_back(color);
    }
  }

  std::vector<glm::uvec3> faces;
  return new Mesh(
    new MeshGeometry(triangles, faces, colors),
    MeshMaterial::meshGetSimplePhongMaterial(color*0.f, color*0.f, 5)
  );
}
