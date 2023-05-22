//
// Created by daniel on 22/05/23.
//

#include "smoothing.h"

void smoothing::setLambda(float lambda) {
    this->lambda = lambda;
}

void smoothing::laplacianSmoothing(int iterations, int k) {
    for (int i = 0; i < iterations; i++) {
        laplacianSmoothingOnce(k);
    }
}

void smoothing::laplacianSmoothingOnce(int k) {
    std::vector<glm::vec2> & newSketchPoints;
    int n = sketchPoints.size();
    for (int i = 0; i < n; i++) {
        glm::vec2 newPoint = (1-lambda)*sketchPoints[i];
        for (int j = -k; j < k; j++) {
            int trueIndex = j + i;

            if (trueIndex < 0 || trueIndex >= n){
                trueIndex = i;
            }// Now trueIndex is between 0 and n-1

            if (j != 0){
                newPoint += lambda*sketchPoints[trueIndex]/k;
            }
        }
        newSketchPoints.push_back(newPoint);
    }
    &sketchPoints = newSketchPoints;
}
