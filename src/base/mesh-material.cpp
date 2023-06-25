#include <mesh-material.hpp>

MeshMaterial * MeshMaterial::meshGetSimplePhongMaterial(
  const glm::vec3 & diffuseColor = glm::vec3(), const glm::vec3 & specularColor = glm::vec3(), unsigned int shininess = 1
) {
  MeshMaterial * mat = new MeshMaterial();
  mat->diffuseColor = diffuseColor;
  mat->specularColor = specularColor;
  mat->shininess = shininess;

  mat->blankTexture = new ImageTexture();
  mat->blankTexture->initFromImage(nullptr, 3, true);

  return mat;
}

MeshMaterial * MeshMaterial::meshGetBasicMaterial(
  const glm::vec3 & basicColor
) {
  MeshMaterial * mat = new MeshMaterial();
  mat->basicColor = basicColor;

  mat->blankTexture = new ImageTexture();
  mat->blankTexture->initFromImage(nullptr, 3, true);

  return mat;
}

MeshMaterial * MeshMaterial::meshGetSimpleTextureMaterial(
  const char * diffuseMapFilename, const glm::vec3 & specularColor = glm::vec3(), unsigned int shininess = 1
) {
  ImageTexture * diffuseMap = new ImageTexture();
  diffuseMap->initFromImage(diffuseMapFilename, 3, true);
  MeshMaterial * mat = new MeshMaterial();
  mat->setDiffuseMap(diffuseMap);
  mat->specularColor = specularColor;
  mat->shininess = shininess;

  mat->blankTexture = new ImageTexture();
  mat->blankTexture->initFromImage(nullptr, 3, true);

  return mat;
}

MeshMaterial * MeshMaterial::meshGetBasicTextureMaterial(
  const char * diffuseMapFilename, const glm::vec3 & basicColor
) {
  ImageTexture * diffuseMap = new ImageTexture();
  diffuseMap->initFromImage(diffuseMapFilename, 3, true);
  MeshMaterial * mat = new MeshMaterial();
  mat->setDiffuseMap(diffuseMap);
  mat->basicColor = basicColor;

  mat->blankTexture = new ImageTexture();
  mat->blankTexture->initFromImage(nullptr, 3, true);

  return mat;
}

MeshMaterial * MeshMaterial::meshGetTextureMaterial(
  const char * diffuseMapFilename, const char * specularMapFilename, const char * normalMapFilename, const glm::vec3 & basicColor
) {
  ImageTexture * diffuseMap = new ImageTexture();
  diffuseMap->initFromImage(diffuseMapFilename, 3, true);
  ImageTexture * specularMap = new ImageTexture();
  specularMap->initFromImage(specularMapFilename, 3, true);
  ImageTexture * normalMap = new ImageTexture();
  normalMap->initFromImage(normalMapFilename, 3, true);
  
  MeshMaterial * mat = new MeshMaterial();
  mat->setDiffuseMap(diffuseMap);
  mat->setSpecularMap(specularMap);
  mat->setNormalMap(normalMap);
  mat->basicColor = basicColor;

  mat->blankTexture = new ImageTexture();
  mat->blankTexture->initFromImage(nullptr, 3, true);

  return mat;
}

void MeshMaterial::updateUniforms(
  const glm::mat4 & viewMatrix,
  const glm::mat4 & projMatrix,
  const glm::mat4 & worldMatrix,
  GLuint program = 0
) {
  glUniformMatrix4fv(glGetUniformLocation(program, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glUniformMatrix4fv(glGetUniformLocation(program, "projMat"), 1, GL_FALSE, glm::value_ptr(projMatrix));
  glUniformMatrix4fv(glGetUniformLocation(program, "worldMat"), 1, GL_FALSE, glm::value_ptr(worldMatrix));
  getOpenGLError("uniform matrices");

  glUniform3f(glGetUniformLocation(program, "basicColor"), basicColor.r, basicColor.g, basicColor.b);
  glUniform3f(glGetUniformLocation(program, "diffuseColor"), diffuseColor.r, diffuseColor.g, diffuseColor.b);
  glUniform3f(glGetUniformLocation(program, "specularColor"), specularColor.r, specularColor.g, specularColor.b);
  glUniform1ui(glGetUniformLocation(program, "shininess"), shininess);
  getOpenGLError("uniform colors");

  if(diffuseMap) {
    diffuseMap->bind(MAP_TEXTURE_UNIT_DIFFUSE);
    glUniform1i(glGetUniformLocation(program, "diffuseMap"), MAP_TEXTURE_UNIT_DIFFUSE);
    getOpenGLError("uniform diffuse map");
  }
  else {
    ImageTexture::unbind(MAP_TEXTURE_UNIT_DIFFUSE);
    //blankTexture->unbind();
  }
  if(specularMap) {
    specularMap->bind(MAP_TEXTURE_UNIT_SPECULAR);
    glUniform1i(glGetUniformLocation(program, "specularMap"), MAP_TEXTURE_UNIT_SPECULAR);
    getOpenGLError("uniform specular map");
  }
  else  {
    ImageTexture::unbind(MAP_TEXTURE_UNIT_SPECULAR);
    //blankTexture->unbind();
  }
  if(shininessMap) {
    shininessMap->bind(MAP_TEXTURE_UNIT_SHININESS);
    glUniform1i(glGetUniformLocation(program, "shininessMap"), MAP_TEXTURE_UNIT_SHININESS);
    getOpenGLError("uniform shininess map");
  }
  else {
    ImageTexture::unbind(MAP_TEXTURE_UNIT_SHININESS);
    //blankTexture->unbind();
  }
  if(normalMap) {
    normalMap->bind(MAP_TEXTURE_UNIT_NORMAL);
    glUniform1i(glGetUniformLocation(program, "normalMap"), MAP_TEXTURE_UNIT_NORMAL);
    getOpenGLError("uniform normal map");
  }
  else {
    ImageTexture::unbind(MAP_TEXTURE_UNIT_NORMAL);
    //blankTexture->unbind();
  }
}
