#version 420 core

layout(location=0) in vec3 vPosition;

uniform mat4 viewMat, projMat, worldMat;

void main() {
    gl_Position = projMat * viewMat * worldMat * vec4(vPosition, 1.0);
}
