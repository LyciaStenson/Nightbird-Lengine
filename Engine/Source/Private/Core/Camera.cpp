#include <Core/Camera.h>

#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Core/CameraUBO.h>

namespace Nightbird
{
	glm::mat4 Camera::GetViewMatrix() const
	{
		return glm::inverse(GetWorldMatrix());
	}

	glm::mat4 Camera::GetProjectionMatrix(float width, float height) const
	{
		glm::mat4 proj = glm::perspective(glm::radians(fov), width / height, 0.01f, 100.0f);
		return proj;
	}

	CameraUBO Camera::GetUBO(VkExtent2D extent) const
	{
		CameraUBO ubo{};
		
		ubo.view = GetViewMatrix();
		
		glm::mat4 proj = GetProjectionMatrix((float)extent.width, (float)extent.height);
		proj[1][1] *= -1;
		ubo.projection = proj;
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