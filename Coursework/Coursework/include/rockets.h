#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "objects.h"

class Rockets : public Objects {
public:
	Rockets(glm::vec3& position, const glm::vec3& top, const glm::vec3& bottom, const glm::vec3& font, float yawl, float pitch, bool is_destroyed = true);

	void Draw(const glm::mat4& view, const glm::mat4& projection, const Model& model_obj, const Shader& shader, float delta_time);

private:
	glm::vec3 font;
	double speed;
	float yawl;
	float pitch;
};

Rockets::Rockets(glm::vec3& position, const glm::vec3& top, const glm::vec3& bottom, const glm::vec3& font, float yawl, float pitch, bool is_destroyed)
	: Objects(position, top, bottom, Model(), Shader(), is_destroyed), font(font), yawl(yawl), pitch(pitch), speed(10.0f)
{

}

void Rockets::Draw(const glm::mat4& view, const glm::mat4& projection, const Model& model_obj, const Shader& shader, float delta_time) {
	shader.use();
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 new_pos = position + font * glm::vec3(speed * delta_time, speed * delta_time, speed * delta_time);
	model = glm::translate(model, new_pos);
	position = new_pos;
	glm::vec3 result_vec = glm::normalize(glm::cross(font, glm::vec3(0.0f, 1.0f, 0.0f)));
	model = glm::rotate(model, glm::radians(pitch), result_vec);
	model = glm::rotate(model, glm::radians(-yawl), glm::vec3(0.0f, 1.0f, 0.0f));
	
	shader.setMat4("model", model);
	model_obj.Draw(shader);
}