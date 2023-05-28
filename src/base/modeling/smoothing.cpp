//
// Created by daniel on 22/05/23.
//

#include "smoothing.hpp"

void smoothing::setLambda(float lambda) {
    this->lambda = lambda;
}

void smoothing::applyLaplacianToMedial(MedialAxis & medialAxis, int iterations) {
    for (int k = 0; k < iterations; k++) {
        std::vector<MedialAxisPoint*> points = medialAxis.getPoints();
        // Iterate through every medial axis point
        for (int i = 0; i < points.size(); i++) {
            MedialAxisPoint* point = points[i];
            glm::vec2 newPoint = (1-lambda)*point->getPoint();
            int adjPointsIndex = point->getAdjIndex(point);
            MedialAxisPoint * adjPoints = point->getAdjs()[adjPointsIndex];
            int v = 0;
            glm::vec2 alternativePoint = glm::vec2(0,0);
            /*
            for (auto point : adjPoints) {
                MedialAxisPoint * adjPoint = adjPoints[j];
                newPoint += lambda*adjPoint->getPoint()/v;
            }
            // Add the corrected point to the medial axis

            */
    }

    }
}
