//
// Created by daniel on 16/06/23.
//

#include "operations.h"

void operations::merge(std::vector<glm::vec3> &positions1, std::vector<glm::uvec3> &faces1,
                       std::vector<glm::vec3> &positions2, std::vector<glm::uvec3> &faces2) {
    // TODO: Implement + use CGAL libs
    // The algorithm is the following
    // 1. Get the intersection of the two meshes, which can be assimilated to a closed loop
    // 2. Each surface mesh is accommodated to the intersection of the loop (cf https://www.cs.tau.ac.il/~dcor/articles/2007/A-Sketch-Based.pdf)
    // 3. Discard the triangles inside the other surface
    // 4. Merge the two meshes along the shared boundary (if necessary, create new vertices by splitting boundary edges)
    // 5. Apply localized Laplacian smoothing (include vertex which closest vertex from the intersection loop is vi, if
    // dist(vertex, vi) < 0.6 dist(vi, center)

    std::vector<glm::vec3> intersectionPoints;

    // How to find the points?
    // Current solution: iterate through the edges and get the set of points corresponding to the intersecting edges



}
