#version 330 core

// Lighting
const int LIGHTS_MAX = 10;
const vec3 diffuse = vec3(0.5f, 0.5f, 0.5f);
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
};

uniform DirectionalLight directionalLight[LIGHTS_MAX];
uniform PointLight pointLight[LIGHTS_MAX];
uniform int pointLightNum;
uniform int directionalLightNum;
uniform mat4 mvp;

uniform vec3 eyePos;

in vec3 fragPos;
in vec3 fragNormal;
out vec4 fragColor;

vec3 getPointLightIntensity(
	PointLight light,
	vec3 fragPos,
	vec3 normal,
	vec3 eyeDir
) {
	vec3 dir = normalize(light.position - fragPos);
	vec3 halfAng = normalize(dir + eyeDir);
	vec3 lambert = light.color * max(dot(normal, dir), 0.0f);
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

	return attenuation * (lambert + phong);
}

vec3 getDirectionalLightIntensity(
	DirectionalLight light,
	vec3 fragPos,
	vec3 normal,
	vec3 eyeDir
) {
	vec3 dir = light.direction;
	vec3 halfAng = normalize(dir + eyeDir);
	vec3 lambert = light.color * max(dot(normal, dir), 0.0f);
	vec3 phong = vec3(pow(
		max(dot(normal, halfAng), 0.0f),
		shininess
	));
	return lambert + phong;
}

void main() {
	vec3 normal = normalize(fragNormal);
	vec3 eyeDir = normalize(eyePos - fragPos);
	vec3 finalColor = vec3(0.0f);

	for (int i = 0; i < pointLightNum; i++) {
		finalColor += getPointLightIntensity(
			pointLight[i],
			fragPos,
			normal,
			eyeDir
		);
	}
	for (int i = 0; i < directionalLightNum; i++) {
		finalColor += getDirectionalLightIntensity(
			directionalLight[i],
			fragPos,
			normal,
			eyeDir
		);
	}

	fragColor = vec4(pow(finalColor, gammaCorr), 1.0f);
}
