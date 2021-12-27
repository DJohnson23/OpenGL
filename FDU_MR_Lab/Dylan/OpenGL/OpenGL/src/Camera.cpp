#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

glm::mat4 Camera::viewMat() const
{
	if (orthographic)
		return glm::mat4(1.0f);

	return glm::lookAt(position, target, up);
}

glm::mat4 Camera::projMat() const
{
	if (orthographic)
	{
		return glm::ortho(0.0f, screenWidth, 0.0f, screenHeight, nearClip, farClip);
	}

	return glm::perspective(glm::radians(fov), screenWidth / screenHeight, nearClip, farClip);
}
