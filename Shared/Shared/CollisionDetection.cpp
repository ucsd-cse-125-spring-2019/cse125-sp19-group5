#include "CollisionDetection.h"

bool CollisionDetection::collisionCheck(BoundingBox * bb1, BoundingBox * bb2) {
	// TODO
	if (bb1 == bb2) {
		return false;
	}
	return false;
}

bool CollisionDetection::collisionCheck(BoundingSphere * bs1, BoundingSphere * bs2) {
	if (bs1 == bs2) {
		return false;
	}
	return glm::distance(bs1->getPosition(), bs2->getPosition()) <= (bs1->getRadius() + bs2->getRadius());
}

bool CollisionDetection::collisionCheck(BoundingBox * bb, BoundingSphere * bs) {
	// TODO
	return false;
}

bool CollisionDetection::collisionCheck(BoundingSphere * bs, BoundingBox * bb) {
	return collisionCheck(bb, bs);
}