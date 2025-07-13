#pragma once

#include <string>

#include <Core/SceneObject.h>
#include <Core/SceneObjectRegistry.h>

#include <volk.h>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	struct PointLightData;
	
	class PointLight : public SceneObject
	{
	public:
		using SceneObject::SceneObject;

		//PointLight() = default;
		~PointLight() override;

		PointLightData GetData() const;
		
		const char* GetTypeName() const override { return "PointLight"; }

		glm::vec3 color = glm::vec3(1.0f);
		float intensity = 1.0f;
		float radius = 10.0f;
		
		RTTR_ENABLE(Nightbird::SceneObject)
	};
}