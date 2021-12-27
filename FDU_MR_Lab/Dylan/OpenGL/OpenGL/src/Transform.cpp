#include "Transform.h"
#include "glm/gtc/matrix_transform.hpp"

glm::mat4 Transform::transformationMat() const
{

	return translationMat() * rotationMat() * scaleMat();
}

glm::mat4 Transform::translationMat() const
{
	return glm::translate(glm::mat4(1.0f), position);
}

glm::mat4 Transform::rotationMat() const
{
	glm::mat4 xRot = glm::rotate(glm::mat4(1.0), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 yRot = glm::rotate(glm::mat4(1.0), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 zRot = glm::rotate(glm::mat4(1.0), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	return yRot * xRot * zRot;
}

glm::mat4 Transform::scaleMat() const
{
	return glm::scale(glm::mat4(1.0f), scale);
}

glm::vec3 Transform::up() const
{
	return rotationMat() * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
}

glm::vec3 Transform::right() const
{
	return rotationMat() * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

glm::vec3 Transform::forward() const
{
	return rotationMat() * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
}
