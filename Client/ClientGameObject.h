#pragma once

#include <Shared/GameObject.h>
#include "Renderer/Shader.h"
#include "Renderer/Model.h"
#include "Renderer/Camera.h"
#include "Renderer/Material.h"
#include "Renderer/DrawPass.h"
#include "Sound/Sound.h"
#include <memory>
#include <unordered_map>

class ClientGameObject {
protected:
	Model *model = nullptr;
	Material *material = nullptr;
	std::unique_ptr<GameObject> gameObject;
	std::unordered_map<string, Sound*> sounds;
	void updateAnimation(float time);
	void updateSound();

public:
	ClientGameObject(std::unique_ptr<GameObject> gameObject);
	virtual void draw(Shader &shader, const Camera *camera, DrawPass pass) const;

	void setModel(const std::string &newModel);
	void setMaterial(const std::string &newMaterial);
	void setAnimation(int id = -1, bool restart = true);
	void playSound(const string &sound, float volume = 1.0f, bool loop = false);
	void stopSound(const string &sound);
	void update(float time);
	GameObject *getGameObject() const;
	Model *getModel() const;
	Material *getMaterial() const;

	virtual ~ClientGameObject();
};
