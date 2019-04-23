#version 330 core

// Lighting
const int LIGHTS_MAX = 10;
const vec3 specular = vec3(0.2f, 0.2f, 0.2f);
const float shininess = 32.0f;

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

uniform DirectionalLight directionalLight[LIGHTS_MAX];
uniform PointLight pointLight[LIGHTS_MAX];
uniform int pointLightNum;
uniform int directionalLightNum;
uniform sampler2D diffuseTex;
uniform sampler2D shadowMap;

uniform vec3 eyePos;

in vec4 lightSpacePos;
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;
out vec4 fragColor;

vec3 getPointLightIntensity(
	PointLight light,
	vec3 fragPos,
	vec3 normal,
	vec3 eyeDir,
	vec3 diffuse
) {
	vec3 dir = normalize(light.position - fragPos);
	vec3 halfAng = normalize(dir + eyeDir);
	vec3 lambert = diffuse * max(dot(normal, dir), 0.0f);
	vec3 phong = vec3(pow(
		max(dot(normal, halfAng), 0.0f),
		shininess
	));

	float dist = length(light.position - fragPos);
	float attenuation = 1.0f / (
		light.constant +
		(light.linear * dist) +
		(light.quadratic * dist * dist)
	);

	return attenuation * light.color * (lambert + phong);
}

vec3 getDirectionalLightIntensity(
	DirectionalLight light,
	vec3 fragPos,
	vec3 normal,
	vec3 eyeDir,
	vec3 diffuse,
	float intensity
) {
	vec3 ambient = light.ambient * diffuse;
	vec3 dir = light.direction;
	vec3 halfAng = normalize(dir + eyeDir);
	vec3 lambert = diffuse * max(dot(normal, dir), 0.0f);
	vec3 phong = vec3(pow(
		max(dot(normal, halfAng), 0.0f),
		shininess
	));
	return ambient + intensity * (light.color * (lambert + phong));
}

const float bias = 0.0005f;

float getShadowIntensity(vec4 lightSpacePos) {
	vec3 pos = lightSpacePos.xyz / lightSpacePos.w;
	pos = pos * 0.5 + 0.5;

	if (pos.z > 1.0f) {
		return 0.0f;
	}

	if (texture(shadowMap, pos.xy).r < pos.z) {
		return 0.1f;
	}
	return 1.0f;
}

void main() {
	vec3 normal = normalize(fragNormal);
	vec3 eyeDir = normalize(eyePos - fragPos);
	vec3 finalColor = vec3(0.0f);
	vec3 diffuse = vec3(texture(diffuseTex, fragTexCoords));
	float intensity = getShadowIntensity(lightSpacePos);

	for (int i = 0; i < pointLightNum; i++) {
		finalColor += intensity * getPointLightIntensity(
			pointLight[i],
			fragPos,
			normal,
			eyeDir,
			diffuse
		);
	}
	for (int i = 0; i < directionalLightNum; i++) {
		finalColor += getDirectionalLightIntensity(
			directionalLight[i],
			fragPos,
			normal,
			eyeDir,
			diffuse,
			intensity
		);
	}

	fragColor = vec4(pow(finalColor, gammaCorr), 1.0f);
}
