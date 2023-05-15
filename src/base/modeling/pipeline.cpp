#include "pipeline.hpp"

pipeline::Pipeline() {
    // Get the user sketch
    sketchPoints = std::vector<glm::vec2>(); // hardcoded for now
}

std::vector<Mesh> pipeline::computeCylinders(){
    // Perform the Chordal axis transform
}

Mesh pipeline::computeMeshFromCylinders(std::vector<Mesh>){
    // Perform the meshing
}

Rigging pipeline::computeSkeleton(Mesh){
    // Perform the rigging
}

pipeline::~Pipeline() {
    // Nothing to do
}

void pipeline::pipelineStep(){



}