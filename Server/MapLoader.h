#pragma once
#include "GameEngine.h"
#include <Shared/Util/json11.hpp>

class MapLoader {
public:
	MapLoader(GameEngine * gameEngine);
	void loadMap(string mapFile);
	void loadGameObjectDefaults(json11::Json gameObjJson, GameObject * gameObject);
	template<class T>
	vector<T *> loadBoxGameObjects(json11::Json mapJson, string key);
	template<class T>
	vector<T *> loadSphereGameObjects(json11::Json mapJson, string key);

	vec3 toVec3(json11::Json json, string key);
	tuple<float, float> toTuple(json11::Json json, string key);

private:
	GameEngine * gameEngine;
};