#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.h"
#include "shader.h"
#include "camera.h"
#include "point.h"

#include <cmath>
#include <random>

#define M_PI 3.14159265358979323846

class Objects {
public:
	Objects(const glm::vec3& position, const glm::vec3& top, const glm::vec3& bottom, const Model& model, const Shader& shader, bool is_destroyed = true);
	
	virtual ~Objects() {};

	virtual void Draw(const glm::mat4& view, const glm::mat4& projection, float delta_time);

	bool is_collision(const Objects& object) const;

	Objects* Get_Child() const;
	
	bool is_destroyed;
	int lives;
	glm::vec3 position;

protected:
	float size_x;
	float size_y;
	float size_z;
	
	Model model_obj;
	Shader shader;

	glm::vec3 start;
	glm::vec3 second;
	glm::vec3 third;

	float all_time;
};

glm::vec3 besie(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, float t) {
	point p1(v1.x, v1.y, v1.z),
		p2(v2.x, v2.y, v2.z),
		p3(v3.x, v3.y, v3.z),
		p4(v4.x, v4.y, v4.z);
	auto position = pow(1 - t, 3) * p1 + 3 * t * pow(1 - t, 2) * p2 + 3 * pow(t, 2) * (1 - t) * p3 + pow(t, 3) * p4;
	return glm::vec3(position.x, position.y, position.z);
}

Objects::Objects(const glm::vec3& position, const glm::vec3& top, const glm::vec3& bottom, const Model& model, const Shader& shader, bool is_destroyed)
	: position(position), is_destroyed(is_destroyed), model_obj(model), shader(shader), lives(1), all_time(0)
{
	size_x = std::abs(top.x - bottom.x) / 2;
	size_y = std::abs(top.y - bottom.y) / 2;
	size_z = std::abs(top.z - bottom.z) / 2;

	std::random_device rd;
	std::uniform_int_distribution<int> dist(-40, 40);
	for (int i = 0; i < 3; ++i) {
		start[i] = dist(rd);
		second[i] = dist(rd);
		third[i] = dist(rd);
	}
}

//void Objects::Draw(const glm::mat4& view, const glm::mat4& projection, float delta_time) {
//	shader.use();
//	shader.setMat4("view", view);
//	shader.setMat4("projection", projection);
//	glm::mat4 model = glm::mat4(1.0f);
//	model = glm::translate(model, position);
//	shader.setMat4("model", model);
//	model_obj.Draw(shader);
//}

void Objects::Draw(const glm::mat4& view, const glm::mat4& projection, float delta_time) {
	shader.use();
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	if (all_time > 1)
		all_time = 0;

	glm::vec3 prevPos = besie(start, second, third, start, all_time);
	position = besie(start, second, third, start, all_time + delta_time);

	all_time += delta_time / 20;


	/*glm::vec3 del = glm::normalize(position - prevPos);
	glm::vec3 asixX(1, 0, 0);
	glm::vec3 proecDelX(glm::normalize(glm::vec3(del.x, del.y, 0)));
	double cosPsi = glm::dot(asixX, proecDelX);
	glm::vec3 vecSign = glm::cross(asixX, proecDelX);
	double sign = vecSign.z / abs(vecSign.z);
	double psi = acos(cosPsi) * 180 / M_PI * sign;
	double teta = acos(del.z) * 180 / M_PI - 90;*/


	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	/*model = glm::rotate(model, (float)teta, glm::vec3(0, 1, 0));
	model = glm::rotate(model, (float)psi, glm::vec3(0, 0, 1));*/
	


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