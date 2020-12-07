#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "objects.h"


class Player : public Objects {
public:
	Player(const glm::vec3& top, const glm::vec3& bottom, const Model& model, const Shader& shader);

	void Draw(const glm::mat4& view, const glm::mat4& projection, const Camera& camera);
}; 

Player::Player(const glm::vec3& top, const glm::vec3& bottom, const Model& model, const Shader& shader)
	: Objects(glm::vec3(0.0f), top, bottom, model, shader, false)
{

}

void Player::Draw(const glm::mat4& view, const glm::mat4& projection, const Camera& camera) {
	shader.use();
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	glm::mat4 model = glm::mat4(1.0f);
	position = camera.Position + (camera.Front * glm::vec3(2.0f, 2.0f, 2.0f));
	model = glm::translate(model, position);
	glm::vec3 result_vec = glm::normalize(glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f)));
	model = glm::rotate(model, glm::radians(camera.Pitch), result_vec);
	model = glm::rotate(model, glm::radians(-camera.Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	shader.setMat4("model", model);
	model_obj.Draw(shader);
}