#pragma once

#include "Core/SpatialObject.h"

#include <glm/glm.hpp>

namespace Nightbird::Core
{
	class Camera : public SpatialObject
	{
	public:
		static const TypeInfo s_TypeInfo;
		const TypeInfo* GetTypeInfo() const override { return &s_TypeInfo; }

		using SpatialObject::SpatialObject;

		float fov = 70.0f;

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjectionMatrix(float width, float height) const;
	};
}
