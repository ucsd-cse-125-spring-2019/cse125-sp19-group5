#pragma once

#include <unordered_map>
#include <string>
#include "Renderer/Texture2d.h"
#include "Renderer/ModelData.h"

using std::string;
using std::unordered_map;

namespace Assets
{
	extern unordered_map<string, Texture2d*> textures;
	extern unordered_map<string, ModelData*> models;

	ModelData *getModelData(const string &path);
	Texture2d *getTexture2d(const string &path);
	void cleanUp();
};

