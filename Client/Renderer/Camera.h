#pragma once

#include <GL/glew.h>
#include <Shared/Common.h>

class Camera {
	private:
	mat4 view;       // Transformation for world -> camera
	mat4 projection; // Transformation for camera -> NDC

	// Basis for camera coordinates.
	vec3 up;
	vec3 forward;
	vec3 right;

	// Location of the camera
	vec3 position;
	vec3 angles;

	// Other misc. camera properties
	GLfloat fov; // This is in degrees, not radians
	GLfloat aspect;
	GLfloat zNear;
	GLfloat zFar;

	// Updates the up, forward, and right vectors to be the result of
	// rotating the normal 3D basis vectors by newAngles.
	// newAngles = (pitch, yaw, roll) - in radians, Euler angles
	void updateEyeCoords(const vec3 &newAngles);

	public:
	Camera(
		const vec3 &position, const vec3 &angles,
		GLfloat fov,
		GLfloat aspect,
		GLfloat zNear = 0.001f, GLfloat zFar = 1000.0f
	);

	void setPosition(const vec3 &newPos);
	void setEyeAngles(const vec3 &newAngles);
	void setFov(GLfloat newFov);
	void setAspect(GLfloat newAspect);

	const vec3 &getPosition() const;
	const vec3 &getUp() const;
	const vec3 &getForward() const;
	const vec3 &getRight() const;
	const vec3 &getAngles() const;

	mat4 getMatrix() const;
	const mat4 &getViewMatrix() const;
	const mat4 &getProjectionMatrix() const;
};

