#pragma once

#include "Core/SpatialObject.h"

namespace Nightbird::Core
{
	class DirectionalLight : public SpatialObject
	{
	public:
		using SpatialObject::SpatialObject;

		glm::vec3 color = glm::vec3(1.0f);
		float intensity = 1.0f;
	};
}