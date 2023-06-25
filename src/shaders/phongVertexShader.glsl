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


struct Bone {
    vec3 A;
    mat4 rotationMat;
    //float weights[1000];
};

uniform Bone bones[15];
uniform uint bonesCount;
uniform uint verticesCount;
layout(binding=4) uniform sampler2D vertexTransformCoef;
uniform bool hasBones;

out float ttt;

void main() {
    frag.fcolor = vColor;

    vec4 pos = vec4(vPosition, 1.0);
    vec4 normal = vec4(vNormal, 0.0);

    ttt = 0;
    
    if(hasBones) {
        for(int b=0; b<bonesCount; b++) {
            vec2 coefVec = vec2(gl_VertexID/float(verticesCount), b/float(bonesCount));
            float coef = texture(vertexTransformCoef, coefVec).r;
            // float coef = bones[b].weights[gl_VertexID];
            if(coef == 1.0) {
                pos = pos - vec4(bones[b].A, 0.0);
                pos = bones[b].rotationMat * pos;
                pos = pos + vec4(bones[b].A, 0.0);
            }
        }
    }

    frag.normal = ( worldMat * normal ).xyz;
    // frag.normal = vNormal;
    frag.texCoord = vTexCoord;
    frag.pos = ( worldMat * pos ).xyz;

    gl_Position = projMat * viewMat * worldMat * pos;
}
