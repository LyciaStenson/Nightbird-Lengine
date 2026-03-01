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
		return glm::perspective(glm::radians(fov), width / height, 0.001f, 1000.0f);
	}
}