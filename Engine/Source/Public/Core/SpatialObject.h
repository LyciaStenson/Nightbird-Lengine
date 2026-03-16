#pragma once

#include "Core/SceneObject.h"
#include "Core/Transform.h"

namespace Nightbird::Core
{
	class SpatialObject : public SceneObject
	{
	public:
		static const TypeInfo s_TypeInfo;
		const TypeInfo* GetTypeInfo() const override { return &s_TypeInfo; }

		using SceneObject::SceneObject;
		~SpatialObject() override = default;

		glm::mat4 GetLocalMatrix() const;
		glm::mat4 GetWorldMatrix() const;

		Transform transform;
	};
}
