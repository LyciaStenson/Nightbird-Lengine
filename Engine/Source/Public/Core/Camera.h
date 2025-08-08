#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

#include <Core/SceneObject.h>
#include <Core/Transform.h>
#include <Vulkan/UniformBuffer.h>

namespace Nightbird
{
	struct CameraUBO;
	
	class Camera : public SceneObject
	{
	public:
		using SceneObject::SceneObject;
		~Camera() override = default;

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjectionMatrix(float width, float height) const;
		
		CameraUBO GetUBO(VkExtent2D extent) const;
		
		float fov = 70.0f;
		
		RTTR_ENABLE(Nightbird::SceneObject)
	};
}