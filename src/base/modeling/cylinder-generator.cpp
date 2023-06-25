#include "cylinder-generator.hpp"

template <>
struct std::less<glm::vec2>  {
    bool operator()(const glm::vec2 & a, const glm::vec2 & b) const {
        return a.x<b.x || (a.x==b.x && a.y<b.y);
    }
};

void CylinderGenerator::compute(unsigned circleSampleCount) {
    float pi = M_PI;
    float stepAngle = (2.0f*pi)/circleSampleCount;
    std::vector<std::vector<glm::vec3>> circles;
    for(unsigned index=0; index<axis.size(); index++) {
        // std::cout << "Axis point " << std::endl;
        auto & axisPoint = axis[index];
        std::vector<glm::vec3> circle;

        auto chordIt = axisPointToChord.find(axisPoint);
        if(chordIt == axisPointToChord.end()) {
            std::cerr << "axisPoint with no chord" << std::endl;
            assert(false);
        }
        auto chord = chordIt->second; //Geometry::getChordOnPoint(axisPoint, shape, chords);
        auto chordPointA = shape[chord.a];
        auto chordPointB = shape[chord.b];

        // showVec(axisPoint, "\t axisPoint = ");
        // showVec(chordPointA, "\t chordPointA = ");
        // showVec(chordPointB, "\t chordPointB = ");

        glm::vec3 rotAxis(0);
        if(index < axis.size()-1) {
            // If not the last point of the axis
            glm::vec2 rotAxis2d = glm::normalize(axis[index+1]-axisPoint);
            rotAxis.x = rotAxis2d.x;
            rotAxis.y = rotAxis2d.y;
        }
        else {
            // If last point of the axis
            glm::vec2 rotAxis2d = glm::normalize(axisPoint-axis[index-1]);
            rotAxis.x = rotAxis2d.x;
            rotAxis.y = rotAxis2d.y;
        }

        glm::vec3 center(
            (chordPointA.x+chordPointB.x)/2.0,
            (chordPointA.y+chordPointB.y)/2.0,
            0
        );

        for(unsigned i=0; i<circleSampleCount; i++) {
            glm::vec3 p(chordPointA, 0.0f);
            glm::mat4 rotMat = glm::rotate(glm::mat4(1.0), -pi/2.f + stepAngle*i, rotAxis);
            // showVec(p, "\t p = ");
            // showVec(center, "\t center = ");
            // showVec(rotAxis, "\t rotAxis = ");
            p = p - center;
            // showVec(p, "\t p-center = ");
            p = rotMat * glm::vec4(p, 0);
            p = p + center;
            circle.push_back(p);
            // showVec(p, "\t");
            // std::cout << std::endl;
        }
        circles.push_back(circle);

        if(index == axis.size()-2) {
            beforeLastChord = chord;
        }
        else if(index == axis.size()-1) {
            lastChord = chord;
        }
    }

    for(unsigned c=0; c<circles.size(); c++) {
        std::vector<glm::vec3> & circle = circles[c];
        for(unsigned p=0; p<circle.size(); p++) {
            genVertexPos.push_back(circle[p]);

            if(c==circles.size()-1) {
                if(p<circle.size()/2) {
                    lastSectionSideA.push_back(genVertexPos.size()-1);
                }
                else {
                    lastSectionSideB.push_back(genVertexPos.size()-1);
                }
            }
        }
    }

    unsigned nextCircleIndex = 0;
    for(unsigned circleIndex=0; circleIndex<circles.size()-1; circleIndex++) {
        nextCircleIndex = circleIndex+1;
        for(unsigned circlePointIndex=0; circlePointIndex<=circleSampleCount; circlePointIndex++) {
            unsigned indexInCicle_curr_circle = circlePointIndex%circleSampleCount;
            unsigned indexInCicle_next_circle = (circlePointIndex+1)%circleSampleCount;
            
            // Points on current circle
            unsigned a = indexInCicle_curr_circle + circleIndex*circleSampleCount;
            unsigned b = indexInCicle_next_circle + circleIndex*circleSampleCount;
            // Points on next circle
            unsigned c = indexInCicle_curr_circle + nextCircleIndex*circleSampleCount;
            unsigned d = indexInCicle_next_circle + nextCircleIndex*circleSampleCount;

            genFaces.push_back({a, b, c});
            genFaces.push_back({b, d, c});
        }
    }

    // Close the start of the cylinder.
    glm::vec3 center;
    for(unsigned circlePointIndex=0; circlePointIndex<=circleSampleCount; circlePointIndex++) {
        unsigned indexInCicle_curr_circle = circlePointIndex%circleSampleCount;
        center += genVertexPos[indexInCicle_curr_circle];
    }
    center *= 1.0f/circleSampleCount;
    genVertexPos.push_back(center);
    unsigned centerIndex = genVertexPos.size()-1;
    for(unsigned circlePointIndex=0; circlePointIndex<=circleSampleCount; circlePointIndex++) {
        unsigned indexInCicle_curr_circle = circlePointIndex%circleSampleCount;
        unsigned indexInCicle_next_circle = (circlePointIndex+1)%circleSampleCount;
        
        // Points on current circle
        unsigned a = indexInCicle_curr_circle;
        unsigned b = indexInCicle_next_circle;

        genFaces.push_back({a, b, centerIndex});
    }
}
