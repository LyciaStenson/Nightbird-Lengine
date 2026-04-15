#pragma once

#include "Core/SpatialObject.h"

namespace Nightbird::Core
{
	class PointLight : public SpatialObject
	{
	public:
		NB_TYPE()
		NB_REFLECTION_FRIEND
			
		using SpatialObject::SpatialObject;

		Vector3 m_Color = Vector3(1.0f);
		float m_Intensity = 1.0f;
		float m_Radius = 10.0f;
	};
}
