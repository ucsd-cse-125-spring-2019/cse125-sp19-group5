#include "MapLoader.h"
#include <Shared/Ball.h>
#include <Shared/Goal.h>
#include <Shared/Wall.h>
#include <fstream>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

MapLoader::MapLoader(GameEngine * gameEngine) {
	this->gameEngine = gameEngine;
}

void MapLoader::loadMap(string mapFile) {
	std::ifstream f(mapFile);

	if (!f.is_open()) {
		std::cerr << "Failed to open map file (" << mapFile << ")" << std::endl;
		return;
	}

	std::string source(
		(std::istreambuf_iterator<char>(f)),
		(std::istreambuf_iterator<char>())
	);

	std::string parseErr = "";
	json11::Json mapJson = json11::Json::parse(source, parseErr);

	if (parseErr != "") {
		std::cerr << "Failed to parse map file (" << mapFile << ")" << std::endl
			<< parseErr << std::endl;
	}

	auto walls = loadBoxGameObjects<Wall>(mapJson, "walls");
	for (auto wall : walls) {
		wall->setCastShadow(false);
	}

	loadSphereGameObjects<Ball>(mapJson, "balls");

	vector<Goal *> goals = loadBoxGameObjects<Goal>(mapJson, "goals");
	if (!mapJson["goals"].is_null() && mapJson["goals"].is_array()) {
		json11::Json::array goalsArr = mapJson["goals"].array_items();
		for (int i = 0; i < goalsArr.size(); i++) {
			goals[i]->setTeam(goalsArr[i]["team"].int_value());

			tuple<float, float> xRange = toTuple(goalsArr[i], "xRange");
			tuple<float, float> yRange = toTuple(goalsArr[i], "yRange");
			tuple<float, float> zRange = toTuple(goalsArr[i], "zRange");

			goals[i]->setBallSpawnRange(xRange, yRange, zRange);
		}
	}	

	loadSpawns(mapJson);
}

void MapLoader::loadSpawns(const json11::Json &json) {
	gameEngine->spawns.clear();
	auto spawns = json["spawns"];
	if (!spawns.is_array()) { return; }

	for (auto &spawnData : spawns.array_items()) {
		auto team = spawnData["team"].int_value();
		auto position = toVec3(spawnData, "position");
		gameEngine->spawns.push_back(std::make_pair(team, position));
	}
}

void MapLoader::loadGameObjectDefaults(json11::Json gameObjJson, GameObject * gameObject) {
	if (!gameObjJson["position"].is_null()) {
		gameObject->setPosition(toVec3(gameObjJson, "position"));
	}
	if (!gameObjJson["velocity"].is_null()) {
		gameObject->setVelocity(toVec3(gameObjJson, "velocity"));
	}
	if (!gameObjJson["scale"].is_null()) {
		gameObject->setScale(toVec3(gameObjJson, "scale"));
	}
	if (!gameObjJson["model"].is_null() && gameObjJson["model"].is_string()) {
		gameObject->setModel(gameObjJson["model"].string_value());
	}
	if (!gameObjJson["material"].is_null() && gameObjJson["material"].is_string()) {
		gameObject->setMaterial(gameObjJson["material"].string_value());
	}
	if (!gameObjJson["animation"].is_null() && gameObjJson["animation"].is_number()) {
		gameObject->setAnimation(gameObjJson["animation"].int_value());
	}
}

template<class T>
vector<T *> MapLoader::loadBoxGameObjects(json11::Json mapJson, string key) {
	vector< T*> ret;
	if (!mapJson[key].is_null() && mapJson[key].is_array()) {
		for (json11::Json objData : mapJson[key].array_items()) {
			T * gameObject = gameEngine->addGameObject<T>();
			ret.push_back(gameObject);

			vec3 position = vec3(0);
			if (!objData["position"].is_null() && objData["position"].is_array()) {
				position = toVec3(objData, "position");
			}

			vec3 direction = vec3(0);
			if (!objData["direction"].is_null()) {
				direction = toVec3(objData, "direction");
			}

			double width = 0, length = 0, height = 0;
			if (!objData["dimensions"].is_null() && objData["dimensions"].is_array()) {
				json11::Json::array arrayVals = objData["dimensions"].array_items();
				width = arrayVals[0].number_value();
				height = arrayVals[1].number_value();
				length = arrayVals[2].number_value();
			}

			gameObject->setBoundingShape(new BoundingBox(position, direction, width, height, length));
			loadGameObjectDefaults(objData, gameObject);
		}
	}
	return ret;
}

template<class T>
vector<T *> MapLoader::loadSphereGameObjects(json11::Json mapJson, string key) {
	vector<T *> ret;
	if (!mapJson[key].is_null() && mapJson[key].is_array()) {
		for (json11::Json objData : mapJson[key].array_items()) {
			T * gameObject = gameEngine->addGameObject<T>();
			ret.push_back(gameObject);

			vec3 position = vec3(0);
			if (!objData["position"].is_null() && objData["position"].is_array()) {
				position = toVec3(objData, "position");
			}

			float radius = 0.0f;
			if (!objData["radius"].is_null() && objData["radius"].is_number()) {
				radius = objData["radius"].number_value();
			}

			gameObject->setBoundingShape(new BoundingSphere(position, radius));
			loadGameObjectDefaults(objData, gameObject);
		}
	}
	return ret;
}

vec3 MapLoader::toVec3(json11::Json json, string key) {
	if (json[key].is_array()) {
		json11::Json::array arrayVals = json[key].array_items();
		return vec3(arrayVals[0].number_value(), arrayVals[1].number_value(), arrayVals[2].number_value());
	}
	else {
		return vec3(json[key].number_value());
	}
}

tuple<float, float> MapLoader::toTuple(json11::Json json, string key) {
	json11::Json::array arr = json[key].array_items();
	return std::make_tuple((float)arr[0].number_value(), (float)arr[1].number_value());
}