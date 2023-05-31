#ifndef _SKETCHY_RIGGING_MESH_
#define _SKETCHY_RIGGING_MESH_

#include "rigging.hpp"
#include <mesh.hpp>

class RiggingMesh {
public:
    RiggingMesh() {}

    inline static Mesh * createRiggingSkeletonMesh(const Rigging & rigging) {
        std::vector<glm::vec3> triangles;
        for(auto & bone : rigging.getBones()) {
            triangles.push_back(bone.getA().getPoint());
            triangles.push_back(bone.getB().getPoint()+glm::vec3(0, 0, 0.02));
            triangles.push_back(bone.getB().getPoint()+glm::vec3(0, 0, -0.02));
        }

        std::vector<glm::uvec3> faces;
        glm::vec3 color = {200, 200, 100};
        return new Mesh(
            new MeshGeometry(triangles, faces),
            MeshMaterial::meshGetBasicMaterial(color)
        );
    }

private:

};

#endif
