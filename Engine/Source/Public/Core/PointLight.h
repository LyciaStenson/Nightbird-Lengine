#pragma once

#include "Core/SpatialObject.h"

namespace Nightbird::Core
{
	class PointLight : public SpatialObject
	{
	public:
		NB_OBJECT(PointLight, SpatialObject)

		using SpatialObject::SpatialObject;

		glm::vec3 color = glm::vec3(1.0f);
		float intensity = 1.0f;
		float radius = 10.0f;
	};
}
