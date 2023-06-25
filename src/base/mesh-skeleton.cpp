#include "mesh-skeleton.hpp"

#include <mesh.hpp>

MeshSkeleton::MeshSkeleton(
  const Rigging & rigging
) {
  bones.reserve(rigging.getBones().size());
  for(unsigned i=0; i<rigging.getBones().size(); i++) {
    const auto & bone = rigging.getBones()[i];
    const auto & boneSkin = ((Rigging&)rigging).getBonesSkins()[i];

    MeshBone meshBone;
    meshBone.A = bone.getA().getPoint();
    meshBone.B = bone.getB().getPoint();
    meshBone.parentIndexPlusOne = 0;
    meshBone.mat = glm::mat4(1);
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

  for(unsigned i=0; i<rigging.getBones().size(); i++) {
    MeshBone * meshBone = &(bones.at(i));
    const auto & bone = rigging.getBones()[i];
    
    for(unsigned j=0; j<rigging.getBones().size(); j++) {
      const auto & b = rigging.getBones()[j];
      if(i != j) {
        if(bone.getA().getId() == b.getB().getId()) {
          meshBone->parentIndexPlusOne = j+1;
        }
      }
    }
  }

  vertexTransformCoefs = new float[bones.size()*verticesCount*textureElemSize*textureElemSize];
  memset(
      vertexTransformCoefs,
      0,
      sizeof(float)*bones.size()*verticesCount*textureElemSize*textureElemSize
  );
}

void MeshSkeleton::updateUniform(GLuint program) {
  glUniform1ui(glGetUniformLocation(program, "bonesCount"), bones.size());
  getOpenGLError("bones count uniform");

  glUniform1ui(glGetUniformLocation(program, "verticesCount"), verticesCount);
  getOpenGLError("vertices count uniform");

  glUniform1ui(glGetUniformLocation(program, "textureElemSize"), textureElemSize);
  getOpenGLError("texture elem size");

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
      glm::value_ptr(bones[b].mat)
    );
    delete name;

    name = buildIndexedString("bones[", b, "].parentIndexPlusOne");
    glUniform1ui(glGetUniformLocation(program, name), bones[b].parentIndexPlusOne);
    delete name;
  }

  vertexTransformCoefsTexture.bind(MAP_TEXTURE_VERTEX_TRANSFORM_COEF);
  glUniform1i(glGetUniformLocation(program, "vertexTransformCoef"), MAP_TEXTURE_VERTEX_TRANSFORM_COEF);
}

void MeshSkeleton::initVerticesTranformsCoef() {
  if(vertexTransformCoefs && verticesCount >  0) {
    vertexTransformCoefsTexture.interpolateToNearest = true;
    vertexTransformCoefsTexture.initFromData(
      vertexTransformCoefs,
      verticesCount*textureElemSize,
      bones.size()*textureElemSize
      , 1, false
    );
  }
}

void MeshSkeleton::setVerticesCoef(unsigned transformIndex, unsigned vertexIndex, float value) {
  for(unsigned i=0; i<textureElemSize; i++) {
    for(unsigned j=0; j<textureElemSize; j++) {
      vertexTransformCoefs[
          ((transformIndex*textureElemSize+i) * verticesCount*textureElemSize)
          + (vertexIndex*textureElemSize+j)
      ] = value;
    }
  }
}

std::vector<Renderable*> & MeshSkeleton::getSkeletonMesh(const glm::vec3 & color, unsigned highlightBoneIndex, const glm::vec3 & colorHighlight) {
  boneMeshes.clear();
  for(int i=0; i<bones.size(); i++) {
    auto & bone = bones.at(i);

    std::vector<glm::vec3> triangles;
    std::vector<glm::vec3> colors;

    auto v = glm::cross(bone.A-bone.B, glm::vec3(0,0,1));
    v = glm::normalize(v);
    
    triangles.push_back(glm::vec3(0));
    triangles.push_back(bone.B+glm::vec3(0, 0, 0.02)-bone.A);
    triangles.push_back(bone.B+glm::vec3(0, 0, -0.02)-bone.A);

    triangles.push_back(glm::vec3(0));
    triangles.push_back(bone.B+v*0.02f-bone.A);
    triangles.push_back(bone.B+glm::vec3(0, 0, 0.02)-bone.A);

    triangles.push_back(glm::vec3(0));
    triangles.push_back(bone.B+v*0.02f-bone.A);
    triangles.push_back(bone.B+glm::vec3(0, 0, -0.02)-bone.A);
    
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

    std::vector<glm::uvec3> faces;
    Mesh * m = new Mesh(
      new MeshGeometry(triangles, faces, colors),
      MeshMaterial::meshGetSimplePhongMaterial(color*0.f, color*0.f, 5)
    );
    m->setPosition(bone.A);
    boneMeshes.push_back(m);
  }

  for(int i=0; i<bones.size(); i++) {
    auto & bone = bones.at(i);
    Mesh * mesh = (Mesh*)boneMeshes.at(i);
    if(bone.parentIndexPlusOne > 0) {
      mesh->setParent((Mesh*)boneMeshes.at(bone.parentIndexPlusOne-1));
      mesh->translateMesh(-(mesh->getCompleteParentPosition()));
    }
  }

  return boneMeshes;
}

void MeshSkeleton::rotateBoneArroundA(unsigned boneIndex, const glm::vec3 & angles) {

  glm::mat4 rot = glm::mat4(1);
  rot = glm::rotate(rot, angles.x, glm::vec3(1.f, 0.f, 0.f));
  rot = glm::rotate(rot, angles.y, glm::vec3(0.f, 1.f, 0.f));
  rot = glm::rotate(rot, angles.z, glm::vec3(0.f, 0.f, 1.f));
  bones.at(boneIndex).mat = rot;

  ((Mesh*)boneMeshes.at(boneIndex))->setOrientation(angles);
}
