#include "BoundingBox.h"
#include "CollisionDetection.h"

BoundingBox::BoundingBox(vec3 position, vec3 direction, float length, float width, float height) : BoundingShape(position) {
	this->length = length;
	this->width = width;
	this->height = height;

	vec3 widthDir = glm::normalize(direction);
	vec3 lengthDir = glm::normalize(glm::cross(widthDir, vec3(0, 1, 0)));
	vec3 heightDir = glm::normalize(glm::cross(widthDir, vec3(0, 0, 1)));
	if (heightDir.y < 0) {
		heightDir.y = -1 * heightDir.y;
	}

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

bool BoundingBox::collideVisit(BoundingShape * boundingShape) {
	return boundingShape->collidesWith(this);
}
bool BoundingBox::collidesWith(BoundingBox * boundingBox) {
	return CollisionDetection::collisionCheck(this, boundingBox);
}
bool BoundingBox::collidesWith(BoundingSphere * boundingSphere) {
	return CollisionDetection::collisionCheck(this, boundingSphere);
}