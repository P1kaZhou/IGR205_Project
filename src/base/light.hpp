#ifndef _SKETCHY_LIGHT_
#define _SKETCHY_LIGHT_

#include <utils.hpp>
#include <program.hpp>
#include <frame-buffer.hpp>

#define MAP_TEXTURE_UNIT_DEPTH_MAP 4

/*
Holds a light source data :
- position
- color components params (ambient, diffuse, specular)
And methods to send uniform to the GPU for the Phong shader program.
*/
class Light {
    friend class Renderer;
    friend class Mesh;
protected:
    glm::vec3 lightPos;
    glm::vec3 lightAmbientColor;
    glm::vec3 lightDiffuseColor;
    glm::vec3 lightSpecularColor;

    float lightAtteConst = 1;
    float lightAtteLin = 0;
    float lightAtteQuad = 0;

    float lightCamNear = 0.1f;
    float lightCamFar = 15.0f;
    glm::vec3 lightCamUp = {1.0f, 0.0f, 0.0f};
    float lightFovAngleDegree = 180.0f;
    glm::vec3 lightLookAt = {0.0f, 0.0f, 0.0f};

    void buildUniformNames(int indexInLightsBuffer);

    char * uniformNamePos = nullptr;
    char * uniformNameAmbientColor = nullptr;
    char * uniformNameDiffuseColor = nullptr;
    char * uniformNameSpecularColor = nullptr;
    char * uniformNameAtteConst = nullptr;
    char * uniformNameAtteLin = nullptr;
    char * uniformNameAtteQuad = nullptr;
    char * uniformNameCamNear = nullptr;
    char * uniformNameCamFar = nullptr;
    char * uniformNameCamUp = nullptr;
    char * uniformNameFovAngleDegree = nullptr;
    char * uniformNameLookAt = nullptr;
    char * uniformNameShadowMap = nullptr;
    char * uniformNameShadowProjViewMat = nullptr;
    char * uniformNameIsSpotLight = nullptr;
    char * uniformNameIsShadowCaster = nullptr;

    FrameBuffer depthFrameBuffer;
    glm::mat4 shadowProjViewMat;

    bool isShadowCaster = true;
    bool isSpotLight = true;

    void init(int width, int height);
    void updateFromLightDepthUniforms();
    void bindDepthFrameBuffer();

    void updateUniforms(GLuint program, unsigned lightIndex);

public:
    Light() {
        Program::getInstanceShadowMap()->subscribe();
    }
    ~Light() {
        if(uniformNamePos) delete uniformNamePos;
        if(uniformNameAmbientColor) delete uniformNameAmbientColor;
        if(uniformNameDiffuseColor) delete uniformNameDiffuseColor;
        if(uniformNameSpecularColor) delete uniformNameSpecularColor;
        if(uniformNameAtteConst) delete uniformNameAtteConst;
        if(uniformNameAtteLin) delete uniformNameAtteLin;
        if(uniformNameAtteQuad) delete uniformNameAtteQuad;
        if(uniformNameCamNear) delete uniformNameCamNear;
        if(uniformNameCamFar) delete uniformNameCamFar;
        if(uniformNameCamUp) delete uniformNameCamUp;
        if(uniformNameFovAngleDegree) delete uniformNameFovAngleDegree;
        if(uniformNameLookAt) delete uniformNameLookAt;
        if(uniformNameIsSpotLight) delete uniformNameIsSpotLight;
        if(uniformNameIsShadowCaster) delete uniformNameIsShadowCaster;
    }

    inline Light * setSpot(
        float near = 0.1f, float far =  5.0f, float fovAngleDegree  = 120.0f,
        const glm::vec3 & lookAt = {1.0f, 0.0f, 0.0f}, const glm::vec3 & up = {0.0f, 0.1f, 0.0f}
    ) {
        lightCamNear = near;
        lightCamFar = far;
        lightCamUp = up;
        lightFovAngleDegree = fovAngleDegree;
        lightLookAt = lookAt;
        return this;
    }

    inline Light * makeSpotLight(bool isSpotLight) {
        this->isSpotLight = isSpotLight;
        return this;
    }
    inline Light * makeShadowCaster(bool isShadowCaster) {
        this->isShadowCaster = isShadowCaster;
        return this;
    }

    static Light * lightGetConstantCaster(
        glm::vec3 lightPos, glm::vec3 lightAmbientColor, glm::vec3 lightDiffuseColor, glm::vec3 lightSpecularColor
    );
};

#endif
