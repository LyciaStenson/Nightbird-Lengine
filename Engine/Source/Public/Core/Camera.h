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
		Camera() = default;
		Camera(const std::string& name);
		~Camera();
		
		CameraUBO GetUBO(VkExtent2D swapChainExtent) const;

		//template <class Archive>
		//void serialize(Archive& archive)
		//{
		//	archive
		//	(
		//		cereal::base_class<::Nightbird::SceneObject>(this),
		//		CEREAL_NVP(fov)
		//	);
		//}
		
		float fov = 70.0f;
	};
}