#pragma once

#include "Core/TypeInfo.h"

#include <glm/glm.hpp>

namespace Nightbird::Core
{
	struct Vector3
	{
		NB_OBJECT_BASE(Vector3)

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		Vector3() = default;

		explicit Vector3(float v)
			: x(v), y(v), z(v) {}

		Vector3(float x, float y, float z)
			: x(x), y(y), z(z) {}

		operator glm::vec3() const { return { x, y, z }; }

		Vector3& operator=(const glm::vec3& v)
		{
			x=v.x; y=v.y; z=v.z;
			return *this;
		}
	};
}
