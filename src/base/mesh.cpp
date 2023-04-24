#include "mesh.hpp"

#define _USE_MATH_DEFINES

// The main rendering call
void Mesh::render(Renderer & renderer, GLuint program = 0,
  const glm::mat4 & extraTransformFirst = glm::mat4(1),
  const glm::mat4 & extraTransformLast = glm::mat4(1))
{
  const glm::mat4 viewMatrix = renderer.getCamera().computeViewMatrix();
  const glm::mat4 projMatrix = renderer.getCamera().computeProjectionMatrix();
  const glm::mat4 worldMatrix = extraTransformLast * getWorldMatrix() * extraTransformFirst;

  getOpenGLError("using program");
  // Camera position
  const auto cameraPos = renderer.getCamera().getPosition();
  glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
  // Material data
  material->updateUniforms(viewMatrix, projMatrix, worldMatrix, program);
  // Lights data
  glUniform1ui(glGetUniformLocation(program, "lightCount"), renderer.getLights().size());
  getOpenGLError("uniform light count");
  for(unsigned i=0; i<renderer.getLights().size(); i++) {
    renderer.getLights()[i]->updateUniforms(program, i);
  }

  glBindVertexArray(m_vao);
  if(geometry->getFaces().size() > 0) {
    glDrawElements(GL_TRIANGLES, geometry->getFacesVCount(), GL_UNSIGNED_INT, 0);
    getOpenGLError("draw elements");
  }
  else {
    glDrawArrays(GL_TRIANGLES, 0, geometry->getVertexColorsVCount());
    getOpenGLError("draw arrays");
  }
}

void Mesh::initVertexPositions() {
  size_t length = sizeof(float)*(geometry->getVertexPositionsVCount());
#ifdef _MY_OPENGL_IS_33_
  glGenBuffers(1, &m_posVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
  glBufferData(GL_ARRAY_BUFFER, length, geometry->getVertexPositions().data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);
#else
  // glCreateBuffers(1, &m_posVbo);
  // glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
  // glNamedBufferStorage(m_posVbo, length, geometry->getVertexPositions().data(), GL_DYNAMIC_STORAGE_BIT);
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  // glEnableVertexAttribArray(0);
#endif
  getOpenGLError("vertex positions attrib");
}

void Mesh::initVertexNormals() {
  size_t length = sizeof(float)*(geometry->getVertexNormalsVCount());
#ifdef _MY_OPENGL_IS_33_
  glGenBuffers(1, &m_normalVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_normalVbo);
  glBufferData(GL_ARRAY_BUFFER, length, geometry->getVertexNormals().data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(1);
#else
  // glCreateBuffers(1, &m_normalVbo);
  // glBindBuffer(GL_ARRAY_BUFFER, m_normalVbo);
  // glNamedBufferStorage(m_normalVbo, length, geometry->getVertexNormals().data(), GL_DYNAMIC_STORAGE_BIT);
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  // glEnableVertexAttribArray(1);
#endif
  getOpenGLError("vertex normals attrib");
}

void Mesh::initVertexColors() {
  size_t length = sizeof(float)*(geometry->getVertexColorsVCount());
#ifdef _MY_OPENGL_IS_33_
  glGenBuffers(1, &m_colorVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo);
  glBufferData(GL_ARRAY_BUFFER, length, geometry->getVertexColors().data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(2);
#else
  // glCreateBuffers(1, &m_colorVbo);
  // glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo);
  // glNamedBufferStorage(m_colorVbo, length, geometry->getVertexColors().data(), GL_DYNAMIC_STORAGE_BIT);
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  // glEnableVertexAttribArray(2);
#endif
  getOpenGLError("vertex colors attrib");
}

void Mesh::initVertexTexCoord() {
  size_t length = sizeof(float)*(geometry->getVertexTexCoordVCount());
#ifdef _MY_OPENGL_IS_33_
  glGenBuffers(1, &m_texCoordVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo);
  glBufferData(GL_ARRAY_BUFFER, length, geometry->getVertexTexCoord().data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(3);
#else
  // glCreateBuffers(1, &m_texCoordVbo);
  // glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo);
  // glNamedBufferStorage(m_texCoordVbo, length, geometry->getVertexTexCoord().data(), GL_DYNAMIC_STORAGE_BIT);
  // glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  // glEnableVertexAttribArray(3);
#endif
  getOpenGLError("vertex texture coordinates attrib");
}

void Mesh::initTriangleIndices() {
  size_t length = sizeof(unsigned int)*(geometry->getFacesVCount());
#ifdef _MY_OPENGL_IS_33_
  glGenBuffers(1, &m_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, geometry->getFaces().data(), GL_DYNAMIC_READ);
#else
//   glCreateBuffers(1, &m_ibo);
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
//   glNamedBufferStorage(m_ibo, length, geometry->getIndices().data(), GL_DYNAMIC_STORAGE_BIT);
#endif
  getOpenGLError("triangle indices buffer");
}

/*Init GPU buffers.*/
void Mesh::init() {
#ifdef _MY_OPENGL_IS_33_
  glGenVertexArrays(1, &m_vao);
#else
//   glCreateVertexArrays(1, &m_vao);
#endif
  glBindVertexArray(m_vao);

  initVertexPositions();
  if(geometry->hasVertexColors() && geometry->getVertexColors().size() == geometry->getVertexPositions().size()) {
    initVertexColors();
  }
  if(geometry->hasVertexNormals() && geometry->getVertexNormals().size() == geometry->getVertexPositions().size()) {
    initVertexNormals();
  }
  if(geometry->hasVertexTexCoord() && geometry->getVertexTexCoord().size() == geometry->getVertexPositions().size()) {
    initVertexTexCoord();
  }
  if(geometry->hasFaces() && geometry->getFaces().size() > 0) {
    initTriangleIndices();
  }

  glBindVertexArray(0);
  isInitiliazed = true;
}

void update(const float currentTimeInSec, Renderer renderer) {

}
