#pragma once

#include "Core/SpatialObject.h"

namespace Nightbird::Core
{
	class DirectionalLight : public SpatialObject
	{
	public:
		static const TypeInfo s_TypeInfo;
		const TypeInfo* GetTypeInfo() const override { return &s_TypeInfo; }

		using SpatialObject::SpatialObject;

		glm::vec3 color = glm::vec3(1.0f);
		float intensity = 1.0f;
	};
}
