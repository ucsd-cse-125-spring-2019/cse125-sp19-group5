#pragma once

#include <Shared/GameObject.h>
#include "Renderer/Shader.h"
#include "Renderer/Model.h"
#include "Renderer/Camera.h"
#include "Renderer/Material.h"
#include "Renderer/DrawPass.h"
#include "Sound/Sound.h"
#include <memory>

class ClientGameObject {
protected:
	Model *model = nullptr;
	Material *material = nullptr;
	std::unique_ptr<GameObject> gameObject;

public:
	ClientGameObject(std::unique_ptr<GameObject> gameObject);
	virtual void draw(Shader &shader, const Camera *camera, DrawPass pass) const;

	void setModel(const std::string &newModel);
	void setMaterial(const std::string &newMaterial);
	void setAnimation(int id = -1, bool restart = true);
	void updateAnimation(float time);
	GameObject *getGameObject() const;
	Model *getModel() const;
	Material *getMaterial() const;

	virtual ~ClientGameObject();
};
