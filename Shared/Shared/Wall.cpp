#include "Wall.h"

Wall::Wall(vec3 position, vec3 velocity, int id, int radius) : GameObject(position, velocity, id, radius) {

}

int Wall::getGameObjectType() const {
	return WALL_TYPE;
}