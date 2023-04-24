#include <light.hpp>

void Light::buildUniformNames(int indexInLightsBuffer) {
    if(!uniformNamePos) {
        uniformNamePos = buildIndexedString("lights[", indexInLightsBuffer, "].lightPos");
        uniformNameDiffuseColor = buildIndexedString("lights[", indexInLightsBuffer, "].lightDiffuseColor");
        uniformNameSpecularColor = buildIndexedString("lights[", indexInLightsBuffer, "].lightSpecularColor");
        uniformNameAmbientColor = buildIndexedString("lights[", indexInLightsBuffer, "].lightAmbientColor");
        uniformNameAtteConst = buildIndexedString("lights[", indexInLightsBuffer, "].lightAtteConst");
        uniformNameAtteLin = buildIndexedString("lights[", indexInLightsBuffer, "].lightAtteLin");
        uniformNameAtteQuad = buildIndexedString("lights[", indexInLightsBuffer, "].lightAtteQuad");

        uniformNameCamNear = buildIndexedString("lights[", indexInLightsBuffer, "].lightCamNear");
        uniformNameCamFar = buildIndexedString("lights[", indexInLightsBuffer, "].lightCamFar");
        uniformNameCamUp = buildIndexedString("lights[", indexInLightsBuffer, "].lightCamUp");
        uniformNameFovAngleDegree = buildIndexedString("lights[", indexInLightsBuffer, "].lightFovAngleDegree");
        uniformNameLookAt = buildIndexedString("lights[", indexInLightsBuffer, "].lightLookAt");

        uniformNameShadowMap = buildIndexedString("lights[", indexInLightsBuffer, "].shadowMap");
        uniformNameShadowProjViewMat = buildIndexedString("lights[", indexInLightsBuffer, "].shadowProjViewMat");

        uniformNameIsSpotLight = buildIndexedString("lights[", indexInLightsBuffer, "].isSpotLight");
        uniformNameIsShadowCaster = buildIndexedString("lights[", indexInLightsBuffer, "].isShadowCaster");
    }
}

void Light::init(int width, int height) {
    if(!depthFrameBuffer.isReady()) {
        depthFrameBuffer.init(width, height, false, true, false);
    }
}

void Light::updateUniforms(GLuint program, unsigned lightIndex) {
    glUniform3f(glGetUniformLocation(program, uniformNamePos),
        lightPos.x, lightPos.y, lightPos.z);
    getOpenGLError("uniform light pos");
    glUniform3f(glGetUniformLocation(program, uniformNameDiffuseColor),
        lightDiffuseColor.r, lightDiffuseColor.g, lightDiffuseColor.b);
    getOpenGLError("uniform light diffuse color");
    glUniform3f(glGetUniformLocation(program, uniformNameSpecularColor),
        lightSpecularColor.r, lightSpecularColor.g, lightSpecularColor.b);
    getOpenGLError("uniform light specular color");
    glUniform3f(glGetUniformLocation(program, uniformNameAmbientColor),
        lightAmbientColor.r, lightAmbientColor.g, lightAmbientColor.b);
    getOpenGLError("uniform light ambient color");
    glUniform1f(glGetUniformLocation(program, uniformNameAtteConst), lightAtteConst);
    getOpenGLError("uniform light attenuation constant");
    glUniform1f(glGetUniformLocation(program, uniformNameAtteLin), lightAtteLin);
    getOpenGLError("uniform light attenuation linear");
    glUniform1f(glGetUniformLocation(program, uniformNameAtteQuad), lightAtteQuad);
    getOpenGLError("uniform light attenuation quadratic");
    glUniform1f(glGetUniformLocation(program, uniformNameCamNear), lightCamNear);
    getOpenGLError("uniform light cam near");
    glUniform1f(glGetUniformLocation(program, uniformNameCamFar), lightCamFar);
    getOpenGLError("uniform light cam far");
    glUniform3f(glGetUniformLocation(program, uniformNameCamUp), lightCamUp.x, lightCamUp.y, lightCamUp.z);
    getOpenGLError("uniform light cam up");
    glUniform1f(glGetUniformLocation(program, uniformNameFovAngleDegree), lightFovAngleDegree);
    getOpenGLError("uniform light fov angle degree");
    glUniform3f(glGetUniformLocation(program, uniformNameLookAt), lightLookAt.x, lightLookAt.y, lightLookAt.z);
    getOpenGLError("uniform light look at");

    glUniform1i(glGetUniformLocation(program, uniformNameIsSpotLight), isSpotLight);
    getOpenGLError("uniform light is spot light");
    glUniform1i(glGetUniformLocation(program, uniformNameIsShadowCaster), isShadowCaster);
    getOpenGLError("uniform light is shadow caster");

    glUniformMatrix4fv(glGetUniformLocation(program, uniformNameShadowProjViewMat), 1, GL_FALSE, glm::value_ptr(shadowProjViewMat));
    getOpenGLError("uniform light shadow proj view mat");

    depthFrameBuffer.bindDepthTexture(MAP_TEXTURE_UNIT_DEPTH_MAP+lightIndex);
    getOpenGLError("uniform light shadow map binding");
    glUniform1i(glGetUniformLocation(program, uniformNameShadowMap), MAP_TEXTURE_UNIT_DEPTH_MAP);
    getOpenGLError("uniform light shadow map");
}

Light * Light::lightGetConstantCaster(
    glm::vec3 pos, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor
) {
    Light * l = new Light();
    l->lightPos = pos;
    l->lightAmbientColor = ambientColor;
    l->lightDiffuseColor = diffuseColor;
    l->lightSpecularColor = specularColor;

    return l;
}

void Light::updateFromLightDepthUniforms() {
    GLuint program = Program::getInstanceShadowMap()->getProgram();
    glm::mat4 shadowViewMat = glm::lookAt(lightPos, lightLookAt, lightCamUp);
    glm::mat4 shadowProjMat = glm::perspective(glm::radians(lightFovAngleDegree), 1.0f, lightCamNear, lightCamFar);
    shadowProjViewMat = shadowProjMat*shadowViewMat;
    glUniformMatrix4fv(glGetUniformLocation(program, "lightProjViewMat"), 1, GL_FALSE, glm::value_ptr(shadowProjViewMat));
}

void Light::bindDepthFrameBuffer() {
    depthFrameBuffer.bindFrameBuffer();
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}
