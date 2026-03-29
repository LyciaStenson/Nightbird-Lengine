#pragma once

#include "Core/SpatialObject.h"

#include <glm/glm.hpp>

namespace Nightbird::Core
{
	class Camera : public SpatialObject
	{
	public:
		NB_OBJECT(Camera, SpatialObject)
		NB_ENABLE(SpatialObject)
		NB_REGISTRATION_FRIEND

		using SpatialObject::SpatialObject;

		float m_Fov = 70.0f;

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjectionMatrix(float width, float height) const;
	};
}
