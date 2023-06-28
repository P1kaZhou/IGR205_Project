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
    uint parentIndexPlusOne;
};

uniform Bone bones[15];
uniform uint bonesCount;
uniform uint verticesCount;
uniform uint textureElemSize;
layout(binding=4) uniform sampler2D vertexTransformCoef;
uniform bool hasBones;

void main() {
    frag.fcolor = vColor;

    vec4 pos = vec4(vPosition, 1.0);
    vec4 normal = vec4(vNormal, 0.0);
    
    if(hasBones) {
        int center = int(textureElemSize/2);
        for(int b=0; b<bonesCount; b++) {
            uint vert = gl_VertexID*textureElemSize+center;
            uint bone = b*textureElemSize+center;
            vec2 coefVec = vec2(
                vert/float(verticesCount*textureElemSize),
                bone/float(bonesCount*textureElemSize)
            );
            float coef = texture(vertexTransformCoef, coefVec).r;
            if(coef == 1.0) {
                pos = pos - vec4(bones[b].A, 0.0);
                pos = bones[b].rotationMat * pos;
                pos = pos + vec4(bones[b].A, 0.0);
                
                Bone parent = bones[b];
                vec4 oldPos = pos;
                while(
                    parent.parentIndexPlusOne > 0 &&
                    parent.parentIndexPlusOne <= bonesCount &&
                    parent.parentIndexPlusOne-1!=b
                ) {
                    parent = bones[parent.parentIndexPlusOne-1];

                    pos = pos - vec4(parent.A, 0.0);
                    pos = parent.rotationMat * pos;
                    pos = pos + vec4(parent.A, 0.0);
                }
                if(length(pos)==0) {
                    pos = oldPos;
                }
            }

        }
    }

    frag.normal = ( worldMat * normal ).xyz;
    // frag.normal = vNormal;
    frag.texCoord = vTexCoord;
    frag.pos = ( worldMat * pos ).xyz;

    gl_Position = projMat * viewMat * worldMat * pos;
}
