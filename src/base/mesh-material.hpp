#ifndef _SKETCHY_MESH_MATERIAL_
#define _SKETCHY_MESH_MATERIAL_

#include <utils.hpp>
#include <texture.hpp>
#include <program.hpp>

// Texture units indices

#define MAP_TEXTURE_UNIT_DIFFUSE 0
#define MAP_TEXTURE_UNIT_SPECULAR 1
#define MAP_TEXTURE_UNIT_SHININESS 2
#define MAP_TEXTURE_UNIT_NORMAL 3

/*
Holds the material of a mesh :
- diffuse color (or map)
- specular color (or map)
- shininess (or map)
- normal map
Has methods to bind uniforms for each material data.
By default, use the Phong Program instance.
*/
class MeshMaterial {
public:
    MeshMaterial() {
        Program::getInstancePhong()->subscribe();
    }
    ~MeshMaterial() {
        if(diffuseMap) delete diffuseMap;
        if(specularMap) delete specularMap;
        if(shininessMap) delete shininessMap;
        if(normalMap) delete normalMap;
        Program::getInstancePhong()->unsubscribe();
    }

    /*Update uniforms in GPU.*/
    void updateUniforms(
        const glm::mat4 & viewMatrix, const glm::mat4 & projMatrix, const glm::mat4 & worldMatrix, GLuint program);
    
    static MeshMaterial * meshGetSimplePhongMaterial(
        const glm::vec3 & diffuseColor, const glm::vec3 & specularColor, unsigned int shininess);
    static MeshMaterial * meshGetBasicMaterial(
        const glm::vec3 & basicColor);
    static MeshMaterial * meshGetSimpleTextureMaterial(
        const char * diffuseMapFilename, const glm::vec3 & specularColor, unsigned int shininess);
    static MeshMaterial * meshGetBasicTextureMaterial(
        const char * diffuseMapFilename, const glm::vec3 & basicColor);
    static MeshMaterial * meshGetTextureMaterial(
        const char * diffuseMapFilename, const char * specularMapFilename, const char * normalMapFilename, const glm::vec3 & basicColor);

    inline MeshMaterial * setBasicColor(glm::vec3 basicColor) { 
        this->basicColor = basicColor;
        return this; }
    inline MeshMaterial * setDiffuseColor(glm::vec3 diffuseColor) { 
        this->diffuseColor = diffuseColor;
        return this; }
    inline MeshMaterial * setSpecularColor(glm::vec3 specularColor) {
        this->specularColor = specularColor;
        return this; }
    inline MeshMaterial * setShininess(unsigned int shininess) {
        this->shininess = shininess;
        return this; }

    inline MeshMaterial * setDiffuseMap(Texture2D * diffuseMap) {
        if(diffuseMap->getComponentCount() == 3)  {
            if(this->diffuseMap) delete this->diffuseMap;
            this->diffuseMap = diffuseMap;
        }
        return this;
    }
    inline MeshMaterial * setSpecularMap(Texture2D * specularMap) {
        if(specularMap->getComponentCount() == 3)  {
            if(this->specularMap) delete this->specularMap;
            this->specularMap = specularMap;
        }
        return this;
    }
    inline MeshMaterial * setShininessMap(Texture2D * shininessMap) {
        if(shininessMap->getComponentCount() == 1)  {
            if(this->shininessMap) delete this->shininessMap;
            this->shininessMap = shininessMap;
        }
        return this;
    }
    inline MeshMaterial * setNormalMap(Texture2D * normalMap) {
        if(normalMap->getComponentCount() == 3)  {
            if(this->normalMap) delete this->normalMap;
            this->normalMap = normalMap;
        }
        return this;
    }

protected:
    glm::vec3 basicColor = glm::vec3(0); // rgb
    glm::vec3 diffuseColor = glm::vec3(0); // rgb
    glm::vec3 specularColor = glm::vec3(0); // rgb
    unsigned int shininess = 1; // i

    Texture2D * diffuseMap = nullptr; // rgb
    Texture2D * specularMap = nullptr; // rgb
    Texture2D * shininessMap = nullptr; // i
    Texture2D * normalMap = nullptr; // xyz

    Texture2D * blankTexture = nullptr; // rgb

};

#endif
