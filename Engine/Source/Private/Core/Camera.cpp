#include "Core/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Nightbird::Core
{
	NB_OBJECT_IMPL(Nightbird::Core::Camera, Nightbird::Core::SpatialObject)

	glm::mat4 Camera::GetViewMatrix() const
	{
		return glm::inverse(GetWorldMatrix());
	}

	glm::mat4 Camera::GetProjectionMatrix(float width, float height) const
	{
		return glm::perspective(glm::radians(m_Fov), width / height, 0.001f, 1000.0f);
	}
}
