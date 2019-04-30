#pragma once

#include <Shared/GameObject.h>
#include "Renderer/Shader.h"
#include "Renderer/Model.h"
#include "Renderer/Camera.h"

class ClientGameObject : public GameObject {
	protected:
	Model *model = nullptr;

	public:
	using GameObject::GameObject;

	virtual void draw(Shader &shader, const Camera *camera) const;

	void setModel(const std::string &newModel);
	void setAnimation(int id = -1, bool restart = true);
	void updateAnimation(float time);
	Model *getModel() const;

	virtual ~ClientGameObject();
};
