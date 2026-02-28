#include "Core/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Nightbird::Core
{
	glm::mat4 Camera::GetViewMatrix() const
	{
		return glm::inverse(GetWorldMatrix());
	}

	glm::mat4 Camera::GetProjectionMatrix(float width, float height) const
	{
		glm::mat4 proj = glm::perspective(glm::radians(fov), width / height, 0.01f, 100.0f);
		proj[1][1] *= -1;
		return proj;
	}
}