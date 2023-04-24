#version 420 core

layout(location=0) in vec3 vPosition;

uniform mat4 lightProjViewMat, worldMat;

void main() {
  gl_Position = lightProjViewMat*worldMat*vec4(vPosition, 1);
}
