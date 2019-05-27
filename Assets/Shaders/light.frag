#version 330 core

// Lighting
const int LIGHTS_MAX = 10;

const int SHADOW_NUM_CASCADES = 4;

const float gamma = 2.2;
const vec3 gammaCorr = vec3(1.0 / gamma);

struct PointLight {
	float constant;
	float linear;
	float quadratic;
	vec3 position;
	vec3 color;
};

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	vec3 ambient;
};

struct Material {
	int hasNormalMap;
	float shininess;
	sampler2D diffuseTex;
	sampler2D normalTex;
	sampler2D specularTex;
	sampler2D emissionTex;
};

uniform Material material;
uniform DirectionalLight directionalLight[LIGHTS_MAX];
uniform PointLight pointLight[LIGHTS_MAX];
uniform sampler2D shadowMap[SHADOW_NUM_CASCADES];
uniform int pointLightNum;
uniform int directionalLightNum;
uniform float cascadeZCutoffs[SHADOW_NUM_CASCADES];

uniform vec3 eyePos;
uniform mat4 viewMat;

in float clipSpaceZ;
in mat3 tbn;
in vec4 lightSpacePos[SHADOW_NUM_CASCADES];
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;
out vec4 fragColor;

vec3 getPointLightIntensity(
	PointLight light,
	vec3 fragPos,
	vec3 normal,
	vec3 eyeDir,
	vec3 diffuse, vec3 specular, vec3 emission, float shininess
) {
	vec3 dir = normalize(light.position - fragPos);
	vec3 halfAng = normalize(dir + eyeDir);
	vec3 lambert = diffuse * max(dot(normal, dir), 0.0f);
	vec3 phong = specular * pow(
		max(dot(normal, halfAng), 0.0f),
		shininess
	);

	float dist = length(light.position - fragPos);
	float attenuation = 1.0f / (
		light.constant +
		(light.linear * dist) +
		(light.quadratic * dist * dist)
	);

	return emission + (attenuation * light.color * (lambert + phong));
}

vec3 getDirectionalLightIntensity(
	DirectionalLight light,
	vec3 fragPos,
	vec3 normal,
	vec3 eyeDir,
	vec3 diffuse, vec3 specular, vec3 emission, float shininess,
	float intensity
) {
	vec3 dir = light.direction;
	float diff = max(dot(normal, dir), 0.0);
	vec3 ambient = light.ambient * diffuse;
	vec3 halfAng = normalize(dir + eyeDir);
	vec3 lambert = diffuse * diff;
	vec3 phong = diff * specular * pow(
		max(dot(normal, halfAng), 0.0f),
		shininess
	);
	return ambient + emission + intensity * (light.color * (lambert + phong));
}

const float SHADOW_BIAS = 0.0002f;

float getShadowIntensity(int cascade, vec4 lightSpacePos) {
	vec3 pos = lightSpacePos.xyz;
	pos = pos * 0.5 + 0.5;

	if (pos.z < 0.0 || pos.z > 1.0) {
		return 1.0;
	}

	vec2 moments = texture(shadowMap[cascade], pos.xy).rg;

	if (pos.z <= moments.x) {
		return 1.0f;
	}

	float variance = max(
		moments.y - (moments.x * moments.x),
		SHADOW_BIAS
	);
	float d = pos.z - moments.x;
	return mix(0.2f, 1.0f, variance / (variance + d*d));
}

void main() {
	vec3 normal = normalize(fragNormal);
	vec3 eyeDir = normalize(eyePos - fragPos);
	vec3 finalColor = vec3(0.0f);
	float intensity = 1.0;
	int cascade;	
	float zPos = gl_FragCoord.z / gl_FragCoord.w;
	for (cascade = 0; cascade < SHADOW_NUM_CASCADES; cascade++) {
		if (zPos < cascadeZCutoffs[cascade]) {
			intensity = getShadowIntensity(cascade, lightSpacePos[cascade]);
			break;
		}
	}

	float shininess = material.shininess;
	vec3 diffuse = texture2D(material.diffuseTex, fragTexCoords).rgb;
	vec3 normalMap = texture2D(material.normalTex, fragTexCoords).rgb;
	vec3 specular = texture2D(material.specularTex, fragTexCoords).rgb;
	vec3 emission = texture2D(material.emissionTex, fragTexCoords).rgb;

	if (material.hasNormalMap > 0) {
		normal = texture2D(material.normalTex, fragTexCoords).rgb;
		normal = normalize(tbn * normalize(normal * 2.0 - 1.0));
	}

	for (int i = 0; i < pointLightNum; i++) {
		finalColor += intensity * getPointLightIntensity(
			pointLight[i],
			fragPos,
			normal,
			eyeDir,
			diffuse, specular, emission, shininess
		);
	}
	for (int i = 0; i < directionalLightNum; i++) {
		finalColor += getDirectionalLightIntensity(
			directionalLight[i],
			fragPos,
			normal,
			eyeDir,
			diffuse, specular, emission, shininess,
			intensity
		);
	}

	fragColor = vec4(pow(finalColor, gammaCorr), 1.0f);
}
