#pragma once

#include <string>

#include <Core/SpatialObject.h>

#include <volk.h>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	struct PointLightData;
	
	class PointLight : public SpatialObject
	{
	public:
		using SpatialObject::SpatialObject;
		~PointLight() override;

		PointLightData GetData() const;
		
		glm::vec3 color = glm::vec3(1.0f);
		float intensity = 1.0f;
		float radius = 10.0f;
		
		RTTR_ENABLE(Nightbird::SpatialObject)
	};
}