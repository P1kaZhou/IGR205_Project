#version 420 core

out vec4 color;
//layout(location = 0) out vec4 color;

// Fragment
in FRAG {
    vec3 pos;
    vec3 normal;
    vec2 texCoord;
    vec3 fcolor;
} frag;

// Camera
uniform vec3 cameraPos;

// Material
uniform vec3 basicColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform uint shininess;
layout(binding=0) uniform sampler2D diffuseMap;
layout(binding=1) uniform sampler2D specularMap;
layout(binding=2) uniform sampler2D shininessMap;
layout(binding=3) uniform sampler2D normalMap;

// Lights
struct Light {
	vec3 lightPos;

	// phong component colors
	vec3 lightDiffuseColor;
	vec3 lightSpecularColor;
	vec3 lightAmbientColor;

	// attenuation constants
	float lightAtteConst;
	float lightAtteLin;
	float lightAtteQuad;

	// Spot light
	float lightCamNear;
    float lightCamFar;
    vec3 lightCamUp;
    float lightFovAngleDegree;
    vec3 lightLookAt;

	bool isSpotLight;
	bool isShadowCaster;

	sampler2D shadowMap;
	mat4 shadowProjViewMat;
};
uniform Light lights[10];
uniform uint lightCount;

vec3 computeColor(
	in Light l, in vec3 fragDiffuseColor, in vec3 fragSpecularColor, in float fragShininess, in vec3 fragNormal
) {
	vec3 lightDirection = normalize(l.lightPos-frag.pos);
    vec3 cameraDirection = normalize(cameraPos-frag.pos);
	float fragDistance = distance(frag.pos, l.lightPos);

	// Diffuse light strengh
	float diff = max(dot(normalize(frag.normal), lightDirection), 0.0);

	// Specular light strengh
	vec3 lightDirectionReflect = -normalize( reflect(lightDirection, normalize(frag.normal)) );
    float spec = max(dot(cameraDirection, lightDirectionReflect), 0.0);
    spec = pow(spec, shininess);

	// Final diffuse and specular colors
	vec3 diffuse = diff * l.lightDiffuseColor * fragDiffuseColor;
    vec3 specular = spec * l.lightSpecularColor * fragSpecularColor;
	vec3 ambient = l.lightAmbientColor * fragDiffuseColor;

	// Attenuation
	float attenuation = 1.0 / (l.lightAtteQuad*fragDistance*fragDistance + l.lightAtteLin*fragDistance + l.lightAtteConst);

	// spot light attenuation
	float spotLightAttenuation = 1.0;
	if(l.isSpotLight) {
		float spotCoef = max(dot(
			normalize(l.lightLookAt-l.lightPos),
			normalize(frag.pos-l.lightPos)
		), 0);
		if(spotCoef < cos(l.lightFovAngleDegree)) {
			spotLightAttenuation = 0.1;
		}
	}

	return (diffuse + specular + ambient) * attenuation  * spotLightAttenuation;
}

in float ttt;

void main() {
	vec3 c = vec3(0.0);
	vec3 fragDiffuseColor = texture(diffuseMap, frag.texCoord).rgb + diffuseColor;
	vec3 fragSpecularColor = texture(specularMap, frag.texCoord).rgb + specularColor;
	float fragShininess = (texture(shininessMap, frag.texCoord).r + shininess) / 2.0;
	vec3 fragNormal = texture(normalMap, frag.texCoord).xyz + frag.normal;

	for(int i=0; i<int(lightCount); i++) {
		// Shadow
		vec4 fromLightSpaceFragPosH = (lights[i].shadowProjViewMat*vec4(frag.pos, 1.0));
		vec3 fromLightSpaceFragPos = fromLightSpaceFragPosH.xyz / fromLightSpaceFragPosH.w;
		vec2 shadowMapTexCoord = fromLightSpaceFragPos.xy;
		shadowMapTexCoord = (shadowMapTexCoord+1)/2.0;

		// Pourcentage closer filtering :
		// sample around the fragment.
		float inShadowFactor = 1.0;
		vec2 texelSize = 1.0 / textureSize(lights[i].shadowMap, 0); // Dimension of level 0 of the texture.
		for(int xx=-1; xx<=1; ++xx) {
			for(int yy=-1; yy<=1; ++yy) {
				float pcfDepth = texture(lights[i].shadowMap, shadowMapTexCoord + vec2(xx, yy)*texelSize).x;
				if(pcfDepth < fromLightSpaceFragPos.z-0.005) {
					// Fragment in shadow
					inShadowFactor -= 1.0/12.0;
				}
			}
		}

		c += inShadowFactor * computeColor(
			lights[i], fragDiffuseColor, fragSpecularColor, fragShininess, fragNormal
		);
	}

	c += basicColor;
	c += frag.fcolor;
	color = vec4(c, 1.0);

	if(ttt>0) {
		color = vec4(ttt, 0.0, 0.0, 1.0);
	}
}
