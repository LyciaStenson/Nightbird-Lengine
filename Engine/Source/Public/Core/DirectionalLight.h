#pragma once

#include <string>

#include <Core/SceneObject.h>

#include <volk.h>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	struct DirectionalLightData;

	class DirectionalLight : public SceneObject
	{
	public:
		using SceneObject::SceneObject;
		~DirectionalLight() override;

		DirectionalLightData GetData() const;

		glm::vec3 color = glm::vec3(1.0f);
		float intensity = 1.0f;
		
		RTTR_ENABLE(Nightbird::SceneObject)
	};
}