//
// Created by daniel on 22/05/23.
//

#ifndef SKETCHY_SMOOTHING_HPP
#define SKETCHY_SMOOTHING_HPP


#include <vector>
#include "glm/vec2.hpp"
#include "medial-axis.hpp"

class smoothing {

private:
    std::vector<glm::vec2> & sketchPoints;
    float lambda;

public:

    void setLambda(float lambda);

    void applyLaplacianToMedial(MedialAxis & medialAxis, int iterations);

};


#endif //SKETCHY_SMOOTHING_HPP
