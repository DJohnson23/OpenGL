#pragma once

#include "glm/glm.hpp"

class Transform
{
public:
	glm::vec3 position = glm::vec3(0,0,0);
	glm::vec3 rotation = glm::vec3(0,0,0);
	glm::vec3 scale = glm::vec3(1,1,1);

	glm::mat4 transformationMat() const;
	glm::mat4 translationMat() const;
	glm::mat4 rotationMat() const;
	glm::mat4 scaleMat() const;

	glm::vec3 up() const;
	glm::vec3 right() const;
	glm::vec3 forward() const;
};