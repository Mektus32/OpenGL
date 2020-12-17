#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "objects.h"
#include "point.h"

#include <random>

#include <cmath>

#define M_PI 3.14159265358979323846

class Evil : public Objects
{
public:
	Evil(const glm::vec3& position, const glm::vec3& top, const glm::vec3& bottom, const Model& model, const Shader& shader, bool is_destroyed = true);

	void Draw(const glm::mat4& view, const glm::mat4& projection, float delta_time) override;

private:
	glm::vec3 start;
	glm::vec3 second;
	glm::vec3 third;
};

Evil::Evil(const glm::vec3& position, const glm::vec3& top, const glm::vec3& bottom, const Model& model, const Shader& shader, bool is_destroyed)
	: Objects(position, top, bottom, model, shader, is_destroyed) {
}

void Evil::Draw(const glm::mat4& view, const glm::mat4& projection, float delta_time) {
	shader.use();
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	static float all_time;
	
	if (all_time > 1)
		all_time = 0;

	glm::vec3 prevPos = besie(start, second, third, start, all_time);
	position = besie(start, second, third, start, all_time + delta_time);

	all_time += delta_time;


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
	std::cout << position.x << ' ' << position.y << ' ' << position.z << std::endl;
	/*model = glm::rotate(model, (float)psi, glm::vec3(0, 0, 1));
	model = glm::rotate(model, (float)teta, glm::vec3(0, 1, 0));*/


	shader.setMat4("model", model);
	model_obj.Draw(shader);
}