#ifndef _SKETCHY_MESH_
#define _SKETCHY_MESH_

#include <utils.hpp>
#include <base.hpp>
#include <mesh-geometry.hpp>
#include <mesh-material.hpp>
#include <mesh-group.hpp>
#include <geometry/geometry.hpp>
#include <mesh-skeleton.hpp>

/*
Holds a mesh:
- The geometry data (vertex coords, vertex normal, vertex color, etc.)
- The material data (color, textures, etc.)
*/
class Mesh: public Renderable, public MeshGroup {
public:
    Mesh(MeshGeometry * geometry, MeshMaterial * material)
    : geometry(geometry), material(material) {
        
    }

    ~Mesh() {
        delete geometry;
        delete material;
        if(m_vao) {
            glDeleteVertexArrays(1, &m_vao);
            m_vao = 0;
        }
        if(m_posVbo) {
            glDeleteBuffers(1, &m_posVbo);
            m_posVbo = 0;
        }
        if(m_normalVbo) {
            glDeleteBuffers(1, &m_normalVbo);
            m_normalVbo = 0;
        }
        if(m_texCoordVbo) {
            glDeleteBuffers(1, &m_texCoordVbo);
            m_texCoordVbo = 0;
        }
        if(m_ibo) {
            glDeleteBuffers(1, &m_ibo);
            m_ibo = 0;
        }
    }

    void init() override;
    void render(Renderer & renderer, GLuint program,
        const glm::mat4 & extraTransformFirst,
        const glm::mat4 & extraTransformLast)  override;

    inline bool isMeshhInitialized() override { return isInitiliazed; }

    inline MeshGeometry * getGeometry() { return geometry; }
    inline MeshMaterial * getMaterial() { return material; }

    inline const Geometry::BoundingBox::BoundingBox & getBoundingBox() override {
        return geometry->getBoundingBox();
    }

    inline glm::mat4 getWorldTransform() override {
        return getWorldMatrix();
    }

    inline void setSkeleton(MeshSkeleton * skeleton) {
        this->skeleton = skeleton;
    }

    bool shouldRender = true;

protected:
    void initVertexPositions();
    void initVertexNormals();
    void initVertexColors();
    void initVertexTexCoord();
    void initTriangleIndices();

    bool isInitiliazed = false;

    MeshGeometry * geometry = nullptr;
    MeshMaterial * material = nullptr;
    MeshSkeleton * skeleton = nullptr;
    GLuint m_vao = 0;
    GLuint m_posVbo = 0;
    GLuint m_normalVbo = 0;
    GLuint m_colorVbo = 0;
    GLuint m_texCoordVbo = 0;
    GLuint m_ibo = 0;
};

#endif
