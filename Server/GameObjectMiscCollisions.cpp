#include <Shared/Bomb.h>
#include <Shared/PhysicsEngine.h>
void Bomb::updateOnServerTick() {
	if (hit) {
		setBoundingShape(new BoundingSphere(getPosition(), lifespan / 10.0f));
		setModel("Models/unit_sphere.obj");
		setMaterial("Materials/ball.json");
		setScale(vec3(getBoundingSphere()->getRadius()));
		lifespan -= 5;
		//sound of the grendae exploding
		this->playSound("Sounds/grenade_boom.wav", 1.0f, false);
	}
	else {
		move(getVelocity());
		setVelocity(PhysicsEngine::applyGravity(getVelocity(), PhysicsEngine::getGravity() * 5.0f));
	}
}