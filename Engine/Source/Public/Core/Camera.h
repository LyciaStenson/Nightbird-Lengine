#pragma once

#include "Core/SpatialObject.h"

#include <glm/glm.hpp>

namespace Nightbird::Core
{
	class Camera : public SpatialObject
	{
	public:
		NB_OBJECT(Camera, SpatialObject)

		using SpatialObject::SpatialObject;

		float fov = 70.0f;

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjectionMatrix(float width, float height) const;
	};
}
