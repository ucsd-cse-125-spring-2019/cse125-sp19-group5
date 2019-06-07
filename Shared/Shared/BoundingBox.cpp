#include "BoundingBox.h"
#include "CollisionDetection.h"
#include <glm/gtx/string_cast.hpp>

BoundingBox::BoundingBox(vec3 position, vec3 direction, float width, float height, float length) : BoundingShape(position) {
	this->length = length;
	this->width = width;
	this->height = height;
	this->direction = direction;

	vec3 widthDir = glm::normalize(direction);
	vec3 lengthDir = glm::normalize(glm::cross(widthDir, vec3(0, 1, 0)));
	vec3 heightDir = glm::normalize(glm::cross(widthDir, lengthDir));
	if (heightDir.y < 0) {
		heightDir.y = -1 * heightDir.y;
	}
	vec3 centerPoint = position + (heightDir * (height / 2));

	// bottom face
	this->bottom = new Plane(
		position,
		position + lengthDir,
		position + widthDir
	);
	if (bottom->pointDistance(centerPoint) > 0) {
		bottom->invertNormal();
	}

	// top face
	this->top = new Plane(
		position + (heightDir * height),
		position + (heightDir * height) + lengthDir,
		position + (heightDir * height) + widthDir
	);
	if (top->pointDistance(centerPoint) > 0) {
		top->invertNormal();
	}


	// front -- width on bottom
	this->front = new Plane(
		position + (lengthDir * (length / 2)),
		position + (lengthDir * (length / 2)) + heightDir,
		position + (lengthDir * (length / 2)) + widthDir
	);
	if (front->pointDistance(centerPoint) > 0) {
		front->invertNormal();
	}

	// back -- width on bottom but on the opposite face
	this->back = new Plane(
		position - (lengthDir * (length / 2)),
		position - (lengthDir * (length / 2)) + heightDir,
		position - (lengthDir * (length / 2)) + widthDir
	);
	if (back->pointDistance(centerPoint) > 0) {
		back->invertNormal();
	}

	// left -- length on bottom
	this->left = new Plane(
		position + (widthDir * (width / 2)),
		position + (widthDir * (width / 2)) + heightDir,
		position + (widthDir * (width / 2)) + lengthDir
	);
	if (left->pointDistance(centerPoint) > 0) {
		left->invertNormal();
	}

	// right -- length on bottom but on the opposite face
	this->right = new Plane(
		position - (widthDir * (width / 2)),
		position - (widthDir * (width / 2)) + heightDir,
		position - (widthDir * (width / 2)) + lengthDir
	);
	if (right->pointDistance(centerPoint) > 0) {
		right->invertNormal();
	}

	this->facePlanes.push_back(bottom);
	this->facePlanes.push_back(top);
	this->facePlanes.push_back(front);
	this->facePlanes.push_back(back);
	this->facePlanes.push_back(left);
	this->facePlanes.push_back(right);
}

vector<Plane *> & BoundingBox::getFacePlanes() {
	return this->facePlanes;
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