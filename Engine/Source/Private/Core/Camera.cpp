#include "Core/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Nightbird::Core
{
	NB_TYPE_IMPL(Camera, SpatialObject)
		NB_FIELD(m_Fov, Float)
	NB_TYPE_IMPL_END()

	glm::mat4 Camera::GetViewMatrix() const
	{
		return glm::inverse(GetWorldMatrix());
	}

	glm::mat4 Camera::GetProjectionMatrix(float width, float height) const
	{
		return glm::perspective(glm::radians(m_Fov), width / height, 0.001f, 1000.0f);
	}
}
