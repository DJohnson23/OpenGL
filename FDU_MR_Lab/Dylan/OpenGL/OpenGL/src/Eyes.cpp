#include "Eyes.h"
#include "glm/gtc/matrix_transform.hpp"

glm::mat4* Eyes::viewMats() const
{
	glm::mat4* mats = new glm::mat4[2];

	glm::vec3 camTrans = glm::vec3(ipd / 2, 0.0f, 0.0f);

	mats[0] = glm::lookAt(center - camTrans, target, upVec);
	mats[1] = glm::lookAt(center + camTrans, target, upVec);

	return mats;
}

glm::mat4 Eyes::projMat() const
{
	return glm::perspective(glm::radians(fov), screenWidth / screenHeight, nearClip, farClip);
}

glm::vec3 Eyes::getRightPos() const
{
	return glm::vec3(center.x + ipd / 2, center.y, center.z);
}

glm::vec3 Eyes::getLeftPos() const
{
	return glm::vec3(center.x - ipd / 2, center.y, center.z);
}
