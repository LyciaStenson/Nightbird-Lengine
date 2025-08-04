#include <Core/DirectionalLight.h>

#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Vulkan/UniformBuffer.h>
#include <Core/DirectionalLightData.h>

namespace Nightbird
{
	DirectionalLight::~DirectionalLight()
	{

	}

	DirectionalLightData DirectionalLight::GetData() const
	{
		DirectionalLightData data;

		glm::mat4 worldMat = GetWorldMatrix();
		glm::vec3 forward = -glm::normalize(glm::vec3(worldMat[2]));
		data.direction = glm::vec4(forward, 0.0f);
		
		data.colorIntensity = glm::vec4(color, intensity);

		return data;
	}
}

RTTR_REGISTRATION
{
	rttr::registration::class_<Nightbird::DirectionalLight>("DirectionalLight")
	.constructor<std::string>()
	.property("Intensity", &Nightbird::DirectionalLight::intensity)
	.property("Color", &Nightbird::DirectionalLight::color);

	rttr::registration::method("CreateDirectionalLight", [](const std::string& name) -> Nightbird::SceneObject*
	{
		return new Nightbird::DirectionalLight(name);
	});
}