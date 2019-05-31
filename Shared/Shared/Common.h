#pragma once

// Commonly used imports and using statements.

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <tuple>
#include <string>
#include <iostream>
#include <exception>

constexpr auto MAX_GAME_OBJS = 256;

using std::cout;
using std::vector;
using std::tuple;
using std::string;
using std::cout;
using std::endl;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

void static printVec3(vec3 vec) {
	cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << endl;
}