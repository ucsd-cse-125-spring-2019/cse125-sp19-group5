#include "Wall.h"

Wall::Wall(vec3 position, vec3 velocity, int id, int radius) : GameObject(position, velocity, id, radius) {

}

GAMEOBJECT_TYPES Wall::getGameObjectType() const {
	return WALL_TYPE;
}