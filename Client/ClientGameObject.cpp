#include "ClientGameObject.h"
#include <glm/gtx/transform.hpp>

ClientGameObject::ClientGameObject(std::unique_ptr<GameObject> gameObject)
: gameObject(std::move(gameObject)) { }

void ClientGameObject::draw(
	Shader &shader,
	const Camera *camera,
	DrawPass pass
) const {
	if (gameObject && model) {
		if (pass == DrawPass::LIGHTING && material) {
			material->bind(shader);
		}

		// TODO (bhang): add rotation support (quaternions or euler angles?)
		auto modelTransform = glm::translate(gameObject->getPosition())
			* glm::scale(gameObject->getScale());
		auto modelInvT = glm::transpose(glm::inverse(mat3(modelTransform)));

		shader.setUniform("model", modelTransform);
		shader.setUniform("modelInvT", modelInvT);
		shader.setUniform("mvp", camera->getMatrix() * modelTransform);

		model->draw(shader);
	}
}

void ClientGameObject::setModel(const std::string &newModel) {
	if (model) {
		delete model;
		model = nullptr;
	}

	if (newModel == "") { return; }

	model = new Model(newModel);
}

void ClientGameObject::setMaterial(const std::string &newMaterial) {
	if (material) {
		delete material;
		material = nullptr;
	}
	if (newMaterial == "") { return; }
	material = new Material(newMaterial);
}

void ClientGameObject::setAnimation(int id, bool restart) {
	if (model) {
		model->setAnimation(id, restart);
	}
}

void ClientGameObject::updateAnimation(float time) {
	if (model) {
		model->updateAnimation(time);
	}
}

Model *ClientGameObject::getModel() const {
	return model;
}

Material *ClientGameObject::getMaterial() const {
	return material;
}

GameObject *ClientGameObject::getGameObject() const {
	return gameObject.get();
}

ClientGameObject::~ClientGameObject() {
	if (model) {
		delete model;
		model = nullptr;
	}
}
