#include <Core/Camera.h>

#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Core/CameraUBO.h>

namespace Nightbird
{
	CameraUBO Camera::GetUBO(VkExtent2D swapChainExtent) const
	{
		CameraUBO ubo{};

		glm::mat4 world = GetWorldMatrix();
		
		ubo.view = glm::inverse(world);
		ubo.projection = glm::perspective(glm::radians(fov), (float)swapChainExtent.width / (float)swapChainExtent.height, 0.01f, 100.0f);
		ubo.projection[1][1] *= -1;
		ubo.position = glm::vec4(GetWorldMatrix()[3]);
		
		return ubo;
	}
}

RTTR_REGISTRATION
{
	rttr::registration::class_<Nightbird::Camera>("Camera")
	.constructor<std::string>()
	.property("FOV", &Nightbird::Camera::fov);

	rttr::registration::method("CreateCamera", [](const std::string& name) -> Nightbird::SceneObject*
	{
		return new Nightbird::Camera(name);
	});
}