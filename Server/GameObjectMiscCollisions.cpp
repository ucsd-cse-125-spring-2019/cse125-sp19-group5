#include <Shared/Bomb.h>
#include <Shared/PhysicsEngine.h>
void Bomb::updateOnServerTick() {
	if (hit) {
		setBoundingShape(new BoundingSphere(getPosition(), lifespan / 10.0f));
		setScale(vec3(getBoundingSphere()->getRadius()));
		lifespan -= 5;
	}
	else {
		move(getVelocity());
		setVelocity(PhysicsEngine::applyGravity(getVelocity(), PhysicsEngine::getGravity() * 5.0f));
	}
}