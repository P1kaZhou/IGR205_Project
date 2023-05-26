#include "cylindrical-douglas-peucker.hpp"

void CDP::compute() {
    skeleton.clear();
    for(auto ax : axis) {
        auto skel = std::vector<glm::vec2>();

        computeSingleAxisSkeleton(
            ax, ax.begin(), ax.begin()+ax.size()-1, skel
        );

        skeleton.push_back(skel);
    }
}

void CDP::computeSingleAxisSkeleton(
    const std::vector<glm::vec2> & axi,
    std::vector<glm::vec2>::iterator start,
    std::vector<glm::vec2>::iterator end,
    std::vector<glm::vec2> & skeleton
){
    std::cout << "CDP axis" << std::endl;
    if(start == end) return;

    auto cur = start+1;
    auto split = start+1;
    bool haveSplit = false;
    float error = 0;
    float maxError = FLT_MIN;
    for(;cur!=end;cur++) {
        error = computeError(*start, *end, *cur);
        std::cout << "error : " << error << std::endl;
        if(error > threshold && maxError <= error) {
            split = cur;
            maxError = error;
            haveSplit = true;
            std::cout << "split" << std::endl;
        }
    }

    if(haveSplit) {
        std::cout << "end" << std::endl;
        computeSingleAxisSkeleton(axi, start, split, skeleton);
        skeleton.push_back(*split);
        computeSingleAxisSkeleton(axi, split, end, skeleton);
    } 
}
