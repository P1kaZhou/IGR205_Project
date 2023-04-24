#version 420 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec3 vColor;
layout(location=3) in vec2 vTexCoord;

out FRAG {
    vec3 pos;
    vec3 normal;
    vec2 texCoord;
    vec3 fcolor;
} frag;

uniform mat4 viewMat, projMat, worldMat;

void main() {
    frag.fcolor = vColor;
    frag.normal = ( worldMat * vec4(vNormal, 0.0) ).xyz;
    // frag.normal = vNormal;
    frag.texCoord = vTexCoord;
    frag.pos = ( worldMat * vec4(vPosition, 1.0) ).xyz;

    gl_Position = projMat * viewMat * worldMat * vec4(vPosition, 1.0);
}
