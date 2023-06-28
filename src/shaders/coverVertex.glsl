#version 420 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec2 vTex;

out vec3 pos;
out vec2 texCoord;

void main() {
  pos = vPosition;
  texCoord = vTex;
  gl_Position = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
}
