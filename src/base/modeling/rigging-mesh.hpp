#ifndef _SKETCHY_RIGGING_MESH_
#define _SKETCHY_RIGGING_MESH_

#include "rigging.hpp"
#include <mesh.hpp>

class RiggingMesh {
public:
    RiggingMesh() {}

    inline static Mesh * createRiggingSkeletonMesh(
        const Rigging & rigging,
        const int highlightBoneIndex,
        const glm::vec3 & color,
        const glm::vec3 & colorHighlight
    ) {
        std::vector<glm::vec3> triangles;
        std::vector<glm::vec3> colors;
        for(int i=0; i<rigging.getBones().size(); i++) {
            auto & bone = rigging.getBones()[i];

            auto v = glm::cross(bone.getA().getPoint()-bone.getB().getPoint(), glm::vec3(0,0,1));
            v = glm::normalize(v);
            
            triangles.push_back(bone.getA().getPoint());
            triangles.push_back(bone.getB().getPoint()+glm::vec3(0, 0, 0.02));
            triangles.push_back(bone.getB().getPoint()+glm::vec3(0, 0, -0.02));

            triangles.push_back(bone.getA().getPoint());
            triangles.push_back(bone.getB().getPoint()+v*0.02f);
            triangles.push_back(bone.getB().getPoint()+glm::vec3(0, 0, 0.02));

            triangles.push_back(bone.getA().getPoint());
            triangles.push_back(bone.getB().getPoint()+v*0.02f);
            triangles.push_back(bone.getB().getPoint()+glm::vec3(0, 0, -0.02));
            
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

private:

};

#endif
