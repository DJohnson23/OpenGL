#pragma once

#include "glm/glm.hpp"

class Camera
{
public:
	glm::vec3 target = glm::vec3(0, 0, 0);
	glm::vec3 position = glm::vec3(0, 0, -10);
	glm::vec3 up = glm::vec3(0, 1, 0);
	bool orthographic = false;
	float screenWidth = 1920.0f, screenHeight = 1080.0f;
	float fov = 60.0f, nearClip = 0.1f, farClip = 100.0f;

	glm::mat4 viewMat() const;
	glm::mat4 projMat() const;
};

