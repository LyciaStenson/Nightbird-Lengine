#pragma once

#include <cstdint>

namespace Nightbird::Core
{
	enum class SceneObjectType : uint8_t
	{
		SceneObject = 0,
		SpatialObject = 1,
		Camera = 2,
		MeshInstance = 3,
		DirectionalLight = 4,
		PointLight = 5
	};
}