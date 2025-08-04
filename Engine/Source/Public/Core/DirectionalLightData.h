#pragma once

#include <glm/glm.hpp>

namespace Nightbird
{
	constexpr int MAX_DIRECTIONAL_LIGHTS = 16;

	struct alignas(16) DirectionalLightData
	{
		alignas(16) glm::vec4 direction;
		alignas(16) glm::vec4 colorIntensity;
	};

	struct alignas(16) DirectionalLightMetaUBO
	{
		uint32_t count;
	};
}