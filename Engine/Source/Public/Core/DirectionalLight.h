#pragma once

#include "Core/SpatialObject.h"

namespace Nightbird::Core
{
	class DirectionalLight : public SpatialObject
	{
	public:
		NB_OBJECT(DirectionalLight, SpatialObject)
		NB_ENABLE(SpatialObject)
		NB_REGISTRATION_FRIEND

		using SpatialObject::SpatialObject;

		glm::vec3 m_Color = glm::vec3(1.0f);
		float m_Intensity = 1.0f;
	};
}
