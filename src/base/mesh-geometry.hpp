#ifndef _SKETCHY_MESH_GEOMETRY_
#define _SKETCHY_MESH_GEOMETRY_

#include <utils.hpp>
#include <geometry/geometry.hpp>

/*
Holds geometry data.
*/
class MeshGeometry {
public:
    MeshGeometry() {}

    MeshGeometry(
        std::vector<glm::vec3> & vertexPositions,
        std::vector<glm::uvec3> & faces
    ): vertexPositions(vertexPositions), faces(faces) {
        boundingBox = Geometry::BoundingBox::computeBoundingBox(vertexPositions);
        recomputeVertexNormals();
    }

    static MeshGeometry * meshGetTriangleData(float width, float height, const glm::vec3 * color);
    static MeshGeometry * meshGetPlaneData(float width, float height, const glm::vec3 * color);
    static MeshGeometry * meshGetCubeData(float lx, float ly, float lz, const glm::vec3 * color);
    static MeshGeometry * meshGetSphereData(float radius, unsigned precH, unsigned precV, const glm::vec3 * color);
    static MeshGeometry * meshGetConeData();
    static MeshGeometry * meshGetPyramidData();

    static MeshGeometry * meshGetFromOfffile(const char * filename);

    inline void clear() {
        vertexPositions.clear();
        vertexNormals.clear();
        vertexColors.clear();
        vertexTexCoord.clear();
        faces.clear();
    }

    inline const std::vector<glm::vec3> & getVertexPositions() const { return vertexPositions; }
    inline const std::vector<glm::vec3> & getVertexNormals() const { return vertexNormals; }
    inline const std::vector<glm::vec3> & getVertexColors() const { return vertexColors; }
    inline const std::vector<glm::vec2> & getVertexTexCoord() const { return vertexTexCoord; }
    inline const std::vector<glm::uvec3> & getFaces() const { return faces; }

    inline bool hasVertexPositions() const { return vertexPositions.size()>0; }
    inline bool hasVertexNormals() const { return vertexNormals.size()>0; }
    inline bool hasVertexColors() const { return vertexColors.size()>0; }
    inline bool hasVertexTexCoord() const { return vertexTexCoord.size()>0; }
    inline bool hasFaces() const { return faces.size()>0; }

    inline const Geometry::BoundingBox::BoundingBox & getBoundingBox() {
        return boundingBox;
    }

    inline unsigned getVertexPositionsVCount() const { return vertexPositions.size()*3; }
    inline unsigned getVertexNormalsVCount() const { return vertexNormals.size()*3; }
    inline unsigned getVertexColorsVCount() const { return vertexColors.size()*3; }
    inline unsigned getVertexTexCoordVCount() const { return vertexTexCoord.size()*2; }
    inline unsigned getFacesVCount() const { return faces.size()*3; }

    void showData() const;

    void recomputeVertexNormals();
    void recomputeVertexTexCoord_flat_mapping(short axis = 1);
    void recomputeVertexTexCoord_sphere_mapping();

    void normalizeVertexPositions();

private:
    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec3> vertexColors;
    std::vector<glm::vec2> vertexTexCoord;
    std::vector<glm::uvec3> faces;

    Geometry::BoundingBox::BoundingBox boundingBox;
};

#endif
