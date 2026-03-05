#pragma once

#include <cstdint>

namespace Nightbird::Core
{
	enum class SceneObjectType : uint8_t
	{
		SceneObject = 0,
		SpatialObject = 1,
		SceneInstance = 2,
		Camera = 3,
		MeshInstance = 4,
		DirectionalLight = 5,
		PointLight = 6
	};
}