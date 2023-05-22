//
// Created by daniel on 22/05/23.
//

#ifndef SKETCHY_SMOOTHING_H
#define SKETCHY_SMOOTHING_H


class smoothing {

private:
    std::vector<glm::vec2> & sketchPoints;
    float lambda;

public:

    void setLambda(float lambda);
    void laplacianSmoothing(int iterations, int k);
    void laplacianSmoothingOnce(int k);

};


#endif //SKETCHY_SMOOTHING_H
