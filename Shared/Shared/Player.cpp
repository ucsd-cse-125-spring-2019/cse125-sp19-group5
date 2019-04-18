#include "Player.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Player::Player(vec3 position, vec3 velocity, string id, int radius) : GameObject(position, velocity, id, radius) {

}

void Player::onCollision(GameObject &gameObject) {
	std::cout << "Player: " + id + glm::to_string(position) + " collided with " + gameObject.getId() + glm::to_string(gameObject.getPosition()) << std::endl;
}