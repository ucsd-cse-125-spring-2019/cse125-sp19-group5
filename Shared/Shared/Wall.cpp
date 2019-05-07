#include "Wall.h"
#include <glm/gtx/string_cast.hpp>

Wall::Wall(vec3 position, vec3 velocity, int id, float length, float width, float height) : GameObject(position, velocity, id, 0.0f) {
	this->length = length;
	this->width = width;
	this->height = height;

	vec3 widthDir = glm::normalize(velocity);
	vec3 lengthDir = glm::normalize(glm::cross(widthDir, vec3(0, 1, 0)));
	vec3 heightDir = glm::normalize(glm::cross(widthDir, vec3(0, 0, 1)));
	if (heightDir.y < 0) {
		heightDir.y = -1 * heightDir.y;
	}

	std::cout << "widthDir: " << glm::to_string(widthDir) << std::endl;
	std::cout << "lengthDir: " << glm::to_string(lengthDir) << std::endl;
	std::cout << "heightDir: " << glm::to_string(heightDir) << std::endl;

	// bottom face
	this->facePlanes.push_back(Plane(
		position, 
		position + lengthDir,
		position + widthDir
	));

	// top face
	this->facePlanes.push_back(Plane(
		position + (heightDir * height),
		position + (heightDir * height) + lengthDir,
		position + (heightDir * height) + widthDir
	));

	// side1 -- width on bottom
	this->facePlanes.push_back(Plane(
		position + (lengthDir * (length / 2)),
		position + (lengthDir * (length / 2)) + heightDir,
		position + (lengthDir * (length / 2)) + widthDir
	));

	// side 2 -- width on bottom but on the opposite face
	this->facePlanes.push_back(Plane(
		position - (lengthDir * (length / 2)),
		position - (lengthDir * (length / 2)) + heightDir,
		position - (lengthDir * (length / 2)) + widthDir
	));

	// side 3 -- length on bottom
	this->facePlanes.push_back(Plane(
		position + (widthDir * (width / 2)),
		position + (widthDir * (width / 2)) + heightDir,
		position + (widthDir * (width / 2)) + lengthDir
	));

	// side 4 -- length on bottom but on the opposite face
	this->facePlanes.push_back(Plane(
		position - (widthDir * (width / 2)),
		position - (widthDir * (width / 2)) + heightDir,
		position - (widthDir * (width / 2)) + lengthDir
	));

	vec3 centerPoint = position + (heightDir * (height / 2));
	for (Plane p : this->facePlanes) {
		if (p.pointDistance(centerPoint)) {
			p.invertNormal();
		}
	}
}

int Wall::getGameObjectType() const {
	return WALL_TYPE;
}