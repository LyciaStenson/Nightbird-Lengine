#pragma once

#include <string>

#include "Core/SpatialObject.h"

#include <volk.h>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	struct DirectionalLightData;

	class DirectionalLight : public SpatialObject
	{
	public:
		using SpatialObject::SpatialObject;
		~DirectionalLight() override;

		DirectionalLightData GetData() const;

		glm::vec3 color = glm::vec3(1.0f);
		float intensity = 1.0f;
		
		RTTR_ENABLE(Nightbird::SpatialObject)
	};
}