#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.h"
#include "shader.h"
#include "camera.h"

#include <cmath>

class Objects {
public:
	Objects(const glm::vec3& position, const glm::vec3& top, const glm::vec3& bottom, const Model& model, const Shader& shader, bool is_destroyed = true);
	
	virtual ~Objects() {};

	void Draw(const glm::mat4& view, const glm::mat4& projection) const;

	bool is_collision(const Objects& object) const;
	
	bool is_destroyed;
	int lives;
	glm::vec3 position;

protected:
	float size_x;
	float size_y;
	float size_z;
	
	Model model_obj;
	Shader shader;
};

Objects::Objects(const glm::vec3& position, const glm::vec3& top, const glm::vec3& bottom, const Model& model, const Shader& shader, bool is_destroyed)
	: position(position), is_destroyed(is_destroyed), model_obj(model), shader(shader), lives(1)
{
	size_x = std::abs(top.x - bottom.x) / 2;
	size_y = std::abs(top.y - bottom.y) / 2;
	size_z = std::abs(top.z - bottom.z) / 2;
}

void Objects::Draw(const glm::mat4& view, const glm::mat4& projection) const {
	shader.use();
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	shader.setMat4("model", model);
	model_obj.Draw(shader);
}

void player_collision(const std::vector<Objects>& objects, Objects& player) {
	for (auto it_obj = objects.begin(); it_obj != objects.end(); ++it_obj) {
		if (player.is_collision(*it_obj)) {
			--player.lives;
		}
	}
}

bool Objects::is_collision(const Objects& object) const {
	bool x = position.x + size_x > object.position.x - object.size_x &&
		position.x - size_x < object.position.x + object.size_x;
	bool y = position.y + size_y > object.position.y - object.size_y &&
		position.y - size_y < object.position.y + object.size_y;
	bool z = position.z + size_z > object.position.z - object.size_z &&
		position.z - size_z < object.position.z + object.size_z;
	return x && y && z;
}