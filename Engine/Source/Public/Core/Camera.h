#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

#include <Core/SceneObject.h>
#include <Core/SceneObjectRegistry.h>
#include <Core/Transform.h>
#include <Vulkan/UniformBuffer.h>

namespace Nightbird
{
	struct CameraUBO;
	
	class Camera : public SceneObject
	{
	public:
		using SceneObject::SceneObject;

		Camera() = default;
		~Camera() override;
		
		CameraUBO GetUBO(VkExtent2D swapChainExtent) const;

		const char* GetTypeName() const override { return "Camera"; }

		void Serialize(json& out) const override;
		void Deserialize(const json& in) override;
		
		float fov = 70.0f;

		//RTTR_ENABLE(Camera)
	};
}