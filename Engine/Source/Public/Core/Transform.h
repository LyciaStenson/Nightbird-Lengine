#pragma once

#include "Core/Reflect.h"

#include "Core/Math/Vector3.h"
#include "Core/Math/Quat.h"

namespace Nightbird::Core
{
	struct Transform
	{
		NB_ENABLE()
		
		glm::mat4 GetLocalMatrix() const;

		Vector3 position = Vector3(0.0f);
		Quat rotation = Quat(1.0f, 0.0f, 0.0f, 0.0f);
		Vector3 scale = Vector3(1.0f);
	};
}
