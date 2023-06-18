//
// Created by daniel on 16/06/23.
//

#ifndef SKETCHY_OPERATIONS_H
#define SKETCHY_OPERATIONS_H

#include <vector>
#include "glm/glm.hpp"

class operations {

    void merge(std::vector<glm::vec3> &positions1, std::vector<glm::uvec3> &faces1,
               std::vector<glm::vec3> &positions2, std::vector<glm::uvec3> &faces2);

};


#endif //SKETCHY_OPERATIONS_H
