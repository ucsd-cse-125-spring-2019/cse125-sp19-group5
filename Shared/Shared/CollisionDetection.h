#pragma once
#include "BoundingBox.h"
#include "BoundingSphere.h"

class CollisionDetection {
public:
	static bool collisionCheck(BoundingBox * bb1, BoundingBox * bb2);
	static bool collisionCheck(BoundingSphere * bs1, BoundingSphere * bs2);
	static bool collisionCheck(BoundingBox * bb, BoundingSphere * bs);
	static bool collisionCheck(BoundingSphere * bs, BoundingBox * bb);
};