#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

constexpr auto PI = glm::pi<float>();
constexpr auto HALF_PI = PI / 2.0f;
constexpr auto CIRCUMFERENCE = PI * 2.0f;

void Camera::updateEyeCoords(const vec3& newAngles) {
	angles = newAngles;

	// Limit ability to look up/down.
	angles.x = glm::clamp(angles.x, -HALF_PI + 0.0001f, HALF_PI - 0.0001f);

	// Mod to stop precision loss over time.
	angles.y = glm::mod(angles.y, CIRCUMFERENCE);
	angles.z = glm::mod(angles.z, CIRCUMFERENCE);

	up = vec3(0.0f, 1.0f, 0.0f);
	up = glm::rotate(up, angles.z, vec3(0.0f, 0.0f, 1.0f));

	// Use spherical coordinates to get forward direction.
	forward[0] = glm::cos(angles.x) * std::cos(angles.y);
	forward[1] = glm::sin(angles.x);
	forward[2] = glm::cos(angles.x) * std::sin(angles.y);

	right = glm::cross(forward, up);
}

Camera::Camera(
	const vec3 &position,
	const vec3 &angles,
	GLfloat fov,
	GLfloat aspect,
	GLfloat zNear, GLfloat zFar
): fov(fov), aspect(aspect), zNear(zNear), zFar(zFar) {
	updateEyeCoords(angles);
	setPosition(position);
	projection = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
}

void Camera::setPosition(const vec3 &newPos) {
	position = newPos;
	view = glm::lookAt(position, position + forward, up);
}

void Camera::setEyeAngles(const vec3 &newAngles) {
	updateEyeCoords(newAngles);
	view = glm::lookAt(position, position + forward, up);
}

void Camera::setFov(GLfloat newFov) {
	fov = newFov;
	projection = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
}

void Camera::setAspect(GLfloat newAspect) {
	aspect = newAspect;
	projection = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
}

const vec3 &Camera::getPosition() const {
	return position;
}

const vec3 &Camera::getUp() const {
	return up;
}

const vec3 &Camera::getForward() const {
	return forward;
}

const vec3 &Camera::getRight() const {
	return right;
}

const vec3 &Camera::getAngles() const {
	return angles;
}

float Camera::getAspect() const {
	return aspect;
}

mat4 Camera::getMatrix() const {
	return projection * view;
}

const mat4 &Camera::getViewMatrix() const {
	return view;
}

const mat4 &Camera::getProjectionMatrix() const {
	return projection;
}