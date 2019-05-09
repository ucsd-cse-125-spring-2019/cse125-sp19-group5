#pragma once

#include <Shared/GameObject.h>
#include "Renderer/Shader.h"
#include "Renderer/Model.h"
#include "Renderer/Camera.h"
#include "Sound/Sound.h"
#include <memory>

class ClientGameObject {
protected:
	Model *model = nullptr;
	std::unique_ptr<GameObject> gameObject;

public:
	ClientGameObject(std::unique_ptr<GameObject> gameObject);
	virtual void draw(Shader &shader, const Camera *camera) const;

	void setModel(const std::string &newModel);
	void setAnimation(int id = -1, bool restart = true);
	void updateAnimation(float time);
	GameObject *getGameObject() const;
	Model *getModel() const;

	virtual ~ClientGameObject();
};
