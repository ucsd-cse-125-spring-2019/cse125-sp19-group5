#pragma once

#include <Shared/Common.h>

class PhysicsEngine
{
public:
	PhysicsEngine();
	~PhysicsEngine();

	float getGravity() const	{ return gravity; }
	float getFloorY() const		{ return floorY; }

	void setGravity(const float gravity)	{ this->gravity = gravity; }
	void setFloorY(const float floorY)		{ this->floorY = floorY; }

private:
	float gravity = -5.0f;
	float floorY = 0.0f;
};

