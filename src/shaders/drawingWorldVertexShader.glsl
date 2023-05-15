#version 420 core

layout(location=0) in vec3 vPositionCamSpace;

uniform mat4 viewMat, projMat;

void main() {
    vec4 worldPos = inverse(projMat * viewMat) * vec4(vPositionCamSpace, 1.0);
    gl_Position = vec4(
        worldPos.xyz/worldPos.w,
        1.0
    );
}
