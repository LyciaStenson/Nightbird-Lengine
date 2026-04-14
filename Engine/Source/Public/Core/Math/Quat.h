#pragma once

#include "Core/TypeInfo.h"

#include <glm/gtc/quaternion.hpp>

namespace Nightbird::Core
{
	struct Quat
	{
		NB_OBJECT_BASE()

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 1.0f;

		Quat() = default;

		Quat(float w, float x, float y, float z)
			: x(x), y(y), z(z), w(w) {}

		operator glm::quat() const { return { w, x, y, z }; }

		Quat& operator=(const glm::quat& q)
		{
			x=q.x; y=q.y; z=q.z; w=q.w;
			return *this;
		}
	};
}
