#pragma once

#include "glm/glm.hpp"

class Eyes
{
public:

	glm::mat4* viewMats() const;
	glm::mat4 projMat() const;
	glm::vec3 getRightPos() const;
	glm::vec3 getLeftPos() const;

	float screenWidth = 1920.0f, screenHeight = 1080.0f;
	float fov = 60.0f, nearClip = 0.1f, farClip = 100.0f;
	float ipd = 0.64f;

	glm::vec3 target = glm::vec3(0, 0, 0);
	glm::vec3 upVec = glm::vec3(0, 1, 0);
	glm::vec3 center = glm::vec3(0, 0, 10);
};