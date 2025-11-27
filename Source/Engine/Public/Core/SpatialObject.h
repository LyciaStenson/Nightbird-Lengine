#pragma once

#include "Core/SceneObject.h"

namespace Nightbird
{
	class SpatialObject : public SceneObject
	{
	public:
		using SceneObject::SceneObject;
		~SpatialObject() override = default;

		glm::mat4 GetLocalMatrix() const;
		glm::mat4 GetWorldMatrix() const;

		Transform transform;
		
		RTTR_ENABLE(Nightbird::SceneObject)
	};
}